/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Andrey Zmievski <andrey@ispi.net>                          |
   +----------------------------------------------------------------------+
 */

/* $Id: wddx_a.c,v 1.14 2000/05/25 12:20:29 andrei Exp $ */

#include "php.h"
#include "internal_functions.h"
#include "php3_string.h"
#include "php3_list.h"
#include "php3_wddx_a.h"

#if HAVE_LIBEXPAT
#include "DList.h"

#define WDDX_PACKET_S			"<wddxPacket version='0.9'>"
#define WDDX_PACKET_E			"</wddxPacket>"
#define WDDX_HEADER				"<header/>"
#define WDDX_HEADER_COMMENT		"<header comment='%s'/>"
#define WDDX_DATA_S				"<data>"
#define WDDX_DATA_E				"</data>"
#define WDDX_STRING_S			"<string>"
#define WDDX_STRING_E			"</string>"
#define WDDX_CHAR				"<char code='%02X'/>"
#define WDDX_NUMBER				"<number>%s</number>"
#define WDDX_ARRAY_S			"<array length='%d'>"
#define WDDX_ARRAY_E			"</array>"
#define WDDX_VAR_S				"<var name='%s'>"
#define WDDX_VAR_E				"</var>"
#define WDDX_STRUCT_S			"<struct>"
#define WDDX_STRUCT_E			"</struct>"

#define WDDX_BUF_LEN			256

#define	EL_STRING				"string"
#define EL_CHAR					"char"
#define EL_CHAR_CODE			"code"
#define EL_NUMBER				"number"
#define EL_ARRAY				"array"
#define EL_STRUCT				"struct"
#define EL_VAR					"var"
#define EL_VAR_NAME				"name"
#define	EL_PACKET				"wddxPacket"
#define EL_VERSION				"version"


static int le_wddx;

typedef struct {
	DLIST *packet_head;
	int packet_length;
} wddx_packet;

typedef struct {
	pval *data;
	enum {
		ST_STRING,
		ST_NUMBER,
		ST_ARRAY,
		ST_STRUCT
	} type;
	char *varname;
} st_entry;

typedef struct {
	int top, max;
	char *varname;
	void **elements;
} wddx_stack;


/* {{{ function prototypes */
static void _php3_wddx_serialize_var(wddx_packet *packet, pval *var, char *name);
static void _php3_wddx_process_data(void *user_data, const char *s, int len);
/* }}} */


/* {{{ module definition structures */

function_entry wddx_functions_a[] = {
	PHP_FE(wddx_serialize_value, NULL)
	PHP_FE(wddx_serialize_vars, NULL)
	PHP_FE(wddx_packet_start, NULL)
	PHP_FE(wddx_packet_end, NULL)
	PHP_FE(wddx_add_vars, NULL)
	PHP_FE(wddx_deserialize, NULL)
	{NULL, NULL, NULL}
};

php3_module_entry wddx_module_entry_a = {
	"WDDX_A", wddx_functions_a, php3_minit_wddx, NULL, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};

/* }}} */

	
/* {{{ int wddx_stack_init(wddx_stack *stack) */
static int wddx_stack_init(wddx_stack *stack)
{
	stack->top = 0;
	stack->elements = (void **) emalloc(sizeof(void **) * STACK_BLOCK_SIZE);
	if (!stack->elements) {
		return FAILURE;
	} else {
		stack->max = STACK_BLOCK_SIZE;
		stack->varname = NULL;
		return SUCCESS;
	}
}
/* }}} */


/* {{{ int wddx_stack_push(wddx_stack *stack, void *element, int size) */
static int wddx_stack_push(wddx_stack *stack, void *element, int size)
{
	if (stack->top >= stack->max) {		/* we need to allocate more memory */
		stack->elements = (void **) erealloc(stack->elements,
				   (sizeof(void **) * (stack->max += STACK_BLOCK_SIZE)));
		if (!stack->elements) {
			return FAILURE;
		}
	}
	stack->elements[stack->top] = (void *) emalloc(size);
	memcpy(stack->elements[stack->top], element, size);
	return stack->top++;
}
/* }}} */


