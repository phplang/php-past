
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


/* $Id: operators.h,v 1.24 1998/02/01 01:27:12 zeev Exp $ */


#ifndef _OPERATORS_H
#define _OPERATORS_H

#include "parser.h"

#define MAX_LENGTH_OF_LONG 11
#define MAX_LENGTH_OF_DOUBLE 14

extern int add_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int sub_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int mul_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int div_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int mod_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int boolean_or_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2);
extern int boolean_and_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2);
extern int boolean_not_function(YYSTYPE *result, YYSTYPE *op1);
extern int bitwise_not_function(YYSTYPE *result, YYSTYPE *op1 INLINE_TLS);
extern int bitwise_or_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int bitwise_and_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int bitwise_xor_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int concat_function_with_free(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int concat_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2, int free_op2 INLINE_TLS);

extern int is_equal_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int is_not_equal_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int is_smaller_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int is_smaller_or_equal_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int is_greater_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern int is_greater_or_equal_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);

extern int increment_function(YYSTYPE *op1);
extern int decrement_function(YYSTYPE *op2);

extern void convert_string_to_number(YYSTYPE *op);
extern PHPAPI void convert_to_string(YYSTYPE *op);
extern void convert_double_to_long(YYSTYPE *op);
extern PHPAPI void convert_to_long(YYSTYPE *op);
extern PHPAPI void convert_to_long_base(YYSTYPE *op, int base);
extern PHPAPI void convert_to_boolean_long(YYSTYPE *op);
extern PHPAPI void convert_to_array(YYSTYPE *op);
extern PHPAPI void convert_to_object(YYSTYPE *op);
extern int add_char_to_string(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);
extern PHPAPI void convert_to_double(YYSTYPE *op);

extern int yystype_true(YYSTYPE *op);
extern int compare_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS);

extern void php3_str_tolower(char *str, unsigned int length);

#endif
