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


/* $Id: control_structures_inline.h,v 1.206 2000/07/04 16:36:07 rasmus Exp $ */

#include "php.h"
#include "internal_functions.h"
#include "functions/head.h"
#include <stdio.h>
#include <string.h>

extern pval *array_ptr;
static pval return_value;
HashTable list, plist;

extern inline int cs_global_variable(pval *varname INLINE_TLS);
extern inline int cs_static_variable(pval *varname, pval *value INLINE_TLS);
extern inline void cs_start_while(pval *while_token, pval *expr INLINE_TLS);
extern inline void cs_end_while(pval *while_token, int *yychar INLINE_TLS);
extern inline void cs_start_do_while(pval *do_token INLINE_TLS);
extern inline void cs_force_eval_do_while( INLINE_TLS_VOID);
extern inline void cs_end_do_while(pval *do_token, pval *expr, int *yychar INLINE_TLS);
extern inline void cs_start_if (pval *expr INLINE_TLS);
extern inline void cs_end_if ( INLINE_TLS_VOID);
extern inline int cs_break_continue(pval *expr, int op_type INLINE_TLS);
extern inline void cs_elseif_start_evaluate( INLINE_TLS_VOID);
extern inline void cs_elseif_end_evaluate( INLINE_TLS_VOID);
extern inline void cs_start_elseif (pval *expr INLINE_TLS);
extern inline void cs_start_else( INLINE_TLS_VOID);
extern inline void get_function_parameter(pval *varname, unsigned char type, pval *default_value INLINE_TLS);
extern inline void pass_parameter_by_value(pval *expr INLINE_TLS);
extern inline void start_function_decleration( INLINE_TLS_VOID);
extern inline void end_function_decleration(pval *function_token, pval *function_name INLINE_TLS);
extern inline void for_pre_expr1(pval *for_token INLINE_TLS);
extern inline void for_pre_expr2(pval *for_token INLINE_TLS);
extern inline void for_pre_expr3(pval *for_token, pval *expr2 INLINE_TLS);
extern inline void for_pre_statement(pval *for_token, pval *expr2, pval *expr3 INLINE_TLS);
extern inline void for_post_statement(pval *for_token, pval *first_semicolon, pval *second_semicolon, pval *close_parentheses, int *yychar INLINE_TLS);
extern inline void cs_return(pval *expr INLINE_TLS);
extern inline void php3cs_start_require(pval *include_token INLINE_TLS);
extern inline void php3cs_end_require(pval *include_token, pval *expr INLINE_TLS);
extern inline void cs_show_source(pval *expr INLINE_TLS);
extern inline void cs_pre_boolean_or(pval *left_expr INLINE_TLS);
extern inline void cs_post_boolean_or(pval *result, pval *left_expr, pval *right_expr INLINE_TLS);
extern inline void cs_pre_boolean_and(pval *left_expr INLINE_TLS);
extern inline void cs_post_boolean_and(pval *result, pval *left_expr, pval *right_expr INLINE_TLS);
extern inline void cs_questionmark_op_pre_expr1(pval *truth_value INLINE_TLS);
extern inline void cs_questionmark_op_pre_expr2(pval *truth_value INLINE_TLS);
extern inline void cs_questionmark_op_post_expr2(pval *result, pval *truth_value, pval *expr1, pval *expr2 INLINE_TLS);
extern inline void cs_functioncall_pre_variable_passing(pval *function_name, pval *class_ptr, unsigned char free_function_name INLINE_TLS);
extern inline void cs_functioncall_post_variable_passing(pval *function_name, int *yychar INLINE_TLS);
extern inline void cs_functioncall_end(pval *result, pval *function_name, pval *close_parentheses, int *yychar, unsigned char free_function_name INLINE_TLS);
extern inline void cs_switch_start(pval *switch_token, pval *expr INLINE_TLS);
extern inline void cs_switch_case_pre(pval *case_expr INLINE_TLS);
extern inline void cs_switch_case_post( INLINE_TLS_VOID);
extern inline void cs_switch_end(pval *expr INLINE_TLS);
extern inline void cs_start_class_decleration(pval *classname, pval *parent INLINE_TLS);
extern inline void cs_end_class_decleration( INLINE_TLS_VOID);
extern inline void start_array_parsing(pval *array_name,pval *class_ptr INLINE_TLS);
extern inline void start_dimensions_parsing(pval *result INLINE_TLS);
extern inline void fetch_array_index(pval *result, pval *expr, pval *dimension INLINE_TLS);
extern inline void end_array_parsing(pval *result, pval *dimensions INLINE_TLS);
extern inline void clean_unassigned_variable_top(int delete_var INLINE_TLS);
extern inline void get_class_variable_pointer(pval *result, pval *class_ptr, pval *varname INLINE_TLS);
extern inline void pass_parameter(pval *var, int by_reference INLINE_TLS);
extern inline void cs_system(pval *result, pval *expr INLINE_TLS);
extern inline void add_regular_encapsed_variable(pval *result,pval *encaps,pval *varname  INLINE_TLS);
extern inline void add_assoc_array_encapsed_variable(pval *result, pval *encaps, pval *varname, pval *index INLINE_TLS);
extern inline void add_regular_array_encapsed_variable(pval *result, pval *encaps, pval *varname, pval *index INLINE_TLS);
extern inline void add_variable_array_encapsed_variable(pval *result, pval *encaps, pval *varname, pval *var_index INLINE_TLS);
extern inline void add_encapsed_object_property(pval *result, pval *encaps, pval *object, pval *property INLINE_TLS);
extern inline void add_indirect_encapsed_variable(pval *result,pval *encaps,pval *varname INLINE_TLS);


inline int cs_global_variable(pval *varname INLINE_TLS)
{
	pval *localdata;

	if (GLOBAL(Execute)) {
		if (!GLOBAL(function_state).function_name) {
			php3_error(E_WARNING, "GLOBAL variable declaration meaningless in main() scope");
			return FAILURE;
		}
		if ((varname->type != IS_STRING)) {
			pval_destructor(varname _INLINE_TLS);
			php3_error(E_WARNING, "Incorrect variable type or name in global in function %s()", GLOBAL(function_state).function_name);
			return FAILURE;
		}
		if (_php3_hash_find(GLOBAL(active_symbol_table), varname->value.str.val, varname->value.str.len+1, (void **) &localdata) == SUCCESS) {
			php3_error(E_WARNING, "Variable used in global statement already exists in the function");
			STR_FREE(varname->value.str.val);
			return FAILURE;
		}
		if (_php3_hash_find(&GLOBAL(symbol_table), varname->value.str.val, varname->value.str.len+1, (void **) &localdata) == FAILURE) {
			pval tmp;
		
			tmp.type = IS_STRING;
			tmp.value.str.val = undefined_variable_string;	
			tmp.value.str.len = 0;
			
			if (_php3_hash_update(&GLOBAL(symbol_table), varname->value.str.val, varname->value.str.len+1, (void *) &tmp, sizeof(pval), (void **) &localdata)==FAILURE) {
				php3_error(E_WARNING, "Unable to initialize global variable $%s",varname->value.str.val);
				STR_FREE(varname->value.str.val);
				return FAILURE;
			}
		}
		if (_php3_hash_pointer_update(GLOBAL(active_symbol_table), varname->value.str.val, varname->value.str.len+1, (void *) localdata) == FAILURE) {
			php3_error(E_WARNING, "Error updating symbol table");
			STR_FREE(varname->value.str.val);
			return FAILURE;
		}
		STR_FREE(varname->value.str.val);
	}
	return SUCCESS;
}