/* {{{ int wddx_stack_top(wddx_stack *stack, void **element) */
static int wddx_stack_top(wddx_stack *stack, void **element)
{
	if (stack->top > 0) {
		*element = stack->elements[stack->top - 1];
		return SUCCESS;
	} else {
		*element = NULL;
		return FAILURE;
	}
}
/* }}} */


/* {{{ int wddx_stack_is_empty(wddx_stack *stack) */
static int wddx_stack_is_empty(wddx_stack *stack)
{
	if (stack->top == 0) {
		return 1;
	} else {
		return 0;
	}
}
/* }}} */


/* {{{ int wddx_stack_destroy(wddx_stack *stack) */
static int wddx_stack_destroy(wddx_stack *stack)
{
	register int i;

	if (stack->elements) {
		for (i = 0; i < stack->top; i++) {
			if (((st_entry *)stack->elements[i])->data)
			{
				pval_destructor(((st_entry *)stack->elements[i])->data);
				efree(((st_entry *)stack->elements[i])->data);
			}
			efree(stack->elements[i]);
		}		
		efree(stack->elements);
	}
	return SUCCESS;
}
/* }}} */


/* {{{ _php3_free_packet_chunk */
static void _php3_free_packet_chunk(char **chunk_ptr)
{
	if ((*chunk_ptr))
		efree((*chunk_ptr));
}
/* }}} */


/* {{{ _php3_wddx_destructor */
static void _php3_wddx_destructor(wddx_packet *packet)
{
	dlst_kill(packet->packet_head, (void (*)(void *))_php3_free_packet_chunk);
	efree(packet);
}
/* }}} */


/* {{{ php3_minit_wddx */
int php3_minit_wddx(INIT_FUNC_ARGS)
{
	le_wddx = register_list_destructors(_php3_wddx_destructor, NULL);
	
	return SUCCESS;
}
/* }}} */


/* {{{ _php3_wddx_add_chunk */
static void _php3_wddx_add_chunk(wddx_packet *packet, char *str)
{
	char **chunk_ptr;
	
	chunk_ptr = (char**)dlst_newnode(sizeof(char *));
	(*chunk_ptr) = estrdup(str);
	dlst_insertafter(packet->packet_head, chunk_ptr, PHP_DLST_TAIL(packet->packet_head));
	packet->packet_length += strlen(str);
}
/* }}} */


/* {{{ _php3_wddx_gather */
static char* _php3_wddx_gather(wddx_packet *packet)
{
	char **chunk;
	char *buf;
	
	buf = (char *)emalloc(packet->packet_length+1);	
	buf[0] = '\0';
	for(chunk=dlst_first(packet->packet_head);
		chunk!=NULL;
		chunk = dlst_next(chunk)) {
		strcat(buf, *chunk);
	}
	
	return buf;
}
/* }}} */


/* {{{ void _php3_wddx_packet_start */
static void _php3_wddx_packet_start(wddx_packet *packet, char *comment)
{
	char tmp_buf[WDDX_BUF_LEN];
	
	_php3_wddx_add_chunk(packet, WDDX_PACKET_S);
	if (comment)
	{
		sprintf(tmp_buf, WDDX_HEADER_COMMENT, comment);
		_php3_wddx_add_chunk(packet, tmp_buf);
	}
	else
		_php3_wddx_add_chunk(packet, WDDX_HEADER);
	_php3_wddx_add_chunk(packet, WDDX_DATA_S);
}
/* }}} */


/* {{{ int _php3_wddx_packet_end */
static void _php3_wddx_packet_end(wddx_packet *packet)
{
	_php3_wddx_add_chunk(packet, WDDX_DATA_E);
	_php3_wddx_add_chunk(packet, WDDX_PACKET_E);	
}
/* }}} */


/* {{{ void _php3_wddx_serialize_var(wddx_packet *packet, pval *var) */
static void _php3_wddx_serialize_string(wddx_packet *packet, pval *var)
{
	char *buf,
		 *c,
		 control_buf[WDDX_BUF_LEN];
	int i;

	_php3_wddx_add_chunk(packet, WDDX_STRING_S);

	i = 0;
	buf = (char *)emalloc(var->value.str.len + 1);
	for(c=var->value.str.val; *c!='\0'; c++)
	{
		if (iscntrl((int)*c))
		{
			if (*buf)
			{
				buf[i] = '\0';
				_php3_wddx_add_chunk(packet, buf);
				i = 0;
				buf[i] = '\0';
			}
			sprintf(control_buf, WDDX_CHAR, *c);
			_php3_wddx_add_chunk(packet, control_buf);
		}
		else
			buf[i++] = *c;
	}
	buf[i] = '\0';
	if (*buf)
		_php3_wddx_add_chunk(packet, buf);				
	efree(buf);
	
	_php3_wddx_add_chunk(packet, WDDX_STRING_E);
}
/* }}} */


