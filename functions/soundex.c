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
   | Author: Bjørn Borud <borud@fast.no>                                  |
   +----------------------------------------------------------------------+
 */
/* $Id: soundex.c,v 1.34 2000/01/01 04:31:17 sas Exp $ */

#include "php.h"
#include "internal_functions.h"
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "php3_string.h"

/* Simple soundex algorithm as described by Knuth in TAOCP, vol 3 */
/* {{{ proto string soundex(string str)
   Calculate the soundex key of a string */
void soundex(INTERNAL_FUNCTION_PARAMETERS)
{
	char	last = 0, code;
	char	*somestring;
	int		i, small, len;
	pval	*arg;
	char	soundex[4 + 1];

	static char soundex_table[26] =
	{0,							/* A */
	 '1',						/* B */
	 '2',						/* C */
	 '3',						/* D */
	 0,							/* E */
	 '1',						/* F */
	 '2',						/* G */
	 0,							/* H */
	 0,							/* I */
	 '2',						/* J */
	 '2',						/* K */
	 '4',						/* L */
	 '5',						/* M */
	 '5',						/* N */
	 0,							/* O */
	 '1',						/* P */
	 '2',						/* Q */
	 '6',						/* R */
	 '2',						/* S */
	 '3',						/* T */
	 0,							/* U */
	 '1',						/* V */
	 0,							/* W */
	 '2',						/* X */
	 0,							/* Y */
	 '2'};						/* Z */

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);
	if (arg->value.str.len==0) {
		RETURN_FALSE;
	}
	somestring = arg->value.str.val;
	len = arg->value.str.len;

	/* build soundex string */
	for (i = 0, small = 0; i < len && small < 4; i++) {
		/* convert chars to upper case and strip non-letter chars */
		/* BUG: should also map here accented letters used in non */
		/* English words or names (also found in English text!): */
		/* esstsett, thorn, n-tilde, c-cedilla, s-caron, ... */
		code = toupper(somestring[i]);
		if (code >= 'A' && code <= 'Z') {
			if (small == 0) {
				/* remember first valid char */
				soundex[small++] = code;
				last = soundex_table[code - 'A'];
			}
			else {
				/* ignore sequences of consonants with same soundex */
				/* code in trail, and vowels unless they separate */
				/* consonant letters */
				code = soundex_table[code - 'A'];
				if (code != last) {
					if (code != 0) {
						soundex[small++] = code;
					}
					last = code;
				}
			}
		}
	}
	/* pad with '0' and terminate with 0 ;-) */
	while (small < 4) {
		soundex[small++] = '0';
	}
	soundex[small] = '\0';

	return_value->value.str.val = estrndup(soundex, small);
	return_value->value.str.len = small;
	return_value->type = IS_STRING;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
