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
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */

/* $Id: formatted_print.c,v 1.31 1998/03/02 20:59:50 ssb Exp $ */

#include <math.h>				/* modf() */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "head.h"
#include "php3_string.h"
#include <stdio.h>

#define ALIGN_LEFT 0
#define ALIGN_RIGHT 1
#define ADJ_WIDTH 1
#define ADJ_PRECISION 2
#define NUM_BUF_SIZE 500
#define	NDIG 80
#define FLOAT_DIGITS 6
#define FLOAT_PRECISION 6
#define MAX_FLOAT_DIGITS 38
#define MAX_FLOAT_PRECISION 40
#undef SPRINTF_DEBUG

static char hexchars[] = "0123456789abcdef";
static char HEXCHARS[] = "0123456789ABCDEF";


/*
 * cvt.c - IEEE floating point formatting routines for FreeBSD
 * from GNU libc-4.6.27
 */

/*
 *    _php3_cvt converts to decimal
 *      the number of digits is specified by ndigit
 *      decpt is set to the position of the decimal point
 *      sign is set to 0 for positive, 1 for negative
 */
static char *
 _php3_cvt(double arg, int ndigits, int *decpt, int *sign, int eflag)
{
	register int r2;
	double fi, fj;
	register char *p, *p1;
	/*THREADX*/
#ifndef THREAD_SAFE
	static char cvt_buf[NDIG];
#endif
	TLS_VARS;

	if (ndigits >= NDIG - 1)
		ndigits = NDIG - 2;
	r2 = 0;
	*sign = 0;
	p = &STATIC(cvt_buf)[0];
	if (arg < 0) {
		*sign = 1;
		arg = -arg;
	}
	arg = modf(arg, &fi);
	p1 = &STATIC(cvt_buf)[NDIG];
	/*
	 * Do integer part
	 */
	if (fi != 0) {
		p1 = &STATIC(cvt_buf)[NDIG];
		while (fi != 0) {
			fj = modf(fi / 10, &fi);
			*--p1 = (int) ((fj + .03) * 10) + '0';
			r2++;
		}
		while (p1 < &STATIC(cvt_buf)[NDIG])
			*p++ = *p1++;
	} else if (arg > 0) {
		while ((fj = arg * 10) < 1) {
			arg = fj;
			r2--;
		}
	}
	p1 = &STATIC(cvt_buf)[ndigits];
	if (eflag == 0)
		p1 += r2;
	*decpt = r2;
	if (p1 < &STATIC(cvt_buf)[0]) {
		STATIC(cvt_buf)[0] = '\0';
		return (STATIC(cvt_buf));
	}
	while (p <= p1 && p < &STATIC(cvt_buf)[NDIG]) {
		arg *= 10;
		arg = modf(arg, &fj);
		*p++ = (int) fj + '0';
	}
	if (p1 >= &STATIC(cvt_buf)[NDIG]) {
		STATIC(cvt_buf)[NDIG - 1] = '\0';
		return (STATIC(cvt_buf));
	}
	p = p1;
	*p1 += 5;
	while (*p1 > '9') {
		*p1 = '0';
		if (p1 > STATIC(cvt_buf))
			++ * --p1;
		else {
			*p1 = '1';
			(*decpt)++;
			if (eflag == 0) {
				if (p > STATIC(cvt_buf))
					*p = '0';
				p++;
			}
		}
	}
	*p = '\0';
	return (STATIC(cvt_buf));
}


inline static void _php3_sprintf_appendchar(char **buffer, int *pos, int *size, char add)
{
	if ((*pos + 1) >= *size) {
		*size <<= 1;
#ifdef SPRINTF_DEBUG
		php3_printf("sprintf: ereallocing buffer to %d bytes\n", *size);
#endif
		*buffer = erealloc(*buffer, *size);
	}
#ifdef SPRINTF_DEBUG
	php3_printf("sprintf: appending '%c', pos=\n", add, *pos);
#endif
	(*buffer)[(*pos)++] = add;
}