/* {{{ void _php3_wddx_serialize_number(wddx_packet *packet, pval *var) */
static void _php3_wddx_serialize_number(wddx_packet *packet, pval *var)
{
	char tmp_buf[WDDX_BUF_LEN];
	
	convert_to_string(var);
	sprintf(tmp_buf, WDDX_NUMBER, var->value.str.val);
	_php3_wddx_add_chunk(packet, tmp_buf);	
}
/* }}} */


/* {{{ void _php3_wddx_serialize_hash(wddx_packet *packet, pval *var) */
static void _php3_wddx_serialize_hash(wddx_packet *packet, pval *var)
{
	pval *ent;
	char *key;
	int hash_type, ent_type;
	ulong idx;
	char tmp_buf[WDDX_BUF_LEN];	

	_php3_hash_internal_pointer_reset(var->value.ht);

	while(_php3_hash_get_current_data(var->value.ht, (void**)&ent) == SUCCESS) {
		if (ent->type == IS_STRING &&
			ent->value.str.val == undefined_variable_string)
			_php3_hash_move_forward(var->value.ht);
		else
			break;
	}
	hash_type = _php3_hash_get_current_key(var->value.ht, &key, &idx);	

	if (hash_type == HASH_KEY_IS_STRING) {
		_php3_wddx_add_chunk(packet, WDDX_STRUCT_S);
		efree(key);
	} else {
		sprintf(tmp_buf, WDDX_ARRAY_S, _php3_hash_num_elements(var->value.ht));
		_php3_wddx_add_chunk(packet, tmp_buf);
	}
		
	while(_php3_hash_get_current_data(var->value.ht, (void**)&ent) == SUCCESS) {
		if (!(ent->type == IS_STRING &&
			ent->value.str.val == undefined_variable_string)) {
			
			if (hash_type == HASH_KEY_IS_STRING) {
				ent_type = _php3_hash_get_current_key(var->value.ht, &key, &idx);

				if (ent_type == HASH_KEY_IS_STRING) {
					_php3_wddx_serialize_var(packet, ent, key);				
					efree(key);
				} else {
					sprintf(tmp_buf, "%ld", idx);
					_php3_wddx_serialize_var(packet, ent, tmp_buf);
				}
			} else
				_php3_wddx_serialize_var(packet, ent, NULL);
			
		}
				
		_php3_hash_move_forward(var->value.ht);
	}
	
	if (hash_type == HASH_KEY_IS_STRING)
		_php3_wddx_add_chunk(packet, WDDX_STRUCT_E);
	else
		_php3_wddx_add_chunk(packet, WDDX_ARRAY_E);
}
/* }}} */


/* {{{ void _php3_wddx_serialize_var(wddx_packet *packet, pval *var, char *name) */
static void _php3_wddx_serialize_var(wddx_packet *packet, pval *var, char *name)
{
	char tmp_buf[WDDX_BUF_LEN];
	
	if (name) {
		sprintf(tmp_buf, WDDX_VAR_S, name);
		_php3_wddx_add_chunk(packet, tmp_buf);
	}
	
	switch(var->type) {
		case IS_STRING:
			_php3_wddx_serialize_string(packet, var);
			break;
			
		case IS_LONG:
		case IS_DOUBLE:
			_php3_wddx_serialize_number(packet, var);
			break;
		
		case IS_ARRAY:
		case IS_OBJECT:
			_php3_wddx_serialize_hash(packet, var);
			break;
	}
	
	if (name) {
		_php3_wddx_add_chunk(packet, WDDX_VAR_E);
	}
}
/* }}} */


