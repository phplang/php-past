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


/* $Id: operators.c,v 1.56 1998/02/01 17:31:57 zeev Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include <stdio.h>
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

void convert_double_to_long(YYSTYPE *op)
{
	if (op->type == IS_DOUBLE) {
		op->value.lval = (long) op->value.dval;
		op->type = IS_LONG;
	}
}


void convert_string_to_number(YYSTYPE *op)
{
	char *strval;

	if (op->type == IS_STRING) {
		strval = op->value.strval;
		if (strchr(strval, '.') || strchr(strval, 'e') || strchr(strval, 'E')) {
			op->type = IS_DOUBLE;
			op->value.dval = atof(op->value.strval);
		} else {
			op->type = IS_LONG;
			op->value.lval = strtol(op->value.strval, NULL, 10);
		}
		STR_FREE(strval);
	}
}

PHPAPI void convert_to_long(YYSTYPE *op)
{
	convert_to_long_base(op, 10);
}

PHPAPI void convert_to_long_base(YYSTYPE *op, int base)
{
	char *strval;
	TLS_VARS;

	switch (op->type) {
		case IS_LONG:
			return;
		case IS_DOUBLE:
			op->value.lval = (long) op->value.dval;
			op->type = IS_LONG;
			break;
		case IS_STRING:
			strval = op->value.strval;
			op->value.lval = strtol(strval, NULL, base);
			op->type = IS_LONG;
			STR_FREE(strval);
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


PHPAPI void convert_to_double(YYSTYPE *op)
{
	char *strval;
	TLS_VARS;

	switch (op->type) {
		case IS_LONG:
			op->value.dval = (double) op->value.lval;
			op->type = IS_DOUBLE;
			break;
		case IS_DOUBLE:
			break;
		case IS_STRING:
			strval = op->value.strval;

			op->value.dval = atof(strval);
			op->type = IS_DOUBLE;
			STR_FREE(strval);
			break;
		default:
			php3_error(E_WARNING, "Cannot convert to real value");
			yystype_destructor(op _INLINE_TLS);
			op->value.dval = 0;
			op->type = IS_DOUBLE;
			break;
	}
}


PHPAPI void convert_to_boolean_long(YYSTYPE *op)
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
			strval = op->value.strval;

			if (op->strlen == 0 || (op->strlen == 1 && strval[0] == '0')) {
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


PHPAPI void convert_to_string(YYSTYPE *op)
{
	long lval;
	double dval;
	TLS_VARS;

	switch (op->type) {
		case IS_STRING:
			break;
		case IS_LONG:
			lval = op->value.lval;

			op->value.strval = (char *) emalloc(MAX_LENGTH_OF_LONG + 1);
			if (!op->value.strval) {
				return;
			}
			op->strlen = sprintf(op->value.strval, "%ld", lval);  /* SAFE */
			op->type = IS_STRING;
			break;
		case IS_DOUBLE:
			dval = op->value.dval;

			op->value.strval = (char *) emalloc(MAX_LENGTH_OF_DOUBLE + 1);
			if (!op->value.strval) {
				return;
			}
			op->strlen = sprintf(op->value.strval, "%f", dval);  /* SAFE */
			op->type = IS_STRING;
			break;
		default:
			yystype_destructor(op _INLINE_TLS);
			var_reset(op);
			break;
	}
}


static void convert_scalar_to_array(YYSTYPE *op)
{
	YYSTYPE tmp = *op;
	
	yystype_copy_constructor(&tmp);
	op->value.ht = (HashTable *) emalloc(sizeof(HashTable));
	hash_init(op->value.ht, 0, NULL, YYSTYPE_DESTRUCTOR, 0);
	hash_index_update(op->value.ht, 0, (void *) &tmp, sizeof(YYSTYPE), NULL);
	op->type = IS_ARRAY;
}


PHPAPI void convert_to_array(YYSTYPE *op)
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
			convert_scalar_to_array(op);
			break;
	}
}


PHPAPI void convert_to_object(YYSTYPE *op)
{
	switch(op->type) {
		case IS_ARRAY:
			op->type = IS_ARRAY;
			return;
			break;
		case IS_OBJECT:
			return;
			break;
		default:
			convert_scalar_to_array(op);
			op->type = IS_OBJECT;
			break;
	}
}

		
int add_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	if (op1->type == IS_ARRAY && op2->type == IS_ARRAY) {
		YYSTYPE tmp;
		
		hash_merge(op1->value.ht,op2->value.ht,(void (*)(void *pData)) yystype_copy_constructor, (void *) &tmp, sizeof(YYSTYPE));
		*result = *op1;
		yystype_destructor(op2 _INLINE_TLS);
		return SUCCESS;
	}
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		result->strlen = op1->strlen + op2->strlen;
		result->value.strval = (char *) emalloc(result->strlen + 1);
		strcpy(result->value.strval, op1->value.strval);
		strcat(result->value.strval, op2->value.strval);
		result->value.strval[result->strlen] = 0;
		result->type = IS_STRING;
		STR_FREE(op1->value.strval);
		STR_FREE(op2->value.strval);
		return SUCCESS;
	}
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval + op2->value.lval;
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