inline int cs_static_variable(pval *varname, pval *value INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval *func_ent, *variable_entry, tmp;

		if (!GLOBAL(function_state).function_name) {
			php3_error(E_WARNING, "STATIC variable declaration meaningless in main() scope");
			STR_FREE(varname->value.str.val);
			if (value) {
				pval_destructor(value _INLINE_TLS);
			}
			return FAILURE;
		}
		if ((varname->type != IS_STRING)) {
			pval_destructor(varname _INLINE_TLS);
			pval_destructor(value _INLINE_TLS);
			php3_error(E_WARNING, "Incorrect variable type or name in static in function %s()", GLOBAL(function_state).function_name);
			return FAILURE;
		}
		if (_php3_hash_find(GLOBAL(function_state).hosting_function_table, GLOBAL(function_state).function_name, strlen(GLOBAL(function_state).function_name)+1, (void **) &func_ent) == FAILURE) {
			STR_FREE(varname->value.str.val);
			if (value) {
				pval_destructor(value _INLINE_TLS);
			}
			return FAILURE;
		}
		if (!func_ent->value.func.addr.statics) {
 			func_ent->value.func.addr.statics = (HashTable *) emalloc(sizeof(HashTable));
			_php3_hash_init(func_ent->value.func.addr.statics, 0, NULL, PVAL_DESTRUCTOR, 0);
		}
		if (_php3_hash_find(func_ent->value.func.addr.statics, varname->value.str.val, varname->value.str.len+1, (void **) &variable_entry) == FAILURE) {
			if (value) {
				_php3_hash_update(func_ent->value.func.addr.statics, varname->value.str.val, varname->value.str.len+1, value, sizeof(pval), (void **) &variable_entry);
			} else {
				var_uninit(&tmp);
				_php3_hash_update(func_ent->value.func.addr.statics, varname->value.str.val, varname->value.str.len+1, &tmp, sizeof(pval), (void **) &variable_entry);
			}
			/*
			if (_php3_hash_find(func_ent->value.func.addr.statics, varname->value.str.val, varname->value.str.len+1, (void **) &variable_entry) == FAILURE) {
				php3_error(E_ERROR, "Inserted static variable got lost");
				STR_FREE(varname->value.str.val);
				if (value) {
					pval_destructor(value);
				}
				return FAILURE;
			}
			*/
		}
		if (_php3_hash_pointer_update(GLOBAL(active_symbol_table), varname->value.str.val, varname->value.str.len+1, (void *) variable_entry) == FAILURE) {
			php3_error(E_ERROR, "Unable to initialize static variable");
			STR_FREE(varname->value.str.val);
			if (value) {
				pval_destructor(value _INLINE_TLS);
			}
			return FAILURE;
		} else {
			_php3_hash_find(GLOBAL(active_symbol_table), varname->value.str.val, varname->value.str.len+1, (void **) &variable_entry);
		}
		STR_FREE(varname->value.str.val);
	}
	return SUCCESS;
}

inline void cs_start_while(pval *while_token, pval *expr INLINE_TLS)
{
	GLOBAL(function_state).loop_nest_level++;
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));

	if (GLOBAL(Execute)) {
		tc_set_token(&GLOBAL(token_cache_manager), while_token->offset, WHILE);
		if (pval_is_true(expr)) {
			GLOBAL(ExecuteFlag) = EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		} else {
			GLOBAL(ExecuteFlag) = DONT_EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		}
		pval_destructor(expr _INLINE_TLS);
	}
}

inline void cs_end_while(pval *while_token,int *yychar INLINE_TLS)
{
#if (_DEBUG_ & CONTROL_DEBUG)
	fprintf(stderr, "end of while, execute=%d, changecount=%d, changetype=%d\n", Execute, GLOBAL(function_state).loop_change_level, GLOBAL(function_state).loop_change_level);
#endif

	if (GLOBAL(Execute)) {
		tc_set_token(&GLOBAL(token_cache_manager), while_token->offset, CONTINUED_WHILE);
 		seek_token(&GLOBAL(token_cache_manager), while_token->offset, yychar);
	} else {
		if ((GLOBAL(function_state).loop_change_type != DO_NOTHING) && (GLOBAL(function_state).loop_change_level == GLOBAL(function_state).loop_nest_level)) {
			if (GLOBAL(function_state).loop_change_type == DO_CONTINUE) {
				tc_set_token(&GLOBAL(token_cache_manager), while_token->offset, CONTINUED_WHILE);
				seek_token(&GLOBAL(token_cache_manager), while_token->offset, yychar);
			}
			GLOBAL(function_state).loop_change_type = DO_NOTHING;
			GLOBAL(function_state).loop_change_level = 0;
		}
	}
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	GLOBAL(function_state).loop_nest_level--;
}


inline void cs_start_do_while(pval *do_token INLINE_TLS)
{
	GLOBAL(function_state).loop_nest_level++;
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(Execute)) {
		tc_set_token(&GLOBAL(token_cache_manager), do_token->offset, DO);
	}
}


/* force evaluation of the expression if we're in a CONTINUE */
inline void cs_force_eval_do_while( INLINE_TLS_VOID)
{
	if (GLOBAL(function_state).loop_change_level == GLOBAL(function_state).loop_nest_level
		&& GLOBAL(function_state).loop_change_type == DO_CONTINUE) {
		GLOBAL(function_state).loop_change_type = DO_NOTHING;
		GLOBAL(function_state).loop_change_level = 0;
		GLOBAL(ExecuteFlag) = EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_end_do_while(pval *do_token, pval *expr, int *yychar INLINE_TLS)
{
#if (_DEBUG_ & CONTROL_DEBUG)
	fprintf(stderr, "end of while, execute=%d, changecount=%d, changetype=%d\n", Execute, GLOBAL(function_state).loop_change_level, GLOBAL(function_state).loop_change_level);
#endif

	if (GLOBAL(Execute) && pval_is_true(expr)) {
		pval_destructor(expr _INLINE_TLS);
		tc_set_token(&GLOBAL(token_cache_manager), do_token->offset, CONTINUED_DOWHILE);
		seek_token(&GLOBAL(token_cache_manager), do_token->offset, yychar);
	} else {
		if (GLOBAL(Execute)) {
			pval_destructor(expr _INLINE_TLS);
		}
		if ((GLOBAL(function_state).loop_change_type != DO_NOTHING) && (GLOBAL(function_state).loop_change_level == GLOBAL(function_state).loop_nest_level)) {
			GLOBAL(function_state).loop_change_type = DO_NOTHING;
			GLOBAL(function_state).loop_change_level = 0;
		}
	}
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	GLOBAL(function_state).loop_nest_level--;
}


inline void cs_start_if (pval *expr INLINE_TLS)
{
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));	/* push current state to stack */

	if (GLOBAL(Execute)) {				/* we're in a code block that needs to be evaluated */
		if (pval_is_true(expr)) {	/* the IF expression is pval_is_true, execute IF code */
			GLOBAL(ExecuteFlag) = EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		} else {				/* the IF expression is false, don't execute IF code */
			GLOBAL(ExecuteFlag) = BEFORE_EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		}
		pval_destructor(expr _INLINE_TLS);
	} else {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
	}
}