/* {{{ void _php3_wddx_add_var(wddx_packet *packet, pval *name_var) */
static void _php3_wddx_add_var(wddx_packet *packet, pval *name_var)
{
	pval *val;
	
	if (name_var->type & IS_STRING)
	{
		if (_php3_hash_find(GLOBAL(active_symbol_table), name_var->value.str.val,
							name_var->value.str.len+1, (void**)&val) != FAILURE) {
			_php3_wddx_serialize_var(packet, val, name_var->value.str.val);
		}		
	}
	else if (name_var->type & IS_ARRAY)
	{
		_php3_hash_internal_pointer_reset(name_var->value.ht);

		while(_php3_hash_get_current_data(name_var->value.ht, (void**)&val) == SUCCESS) {
			if (!(val->type == IS_STRING &&
				val->value.str.val == undefined_variable_string))
				_php3_wddx_add_var(packet, val);
				
			_php3_hash_move_forward(name_var->value.ht);
		}
	}
}
/* }}} */


/* {{{ void _php3_wddx_push_element(void *user_data, const char *name, const char **atts) */
static void _php3_wddx_push_element(void *user_data, const char *name, const char **atts)
{
	st_entry ent;
	wddx_stack *stack = (wddx_stack *)user_data;
	
	if (!strcmp(name, EL_PACKET)) {
		int i;
		
		for (i=0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_VERSION)) {
			}
		}
	} else if (!strcmp(name, EL_STRING)) {
		ent.type = ST_STRING;
		if (stack->varname) {
			ent.varname = estrdup(stack->varname);		
			efree(stack->varname);
			stack->varname = NULL;
		} else
			ent.varname = NULL;
		
		ent.data = (pval *)emalloc(sizeof(pval));
		ent.data->value.str.val = NULL;
		ent.data->value.str.len = 0;
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_CHAR)) {
		int i;
		char tmp_buf[2];
		
		for (i=0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_CHAR_CODE) && atts[i+1]) {
				sprintf(tmp_buf, "%c", (char)strtol(atts[i+1], NULL, 16));
				_php3_wddx_process_data(user_data, tmp_buf, strlen(tmp_buf));
			}
		}
	} else if (!strcmp(name, EL_NUMBER)) {
		ent.type = ST_NUMBER;
		if (stack->varname) {
			ent.varname = estrdup(stack->varname);		
			efree(stack->varname);
			stack->varname = NULL;
		} else
			ent.varname = NULL;
		
		ent.data = (pval *)emalloc(sizeof(pval));
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_ARRAY)) {
		ent.type = ST_ARRAY;
		if (stack->varname) {
			ent.varname = estrdup(stack->varname);		
			efree(stack->varname);
			stack->varname = NULL;
		} else
			ent.varname = NULL;
		
		ent.data = (pval *)emalloc(sizeof(pval));
		array_init(ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_STRUCT)) {
		ent.type = ST_STRUCT;
		if (stack->varname) {
			ent.varname = estrdup(stack->varname);		
			efree(stack->varname);
			stack->varname = NULL;
		} else
			ent.varname = NULL;
		
		ent.data = (pval *)emalloc(sizeof(pval));
		array_init(ent.data);
		wddx_stack_push((wddx_stack *)stack, &ent, sizeof(st_entry));
	} else if (!strcmp(name, EL_VAR)) {
		int i;
		
		for (i=0; atts[i]; i++) {
			if (!strcmp(atts[i], EL_VAR_NAME) && atts[i+1]) {
				stack->varname = estrdup(atts[i+1]);
			}
		}
	}

}
/* }}} */


/* {{{ void _php3_wddx_pop_element(void *user_data, const char *name) */
static void _php3_wddx_pop_element(void *user_data, const char *name)
{
	st_entry *ent1, *ent2;
	wddx_stack *stack = (wddx_stack *)user_data;
	
	if (!strcmp(name, EL_STRING) || !strcmp(name, EL_NUMBER) ||
		!strcmp(name, EL_ARRAY) || !strcmp(name, EL_STRUCT)) {
		if (stack->top > 1) {
			wddx_stack_top(stack, (void**)&ent1);
			stack->top--;
			wddx_stack_top(stack, (void**)&ent2);
			if (ent2->data->type == IS_ARRAY) {
				if (ent1->varname) {
					_php3_hash_update(ent2->data->value.ht, ent1->varname, strlen(ent1->varname)+1,
									  ent1->data, sizeof(pval), NULL);
					efree(ent1->varname);
				} else
					_php3_hash_next_index_insert(ent2->data->value.ht, ent1->data,
												 sizeof(pval), NULL);
			}
			efree(ent1->data);
			efree(ent1);
		}
	}
	else if (!strcmp(name, EL_VAR) && stack->varname)
		efree(stack->varname);
}
/* }}} */


