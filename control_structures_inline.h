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


/* $Id: control_structures_inline.h,v 1.137 1998/02/01 23:53:05 zeev Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "functions/head.h"
#include <stdio.h>
#include <string.h>

#ifndef THREAD_SAFE
extern YYSTYPE *array_ptr;
static YYSTYPE return_value;
static HashTable list, plist;
#endif

extern inline int cs_global_variable(YYSTYPE *varname INLINE_TLS);
extern inline int cs_static_variable(YYSTYPE *varname, YYSTYPE *value INLINE_TLS);
extern inline void cs_start_while(YYSTYPE *expr INLINE_TLS);
extern inline void cs_end_while(YYSTYPE *while_token INLINE_TLS);
extern inline void cs_start_do_while( INLINE_TLS_VOID);
extern inline void cs_force_eval_do_while( INLINE_TLS_VOID);
extern inline void cs_end_do_while(YYSTYPE *do_token, YYSTYPE *expr INLINE_TLS);
extern inline void cs_start_if(YYSTYPE *expr INLINE_TLS);
extern inline void cs_end_if( INLINE_TLS_VOID);
extern inline int cs_break_continue(YYSTYPE *expr, int op_type INLINE_TLS);
extern inline void cs_elseif_start_evaluate( INLINE_TLS_VOID);
extern inline void cs_elseif_end_evaluate( INLINE_TLS_VOID);
extern inline void cs_start_elseif(YYSTYPE *expr INLINE_TLS);
extern inline void cs_start_else( INLINE_TLS_VOID);
extern inline void get_function_parameter(YYSTYPE *varname INLINE_TLS);
extern inline void pass_parameter_by_value(YYSTYPE *expr INLINE_TLS);
extern inline void start_function_decleration(YYSTYPE *function_token, YYSTYPE *function_name INLINE_TLS);
extern inline void end_function_decleration( INLINE_TLS_VOID);
extern inline void for_pre_expr1(YYSTYPE *for_token INLINE_TLS);
extern inline void for_pre_expr2(YYSTYPE *for_token INLINE_TLS);
extern inline void for_pre_expr3(YYSTYPE *for_token, YYSTYPE *expr2 INLINE_TLS);
extern inline void for_pre_statement(YYSTYPE *for_token, YYSTYPE *expr2, YYSTYPE *expr3 INLINE_TLS);
extern inline void for_post_statement(YYSTYPE *for_token, YYSTYPE *first_semicolon, YYSTYPE *second_semicolon, YYSTYPE *close_parentheses INLINE_TLS);
extern inline void cs_return(YYSTYPE *expr INLINE_TLS);
extern inline void cs_start_include(YYSTYPE *include_token INLINE_TLS);
extern inline void cs_end_include(YYSTYPE *include_token, YYSTYPE *expr INLINE_TLS);
extern inline void cs_show_source(YYSTYPE *expr INLINE_TLS);
extern inline void cs_pre_boolean_or(YYSTYPE *left_expr INLINE_TLS);
extern inline void cs_post_boolean_or(YYSTYPE *result, YYSTYPE *left_expr, YYSTYPE *right_expr INLINE_TLS);
extern inline void cs_pre_boolean_and(YYSTYPE *left_expr INLINE_TLS);
extern inline void cs_post_boolean_and(YYSTYPE *result, YYSTYPE *left_expr, YYSTYPE *right_expr INLINE_TLS);
extern inline void cs_questionmark_op_pre_expr1(YYSTYPE *truth_value INLINE_TLS);
extern inline void cs_questionmark_op_pre_expr2(YYSTYPE *truth_value INLINE_TLS);
extern inline void cs_questionmark_op_post_expr2(YYSTYPE *result, YYSTYPE *truth_value, YYSTYPE *expr1, YYSTYPE *expr2 INLINE_TLS);
extern inline void cs_functioncall_pre_variable_passing(YYSTYPE *function_name, YYSTYPE *class_ptr INLINE_TLS);
extern inline void cs_functioncall_post_variable_passing(YYSTYPE *function_name INLINE_TLS);
extern inline void cs_functioncall_end(YYSTYPE *result, YYSTYPE *function_name, YYSTYPE *close_parentheses INLINE_TLS);
extern inline void cs_switch_start(YYSTYPE *switch_token, YYSTYPE *expr INLINE_TLS);
extern inline void cs_switch_case_pre(YYSTYPE *case_expr INLINE_TLS);
extern inline void cs_switch_case_post( INLINE_TLS_VOID);
extern inline void cs_switch_end(YYSTYPE *expr INLINE_TLS);
extern inline void cs_start_class_decleration(YYSTYPE *classname, YYSTYPE *parent INLINE_TLS);
extern inline void cs_end_class_decleration( INLINE_TLS_VOID);
extern inline void start_array_parsing(YYSTYPE *array_name,YYSTYPE *class_ptr INLINE_TLS);
extern inline void start_dimensions_parsing(YYSTYPE *result INLINE_TLS);
extern inline void fetch_array_index(YYSTYPE *result, YYSTYPE *expr, YYSTYPE *dimension INLINE_TLS);
extern inline void end_array_parsing(YYSTYPE *result, YYSTYPE *dimensions INLINE_TLS);
extern inline void clean_unassigned_variable_top(int delete_var INLINE_TLS);
extern inline void get_class_variable_pointer(YYSTYPE *result, YYSTYPE *class_ptr, YYSTYPE *varname INLINE_TLS);
extern inline void pass_parameter(YYSTYPE *var, int by_reference INLINE_TLS);
extern inline void cs_system(YYSTYPE *result, YYSTYPE *expr INLINE_TLS);

inline int cs_global_variable(YYSTYPE *varname INLINE_TLS)
{
	YYSTYPE *localdata;

	if (GLOBAL(Execute)) {
		if (!GLOBAL(function_state).function_name) {
			php3_error(E_WARNING, "GLOBAL variable decleration meaningless in main() scope");
			return FAILURE;
		}
		if ((varname->type != IS_STRING)) {
			yystype_destructor(varname _INLINE_TLS);
			php3_error(E_WARNING, "Incorrect variable type or name in global in function %s()", GLOBAL(function_state).function_name);
			return FAILURE;
		}
		if (hash_find(GLOBAL(active_symbol_table), varname->value.strval, varname->strlen+1, (void **) &localdata) == SUCCESS) {
			php3_error(E_WARNING, "Variable used in global statement already exists in the function");
			STR_FREE(varname->value.strval);
			return FAILURE;
		}
		if (hash_find(&GLOBAL(symbol_table), varname->value.strval, varname->strlen+1, (void **) &localdata) == FAILURE) {
			YYSTYPE tmp;
		
			tmp.type = IS_STRING;
			tmp.value.strval = undefined_variable_string;	
			tmp.strlen = 0;
			
			if (hash_update(&GLOBAL(symbol_table), varname->value.strval, varname->strlen+1, (void *) &tmp, sizeof(YYSTYPE), (void **) &localdata)==FAILURE) {
				php3_error(E_WARNING, "Unable to initialize global variable $%s",varname->value.strval);
				STR_FREE(varname->value.strval);
				return FAILURE;
			}
		}
		if (hash_pointer_update(GLOBAL(active_symbol_table), varname->value.strval, varname->strlen+1, (void *) localdata) == FAILURE) {
			php3_error(E_WARNING, "Error updating symbol table");
			STR_FREE(varname->value.strval);
			return FAILURE;
		}
		STR_FREE(varname->value.strval);
	}
	return SUCCESS;
}

inline int cs_static_variable(YYSTYPE *varname, YYSTYPE *value INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE *func_ent, *variable_entry, tmp;

		if (!GLOBAL(function_state).function_name) {
			php3_error(E_WARNING, "STATIC variable decleration meaningless in main() scope");
			STR_FREE(varname->value.strval);
			if (value) {
				yystype_destructor(value _INLINE_TLS);
			}
			return FAILURE;
		}
		if ((varname->type != IS_STRING)) {
			yystype_destructor(varname _INLINE_TLS);
			yystype_destructor(value _INLINE_TLS);
			php3_error(E_WARNING, "Incorrect variable type or name in static in function %s()", GLOBAL(function_state).function_name);
			return FAILURE;
		}
		if (hash_find(&GLOBAL(function_table), GLOBAL(function_state).function_name, strlen(GLOBAL(function_state).function_name)+1, (void **) &func_ent) == FAILURE) {
			STR_FREE(varname->value.strval);
			if (value) {
				yystype_destructor(value _INLINE_TLS);
			}
			return FAILURE;
		}
		if (!func_ent->value.ht) {
 			func_ent->value.ht = (HashTable *) emalloc(sizeof(HashTable));
			hash_init(func_ent->value.ht, 0, NULL, YYSTYPE_DESTRUCTOR, 0);
		}
		if (hash_find(func_ent->value.ht, varname->value.strval, varname->strlen+1, (void **) &variable_entry) == FAILURE) {
			if (value) {
				hash_update(func_ent->value.ht, varname->value.strval, varname->strlen+1, value, sizeof(YYSTYPE), (void **) &variable_entry);
			} else {
				var_reset(&tmp);
				hash_update(func_ent->value.ht, varname->value.strval, varname->strlen+1, &tmp, sizeof(YYSTYPE), (void **) &variable_entry);
			}
			/*
			if (hash_find(func_ent->value.ht, varname->value.strval, varname->strlen+1, (void **) &variable_entry) == FAILURE) {
				php3_error(E_ERROR, "Inserted static variable got lost");
				STR_FREE(varname->value.strval);
				if (value) {
					yystype_destructor(value);
				}
				return FAILURE;
			}
			*/
		}
		if (hash_pointer_update(GLOBAL(active_symbol_table), varname->value.strval, varname->strlen+1, (void *) variable_entry) == FAILURE) {
			php3_error(E_ERROR, "Unable to initialize static variable");
			STR_FREE(varname->value.strval);
			if (value) {
				yystype_destructor(value _INLINE_TLS);
			}
			return FAILURE;
		} else {
			hash_find(GLOBAL(active_symbol_table), varname->value.strval, varname->strlen+1, (void **) &variable_entry);
		}
		STR_FREE(varname->value.strval);
	}
	return SUCCESS;
}

