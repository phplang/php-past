/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: variables.c,v 1.123 1998/01/29 22:33:41 shane Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif

#include "parser.h"
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>
#include "control_structures.h"
#include "functions/head.h"

PHPAPI char *empty_string = "";	/* in order to save emalloc() and efree() time for
								 * empty strings (usually used to denote empty
								 * return values in failed functions).
								 * The macro STR_FREE() will not efree() it.
								 */

PHPAPI char *undefined_variable_string = "\0";

/* this function MUST set the value for the variable to an empty string */
/* and empty strings must be evaluated as FALSE */
inline PHPAPI void var_reset(YYSTYPE *var)
{
	var->type = IS_STRING;
	var->value.strval = empty_string;
	var->strlen = 0;
}


inline void yystype_destructor(YYSTYPE *yystype INLINE_TLS)
{
	if (yystype->type == IS_STRING) {
		STR_FREE(yystype->value.strval);
	} else if (yystype->type & (IS_ARRAY | IS_CLASS | IS_OBJECT | IS_USER_FUNCTION)) {
		if (yystype->value.ht && (yystype->value.ht != &GLOBAL(symbol_table))) {
			hash_destroy(yystype->value.ht);
			efree(yystype->value.ht);
		}
	}
}

/* returns 1 if the type of yystype is NOT IS_INTERNAL_FUNCTION */
inline int is_not_internal_function(YYSTYPE *yystype)
{
	if (yystype->type == IS_INTERNAL_FUNCTION) {
		return 0;
	} else {
		return 1;
	}
}


int yystype_copy_constructor(YYSTYPE *yystype)
{
	TLS_VARS;

	if (yystype->type == IS_STRING && yystype->value.strval) {
		if (yystype->strlen==0) {
			yystype->value.strval = empty_string;
			return SUCCESS;
		}
		yystype->value.strval = (char *) estrndup(yystype->value.strval, yystype->strlen);
		if (!yystype->value.strval) {
			var_reset(yystype);
			return FAILURE;
		}
	} else if (yystype->type & (IS_ARRAY | IS_CLASS | IS_OBJECT)) {
		YYSTYPE tmp;

		if (!yystype->value.ht || (yystype->value.ht == &GLOBAL(symbol_table))) {
			var_reset(yystype);
			return FAILURE;
		}
		hash_copy(&(yystype->value.ht), yystype->value.ht, (void (*)(void *pData)) yystype_copy_constructor, (void *) &tmp, sizeof(YYSTYPE));
		if (!yystype->value.ht) {
			var_reset(yystype);
			return FAILURE;
		}
	}
	return SUCCESS;
}

void assign_to_list(YYSTYPE *result, YYSTYPE *list, YYSTYPE *expr INLINE_TLS)
{
	int i,list_size;
	YYSTYPE *varp=NULL, *vardata=NULL, *exprdata=NULL; /* Shut up the compiler */

	if (GLOBAL(Execute)) {
		list_size = hash_num_elements(list->value.ht);

		for (i=list_size-1; i>=0; i--) { /* Go in reverse because of unassigned variable stack */
			hash_index_find(list->value.ht, i, (void **)&varp);
			if (!varp->value.yystype_ptr) {
				continue; /* Bad variable name */
			}
			if (varp->cs_data.array_write) {
				clean_unassigned_variable_top(0 _INLINE_TLS);
			}
			vardata = (YYSTYPE *)varp->value.yystype_ptr;
			yystype_destructor(vardata _INLINE_TLS);	
			if (expr->type != IS_ARRAY) {
				if (i != 0) {
					var_reset(vardata);
				} else {
					*vardata = *expr;
					yystype_copy_constructor(vardata);
				}
			} else {
				if (hash_index_find(expr->value.ht, i, (void **)&exprdata) == FAILURE) {
					var_reset(vardata);
				} else {
					*vardata = *exprdata;
					yystype_copy_constructor(vardata);
				}
			}
		}
		*result = *expr;
		yystype_destructor(list _INLINE_TLS);
	}
}