/* {{{ void _php3_wddx_process_data(void *user_data, const char *s, int len) */
static void _php3_wddx_process_data(void *user_data, const char *s, int len)
{
	st_entry *ent;
	wddx_stack *stack = (wddx_stack *)user_data;

	if (!wddx_stack_is_empty(stack)) {
		wddx_stack_top(stack, (void**)&ent);
		switch (ent->type) {
			case ST_STRING:
				ent->data->type = IS_STRING;
				if (ent->data->value.str.len == 0) {
					ent->data->value.str.val = estrndup(s, len);
					ent->data->value.str.len = len;
				} else {
					ent->data->value.str.val = erealloc(ent->data->value.str.val,
							ent->data->value.str.len + len + 1);
					strncpy(ent->data->value.str.val+ent->data->value.str.len, s, len);
					ent->data->value.str.len += len;
					ent->data->value.str.val[ent->data->value.str.len] = '\0';
				}
				break;

			case ST_NUMBER:
				ent->data->type = IS_STRING;
				ent->data->value.str.len = len;
				ent->data->value.str.val = estrndup(s, len);
				convert_string_to_number(ent->data);
				break;

			default:
				break;
		}
	}
}
/* }}} */


/* {{{ void _php3_wddx_deserialize(pval *packet, pval *return_value) */
static void _php3_wddx_deserialize(pval *packet, pval *return_value)
{
	wddx_stack stack;
	XML_Parser parser;
	st_entry *ent;
	
	wddx_stack_init(&stack);
	parser = XML_ParserCreate(NULL);

	XML_SetUserData(parser, &stack);
	XML_SetElementHandler(parser, _php3_wddx_push_element, _php3_wddx_pop_element);
	XML_SetCharacterDataHandler(parser, _php3_wddx_process_data);
	
	XML_Parse(parser, packet->value.str.val, packet->value.str.len, 1);
	
	XML_ParserFree(parser);

	if (!wddx_stack_is_empty(&stack)) {
		wddx_stack_top(&stack, (void**)&ent);
		*return_value = *(ent->data);
		pval_copy_constructor(return_value);
	}
	
	wddx_stack_destroy(&stack);
}
/* }}} */


/* {{{ proto string wddx_serialize_value(mixed var [, string comment])
   Creates a new packet and serializes the given value */