inline void cs_end_if ( INLINE_TLS_VOID)
{
	/* restore previous status */
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
}


inline int cs_break_continue(pval *expr, int op_type INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		if (expr) {
			convert_to_long(expr);
			GLOBAL(function_state).loop_change_level = GLOBAL(function_state).loop_nest_level - expr->value.lval + 1;
		} else {
			GLOBAL(function_state).loop_change_level = GLOBAL(function_state).loop_nest_level;
		}
		if (GLOBAL(function_state).loop_change_level <= 0) {
			php3_error(E_ERROR, "Cannot %s from %d loop(s) from nesting level %d",
						(op_type == DO_BREAK ? "break" : "continue"), (expr ? expr->value.lval : 1), GLOBAL(function_state).loop_nest_level);
			if (expr) {	
				pval_destructor(expr _INLINE_TLS);			
			}
			return FAILURE;
		} else if (GLOBAL(function_state).loop_change_level > GLOBAL(function_state).loop_nest_level) {
			php3_error(E_ERROR, "Cannot continue from %d loops", (expr ? expr->value.lval : -1));
			if (expr) {
				pval_destructor(expr _INLINE_TLS);
			}
			return FAILURE;
		}
		GLOBAL(function_state).loop_change_type = op_type;
		GLOBAL(Execute) = 0;
		if (expr) {
			pval_destructor(expr _INLINE_TLS);
		}
	}
	return SUCCESS;
}


inline void cs_elseif_start_evaluate( INLINE_TLS_VOID)
{
	int local_stack_top;

	if (GLOBAL(ExecuteFlag) == EXECUTE) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}	
	local_stack_top = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(ExecuteFlag) == BEFORE_EXECUTE && local_stack_top == EXECUTE) {
		GLOBAL(ExecuteFlag) = EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_elseif_end_evaluate( INLINE_TLS_VOID)
{
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
}


inline void cs_start_elseif (pval *expr INLINE_TLS)
{
	if (GLOBAL(ExecuteFlag) == EXECUTE) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
	if (GLOBAL(ExecuteFlag) == BEFORE_EXECUTE) {
		if (pval_is_true(expr)) {
			GLOBAL(ExecuteFlag) = EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		}
		pval_destructor(expr _INLINE_TLS);
	}
}


inline void cs_start_else( INLINE_TLS_VOID)
{
	if (GLOBAL(ExecuteFlag) == EXECUTE) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
	if (GLOBAL(ExecuteFlag) == BEFORE_EXECUTE) {
		GLOBAL(ExecuteFlag) = EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void get_function_parameter(pval *varname, unsigned char type, pval *default_value INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval tmp,*data;

		if (_php3_hash_index_find(GLOBAL(active_symbol_table), GLOBAL(param_index), (void **) &data) == FAILURE) {
			if (default_value) {
				tmp = *default_value;
			} else {
				php3_error(E_WARNING, "Missing argument %d in call to %s()",GLOBAL(param_index)+1,GLOBAL(function_state).function_name);
				var_uninit(&tmp);
			}
			_php3_hash_update(GLOBAL(active_symbol_table), varname->value.str.val, varname->value.str.len+1, &tmp, sizeof(pval), NULL);
		} else if (!_php3_hash_index_is_pointer(GLOBAL(active_symbol_table), GLOBAL(param_index))) { /* passed by value */
			tmp = *data;
			pval_copy_constructor(&tmp);
			_php3_hash_update(GLOBAL(active_symbol_table), varname->value.str.val, varname->value.str.len+1, &tmp, sizeof(pval), NULL);
			if (default_value) {
				pval_destructor(default_value _INLINE_TLS);
			}
		} else { /* passed by reference */
			_php3_hash_pointer_update(GLOBAL(active_symbol_table), varname->value.str.val, varname->value.str.len+1, data);
			_php3_hash_index_del(GLOBAL(active_symbol_table), GLOBAL(param_index));
			if (default_value) {
				pval_destructor(default_value _INLINE_TLS);
			}
		}
	} else if (!GLOBAL(php3_display_source)) {
		switch(type) {
			case BYREF_NONE:  /* passed by value */
				break;
			case BYREF_FORCE:  /* passed by reference */
			case BYREF_ALLOW:  /* passed by value reference (well, not exactly:) */
				if (!GLOBAL(function_state).func_arg_types) {
					GLOBAL(function_state).func_arg_types = ecalloc(sizeof(unsigned char),(GLOBAL(param_index)+2));
				} else {
					unsigned int i;
					
					GLOBAL(function_state).func_arg_types = erealloc(GLOBAL(function_state).func_arg_types,sizeof(unsigned char)*(GLOBAL(param_index)+2));
					for (i=GLOBAL(function_state).func_arg_types[0]+1; i<GLOBAL(param_index)+2-1; i++) {
						GLOBAL(function_state).func_arg_types[i]=0;
					}
					
				}
				GLOBAL(function_state).func_arg_types[0] = GLOBAL(param_index)+1;
				GLOBAL(function_state).func_arg_types[GLOBAL(param_index)+1]=type;
				break;
		}
	}
	GLOBAL(param_index)++;
}


inline void pass_parameter_by_value(pval *expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		if (GLOBAL(function_state).func_arg_types) {
			unsigned char argument_offset=(unsigned char) _php3_hash_next_free_element(GLOBAL(function_state).function_symbol_table)+1;
			
			if (argument_offset<=GLOBAL(function_state).func_arg_types[0]
				&& GLOBAL(function_state).func_arg_types[argument_offset]==BYREF_FORCE) {
				php3_error(E_WARNING,"Cannot pass expression as argument %d by reference",argument_offset);
				GLOBAL(function_state).function_type = 0;	/* don't execute the function call */
			}
		}
		
		if (_php3_hash_next_index_insert(GLOBAL(function_state).function_symbol_table, expr, sizeof(pval),NULL) == FAILURE) {
			php3_error(E_WARNING, "Error updating symbol table");
			pval_destructor(expr _INLINE_TLS);
			GLOBAL(function_state).function_type = 0;	/* don't execute the function call */
		}
	}
}


inline void start_function_decleration(INLINE_TLS_VOID)
{
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	GLOBAL(ExecuteFlag) = DONT_EXECUTE;
	GLOBAL(Execute) = SHOULD_EXECUTE;
	GLOBAL(function_state).func_arg_types = NULL;
}


inline void end_function_decleration(pval *function_token, pval *function_name INLINE_TLS)
{
	HashTable *target_symbol_table;
	
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	php3i_stack_del_top(&GLOBAL(css));
	
	if (GLOBAL(Execute)) {
		php3_str_tolower(function_name->value.str.val, function_name->value.str.len);
		if (GLOBAL(class_name)) {
			target_symbol_table = GLOBAL(class_symbol_table);
			_php3_hash_del(GLOBAL(class_symbol_table), function_name->value.str.val, function_name->value.str.len+1);	/* for inheritance */
		} else {
			target_symbol_table = &GLOBAL(function_table);
		}
		if (_php3_hash_exists(target_symbol_table, function_name->value.str.val, function_name->value.str.len+1)) {
			php3_error(E_ERROR, "Can't redeclare already declared function");
			return;
		}
		function_token->type = IS_USER_FUNCTION;
		function_token->value.func.addr.statics = NULL;
		function_token->value.func.arg_types = GLOBAL(function_state).func_arg_types;
		_php3_hash_update(target_symbol_table, function_name->value.str.val, function_name->value.str.len+1, function_token, sizeof(pval), NULL);
	} else {
		if (GLOBAL(function_state).func_arg_types) {
			efree(GLOBAL(function_state).func_arg_types);
		}
	}
	GLOBAL(function_state).func_arg_types = NULL;
}


/* don't execute expr1 except for the first iteration */
inline void for_pre_expr1(pval *for_token INLINE_TLS)
{
	GLOBAL(function_state).loop_nest_level++;
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(Execute)) {
		tc_set_token(&GLOBAL(token_cache_manager), for_token->offset, FOR);
		if (php3i_stack_int_top(&GLOBAL(for_stack)) == (long) for_token->offset) {	/* 2nd or later iteration */
			GLOBAL(ExecuteFlag) = DONT_EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		}
	}
}


