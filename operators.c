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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: operators.c,v 1.90 1998/06/05 07:11:00 andi Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"
#include "functions/number.h"
#include <stdio.h>
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include "snprintf.h"
#include <errno.h>

static inline int is_numeric_string(char *str, int length, long *lval, double *dval);

void convert_double_to_long(pval *op)
{
	if (op->type == IS_DOUBLE) {
		op->value.lval = (long) op->value.dval;
		op->type = IS_LONG;
	}
}


void convert_string_to_number(pval *op)
{
	char *strval;

	if (op->type == IS_STRING) {
		strval = op->value.str.val;
		switch ((op->type=is_numeric_string(strval, op->value.str.len, &op->value.lval, &op->value.dval))) {
			case IS_DOUBLE:
			case IS_LONG:
				break;
			case IS_BC:
				op->type = IS_DOUBLE; /* may have lost significant digits */
				break;
			default:
				op->value.lval = strtol(op->value.str.val, NULL, 10);
				op->type = IS_LONG;
				break;
		}
		STR_FREE(strval);
	}
}

PHPAPI void convert_to_long(pval *op)
{
	convert_to_long_base(op, 10);
}

PHPAPI void convert_to_long_base(pval *op, int base)
{
	char *strval;
	long tmp;
	TLS_VARS;

	switch (op->type) {
		case IS_LONG:
			return;
		case IS_DOUBLE:
			op->value.lval = (long) op->value.dval;
			op->type = IS_LONG;
			break;
		case IS_STRING:
			strval = op->value.str.val;
			op->value.lval = strtol(strval, NULL, base);
			op->type = IS_LONG;
			STR_FREE(strval);
			break;
		case IS_ARRAY:
		case IS_OBJECT:
			tmp = (hash_num_elements(op->value.ht)?1:0);
			yystype_destructor(op _INLINE_TLS);
			op->value.lval = tmp;
			op->type = IS_LONG;
			break;
		default:
			php3_error(E_WARNING, "Cannot convert to ordinal value");
			yystype_destructor(op _INLINE_TLS);
			op->value.lval = 0;
			op->type = IS_LONG;
			break;
	}

	op->type = IS_LONG;
}


PHPAPI void convert_to_double(pval *op)
{
	char *strval;
	double tmp;
	TLS_VARS;

	switch (op->type) {
		case IS_LONG:
			op->value.dval = (double) op->value.lval;
			op->type = IS_DOUBLE;
			break;
		case IS_DOUBLE:
			break;
		case IS_STRING:
			strval = op->value.str.val;

			op->value.dval = strtod(strval, NULL);
			op->type = IS_DOUBLE;
			STR_FREE(strval);
			break;
		case IS_ARRAY:
		case IS_OBJECT:
			tmp = (hash_num_elements(op->value.ht)?1:0);
			yystype_destructor(op _INLINE_TLS);
			op->value.dval = tmp;
			op->type = IS_DOUBLE;
			break;
		default:
			php3_error(E_WARNING, "Cannot convert to real value");
			yystype_destructor(op _INLINE_TLS);
			op->value.dval = 0;
			op->type = IS_DOUBLE;
			break;
	}
}


PHPAPI void convert_to_boolean_long(pval *op)
{
	char *strval;
	int tmp;
	TLS_VARS;

	switch (op->type) {
		case IS_LONG:
			break;
		case IS_DOUBLE:
			op->value.lval = (op->value.dval ? 1 : 0);
			op->type = IS_LONG;
			break;
		case IS_STRING:
			strval = op->value.str.val;

			if (op->value.str.len == 0
				|| (op->value.str.len==1 && op->value.str.val[0]=='0')) {
				op->value.lval = 0;
			} else {
				op->value.lval = 1;
			}
			STR_FREE(strval);
			op->type = IS_LONG;
			break;
		case IS_ARRAY:
		case IS_OBJECT:
			tmp = (hash_num_elements(op->value.ht)?1:0);
			yystype_destructor(op _INLINE_TLS);
			op->value.lval = tmp;
			op->type = IS_LONG;
			break;
		default:
			yystype_destructor(op _INLINE_TLS);
			op->value.lval = 0;
			op->type = IS_LONG;
			break;
	}
}


