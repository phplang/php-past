%{

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


/* $Id: language-parser.y,v 1.116 1998/02/01 20:53:30 zeev Exp $ */


/* 
 * LALR shift/reduce conflicts and how they are resolved:
 *
 * - 2 shift/reduce conflicts due to the dangeling elseif/else ambiguity.  Solved by shift.
 * - 1 shift/reduce conflict due to arrays within encapsulated strings. Solved by shift. 
 * - 1 shift/reduce conflict due to objects within encapsulated strings.  Solved by shift.
 * 
 */

#ifdef THREAD_SAFE
#include "tls.h"
#define PHP3_THREAD_SAFE
#else
extern char *phptext;
extern int phpleng;
#define YY_TLS_VARS
#endif
#include "parser.h"
#include "control_structures.h"
#include "control_structures_inline.h"
#include "main.h"
#include "functions/head.h"
#include "list.h"

#define YYERROR_VERBOSE

#ifndef THREAD_SAFE
HashTable symbol_table;		/* main symbol table */
HashTable function_table;	/* function symbol table */
HashTable include_names;	/* included file names are stored here, for error messages */
TokenCacheManager token_cache_manager;
Stack css;					/* Control Structure Stack, used to store execution states */
Stack for_stack;			/* For Stack, used in order to determine
							 * whether we're in the first iteration of 
							 * a for loop or not
							 */
Stack input_source_stack;		/* Include Stack, used to keep track of
							 * of the file pointers when include()'ing
							 * files.
							 */
Stack function_state_stack;	/* Function State Stack, used to keep inforation
							 * about the function call, such as its
							 * loop nesting level, symbol table, etc.
							 */
Stack switch_stack;			/* Switch stack, used by the switch() control
							 * to determine whether a case was already
							 * matched.
							 */

Stack variable_unassign_stack; /* Stack used to unassign variables that weren't properly defined */

HashTable *active_symbol_table;  /* this always points to the
								  * currently active symbol
								  * table.
								  */

int Execute;	/* This flag is used by all parts of the program, in order
				 * to determine whether or not execution should take place.
				 * It's value is dependant on the value of the ExecuteFlag,
				 * the loop_change_level (if we're during a break or 
				 * continue) and the function_state.returned (whether
				 * our current function has been return()ed from).
				 * In order to restore the correct value, after changing
				 * one of the above (e.g. when popping the css),
				 * one should use the macro Execute = SHOULD_EXECUTE;
				 */
int ExecuteFlag;
int current_lineno;		/* Current line number, broken with include()s */
int include_count;

/* This structure maintains information about the current active scope.
 * Kept are the loop nesting level, information about break's and continue's
 * we might be in, whether or not our current function has been return()'d
 * from, and also, a pointer to the active symbol table.  During variable
 * -passing in a function call, it also contains a pointer to the
 * soon-to-be-called function symbol table.
 */
FunctionState function_state;

/* The following two variables are used when inside class definitions. */
char *class_name=NULL;
HashTable *class_symbol_table=NULL;

/* Variables used in function calls */
static YYSTYPE return_value;
static HashTable list, plist;
YYSTYPE *data,globals;
unsigned int param_index;  /* used during function calls */

/* Temporary variable used for multi dimensional arrays */
YYSTYPE *array_ptr;

extern int shutdown_requested;
#endif /* THREAD SAFE*/

int init_resource_list(void)
{
	TLS_VARS;
	
	return hash_init(&GLOBAL(list), 0, NULL, list_entry_destructor, 0);
}

int init_resource_plist(void)
{
	TLS_VARS;
	
	return hash_init(&GLOBAL(plist), 0, NULL, plist_entry_destructor, 1);
}

void destroy_resource_list(void)
{
	TLS_VARS;
	
	hash_destroy(&GLOBAL(list));
}

void destroy_resource_plist(void)
{
	TLS_VARS;
	
	hash_destroy(&GLOBAL(plist));
}

%}

%pure_parser
%expect 4