int assign_to_variable(YYSTYPE *result, YYSTYPE *var_ptr, YYSTYPE *expr, int (*func) (YYSTYPE *, YYSTYPE *, YYSTYPE *  INLINE_TLS) INLINE_TLS)
{
	YYSTYPE *var = var_ptr->value.yystype_ptr;

	if (func && (var_ptr->cs_data.array_write || (var->type==IS_STRING && var->value.strval==undefined_variable_string))) {
		variable_tracker *vt;

		if (stack_top(&GLOBAL(variable_unassign_stack),(void **) &vt)==SUCCESS) {
			switch(vt->type) {
				case IS_STRING:
					php3_error(E_NOTICE,"Uninitialized variable or array index or property (%s)",vt->strval);
					break;
				case IS_LONG:
					php3_error(E_NOTICE,"Uninitialized array index (%d)",vt->lval);
					break;
			}
		} else {
			php3_error(E_NOTICE,"Uninitialized variable");
		}
	}
	if (var_ptr->cs_data.array_write) {
		clean_unassigned_variable_top(0 _INLINE_TLS);
	}
	if (!var_ptr->value.yystype_ptr) {
		var_reset(result);
		return FAILURE;
	}
	if (var_ptr->strlen == -1) { /* not indexed string */
		if (func) {
			func(result, var_ptr->value.yystype_ptr, expr _INLINE_TLS);
			*((YYSTYPE *)var_ptr->value.yystype_ptr) = *result;
		} else {
			yystype_destructor((YYSTYPE *)var_ptr->value.yystype_ptr _INLINE_TLS);
			*((YYSTYPE *)var_ptr->value.yystype_ptr) = *expr;
			*result = *expr;
		}
		return yystype_copy_constructor(result);
	} else { /* indexed string */
		if (func) {
			php3_error(E_WARNING,"Only simple assignment supported with string indices");
			var_reset(result);
		} else {
			convert_to_string(expr);
			if (expr->strlen > 0) {
				/* Understand this and you're immediately accepted as a core developer! :) */
				((YYSTYPE *)var_ptr->value.yystype_ptr)->value.strval[var_ptr->strlen] = expr->value.strval[0];
				*result = *expr;
			} else {
				var_reset(result);
			}
		}
		return SUCCESS;
	}
}


int get_regular_variable_contents(YYSTYPE *result, YYSTYPE *varname, int free_varname INLINE_TLS)
{
	YYSTYPE *data;

	if ((varname->type != IS_STRING)) {
		php3_error(E_WARNING, "Illegal variable name");
		var_reset(result);
		if (free_varname) {
			yystype_destructor(varname _INLINE_TLS);
		}
		return FAILURE;
	} else {
		if (hash_find(GLOBAL(active_symbol_table), varname->value.strval, varname->strlen+1, (void **) &data) == FAILURE) {
			php3_error(E_NOTICE, "Using uninitialized variable $%s", varname->value.strval);
			var_reset(result);
			if (free_varname) {
				STR_FREE(varname->value.strval);
			}
			return FAILURE;
		} else {
			*result = *data;
			if (free_varname) {
				STR_FREE(varname->value.strval);
			}
			return yystype_copy_constructor(result);
		}
	}
}


void get_array_variable(YYSTYPE *result, YYSTYPE *varname, YYSTYPE *index INLINE_TLS)
{
	YYSTYPE *array, *data;

	convert_double_to_long(index);

	if (hash_find(GLOBAL(active_symbol_table), varname->value.strval, varname->strlen+1, (void **) &array) == FAILURE) {
		php3_error(E_NOTICE, "Using uninitialized array $%s", varname->value.strval);
		var_reset(result);
		return;
	} else if (array->type == IS_STRING && index->type == IS_LONG) {
		if (index->value.lval < (long) array->strlen) {
			result->type = IS_STRING;
			result->value.strval = (char *) emalloc(2);
			result->strlen = 1;
			result->value.strval[0] = array->value.strval[index->value.lval];
			result->value.strval[1] = '\0';
		} else {
			php3_error(E_WARNING, "No such index in string");
			var_reset(result);
			return;
		}
	} else if ((array->type != IS_ARRAY) && (array->type != IS_OBJECT) && (index->type == IS_LONG) && (index->value.lval == 0)) {
		/* For compatability with PHP 2.0 */
		get_regular_variable_contents(result, varname, 0 _INLINE_TLS);
		return;
	} else if (array->type != IS_ARRAY && array->type != IS_OBJECT) {
		php3_error(E_WARNING, "Using scalar variable $%s as an array or object", varname->value.strval);
		var_reset(result);
		return;
	} else {
		if (index->type == IS_LONG) {
			if (hash_index_find(array->value.ht, index->value.lval, (void **) &data) == FAILURE) {
				php3_error(E_NOTICE, "Using uninitialized index or property of $%s - %d", varname->value.strval, index->value.lval);
				var_reset(result);
				return;
			} else {
				*result = *data;
				if (yystype_copy_constructor(result) == FAILURE) {
					var_reset(result);
					return;
				}
			}
		} else if (index->type == IS_STRING) {
			if (hash_find(array->value.ht, index->value.strval, index->strlen+1, (void **) &data) == FAILURE) {
				php3_error(E_NOTICE, "Using uninitialized index or property of $%s - '%s'", varname->value.strval, index->value.strval);
				var_reset(result);
				return;
			} else {
				*result = *data;
				if (yystype_copy_constructor(result) == FAILURE) {
					var_reset(result);
					return;
				}
			}
		}
	}
	
	return;
}


