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
   | Author: Jim Winstead (jimw@php.net)                                  |
   +----------------------------------------------------------------------+
 */
/* $Id: base64.c,v 1.15 1998/01/15 22:01:06 jim Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include <string.h>

#include "parser.h"
#include "internal_functions.h"
#include "base64.h"

static char base64_table[] =
	{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
	};
static char base64_pad = '=';

char *base64_encode(const char *string) {
	const char *current = string;
	int length = strlen(string);
	int i = 0;
	char *result = (char *)emalloc(((length + 3 - length % 3) * 4 / 3 + 1) * sizeof(char));

	while (length > 2) { /* keep going until we have less than 24 bits */
		result[i++] = base64_table[current[0] >> 2];
		result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
		result[i++] = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
		result[i++] = base64_table[current[2] & 0x3f];

		current += 3;
		length -= 3; /* we just handle 3 octets of data */
	}

	/* now deal with the tail end of things */
	if (length != 0) {
		result[i++] = base64_table[current[0] >> 2];
		if (length > 1) {
			result[i++] = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
			result[i++] = base64_table[(current[1] & 0x0f) << 2];
			result[i++] = base64_pad;
		}
		else {
			result[i++] = base64_table[(current[0] & 0x03) << 4];
			result[i++] = base64_pad;
			result[i++] = base64_pad;
		}
	}

	result[i] = '\0';
	return result;
}

/* as above, but backwards. :) */
char *base64_decode(const char *string) {
	const char *current = string;
	int length = strlen(string);
	int ch, i = 0, j = 0;

	char *result = (char *)emalloc((length / 4 * 3 + 1) * sizeof(char));
	if (result == NULL) {
		return NULL;
	}

	/* run through the whole string, converting as we go */
	while ((ch = *current++) != '\0') {
		if (ch == base64_pad) break;
		ch = (int)strchr(base64_table, ch);
		if (ch == 0) {
			efree(result);
			return NULL;
		}
		ch -= (int)base64_table;

		switch(i % 4) {
		case 0:
			result[j] = ch << 2;
			break;
		case 1:
			result[j++] |= ch >> 4;
			result[j] = (ch & 0x0f) << 4;
			break;
		case 2:
			result[j++] |= ch >>2;
			result[j] = (ch & 0x03) << 6;
			break;
		case 3:
			result[j++] |= ch;
			break;
		}
		i++;
	}

	/* mop things up if we ended on a boundary */
	if (ch == base64_pad) {
		switch(i % 4) {
		case 0:
		case 1:
			efree(result);
			return NULL;
		case 2:
			j++;
		case 3:
			result[j++] = 0;
		}
	}

	result[j] = '\0';
	return result;
}

void php3_base64_encode(INTERNAL_FUNCTION_PARAMETERS) {
	YYSTYPE *string;
	char *result;
	TLS_VARS;

	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&string) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(string);
	result = base64_encode(string->value.strval);
	if (result != NULL) {
		return_value->value.strval = result;
		return_value->strlen = strlen(result);
		return_value->type = IS_STRING;
	} else {
		RETURN_FALSE;
	}
}

void php3_base64_decode(INTERNAL_FUNCTION_PARAMETERS) {
	YYSTYPE *string;
	char *result;
	TLS_VARS;

	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&string) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(string);
	result = base64_decode(string->value.strval);
	if (result != NULL) {
		return_value->value.strval = result;
		return_value->strlen = strlen(result);
		return_value->type = IS_STRING;
	} else {
		RETURN_FALSE;
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