%left ','
%left LOGICAL_OR
%left LOGICAL_AND
%right PHP_PRINT
%left '=' PLUS_EQUAL MINUS_EQUAL MUL_EQUAL DIV_EQUAL CONCAT_EQUAL MOD_EQUAL AND_EQUAL OR_EQUAL XOR_EQUAL
%left '?' ':'
%left BOOLEAN_OR
%left BOOLEAN_AND
%left '|'
%left '^'
%left '&'
%nonassoc IS_EQUAL IS_NOT_EQUAL
%nonassoc '<' IS_SMALLER_OR_EQUAL '>' IS_GREATER_OR_EQUAL
%left SHIFT_LEFT SHIFT_RIGHT
%left '+' '-' '.'
%left '*' '/' '%'
%right '!' '~' INCREMENT DECREMENT INT_CAST DOUBLE_CAST STRING_CAST '@'
%right '['
%token EXIT
%token IF
%left ELSEIF
%left ELSE
%left ENDIF
%token LNUMBER
%token DNUMBER
%token STRING
%token NUM_STRING
%token INLINE_HTML
%token CHARACTER
%token BAD_CHARACTER
%token ENCAPSED_AND_WHITESPACE
%token PHP_ECHO
%token DO
%token WHILE
%token ENDWHILE
%token FOR
%token SWITCH
%token ENDSWITCH
%token CASE
%token DEFAULT
%token BREAK
%token CONTINUE

%token OLD_FUNCTION
%token FUNCTION
%token RETURN
%token INCLUDE
%token REQUIRE
%token SHOW_SOURCE
%token PHP_GLOBAL
%token PHP_STATIC
%token PHP_UNSET
%token PHP_ISSET
%token CLASS
%token EXTENDS
%token PHP_CLASS_OPERATOR
%token PHP_DOUBLE_ARROW
%token PHP_LIST
%token PHP_ARRAY
%token NEW
%token VAR
%token EVAL
%token DONE_EVAL
%token PHP_LINE
%token PHP_FILE

%% /* Rules */

statement_list:	
		statement_list statement
	|	/* empty */
;


statement:
		'{' statement_list '}'
	|	IF '(' expr ')' { cs_start_if(&$3 _INLINE_TLS); } statement elseif_list else_single { cs_end_if( _INLINE_TLS_VOID); }
	|	IF '(' expr ')' ':' { cs_start_if(&$3 _INLINE_TLS); } statement_list new_elseif_list new_else_single { cs_end_if( _INLINE_TLS_VOID); } ENDIF ';'
	|	WHILE '(' expr ')' { cs_start_while(&$3 _INLINE_TLS); } statement { cs_end_while(&$1 _INLINE_TLS); }
	|	WHILE '(' expr ')' ':' { cs_start_while(&$3 _INLINE_TLS); } statement_list ENDWHILE ';' { cs_end_while(&$1 _INLINE_TLS); }
	|	DO { cs_start_do_while( _INLINE_TLS_VOID); } statement WHILE { cs_force_eval_do_while( _INLINE_TLS_VOID); } '(' expr ')' ';'{ cs_end_do_while(&$1,&$7 _INLINE_TLS); }
	|	FOR { for_pre_expr1(&$1 _INLINE_TLS); }
			'(' for_expr { if (GLOBAL(Execute)) yystype_destructor(&$4 _INLINE_TLS); for_pre_expr2(&$1 _INLINE_TLS); }
			';' for_expr { for_pre_expr3(&$1,&$7 _INLINE_TLS); }
			';' for_expr { for_pre_statement(&$1,&$7,&$10 _INLINE_TLS); }
			')' statement { for_post_statement(&$1,&$6,&$9,&$12 _INLINE_TLS); }
	|	SWITCH '(' expr ')' { cs_switch_start(&$1,&$3 _INLINE_TLS); } '{' case_list '}' { cs_switch_end(&$3 _INLINE_TLS); }
	|	SWITCH '(' expr ')' { cs_switch_start(&$1,&$3 _INLINE_TLS); } ':' case_list ENDSWITCH ';' { cs_switch_end(&$3 _INLINE_TLS); }
	|	BREAK ';' { DO_OR_DIE(cs_break_continue(NULL,DO_BREAK _INLINE_TLS)) }	
	|	BREAK expr ';' { DO_OR_DIE(cs_break_continue(&$2,DO_BREAK _INLINE_TLS)) }
	|	CONTINUE ';' { DO_OR_DIE(cs_break_continue(NULL,DO_CONTINUE _INLINE_TLS)) }
	|	CONTINUE expr ';' { DO_OR_DIE(cs_break_continue(&$2,DO_CONTINUE _INLINE_TLS)) }
	|	OLD_FUNCTION STRING { start_function_decleration(&$1,&$2 _INLINE_TLS); } 
			parameter_list '(' statement_list ')' ';' { end_function_decleration( _INLINE_TLS_VOID); }
	|	FUNCTION STRING '(' { start_function_decleration(&$1,&$2 _INLINE_TLS); }
			parameter_list ')' '{' statement_list '}' { end_function_decleration( _INLINE_TLS_VOID); }
	|	RETURN ';'   { cs_return(NULL _INLINE_TLS); }
	|	RETURN expr ';'   { cs_return(&$2 _INLINE_TLS); }
	|	PHP_GLOBAL global_var_list
	|	PHP_STATIC static_var_list
	|	CLASS STRING { cs_start_class_decleration(&$2,NULL _INLINE_TLS); } '{' class_statement_list '}' ';' { cs_end_class_decleration( _INLINE_TLS_VOID); }
	|	CLASS STRING EXTENDS STRING { cs_start_class_decleration(&$2,&$4 _INLINE_TLS); } '{' class_statement_list '}' ';' { cs_end_class_decleration( _INLINE_TLS_VOID); }
	|	PHP_ECHO echo_expr_list ';'
	|	INLINE_HTML { if (GLOBAL(Execute)) { if(php3_header(0,NULL)) PUTS($1.value.strval); } }
	|	expr ';'  { if (GLOBAL(Execute)) yystype_destructor(&$1 _INLINE_TLS); }
	|	REQUIRE { cs_start_include(&$1 _INLINE_TLS); } expr ';' { cs_end_include(&$1,&$3 _INLINE_TLS); }
	|	SHOW_SOURCE expr ';' { if (GLOBAL(Execute)) cs_show_source(&$2 _INLINE_TLS); }
	|	EVAL '(' expr ')' ';' { if (GLOBAL(Execute)) eval_string(&$3,&$5 _INLINE_TLS); }
	|	INCLUDE expr ';' { if (GLOBAL(Execute)) conditional_include_file(&$2,&$3 _INLINE_TLS); }
	|	';'		/* empty statement */
