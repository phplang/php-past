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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id: variables.c,v 1.165 2000/04/10 19:29:36 andi Exp $ */
#include "php.h"
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>
#include "control_structures.h"
#include "internal_functions.h"
#include "functions/head.h"

PHPAPI char *empty_string = "";	/* in order to save emalloc() and efree() time for
								 * empty strings (usually used to denote empty
								 * return values in failed functions).
								 * The macro STR_FREE() will not efree() it.
								 */

PHPAPI char *undefined_variable_string = "\0";

/* this function MUST set the value for the variable to an empty string */
/* and empty strings must be evaluated as FALSE */
inline PHPAPI void var_reset(pval *var)
{
	var->type = IS_STRING;
	var->value.str.val = empty_string;
	var->value.str.len = 0;
}

inline PHPAPI void var_uninit(pval *var)
{
	var->type = IS_STRING;
	var->value.str.val = undefined_variable_string;
	var->value.str.len = 0;
}
		

inline void pval_destructor(pval *pvalue INLINE_TLS)
{
	if (pvalue->type == IS_STRING) {
		STR_FREE(pvalue->value.str.val);
	} else if (pvalue->type & (IS_ARRAY | IS_CLASS | IS_OBJECT)) {
		if (pvalue->value.ht && (pvalue->value.ht != &GLOBAL(symbol_table))) {
			_php3_hash_destroy(pvalue->value.ht);
			efree(pvalue->value.ht);
		}
	} else if (pvalue->type==IS_USER_FUNCTION) {
		if (pvalue->value.func.addr.statics) {
			_php3_hash_destroy(pvalue->value.func.addr.statics);
			efree(pvalue->value.func.addr.statics);
		}
		if (pvalue->value.func.arg_types) {
			efree(pvalue->value.func.arg_types);
		}
	}
}


PHPAPI void php3tls_pval_destructor(pval *pvalue)
{
	TLS_VARS;

	pval_destructor(pvalue _INLINE_TLS);
}


/* returns 1 if the type of pvalue is NOT IS_INTERNAL_FUNCTION */
inline int is_not_internal_function(pval *pvalue)
{
	if (pvalue->type == IS_INTERNAL_FUNCTION) {
		return 0;
	} else {
		return 1;
	}
}


PHPAPI int pval_copy_constructor(pval *pvalue)
{
	TLS_VARS;

	if (pvalue->type == IS_STRING && pvalue->value.str.val) {
		if (pvalue->value.str.len==0) {
			if (pvalue->value.str.val==undefined_variable_string) {
				pvalue->value.str.val = undefined_variable_string;
			} else {
				pvalue->value.str.val = empty_string;
			}
			return SUCCESS;
		}
		pvalue->value.str.val = (char *) estrndup(pvalue->value.str.val, pvalue->value.str.len);
		if (!pvalue->value.str.val) {
			var_reset(pvalue);
			return FAILURE;
		}
	} else if (pvalue->type & (IS_ARRAY | IS_CLASS | IS_OBJECT)) {
		pval tmp;

		if (!pvalue->value.ht || (pvalue->value.ht == &GLOBAL(symbol_table))) {
			var_reset(pvalue);
			return FAILURE;
		}
		_php3_hash_copy(&(pvalue->value.ht), pvalue->value.ht, (void (*)(void *pData)) pval_copy_constructor, (void *) &tmp, sizeof(pval));
		if (!pvalue->value.ht) {
			var_reset(pvalue);
			return FAILURE;
		}
	} else if (pvalue->type & VALID_FUNCTION) {
		if (pvalue->value.func.arg_types) {
			unsigned char *arg_types = pvalue->value.func.arg_types;
			
			pvalue->value.func.arg_types = emalloc(sizeof(unsigned char)*(arg_types[0]+1));
			memcpy(pvalue->value.func.arg_types, arg_types, sizeof(unsigned char)*(arg_types[0]+1));
		}
		if (pvalue->type == IS_USER_FUNCTION && pvalue->value.func.addr.statics) {
			HashTable *statics = pvalue->value.func.addr.statics;
			pval tmp;

			_php3_hash_copy(&pvalue->value.func.addr.statics, statics, (void (*)(void *)) pval_copy_constructor, &tmp, sizeof(pval));
		}
			
	}
	return SUCCESS;
}

