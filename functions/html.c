/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Jaakko Hyvätti <jaakko.hyvatti@iki.fi>                      |
   +----------------------------------------------------------------------+
 */

/* $Id: html.c,v 1.30 1999/02/11 19:09:34 thies Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"
#include "internal_functions.h"
#include "reg.h"
#include "html.h"

/* This must be fixed to handle the input string according to LC_CTYPE.
   Defaults to ISO-8859-1 for now. */
	
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

PHPAPI char * _php3_htmlentities(char *s, int i, int all)
{
	int len, maxlen;
    unsigned char *old;
	char *new;

	old = (unsigned char *)s;

	maxlen = 2 * i;
	if (maxlen < 128)
		maxlen = 128;
	new = emalloc (maxlen);
	len = 0;

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
		} else if (all && 160 <= *old) {
			new [len++] = '&';
			strcpy (new + len, EntTable [*old - 160]);
			len += strlen (EntTable [*old - 160]);
			new [len++] = ';';
		} else {
			new [len++] = *old;
		}
		old++;
	}
    new [len] = '\0';
	return new;
}

static void _htmlentities(INTERNAL_FUNCTION_PARAMETERS, int all)
{
    pval *arg;
	char *new;
	TLS_VARS;

    if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
    }

    convert_to_string(arg);
 
	new = _php3_htmlentities(arg->value.str.val, arg->value.str.len, all);

	RETVAL_STRINGL(new,strlen(new),0);
}

/* {{{ proto string htmlspecialchars(string string)
   Convert special characters to HTML entities */
void php3_htmlspecialchars(INTERNAL_FUNCTION_PARAMETERS)
{
/*      _php3_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);*/
	_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto string htmlentities(string string)
   Convert all applicable characters to HTML entities */
void php3_htmlentities(INTERNAL_FUNCTION_PARAMETERS)
{
/*      _php3_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);*/
	_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