int sub_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval - op2->value.lval;
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


int mul_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval * op2->value.lval;
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

int div_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if ((op2->type == IS_LONG && op2->value.lval == 0) || (op2->type == IS_DOUBLE && op2->value.dval == 0.0)) {
		php3_error(E_WARNING, "Division by zero");
		var_reset(result);
		return FAILURE;			/* division by zero */
	}
	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval / op2->value.lval;
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

int mod_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	convert_string_to_number(op1);
	convert_string_to_number(op2);

	if (op1->type == IS_DOUBLE) {
		op1->value.lval = (long) op1->value.dval;
		op1->type = IS_LONG;
	}
	if (op2->type == IS_DOUBLE) {
		op2->value.lval = (long) op2->value.dval;
		op2->type = IS_LONG;
	}
	if (op2->value.lval == 0) {
		return FAILURE;			/* modulus by zero */
	}
	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval % op2->value.lval;
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	yystype_destructor(op2 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}

int boolean_or_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2)
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


int boolean_and_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2)
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


int boolean_not_function(YYSTYPE *result, YYSTYPE *op1)
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


int bitwise_not_function(YYSTYPE *result, YYSTYPE *op1 INLINE_TLS)
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
		result->value.strval = op1->value.strval;
		result->strlen = op1->strlen;
		for (i = 0; i < op1->strlen; i++) {
			result->value.strval[i] = ~op1->value.strval[i];
		}
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}


int bitwise_or_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	if (op1->type == IS_DOUBLE) {
		op1->value.lval = (long) op1->value.dval;
		op1->type = IS_LONG;
	}
	if (op2->type == IS_DOUBLE) {
		op2->value.lval = (long) op2->value.dval;
		op2->type = IS_LONG;
	}
	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval | op2->value.lval;
		return SUCCESS;
	}
	if ((op1->type == IS_STRING && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_STRING)) {
		/* assume that the string is 0 */
		result->type = IS_LONG;
		result->value.lval = (op1->type == IS_LONG ? op1->value.lval : op2->value.lval);
		yystype_destructor(op1 _INLINE_TLS);
		yystype_destructor(op2 _INLINE_TLS);
		return SUCCESS;
	}
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		YYSTYPE *longer, *shorter;
		int i;

		if (op1->strlen >= op2->strlen) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		result->strlen = longer->strlen;
		result->value.strval = longer->value.strval;
		for (i = 0; i < shorter->strlen; i++) {
			result->value.strval[i] |= shorter->value.strval[i];
		}
		STR_FREE(shorter->value.strval);
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	yystype_destructor(op2 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}


int bitwise_and_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	if (op1->type == IS_DOUBLE) {
		op1->value.lval = (long) op1->value.dval;
		op1->type = IS_LONG;
	}
	if (op2->type == IS_DOUBLE) {
		op2->value.lval = (long) op2->value.dval;
		op2->type = IS_LONG;
	}
	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval & op2->value.lval;
		return SUCCESS;
	}
	if ((op1->type == IS_STRING && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_STRING)) {
		/* assume that the string is 0 */
		result->type = IS_LONG;
		result->value.lval = 0;  /* AND with 0 is 0 */
		yystype_destructor(op1 _INLINE_TLS);
		yystype_destructor(op2 _INLINE_TLS);
		return SUCCESS;
	}
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		YYSTYPE *longer, *shorter;
		int i;

		if (op1->strlen >= op2->strlen) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		result->strlen = shorter->strlen;
		result->value.strval = shorter->value.strval;
		for (i = 0; i < shorter->strlen; i++) {
			result->value.strval[i] &= longer->value.strval[i];
		}
		STR_FREE(longer->value.strval);
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	yystype_destructor(op2 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}