void assign_to_list(pval *result, pval *list, pval *expr INLINE_TLS)
{
	int i,list_size;
	pval *varp=NULL, *vardata=NULL, *exprdata=NULL; /* Shut up the compiler */

	if (GLOBAL(Execute)) {
		list_size = _php3_hash_num_elements(list->value.ht);

		for (i=list_size-1; i>=0; i--) { /* Go in reverse because of unassigned variable stack */
			if (_php3_hash_index_find(list->value.ht, i, (void **)&varp) == FAILURE) {
				continue;
			}
			if (!varp->value.varptr.pvalue) {
				continue; /* Bad variable name */
			}
			if (varp->cs_data.array_write) {
				clean_unassigned_variable_top(0 _INLINE_TLS);
			}
			vardata = (pval *)varp->value.varptr.pvalue;
			pval_destructor(vardata _INLINE_TLS);	
			if (expr->type != IS_ARRAY) {
				if (i != 0) {
					var_uninit(vardata);
				} else {
					*vardata = *expr;
					pval_copy_constructor(vardata);
				}
			} else {
				if (_php3_hash_index_find(expr->value.ht, i, (void **)&exprdata) == FAILURE) {
					var_uninit(vardata);
				} else {
					*vardata = *exprdata;
					pval_copy_constructor(vardata);
				}
			}
		}
		*result = *expr;
		pval_destructor(list _INLINE_TLS);
	}
}


