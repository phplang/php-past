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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Jaakko Hyvätti <jaakko.hyvatti@iki.fi>                      |
   +----------------------------------------------------------------------+
 */

/* $Id: html.c,v 1.37 2000/09/12 17:23:30 rasmus Exp $ */

#include "php.h"
#include "internal_functions.h"
#include "reg.h"
#include "html.h"

#include <math.h>

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

#ifndef CHARSET_EBCDIC
#define ASC(ch) ch
#else
#define ASC(ch) os_toascii[(unsigned char)ch]
#endif /* CHARSET_EBCDIC */

PHPAPI char * _php3_htmlentities(char *s, int i, int all, int quote_style)
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
		if ('&' == *old) {
			memcpy (new + len, "&amp;", 5);
			len += 5;
		} else if ('"' == *old && !(quote_style&ENT_NOQUOTES)) {
			memcpy (new + len, "&quot;", 6);
			len += 6;
		} else if (39 == *old && (quote_style&ENT_QUOTES)) {
			/* single quote (') */
			memcpy (new + len, "&#039;",6);
			len += 6;		
		} else if ('<' == *old) {
			memcpy (new + len, "&lt;", 4);
			len += 4;
		} else if ('>' == *old) {
			memcpy (new + len, "&gt;", 4);
			len += 4;
		} else if (all && 160 <= ASC(*old)) {
			new [len++] = '&';
			strcpy (new + len, EntTable [ASC(*old) - 160]);
			len += strlen (EntTable [ASC(*old) - 160]);
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
    pval *arg, *quotes;
	char *new;
	int argc, quote_style;
	TLS_VARS;

	argc = ARG_COUNT(ht);

    if ((argc == 1 && getParameters(ht, 1, &arg) == FAILURE) ||
		(argc == 2 && getParameters(ht, 2, &arg, &quotes) == FAILURE) ||
		argc < 1 || argc > 2) {
		WRONG_PARAM_COUNT;
    }

    convert_to_string(arg);
	if(argc==2) {
		convert_to_long(quotes);
		quote_style = quotes->value.lval;	
	} else quote_style = ENT_COMPAT;

	new = _php3_htmlentities(arg->value.str.val, arg->value.str.len, all, quote_style);

	RETVAL_STRINGL(new,strlen(new),0);
}

/* {{{ proto string htmlspecialchars(string string [, int quote_style])
   Convert special characters to HTML entities */
void php3_htmlspecialchars(INTERNAL_FUNCTION_PARAMETERS)
{
/*      _php3_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);*/
	_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto string htmlentities(string string [, int quote_style])
   Convert all applicable characters to HTML entities */
void php3_htmlentities(INTERNAL_FUNCTION_PARAMETERS)
{
/*      _php3_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);*/
	_htmlentities(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto string gamma_correct_tag(string color, double inputgamma, double outputgamma)
   Apply a gamma correction to a HTML color value (#rrggbb) */
void php3_gamma_correct_tag(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *input, *inputgamma, *outputgamma;
	char *rr, *gg, *bb, *output;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &input, &inputgamma, &outputgamma)
		== FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (input->value.str.len < 7) {
		php3_error(E_WARNING, "Color argument to %s() should be in #rrggbb format",
			GLOBAL(function_state).function_name);
		RETURN_FALSE;
	}
 
	convert_to_string(input);
	convert_to_double(inputgamma);
	convert_to_double(outputgamma);

	rr = emalloc(2 + 1);
	gg = emalloc(2 + 1);
	bb = emalloc(2 + 1);
	output = emalloc(7 + 1);
	
	strncpy(rr,input->value.str.val+1,2);
	strncpy(gg,input->value.str.val+3,2);
	strncpy(bb,input->value.str.val+5,2);
	
	efree(rr);
	efree(gg);
	efree(bb);
	
	sprintf(output,"#%02X%02X%02X",
		(int)((pow((pow((strtol(rr, NULL, 16) / 255.0),inputgamma->value.dval)), 1.0 / outputgamma->value.dval) * 255)+.5),
		(int)((pow((pow((strtol(gg, NULL, 16) / 255.0),inputgamma->value.dval)), 1.0 / outputgamma->value.dval) * 255)+.5),
		(int)((pow((pow((strtol(bb, NULL, 16) / 255.0),inputgamma->value.dval)), 1.0 / outputgamma->value.dval) * 255)+.5));

	return_value->type = IS_STRING;
	return_value->value.str.len = strlen(output);
	return_value->value.str.val = output;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