int bitwise_xor_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	if (op1->type == IS_DOUBLE) {
		op1->value.lval = (long) op1->value.dval;
		op1->type = IS_LONG;
	}
	if (op2->type == IS_DOUBLE) {
		op2->value.lval = (long) op2->value.dval;
		op2->type = IS_LONG;
	}
	if (op1->type == IS_LONG && op2->type == IS_LONG) {
		result->type = IS_LONG;
		result->value.lval = op1->value.lval ^ op2->value.lval;
		return SUCCESS;
	}
	if ((op1->type == IS_STRING && op2->type == IS_LONG)
		|| (op1->type == IS_LONG && op2->type == IS_STRING)) {
		/* assume that the string is 0 */
		result->type = IS_LONG;
		result->value.lval = (op1->type == IS_LONG ? op1->value.lval : op2->value.lval) ^ 0;
		yystype_destructor(op1 _INLINE_TLS);
		yystype_destructor(op2 _INLINE_TLS);
		return SUCCESS;
	}
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		YYSTYPE *longer, *shorter;
		int i;

		if (op1->strlen >= op2->strlen) {
			longer = op1;
			shorter = op2;
		} else {
			longer = op2;
			shorter = op1;
		}

		result->strlen = shorter->strlen;
		result->value.strval = shorter->value.strval;
		for (i = 0; i < shorter->strlen; i++) {
			result->value.strval[i] ^= longer->value.strval[i];
		}
		STR_FREE(longer->value.strval);
		return SUCCESS;
	}
	yystype_destructor(op1 _INLINE_TLS);
	yystype_destructor(op2 _INLINE_TLS);
	var_reset(result);
	return FAILURE;				/* unknown datatype */
}

int add_char_to_string(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	if (op1->type != IS_STRING) {
		yystype_destructor(op1 _INLINE_TLS);
		yystype_destructor(op2 _INLINE_TLS);
		var_reset(result);
		return FAILURE;
	}
	result->strlen = op1->strlen + 1;
	result->value.strval = (char *) emalloc(result->strlen + 1);
	strcpy(result->value.strval, op1->value.strval);
	result->value.strval[result->strlen - 1] = op2->value.chval;
	result->value.strval[result->strlen] = 0;
	result->type = IS_STRING;
	STR_FREE(op1->value.strval);

	return SUCCESS;
}

int concat_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2, int free_op2 INLINE_TLS)
{
	convert_to_string(op1);
	convert_to_string(op2);

	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		result->strlen = op1->strlen + op2->strlen;
		result->value.strval = (char *) emalloc(result->strlen + 1);
		memcpy(result->value.strval, op1->value.strval,op1->strlen);
		memcpy(result->value.strval+op1->strlen, op2->value.strval,op2->strlen);
		result->value.strval[result->strlen] = 0;
		result->type = IS_STRING;
		STR_FREE(op1->value.strval);
		if (free_op2) {
			STR_FREE(op2->value.strval);
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

int concat_function_with_free(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	return concat_function(result,op1,op2,1 _INLINE_TLS);
}

int compare_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
{
	if (op1->type == IS_STRING && op2->type == IS_STRING) {
		result->type = IS_LONG;
		result->value.lval = strcmp(op1->value.strval, op2->value.strval);
		STR_FREE(op1->value.strval);
		STR_FREE(op2->value.strval);
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


int is_equal_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
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


int is_not_equal_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
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


int is_smaller_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
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


int is_smaller_or_equal_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
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

int is_greater_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
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


int is_greater_or_equal_function(YYSTYPE *result, YYSTYPE *op1, YYSTYPE *op2 INLINE_TLS)
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

static void increment_string(YYSTYPE *str)
{
    int carry=0;
    int pos=str->strlen-1;
    char *s=str->value.strval;
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
        t = (char *) emalloc(str->strlen+1+1);
        memcpy(t+1,str->value.strval, str->strlen);
        str->strlen++;
        t[str->strlen] = '\0';
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
        STR_FREE(str->value.strval);
        str->value.strval = t;
    }
}


int increment_function(YYSTYPE *op1)
{
	switch (op1->type) {
		case IS_LONG:
			op1->value.lval++;
			break;
		case IS_DOUBLE:
			op1->value.dval = op1->value.dval + 1;
			break;
		case IS_STRING: /* Perl style string increment */
			if (op1->strlen==0) { /* consider as 0 */
				STR_FREE(op1->value.strval);
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


int decrement_function(YYSTYPE *op1)
{
	switch (op1->type) {
		case IS_LONG:
			op1->value.lval--;
			break;
		case IS_DOUBLE:
			op1->value.dval = op1->value.dval - 1;
			break;
		case IS_STRING:		/* Like perl we only support string increment */
			if (op1->strlen==0) { /* consider as 0 */
				STR_FREE(op1->value.strval);
				op1->value.lval = -1;
				op1->type = IS_LONG;
				break;
			}
		default:
			return FAILURE;
	}

	return SUCCESS;
}

int yystype_true(YYSTYPE *op)
{
	convert_to_boolean_long(op);
	return (op->value.lval ? 1 : 0);
}


void php3_str_tolower(char *str, unsigned int length)
{
	register uint i;

	for (i = 0; i < length; i++) {
		str[i] = tolower(str[i]);
	}
}