inline void cs_start_while(YYSTYPE *expr INLINE_TLS)
{
	GLOBAL(function_state).loop_nest_level++;
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));

	if (GLOBAL(Execute)) {
		if (yystype_true(expr)) {
			GLOBAL(ExecuteFlag) = EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		} else {
			GLOBAL(ExecuteFlag) = DONT_EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		}
		yystype_destructor(expr _INLINE_TLS);
	}
}

inline void cs_end_while(YYSTYPE *while_token INLINE_TLS)
{
#if (_DEBUG_ & CONTROL_DEBUG)
	fprintf(stderr, "end of while, execute=%d, changecount=%d, changetype=%d\n", Execute, GLOBAL(function_state).loop_change_level, GLOBAL(function_state).loop_change_level);
#endif

	if (GLOBAL(Execute)) {
 		seek_token(&GLOBAL(token_cache_manager), while_token->offset);
	} else {
		if ((GLOBAL(function_state).loop_change_type != DO_NOTHING) && (GLOBAL(function_state).loop_change_level == GLOBAL(function_state).loop_nest_level)) {
			if (GLOBAL(function_state).loop_change_type == DO_CONTINUE) {
				seek_token(&GLOBAL(token_cache_manager), while_token->offset);
			}
			GLOBAL(function_state).loop_change_type = DO_NOTHING;
			GLOBAL(function_state).loop_change_level = 0;
		}
	}
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	GLOBAL(function_state).loop_nest_level--;
}


