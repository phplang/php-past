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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

/* $Id: wddx.c,v 1.10 2000/07/10 10:15:36 sas Exp $ */

#include "php.h"
#include "internal_functions.h"
#include "php3_string.h"
#include "php3_list.h"
#include "php3_wddx.h"

#if HAVE_LIBEXPAT

/* {{{ extension definition structures */

function_entry wddx_functions[] = {
	PHP_FE(wddx_serialize, NULL)
	PHP_FE(wddx_unserialize, NULL)
	{NULL, NULL, NULL}
};

php3_module_entry wddx_module_entry = {
	"WDDX", wddx_functions, NULL, NULL, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};

/* }}} */

/* {{{ local structs */
typedef struct {
	enum {
		LT_ARRAY,
		LT_STRING,
		LT_NUMBER,
		LT_VAR
	} last_type;
	int notfirst;
	char *var_name;
	char *buf;
	char *prefix;
	int len;
	int depth;
	int type;
} _php3_wddx_parser;
/* }}} */

/* {{{ function prototypes */
/* added, if the compiler complains */
static char *_php3_wddx_build(char *, pval *);
/* }}} */

/* {{{ functions needed to build/serialize */

	/* {{{ _php3_wddx_var() */

static char *_php3_wddx_var(char *name, char *str)
{
	if(name) {
		char *buf;

		buf = emalloc(strlen(str) + strlen(name) + 30);
		sprintf(buf, "<var name='%s'>%s</var>", name, str);
		efree(str);
		str = buf;
	}

	return str;
}
/* }}} */
	/* {{{ _php3_wddx_number() */

static char *_php3_wddx_number(double val)
{
	char *buf;

	buf = emalloc(40);
	sprintf(buf, "<number>%g</number>", val);

	return buf;
}
/* }}} */
	/* {{{ _php3_wddx_string() */

static char *_php3_wddx_string(char *str)
{
	char *buf;

	buf = emalloc(strlen(str) + 20);
	sprintf(buf, "<string>%s</string>", str);

	return buf;
}
/* }}} */
	/* {{{ _php3_wddx_array() */

static char *_php3_wddx_array(HashTable *ht)
{
	char *buf;
	char *new;
	int len = 40;
	pval *obj;
	char *off;
	
	buf = emalloc(len);
	sprintf(buf, "<array length='%d'>", _php3_hash_num_elements(ht));
	
	_php3_hash_internal_pointer_reset(ht);
	while(_php3_hash_get_current_data(ht, (void **) &obj) == SUCCESS) {
		new = _php3_wddx_build(NULL, obj);
		len += strlen(new);
		off = erealloc(buf, len);
		if(!off) {
			efree(buf);
			return empty_string;
		}
		buf = off;
		strcat(buf, new);
		efree(new);
		_php3_hash_move_forward(ht);
	}
	strcat(buf, "</array>");

	return buf;
}
/* }}} */
	/* {{{ _php3_wddx_struct() */

static char *_php3_wddx_struct(HashTable *ht)
{
	char *buf;
	char *new;
	int len = 40;
	ulong ikey = 0;
	pval *obj;
	char *key;
	char *off;
	
	buf = emalloc(len);
	strcpy(buf, "<struct>");
	
	_php3_hash_internal_pointer_reset(ht);
	while(_php3_hash_get_current_data(ht, (void **) &obj) == SUCCESS) {
		if(_php3_hash_get_current_key(ht, &key, &ikey) ==
				HASH_KEY_IS_LONG) {
			key = emalloc(20);
			sprintf(key, "%ld", ikey);
		}
		new = _php3_wddx_build(key, obj);
		efree(key);
		len += strlen(new);
		off = erealloc(buf, len);
		if(!off) {
			efree(buf);
			return empty_string;
		}
		buf = off;
		strcat(buf, new);
		efree(new);
		_php3_hash_move_forward(ht);
	}
	strcat(buf, "</struct>");

	return buf;
}
/* }}} */
	/* {{{ _php3_wddx_struct_or_array() */

static char *_php3_wddx_struct_or_array(HashTable *ht)
{
	char *pkey = NULL;
	ulong ukey;

	_php3_hash_internal_pointer_reset(ht);
	switch(_php3_hash_get_current_key(ht, &pkey, &ukey)) {
		case HASH_KEY_IS_STRING:
			efree(pkey);
			return _php3_wddx_struct(ht);
		case HASH_KEY_IS_LONG:
			return _php3_wddx_array(ht);
	}
	return empty_string;
}
/* }}} */
	/* {{{ _php3_wddx_build() */