/* expr2 is the increment if we're switched.  don't execute it in the first
 * iteration, and execute it otherwise.
 * if we're not switched, it's the truth value, and we want to evaluate it anyway.
 */
inline void for_pre_expr2(pval *for_token INLINE_TLS)
{	
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (for_token->cs_data.switched && php3i_stack_int_top(&GLOBAL(for_stack)) != (long) for_token->offset) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;	/* first iteration of already switched for */
	}
}


/* if we're not switched, expr3 is the increment.  we shouldn't execute it
 * in the first iteration.  since not being switched --> first iteration,
 * we don't have to check the for_stack.
 */
inline void for_pre_expr3(pval *for_token, pval *expr2 INLINE_TLS)
{
	if (for_token->cs_data.switched && php3i_stack_int_top(&GLOBAL(for_stack)) != (long) for_token->offset) {
		var_reset(expr2);
	}
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute) && !for_token->cs_data.switched) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void for_pre_statement(pval *for_token, pval *expr2, pval *expr3 INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute) && !for_token->cs_data.switched) {
		var_reset(expr3);
	}
	if (GLOBAL(Execute) && for_token->cs_data.switched) {
		if (pval_is_true(expr3)) {
			GLOBAL(ExecuteFlag) = EXECUTE;
		} else {
			GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		}
		GLOBAL(Execute) = SHOULD_EXECUTE;
		pval_destructor(expr2 _INLINE_TLS);
		pval_destructor(expr3 _INLINE_TLS);
	} else if (GLOBAL(Execute) && !for_token->cs_data.switched) {
		if (pval_is_true(expr2)) {
			GLOBAL(ExecuteFlag) = EXECUTE;
		} else {
			GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		}
		GLOBAL(Execute) = SHOULD_EXECUTE;
		pval_destructor(expr2 _INLINE_TLS);
		pval_destructor(expr3 _INLINE_TLS);
	}
}

inline void for_post_statement(pval *for_token, pval *first_semicolon, pval *second_semicolon, pval *close_parentheses, int *yychar INLINE_TLS)
{
	if (php3i_stack_int_top(&GLOBAL(for_stack)) != (long) for_token->offset) {	/* first iteration */
		php3i_stack_push(&GLOBAL(for_stack), &for_token->offset, sizeof(int));
	}
	if (!for_token->cs_data.switched) {
		tc_switch(&GLOBAL(token_cache_manager), first_semicolon->offset + 1, close_parentheses->offset - 1, second_semicolon->offset);
		tc_set_switched(&GLOBAL(token_cache_manager), for_token->offset);
	}
	if (GLOBAL(Execute)) {
		tc_set_token(&GLOBAL(token_cache_manager), for_token->offset, CONTINUED_FOR);
		seek_token(&GLOBAL(token_cache_manager), for_token->offset, yychar);
	} else {
		if ((GLOBAL(function_state).loop_change_type != DO_NOTHING) && (GLOBAL(function_state).loop_change_level == GLOBAL(function_state).loop_nest_level)) {
			if (GLOBAL(function_state).loop_change_type == DO_CONTINUE) {
				tc_set_token(&GLOBAL(token_cache_manager), for_token->offset, CONTINUED_FOR);
				seek_token(&GLOBAL(token_cache_manager), for_token->offset, yychar);
			} else {
				if (php3i_stack_int_top(&GLOBAL(for_stack)) == (long) for_token->offset) {
#if (_DEBUG_ & CONTROL_DEBUG)
					fprintf(stderr, "Deleting for addr=%d from for stack", for_token->offset);
#endif
					php3i_stack_del_top(&GLOBAL(for_stack));
				}
			}
			GLOBAL(function_state).loop_change_type = DO_NOTHING;
			GLOBAL(function_state).loop_change_level = 0;
		} else {
			if (php3i_stack_int_top(&GLOBAL(for_stack)) == (long) for_token->offset) {
#if (_DEBUG_ & CONTROL_DEBUG)
				fprintf(stderr, "Deleting for addr=%d from for stack", for_token->offset);
#endif
				php3i_stack_del_top(&GLOBAL(for_stack));
			}
		}
	}
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;

	GLOBAL(function_state).loop_nest_level--;
}


inline void cs_return(pval *expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		if (GLOBAL(function_state).function_name) {
			if (expr) {
				GLOBAL(return_value) = *expr;
			} else {
				var_reset(&GLOBAL(return_value));
			}
			GLOBAL(function_state).returned = 1;
			GLOBAL(ExecuteFlag) = DONT_EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		} else {
			int retval;
			
			if (end_current_file_execution(&retval) && retval==0) {
				/* the scanner return 0, terminate execution completely (behave like exit()) */
				php3_header();
				GLOBAL(shutdown_requested) = ABNORMAL_SHUTDOWN;
			}
			if (expr) {
				php3i_print_variable(expr);
				pval_destructor(expr);
			}
		}
	}
}