inline void cs_start_do_while( INLINE_TLS_VOID)
{
	GLOBAL(function_state).loop_nest_level++;
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
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


inline void cs_end_do_while(YYSTYPE *do_token, YYSTYPE *expr INLINE_TLS)
{
#if (_DEBUG_ & CONTROL_DEBUG)
	fprintf(stderr, "end of while, execute=%d, changecount=%d, changetype=%d\n", Execute, GLOBAL(function_state).loop_change_level, GLOBAL(function_state).loop_change_level);
#endif

	if (GLOBAL(Execute) && yystype_true(expr)) {
		yystype_destructor(expr _INLINE_TLS);
		seek_token(&GLOBAL(token_cache_manager), do_token->offset);
	} else {
		if (GLOBAL(Execute)) {
			yystype_destructor(expr _INLINE_TLS);
		}
		if ((GLOBAL(function_state).loop_change_type != DO_NOTHING) && (GLOBAL(function_state).loop_change_level == GLOBAL(function_state).loop_nest_level)) {
			GLOBAL(function_state).loop_change_type = DO_NOTHING;
			GLOBAL(function_state).loop_change_level = 0;
		}
	}
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	GLOBAL(function_state).loop_nest_level--;
}


inline void cs_start_if(YYSTYPE *expr INLINE_TLS)
{
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));	/* push current state to stack */

	if (GLOBAL(Execute)) {				/* we're in a code block that needs to be evaluated */
		if (yystype_true(expr)) {	/* the IF expression is yystype_true, execute IF code */
			GLOBAL(ExecuteFlag) = EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		} else {				/* the IF expression is false, don't execute IF code */
			GLOBAL(ExecuteFlag) = BEFORE_EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		}
		yystype_destructor(expr _INLINE_TLS);
	} else {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
	}
}


inline void cs_end_if( INLINE_TLS_VOID)
{
	/* restore previous status */
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
}


inline int cs_break_continue(YYSTYPE *expr, int op_type INLINE_TLS)
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
				yystype_destructor(expr _INLINE_TLS);			
			}
			return FAILURE;
		} else if (GLOBAL(function_state).loop_change_level > GLOBAL(function_state).loop_nest_level) {
			php3_error(E_ERROR, "Cannot continue from %d loops", (expr ? expr->value.lval : -1));
			if (expr) {
				yystype_destructor(expr _INLINE_TLS);
			}
			return FAILURE;
		}
		GLOBAL(function_state).loop_change_type = op_type;
		GLOBAL(Execute) = 0;
		if (expr) {
			yystype_destructor(expr _INLINE_TLS);
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
	local_stack_top = stack_int_top(&GLOBAL(css));
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(ExecuteFlag) == BEFORE_EXECUTE && local_stack_top == EXECUTE) {
		GLOBAL(ExecuteFlag) = EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_elseif_end_evaluate( INLINE_TLS_VOID)
{
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
}


inline void cs_start_elseif(YYSTYPE *expr INLINE_TLS)
{
	if (GLOBAL(ExecuteFlag) == EXECUTE) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
	if (GLOBAL(ExecuteFlag) == BEFORE_EXECUTE) {
		if (yystype_true(expr)) {
			GLOBAL(ExecuteFlag) = EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		}
		yystype_destructor(expr _INLINE_TLS);
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


inline void get_function_parameter(YYSTYPE *varname INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE tmp;

		if (hash_index_find(GLOBAL(active_symbol_table), GLOBAL(param_index), (void **) &GLOBAL(data)) == FAILURE) {
			php3_error(E_WARNING, "Missing argument %d in call to %s()",GLOBAL(param_index)+1,GLOBAL(function_state).function_name);
			var_reset(&tmp);
			hash_update(GLOBAL(active_symbol_table), varname->value.strval, varname->strlen+1, &tmp, sizeof(YYSTYPE), NULL);
		} else if (!hash_index_is_pointer(GLOBAL(active_symbol_table), GLOBAL(param_index))) { /* passed by value */
			tmp = *GLOBAL(data);
			yystype_copy_constructor(&tmp);
			hash_update(GLOBAL(active_symbol_table), varname->value.strval, varname->strlen+1, &tmp, sizeof(YYSTYPE), NULL);
		} else { /* passed by reference */
			hash_pointer_update(GLOBAL(active_symbol_table), varname->value.strval, varname->strlen+1, GLOBAL(data));
			hash_index_del(GLOBAL(active_symbol_table), GLOBAL(param_index));
		}

		GLOBAL(param_index)++;
	}
}


inline void pass_parameter_by_value(YYSTYPE *expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		if (GLOBAL(function_state).func_arg_types) {
			unsigned char argument_offset=hash_next_free_element(GLOBAL(function_state).function_symbol_table)+1;
			
			if (argument_offset<=GLOBAL(function_state).func_arg_types[0]
				&& GLOBAL(function_state).func_arg_types[argument_offset]==BYREF_FORCE) {
				php3_error(E_WARNING,"Cannot pass expression as argument %d by reference",argument_offset);
				yystype_destructor(expr _INLINE_TLS);
				GLOBAL(function_state).function_type = 0;	/* don't execute the function call */
				return;
			}
		}
		
		if (hash_next_index_insert(GLOBAL(function_state).function_symbol_table, expr, sizeof(YYSTYPE),NULL) == FAILURE) {
			php3_error(E_WARNING, "Error updating symbol table");
			yystype_destructor(expr _INLINE_TLS);
			GLOBAL(function_state).function_type = 0;	/* don't execute the function call */
		}
	}
}


inline void start_function_decleration(YYSTYPE *function_token, YYSTYPE *function_name INLINE_TLS)
{
	HashTable *target_symbol_table;
	
	if (GLOBAL(Execute)) {
		yystype_copy_constructor(function_name);
		
		if (GLOBAL(class_name)) {
			target_symbol_table = GLOBAL(class_symbol_table);
			hash_del(GLOBAL(class_symbol_table), function_name->value.strval, function_name->strlen+1);	/* for inheritance */
		} else {
			target_symbol_table = &GLOBAL(function_table);
		}
		php3_str_tolower(function_name->value.strval, function_name->strlen);
		if (hash_exists(target_symbol_table, function_name->value.strval, function_name->strlen+1)) {
			php3_error(E_ERROR, "Can't redeclare already declared function");
			yystype_destructor(function_name _INLINE_TLS);
			return;
		}
		function_token->type = IS_USER_FUNCTION;
		function_token->value.ht = NULL;
		function_token->func_arg_types = NULL;
		hash_update(target_symbol_table, function_name->value.strval, function_name->strlen+1, function_token, sizeof(YYSTYPE), NULL);
		yystype_destructor(function_name _INLINE_TLS);
	}
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	GLOBAL(ExecuteFlag) = DONT_EXECUTE;
	GLOBAL(Execute) = SHOULD_EXECUTE;
}


inline void end_function_decleration( INLINE_TLS_VOID)
{
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	stack_del_top(&GLOBAL(css));
}


/* don't execute expr1 except for the first iteration */
inline void for_pre_expr1(YYSTYPE *for_token INLINE_TLS)
{
	GLOBAL(function_state).loop_nest_level++;
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(Execute)) {
		if (stack_int_top(&GLOBAL(for_stack)) == (long) for_token->offset) {	/* 2nd or later iteration */
			GLOBAL(ExecuteFlag) = DONT_EXECUTE;
			GLOBAL(Execute) = SHOULD_EXECUTE;
		}
	}
}