static char *_php3_wddx_build(char *name, pval *obj)
{
	char *str = empty_string;

	switch(obj->type) {
		case IS_LONG:
			str = _php3_wddx_number(obj->value.lval);
			break;
		case IS_DOUBLE:
			str = _php3_wddx_number(obj->value.dval);
			break;
		case IS_STRING:
			str = _php3_wddx_string(obj->value.str.val);
			break;
		case IS_ARRAY:
			str = _php3_wddx_struct_or_array(obj->value.ht);
			break;
	}

	return _php3_wddx_var(name, str);
}
/* }}} */
	/* {{{ _php3_wddx_get_parameter_name() */

/*
 * This won't work in PHP 3.1
 */

static char *_php3_wddx_get_parameter_name(pval *obj)
{
	pval *orig;

	_php3_hash_internal_pointer_reset(GLOBAL(active_symbol_table));
	while(_php3_hash_get_current_data(GLOBAL(active_symbol_table), 
			(void **) &orig) == SUCCESS) {
		if(orig == obj) {
			char *str = NULL;
			ulong ndx;

			_php3_hash_get_current_key(GLOBAL(active_symbol_table), 
					&str, &ndx);
			if(str)
				return str;
			break;
		}
		_php3_hash_move_forward(GLOBAL(active_symbol_table));
	}
	return NULL;
}
/* }}} */
	/* {{{ _php3_wddx_args_array() */

static char *_php3_wddx_args_array(HashTable *ht, pval **args, int count)
{
	int i;
	int len = 0;
	int freename;
	char *str = NULL;
	char *new;
	char *name;
	pval *obj;

	for(i = 0; i < count; i++) {
		/* depending on how the parameter was passed... */
		obj = args[i];
		if(ParameterPassedByReference(ht, i+1)) {
			/* ...we use its variable name */
			name = _php3_wddx_get_parameter_name(args[i]);
			freename = 1;
		} else {
			/* ...or the content of the variable behind it */
			i++;
			if(i >= count) {
				/* too few arguments */
				if(str) efree(str);
				return empty_string;
			}
			convert_to_string(args[i]);
			name = args[i]->value.str.val;
			freename = 0;
		}
		new = _php3_wddx_build(name, obj);
		len += strlen(new);
		if(str) {
			str = erealloc(str, len + 1);
			strcat(str, new);
			efree(new);
		} else {
			str = new;
		}
		if(freename && name) efree(name);
	}
	return str;
}
/* }}} */
/* }}} */

/* {{{ functions needed to unserialize */
/* buffer management and strcat */
#define ATTACH(s) {\
		wddx->len += strlen(s); \
		if(wddx->buf) \
			wddx->buf = erealloc(wddx->buf, wddx->len + 1); \
		else {\
			wddx->buf = emalloc(wddx->len + 1); \
			wddx->buf[0] = '\0'; \
		} \
		strcat(wddx->buf, s); }

/* is this the first element of a list? */
#define FIRST \
		if(wddx->notfirst) \
			if(wddx->depth) ATTACH(", ") else ATTACH("; ") \
		else \
			wddx->notfirst++

/* do we have a var name? could be an associative array or a top level assign */
#define VAR_NAME \
		if(wddx->var_name) { \
			if(wddx->type) { \
				ATTACH("\""); \
				ATTACH(wddx->var_name); \
				ATTACH("\" => "); \
			} else { \
				ATTACH("$"); \
				if(wddx->prefix) ATTACH(wddx->prefix) \
				ATTACH(wddx->var_name); \
				ATTACH(" = "); \
			} \
			efree(wddx->var_name); \
			wddx->var_name = NULL; \
		}

	/* {{{ _php3_wddx_start_element() */
		