PHPAPI void convert_to_string(pval *op)
{
	long lval;
	double dval;
	TLS_VARS;

	switch (op->type) {
		case IS_STRING:
			break;
		case IS_LONG:
			lval = op->value.lval;

			op->value.str.val = (char *) emalloc(MAX_LENGTH_OF_LONG + 1);
			if (!op->value.str.val) {
				return;
			}
			op->value.str.len = _php3_sprintf(op->value.str.val, "%ld", lval);  /* SAFE */
			op->type = IS_STRING;
			break;
		case IS_DOUBLE: {
			dval = op->value.dval;
			op->value.str.val = (char *) emalloc(MAX_LENGTH_OF_DOUBLE + php3_ini.precision + 1);
			if (!op->value.str.val) {
				return;
			}
			op->value.str.len = _php3_sprintf(op->value.str.val, "%.*G", (int) php3_ini.precision, dval);  /* SAFE */
			/* %G already handles removing trailing zeros from the fractional part, yay */
			op->type = IS_STRING;
			break;
		}
		case IS_ARRAY:
			yystype_destructor(op _INLINE_TLS);
			op->value.str.val = estrndup("Array",sizeof("Array")-1);
			op->value.str.len = sizeof("Array")-1;
			op->type = IS_STRING;
			break;
		case IS_OBJECT:
			yystype_destructor(op _INLINE_TLS);
			op->value.str.val = estrndup("Object",sizeof("Object")-1);
			op->value.str.len = sizeof("Object")-1;
			op->type = IS_STRING;
			break;
		default:
			yystype_destructor(op _INLINE_TLS);
			var_reset(op);
			break;
	}
}


static void convert_scalar_to_array(pval *op,int type)
{
	pval tmp = *op;
	
	op->value.ht = (HashTable *) emalloc(sizeof(HashTable));
	hash_init(op->value.ht, 0, NULL, pval_DESTRUCTOR, 0);
	switch (type) {
		case IS_ARRAY:
			hash_index_update(op->value.ht, 0, (void *) &tmp, sizeof(pval), NULL);
			op->type = IS_ARRAY;
			break;
		case IS_OBJECT:
			hash_update(op->value.ht, "scalar", sizeof("scalar"), (void *) &tmp, sizeof(pval), NULL);
			op->type = IS_OBJECT;
			break;
	}
}


PHPAPI void convert_to_array(pval *op)
{
	switch(op->type) {
		case IS_ARRAY:
			return;
			break;
		case IS_OBJECT:
			op->type = IS_ARRAY;
			return;
			break;
		default:
			convert_scalar_to_array(op,IS_ARRAY);
			break;
	}
}