/* expr2 is the increment if we're switched.  don't execute it in the first
 * iteration, and execute it otherwise.
 * if we're not switched, it's the truth value, and we want to evaluate it anyway.
 */
inline void for_pre_expr2(YYSTYPE *for_token INLINE_TLS)
{	
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (for_token->cs_data.switched && stack_int_top(&GLOBAL(for_stack)) != (long) for_token->offset) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;	/* first iteration of already switched for */
	}
}


/* if we're not switched, expr3 is the increment.  we shouldn't execute it
 * in the first iteration.  since not being switched --> first iteration,
 * we don't have to check the for_stack.
 */
inline void for_pre_expr3(YYSTYPE *for_token, YYSTYPE *expr2 INLINE_TLS)
{
	if (for_token->cs_data.switched && stack_int_top(&GLOBAL(for_stack)) != (long) for_token->offset) {
		var_reset(expr2);
	}
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute) && !for_token->cs_data.switched) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void for_pre_statement(YYSTYPE *for_token, YYSTYPE *expr2, YYSTYPE *expr3 INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute) && !for_token->cs_data.switched) {
		var_reset(expr3);
	}
	if (GLOBAL(Execute) && for_token->cs_data.switched) {
		if (yystype_true(expr3)) {
			GLOBAL(ExecuteFlag) = EXECUTE;
		} else {
			GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		}
		GLOBAL(Execute) = SHOULD_EXECUTE;
		yystype_destructor(expr2 _INLINE_TLS);
		yystype_destructor(expr3 _INLINE_TLS);
	} else if (GLOBAL(Execute) && !for_token->cs_data.switched) {
		if (yystype_true(expr2)) {
			GLOBAL(ExecuteFlag) = EXECUTE;
		} else {
			GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		}
		GLOBAL(Execute) = SHOULD_EXECUTE;
		yystype_destructor(expr2 _INLINE_TLS);
		yystype_destructor(expr3 _INLINE_TLS);
	}
}

inline void for_post_statement(YYSTYPE *for_token, YYSTYPE *first_semicolon, YYSTYPE *second_semicolon, YYSTYPE *close_parentheses INLINE_TLS)
{
	if (stack_int_top(&GLOBAL(for_stack)) != (long) for_token->offset) {	/* first iteration */
		stack_push(&GLOBAL(for_stack), &for_token->offset, sizeof(int));
	}
	if (!for_token->cs_data.switched) {
		tc_switch(&GLOBAL(token_cache_manager), first_semicolon->offset + 1, close_parentheses->offset - 1, second_semicolon->offset);
		tc_set_switched(&GLOBAL(token_cache_manager), for_token->offset);
	}
	if (GLOBAL(Execute)) {
		seek_token(&GLOBAL(token_cache_manager), for_token->offset);
	} else {
		if ((GLOBAL(function_state).loop_change_type != DO_NOTHING) && (GLOBAL(function_state).loop_change_level == GLOBAL(function_state).loop_nest_level)) {
			if (GLOBAL(function_state).loop_change_type == DO_CONTINUE) {
				seek_token(&GLOBAL(token_cache_manager), for_token->offset);
			} else {
				if (stack_int_top(&GLOBAL(for_stack)) == (long) for_token->offset) {
#if (_DEBUG_ & CONTROL_DEBUG)
					fprintf(stderr, "Deleting for addr=%d from for stack", for_token->offset);
#endif
					stack_del_top(&GLOBAL(for_stack));
				}
			}
			GLOBAL(function_state).loop_change_type = DO_NOTHING;
			GLOBAL(function_state).loop_change_level = 0;
		} else {
			if (stack_int_top(&GLOBAL(for_stack)) == (long) for_token->offset) {
#if (_DEBUG_ & CONTROL_DEBUG)
				fprintf(stderr, "Deleting for addr=%d from for stack", for_token->offset);
#endif
				stack_del_top(&GLOBAL(for_stack));
			}
		}
	}
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;

	GLOBAL(function_state).loop_nest_level--;
}


inline void cs_return(YYSTYPE *expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		if (expr) {
			GLOBAL(return_value) = *expr;
		} else {
			var_reset(&GLOBAL(return_value));
		}
		GLOBAL(function_state).returned = 1;
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_start_include(YYSTYPE *include_token INLINE_TLS)
{
	/* evaluate the expression even at Execute=0, if it's the first time we see this include(), otherwise,
	 * don't evaluate it.
	 */
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (!include_token->cs_data.included) {
		GLOBAL(ExecuteFlag) = EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	} else {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}

inline void cs_end_include(YYSTYPE *include_token, YYSTYPE *expr INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (!include_token->cs_data.included) {
		if (!GLOBAL(php3_display_source)) {
			include_file(expr,0);
		}
		tc_set_included(&GLOBAL(token_cache_manager), include_token->offset);
		yystype_destructor(expr _INLINE_TLS);
	}
}



inline void cs_show_source(YYSTYPE *expr INLINE_TLS)
{
	if (include_file(expr,1)==SUCCESS) {
		php3_header(0, NULL);
		stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = 0;
		GLOBAL(php3_display_source)=1;
		php3_printf("<font color=%s>", php3_ini.highlight_html);
	}
	yystype_destructor(expr _INLINE_TLS);
}


inline void cs_pre_boolean_or(YYSTYPE *left_expr INLINE_TLS)
{
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(Execute) && yystype_true(left_expr)) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;;
	}
}


inline void cs_post_boolean_or(YYSTYPE *result, YYSTYPE *left_expr, YYSTYPE *right_expr INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute)) {
		boolean_or_function(result, left_expr, right_expr);
	}
}