inline void php3cs_start_require(pval *include_token INLINE_TLS)
{
	/* evaluate the expression even at Execute=0, if it's the first time we see this include(), otherwise,
	 * don't evaluate it.
	 */
	if (GLOBAL(php3_display_source)) {
		return;
	}

	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	GLOBAL(php3g_function_state_for_require) = GLOBAL(function_state);

	if (!include_token->cs_data.included) {
		GLOBAL(function_state).loop_nest_level = GLOBAL(function_state).loop_change_level = GLOBAL(function_state).loop_change_type = 0;
		GLOBAL(function_state).returned = 0;
		GLOBAL(ExecuteFlag) = EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	} else {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void php3cs_end_require(pval *include_token, pval *expr INLINE_TLS)
{
	if (GLOBAL(php3_display_source)) {
		return;
	}
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(function_state) = GLOBAL(php3g_function_state_for_require);
	GLOBAL(Execute) = SHOULD_EXECUTE;
	
	if (!include_token->cs_data.included) {
		if (!GLOBAL(php3_display_source)) {
			include_file(expr,0);
		}
		tc_set_included(&GLOBAL(token_cache_manager), include_token->offset);
		pval_destructor(expr _INLINE_TLS);
	}
}



inline void start_display_source(int start_in_php INLINE_TLS)
{
	php3_header();
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	GLOBAL(ExecuteFlag) = DONT_EXECUTE;
	GLOBAL(Execute) = 0;
	GLOBAL(php3_display_source)=1;
	php3_printf("<FONT color=%s>", (start_in_php?php3_ini.highlight_default:php3_ini.highlight_html));
}


inline void cs_show_source(pval *expr INLINE_TLS)
{
	if (include_file(expr,1)==SUCCESS) {
		start_display_source(0 _INLINE_TLS);
	}
	pval_destructor(expr _INLINE_TLS);
}


inline void cs_pre_boolean_or(pval *left_expr INLINE_TLS)
{
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(Execute) && pval_is_true(left_expr)) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;;
	}
}


inline void cs_post_boolean_or(pval *result, pval *left_expr, pval *right_expr INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute)) {
		boolean_or_function(result, left_expr, right_expr);
	}
}


inline void cs_pre_boolean_and(pval *left_expr INLINE_TLS)
{
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(Execute) && !pval_is_true(left_expr)) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_post_boolean_and(pval *result, pval *left_expr, pval *right_expr INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute)) {
		boolean_and_function(result, left_expr, right_expr);
	}
}


inline void cs_questionmark_op_pre_expr1(pval *truth_value INLINE_TLS)
{
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(Execute) && !pval_is_true(truth_value)) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_questionmark_op_pre_expr2(pval *truth_value INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute) && pval_is_true(truth_value)) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_questionmark_op_post_expr2(pval *result, pval *truth_value, pval *expr1, pval *expr2 INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute)) {
		if (pval_is_true(truth_value)) {
			*result = *expr1;
		} else {
			*result = *expr2;
		}
		pval_destructor(truth_value _INLINE_TLS);
	}
}


inline void cs_functioncall_pre_variable_passing(pval *function_name, pval *class_ptr, unsigned char free_function_name INLINE_TLS)
{
	int minus_one = -1;
	HashTable *target_symbol_table;	/* the symbol table in which the function would be searched */
	pval *object=NULL;

	target_symbol_table = &GLOBAL(function_table);
	if (GLOBAL(Execute)) {
		pval *data;
		
		if (class_ptr) {	/* use member function rather than global function */
			object = (pval *) class_ptr->value.varptr.pvalue;
			
			if (!object) {
				if (free_function_name) {
					pval_destructor(function_name _INLINE_TLS);
				}
				php3_error(E_ERROR, "Member function used on a non-object");
				return;
			}
			target_symbol_table = object->value.ht;
		}
		if (function_name->type != IS_STRING) {
			php3_error(E_ERROR, "Function names must be strings");
			if (free_function_name) {
				pval_destructor(function_name _INLINE_TLS);
			}
			return;
		}
		php3_str_tolower(function_name->value.str.val, function_name->value.str.len);
		if (_php3_hash_find(target_symbol_table, function_name->value.str.val, function_name->value.str.len+1, (void **) &data) == SUCCESS) {
			if (!(data->type & VALID_FUNCTION)) {
				php3_error(E_ERROR, "Function call to a non-function (%s)", function_name->value.str.val);
				function_name->cs_data.function_call_type=0;
				GLOBAL(function_state).symbol_table = NULL;
				return;
			}
			/* we're gonna call the function... */
			php3i_stack_push(&GLOBAL(for_stack), &minus_one, sizeof(int));
			php3i_stack_push(&GLOBAL(function_state_stack), &GLOBAL(function_state), sizeof(FunctionState));	/* save function state */
			function_name->cs_data.function_call_type = data->type;
			function_name->offset = data->offset;
			GLOBAL(function_state).function_symbol_table = (HashTable *) emalloc(sizeof(HashTable));
			GLOBAL(function_state).function_name = function_name->value.str.val;
			GLOBAL(function_state).function_type = data->type;
			GLOBAL(function_state).handler = (void (*)(INTERNAL_FUNCTION_PARAMETERS)) data->value.func.addr.internal;
			GLOBAL(function_state).func_arg_types = data->value.func.arg_types;
			GLOBAL(function_state).lineno = GLOBAL(current_lineno);
			GLOBAL(function_state).hosting_function_table = target_symbol_table;
			if (!GLOBAL(function_state).function_symbol_table) {
				php3_error(E_ERROR, "Unable to allocate necessary memory for function call");
				function_name->cs_data.function_call_type=0;
				GLOBAL(function_state).symbol_table = NULL;
				return;
			}
			if (_php3_hash_init(GLOBAL(function_state).function_symbol_table, 0, NULL, PVAL_DESTRUCTOR, 0) == FAILURE) {
				php3_error(E_ERROR, "Unable to initialize new symbol table in function call");
				function_name->cs_data.function_call_type=0;
				GLOBAL(function_state).symbol_table = NULL;
				return;
			}
			/* push GLOBAL */
			GLOBAL(globals).type = IS_ARRAY;
			GLOBAL(globals).value.ht = &GLOBAL(symbol_table);
			if (data->type==IS_USER_FUNCTION) {
				_php3_hash_pointer_update(GLOBAL(function_state).function_symbol_table, "GLOBALS", sizeof("GLOBALS"), (void *) &GLOBAL(globals));
			}
			if (object) {	/* push $this */
				GLOBAL(function_state).object_pointer = (pval *) emalloc(sizeof(pval));
				*(GLOBAL(function_state).object_pointer) = *object;
				_php3_hash_pointer_update(GLOBAL(function_state).function_symbol_table, "this", sizeof("this"), (void *) GLOBAL(function_state).object_pointer);
			} else {
				GLOBAL(function_state).object_pointer = NULL;
			}
		} else {
			php3_error(E_ERROR, "Call to unsupported or undefined function %s()", function_name->value.str.val);
			function_name->cs_data.function_call_type=0;
			GLOBAL(function_state).symbol_table = NULL;
			return;
		}
	} else {
		function_name->cs_data.function_call_type = 0;
	}
}