PHPAPI void convert_to_object(pval *op)
{
	switch(op->type) {
		case IS_ARRAY:
			op->type = IS_OBJECT;
			return;
			break;
		case IS_OBJECT:
			return;
			break;
		default:
			convert_scalar_to_array(op,IS_OBJECT);
			op->type = IS_OBJECT;
			break;
	}
}

		
int add_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (op1->type == IS_ARRAY && op2->type == IS_ARRAY) {
		pval tmp;
		
		hash_merge(op1->value.ht,op2->value.ht,(void (*)(void *pData)) yystype_copy_constructor, (void *) &tmp, sizeof(pval));
		*result = *op1;
		yystype_destructor(op2 _INLINE_TLS);
		return SUCCESS;
	}
	if (php3_ini.warn_plus_overloading) {
		if ((op1->type == IS_STRING && !is_numeric_string(op1->value.str.val, op1->value.str.len, NULL, NULL))
			|| (op2->type == IS_STRING && !is_numeric_string(op2->value.str.val, op2->value.str.len, NULL, NULL))) {
			php3_error(E_NOTICE,"Using plus operator on string operands");
		}
	}
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		double dval = (double) op1->value.lval + (double) op2->value.lval;

		if (dval > (double) LONG_MAX) {
			result->value.dval = dval;
			result->type = IS_DOUBLE;
		} else {
			result->value.lval = op1->value.lval + op2->value.lval;
			result->type = IS_LONG;
		}
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_DOUBLE)) {
		result->type = IS_DOUBLE;
		result->value.dval = (op1->type == IS_LONG ?
						 (((double) op1->value.lval) + op2->value.dval) :
						 (op1->value.dval + ((double) op2->value.lval)));
		return SUCCESS;
	}
	if (op1->type == IS_DOUBLE && op2->type == IS_DOUBLE) {
		result->type = IS_DOUBLE;
		result->value.dval = op1->value.dval + op2->value.dval;
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	yystype_destructor(op2 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}


int sub_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		double dval = (double) op1->value.lval - (double) op2->value.lval;

		if (dval < (double) LONG_MIN) {
			result->value.dval = dval;
			result->type = IS_DOUBLE;
		} else {
			result->value.lval = op1->value.lval - op2->value.lval;
			result->type = IS_LONG;
		}
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_DOUBLE)) {
		result->type = IS_DOUBLE;
		result->value.dval = (op1->type == IS_LONG ?
						 (((double) op1->value.lval) - op2->value.dval) :
						 (op1->value.dval - ((double) op2->value.lval)));
		return SUCCESS;
	}
	if (op1->type == IS_DOUBLE && op2->type == IS_DOUBLE) {
		result->type = IS_DOUBLE;
		result->value.dval = op1->value.dval - op2->value.dval;
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	yystype_destructor(op2 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}


int mul_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		double dval = (double) op1->value.lval * (double) op2->value.lval;

		if (dval > (double) LONG_MAX) {
			result->value.dval = dval;
			result->type = IS_DOUBLE;
		} else {
			result->value.lval = op1->value.lval * op2->value.lval;
			result->type = IS_LONG;
		}
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_DOUBLE)) {
		result->type = IS_DOUBLE;
		result->value.dval = (op1->type == IS_LONG ?
						 (((double) op1->value.lval) * op2->value.dval) :
						 (op1->value.dval * ((double) op2->value.lval)));
		return SUCCESS;
	}
	if (op1->type == IS_DOUBLE && op2->type == IS_DOUBLE) {
		result->type = IS_DOUBLE;
		result->value.dval = op1->value.dval * op2->value.dval;
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	yystype_destructor(op2 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}

int div_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if ((op2->type == IS_LONG && op2->value.lval == 0) || (op2->type == IS_DOUBLE && op2->value.dval == 0.0)) {
		php3_error(E_WARNING, "Division by zero");
		var_reset(result);
		return FAILURE;			/* division by zero */
	}
	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		if (op1->value.lval % op2->value.lval == 0) { /* integer */
			result->type = IS_LONG;
			result->value.lval = op1->value.lval / op2->value.lval;
		} else {
			result->type = IS_DOUBLE;
			result->value.dval = ((double) op1->value.lval) / op2->value.lval;
		}
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_DOUBLE)) {
		result->type = IS_DOUBLE;
		result->value.dval = (op1->type == IS_LONG ?
						 (((double) op1->value.lval) / op2->value.dval) :
						 (op1->value.dval / ((double) op2->value.lval)));
		return SUCCESS;
	}
	if (op1->type == IS_DOUBLE && op2->type == IS_DOUBLE) {
		result->type = IS_DOUBLE;
		result->value.dval = op1->value.dval / op2->value.dval;
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	yystype_destructor(op2 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}

int mod_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	convert_to_long(op1);
	convert_to_long(op2);

	if (op2->value.lval == 0) {
		var_reset(result);
		return FAILURE;			/* modulus by zero */
	}

	result->type = IS_LONG;
	result->value.lval = op1->value.lval % op2->value.lval;
	return SUCCESS;
}

int boolean_or_function(pval *result, pval *op1, pval *op2)
{
	result->type = IS_LONG;

	convert_to_boolean_long(op1);
	if (op1->type == IS_LONG && op1->value.lval) {
		result->value.lval = 1;
		return SUCCESS;
	}
	convert_to_boolean_long(op2);
	if (op2->type == IS_LONG && op2->value.lval) {
		result->value.lval = 1;
		return SUCCESS;
	}
	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->value.lval = 0;
		return SUCCESS;
	}
	/* some error converting the variables */
	var_reset(result);
	return FAILURE;
}