static void _php3_wddx_start_element(void *data, const char *name, const char **atts)
{
	_php3_wddx_parser *wddx = (_php3_wddx_parser *) data;
	
	if(!strcmp(name, "var")) {
		int i;
		
		wddx->last_type = LT_VAR;
		for(i = 0; atts[i]; i++) 
			if(!strcmp("name", atts[i])) {
				if(atts[i + 1]) wddx->var_name = estrdup(atts[i + 1]);
				break;
			}
	} else if(!strcmp(name, "string")) {
		wddx->last_type = LT_STRING;
	} else if(!strcmp(name, "number")) {
		wddx->last_type = LT_NUMBER;
	} else if(!strcmp(name, "array") || !strcmp(name, "struct")) {
		if(wddx->depth++ || wddx->last_type != LT_VAR)
			wddx->type++;
		wddx->last_type = LT_ARRAY;
		FIRST;
		VAR_NAME;
		ATTACH("array(");
		wddx->notfirst = 0;
	}
}
/* }}} */
	/* {{{ _php3_wddx_end_element() */

static void _php3_wddx_end_element(void *data, const char *name)
{
	_php3_wddx_parser *wddx = (_php3_wddx_parser *) data;

	 if(!strcmp(name, "array") || !strcmp(name, "struct")) {
		 wddx->depth--;
		 wddx->type--;
		 ATTACH(")");
	 }
}
/* }}} */
	/* {{{ _php3_wddx_char_data() */

static void _php3_wddx_char_data(void *data, const char *s, int len)
{
	char buf[10];
	char *str;
	_php3_wddx_parser *wddx = (_php3_wddx_parser *) data;

	switch(wddx->last_type) {
		case LT_STRING:
			FIRST;
			VAR_NAME;
			sprintf(buf, "\"%%.%ds\"", len);
			str = emalloc(len + 4);
			sprintf(str, buf, s);
			ATTACH(str);
			efree(str);
			break;
		case LT_NUMBER:
			FIRST;
			VAR_NAME;
			sprintf(buf, "%%.%ds", len);
			str = emalloc(len + 4);
			sprintf(str, buf, s);
			ATTACH(str);
			efree(str);
			break;
		/* make compiler happy */
		case LT_ARRAY:
		case LT_VAR:
		default:
			break;
	}
}
/* }}} */
	/* {{{ _php3_wddx_unserialize() */

static char *_php3_wddx_unserialize(char *buf, char *prefix) 
{
	XML_Parser parser = XML_ParserCreate(NULL);
	_php3_wddx_parser wddx;
	
	memset(&wddx, 0, sizeof(wddx));
	wddx.prefix = prefix;
	XML_SetUserData(parser, &wddx);
	XML_SetElementHandler(parser, _php3_wddx_start_element, 
			_php3_wddx_end_element),
	XML_SetCharacterDataHandler(parser, _php3_wddx_char_data);
	XML_Parse(parser, buf, strlen(buf), 1);
	XML_ParserFree(parser);

	return wddx.buf ? wddx.buf : empty_string;
}
/* }}} */

/* {{{ proto string wddx_serialize(mixed var_name [, mixed ...])
   Creates a WDDX conforming representation of its arguments */
PHP_FUNCTION(wddx_serialize)
{
	pval **args;
	int ac = ARG_COUNT(ht);
	char *str;
	char *new;

	if(ac == 0 || (!(args = emalloc(sizeof(*args) * ac))) ||
			getParametersArray(ht, ac, args) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(ac == 1 && args[0]->type & IS_HASH && 
			!ParameterPassedByReference(ht, 1)) {
		str = _php3_wddx_struct_or_array(args[0]->value.ht);
	} else {
		str = _php3_wddx_args_array(ht, args, ac);
	}

	efree(args);
	
	if(str) {
		new = emalloc(strlen(str) + 100);
		sprintf(new, "<wddxPacket version='0.9'><header/><data>%s"
				"</data></wddxPacket>", str);
		efree(str);

		RETURN_STRING(new, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array wddx_unserialize(string arg [, string prefix])
   Unserializes a string in WDDX format */
PHP_FUNCTION(wddx_unserialize)
{
	pval *arg, *pprefix;
	int ac = ARG_COUNT(ht);
	char *prefix = NULL;
	
	if(ac < 1 || ac > 2 || getParameters(ht, ac, &arg, &pprefix) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	switch(ac) {
		case 2:
			convert_to_string(pprefix);
			prefix = pprefix->value.str.val;
		default:
			convert_to_string(arg);
	}

	RETURN_STRING(_php3_wddx_unserialize(arg->value.str.val, prefix), 0);
}
/* }}} */

#endif