inline void cs_functioncall_post_variable_passing(pval *function_name, int *yychar INLINE_TLS)
{
	if (function_name->cs_data.function_call_type) {
		php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));


		/* prepare a new function state */
		GLOBAL(function_state).symbol_table = GLOBAL(function_state).function_symbol_table;
		GLOBAL(function_state).function_symbol_table = NULL;
		GLOBAL(function_state).loop_nest_level = GLOBAL(function_state).loop_change_level = GLOBAL(function_state).loop_change_type = 0;

		var_reset(&GLOBAL(return_value)); 
		switch (GLOBAL(function_state).function_type) {
			case IS_USER_FUNCTION:
				GLOBAL(active_symbol_table) = GLOBAL(function_state).symbol_table;
				seek_token(&GLOBAL(token_cache_manager), function_name->offset, yychar);
				break;
			case IS_INTERNAL_FUNCTION:
				GLOBAL(function_state).handler(GLOBAL(function_state).symbol_table,&GLOBAL(return_value),&GLOBAL(list),&GLOBAL(plist));
				break;
			default:			/* don't execute the function call */
				break;
		}
	}
}


inline void cs_functioncall_end(pval *result, pval *function_name, pval *close_parentheses, int *yychar, unsigned char free_function_name INLINE_TLS)
{
	if (function_name->cs_data.function_call_type) {
		FunctionState *fs_ptr;

		/* handle return value for user functions (internal functions do that themselves) */
		*result = GLOBAL(return_value);
		if (GLOBAL(function_state).function_type==IS_USER_FUNCTION && !GLOBAL(function_state).returned) {
			var_reset(result);  /* default return value */
		}
		
		/* clean up */
		if (GLOBAL(function_state).symbol_table) {
			_php3_hash_destroy(GLOBAL(function_state).symbol_table);
			efree(GLOBAL(function_state).symbol_table);
		}
		if (GLOBAL(function_state).object_pointer) {
			efree(GLOBAL(function_state).object_pointer);
		}
		if (free_function_name) {
			pval_destructor(function_name _INLINE_TLS);
		}
		
		while (php3i_stack_int_top(&GLOBAL(for_stack)) != -1) {  /* pop FOR stack */
			php3i_stack_del_top(&GLOBAL(for_stack));
		}
		php3i_stack_del_top(&GLOBAL(for_stack));
		
		/* jump back */
		if (GLOBAL(function_state).function_type == IS_USER_FUNCTION) {
			seek_token(&GLOBAL(token_cache_manager), close_parentheses->offset + 1, yychar);
		}
		
		/* get previous function state */
		php3i_stack_top(&GLOBAL(function_state_stack), (void **) &fs_ptr);
		GLOBAL(function_state) = *fs_ptr;
		php3i_stack_del_top(&GLOBAL(function_state_stack));
		GLOBAL(active_symbol_table) = GLOBAL(function_state).symbol_table;

		/* restore execution state */
		GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
		php3i_stack_del_top(&GLOBAL(css));
		GLOBAL(Execute) = SHOULD_EXECUTE;
	} else {
		var_reset(result);
	}
}


inline void cs_switch_start(pval *switch_token, pval *expr INLINE_TLS)
{
	switch_expr se;

	GLOBAL(function_state).loop_nest_level++;
	php3i_stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));

	se.expr = *expr;
	se.offset = switch_token->offset;
	se.Execute = GLOBAL(Execute);
	php3i_stack_push(&GLOBAL(switch_stack), &se, sizeof(switch_expr));
}


/* if case_expr is NULL, assume default */
inline void cs_switch_case_pre(pval *case_expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		switch_expr *se;
		pval expr, result;
		int is_equal = 0;

		php3i_stack_top(&GLOBAL(switch_stack), (void **) &se);
		if (se->offset != -1) {	/* no matched case yet */
			/* if case_expr is NULL, we're in the special 'default' case */
			if (case_expr) {
				expr = se->expr;
				pval_copy_constructor(&expr);
				is_equal_function(&result, &expr, case_expr _INLINE_TLS);
				is_equal = pval_is_true(&result);
			}
			if (!case_expr || is_equal) {
				se->offset=-1;
				GLOBAL(ExecuteFlag) = EXECUTE;
				GLOBAL(Execute) = SHOULD_EXECUTE;
			} else {
				GLOBAL(ExecuteFlag) = DONT_EXECUTE;
				GLOBAL(Execute) = SHOULD_EXECUTE;
			}
		} else {
			if (case_expr) {
				pval_destructor(case_expr _INLINE_TLS);
			}
		}
	}
}

inline void cs_switch_case_post( INLINE_TLS_VOID)
{
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
}


inline void cs_switch_end(pval *expr INLINE_TLS)
{
	switch_expr *se;
	
	if (!GLOBAL(Execute)) {
		if (GLOBAL(function_state).loop_change_level == GLOBAL(function_state).loop_nest_level) {
			GLOBAL(function_state).loop_change_type = DO_NOTHING;
		}
	}
	GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
	php3i_stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	
	php3i_stack_top(&GLOBAL(switch_stack), (void **) &se);
	if (se->Execute) {
		pval_destructor(expr _INLINE_TLS);
	}
	php3i_stack_del_top(&GLOBAL(switch_stack));
	GLOBAL(function_state).loop_nest_level--;
}


inline void cs_start_class_decleration(pval *classname, pval *parent INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval new_class, *parent_ptr;

		if (_php3_hash_exists(&GLOBAL(function_table), classname->value.str.val, classname->value.str.len+1)) {
			php3_error(E_ERROR,"%s is already a function or class",classname->value.str.val);
		}
		if (parent) { /* inheritance */
			pval *ctor_ptr, ctor;
			
			if (_php3_hash_find(&GLOBAL(function_table), parent->value.str.val, parent->value.str.len+1, (void **) &parent_ptr) == FAILURE) {
				php3_error(E_ERROR, "Cannot extend non existant class %s", parent->value.str.val);
				return;
			}
			new_class = *parent_ptr;
			pval_copy_constructor(&new_class);
			
			if (_php3_hash_find(new_class.value.ht, parent->value.str.val, parent->value.str.len+1, (void **) &ctor_ptr)==SUCCESS) {
				ctor = *ctor_ptr;
				pval_copy_constructor(&ctor);
				_php3_hash_update(new_class.value.ht, classname->value.str.val, classname->value.str.len+1, &ctor, sizeof(pval), NULL);
			}
				
		} else {
			new_class.type = IS_CLASS;
			new_class.value.ht = (HashTable *) emalloc(sizeof(HashTable));
			_php3_hash_init(new_class.value.ht, 0, NULL, PVAL_DESTRUCTOR, 0);
		}
		if (_php3_hash_update(&GLOBAL(function_table), classname->value.str.val, classname->value.str.len+1, &new_class, sizeof(pval),NULL) == FAILURE) {
			php3_error(E_ERROR, "Unable to initialize new class");
		}
		GLOBAL(class_name) = classname->value.str.val;
		GLOBAL(class_symbol_table) = new_class.value.ht;
	}
}