inline void cs_pre_boolean_and(YYSTYPE *left_expr INLINE_TLS)
{
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(Execute) && !yystype_true(left_expr)) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_post_boolean_and(YYSTYPE *result, YYSTYPE *left_expr, YYSTYPE *right_expr INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute)) {
		boolean_and_function(result, left_expr, right_expr);
	}
}


inline void cs_questionmark_op_pre_expr1(YYSTYPE *truth_value INLINE_TLS)
{
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));
	if (GLOBAL(Execute) && !yystype_true(truth_value)) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_questionmark_op_pre_expr2(YYSTYPE *truth_value INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute) && yystype_true(truth_value)) {
		GLOBAL(ExecuteFlag) = DONT_EXECUTE;
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_questionmark_op_post_expr2(YYSTYPE *result, YYSTYPE *truth_value, YYSTYPE *expr1, YYSTYPE *expr2 INLINE_TLS)
{
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute)) {
		if (yystype_true(truth_value)) {
			*result = *expr1;
		} else {
			*result = *expr2;
		}
		yystype_destructor(truth_value _INLINE_TLS);
	}
}


inline void cs_functioncall_pre_variable_passing(YYSTYPE *function_name, YYSTYPE *class_ptr INLINE_TLS)
{
	int minus_one = -1;
	HashTable *target_symbol_table;	/* the symbol table in which the function would be searched */
	YYSTYPE *object=NULL;

	target_symbol_table = &GLOBAL(function_table);
	if (GLOBAL(Execute)) {
		if (class_ptr) {		/* use member function rather than global function */
			object = (YYSTYPE *) class_ptr->value.yystype_ptr;
			
			if (!object) {
				yystype_destructor(function_name _INLINE_TLS);
				php3_error(E_ERROR, "Member function used on a non-object");
				return;
			}
			target_symbol_table = object->value.ht;
		}
		if (function_name->type != IS_STRING) {
			php3_error(E_ERROR, "Function names must be strings");
			yystype_destructor(function_name _INLINE_TLS);
			return;
		}
		php3_str_tolower(function_name->value.strval, function_name->strlen);
		if (hash_find(target_symbol_table, function_name->value.strval, function_name->strlen+1, (void **) &GLOBAL(data)) == SUCCESS) {
			if (!(GLOBAL(data)->type & VALID_FUNCTION)) {
				if (GLOBAL(data)->type == IS_UNSUPPORTED_FUNCTION) {
					php3_error(E_ERROR, "Function %s() is not supported in this compilation", function_name->value.strval);
				} else {
					php3_error(E_ERROR, "Function call to a non-function (%s)", function_name->value.strval);
				}
				function_name->cs_data.function_call_type=0;
				return;
			}
			/* we're gonna call the function... */
			stack_push(&GLOBAL(for_stack), &minus_one, sizeof(int));
			stack_push(&GLOBAL(function_state_stack), &GLOBAL(function_state), sizeof(FunctionState));	/* save function state */
			function_name->cs_data.function_call_type = GLOBAL(data)->type;
			function_name->offset = GLOBAL(data)->offset;
			GLOBAL(function_state).function_symbol_table = (HashTable *) emalloc(sizeof(HashTable));
			GLOBAL(function_state).function_name = (char *) estrndup(function_name->value.strval,function_name->strlen);
			GLOBAL(function_state).function_type = GLOBAL(data)->type;
			GLOBAL(function_state).handler = (void (*)(INTERNAL_FUNCTION_PARAMETERS)) GLOBAL(data)->value.internal_function;
			GLOBAL(function_state).func_arg_types = GLOBAL(data)->func_arg_types;
			GLOBAL(function_state).lineno = GLOBAL(current_lineno);
			if (!GLOBAL(function_state).function_symbol_table || !GLOBAL(function_state).function_name) {
				php3_error(E_ERROR, "Unable to allocate necessary memory for function call");
				STR_FREE(function_name->value.strval);
				function_name->cs_data.function_call_type=0;
				return;
			}
			if (hash_init(GLOBAL(function_state).function_symbol_table, 0, NULL, YYSTYPE_DESTRUCTOR, 0) == FAILURE) {
				php3_error(E_ERROR, "Unable to initialize new symbol table in function call");
				STR_FREE(function_name->value.strval);
				function_name->cs_data.function_call_type=0;
				return;
			}
			/* push GLOBAL */
			GLOBAL(globals).type = IS_ARRAY;
			GLOBAL(globals).value.ht = &GLOBAL(symbol_table);
			hash_pointer_update(GLOBAL(function_state).function_symbol_table, "GLOBALS", sizeof("GLOBALS"), (void *) &GLOBAL(globals));
			if (object) {	/* push $this */
				hash_pointer_update(GLOBAL(function_state).function_symbol_table, "this", sizeof("this"), (void *) object);
			}
		} else {
			php3_error(E_ERROR, "Call to unsupported or undefined function %s()", function_name->value.strval);
			function_name->cs_data.function_call_type=0;
			return;
		}
	} else {
		function_name->cs_data.function_call_type = 0;
	}
}


inline void cs_functioncall_post_variable_passing(YYSTYPE *function_name INLINE_TLS)
{
	if (function_name->cs_data.function_call_type) {
		stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));

		/* prepare a new function state */
		GLOBAL(function_state).symbol_table = GLOBAL(function_state).function_symbol_table;
		GLOBAL(active_symbol_table) = GLOBAL(function_state).symbol_table;
		GLOBAL(function_state).function_symbol_table = NULL;
		GLOBAL(function_state).loop_nest_level = GLOBAL(function_state).loop_change_level = GLOBAL(function_state).loop_change_type = 0;

		switch (GLOBAL(function_state).function_type) {
			case IS_USER_FUNCTION:
				seek_token(&GLOBAL(token_cache_manager), function_name->offset);
				break;
			case IS_INTERNAL_FUNCTION:
			case IS_TEMPORARY_INTERNAL_FUNCTION:
				var_reset(&GLOBAL(return_value)); 
				GLOBAL(function_state).handler(GLOBAL(function_state).symbol_table,&GLOBAL(return_value),&GLOBAL(list),&GLOBAL(plist));
				break;
			default:			/* don't execute the function call */
				break;
		}
	}
}