int incdec_variable(YYSTYPE *result, YYSTYPE *var_ptr, int (*func) (YYSTYPE *), int post INLINE_TLS)
{
	int ret=0;
	YYSTYPE *var = var_ptr->value.yystype_ptr;
	
	if (var_ptr->cs_data.array_write || (var->type==IS_STRING && var->value.strval==undefined_variable_string)) {
		variable_tracker *vt;

		if (stack_top(&GLOBAL(variable_unassign_stack),(void **) &vt)==SUCCESS) {
			switch(vt->type) {
				case IS_STRING:
					php3_error(E_NOTICE,"Uninitialized variable or array index or property (%s)",vt->strval);
					break;
				case IS_LONG:
					php3_error(E_NOTICE,"Uninitialized array index (%d)",vt->lval);
					break;
			}
		} else {
			php3_error(E_NOTICE,"Uninitialized variable");
		}
	}
	if (var_ptr->cs_data.array_write) {
		clean_unassigned_variable_top(0 _INLINE_TLS);
	}
	
	if (!var_ptr->value.yystype_ptr) { /* didn't successfully parse the array */
		var_reset(result);
		return FAILURE;
	}

	if (post) {				/* post increment, use the original value */
		*result = *((YYSTYPE *)var_ptr->value.yystype_ptr);
		ret=yystype_copy_constructor(result);
	}
	func(var_ptr->value.yystype_ptr);
	if (!post) {
		*result = *((YYSTYPE *)var_ptr->value.yystype_ptr);
		ret=yystype_copy_constructor(result);
	}

	return ret;
}


void print_variable(YYSTYPE *varname INLINE_TLS) 
{
	if(!php3_header(0, NULL)) return;
	switch (varname->type) {
		case IS_LONG:
			php3_printf("%ld", varname->value.lval);
			break;
		case IS_DOUBLE:
			php3_printf("%f", varname->value.dval);
			break;
		case IS_STRING:
#if 0
/*
	Really not sure about this one.  Will probably lead to lots of
	confusion - although this is the way it was done in PHP2 - Rasmus
*/

			if(php3_ini.magic_quotes_gpc) {
				_php3_stripslashes(varname->value.strval);
				varname->strlen = strlen(varname->value.strval);
			}
#endif
			PUTS(varname->value.strval);
			break;
		case IS_ARRAY:
			PUTS("Array\n");
			break;
		case IS_OBJECT:
			PUTS("Object\n");
			break;
		default:
			PUTS("Unknown data type in echo function.\n");
			break;
	}
}

/* Not tested return values from the hash functions and emalloc like in the rest of this file.
 * this must be added at some stage although if they don't work we're probably gonna die 
 * anyways, because that'll usually mean out of memory
 */
