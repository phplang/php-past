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
   | Authors: Jim Winstead (jimw@php.net)                                 |
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "phpmath.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void php3_abs(INTERNAL_FUNCTION_PARAMETERS) {
	YYSTYPE *value;
	TLS_VARS;
	
	if (ARG_COUNT(ht)!=1||getParameters(ht,1,&value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (value->type == IS_STRING) {
		convert_string_to_number(value);
	}

	if (value->type == IS_DOUBLE) {
		RETURN_DOUBLE(fabs(value->value.dval));
	}
	else if (value->type == IS_LONG) {
		RETURN_LONG(labs(value->value.lval));
	}

	RETURN_FALSE;
}

void php3_ceil(INTERNAL_FUNCTION_PARAMETERS) {
	YYSTYPE *value;
	TLS_VARS;
	
	if (ARG_COUNT(ht)!=1||getParameters(ht,1,&value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (value->type == IS_STRING) {
		convert_string_to_number(value);
	}

	if (value->type == IS_DOUBLE) {
		RETURN_LONG((long)ceil(value->value.dval));
	}
	else if (value->type == IS_LONG) {
		RETURN_LONG(value->value.lval);
	}

	RETURN_FALSE;
}

void php3_floor(INTERNAL_FUNCTION_PARAMETERS) {
	YYSTYPE *value;
	TLS_VARS;
	
	if (ARG_COUNT(ht)!=1||getParameters(ht,1,&value)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (value->type == IS_STRING) {
		convert_string_to_number(value);
	}

	if (value->type == IS_DOUBLE) {
		RETURN_LONG((long)floor(value->value.dval));
	}
	else if (value->type == IS_LONG) {
		RETURN_LONG(value->value.lval);
	}

	RETURN_FALSE;
}

#ifndef HAVE_RINT
/* emulate rint */
inline double rint(double n)
{
	double i, f;
	f = modf(n, &i);
	if (f > .5)
		i++;
	else if (f < -.5)
		i--;
	return i;
}
#endif

void php3_round(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *value;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (value->type == IS_STRING) {
		convert_string_to_number(value);
	}
	if (value->type == IS_DOUBLE) {
		RETURN_DOUBLE(rint(value->value.dval));
	}
	if (value->type == IS_LONG) {
		RETURN_DOUBLE((double)value->value.lval);
	}
	RETURN_FALSE;
}

void php3_sin(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = sin(num->value.dval);
	return_value->type = IS_DOUBLE;
}

void php3_cos(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = cos(num->value.dval);
	return_value->type = IS_DOUBLE;
}

void php3_tan(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = tan(num->value.dval);
	return_value->type = IS_DOUBLE;
}

void php3_asin(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = asin(num->value.dval);
	return_value->type = IS_DOUBLE;
}

void php3_acos(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = acos(num->value.dval);
	return_value->type = IS_DOUBLE;
}

void php3_atan(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = atan(num->value.dval);
	return_value->type = IS_DOUBLE;
}

void php3_pi(INTERNAL_FUNCTION_PARAMETERS)
{
	return_value->value.dval = M_PI;
	return_value->type = IS_DOUBLE;
}

void php3_pow(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num1, *num2;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht,2,&num1,&num2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num1);
	convert_to_double(num2);
	RETURN_DOUBLE(pow(num1->value.dval, num2->value.dval));
}

void php3_exp(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = exp(num->value.dval);
	return_value->type = IS_DOUBLE;
}

void php3_log(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = log(num->value.dval);
	return_value->type = IS_DOUBLE;
}

void php3_log10(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = log10(num->value.dval);
	return_value->type = IS_DOUBLE;
}

void php3_sqrt(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	return_value->value.dval = sqrt(num->value.dval);
	return_value->type = IS_DOUBLE;
}


/*
 * Convert a string representation of a base(2-36) number to a long.
 */
static long
_php3_basetolong(YYSTYPE *arg, int base) {
	long mult = 1, num = 0, digit;
	int i;
	char c, *s;

	if (arg->type != IS_STRING || base < 2 || base > 36) {
		return 0;
	}

	s = arg->value.strval;

	for (i = arg->strlen - 1; i >= 0; i--, mult *= base) {
		c = toupper(s[i]);
		if (c >= '0' && c <= '9') {
			digit = (c - '0');
		} else if (c >= 'A' && c <= 'Z') {
			digit = (c - 'A' + 10);
		} else {
			continue;
		}
		if (digit >= base) {
			continue;
		}
		num += mult * digit;
	}

	return num;
}


/*
 * Convert a long to a string containing a base(2-36) representation of
 * the number.
 */
static char *
_php3_longtobase(YYSTYPE *arg, int base)
{
	static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char *result, *ptr, *ret;
	int len, digit;
	long value;

	if (arg->type != IS_LONG || base < 2 || base > 36) {
		return empty_string;
	}

	value = arg->value.lval;

	/* allocates space for the longest possible result with the lowest base */
	len = (sizeof(arg->value.lval) * 8) + 1;
	result = emalloc((sizeof(arg->value.lval) * 8) + 1);

	ptr = result + len - 1;
	*ptr-- = '\0';

	do {
		digit = value % base;
		*ptr = digits[digit];
		value /= base;
	}
	while (ptr-- > result && value);
	ptr++;
	ret = estrdup(ptr);
	efree(result);

	return ret;
}	


void php3_bindec(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	long ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg);
	ret = _php3_basetolong(arg, 2);

	RETVAL_LONG(ret);
}


/* Hexadecimal to Decimal */
void php3_hexdec(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	long ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg);

	ret = _php3_basetolong(arg, 16);
	RETVAL_LONG(ret);
}


/* Octal to Decimal */
void php3_octdec(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	long ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(arg);

	ret = _php3_basetolong(arg, 8);
	RETVAL_LONG(ret);
}


void php3_decbin(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	char *result;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);

	result = _php3_longtobase(arg, 2);
	return_value->type = IS_STRING;
	return_value->strlen = strlen(result);
	return_value->value.strval = result;
}


void php3_decoct(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	char *result;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);

	result = _php3_longtobase(arg, 8);
	return_value->type = IS_STRING;
	return_value->strlen = strlen(result);
	return_value->value.strval = result;
}


void php3_dechex(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	char *result;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg);

	result = _php3_longtobase(arg, 16);
	return_value->type = IS_STRING;
	return_value->strlen = strlen(result);
	return_value->value.strval = result;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