;


elseif_list:
		/* empty */
	|	elseif_list ELSEIF '(' { cs_elseif_start_evaluate( _INLINE_TLS_VOID); } 
			expr { cs_elseif_end_evaluate( _INLINE_TLS_VOID); } ')' { cs_start_elseif(&$5 _INLINE_TLS); } statement
;


new_elseif_list:
		/* empty */
	|	new_elseif_list ELSEIF '(' { cs_elseif_start_evaluate( _INLINE_TLS_VOID); } 
			expr { cs_elseif_end_evaluate( _INLINE_TLS_VOID); } ')' ':' { cs_start_elseif(&$6 _INLINE_TLS); } statement_list
;


else_single:
		/* empty */
	|	ELSE { cs_start_else( _INLINE_TLS_VOID); } statement
;


new_else_single:
		/* empty */
	|	ELSE ':' { cs_start_else( _INLINE_TLS_VOID); } statement_list
;


case_list:
		/* empty */
	|	CASE expr ':' { cs_switch_case_pre(&$2 _INLINE_TLS); } statement_list { cs_switch_case_post( _INLINE_TLS_VOID); } case_list
	|	DEFAULT ':' { cs_switch_case_pre(NULL _INLINE_TLS); } statement_list { cs_switch_case_post( _INLINE_TLS_VOID); }
	|	CASE expr ';' { cs_switch_case_pre(&$2 _INLINE_TLS); } statement_list { cs_switch_case_post( _INLINE_TLS_VOID); } case_list
	|	DEFAULT ';' { cs_switch_case_pre(NULL _INLINE_TLS); } statement_list { cs_switch_case_post( _INLINE_TLS_VOID); }
;


parameter_list: 
		{ GLOBAL(param_index)=0; } non_empty_parameter_list
	|	/* empty */
;


non_empty_parameter_list:
		'$' varname_scalar { get_function_parameter(&$2 _INLINE_TLS); }
	|	non_empty_parameter_list ',' '$' varname_scalar { get_function_parameter(&$4 _INLINE_TLS); }
;


function_call_parameter_list:
		non_empty_function_call_parameter_list
	|	/* empty */
;