void add_array_pair_list(YYSTYPE *result, YYSTYPE *index, YYSTYPE *value, int initialize INLINE_TLS)
{
	if (initialize) {
		result->value.ht = (HashTable *) emalloc(sizeof(HashTable));
		hash_init(result->value.ht, 0, NULL, YYSTYPE_DESTRUCTOR, 0);
		result->type = IS_ARRAY;
	}
	if (index) {
		if (index->type == IS_STRING) {
			hash_update(result->value.ht, index->value.strval, index->strlen+1, value, sizeof(YYSTYPE),NULL);
			yystype_destructor(index _INLINE_TLS);
			return;
		} else if (index->type == IS_LONG || index->type == IS_DOUBLE) {
			if (index->type == IS_DOUBLE) {
				convert_to_long(index);
			}
			hash_index_update(result->value.ht, index->value.lval, value, sizeof(YYSTYPE),NULL);
		} else {
			yystype_destructor(index _INLINE_TLS);
			yystype_destructor(value _INLINE_TLS);
			var_reset(result);
		}
	} else {
		hash_next_index_insert(result->value.ht, value, sizeof(YYSTYPE),NULL);
	}
}


/****************************
 * Functions with YACC Rule *
 ****************************/

/* This function unset's the passed variable and returns 1 for success */
void php3_unset(YYSTYPE *result, YYSTYPE *var_ptr)
{
	TLS_VARS;

	if (GLOBAL(Execute)) {
		YYSTYPE *var;
		
		if (!var_ptr || var_ptr->cs_data.array_write) { /* variable didn't exist before unset(), remove it completely */
			clean_unassigned_variable_top(1 _INLINE_TLS);
		} else {
			var = var_ptr->value.yystype_ptr;
			yystype_destructor(var _INLINE_TLS);
			var->value.strval = undefined_variable_string;
			var->strlen=0;
			var->type = IS_STRING;
			/*clean_unassigned_variable_top(0 _INLINE_TLS);*/
		}
		result->value.lval = 1;	/* Successfull */
	}
}


void php3_isset(YYSTYPE *result, YYSTYPE *var_ptr)
{
	TLS_VARS;

	if (GLOBAL(Execute)) {
		YYSTYPE *var;
		
		result->type = IS_LONG;
		if (!var_ptr || var_ptr->cs_data.array_write  || !(var=var_ptr->value.yystype_ptr)
			|| (var->type == IS_STRING && var->value.strval==undefined_variable_string)) {
			result->value.lval=0;
			if (var_ptr && var_ptr->cs_data.array_write) {
				clean_unassigned_variable_top(1 _INLINE_TLS);
			}
		} else {
			result->value.lval=1;
			/*clean_unassigned_variable_top(0 _INLINE_TLS);*/
		}
	}
}


void declare_class_variable(YYSTYPE *varname, YYSTYPE *expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE new_var;

		if (expr) {
			new_var = *expr;
		} else {
			var_reset(&new_var);
		}
		if (hash_update(GLOBAL(class_symbol_table), varname->value.strval, varname->strlen+1, &new_var, sizeof(YYSTYPE),NULL) == FAILURE) {
			php3_error(E_ERROR, "Unable to declare new variable %s::$%s", GLOBAL(class_name), varname->value.strval);
		}
	}
}


void get_object_property(YYSTYPE *result, YYSTYPE *class_ptr, YYSTYPE *varname INLINE_TLS)
{
	YYSTYPE *data,*object=(YYSTYPE *) class_ptr->value.yystype_ptr;

	var_reset(result);

	if (!object) {
		yystype_destructor(varname _INLINE_TLS);
		return;
	}
	if (varname->type != IS_STRING) {
		php3_error(E_WARNING, "Cannot use a numeric value as a variable name");
		yystype_destructor(varname _INLINE_TLS);
		return;
	}
	if (hash_find(object->value.ht, varname->value.strval, varname->strlen+1, (void **) &data) == FAILURE) {
		php3_error(E_NOTICE, "Using uninitialized variable $%s", varname->value.strval);
		STR_FREE(varname->value.strval);
		var_reset(result);
		return;
	} else {
		*result = *data;
		yystype_copy_constructor(result);
		STR_FREE(varname->value.strval);
	}
}


/* Use yystype_ptr to maintain a pointer to the right symbol table.
 * At the end of the parsing, we should be pointing to the right symbol table.
 * 'parent' doesn't have to be cleaned since its just a pointer and doesn't
 * contain any actual data.
 */
