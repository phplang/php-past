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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Jaakko Hyvätti <jaakko.hyvatti@iki.fi>                      |
   +----------------------------------------------------------------------+
 */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "reg.h"
#include "html.h"

typedef struct php_extra_ents {
	int code;
	char str[6];
} php_extra_ents;

void php3_htmlspecialchars(INTERNAL_FUNCTION_PARAMETERS)
{
    YYSTYPE *arg;
    int i, len, maxlen;
    char *old, *new;
	TLS_VARS;
	
#ifdef HARDCODE_ISO_8859_1_AND_DO_SOMETHING_UNNECESSARY_ANYWAY
    static char EntTable[][7] =
    {
	"nbsp","iexcl","cent","pound","curren","yen","brvbar",
	"sect","uml","copy","ordf","laquo","not","shy","reg",
	"macr","deg","plusmn","sup2","sup3","acute","micro",
	"para","middot","cedil","sup1","ordm","raquo","frac14",
	"frac12","frac34","iquest","Agrave","Aacute","Acirc",
	"Atilde","Auml","Aring","AElig","Ccedil","Egrave",
	"Eacute","Ecirc","Euml","Igrave","Iacute","Icirc",
	"Iuml","ETH","Ntilde","Ograve","Oacute","Ocirc","Otilde",
	"Ouml","times","Oslash","Ugrave","Uacute","Ucirc","Uuml",
	"Yacute","THORN","szlig","agrave","aacute","acirc",
	"atilde","auml","aring","aelig","ccedil","egrave",
	"eacute","ecirc","euml","igrave","iacute","icirc",
	"iuml","eth","ntilde","ograve","oacute","ocirc","otilde",
	"ouml","divide","oslash","ugrave","uacute","ucirc",
	"uuml","yacute","thorn","yuml"
    };
#endif

    if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
    }

    convert_to_string(arg);

	maxlen = 2 * arg->strlen;
	if (maxlen < 128)
		maxlen = 128;
	new = emalloc (maxlen);
	len = 0;

	old = arg->value.strval;
	i = arg->strlen;
	while (i--) {
		if (len + 9 > maxlen)
			new = erealloc (new, maxlen += 128);
		if (38 == *old) {
			memcpy (new + len, "&amp;", 5);
			len += 5;
		} else if (34 == *old) {
			memcpy (new + len, "&quot;", 6);
			len += 6;
		} else if (60 == *old) {
			memcpy (new + len, "&lt;", 4);
			len += 4;
		} else if (62 == *old) {
			memcpy (new + len, "&gt;", 4);
			len += 4;
#ifdef HARDCODE_ISO_8859_1_AND_DO_SOMETHING_UNNECESSARY_ANYWAY
		} else if (160 <= *old) {
			new [len++] = '&';
			strcpy (new + len, EntTable [*old - 160]);
			len += strlen (EntTable [*old - 160]);
			new [len++] = ';';
#endif
		} else {
			new [len++] = *old;
		}
		old++;
	}
    new [len] = '\0';

    RETVAL_STRING(new);
    efree(new);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