inline void cs_end_class_decleration( INLINE_TLS_VOID)
{
	if (GLOBAL(Execute)) {
		GLOBAL(class_name) = NULL;
		GLOBAL(class_symbol_table) = NULL;
	}
}


inline void start_array_parsing(pval *array_name,pval *class_ptr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		HashTable *target_symbol_table=GLOBAL(active_symbol_table);
	
		if ((array_name->type != IS_STRING)) {
			php3_error(E_WARNING,"Illegal array name");
			GLOBAL(array_ptr) = NULL;
		} else {
			if (class_ptr) {
				pval *object=(pval *) class_ptr->value.varptr.pvalue;
				
				if (!object) {
					GLOBAL(array_ptr)=NULL;
					pval_destructor(array_name _INLINE_TLS);
					return;
				}
				target_symbol_table = object->value.ht;
			}
			if (_php3_hash_find(target_symbol_table,array_name->value.str.val, array_name->value.str.len+1, (void **)&GLOBAL(array_ptr)) == FAILURE) {
				pval tmp;
				variable_tracker vt;

				array_init(&tmp);
				_php3_hash_update(target_symbol_table,array_name->value.str.val,array_name->value.str.len+1,(void *) &tmp, sizeof(pval),(void **) &GLOBAL(array_ptr));
				GLOBAL(array_ptr)->cs_data.array_write=1;
								
				vt.type = IS_STRING;
				vt.strlen = array_name->value.str.len;
				vt.strval = estrndup(array_name->value.str.val,vt.strlen);
				vt.ht = target_symbol_table;
				php3i_stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
			} else {
				GLOBAL(array_ptr)->cs_data.array_write=0;
			}
			if (GLOBAL(array_ptr)->type != IS_ARRAY && GLOBAL(array_ptr)->type != IS_STRING) {
	            php3_error(E_WARNING, "Variable $%s is not an array or string", array_name->value.str.val);
				GLOBAL(array_ptr) = NULL;
			}
		}
		pval_destructor(array_name _INLINE_TLS);
	}
}

inline void start_dimensions_parsing(pval *result INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		result->value.varptr.pvalue = GLOBAL(array_ptr);
		if (GLOBAL(array_ptr)) {
			result->cs_data.array_write=GLOBAL(array_ptr)->cs_data.array_write;
		}
		result->value.varptr.string_offset=-1;
	}
}
  
inline void fetch_array_index(pval *result, pval *expr, pval *dimension INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		int original_array_write = dimension->cs_data.array_write;
		
		result->cs_data.array_write = dimension->cs_data.array_write;
			
		if (dimension->value.varptr.pvalue) {
			pval *localdata,tmp,*arr_ptr=dimension->value.varptr.pvalue;
			int new_array=0; /* whether we created the array just now */
	
			if (arr_ptr->type==IS_STRING && arr_ptr->value.str.val!=empty_string && arr_ptr->value.str.val!=undefined_variable_string) {
				if (dimension->value.varptr.string_offset == -1) { /* ok */
					if (!expr) {
						php3_error(E_WARNING,"No string index specified");
						result->value.varptr.pvalue=NULL;
						return;
					}
					convert_to_long(expr);
					if (expr->value.lval<0 || expr->value.lval>=arr_ptr->value.str.len) {
						php3_error(E_WARNING,"Illegal string index");
						result->value.varptr.pvalue=NULL;
						return;
					}
					result->value.varptr.string_offset = expr->value.lval;
					return;
				} else {
					php3_error(E_WARNING,"Cannot index a string index");
					result->value.varptr.pvalue=NULL;
					return;
				}
			}
			if (arr_ptr->type==IS_NULL || (arr_ptr->type==IS_STRING && 
				(arr_ptr->value.str.val==empty_string || arr_ptr->value.str.val==undefined_variable_string))) {
				array_init(arr_ptr);
				new_array=1;
			} else if (arr_ptr->type!=IS_ARRAY) {
				php3_error(E_WARNING,"Index referencing a non-array");
				result->value.varptr.pvalue=NULL;
				if (expr) { 
					pval_destructor(expr _INLINE_TLS);
				}
				return;
			}
			if (expr) {
				switch (expr->type) {
					case IS_LONG:
					case IS_DOUBLE:
						convert_to_long(expr);
						if (new_array || _php3_hash_index_find(arr_ptr->value.ht, expr->value.lval, (void **) &localdata) == FAILURE) {
							tmp.type = IS_NULL;
							_php3_hash_index_update(arr_ptr->value.ht, expr->value.lval, (void *)&tmp, sizeof(pval),(void **) &localdata);
							result->value.varptr.pvalue = localdata;
							result->cs_data.array_write = 1;
							if (!original_array_write) { /* we raised the write flag just now, record this variable */
								variable_tracker vt;
								
								vt.type = IS_LONG;
								vt.lval = expr->value.lval;
								vt.ht = arr_ptr->value.ht;
								php3i_stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
							}
						} else {
							result->value.varptr.pvalue = localdata;
						}
						break;
					case IS_STRING:
						if (new_array || _php3_hash_find(arr_ptr->value.ht, expr->value.str.val, expr->value.str.len+1, (void **) &localdata)==FAILURE) {
							tmp.type = IS_NULL;
							if (_php3_hash_update(arr_ptr->value.ht, expr->value.str.val, expr->value.str.len+1, (void *) &tmp, sizeof(pval),(void **) &localdata)==FAILURE) {
								result->value.varptr.pvalue=NULL;
								STR_FREE(expr->value.str.val);
								return;
							}
							result->value.varptr.pvalue = localdata;
							result->cs_data.array_write = 1;
							if (!original_array_write) { /* we raised the write flag just now, record this variable */
								variable_tracker vt;
								
								vt.type = IS_STRING;
								vt.strval = estrndup(expr->value.str.val,expr->value.str.len);
								vt.strlen = expr->value.str.len;
								vt.ht = arr_ptr->value.ht;
								php3i_stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
							}
						} else {
							result->value.varptr.pvalue = localdata;
						}
						break;
					default:
						result->value.varptr.pvalue = NULL;
						php3_error(E_WARNING, "Illegal index type");
						break;
				}
			} else { /* assign-next */
				int new_index=_php3_hash_next_free_element(arr_ptr->value.ht);
				
				tmp.type = IS_NULL;
				_php3_hash_next_index_insert(arr_ptr->value.ht, (void *) &tmp, sizeof(pval), (void **) &localdata);
				result->value.varptr.pvalue = localdata;
				result->cs_data.array_write=1;
				if (!original_array_write) { /* we raised the write flag just now, record this variable */
					variable_tracker vt;
								
					vt.type = IS_LONG;
					vt.lval = new_index;
					vt.ht = arr_ptr->value.ht;
					php3i_stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
				}
			}
		} else {
			result->value.varptr.pvalue=NULL;
		}
		if (expr) {
			pval_destructor(expr _INLINE_TLS);
		}
	}
}	