inline static void _php3_sprintf_appendstring(char **buffer, int *pos, int *size, char *add,
							  int min_width, int max_width, char padding,
											  int alignment)
{
	register int len = strlen(add), npad = min_width - MIN(len,(max_width?max_width:len));

	if (npad<0) {
		npad=0;
	}
	
#ifdef SPRINTF_DEBUG
	php3_printf("sprintf: appendstring(%x, %d, %d, \"%s\", %d, '%c', %d)\n",
				*buffer, *pos, *size, add, min_width, padding, alignment);
#endif

	if (max_width == 0) {
		max_width = MAX(min_width,len);
	}
	if ((*pos + max_width) >= *size) {
		while ((*pos + max_width) >= *size) {
			*size <<= 1;
		}
#ifdef SPRINTF_DEBUG
		php3_printf("sprintf ereallocing buffer to %d bytes\n", *size);
#endif
		*buffer = erealloc(*buffer, *size);
	}
	if (alignment == ALIGN_RIGHT) {
		while (npad-- > 0) {
			(*buffer)[(*pos)++] = padding;
		}
	}
#ifdef SPRINTF_DEBUG
	php3_printf("sprintf: appending \"%s\"\n", add);
#endif
	strncpy(&(*buffer)[*pos], add, max_width);
	*pos += MIN(max_width,len);
	if (alignment == ALIGN_LEFT) {
		while (npad--) {
			(*buffer)[(*pos)++] = padding;
		}
	}
}


inline static void _php3_sprintf_appendint(char **buffer, int *pos, int *size, int number,
								  int width, char padding, int alignment)
{
	char numbuf[NUM_BUF_SIZE];
	register unsigned int magn, nmagn, i = NUM_BUF_SIZE - 1, neg = 0;

#ifdef SPRINTF_DEBUG
	php3_printf("sprintf: appendint(%x, %x, %x, %d, %d, '%c', %d)\n",
				*buffer, pos, size, number, width, padding, alignment);
#endif

	if (number < 0) {
		neg = 1;
		magn = ((unsigned int) -(number + 1)) + 1;
	} else {
		magn = (unsigned int) number;
	}

	numbuf[i] = '\0';

	do {
		nmagn = magn / 10;

		numbuf[--i] = (magn - (nmagn * 10)) + '0';
		magn = nmagn;
	}
	while (magn > 0 && i > 0);
	if (neg) {
		numbuf[--i] = '-';
	}
#ifdef SPRINTF_DEBUG
	php3_printf("sprintf: appending %d as \"%s\", i=%d\n",
				number, &numbuf[i], i);
#endif

	_php3_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							   padding, alignment);
}


inline static void _php3_sprintf_appenddouble(char **buffer, int *pos,
											  int *size, double number,
											  int width, char padding,
											  int alignment, int precision,
											  int adjust, char fmt)
{
	char numbuf[NUM_BUF_SIZE];
	char *cvt;
	register int i = 0, j = 0;
	int sign, decpt;

#ifdef SPRINTF_DEBUG
	php3_printf("sprintf: appenddouble(%x, %x, %x, %f, %d, '%c', %d, %c)\n",
				*buffer, pos, size, number, width, padding, alignment, fmt);
#endif

	if ((adjust & ADJ_PRECISION) == 0) {
		precision = FLOAT_PRECISION;
	} else if (precision > MAX_FLOAT_PRECISION) {
		precision = MAX_FLOAT_PRECISION;
	}
	cvt = _php3_cvt(number, precision, &decpt, &sign, (fmt == 'e'));

	if (sign) {
		numbuf[i++] = '-';
	}

	if (fmt == 'f') {
		if (decpt <= 0) {
			numbuf[i++] = '0';
			if (precision > 0) {
				int k = precision;
				numbuf[i++] = '.';
				while ((decpt++ < 0) && k--) {
					numbuf[i++] = '0';
				}
			}
		} else {
			while (decpt-- > 0)
				numbuf[i++] = cvt[j++];
			if (precision > 0)
				numbuf[i++] = '.';
		}
	} else {
		numbuf[i++] = cvt[j++];
		if (precision > 0)
			numbuf[i++] = '.';
	}

	while (cvt[j]) {
		numbuf[i++] = cvt[j++];
	}

	numbuf[i] = '\0';

	if (precision > 0) {
		width += (precision + 1);
	}
	_php3_sprintf_appendstring(buffer, pos, size, numbuf, width, 0, padding,
							   alignment);
}