int assign_to_variable(pval *result, pval *var_ptr, pval *expr, int (*func) (pval *, pval *, pval *  INLINE_TLS) INLINE_TLS)
{
	pval *var = var_ptr->value.varptr.pvalue;

	if (!var) { /* didn't parse the variable successfully */
		pval_destructor(expr _INLINE_TLS);
		var_uninit(result);
		if (var_ptr->cs_data.array_write) { /* I'm not sure it can happen - should check it  -- Zeev */
			clean_unassigned_variable_top(1 _INLINE_TLS);
		}
		return FAILURE;
	}
	if (func && (var_ptr->cs_data.array_write || (var->type==IS_STRING && var->value.str.val==undefined_variable_string))) {
		variable_tracker *vt;

		if (php3i_stack_top(&GLOBAL(variable_unassign_stack),(void **) &vt)==SUCCESS) {
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
	if (var_ptr->value.varptr.string_offset == -1) { /* not indexed string */
		if (func) {
			func(result, var, expr _INLINE_TLS);
			*((pval *)var) = *result;
		} else {
			pval_destructor((pval *)var _INLINE_TLS);
			*((pval *)var) = *expr;
			*result = *expr;
		}
		return pval_copy_constructor(result);
	} else { /* indexed string */
		if (func) {
			php3_error(E_WARNING,"Only simple assignment supported with string indices");
			var_reset(result);
		} else {
			convert_to_string(expr);
			if (expr->value.str.len > 0) {
				/* Understand this and you're immediately accepted as a core developer! :) */
				((pval *)var)->value.str.val[var_ptr->value.varptr.string_offset] = expr->value.str.val[0];
				*result = *expr;
			} else {
				var_reset(result);
			}
		}
		return SUCCESS;
	}
}


int get_regular_variable_contents(pval *result, pval *varname, int free_varname INLINE_TLS)
{
	pval *data;

	if ((varname->type != IS_STRING)) {
		php3_error(E_WARNING, "Illegal variable name");
		var_reset(result);
		if (free_varname) {
			pval_destructor(varname _INLINE_TLS);
		}
		return FAILURE;
	} else {
		if (_php3_hash_find(GLOBAL(active_symbol_table), varname->value.str.val, varname->value.str.len+1, (void **) &data) == FAILURE) {
			php3_error(E_NOTICE, "Using uninitialized variable $%s", varname->value.str.val);
			var_reset(result);
			if (free_varname) {
				STR_FREE(varname->value.str.val);
			}
			return FAILURE;
		} else {
			*result = *data;
			if (free_varname) {
				STR_FREE(varname->value.str.val);
			}
			return pval_copy_constructor(result);
		}
	}
}


void get_array_variable(pval *result, pval *varname, pval *index INLINE_TLS)
{
	pval *array, *data;

	convert_double_to_long(index);

	if (_php3_hash_find(GLOBAL(active_symbol_table), varname->value.str.val, varname->value.str.len+1, (void **) &array) == FAILURE) {
		php3_error(E_NOTICE, "Using uninitialized array $%s", varname->value.str.val);
		var_reset(result);
		return;
	} else if (array->type == IS_STRING && index->type == IS_LONG) {
		if (index->value.lval < (long) array->value.str.len) {
			result->type = IS_STRING;
			result->value.str.val = (char *) emalloc(2);
			result->value.str.len = 1;
			result->value.str.val[0] = array->value.str.val[index->value.lval];
			result->value.str.val[1] = '\0';
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
		php3_error(E_WARNING, "Using scalar variable $%s as an array or object", varname->value.str.val);
		var_reset(result);
		return;
	} else {
		if (index->type == IS_LONG) {
			if (_php3_hash_index_find(array->value.ht, index->value.lval, (void **) &data) == FAILURE) {
				php3_error(E_NOTICE, "Using uninitialized index or property of $%s - %d", varname->value.str.val, index->value.lval);
				var_reset(result);
				return;
			} else {
				*result = *data;
				if (pval_copy_constructor(result) == FAILURE) {
					var_reset(result);
					return;
				}
			}
		} else if (index->type == IS_STRING) {
			if (_php3_hash_find(array->value.ht, index->value.str.val, index->value.str.len+1, (void **) &data) == FAILURE) {
				php3_error(E_NOTICE, "Using uninitialized index or property of $%s - '%s'", varname->value.str.val, index->value.str.val);
				var_reset(result);
				return;
			} else {
				*result = *data;
				if (pval_copy_constructor(result) == FAILURE) {
					var_reset(result);
					return;
				}
			}
		}
	}
	
	return;
}


int incdec_variable(pval *result, pval *var_ptr, int (*func) (pval *), int post INLINE_TLS)
{
	int ret=0;
	pval *var = var_ptr->value.varptr.pvalue;
	
	if (!var_ptr->value.varptr.pvalue) { /* didn't successfully parse the array */
		var_reset(result);
		if (var_ptr->cs_data.array_write) {  /* can probably not happen - should check  -- Zeev */
			clean_unassigned_variable_top(1 _INLINE_TLS);
		}
		return FAILURE;
	}
	if (var_ptr->cs_data.array_write || (var->type==IS_STRING && var->value.str.val==undefined_variable_string)) {
		variable_tracker *vt;

		if (php3i_stack_top(&GLOBAL(variable_unassign_stack),(void **) &vt)==SUCCESS) {
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
	
	if (post) {				/* post increment, use the original value */
		*result = *((pval *)var_ptr->value.varptr.pvalue);
		ret=pval_copy_constructor(result);
	}
	func(var_ptr->value.varptr.pvalue);
	if (!post) {
		*result = *((pval *)var_ptr->value.varptr.pvalue);
		ret=pval_copy_constructor(result);
	}

	return ret;
}


void php3i_print_variable(pval *var INLINE_TLS) 
{
	convert_to_string(var);
	if (var->value.str.len==0) { /* optimize away empty strings */
		return;
	}
	if (!php3_header()) return;
	PHPWRITE(var->value.str.val,var->value.str.len);
}

/* Not tested return values from the hash functions and emalloc like in the rest of this file.
 * this must be added at some stage although if they don't work we're probably gonna die 
 * anyways, because that'll usually mean out of memory
 */
void add_array_pair_list(pval *result, pval *index, pval *value, int initialize INLINE_TLS)
{
	if (initialize) {
		result->value.ht = (HashTable *) emalloc(sizeof(HashTable));
		_php3_hash_init(result->value.ht, 0, NULL, PVAL_DESTRUCTOR, 0);
		result->type = IS_ARRAY;
	}
	if (index) {
		if (index->type == IS_STRING) {
			_php3_hash_update(result->value.ht, index->value.str.val, index->value.str.len+1, value, sizeof(pval),NULL);
			pval_destructor(index _INLINE_TLS);
			return;
		} else if (index->type == IS_LONG || index->type == IS_DOUBLE) {
			if (index->type == IS_DOUBLE) {
				convert_to_long(index);
			}
			_php3_hash_index_update(result->value.ht, index->value.lval, value, sizeof(pval),NULL);
		} else {
			pval_destructor(index _INLINE_TLS);
			pval_destructor(value _INLINE_TLS);
			var_reset(result);
		}
	} else {
		_php3_hash_next_index_insert(result->value.ht, value, sizeof(pval),NULL);
	}
}


/****************************
 * Functions with YACC Rule *
 ****************************/

/* This function unset's the passed variable and returns 1 for success */
void php3_unset(pval *result, pval *var_ptr)
{
	TLS_VARS;

	if (GLOBAL(Execute)) {
		pval *var;
		
		if (!var_ptr || var_ptr->cs_data.array_write) { /* variable didn't exist before unset(), remove it completely */
			clean_unassigned_variable_top(1 _INLINE_TLS);
		} else if (!(var=var_ptr->value.varptr.pvalue)) {
			result->value.lval = 1; /* the variable didn't parse properly, so it was obviously not set */
		} else {
			pval_destructor(var _INLINE_TLS);
			var->value.str.val = undefined_variable_string;
			var->value.str.len=0;
			var->type = IS_STRING;
			/*clean_unassigned_variable_top(0 _INLINE_TLS);*/
		}
		result->value.lval = 1;	/* Successfull */
	}
}


void php3_isset(pval *result, pval *var_ptr)
{
	TLS_VARS;

	if (GLOBAL(Execute)) {
		pval *var;
		
		result->type = IS_LONG;
		if (!var_ptr || var_ptr->cs_data.array_write  || !(var=var_ptr->value.varptr.pvalue)
			|| (var->type == IS_STRING && var->value.str.val==undefined_variable_string)) {
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


void php3_empty(pval *result, pval *var_ptr)
{
	TLS_VARS;
	if (GLOBAL(Execute)) {
		TLS_VARS;
		
		php3_isset(result,var_ptr);
		if (result->value.lval) { /* variable is set */
			pval var = *((pval *) var_ptr->value.varptr.pvalue);
			
			if ((var.type == IS_STRING) && (var.value.str.len == 1) && (var.value.str.val[0] == '0')) {
				/* don't consider "0" as empty */
				result->value.lval=0;
			} else {
				pval_copy_constructor(&var);
				if (pval_is_true(&var)) {
					result->value.lval=0;
				}
			}
		} else { /* variable is not set */
			result->value.lval = 1;
		}
	}
}


void declare_class_variable(pval *varname, pval *expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval new_var;

		if (expr) {
			new_var = *expr;
		} else {
			var_reset(&new_var);
		}
		if (_php3_hash_update(GLOBAL(class_symbol_table), varname->value.str.val, varname->value.str.len+1, &new_var, sizeof(pval),NULL) == FAILURE) {
			php3_error(E_ERROR, "Unable to declare new variable %s::$%s", GLOBAL(class_name), varname->value.str.val);
		}
	}
}


void get_object_property(pval *result, pval *class_ptr, pval *varname INLINE_TLS)
{
	pval *data,*object=(pval *) class_ptr->value.varptr.pvalue;

	var_reset(result);

	if (!object) {
		pval_destructor(varname _INLINE_TLS);
		return;
	}
	if (varname->type != IS_STRING) {
		php3_error(E_WARNING, "Cannot use a numeric value as a variable name");
		pval_destructor(varname _INLINE_TLS);
		return;
	}
	if (_php3_hash_find(object->value.ht, varname->value.str.val, varname->value.str.len+1, (void **) &data) == FAILURE) {
		php3_error(E_NOTICE, "Using uninitialized variable $%s", varname->value.str.val);
		STR_FREE(varname->value.str.val);
		var_reset(result);
		return;
	} else {
		*result = *data;
		pval_copy_constructor(result);
		STR_FREE(varname->value.str.val);
	}
}


/* Use varptr.pvalue to maintain a pointer to the right symbol table.
 * At the end of the parsing, we should be pointing to the right symbol table.
 * 'parent' doesn't have to be cleaned since its just a pointer and doesn't
 * contain any actual data.
 */
void get_object_symtable(pval *result, pval *parent, pval *child INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		HashTable *target_symbol_table;
		pval *data;

		if (parent) {
			pval *object=(pval *) parent->value.varptr.pvalue;
			
			if (!object) {
				result->value.varptr.pvalue=NULL;
				return;
			}
			target_symbol_table = object->value.ht;
		} else {
			target_symbol_table = GLOBAL(active_symbol_table);
		}

		if (!GLOBAL(active_symbol_table)) {
			return;
		}
		
		if (_php3_hash_find(target_symbol_table, child->value.str.val, child->value.str.len+1, (void **) &data) == FAILURE) {
			php3_error(E_WARNING, "Object %s not found", child->value.str.val);
			result->value.varptr.pvalue=NULL;
			return;
		}
		if (data->type != IS_OBJECT) {
			php3_error(E_WARNING, "$%s is not an object", child->value.str.val);
			result->value.varptr.pvalue=NULL;
			return;
		}
		result->value.varptr.pvalue = data;		/* not running copy constructor intentionally */
	}
}


void assign_new_object(pval *result, pval *classname INLINE_TLS, unsigned char implicit_ctor)
{
	if (GLOBAL(Execute)) {
		pval *data, ctor_retval;

		convert_to_string(classname);
		if (_php3_hash_find(&GLOBAL(function_table), classname->value.str.val, classname->value.str.len+1, (void **) &data) == FAILURE
			|| data->type != IS_CLASS) {
			php3_error(E_ERROR, "%s is not a class", classname->value.str.val);
			return;
		}
		*result = *data;
		result->type = IS_OBJECT;
		pval_copy_constructor(result);
		
		if (implicit_ctor) {
			if (call_user_function(NULL, result, classname, &ctor_retval, 0, NULL)==SUCCESS) {
				pval_destructor(&ctor_retval);			
		  	}
		  	pval_destructor(classname); 
		}
	}
}


void read_pointer_value(pval *result,pval *var_ptr INLINE_TLS)
{
	pval *var=var_ptr->value.varptr.pvalue;
	
	if (var) {
		if (var_ptr->cs_data.array_write || (var->type==IS_STRING && var->value.str.val==undefined_variable_string)) {
		    variable_tracker *vt;
		        
			if (php3i_stack_top(&GLOBAL(variable_unassign_stack),(void **) &vt)==SUCCESS) {
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
			var_uninit(result);
			if (var_ptr->cs_data.array_write) {
				clean_unassigned_variable_top(1 _INLINE_TLS);
			}
		} else {
			if (var_ptr->value.str.len == -1) {  /* not indexed string */
				*result = *((pval *)var);
				pval_copy_constructor(result);
			} else {  /* indexed string */
				result->value.str.val = (char *) emalloc(2);
				result->value.str.val[0]=var->value.str.val[var_ptr->value.str.len];
				result->value.str.val[1]=0;
				result->value.str.len=1;
				result->type = IS_STRING;
			}
		}
	} else {
		var_uninit(result);
	}	
}


void get_regular_variable_pointer(pval *result, pval *varname INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		if ((varname->type != IS_STRING)) {
			php3_error(E_WARNING,"Illegal variable name");
			result->value.varptr.pvalue = NULL;
			return;
		} else {
			pval *data;

			if (_php3_hash_find(GLOBAL(active_symbol_table),varname->value.str.val, varname->value.str.len+1, (void **)&data) == FAILURE) {
				pval tmp;
				variable_tracker vt;

				var_uninit(&tmp);
				_php3_hash_update(GLOBAL(active_symbol_table),varname->value.str.val,varname->value.str.len+1,(void *) &tmp, sizeof(pval), (void **) &data);
				
				vt.type = IS_STRING;
				vt.strlen = varname->value.str.len;
				vt.strval = estrndup(varname->value.str.val,vt.strlen);
				vt.ht = GLOBAL(active_symbol_table);
				php3i_stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
				result->cs_data.array_write = 1;
			} else {
				result->cs_data.array_write = 0;
			}
			result->value.varptr.pvalue = data;
			result->value.str.len = -1; /* Not indexed string */
		}
		pval_destructor(varname _INLINE_TLS);
	}
}


/* CALLBACK functions to provide pointers to external modules
 * that may need them. 
 */
PHPAPI HashTable *php3i_get_symbol_table(void) {
	TLS_VARS;
	return &GLOBAL(symbol_table);
}

PHPAPI HashTable *php3i_get_function_table(void) {
	TLS_VARS;
	return &GLOBAL(function_table);
}


PHPAPI pval *php3i_long_pval(long value)
{
	pval *ret = emalloc(sizeof(pval));

	ret->type = IS_LONG;
	ret->value.lval = value;
	return ret;
}


PHPAPI pval *php3i_double_pval(double value)
{
	pval *ret = emalloc(sizeof(pval));

	ret->type = IS_DOUBLE;
	ret->value.dval = value;
	return ret;
}


PHPAPI pval *php3i_string_pval(const char *str)
{
	pval *ret = emalloc(sizeof(pval));
	int len = strlen(str);

	ret->type = IS_STRING;
	ret->value.str.len = len;
	ret->value.str.val = estrndup(str, len);
	return ret;
}


PHPAPI char *php3i_pval_strdup(pval *val)
{
	if (val->type == IS_STRING) {
		char *buf = emalloc(val->value.str.len + 1);
		memcpy(buf, val->value.str.val, val->value.str.len);
		buf[val->value.str.len] = '\0';
		return buf;
	}
	return NULL;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
