%{

/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Andi Gutmans <andi@vipe.technion.ac.il>                     |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


/* $Id: language-parser.y,v 1.12 2000/02/07 23:54:50 zeev Exp $ */

#include "token_cache.h"
#include "main.h"
#if WIN32|WINNT
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#endif
extern void phperror(char *str);
extern int phplex(pval *phplval);
extern void end_php();

%}

%pure_parser
%expect 2

%left ','
%left LOGICAL_OR
%left LOGICAL_AND
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
%token CHAR
%token ENCAPSED_AND_WHITESPACE
%token PHPECHO
%token PHPHEADER
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
%token FUNCTION
%token RETURN
%token INCLUDE
%token _INCLUDE
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
%token END_PHP3


%% /* Rules */

statement_list:	
		statement_list statement
	|	
;


statement:
		'{' statement_list '}'
	|	IF '(' expr_with_gt ')' '{' statement_list '}'  elseif_list else_single
	|	IF '(' expr_with_gt ')' end_statement_2 statement_list new_elseif_list new_else_single ENDIF end_statement
	|	WHILE '(' expr_with_gt ')' '{' statement_list '}'
	|	WHILE '(' expr_with_gt ')'  end_statement_2 statement_list ENDWHILE end_statement
	|	SWITCH '(' expr_with_gt ')' '{' case_list '}'
	|	SWITCH '(' expr_with_gt ')' end_statement_2 case_list ENDSWITCH end_statement
	|	CONTINUE end_statement
	|	CONTINUE expr end_statement
	|	BREAK end_statement
	|	BREAK expr end_statement
	|	FUNCTION STRING parameter_list '(' statement_list ')' end_statement
	|	RETURN end_statement
	|	RETURN expr end_statement
	|	PHP_GLOBAL global_var_list end_statement
	|	PHP_STATIC static_var_list end_statement
	|	PHPECHO echo_expr_list end_statement
	|	PHPHEADER expr end_statement
	|	INLINE_HTML
	|	expr end_statement
	|	EVAL '(' var ')' end_statement
	|	INCLUDE expr end_statement
	| 	'>'   { end_php(); printf("?>"); }
	|	END_PHP3 { end_php(); }
;


end_statement:
		';'  { printf(";"); }
	|	'>'  { end_php(); printf("?>"); }
	|	END_PHP3 { end_php(); }
;

end_statement_2:
		';'	 { printf(":"); }
	|	'>'	{ end_php(); printf(": ?>"); }
	|	END_PHP3 { end_php(); }
;


elseif_list:
	|	elseif_list ELSEIF '(' expr_with_gt ')' '{' statement_list '}'
;


new_elseif_list:
	|	new_elseif_list ELSEIF '(' expr_with_gt ')' end_statement_2 statement_list
;


else_single:
	|	ELSE '{' statement_list '}'
;


new_else_single:
	|	ELSE end_statement_2 statement_list
;


case_list:
	|	CASE expr end_statement_2 statement_list case_list
	|	DEFAULT end_statement_2 statement_list
;


parameter_list: 
		non_empty_parameter_list
	|	
;


non_empty_parameter_list:
		'$' STRING
	|	non_empty_parameter_list ',' '$' STRING
;


function_call_parameter_list:
		non_empty_function_call_parameter_list
	|	
;


non_empty_function_call_parameter_list:
		expr_without_variable
	|	assignment_variable_pointer
	|	'&' assignment_variable_pointer
	|	non_empty_function_call_parameter_list ',' expr_without_variable
	|	non_empty_function_call_parameter_list ',' assignment_variable_pointer
	|	non_empty_function_call_parameter_list ',' '&' assignment_variable_pointer
;

global_var_list:
		global_var_list ',' '$' var
	|	'$' var
;


static_var_list:
		static_var_list ',' '$' var
	|	static_var_list ',' '$' var '=' unambiguous_static_assignment
	|	'$' var
	|	'$' var '=' unambiguous_static_assignment

;


unambiguous_static_assignment:
		numeric_value
	|	string
	|	'(' expr_with_gt ')'
;


echo_expr_list:	
		
	|	echo_expr_list ',' expr
	|	expr
;


expr_without_variable:	
		assignment_variable_pointer '=' expr
	|	assignment_variable_pointer PLUS_EQUAL expr
	|	assignment_variable_pointer	MINUS_EQUAL expr
	|	assignment_variable_pointer	MUL_EQUAL expr
	|	assignment_variable_pointer DIV_EQUAL expr
	|	assignment_variable_pointer	CONCAT_EQUAL expr
	|	assignment_variable_pointer MOD_EQUAL expr
	|	assignment_variable_pointer	AND_EQUAL expr
	|	assignment_variable_pointer	OR_EQUAL expr
	|	assignment_variable_pointer	XOR_EQUAL expr
	|	assignment_variable_pointer  INCREMENT
	|	INCREMENT assignment_variable_pointer
	|	assignment_variable_pointer DECREMENT
	|	DECREMENT assignment_variable_pointer
	|	expr BOOLEAN_OR expr
	|	expr BOOLEAN_AND expr
	|	expr LOGICAL_OR expr
	|	expr LOGICAL_AND expr
	|	expr '|' expr
	|	expr '^' expr
	|	expr '&' expr
	|	expr '.' expr
	|	expr '+' expr
	|	expr '-' expr
	|	expr '*' expr
	|	expr '/' expr
	|	expr '%' expr
	|	'+' expr
	|	'-' expr
	|	'!' expr
	|	'~' expr
	|	expr IS_EQUAL expr
	|	expr IS_NOT_EQUAL expr
	|	expr '<' expr
	|	expr IS_SMALLER_OR_EQUAL expr
	|	expr IS_GREATER_OR_EQUAL expr
	|	'(' expr_with_gt ')'
	|	numeric_value
	|	var '(' function_call_parameter_list ')' possible_function_call
	|	internal_functions_in_yacc
	|	NEW STRING
	|	EXIT
	|	EXIT '(' ')'
	|	EXIT '(' expr_with_gt ')'
	|	'@' expr
	|   string
;

expr_with_gt:
		expr
	|	expr '>' { printf(">"); } expr 
;

numeric_value:
		LNUMBER
	|	DNUMBER
;


expr:
		assignment_variable_pointer
	|	expr_without_variable
;


unambiguous_variable_name:
		var
	|	'{' expr '}'
;


unambiguous_array_name:
		string
	|	'{' expr '}'

;


assignment_variable_pointer:
		'$' unambiguous_variable_name
	|	'$' multi_dimensional_array
;


var:	
		'$' unambiguous_variable_name
	|	string
	|	'$' multi_dimensional_array
;

multi_dimensional_array:
		unambiguous_array_name '[' dimensions
;


dimensions:
		dimensions '[' expr ']'
	|	dimensions '[' ']'
	|	']'
	|	expr ']'
;


string:		
		STRING
	|	'"' encaps_list '"'
	|	'\'' encaps_list '\''
;


encaps_list:
		encaps_list '$' STRING 
	|	encaps_list NUM_STRING
	|	encaps_list ENCAPSED_AND_WHITESPACE
	|	encaps_list CHAR
	|	encaps_list STRING
	|	encaps_list '['
	|	encaps_list ']'
	|	encaps_list '\\'
	|	encaps_list PHP_CLASS_OPERATOR
	|	/* empty */
;


internal_functions_in_yacc:
		PHP_UNSET '(' assignment_variable_pointer ')'
	|	PHP_ISSET '(' assignment_variable_pointer ')'
;


possible_function_call:
	|	FUNCTION STRING parameter_list '(' statement_list ')' end_statement
;

%%