void get_object_symtable(YYSTYPE *result, YYSTYPE *parent, YYSTYPE *child INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		HashTable *target_symbol_table;
		YYSTYPE *data;

		if (parent) {
			YYSTYPE *object=(YYSTYPE *) parent->value.yystype_ptr;
			
			if (!object) {
				result->value.yystype_ptr=NULL;
				return;
			}
			target_symbol_table = object->value.ht;
		} else {
			target_symbol_table = GLOBAL(active_symbol_table);
		}

		if (!GLOBAL(active_symbol_table)) {
			return;
		}
		
		if (hash_find(target_symbol_table, child->value.strval, child->strlen+1, (void **) &data) == FAILURE) {
			php3_error(E_WARNING, "Object %s not found", child->value.strval);
			result->value.yystype_ptr=NULL;
			return;
		}
		if (data->type != IS_OBJECT) {
			php3_error(E_WARNING, "$%s is not an object", child->value.strval);
			result->value.yystype_ptr=NULL;
			return;
		}
		result->value.yystype_ptr = data;		/* not running copy constructor intentionally */
	}
}


void assign_new_object(YYSTYPE *result, YYSTYPE *classname INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE *data;

		if (hash_find(&GLOBAL(function_table), classname->value.strval, classname->strlen+1, (void **) &data) == FAILURE
			|| data->type != IS_CLASS) {
			php3_error(E_ERROR, "%s is not a class", classname->value.strval);
			return;
		}
		*result = *data;
		result->type = IS_OBJECT;
		yystype_copy_constructor(result);
	}
}


void read_pointer_value(YYSTYPE *result,YYSTYPE *var_ptr INLINE_TLS)
{
	YYSTYPE *var=var_ptr->value.yystype_ptr;
	
	if (var) {
		if (var_ptr->cs_data.array_write || (var->type==IS_STRING && var->value.strval==undefined_variable_string)) {
		    variable_tracker *vt;
		        
			if (stack_top(&GLOBAL(variable_unassign_stack),(void **) &vt)==SUCCESS) {
				switch(vt->type) {
					case IS_STRING:
						php3_error(E_NOTICE,"Uninitialized variable or array index or property (%s)",vt->strval);
						break;
					case IS_LONG:
						php3_error(E_NOTICE,"Uninitialized array index (%d)",vt->lval);
						break;
				}
			} else {
				php3_error(E_NOTICE,"Uninitialized variable");
			}
			var_reset(result);
			if (var_ptr->cs_data.array_write) {
				clean_unassigned_variable_top(1 _INLINE_TLS);
			}
		} else {
			if (var_ptr->strlen == -1) {  /* not indexed string */
				*result = *((YYSTYPE *)var_ptr->value.yystype_ptr);
				yystype_copy_constructor(result);
			} else {  /* indexed string */
				YYSTYPE *tmp=(YYSTYPE *) var_ptr->value.yystype_ptr;
	
				result->value.strval = (char *) emalloc(2);
				result->value.strval[0]=tmp->value.strval[var_ptr->strlen];
				result->value.strval[1]=0;
				result->strlen=1;
				result->type = IS_STRING;
			}
		}
	} else {
		var_reset(result);
	}	
}


void get_regular_variable_pointer(YYSTYPE *result, YYSTYPE *varname INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		if ((varname->type != IS_STRING)) {
			php3_error(E_WARNING,"Illegal variable name");
			result->value.yystype_ptr = NULL;
			return;
		} else {
			YYSTYPE *data;

			if (hash_find(GLOBAL(active_symbol_table),varname->value.strval, varname->strlen+1, (void **)&data) == FAILURE) {
				YYSTYPE tmp;
				variable_tracker vt;

				var_reset(&tmp);
				hash_update(GLOBAL(active_symbol_table),varname->value.strval,varname->strlen+1,(void *) &tmp, sizeof(YYSTYPE), (void **) &data);
				
				vt.type = IS_STRING;
				vt.strlen = varname->strlen;
				vt.strval = estrndup(varname->value.strval,vt.strlen);
				vt.ht = GLOBAL(active_symbol_table);
				stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
				result->cs_data.array_write = 1;
			} else {
				result->cs_data.array_write = 0;
			}
			result->value.yystype_ptr = data;
			result->strlen = -1; /* Not indexed string */
		}
		yystype_destructor(varname _INLINE_TLS);
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