inline void cs_functioncall_end(YYSTYPE *result, YYSTYPE *function_name, YYSTYPE *close_parentheses INLINE_TLS)
{
	if (function_name->cs_data.function_call_type) {
		FunctionState *fs_ptr;

		/* handle return value for user functions (internal functions do that themselves) */
		*result = GLOBAL(return_value);
		if (GLOBAL(function_state).function_type==IS_USER_FUNCTION && !GLOBAL(function_state).returned) {
			var_reset(result);  /* default return value */
		}
		
		/* clean up */
		hash_destroy(GLOBAL(function_state).symbol_table);
		efree(GLOBAL(function_state).symbol_table);
		STR_FREE(GLOBAL(function_state).function_name);
		yystype_destructor(function_name _INLINE_TLS);
		
		while (stack_int_top(&GLOBAL(for_stack)) != -1) {  /* pop FOR stack */
			stack_del_top(&GLOBAL(for_stack));
		}
		stack_del_top(&GLOBAL(for_stack));
		
	
		/* jump back */
		if (GLOBAL(function_state).function_type == IS_USER_FUNCTION) {
			seek_token(&GLOBAL(token_cache_manager), close_parentheses->offset + 1);
		}
		
		
		/* get previous function state */
		stack_top(&GLOBAL(function_state_stack), (void **) &fs_ptr);
		GLOBAL(function_state) = *fs_ptr;
		stack_del_top(&GLOBAL(function_state_stack));
		GLOBAL(active_symbol_table) = GLOBAL(function_state).symbol_table;


		/* restore execution state */
		GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
		stack_del_top(&GLOBAL(css));
		GLOBAL(Execute) = SHOULD_EXECUTE;
	}
}


inline void cs_switch_start(YYSTYPE *switch_token, YYSTYPE *expr INLINE_TLS)
{
	switch_expr se;

	GLOBAL(function_state).loop_nest_level++;
	stack_push(&GLOBAL(css), &GLOBAL(ExecuteFlag), sizeof(int));

	se.expr = *expr;
	se.offset = switch_token->offset;
	stack_push(&GLOBAL(switch_stack), &se, sizeof(switch_expr));
}


/* if case_expr is NULL, assume default */
inline void cs_switch_case_pre(YYSTYPE *case_expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		switch_expr *se;
		YYSTYPE expr, result;
		int is_equal = 0;

		stack_top(&GLOBAL(switch_stack), (void **) &se);
		if (se->offset != -1) {	/* no matched case yet */
			/* if case_expr is NULL, we're in the special 'default' case */
			if (case_expr) {
				expr = se->expr;
				yystype_copy_constructor(&expr);
				is_equal_function(&result, &expr, case_expr _INLINE_TLS);
				is_equal = yystype_true(&result);
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
				yystype_destructor(case_expr _INLINE_TLS);
			}
		}
	}
}

inline void cs_switch_case_post( INLINE_TLS_VOID)
{
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
}


inline void cs_switch_end(YYSTYPE *expr INLINE_TLS)
{
	if (!GLOBAL(Execute)) {
		if (GLOBAL(function_state).loop_change_level == GLOBAL(function_state).loop_nest_level) {
			GLOBAL(function_state).loop_change_type = DO_NOTHING;
		}
	}
	GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
	stack_del_top(&GLOBAL(css));
	GLOBAL(Execute) = SHOULD_EXECUTE;
	if (GLOBAL(Execute)) {
		yystype_destructor(expr _INLINE_TLS);
	}
	stack_del_top(&GLOBAL(switch_stack));
	GLOBAL(function_state).loop_nest_level--;
}


inline void cs_start_class_decleration(YYSTYPE *classname, YYSTYPE *parent INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE new_class, *parent_ptr;

		if (hash_exists(&GLOBAL(function_table), classname->value.strval, classname->strlen+1)) {
			php3_error(E_ERROR,"%s is already a function or class",classname->value.strval);
		}
		if (parent) {
			if (hash_find(&GLOBAL(function_table), parent->value.strval, parent->strlen+1, (void **) &parent_ptr) == FAILURE) {
				php3_error(E_ERROR, "Cannot extend non existant class %s", parent->value.strval);
				return;
			}
			new_class = *parent_ptr;
			yystype_copy_constructor(&new_class);
		} else {
			new_class.type = IS_CLASS;
			new_class.value.ht = (HashTable *) emalloc(sizeof(HashTable));
			hash_init(new_class.value.ht, 0, NULL, YYSTYPE_DESTRUCTOR, 0);
		}
		if (hash_update(&GLOBAL(function_table), classname->value.strval, classname->strlen+1, &new_class, sizeof(YYSTYPE),NULL) == FAILURE) {
			php3_error(E_ERROR, "Unable to initialize new class");
		}
		GLOBAL(class_name) = classname->value.strval;
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


inline void start_array_parsing(YYSTYPE *array_name,YYSTYPE *class_ptr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		HashTable *target_symbol_table=GLOBAL(active_symbol_table);
	
		if ((array_name->type != IS_STRING)) {
			php3_error(E_WARNING,"Illegal array name");
			GLOBAL(array_ptr) = NULL;
		} else {
			if (class_ptr) {
				YYSTYPE *object=(YYSTYPE *) class_ptr->value.yystype_ptr;
				
				if (!object) {
					GLOBAL(array_ptr)=NULL;
					yystype_destructor(array_name _INLINE_TLS);
					return;
				}
				target_symbol_table = object->value.ht;
			}
			if (hash_find(target_symbol_table,array_name->value.strval, array_name->strlen+1, (void **)&GLOBAL(array_ptr)) == FAILURE) {
				YYSTYPE tmp;
				variable_tracker vt;

				array_init(&tmp);
				hash_update(target_symbol_table,array_name->value.strval,array_name->strlen+1,(void *) &tmp, sizeof(YYSTYPE),(void **) &GLOBAL(array_ptr));
				GLOBAL(array_ptr)->cs_data.array_write=1;
								
				vt.type = IS_STRING;
				vt.strlen = array_name->strlen;
				vt.strval = estrndup(array_name->value.strval,vt.strlen);
				vt.ht = target_symbol_table;
				stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
			} else {
				GLOBAL(array_ptr)->cs_data.array_write=0;
			}
			if (GLOBAL(array_ptr)->type != IS_ARRAY && GLOBAL(array_ptr)->type != IS_STRING) {
	            php3_error(E_WARNING, "Variable $%s is not an array or string", array_name->value.strval);
				GLOBAL(array_ptr) = NULL;
			}
		}
		yystype_destructor(array_name _INLINE_TLS);
	}
}