inline static void _php3_sprintf_append2n(char **buffer, int *pos, int *size, int number,
						   int width, char padding, int alignment, int n,
										  char *chartable)
{
	char numbuf[NUM_BUF_SIZE];
	register unsigned int num, i = NUM_BUF_SIZE - 1, neg = 0;
	register int andbits = (1 << n) - 1;

#ifdef SPRINTF_DEBUG
	php3_printf("sprintf: append2n(%x, %x, %x, %d, %d, '%c', %d, %d, %x)\n",
				*buffer, pos, size, number, width, padding, alignment, n,
				chartable);
	php3_printf("sprintf: append2n 2^%d andbits=%x\n", n, andbits);
#endif

	if (number < 0) {
		neg = 1;
		num = ((unsigned int) -(number + 1)) + 1;
	} else {
		num = (unsigned int) number;
	}

	numbuf[i] = '\0';

	do {
		numbuf[--i] = chartable[(num & andbits)];
		num >>= n;
	}
	while (num > 0);

	if (neg) {
		numbuf[--i] = '-';
	}
	_php3_sprintf_appendstring(buffer, pos, size, &numbuf[i], width, 0,
							   padding, alignment);
}


inline static int _php3_sprintf_getnumber(char *buffer, int *pos)
{
	char *endptr;
	register int num = strtol(&buffer[*pos], &endptr, 10);
	register int i = 0;

	if (endptr != NULL) {
		i = (endptr - &buffer[*pos]);
	}
#ifdef SPRINTF_DEBUG
	php3_printf("sprintf_getnumber: number was %d bytes long\n", i);
#endif
	*pos += i;
	return num;
}


/*
 * New sprintf implementation for PHP.
 *
 * Modifiers:
 *
 *  " "   pad integers with spaces
 *  "-"   left adjusted field
 *   n    field size
 *  "."n  precision (floats only)
 *
 * Type specifiers:
 *
 *  "%"   literal "%", modifiers are ignored.
 *  "b"   integer argument is printed as binary
 *  "c"   integer argument is printed as a single character
 *  "d"   argument is an integer
 *  "f"   the argument is a float
 *  "o"   integer argument is printed as octal
 *  "s"   argument is a string
 *  "x"   integer argument is printed as lowercase hexadecimal
 *  "X"   integer argument is printed as uppercase hexadecimal
 *
 */