int boolean_and_function(pval *result, pval *op1, pval *op2)
{
	result->type = IS_LONG;

	convert_to_boolean_long(op1);
	if (op1->type == IS_LONG && !op1->value.lval) {
		result->value.lval = 0;
		return SUCCESS;
	}
	convert_to_boolean_long(op2);
	if (op2->type == IS_LONG && !op2->value.lval) {
		result->value.lval = 0;
		return SUCCESS;
	}
	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->value.lval = 1;
		return SUCCESS;
	} 
	/* some error converting the variables */
	var_reset(result);
	return FAILURE;
}


int boolean_xor_function(pval *result, pval *op1, pval *op2)
{
	result->type = IS_LONG;

	convert_to_boolean_long(op1);
	convert_to_boolean_long(op2);
	result->value.lval = op1->value.lval ^ op2->value.lval;
	return SUCCESS;
}


int boolean_not_function(pval *result, pval *op1)
{
	convert_to_boolean_long(op1);

	if (op1->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = !op1->value.lval;
		return SUCCESS;
	}
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}


int bitwise_not_function(pval *result, pval *op1 INLINE_TLS)
{
	if (op1->type == IS_DOUBLE) {
		op1->value.lval = (long) op1->value.dval;
		op1->type = IS_LONG;
	}
	if (op1->type == IS_LONG) {
		result->value.lval = ~op1->value.lval;
		result->type = IS_LONG;
		return SUCCESS;
	}
	if (op1->type == IS_STRING) {
		int i;

		result->type = IS_STRING;
		result->value.str.val = op1->value.str.val;
		result->value.str.len = op1->value.str.len;
		for (i = 0; i < op1->value.str.len; i++) {
			result->value.str.val[i] = ~op1->value.str.val[i];
		}
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}


int bitwise_or_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		pval *longer, *shorter;
		int i;

		if (op1->value.str.len >= op2->value.str.len) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		result->value.str.len = longer->value.str.len;
		result->value.str.val = longer->value.str.val;
		for (i = 0; i < shorter->value.str.len; i++) {
			result->value.str.val[i] |= shorter->value.str.val[i];
		}
		STR_FREE(shorter->value.str.val);
		return SUCCESS;
	}
	convert_to_long(op1);
	convert_to_long(op2);

	result->type = IS_LONG;
	result->value.lval = op1->value.lval | op2->value.lval;
	return SUCCESS;
}


int bitwise_and_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		pval *longer, *shorter;
		int i;

		if (op1->value.str.len >= op2->value.str.len) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		result->value.str.len = shorter->value.str.len;
		result->value.str.val = shorter->value.str.val;
		for (i = 0; i < shorter->value.str.len; i++) {
			result->value.str.val[i] &= longer->value.str.val[i];
		}
		STR_FREE(longer->value.str.val);
		return SUCCESS;
	}
	

	convert_to_long(op1);
	convert_to_long(op2);

	result->type = IS_LONG;
	result->value.lval = op1->value.lval & op2->value.lval;
	return SUCCESS;
}


int bitwise_xor_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		pval *longer, *shorter;
		int i;

		if (op1->value.str.len >= op2->value.str.len) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		result->value.str.len = shorter->value.str.len;
		result->value.str.val = shorter->value.str.val;
		for (i = 0; i < shorter->value.str.len; i++) {
			result->value.str.val[i] ^= longer->value.str.val[i];
		}
		STR_FREE(longer->value.str.val);
		return SUCCESS;
	}

	convert_to_long(op1);	
	convert_to_long(op2);	

	result->type = IS_LONG;
	result->value.lval = op1->value.lval ^ op2->value.lval;
	return SUCCESS;
}

int add_char_to_string(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (op1->type != IS_STRING) {
		yystype_destructor(op1 _INLINE_TLS);
		yystype_destructor(op2 _INLINE_TLS);
		var_reset(result);
		return FAILURE;
	}
	result->value.str.len = op1->value.str.len + 1;
	result->value.str.val = (char *) emalloc(result->value.str.len + 1);
	memcpy(result->value.str.val, op1->value.str.val, op1->value.str.len);
	result->value.str.val[result->value.str.len - 1] = op2->value.chval;
	result->value.str.val[result->value.str.len] = 0;
	result->type = IS_STRING;
	STR_FREE(op1->value.str.val);

	return SUCCESS;
}