non_empty_function_call_parameter_list:
		expr_without_variable { pass_parameter_by_value(&$1 _INLINE_TLS); }
	|	assignment_variable_pointer { pass_parameter(&$1,0 _INLINE_TLS); }
	|	'&' assignment_variable_pointer { pass_parameter(&$2,1 _INLINE_TLS); }
	|	non_empty_function_call_parameter_list ',' expr_without_variable { pass_parameter_by_value(&$3 _INLINE_TLS); }
	|	non_empty_function_call_parameter_list ',' assignment_variable_pointer { pass_parameter(&$3,0 _INLINE_TLS); }
	|	non_empty_function_call_parameter_list ',' '&' assignment_variable_pointer { pass_parameter(&$4,1 _INLINE_TLS); }
;

global_var_list:
		global_var_list ',' '$' var { cs_global_variable(&$4 _INLINE_TLS); }
	|	'$' var { cs_global_variable(&$2 _INLINE_TLS); }
;


static_var_list:
		static_var_list ',' '$' var { cs_static_variable(&$4,NULL _INLINE_TLS); }
	|	static_var_list ',' '$' var '=' unambiguous_static_assignment { cs_static_variable(&$4,&$6 _INLINE_TLS); }
	|	'$' var { cs_static_variable(&$2,NULL _INLINE_TLS); }
	|	'$' var '=' unambiguous_static_assignment { cs_static_variable(&$2,&$4 _INLINE_TLS); }

;


unambiguous_static_assignment:
		scalar { if (GLOBAL(Execute)) $$ = $1; }
	|	'(' expr ')' { if (GLOBAL(Execute)) $$ = $2; }
;


class_statement_list:
		class_statement_list class_statement
	|	/* empty */
;


class_statement:
		VAR class_variable_decleration ';'
	|	OLD_FUNCTION STRING { start_function_decleration(&$1,&$2 _INLINE_TLS); } 
			parameter_list '(' statement_list ')' ';' { end_function_decleration( _INLINE_TLS_VOID); }
	|	FUNCTION STRING '(' { start_function_decleration(&$1,&$2 _INLINE_TLS); }
			parameter_list ')' '{' statement_list '}' { end_function_decleration( _INLINE_TLS_VOID); }

;


class_variable_decleration:
		class_variable_decleration ',' '$' varname_scalar { declare_class_variable(&$4,NULL _INLINE_TLS); }
	|	class_variable_decleration ',' '$' varname_scalar '=' expr { declare_class_variable(&$4,&$6 _INLINE_TLS); }
	|	'$' varname_scalar { declare_class_variable(&$2,NULL _INLINE_TLS); }
	|	'$' varname_scalar '=' expr { declare_class_variable(&$2,&$4 _INLINE_TLS); }
;

	
echo_expr_list:	
		/* empty */
	|	echo_expr_list ',' expr { if (GLOBAL(Execute)) { print_variable(&$3 _INLINE_TLS); yystype_destructor(&$3 _INLINE_TLS); } }
	|	expr { if (GLOBAL(Execute)) { print_variable(&$1 _INLINE_TLS); yystype_destructor(&$1 _INLINE_TLS); } }
;


for_expr:
		/* empty */ { $$.value.lval=1;  $$.type=IS_LONG; }  /* ensure empty truth values are considered TRUE */
	|	for_expr ',' expr { if (GLOBAL(Execute)) { $$ = $3; yystype_destructor(&$1 _INLINE_TLS); } }
	|	expr { if (GLOBAL(Execute)) $$ = $1; }
;