inline void start_dimensions_parsing(YYSTYPE *result INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		result->value.yystype_ptr = GLOBAL(array_ptr);
		if (GLOBAL(array_ptr)) {
			result->cs_data.array_write=GLOBAL(array_ptr)->cs_data.array_write;
		}
		result->strlen=-1;
	}
}
  
inline void fetch_array_index(YYSTYPE *result, YYSTYPE *expr, YYSTYPE *dimension INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		int original_array_write = dimension->cs_data.array_write;
		
		result->cs_data.array_write = dimension->cs_data.array_write;
			
		if (dimension->value.yystype_ptr) {
			YYSTYPE *localdata,tmp,*arr_ptr=dimension->value.yystype_ptr;
			int new_array=0; /* whether we created the array just now */
	
			if (arr_ptr->type==IS_STRING && arr_ptr->value.strval!=empty_string && arr_ptr->value.strval!=undefined_variable_string) {
				if (dimension->strlen == -1) { /* ok */
					if (!expr) {
						php3_error(E_WARNING,"No string index specified");
						result->value.yystype_ptr=NULL;
						return;
					}
					convert_to_long(expr);
					if (expr->value.lval<0 || expr->value.lval>=arr_ptr->strlen) {
						php3_error(E_WARNING,"Illegal string index");
						result->value.yystype_ptr=NULL;
						return;
					}
					result->strlen = expr->value.lval;
					return;
				} else {
					php3_error(E_WARNING,"Cannot index a string index");
					result->value.yystype_ptr=NULL;
					return;
				}
			}
			if (arr_ptr->type==IS_NULL || (arr_ptr->type==IS_STRING && 
				(arr_ptr->value.strval==empty_string || arr_ptr->value.strval==undefined_variable_string))) {
				array_init(arr_ptr);
				new_array=1;
			} else if (arr_ptr->type!=IS_ARRAY) {
				php3_error(E_WARNING,"Index referencing a non-array");
				result->value.yystype_ptr=NULL;
				yystype_destructor(expr _INLINE_TLS);
				return;
			}
			if (expr) {
				switch (expr->type) {
					case IS_LONG:
					case IS_DOUBLE:
						convert_to_long(expr);
						if (new_array || hash_index_find(arr_ptr->value.ht, expr->value.lval, (void **) &localdata) == FAILURE) {
							tmp.type = IS_NULL;
							hash_index_update(arr_ptr->value.ht, expr->value.lval, (void *)&tmp, sizeof(YYSTYPE),(void **) &localdata);
							result->value.yystype_ptr = localdata;
							result->cs_data.array_write = 1;
							if (!original_array_write) { /* we raised the write flag just now, record this variable */
								variable_tracker vt;
								
								vt.type = IS_LONG;
								vt.lval = expr->value.lval;
								vt.ht = arr_ptr->value.ht;
								stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
							}
						} else {
							result->value.yystype_ptr = localdata;
						}
						break;
					case IS_STRING:
						if (new_array || hash_find(arr_ptr->value.ht, expr->value.strval, expr->strlen+1, (void **) &localdata)==FAILURE) {
							tmp.type = IS_NULL;
							if (hash_update(arr_ptr->value.ht, expr->value.strval, expr->strlen+1, (void *) &tmp, sizeof(YYSTYPE),(void **) &localdata)==FAILURE) {
								result->value.yystype_ptr=NULL;
								STR_FREE(expr->value.strval);
								return;
							}
							result->value.yystype_ptr = localdata;
							result->cs_data.array_write = 1;
							if (!original_array_write) { /* we raised the write flag just now, record this variable */
								variable_tracker vt;
								
								vt.type = IS_STRING;
								vt.strval = estrndup(expr->value.strval,expr->strlen);
								vt.strlen = expr->strlen;
								vt.ht = arr_ptr->value.ht;
								stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
							}
						} else {
							result->value.yystype_ptr = localdata;
						}
						break;
					default:
						result->value.yystype_ptr = NULL;
						php3_error(E_WARNING, "Illegal index type");
						break;
				}
			} else { /* assign-next */
				int new_index=hash_next_free_element(arr_ptr->value.ht);
				
				tmp.type = IS_NULL;
				hash_next_index_insert(arr_ptr->value.ht, (void *) &tmp, sizeof(YYSTYPE), (void **) &localdata);
				result->value.yystype_ptr = localdata;
				result->cs_data.array_write=1;
				if (!original_array_write) { /* we raised the write flag just now, record this variable */
					variable_tracker vt;
								
					vt.type = IS_LONG;
					vt.lval = new_index;
					vt.ht = arr_ptr->value.ht;
					stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
				}
			}
		} else {
			result->value.yystype_ptr=NULL;
		}
		if (expr) {
			yystype_destructor(expr _INLINE_TLS);
		}
	}
}	


inline void end_array_parsing(YYSTYPE *result, YYSTYPE *dimensions INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE *ptr=(YYSTYPE *) dimensions->value.yystype_ptr;

		*result = *dimensions;
		if (ptr && ptr->type==IS_NULL) {
			var_reset(ptr);
		}
	}
}


inline void clean_unassigned_variable_top(int delete_var INLINE_TLS)
{
	variable_tracker *vt;
	
	if (stack_top(&GLOBAL(variable_unassign_stack),(void **) &vt)==SUCCESS) {
		switch(vt->type) {
			case IS_LONG:
				if (delete_var) {
					hash_index_del(vt->ht,vt->lval);
				}
				break;
			case IS_STRING:
				if (delete_var) {
					hash_del(vt->ht,vt->strval,vt->strlen+1);
				}
				STR_FREE(vt->strval);
				break;
		}
	}
	stack_del_top(&GLOBAL(variable_unassign_stack));
}