int concat_function(pval *result, pval *op1, pval *op2, int free_op2 INLINE_TLS)
{
	convert_to_string(op1);
	convert_to_string(op2);

	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		result->value.str.len = op1->value.str.len + op2->value.str.len;
		if (op1->value.str.len == 0) { /* Takes care of empty_string etc. cases where we can't erealloc() */
			result->value.str.val = (char *) emalloc(result->value.str.len + 1);
			STR_FREE(op1->value.str.val);
		} else {
			result->value.str.val = (char *) erealloc(op1->value.str.val, result->value.str.len + 1);
		}
		memcpy(result->value.str.val+op1->value.str.len, op2->value.str.val,op2->value.str.len);
		result->value.str.val[result->value.str.len] = 0;
		result->type = IS_STRING;
		if (free_op2) {
			STR_FREE(op2->value.str.val);
		}
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	if (free_op2) {
		yystype_destructor(op2 _INLINE_TLS);
	}
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}

int concat_function_with_free(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	return concat_function(result,op1,op2,1 _INLINE_TLS);
}

int compare_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		php3_smart_strcmp(result,op1,op2);
		return SUCCESS;
	}
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval - op2->value.lval;
		return SUCCESS;
	}
	if ((op1->type == IS_DOUBLE || op1->type == IS_LONG)
		&& (op2->type == IS_DOUBLE || op2->type == IS_LONG)) {
		result->type = IS_DOUBLE;
		result->value.dval = (op1->type == IS_LONG ? (double) op1->value.lval : op1->value.dval) - (op2->type == IS_LONG ? (double) op2->value.lval : op2->value.dval);
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	yystype_destructor(op2 _INLINE_TLS);
	var_reset(result);
	return FAILURE;
}


