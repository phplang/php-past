
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


/* $Id: operators.h,v 1.40 2000/04/10 19:29:36 andi Exp $ */


#ifndef _OPERATORS_H
#define _OPERATORS_H

#include "php.h"

#define MAX_LENGTH_OF_LONG 18
#define MAX_LENGTH_OF_DOUBLE 32

extern int add_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int sub_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int mul_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int div_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int mod_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int boolean_or_function(pval *result, pval *op1, pval *op2);
extern int boolean_xor_function(pval *result, pval *op1, pval *op2);
extern int boolean_and_function(pval *result, pval *op1, pval *op2);
extern int boolean_not_function(pval *result, pval *op1);
extern int bitwise_not_function(pval *result, pval *op1 INLINE_TLS);
extern int bitwise_or_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int bitwise_and_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int bitwise_xor_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int shift_left_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int shift_right_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int concat_function_with_free(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int concat_function(pval *result, pval *op1, pval *op2, int free_op2 INLINE_TLS);

extern int is_equal_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int is_not_equal_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int is_smaller_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int is_smaller_or_equal_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int is_greater_function(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern int is_greater_or_equal_function(pval *result, pval *op1, pval *op2 INLINE_TLS);

extern int increment_function(pval *op1);
extern int decrement_function(pval *op2);

extern void convert_string_to_number(pval *op);
extern PHPAPI void convert_to_string(pval *op);
extern void convert_double_to_long(pval *op);
extern PHPAPI void convert_to_long(pval *op);
extern PHPAPI void convert_to_long_base(pval *op, int base);
extern PHPAPI void convert_to_boolean_long(pval *op);
extern PHPAPI void convert_to_array(pval *op);
extern PHPAPI void convert_to_object(pval *op);
extern int add_char_to_string(pval *result, pval *op1, pval *op2 INLINE_TLS);
extern PHPAPI void convert_to_double(pval *op);

extern int pval_is_true(pval *op);
extern int compare_function(pval *result, pval *op1, pval *op2 INLINE_TLS);

extern void php3_str_tolower(char *str, unsigned int length);
extern int php3_binary_strcasecmp(pval *s1, pval *s2);
extern int php3_binary_strcmp(pval *s1, pval *s2);
extern inline void php3_smart_strcmp(pval *result, pval *s1, pval *s2);

#endif