inline void get_class_variable_pointer(YYSTYPE *result, YYSTYPE *class_ptr, YYSTYPE *varname INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE *object = (YYSTYPE *) class_ptr->value.yystype_ptr;
		
		if (!object) {
			result->value.yystype_ptr = NULL;
			yystype_destructor(varname _INLINE_TLS);
			return;
		}
		if ((varname->type != IS_STRING)) {
			php3_error(E_WARNING,"Illegal property name");
			result->value.yystype_ptr = NULL;
			yystype_destructor(varname _INLINE_TLS);
			return;
		} else {
			YYSTYPE *data;

			if (hash_find(object->value.ht,varname->value.strval, varname->strlen+1, (void **)&data) == FAILURE) {
				YYSTYPE tmp;
				variable_tracker vt;

				var_reset(&tmp);
				hash_update(object->value.ht,varname->value.strval,varname->strlen+1,(void *) &tmp, sizeof(YYSTYPE),(void **) &data);
				
				vt.type = IS_STRING;
				vt.strlen = varname->strlen;
				vt.strval = estrndup(varname->value.strval,vt.strlen);
				vt.ht = object->value.ht;
				stack_push(&GLOBAL(variable_unassign_stack),(void *) &vt, sizeof(variable_tracker));
				result->cs_data.array_write = 1;
			} else {
				result->cs_data.array_write = 0;
			}
			result->value.yystype_ptr = data;
			result->strlen = -1; /* Not indexed string */
			yystype_destructor(varname _INLINE_TLS);
		}
	}
}


inline void pass_parameter(YYSTYPE *var, int by_reference INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE tmp;
		/* ... check if we need to force by reference (set by_reference=1) */

		if (GLOBAL(function_state).func_arg_types) {
			unsigned char argument_offset=hash_next_free_element(GLOBAL(function_state).function_symbol_table)+1;
			
			if (argument_offset<=GLOBAL(function_state).func_arg_types[0]
				&& GLOBAL(function_state).func_arg_types[argument_offset]!=BYREF_NONE) {
				by_reference=1;
			}
		}
				
		if (!by_reference) {
			read_pointer_value(&tmp,var _INLINE_TLS);
			if (hash_next_index_insert(GLOBAL(function_state).function_symbol_table, &tmp, sizeof(YYSTYPE),NULL) == FAILURE) {
				php3_error(E_WARNING, "Error updating symbol table");
				yystype_destructor(&tmp _INLINE_TLS);
				GLOBAL(function_state).function_type = 0;	/* don't execute the function call */
				return;
			}
		} else {
			if (var->cs_data.array_write) {
				clean_unassigned_variable_top(0 _INLINE_TLS);
			}
			if (var->value.yystype_ptr==NULL) {
				GLOBAL(function_state).function_type = 0;
				return;
			}
			if (hash_next_index_pointer_insert(GLOBAL(function_state).function_symbol_table, (void *) var->value.yystype_ptr) == FAILURE) {
				php3_error(E_WARNING, "Error updating symbol table");
				GLOBAL(function_state).function_type = 0;	/* don't execute the function call */
				return;
			}
		}
	}
}


inline void cs_system(YYSTYPE *result,YYSTYPE *expr INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		FILE *in;
		int readbytes,total_readbytes=0,allocated_space;
		
		if (php3_ini.safe_mode) {
			php3_error(E_WARNING,"Cannot execute using backqutoes in safe mode");
			yystype_destructor(expr _INLINE_TLS);
			var_reset(result);
			return;
		}

		convert_to_string(expr);
#if WIN32|WINNT
		if ((in=popen(expr->value.strval,"rt"))==NULL) {
#else
		if ((in=popen(expr->value.strval,"r"))==NULL) {
#endif
			php3_error(E_WARNING,"Unable to execute '%s'",expr->value.strval);
		}
		allocated_space = EXEC_INPUT_BUF;
		result->value.strval = (char *) emalloc(allocated_space);
		while (1) {
			readbytes = fread(result->value.strval+total_readbytes,1,EXEC_INPUT_BUF,in);
			if (readbytes<=0) {
				break;
			}
			total_readbytes += readbytes;
			allocated_space = total_readbytes+EXEC_INPUT_BUF;
			result->value.strval = (char *) erealloc(result->value.strval,allocated_space);
		}
		fclose(in);
		
		result->value.strval = erealloc(result->value.strval,total_readbytes+1);
		result->value.strval[total_readbytes]=0;
		result->strlen = total_readbytes;
		result->type = IS_STRING;
		STR_FREE(expr->value.strval);
	}
}


inline void add_regular_encapsed_variable(YYSTYPE *result,YYSTYPE *encaps,YYSTYPE *varname  INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE tmp;
		
		get_regular_variable_contents(&tmp,varname,0 _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
	}
}


inline void add_assoc_array_encapsed_variable(YYSTYPE *result, YYSTYPE *encaps, YYSTYPE *varname, YYSTYPE *index INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE tmp;
				
		get_array_variable(&tmp,varname,index _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
	}
}

inline void add_regular_array_encapsed_variable(YYSTYPE *result, YYSTYPE *encaps, YYSTYPE *varname, YYSTYPE *index INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE tmp;

		COPY_STRING(*index);
		convert_to_long(index);
		get_array_variable(&tmp,varname,index _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
	}
}


inline void add_variable_array_encapsed_variable(YYSTYPE *result, YYSTYPE *encaps, YYSTYPE *varname, YYSTYPE *var_index INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE index,tmp;
		
		get_regular_variable_contents(&index,var_index,0 _INLINE_TLS);
		get_array_variable(&tmp,varname,&index _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
	}
}


inline void add_encapsed_object_property(YYSTYPE *result, YYSTYPE *encaps, YYSTYPE *object, YYSTYPE *property INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE tmp;
				
		get_array_variable(&tmp,object,property _INLINE_TLS);
		concat_function(result,encaps,&tmp,1 _INLINE_TLS);
	}
}


inline void add_indirect_encapsed_variable(YYSTYPE *result,YYSTYPE *encaps,YYSTYPE *varname INLINE_TLS)
{
	if (GLOBAL(Execute)) {
		YYSTYPE tmp,tmp2;
		
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