int is_equal_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (compare_function(result, op1, op2 _INLINE_TLS) == FAILURE) {
		return FAILURE;
	}
	convert_to_boolean_long(result);
	if (result->type == IS_LONG) {
		if (result->value.lval == 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	var_reset(result);
	return FAILURE;
}


int is_not_equal_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (compare_function(result, op1, op2 _INLINE_TLS) == FAILURE) {
		return FAILURE;
	}
	convert_to_boolean_long(result);
	if (result->type == IS_LONG) {
		if (result->value.lval) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	var_reset(result);
	return FAILURE;
}


int is_smaller_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (compare_function(result, op1, op2 _INLINE_TLS) == FAILURE) {
		return FAILURE;
	}
	if (result->type == IS_LONG) {
		if (result->value.lval < 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	if (result->type == IS_DOUBLE) {
		result->type = IS_LONG;
		if (result->value.dval < 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	var_reset(result);
	return FAILURE;
}


int is_smaller_or_equal_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (compare_function(result, op1, op2 _INLINE_TLS) == FAILURE) {
		return FAILURE;
	}
	if (result->type == IS_LONG) {
		if (result->value.lval <= 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	if (result->type == IS_DOUBLE) {
		result->type = IS_LONG;
		if (result->value.dval <= 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	var_reset(result);
	return FAILURE;
}

int is_greater_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (compare_function(result, op1, op2 _INLINE_TLS) == FAILURE) {
		return FAILURE;
	}
	if (result->type == IS_LONG) {
		if (result->value.lval > 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	if (result->type == IS_DOUBLE) {
		result->type = IS_LONG;
		if (result->value.dval > 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	var_reset(result);
	return FAILURE;
}


int is_greater_or_equal_function(pval *result, pval *op1, pval *op2 INLINE_TLS)
{
	if (compare_function(result, op1, op2 _INLINE_TLS) == FAILURE) {
		return FAILURE;
	}
	if (result->type == IS_LONG) {
		if (result->value.lval >= 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	if (result->type == IS_DOUBLE) {
		result->type = IS_LONG;
		if (result->value.dval >= 0) {
			result->value.lval = 1;
		} else {
			result->value.lval = 0;
		}
		return SUCCESS;
	}
	var_reset(result);
	return FAILURE;
}

#define LOWER_CASE 1
#define UPPER_CASE 2
#define NUMERIC 3

static void increment_string(pval *str)
{
    int carry=0;
    int pos=str->value.str.len-1;
    char *s=str->value.str.val;
    char *t;
    int last=0; /* Shut up the compiler warning */
    int ch;
    
	while(pos >= 0) {
        ch = s[pos];
        if (ch >= 'a' && ch <= 'z') {
            if (ch == 'z') {
                s[pos] = 'a';
                carry=1;
            } else {
                s[pos]++;
                carry=0;
            }
            last=LOWER_CASE;
        } else if (ch >= 'A' && ch <= 'Z') {
            if (ch == 'Z') {
                s[pos] = 'A';
                carry=1;
            } else {
                s[pos]++;
                carry=0;
            }
            last=UPPER_CASE;
        } else if (ch >= '0' && ch <= '9') {
            if (ch == '9') {
                s[pos] = '0';
                carry=1;
            } else {
                s[pos]++;
                carry=0;
            }
            last = NUMERIC;
        } else {
            carry=0;
            break;
        }
        if (carry == 0) {
            break;
        }
        pos--;
    }

    if (carry) {
        t = (char *) emalloc(str->value.str.len+1+1);
        memcpy(t+1,str->value.str.val, str->value.str.len);
        str->value.str.len++;
        t[str->value.str.len] = '\0';
        switch (last) {
            case NUMERIC:
            	t[0] = '1';
            	break;
            case UPPER_CASE:
            	t[0] = 'A';
            	break;
            case LOWER_CASE:
            	t[0] = 'a';
            	break;
        }
        STR_FREE(str->value.str.val);
        str->value.str.val = t;
    }
}


int increment_function(pval *op1)
{
	switch (op1->type) {
		case IS_LONG:
			op1->value.lval++;
			break;
		case IS_DOUBLE:
			op1->value.dval = op1->value.dval + 1;
			break;
		case IS_STRING: /* Perl style string increment */
			if (op1->value.str.len==0) { /* consider as 0 */
				STR_FREE(op1->value.str.val);
				op1->value.lval = 1;
				op1->type = IS_LONG;
			} else {
				increment_string(op1);
			}
			break;
		default:
			return FAILURE;
	}

	return SUCCESS;
}


int decrement_function(pval *op1)
{
	long lval;
	
	switch (op1->type) {
		case IS_LONG:
			op1->value.lval--;
			break;
		case IS_DOUBLE:
			op1->value.dval = op1->value.dval - 1;
			break;
		case IS_STRING:		/* Like perl we only support string increment */
			if (op1->value.str.len==0) { /* consider as 0 */
				STR_FREE(op1->value.str.val);
				op1->value.lval = -1;
				op1->type = IS_LONG;
				break;
			} else if (is_numeric_string(op1->value.str.val, op1->value.str.len, &lval, NULL)==IS_LONG) { /* long */
				op1->value.lval = lval-1;
				op1->type = IS_LONG;
				break;
			}
			break;
		default:
			return FAILURE;
	}

	return SUCCESS;
}

int yystype_true(pval *op)
{
	convert_to_boolean_long(op);
	return (op->value.lval ? 1 : 0);
}


void php3_str_tolower(char *str, unsigned int length)
{
	register char *p=--str, *end=p+length;
	
	while ((p++)<end) {
		*p = tolower(*p);
	}
}


inline int php3_binary_strcmp(pval *s1, pval *s2)
{
	int retval;
	
	retval = memcmp(s1->value.str.val, s2->value.str.val, MIN(s1->value.str.len,s2->value.str.len));
	if (!retval) {
		return (s1->value.str.len - s2->value.str.len);
	} else {
		return retval;
	}
}

inline void php3_smart_strcmp(pval *result, pval *s1, pval *s2)
{
	int ret1,ret2;
	long lval1, lval2;
	double dval1, dval2;
	
	if ((ret1=is_numeric_string(s1->value.str.val, s1->value.str.len, &lval1, &dval1)) &&
		(ret2=is_numeric_string(s2->value.str.val, s2->value.str.len, &lval2, &dval2))) {
		if ((ret1==IS_BC) || (ret2==IS_BC)) {
			bc_num first, second;
			
			/* use the BC math library to compare the numbers */
			init_num(&first);
			init_num(&second);
			str2num(&first,s1->value.str.val,100); /* this scale should do */
			str2num(&second,s2->value.str.val,100); /* ditto */
			result->value.lval = bc_compare(first,second);
			result->type = IS_LONG;
			free_num(&first);
			free_num(&second);
		} else if ((ret1==IS_DOUBLE) || (ret2==IS_DOUBLE)) {
			if (ret1!=IS_DOUBLE) {
				dval1 = strtod(s1->value.str.val, NULL);
			} else if (ret2!=IS_DOUBLE) {
				dval2 = strtod(s2->value.str.val, NULL);
			}
			result->value.dval = dval1 - dval2;
			result->type = IS_DOUBLE;
		} else { /* they both have to be long's */
			result->value.lval = lval1 - lval2;
			result->type = IS_LONG;
		}
	} else {
		result->value.lval = php3_binary_strcmp(s1,s2);
		result->type = IS_LONG;
	}
	STR_FREE(s1->value.str.val);
	STR_FREE(s2->value.str.val);
	return;	
}


/* returns 0 for non-numeric string
 * returns IS_DOUBLE for floating point string, and assigns the value to *dval (if it's not NULL)
 * returns IS_LONG for integer strings, and assigns the value to *lval (if it's not NULL)
 * returns IS_BC if the number might lose accuracy when converted to a double
 */
 
#if 1
static inline int is_numeric_string(char *str, int length, long *lval, double *dval)
{
	long local_lval;
	double local_dval;
	char *end_ptr;

	if (!length) {
		return 0;
	}
	
	errno=0;
	local_lval = strtol(str, &end_ptr, 10);
	if (errno!=ERANGE && end_ptr == str+length) { /* integer string */
		if (lval) {
			*lval = local_lval;
		}
		return IS_LONG;
	}

	errno=0;
	local_dval = strtod(str, &end_ptr);
	if (errno!=ERANGE && end_ptr == str+length) { /* floating point string */
		if (dval) {
			*dval = local_dval;
		}
		if (length>16) {
			register char *ptr=str, *end=str+length;
			
			while(ptr<end) {
				switch(*ptr++) {
					case 'e':
					case 'E':
						/* scientific notation, not handled by the BC library */
						return IS_DOUBLE;
						break;
					default:
						break;
				}
			}
			return IS_BC;
		} else {
			return IS_DOUBLE;
		}
	}
	
	return 0;
}

#else

static inline int is_numeric_string(char *str, int length, long *lval, double *dval)
{
	register char *p=str, *end=str+length;
	unsigned char had_period=0,had_exponent=0;
	char *end_ptr;
	
	if (!length) {
		return 0;
	}
	switch (*p) {
		case '-':
		case '+':
			while (*++p==' ');  /* ignore spaces after the sign */
			break;
		default:
			break;
	}
	while (p<end) {
		if (isdigit((int)(unsigned char)*p)) {
			p++;
		} else if (*p=='.') {
			if (had_period) {
				return 0;
			} else {
				had_period=1;
				p++;
			}
		} else if (*p=='e' || *p=='E') {
			p++;
			if (is_numeric_string(p, length - (int) (p-str), NULL, NULL)==IS_LONG) { /* valid exponent */
				had_exponent=1;
				break;
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}
	errno=0;
	if (had_period || had_exponent) { /* floating point number */
		double local_dval;
		
		local_dval = strtod(str, &end_ptr);
		if (errno==ERANGE || end_ptr != str+length) { /* overflow or bad string */
			return 0;
		} else {
			if (dval) {
				*dval = local_dval;
			}
			return IS_DOUBLE;
		}
	} else {
		long local_lval;
		
		local_lval = strtol(str, &end_ptr, 10);
		if (errno==ERANGE || end_ptr != str+length) { /* overflow or bad string */
			return 0;
		} else {
			if (lval) {
				*lval = local_lval;
			}
			return IS_LONG;
		}
	}
}

#endif