inline void end_array_parsing(pval *result, pval *dimensions INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval *ptr=(pval *) dimensions->value.varptr.pvalue;

		*result = *dimensions;
		if (ptr && ptr->type==IS_NULL) {
			var_reset(ptr);
		}
	}
}


inline void clean_unassigned_variable_top(int delete_var INLINE_TLS)
{
	variable_tracker *vt;
	
	if (php3i_stack_top(&GLOBAL(variable_unassign_stack),(void **) &vt)==SUCCESS) {
		switch(vt->type) {
			case IS_LONG:
				if (delete_var) {
					_php3_hash_index_del(vt->ht,vt->lval);
				}
				break;
			case IS_STRING:
				if (delete_var) {
					_php3_hash_del(vt->ht,vt->strval,vt->strlen+1);
				}
				STR_FREE(vt->strval);
				break;
		}
	}
	php3i_stack_del_top(&GLOBAL(variable_unassign_stack));
}


inline void get_class_variable_pointer(pval *result, pval *class_ptr, pval *varname INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval *object = (pval *) class_ptr->value.varptr.pvalue;
		
		if (!object) {
			result->value.varptr.pvalue = NULL;
			pval_destructor(varname _INLINE_TLS);
			return;
		}
		if ((varname->type != IS_STRING)) {
			php3_error(E_WARNING,"Illegal property name");
			result->value.varptr.pvalue = NULL;
			pval_destructor(varname _INLINE_TLS);
			return;
		} else {
			pval *data;

			if (_php3_hash_find(object->value.ht,varname->value.str.val, varname->value.str.len+1, (void **)&data) == FAILURE) {
				pval tmp;
				variable_tracker vt;

				var_reset(&tmp);
				_php3_hash_update(object->value.ht,varname->value.str.val,varname->value.str.len+1,(void *) &tmp, sizeof(pval),(void **) &data);
				
				vt.type = IS_STRING;
				vt.strlen = varname->value.str.len;
				vt.strval = estrndup(varname->value.str.val,vt.strlen);
				vt.ht = object->value.ht;
				php3i_stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
				result->cs_data.array_write = 1;
			} else {
				result->cs_data.array_write = 0;
			}
			result->value.varptr.pvalue = data;
			result->value.str.len = -1; /* Not indexed string */
			pval_destructor(varname _INLINE_TLS);
		}
	}
}


inline void pass_parameter(pval *var, int by_reference INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval tmp;
		/* ... check if we need to force by reference (set by_reference=1) */

		if (GLOBAL(function_state).func_arg_types) {
			unsigned char argument_offset=(unsigned char) _php3_hash_next_free_element(GLOBAL(function_state).function_symbol_table)+1;
			
			if (argument_offset<=GLOBAL(function_state).func_arg_types[0]
				&& GLOBAL(function_state).func_arg_types[argument_offset]!=BYREF_NONE) {
				by_reference=1;
			}
		}
				
		if (!by_reference) {
			read_pointer_value(&tmp,var _INLINE_TLS);
			if (_php3_hash_next_index_insert(GLOBAL(function_state).function_symbol_table, &tmp, sizeof(pval),NULL) == FAILURE) {
				php3_error(E_WARNING, "Error updating symbol table");
				pval_destructor(&tmp _INLINE_TLS);
				GLOBAL(function_state).function_type = 0;	/* don't execute the function call */
				return;
			}
		} else {
			if (var->cs_data.array_write) {
				clean_unassigned_variable_top(0 _INLINE_TLS);
			}
			if (var->value.varptr.pvalue==NULL) {
				GLOBAL(function_state).function_type = 0;
				return;
			}
			if (_php3_hash_next_index_pointer_insert(GLOBAL(function_state).function_symbol_table, (void *) var->value.varptr.pvalue) == FAILURE) {
				php3_error(E_WARNING, "Error updating symbol table");
				GLOBAL(function_state).function_type = 0;	/* don't execute the function call */
				return;
			}
		}
	}
}


inline void cs_system(pval *result,pval *expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		FILE *in;
		int readbytes,total_readbytes=0,allocated_space;
		
		if (php3_ini.safe_mode) {
			php3_error(E_WARNING,"Cannot execute using backquotes in safe mode");
			pval_destructor(expr _INLINE_TLS);
			var_reset(result);
			return;
		}

		convert_to_string(expr);
#if WIN32|WINNT
		(void)AllocConsole();
		if ((in=popen(expr->value.str.val,"rt"))==NULL) {
#else
		if ((in=popen(expr->value.str.val,"r"))==NULL) {
#endif
			php3_error(E_WARNING,"Unable to execute '%s'",expr->value.str.val);
		}
		allocated_space = EXEC_INPUT_BUF;
		result->value.str.val = (char *) emalloc(allocated_space);
		while (1) {
			readbytes = fread(result->value.str.val+total_readbytes,1,EXEC_INPUT_BUF,in);
			if (readbytes<=0) {
				break;
			}
			total_readbytes += readbytes;
			allocated_space = total_readbytes+EXEC_INPUT_BUF;
			result->value.str.val = (char *) erealloc(result->value.str.val,allocated_space);
		}
		pclose(in);
		
		result->value.str.val = erealloc(result->value.str.val,total_readbytes+1);
		result->value.str.val[total_readbytes]=0;
		result->value.str.len = total_readbytes;
		result->type = IS_STRING;
		STR_FREE(expr->value.str.val);
	}
}


inline void add_regular_encapsed_variable(pval *result,pval *encaps,pval *varname  INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval tmp;
		
		get_regular_variable_contents(&tmp,varname,0 _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
	}
}


inline void add_assoc_array_encapsed_variable(pval *result, pval *encaps, pval *varname, pval *index INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval tmp;
				
		get_array_variable(&tmp,varname,index _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
	}
}

inline void add_regular_array_encapsed_variable(pval *result, pval *encaps, pval *varname, pval *index INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval tmp;

		COPY_STRING(*index);
		convert_to_long(index);
		get_array_variable(&tmp,varname,index _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
	}
}


inline void add_variable_array_encapsed_variable(pval *result, pval *encaps, pval *varname, pval *var_index INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval index,tmp;
		
		get_regular_variable_contents(&index,var_index,0 _INLINE_TLS);
		get_array_variable(&tmp,varname,&index _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
		pval_destructor(&index _INLINE_TLS);
	}
}


inline void add_encapsed_object_property(pval *result, pval *encaps, pval *object, pval *property INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval tmp;
				
		get_array_variable(&tmp,object,property _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
	}
}


inline void add_indirect_encapsed_variable(pval *result,pval *encaps,pval *varname INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		pval tmp,tmp2;
		
		get_regular_variable_contents(&tmp,varname,0 _INLINE_TLS);
		
		convert_to_string(&tmp);
		
		get_regular_variable_contents(&tmp2,&tmp,1 _INLINE_TLS);
		concat_function(result,encaps,&tmp2,1 _INLINE_TLS);
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