static char *php3_formatted_print(HashTable *ht)
{
	YYSTYPE **args;
	int argc, size = 240, inpos = 0, outpos = 0;
	int alignment, width, precision, currarg, adjusting;
	char *format, *result, padding;

	argc = ARG_COUNT(ht);

	if (argc < 1) {
		WRONG_PARAM_COUNT_WITH_RETVAL(NULL);
	}
	args = emalloc(argc * sizeof(YYSTYPE *));

	if (getParametersArray(ht, argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT_WITH_RETVAL(NULL);
	}
	convert_to_string(args[0]);
	format = args[0]->value.strval;
	result = emalloc(size);

	currarg = 1;

	while (format[inpos]) {
#ifdef SPRINTF_DEBUG
		php3_printf("sprintf: format[%d]='%c'\n", inpos, format[inpos]);
		php3_printf("sprintf: outpos=%d\n", outpos);
#endif
		if (format[inpos] != '%') {
			_php3_sprintf_appendchar(&result, &outpos, &size, format[inpos++]);
		} else if (format[inpos + 1] == '%') {
			_php3_sprintf_appendchar(&result, &outpos, &size, '%');
			inpos += 2;
		} else {
			if (currarg >= argc && format[inpos + 1] != '%') {
				php3_error(E_WARNING, "sprintf: too few arguments");
				return NULL;
			}
			/* starting a new format specifier, reset variables */
			alignment = ALIGN_RIGHT;
			adjusting = 0;
			padding = ' ';
			inpos++;			/* skip the '%' */

#ifdef SPRINTF_DEBUG
			php3_printf("sprintf: first looking at '%c', inpos=%d\n",
						format[inpos], inpos);
#endif

			if (isascii((int)format[inpos]) && !isalpha((int)format[inpos])) {
				/* first look for modifiers */
#ifdef SPRINTF_DEBUG
				php3_printf("sprintf: looking for modifiers\n"
							"sprintf: now looking at '%c', inpos=%d\n",
							format[inpos], inpos);
#endif
				for (;; inpos++) {
					if (format[inpos] == ' ' || format[inpos] == '0') {
						padding = format[inpos];
					} else if (format[inpos] == '-') {
						alignment = ALIGN_LEFT;
						/* space padding, the default */
					} else if (format[inpos] == '\'') {
						padding = format[++inpos];
					} else {
#ifdef SPRINTF_DEBUG
						php3_printf("sprintf: end of modifiers\n");
#endif
						break;
					}
				}
#ifdef SPRINTF_DEBUG
				php3_printf("sprintf: padding='%c'\n", padding);
				php3_printf("sprintf: alignment=%s\n",
							(alignment == ALIGN_LEFT) ? "left" : "right");
#endif


				/* after modifiers comes width */
				if (isdigit((int)format[inpos])) {
#ifdef SPRINTF_DEBUG
					php3_printf("sprintf: getting width\n");
#endif
					width = _php3_sprintf_getnumber(format, &inpos);
					adjusting |= ADJ_WIDTH;
				} else {
					width = 0;
				}
#ifdef SPRINTF_DEBUG
				php3_printf("sprintf: width=%d\n", width);
#endif

				/* after width comes precision */
				if (format[inpos] == '.') {
					inpos++;
#ifdef SPRINTF_DEBUG
					php3_printf("sprintf: getting precision\n");
#endif
					if (isdigit((int)format[inpos])) {
						precision = _php3_sprintf_getnumber(format, &inpos);
						adjusting |= ADJ_PRECISION;
					} else {
						precision = 0;
					}
				} else {
					precision = 0;
				}
#ifdef SPRINTF_DEBUG
				php3_printf("sprintf: precision=%d\n", precision);
#endif
			} else {
				width = precision = 0;
			}

			if (format[inpos] == 'l') {
				inpos++;
			}
#ifdef SPRINTF_DEBUG
			php3_printf("sprintf: format character='%c'\n", format[inpos]);
#endif
			/* now we expect to find a type specifier */
			switch (format[inpos]) {
				case 's':
					convert_to_string(args[currarg]);
					_php3_sprintf_appendstring(&result, &outpos, &size,
											 args[currarg]->value.strval,
											   width, precision, padding,
											   alignment);
					break;

				case 'd':
					convert_to_long(args[currarg]);
					_php3_sprintf_appendint(&result, &outpos, &size,
											args[currarg]->value.lval,
											width, padding, alignment);
					break;

				case 'e':
				case 'f':
					/* XXX not done */
					convert_to_double(args[currarg]);
					_php3_sprintf_appenddouble(&result, &outpos, &size,
											   args[currarg]->value.dval,
											   width, padding, alignment,
											   precision, adjusting,
											   format[inpos]);
					break;

				case 'c':
					convert_to_long(args[currarg]);
					_php3_sprintf_appendchar(&result, &outpos, &size,
									   (char) args[currarg]->value.lval);
					break;

				case 'o':
					convert_to_long(args[currarg]);
					_php3_sprintf_append2n(&result, &outpos, &size,
										   args[currarg]->value.lval,
										   width, padding, alignment, 3,
										   hexchars);
					break;

				case 'x':
					convert_to_long(args[currarg]);
					_php3_sprintf_append2n(&result, &outpos, &size,
										   args[currarg]->value.lval,
										   width, padding, alignment, 4,
										   hexchars);
					break;

				case 'X':
					convert_to_long(args[currarg]);
					_php3_sprintf_append2n(&result, &outpos, &size,
										   args[currarg]->value.lval,
										   width, padding, alignment, 4,
										   HEXCHARS);
					break;

				case 'b':
					convert_to_long(args[currarg]);
					_php3_sprintf_append2n(&result, &outpos, &size,
										   args[currarg]->value.lval,
										   width, padding, alignment, 1,
										   hexchars);
					break;

				case '%':
					_php3_sprintf_appendchar(&result, &outpos, &size, '%');

					break;
				default:
					break;
			}
			currarg++;
			inpos++;
		}
	}
	
	efree(args);
	
	/* possibly, we have to make sure we have room for the terminating null? */
	result[outpos]=0;
	
	return result;
}


void php3_user_sprintf(INTERNAL_FUNCTION_PARAMETERS)
{
	char *result;
	TLS_VARS;
	
	if ((result=php3_formatted_print(ht))==NULL) {
		RETURN_FALSE;
	}
	RETVAL_STRING(result);
	efree(result);
}


void php3_user_printf(INTERNAL_FUNCTION_PARAMETERS)
{
	char *result;
	TLS_VARS;
	
	if ((result=php3_formatted_print(ht))==NULL) {
		RETURN_FALSE;
	}
	if(php3_header(0,NULL))
		PUTS(result);
	efree(result);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