PHP_FUNCTION(wddx_serialize_value)
{
	int argc;
	pval *var,
		 *comment;
	wddx_packet *packet;
	char *buf;
	
	argc = ARG_COUNT(ht);
	if(argc < 1 || argc > 2 || getParameters(ht, argc, &var, &comment) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	packet = emalloc(sizeof(wddx_packet));
	if (!packet) {
		php3_error(E_WARNING, "Unable to allocate memory in php3_wddx_packet_start");
		RETURN_FALSE;
	}
	
	packet->packet_head = dlst_init();
	packet->packet_length = 0;

	if (argc == 2)
	{
		convert_to_string(comment);
		_php3_wddx_packet_start(packet, comment->value.str.val);
	}
	else
		_php3_wddx_packet_start(packet, NULL);

	_php3_wddx_serialize_var(packet, var, NULL);		
	_php3_wddx_packet_end(packet);
	buf = _php3_wddx_gather(packet);
	_php3_wddx_destructor(packet);
					
	RETURN_STRING(buf, 0);
}
/* }}} */


/* {{{ proto string wddx_serialize_vars(mixed var_name [, mixed ...])
   Creates a new packet and serializes given variables into a struct */
PHP_FUNCTION(wddx_serialize_vars)
{
	int argc, i;
	wddx_packet *packet;
	pval *name_var;
	char *buf;
		
	argc = ARG_COUNT(ht);
	
	packet = emalloc(sizeof(wddx_packet));
	if (!packet) {
		php3_error(E_WARNING, "Unable to allocate memory in php3_wddx_packet_start");
		RETURN_FALSE;
	}
	
	packet->packet_head = dlst_init();
	packet->packet_length = 0;

	_php3_wddx_packet_start(packet, NULL);
	_php3_wddx_add_chunk(packet, WDDX_STRUCT_S);
	
	for (i=0; i<argc; i++) {
		if (_php3_hash_index_find(ht, i, (void**)&name_var) == FAILURE) {
			php3_error(E_WARNING, "error in wddx_serialize_vars()");
			RETURN_FALSE;
		}
		
		_php3_wddx_add_var(packet, name_var);
	}	
	
	_php3_wddx_add_chunk(packet, WDDX_STRUCT_E);
	_php3_wddx_packet_end(packet);
	buf = _php3_wddx_gather(packet);
	_php3_wddx_destructor(packet);
					
	RETURN_STRING(buf, 0);	
}
/* }}} */


/* {{{ proto int wddx_packet_start([string comment])
   Starts a WDDX packet with optional comment and returns the packet id */
PHP_FUNCTION(wddx_packet_start)
{
	int argc;
	pval *comment;
	wddx_packet *packet;

	comment = NULL;
	argc = ARG_COUNT(ht);

	if (argc>1 || (argc==1 && getParameters(ht, 1, &comment)==FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	packet = emalloc(sizeof(wddx_packet));
	if (!packet) {
		php3_error(E_WARNING, "Unable to allocate memory in php3_wddx_packet_start");
		RETURN_FALSE;
	}
	
	packet->packet_head = dlst_init();
	packet->packet_length = 0;
	
	if (argc == 1) {
		convert_to_string(comment);
		_php3_wddx_packet_start(packet, comment->value.str.val);		
	}
	else
		_php3_wddx_packet_start(packet, NULL);
	
	_php3_wddx_add_chunk(packet, WDDX_STRUCT_S);
	
	RETURN_LONG(php3_list_insert(packet, le_wddx));
}
/* }}} */


/* {{{ proto string wddx_packet_end(int packet_id)
   Ends specified WDDX packet and returns the string containing the packet */
PHP_FUNCTION(wddx_packet_end)
{
	pval *packet_id;
	char *buf;
	wddx_packet *packet;
	int type, id;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &packet_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(packet_id);
	id = packet_id->value.lval;
	packet = php3_list_find(id, &type);
	if (type!=le_wddx) {
		php3_error(E_WARNING, "%d is not a valid WDDX packet id", id);
		RETURN_FALSE;
	}

	_php3_wddx_add_chunk(packet, WDDX_STRUCT_E);	
	
	_php3_wddx_packet_end(packet);

	buf = _php3_wddx_gather(packet);
	
	php3_list_delete(id);
	
	RETURN_STRING(buf, 0);
}
/* }}} */


/* {{{ proto int wddx_add_vars(int packet_id, mixed name_var [, mixed ...])
   Serializes given variables and adds them to packet given by packet_id */
PHP_FUNCTION(wddx_add_vars)
{
	int argc, type, id, i;
	pval *name_var;
	pval *packet_id;
	wddx_packet *packet;
	
	argc = ARG_COUNT(ht);
	if (argc < 2 || getParameters(ht, 1, &packet_id)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(packet_id);
	id = packet_id->value.lval;
	packet = php3_list_find(id, &type);
	if (type!=le_wddx) {
		php3_error(E_WARNING, "%d is not a valid WDDX packet id", id);
		RETURN_FALSE;
	}
		
	for (i=1; i<argc; i++) {
		if (_php3_hash_index_find(ht, i, (void**)&name_var) == FAILURE) {
			php3_error(E_WARNING, "error in wddx_add_vars()");
			RETURN_FALSE;
		}
		
		_php3_wddx_add_var(packet, name_var);
	}
	
	RETURN_TRUE;
}
/* }}} */


/* {{{  proto mixed wddx_deserialized(string packet) 
   Deserializes given packet and returns a PHP value */
PHP_FUNCTION(wddx_deserialize)
{
	pval *packet;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &packet) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(packet);
	if (packet->value.str.len == 0)
		return;
	_php3_wddx_deserialize(packet, return_value);
}
/* }}} */


#endif /* HAVE_LIBEXPAT */