expr_without_variable:	
		PHP_LIST '(' assignment_list ')' '=' expr { assign_to_list(&$$, &$3, &$6 _INLINE_TLS);}
	|	assignment_variable_pointer '=' expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,NULL _INLINE_TLS); }
	|	assignment_variable_pointer PLUS_EQUAL expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,add_function _INLINE_TLS); }
	|	assignment_variable_pointer	MINUS_EQUAL expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,sub_function _INLINE_TLS); }
	|	assignment_variable_pointer	MUL_EQUAL expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,mul_function _INLINE_TLS); }	
	|	assignment_variable_pointer DIV_EQUAL expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,div_function _INLINE_TLS); }
	|	assignment_variable_pointer	CONCAT_EQUAL expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,concat_function_with_free _INLINE_TLS); }
	|	assignment_variable_pointer MOD_EQUAL expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,mod_function _INLINE_TLS); }
	|	assignment_variable_pointer	AND_EQUAL expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,bitwise_and_function _INLINE_TLS); }	
	|	assignment_variable_pointer	OR_EQUAL expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,bitwise_or_function _INLINE_TLS); }
	|	assignment_variable_pointer	XOR_EQUAL expr { if (GLOBAL(Execute)) assign_to_variable(&$$,&$1,&$3,bitwise_xor_function _INLINE_TLS); }
	|	assignment_variable_pointer  INCREMENT { if (GLOBAL(Execute)) incdec_variable(&$$,&$1,increment_function,1 _INLINE_TLS); }
	|	INCREMENT assignment_variable_pointer { if (GLOBAL(Execute)) incdec_variable(&$$,&$2,increment_function,0 _INLINE_TLS); }
	|	assignment_variable_pointer DECREMENT { if (GLOBAL(Execute)) incdec_variable(&$$,&$1,decrement_function,1 _INLINE_TLS); }
	|	DECREMENT assignment_variable_pointer { if (GLOBAL(Execute)) incdec_variable(&$$,&$2,decrement_function,0 _INLINE_TLS); }
	|	expr BOOLEAN_OR { cs_pre_boolean_or(&$1 _INLINE_TLS); } expr { cs_post_boolean_or(&$$,&$1,&$4 _INLINE_TLS); }
	|	expr BOOLEAN_AND { cs_pre_boolean_and(&$1 _INLINE_TLS); } expr { cs_post_boolean_and(&$$,&$1,&$4 _INLINE_TLS); }
	|	expr LOGICAL_OR { cs_pre_boolean_or(&$1 _INLINE_TLS); } expr { cs_post_boolean_or(&$$,&$1,&$4 _INLINE_TLS); }
	|	expr LOGICAL_AND { cs_pre_boolean_and(&$1 _INLINE_TLS); } expr { cs_post_boolean_and(&$$,&$1,&$4 _INLINE_TLS); }
	|	expr '|' expr { if (GLOBAL(Execute)) bitwise_or_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr '^' expr { if (GLOBAL(Execute)) bitwise_xor_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr '&' expr { if (GLOBAL(Execute)) bitwise_and_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr '.' expr { if (GLOBAL(Execute)) concat_function(&$$,&$1,&$3,1 _INLINE_TLS); }
	|	expr '+' expr { if (GLOBAL(Execute)) add_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr '-' expr { if (GLOBAL(Execute)) sub_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr '*' expr { if (GLOBAL(Execute)) mul_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr '/' expr { if (GLOBAL(Execute)) div_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr '%' expr { if (GLOBAL(Execute)) mod_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	'+' expr { if (GLOBAL(Execute)) { YYSTYPE tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  add_function(&$$,&tmp,&$2 _INLINE_TLS); } }
	|	'-' expr { if (GLOBAL(Execute)) { YYSTYPE tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  sub_function(&$$,&tmp,&$2 _INLINE_TLS); } }
	|	'!' expr { if (GLOBAL(Execute)) boolean_not_function(&$$,&$2); }
	|	'~' expr { if (GLOBAL(Execute)) bitwise_not_function(&$$,&$2 _INLINE_TLS); }
	|	expr IS_EQUAL expr { if (GLOBAL(Execute)) is_equal_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr IS_NOT_EQUAL expr { if (GLOBAL(Execute)) is_not_equal_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr '<' expr { if (GLOBAL(Execute)) is_smaller_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr IS_SMALLER_OR_EQUAL expr { if (GLOBAL(Execute)) is_smaller_or_equal_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr '>' expr { if (GLOBAL(Execute)) is_greater_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	expr IS_GREATER_OR_EQUAL expr { if (GLOBAL(Execute)) is_greater_or_equal_function(&$$,&$1,&$3 _INLINE_TLS); }
	|	'(' expr ')' { if (GLOBAL(Execute)) $$ = $2; }
	|	'(' expr error {  php3_error(E_PARSE,"'(' unmatched",GLOBAL(current_lineno)); if (GLOBAL(Execute)) $$ = $2; yyerrok; }
	|	expr '?' { cs_questionmark_op_pre_expr1(&$1 _INLINE_TLS); }
			expr ':' { cs_questionmark_op_pre_expr2(&$1 _INLINE_TLS); }
			expr { cs_questionmark_op_post_expr2(&$$,&$1,&$4,&$7 _INLINE_TLS); }
	|	var { cs_functioncall_pre_variable_passing(&$1,NULL _INLINE_TLS); }
			'(' function_call_parameter_list ')' { cs_functioncall_post_variable_passing(&$1 _INLINE_TLS); }
			possible_function_call { cs_functioncall_end(&$$,&$1,&$5 _INLINE_TLS);}
	|	internal_functions_in_yacc
	|	'$' unambiguous_class_name PHP_CLASS_OPERATOR var { cs_functioncall_pre_variable_passing(&$4,&$2 _INLINE_TLS); }
			'(' function_call_parameter_list ')' { cs_functioncall_post_variable_passing(&$4 _INLINE_TLS); }
			possible_function_call { cs_functioncall_end(&$$,&$4,&$8 _INLINE_TLS); }
	|	NEW STRING  { assign_new_object(&$$,&$2 _INLINE_TLS); }
	|	INT_CAST expr { if (GLOBAL(Execute)) { convert_to_long(&$2); $$ = $2; } }
	|	DOUBLE_CAST expr { if (GLOBAL(Execute)) { convert_to_double(&$2); $$ = $2; } }
	|	STRING_CAST expr { if (GLOBAL(Execute)) { convert_to_string(&$2); $$ = $2; } }
	|	EXIT { if (GLOBAL(Execute)) { php3_header(0,NULL); GLOBAL(shutdown_requested)=NORMAL_SHUTDOWN; $$.type=IS_LONG; $$.value.lval=1; } }
	|	EXIT '(' ')'  { if (GLOBAL(Execute)) { php3_header(0,NULL); GLOBAL(shutdown_requested)=NORMAL_SHUTDOWN; $$.type=IS_LONG; $$.value.lval=1; } }
	|	EXIT '(' expr ')'  { if (GLOBAL(Execute)) { if(php3_header(0,NULL)) { convert_to_string(&$3);  PUTS($3.value.strval);  yystype_destructor(&$3 _INLINE_TLS); } GLOBAL(shutdown_requested)=NORMAL_SHUTDOWN; $$.type=IS_LONG; $$.value.lval=1; } }
	|	'@' { $1.cs_data.error_reporting=GLOBAL(error_reporting); GLOBAL(error_reporting)=0; } expr { GLOBAL(error_reporting)=$1.cs_data.error_reporting; $$ = $3; }
	|   scalar { if (GLOBAL(Execute)) $$ = $1; }
	|	PHP_ARRAY '(' array_pair_list ')' { if (GLOBAL(Execute)) $$ = $3; }
	|	'`' encaps_list '`'  { cs_system(&$$,&$2 _INLINE_TLS); }
	|	PHP_PRINT expr { if (GLOBAL(Execute)) { print_variable(&$2 _INLINE_TLS);  yystype_destructor(&$2 _INLINE_TLS);  $$.value.lval=1; $$.type=IS_LONG; } }
	|	PHP_LINE { if (GLOBAL(Execute)) { $$ = $1; } }
	|	PHP_FILE { if (GLOBAL(Execute)) { $$ = $1; COPY_STRING($$); } }
;


scalar:
		LNUMBER { if (GLOBAL(Execute)) $$ = $1; }
	|	DNUMBER     { if (GLOBAL(Execute)) $$ = $1; }
	|	string     { if (GLOBAL(Execute)) $$ = $1; }
;

varname_scalar:
       STRING     { if (GLOBAL(Execute)){ $$ = $1; }}
;



expr:
		assignment_variable_pointer { if (GLOBAL(Execute)) read_pointer_value(&$$,&$1 _INLINE_TLS); }
	|	expr_without_variable { if (GLOBAL(Execute)) $$ = $1; }
;


unambiguous_variable_name:
		var { if (GLOBAL(Execute)) $$ = $1; }
	|	'{' expr '}' { if (GLOBAL(Execute)) $$ = $2; }
;


unambiguous_array_name:
		varname_scalar { if (GLOBAL(Execute)) {$$ = $1;COPY_STRING($$); }}
	|	'{' expr '}' { if (GLOBAL(Execute)) $$ = $2; }

;


unambiguous_class_name:
		unambiguous_class_name PHP_CLASS_OPERATOR varname_scalar { get_object_symtable(&$$,&$1,&$3 _INLINE_TLS); }
	|	multi_dimensional_array { if (GLOBAL(Execute)) { if (((YYSTYPE *)$1.value.yystype_ptr)->type == IS_OBJECT) { $$=$1; } else { $$.value.yystype_ptr=NULL; } } }
	|	varname_scalar { get_object_symtable(&$$,NULL,&$1 _INLINE_TLS); }
;

assignment_list:
		assignment_list ',' assignment_variable_pointer {

	if (GLOBAL(Execute)) {
		$$=$1;
		hash_next_index_insert($$.value.ht,&$3,sizeof(YYSTYPE),NULL);
	}
}
	|	assignment_variable_pointer {

	if (GLOBAL(Execute)) {
		$$.value.ht = (HashTable *) emalloc(sizeof(HashTable));
		hash_init($$.value.ht,0,NULL,NULL,0);
		$$.type = IS_ARRAY;
		hash_next_index_insert($$.value.ht,&$1,sizeof(YYSTYPE),NULL);
	}
}
;


assignment_variable_pointer:
		'$' unambiguous_variable_name { get_regular_variable_pointer(&$$,&$2 _INLINE_TLS); }
	|	'$' multi_dimensional_array { if (GLOBAL(Execute)) $$=$2; }
	|	'$' unambiguous_class_name PHP_CLASS_OPERATOR unambiguous_variable_name { get_class_variable_pointer(&$$,&$2,&$4 _INLINE_TLS); }
	|	'$' unambiguous_class_name PHP_CLASS_OPERATOR unambiguous_array_name '[' { start_array_parsing(&$4,&$2 _INLINE_TLS); } dimensions { end_array_parsing(&$$,&$7 _INLINE_TLS); }
;


var:	
		'$' unambiguous_variable_name { if (GLOBAL(Execute)) get_regular_variable_contents(&$$,&$2,1 _INLINE_TLS); }
	|	varname_scalar { if (GLOBAL(Execute)) {$$ = $1;COPY_STRING($$);} }
	|	'$' multi_dimensional_array {  if (GLOBAL(Execute)) read_pointer_value(&$$,&$2 _INLINE_TLS); }
;

multi_dimensional_array:
		unambiguous_array_name '[' { start_array_parsing(&$1,NULL _INLINE_TLS); } dimensions { end_array_parsing(&$$,&$4 _INLINE_TLS); }
;


dimensions:
		dimensions '[' expr ']' { fetch_array_index(&$$,&$3,&$1 _INLINE_TLS); }
	|	dimensions '[' ']' { fetch_array_index(&$$,NULL,&$1 _INLINE_TLS); }
	|	{ start_dimensions_parsing(&$$ _INLINE_TLS); } ']' { fetch_array_index(&$$,NULL,&$$ _INLINE_TLS); }
	|	{ start_dimensions_parsing(&$$ _INLINE_TLS); } expr ']' { fetch_array_index(&$$,&$2,&$$ _INLINE_TLS); }
;


array_pair_list:
		/* empty */ { if (GLOBAL(Execute)) { $$.value.ht = (HashTable *) emalloc(sizeof(HashTable));  hash_init($$.value.ht,0,NULL,YYSTYPE_DESTRUCTOR,0); $$.type = IS_ARRAY; } }
	|	non_empty_array_pair_list { $$ = $1; }
;

non_empty_array_pair_list:
		non_empty_array_pair_list ',' expr PHP_DOUBLE_ARROW expr { if (GLOBAL(Execute)) {$$=$1; add_array_pair_list(&$$, &$3, &$5, 0 _INLINE_TLS);} }
	|	non_empty_array_pair_list ',' expr { if (GLOBAL(Execute)) {$$=$1; add_array_pair_list(&$$, NULL, &$3, 0 _INLINE_TLS);} }
	|	expr PHP_DOUBLE_ARROW expr { if (GLOBAL(Execute)) add_array_pair_list(&$$, &$1, &$3, 1 _INLINE_TLS); }
	|	expr { if (GLOBAL(Execute)) add_array_pair_list(&$$, NULL, &$1, 1 _INLINE_TLS); }
;


string:		
		STRING { if (GLOBAL(Execute)) { COPY_STRING($1); $$ = $1; } }
	|	'"' encaps_list '"' { $$ = $2; }
	|	'\'' encaps_list '\'' { $$ = $2; }
;


encaps_list:
		encaps_list '$' STRING { add_regular_encapsed_variable(&$$,&$1,&$3 _INLINE_TLS); }
	|	encaps_list '$' STRING '[' STRING ']' { add_assoc_array_encapsed_variable(&$$,&$1,&$3,&$5 _INLINE_TLS); }
	|	encaps_list '$' STRING '[' NUM_STRING ']' { add_regular_array_encapsed_variable(&$$,&$1,&$3,&$5 _INLINE_TLS); }
	|	encaps_list '$' STRING '[' '$' STRING ']' { add_variable_array_encapsed_variable(&$$,&$1,&$3,&$6 _INLINE_TLS); }
	|	encaps_list '$' STRING PHP_CLASS_OPERATOR STRING { add_encapsed_object_property(&$$,&$1,&$3,&$5 _INLINE_TLS); }
	|	encaps_list '$' '{' STRING '}' { add_regular_encapsed_variable(&$$,&$1,&$4 _INLINE_TLS); }
	|	encaps_list '$' '{' STRING '[' STRING ']' '}' { add_assoc_array_encapsed_variable(&$$,&$1,&$4,&$6 _INLINE_TLS); }
	|	encaps_list '$' '{' STRING '[' NUM_STRING ']' '}' { add_regular_array_encapsed_variable(&$$,&$1,&$4,&$6 _INLINE_TLS); }
	|	encaps_list '$' '{' STRING '[' '$' STRING ']' '}' { add_variable_array_encapsed_variable(&$$,&$1,&$4,&$7 _INLINE_TLS); }
	|	encaps_list '$' '{' STRING PHP_CLASS_OPERATOR STRING '}' { add_encapsed_object_property(&$$,&$1,&$4,&$6 _INLINE_TLS); }
	|	encaps_list '$' '{' '$' STRING '}' { add_indirect_encapsed_variable(&$$,&$1,&$5 _INLINE_TLS); }
	|	encaps_list STRING	{ if (GLOBAL(Execute)) { concat_function(&$$,&$1,&$2,0 _INLINE_TLS); } }
	|	encaps_list NUM_STRING	{ if (GLOBAL(Execute)) { concat_function(&$$,&$1,&$2,0 _INLINE_TLS); } }
	|	encaps_list ENCAPSED_AND_WHITESPACE { if (GLOBAL(Execute)) { concat_function(&$$,&$1,&$2,0 _INLINE_TLS); } }
	|	encaps_list CHARACTER { if (GLOBAL(Execute)) add_char_to_string(&$$,&$1,&$2 _INLINE_TLS); }
	|	encaps_list BAD_CHARACTER { if (GLOBAL(Execute)) { php3_error(E_NOTICE,"Bad escape sequence:  %s",$2.value.strval); concat_function(&$$,&$1,&$2,0 _INLINE_TLS); } }
	|	encaps_list '['		{ if (GLOBAL(Execute)) add_char_to_string(&$$,&$1,&$2 _INLINE_TLS); }
	|	encaps_list ']'		{ if (GLOBAL(Execute)) add_char_to_string(&$$,&$1,&$2 _INLINE_TLS); }
	|	encaps_list '{'	{ if (GLOBAL(Execute)) add_char_to_string(&$$,&$1,&$2 _INLINE_TLS); }
	|	encaps_list '}'	{ if (GLOBAL(Execute)) add_char_to_string(&$$,&$1,&$2 _INLINE_TLS); }
	|	encaps_list PHP_CLASS_OPERATOR {  if (GLOBAL(Execute)) { YYSTYPE tmp;  tmp.value.strval="->"; tmp.strlen=2; tmp.type=IS_STRING; concat_function(&$$,&$1,&tmp,0 _INLINE_TLS); } }
	|	/* empty */			{ if (GLOBAL(Execute)) var_reset(&$$); }
;


internal_functions_in_yacc:
		PHP_UNSET '(' assignment_variable_pointer ')' { php3_unset(&$$, &$3); }
	|	PHP_ISSET '(' assignment_variable_pointer ')' { php3_isset(&$$, &$3); }
;


possible_function_call:
		/* empty */
	|	OLD_FUNCTION STRING parameter_list '(' statement_list ')' ';'
	|	FUNCTION STRING '(' parameter_list ')' '{' statement_list '}'
;

%%
