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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   |          Zeev Suraski <bourbon@nevision.net.il>                      |
   +----------------------------------------------------------------------+
 */


/* $Id: string.c,v 1.140 1998/09/18 16:55:10 rasmus Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include <stdio.h>
#include "php.h"
#include "internal_functions.h"
#include "reg.h"
#include "post.h"
#include "php3_string.h"
#if HAVE_SETLOCALE
#include <locale.h>
#endif

void php3_strlen(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	RETVAL_LONG(str->value.str.len);
}


void php3_strcmp(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *s1,*s2;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(s1);
	convert_to_string(s2);
	RETURN_LONG(php3_binary_strcmp(s1,s2));
}


void php3_strcasecmp(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *s1,*s2;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(s1);
	convert_to_string(s2);
	RETURN_LONG(strcasecmp(s1->value.str.val,s2->value.str.val));
}

void php3_strspn(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *s1,*s2;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(s1);
	convert_to_string(s2);
	RETURN_LONG(strspn(s1->value.str.val,s2->value.str.val));
}

void php3_strcspn(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *s1,*s2;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &s1, &s2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(s1);
	convert_to_string(s2);
	RETURN_LONG(strcspn(s1->value.str.val,s2->value.str.val));
}


void php3_chop(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	register int i;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	if (str->type == IS_STRING) {
		int len = str->value.str.len;
		char *c = str->value.str.val;
		for (i = len - 1; i >= 0; i--) {
			if (c[i] == ' ' || c[i] == '\n' || c[i] == '\r' ||
				c[i] == '\t' || c[i] == '\v') {
				len--;
			} else {
				break;
			}
		}
		RETVAL_STRINGL(c, len, 1);
		return;
	}
	RETURN_FALSE;
}

void php3_trim(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	register int i;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	if (str->type == IS_STRING) {
		int len = str->value.str.len;
		int trimmed = 0;
		char *c = str->value.str.val;
		for (i = 0; i < len; i++) {
			if (c[i] == ' ' || c[i] == '\n' || c[i] == '\r' ||
				c[i] == '\t' || c[i] == '\v') {
				trimmed++;
			} else {
				break;
			}
		}
		len-=trimmed;
		c+=trimmed;
		for (i = len - 1; i >= 0; i--) {
			if (c[i] == ' ' || c[i] == '\n' || c[i] == '\r' ||
				c[i] == '\t' || c[i] == '\v') {
				len--;
			} else {
				break;
			}
		}
		RETVAL_STRINGL(c, len, 1);
		return;
	}
	RETURN_FALSE;
}

void php3_ltrim(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	register int i;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	if (str->type == IS_STRING) {
		int len = str->value.str.len;
		int trimmed = 0;
		char *c = str->value.str.val;
		for (i = 0; i < len; i++) {
			if (c[i] == ' ' || c[i] == '\n' || c[i] == '\r' ||
				c[i] == '\t' || c[i] == '\v') {
				trimmed++;
			} else {
				break;
			}
		}
		RETVAL_STRINGL(c+trimmed, len-trimmed, 1);
		return;
	}
	RETURN_FALSE;
}

void php3_explode(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str, *delim;
	char *work_str, *p1, *p2;
	int i = 0;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &delim, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	convert_to_string(delim);

	if (strlen(delim->value.str.val)==0) {
		/* the delimiter must be a valid C string that's at least 1 character long */
		php3_error(E_WARNING,"Empty delimiter");
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		return;
	}
	work_str = p1 = estrndup(str->value.str.val,str->value.str.len);
	p2 = strstr(p1, delim->value.str.val);
	if (p2 == NULL) {
		add_index_string(return_value, i++, p1, 1);
	} else do {
		p2[0] = 0;
		add_index_string(return_value, i++, p1, 1);
		p1 = p2 + delim->value.str.len;
	} while ((p2 = strstr(p1, delim->value.str.val)) && p2 != work_str);
	if (p1 != work_str) {
		add_index_string(return_value, i++, p1, 1);
	}
	efree(work_str);
}


void php3_implode(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1, *arg2, *delim, *tmp, *arr;
	int len = 0, count = 0;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (arg1->type == IS_ARRAY && arg2->type == IS_STRING) {
		arr = arg1;
		delim = arg2;
	} else if (arg2->type == IS_ARRAY) {
		convert_to_string(arg1);
		arr = arg2;
		delim = arg1;
	} else {
		php3_error(E_WARNING, "Bad arguments to %s()",
				   GLOBAL(function_state).function_name);
		return;
	}

	/* convert everything to strings, and calculate length */
	_php3_hash_internal_pointer_reset(arr->value.ht);
	while (_php3_hash_get_current_data(arr->value.ht, (void **) &tmp) == SUCCESS) {
		convert_to_string(tmp);
		if (tmp->type == IS_STRING) {
			len += tmp->value.str.len;
			if (count>0) {
				len += delim->value.str.len;
			}
			count++;
		}
		_php3_hash_move_forward(arr->value.ht);
	}

	/* do it */
	return_value->value.str.val = (char *) emalloc(len + 1);
	return_value->value.str.val[0] = '\0';
	return_value->value.str.val[len] = '\0';
	_php3_hash_internal_pointer_reset(arr->value.ht);
	while (_php3_hash_get_current_data(arr->value.ht, (void **) &tmp) == SUCCESS) {
		if (tmp->type == IS_STRING) {
			count--;
			strcat(return_value->value.str.val, tmp->value.str.val);
			if (count > 0) {
				strcat(return_value->value.str.val, delim->value.str.val);
			}
		}
		_php3_hash_move_forward(arr->value.ht);
	}
	return_value->type = IS_STRING;
	return_value->value.str.len = len;
}

#ifndef THREAD_SAFE
char *strtok_string;
#endif

void php3_strtok(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str, *tok;
#ifndef THREAD_SAFE
	static char *strtok_pos1 = NULL;
	static char *strtok_pos2 = NULL;
#endif
	char *token = NULL;
	char *first = NULL;
	int argc;
	TLS_VARS;
	
	argc = ARG_COUNT(ht);

	if ((argc == 1 && getParameters(ht, 1, &tok) == FAILURE) ||
		(argc == 2 && getParameters(ht, 2, &str, &tok) == FAILURE) ||
		argc < 1 || argc > 2) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(tok);
	token = tok->value.str.val;

	if (argc == 2) {
		convert_to_string(str);

		STR_FREE(GLOBAL(strtok_string));
		GLOBAL(strtok_string) = estrndup(str->value.str.val,str->value.str.len);
		STATIC(strtok_pos1) = GLOBAL(strtok_string);
		STATIC(strtok_pos2) = NULL;
	}
	if (STATIC(strtok_pos1) && *STATIC(strtok_pos1)) {
		for ( /* NOP */ ; token && *token; token++) {
			STATIC(strtok_pos2) = strchr(STATIC(strtok_pos1), (int) *token);
			if (!first || (STATIC(strtok_pos2) && STATIC(strtok_pos2) < first)) {
				first = STATIC(strtok_pos2);
			}
		}						/* NB: token is unusable now */
		STATIC(strtok_pos2) = first;
		if (STATIC(strtok_pos2)) {
			*STATIC(strtok_pos2) = '\0';
		}
		RETVAL_STRING(STATIC(strtok_pos1),1);
		if (STATIC(strtok_pos2))
			STATIC(strtok_pos1) = STATIC(strtok_pos2) + 1;
		else
			STATIC(strtok_pos1) = NULL;
	} else {
		RETVAL_FALSE;
	}
}

PHPAPI char *_php3_strtoupper(char *s)
{
	char *c;
	int ch;

	c = s;
	while (*c) {
		ch = toupper((unsigned char)*c);
		*c++ = ch;
	}
	return (s);
}


void php3_strtoupper(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	char *ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	ret = _php3_strtoupper(arg->value.str.val);
	RETVAL_STRING(ret,1);
}


PHPAPI char *_php3_strtolower(char *s)
{
	register int ch;
	char *c;

	c = s;
	while (*c) {
		ch = tolower((unsigned char)*c);
		*c++ = ch;
	}
	return (s);
}


void php3_strtolower(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	char *ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	ret = _php3_strtolower(str->value.str.val);
	RETVAL_STRING(ret,1);
}

void php3_basename(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	char *ret, *c;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	ret = estrdup(str->value.str.val);
	c = ret + str->value.str.len -1;	
	while (*c == '/'
#ifdef MSVC5
		   || *c == '\\'
#endif
		)
		c--;
	*(c + 1) = '\0';	
	if ((c = strrchr(ret, '/'))
#ifdef MSVC5
		|| (c = strrchr(ret, '\\'))
#endif
		) {
		RETVAL_STRING(c + 1,1);
	} else {
		RETVAL_STRING(str->value.str.val,1);
	}
	efree(ret);
}

PHPAPI void _php3_dirname(char *str, int len) {
	register char *c;

	c = str + len - 1;
	while (*c == '/'
#ifdef MSVC5
		   || *c == '\\'
#endif
		)
		c--; /* strip trailing slashes */
	*(c + 1) = '\0';
	if ((c = strrchr(str, '/'))
#ifdef MSVC5
		|| (c = strrchr(str, '\\'))
#endif
		)
		*c='\0';
}

void php3_dirname(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	char *ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	ret = estrdup(str->value.str.val);
	_php3_dirname(ret,str->value.str.len);
	RETVAL_STRING(ret,1);
	efree(ret);
}

void php3_strstr(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *haystack, *needle;
	char *found = NULL;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &haystack, &needle) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(haystack);

	if (needle->type == IS_STRING) {
		if (strlen(needle->value.str.val)==0) {
			php3_error(E_WARNING,"Empty delimiter");
			RETURN_FALSE;
		}
		found = strstr(haystack->value.str.val, needle->value.str.val);
	} else {
		convert_to_long(needle);
		found = strchr(haystack->value.str.val, (char) needle->value.lval);
	}


	if (found) {
		RETVAL_STRING(found,1);
	} else {
		RETVAL_FALSE;
	}
}

void php3_strpos(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *haystack, *needle;
	char *found = NULL;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &haystack, &needle) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(haystack);

	if (needle->type == IS_STRING) {
		if (needle->value.str.len==0) {
			php3_error(E_WARNING,"Empty delimiter");
			RETURN_FALSE;
		}
		found = strstr(haystack->value.str.val, needle->value.str.val);
	} else {
		convert_to_long(needle);
		found = strchr(haystack->value.str.val, (char) needle->value.lval);
	}

	if (found) {
		RETVAL_LONG(haystack->value.str.len - strlen(found));
	} else {
		RETVAL_FALSE;
	}
}

void php3_strrpos(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *haystack, *needle;
	char *found = NULL;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &haystack, &needle) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(haystack);

	if (needle->type == IS_STRING) {
		found = strrchr(haystack->value.str.val, *needle->value.str.val);
	} else {
		convert_to_long(needle);
		found = strrchr(haystack->value.str.val, (char) needle->value.lval);
	}

	if (found) {
		RETVAL_LONG(haystack->value.str.len - strlen(found));
	} else {
		RETVAL_FALSE;
	}
}

void php3_strrchr(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *haystack, *needle;
	char *found = NULL;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &haystack, &needle) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(haystack);

	if (needle->type == IS_STRING) {
		found = strrchr(haystack->value.str.val, *needle->value.str.val);
	} else {

		convert_to_long(needle);
		found = strrchr(haystack->value.str.val, needle->value.lval);
	}


	if (found) {
		RETVAL_STRING(found,1);
	} else {
		RETVAL_FALSE;
	}
}


/* args s,m,n */
void php3_substr(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *string, *from, *len;
	int argc, l;
	int f;
	TLS_VARS;
	
	argc = ARG_COUNT(ht);

	if ((argc == 2 && getParameters(ht, 2, &string, &from) == FAILURE) ||
		(argc == 3 && getParameters(ht, 3, &string, &from, &len) == FAILURE) ||
		argc < 2 || argc > 3) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(string);
	convert_to_long(from);
	f = from->value.lval;

	if (argc == 2) {
		l = string->value.str.len;
	} else {
		convert_to_long(len);
		l = len->value.lval;
	}

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (f < 0) {
		f = string->value.str.len + f;
		if (f < 0) {
			f = 0;
		}
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (l < 0) {
		l = (string->value.str.len - f) + l;
		if (l < 0) {
			l = 0;
		}
	}

	if (f >= (int)string->value.str.len) {
		RETURN_FALSE;
	}

	if ((f + l) < (int)string->value.str.len) {
		string->value.str.val[f + l] = '\0';
	}
	RETVAL_STRING(string->value.str.val + f,1);
}


void php3_quotemeta(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	register int x, y;
	char *str, *old;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	old = arg->value.str.val;

	if (!*old) {
		RETURN_FALSE;
	}
	str = emalloc(2 * arg->value.str.len + 1);
	for (x = 0, y = 0; old[x]; x++, y++) {
		switch (old[x]) {
			case '.':
			case '\\':
			case '+':
			case '*':
			case '?':
			case '[':
			case '^':
			case ']':
			case '$':
			case '(':
			case ')':
				str[y++] = '\\';
		}
		str[y] = old[x];
	}
	str[y] = '\0';
	RETVAL_STRING(str,1);
	efree(str);
}


void php3_ord(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	RETVAL_LONG((unsigned char)str->value.str.val[0]);
}


void php3_chr(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *num;
	char temp[2];
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(num);
	temp[0] = (char) num->value.lval;
	temp[1] = '\0';
	RETVAL_STRINGL(temp, 1,1);
}


void php3_ucfirst(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	if (!*arg->value.str.val) {
		RETURN_FALSE;
	}
	*arg->value.str.val = toupper((unsigned char)*arg->value.str.val);
	RETVAL_STRING(arg->value.str.val,1);
}

void php3_ucwords(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	char *r;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	if (!*arg->value.str.val) {
		RETURN_FALSE;
	}
	*arg->value.str.val = toupper((unsigned char)*arg->value.str.val);
	r=arg->value.str.val;
	while((r=strstr(r," "))){
		if(*(r+1)){
			r++;
			*r=toupper((unsigned char)*r);
		}
	}
	RETVAL_STRING(arg->value.str.val,1);
}

void php3_strtr(INTERNAL_FUNCTION_PARAMETERS)
{								/* strtr(STRING,FROM,TO) */
	pval *str, *from, *to;
	unsigned char xlat[256];
	unsigned char *str_from, *str_to, *string;
	int i, len1, len2;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &str, &from, &to) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	convert_to_string(from);
	convert_to_string(to);

	string = (unsigned char*) str->value.str.val;
	str_from = (unsigned char*) from->value.str.val;
	str_to = (unsigned char*) to->value.str.val;

	len1 = from->value.str.len;
	len2 = to->value.str.len;

	if (len1 > len2) {
		str_from[len2] = '\0';
		len1 = len2;
	}
	for (i = 0; i < 256; xlat[i] = i, i++);

	for (i = 0; i < len1; i++) {
		xlat[(unsigned char) str_from[i]] = str_to[i];
	}

	for (i = 0; i < str->value.str.len; i++) {
		string[i] = xlat[(unsigned char) string[i]];
	}

	RETVAL_STRING((char *)string,1);
}



void php3_strrev(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	int i,len;
	char c;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &str)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(str);
	
	len = str->value.str.len;
	
	for (i=0; i<len-1-i; i++) {
		c=str->value.str.val[i];
		str->value.str.val[i] = str->value.str.val[len-1-i];
		str->value.str.val[len-1-i]=c;
	}

	*return_value = *str;
	pval_copy_constructor(return_value);
}


/* be careful, this edits the string in-place */
PHPAPI void _php3_stripslashes(char *string, int *len)
{
	char *s, *t;
	int l;
	char escape_char='\\';

	if (php3_ini.magic_quotes_sybase) {
		escape_char='\'';
	}

	if (len != NULL) {
		l = *len;
	} else {
		l = strlen(string);
	}
	s = string;
	t = string;
	while (l > 0) {
		if (*t == escape_char) {
			t++;				/* skip the slash */
			if (len != NULL)
				(*len)--;
			l--;
			if (l > 0) {
				if(*t=='0') {
					*s++='\0';
					t++;
				} else {
					*s++ = *t++;	/* preserve the next character */
				}
				l--;
			}
		} else {
			if (s != t)
				*s++ = *t++;
			else {
				s++;
				t++;
			}
			l--;
		}
	}
	if (s != t) {
		*s = '\0';
	}
}


void php3_addslashes(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	return_value->value.str.val = _php3_addslashes(str->value.str.val,str->value.str.len,&return_value->value.str.len,0);
	return_value->type = IS_STRING;
}

void php3_stripslashes(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	/* let RETVAL do the estrdup() */
	RETVAL_STRING(str->value.str.val,1);
	_php3_stripslashes(return_value->value.str.val,&return_value->value.str.len);
}

#ifndef HAVE_STRERROR
#if !APACHE
char *strerror(int errnum) 
{
	extern int sys_nerr;
	extern char *sys_errlist[];
#ifndef THREAD_SAFE
	static char str_ebuf[40];
#endif

	if ((unsigned int)errnum < sys_nerr) return(sys_errlist[errnum]);
	(void)sprintf(STATIC(str_ebuf), "Unknown error: %d", errnum);
	return(STATIC(str_ebuf));
}
#endif
#endif


PHPAPI char *_php3_addslashes(char *str, int length, int *new_length, int should_free)
{
	char *new_str = (char *) emalloc((length?length:strlen(str))*2+1); /* maximum string length, worst case situation */
	register char *source,*target;
	char *end;
	
	if (new_length) {
		*new_length=0;
	}
	for (source=str,end=source+length,target=new_str; *source || source<end; source++) {
		switch(*source) {
			case '\0':
				*target++ = '\\';
				*target++ = '0';
				if (new_length) {
					(*new_length) +=2;
				}
				break;
			case '\'':
				if (php3_ini.magic_quotes_sybase) {
					*target++ = '\'';
					*target++ = '\'';
					if (new_length) {
						(*new_length) += 2;
					}
					break;
				}
				/* break is missing *intentionally* */
			case '\"':
			case '\\':
				if (!php3_ini.magic_quotes_sybase) {
					*target++ = '\\';
					if (new_length) {
						(*new_length)++;
					}
				}
				/* break is missing *intentionally* */
			default:
				*target++ = *source;
				if (new_length) {
					(*new_length)++;
				}
				break;
		}
	}
	*target = 0;
	if (should_free) {
		STR_FREE(str);
	}
	return new_str;
}


#define _HEB_BLOCK_TYPE_ENG 1
#define _HEB_BLOCK_TYPE_HEB 2
#define isheb(c) (((((unsigned char) c)>=224) && (((unsigned char) c)<=250)) ? 1 : 0)
#define _isblank(c) (((((unsigned char) c)==' ' || ((unsigned char) c)=='\t')) ? 1 : 0)
#define _isnewline(c) (((((unsigned char) c)=='\n' || ((unsigned char) c)=='\r')) ? 1 : 0)

static void _php3_char_to_str(char *str,uint len,char from,char *to,int to_len,pval *result)
{
	int char_count=0;
	char *source,*target,*tmp;
	
	for (source=str; *source; source++) {
		if (*source==from) {
			char_count++;
		}
	}

	result->type = IS_STRING;
		
	if (char_count==0) {
		result->value.str.val = estrndup(str,len);
		result->value.str.len = len;
		return;
	}
	
	result->value.str.len = len+char_count*(to_len-1);
	result->value.str.val = target = (char *) emalloc(result->value.str.len+1);
	
	for (source=str; *source; source++) {
		if (*source==from) {
			for (tmp=to; *tmp; tmp++) {
				*target = *tmp;
				target++;
			}
		} else {
			*target = *source;
			target++;
		}
	}
	*target = *source;
}



/* Converts Logical Hebrew text (Hebrew Windows style) to Visual text
 * Cheers/complaints/flames - Zeev Suraski <zeev@php.net>
 */
static void _php3_hebrev(INTERNAL_FUNCTION_PARAMETERS,int convert_newlines)
{
	pval *str,*max_chars_per_line;
	char *heb_str,*tmp,*target,*opposite_target,*broken_str;
	int block_start, block_end, block_type, block_length, i;
	int block_ended;
	long max_chars=0;
	int begin,end,char_count,orig_begin;

	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &str)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (getParameters(ht, 2, &str, &max_chars_per_line)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(max_chars_per_line);
			max_chars = max_chars_per_line->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_string(str);
	
	if (str->value.str.len==0) {
		RETURN_FALSE;
	}

	tmp = str->value.str.val;
	block_start=block_end=0;
	block_ended=0;

	heb_str = (char *) emalloc(str->value.str.len+1);
	target = heb_str+str->value.str.len;
	opposite_target = heb_str;
	*target = 0;
	target--;

	block_length=0;

	if (isheb(*tmp)) {
		block_type = _HEB_BLOCK_TYPE_HEB;
	} else {
		block_type = _HEB_BLOCK_TYPE_ENG;
	}
	
	do {
		if (block_type==_HEB_BLOCK_TYPE_HEB) {
			while((isheb((int)*(tmp+1)) || _isblank((int)*(tmp+1)) || ispunct((int)*(tmp+1)) || (int)*(tmp+1)=='\n' ) && block_end<str->value.str.len-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			for (i=block_start; i<=block_end; i++) {
				*target = str->value.str.val[i];
				switch (*target) {
					case '(':
						*target = ')';
						break;
					case ')':
						*target = '(';
						break;
					default:
						break;
				}
				target--;
			}
			block_type = _HEB_BLOCK_TYPE_ENG;
		} else {
			while(!isheb(*(tmp+1)) && (int)*(tmp+1)!='\n' && block_end<str->value.str.len-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			while ((_isblank((int)*tmp) || ispunct((int)*tmp)) && *tmp!='/' && *tmp!='-' && block_end>block_start) {
				tmp--;
				block_end--;
			}
			for (i=block_end; i>=block_start; i--) {
				*target = str->value.str.val[i];
				target--;
			}
			block_type = _HEB_BLOCK_TYPE_HEB;
		}
		block_start=block_end+1;
	} while(block_end<str->value.str.len-1);


	broken_str = (char *) emalloc(str->value.str.len+1);
	begin=end=str->value.str.len-1;
	target = broken_str;
		
	while (1) {
		char_count=0;
		while ((!max_chars || char_count<max_chars) && begin>0) {
			char_count++;
			begin--;
			if (begin<=0 || _isnewline(heb_str[begin])) {
				while(begin>0 && _isnewline(heb_str[begin-1])) {
					begin--;
					char_count++;
				}
				break;
			}
		}
		if (char_count==max_chars) { /* try to avoid breaking words */
			int new_char_count=char_count, new_begin=begin;
			
			while (new_char_count>0) {
				if (_isblank(heb_str[new_begin]) || _isnewline(heb_str[new_begin])) {
					break;
				}
				new_begin++;
				new_char_count--;
			}
			if (new_char_count>0) {
				char_count=new_char_count;
				begin=new_begin;
			}
		}
		orig_begin=begin;
		
		if (_isblank(heb_str[begin])) {
			heb_str[begin]='\n';
		}
		while (begin<=end && _isnewline(heb_str[begin])) { /* skip leading newlines */
			begin++;
		}
		for (i=begin; i<=end; i++) { /* copy content */
			*target = heb_str[i];
			target++;
		}
		for (i=orig_begin; i<=end && _isnewline(heb_str[i]); i++) {
			*target = heb_str[i];
			target++;
		}
		begin=orig_begin;

		if (begin<=0) {
			*target = 0;
			break;
		}
		begin--;
		end=begin;
	}
	efree(heb_str);

	if (convert_newlines) {
		_php3_char_to_str(broken_str,str->value.str.len,'\n',"<br>\n",5,return_value);
		efree(broken_str);
	} else {
		return_value->value.str.val = broken_str;
		return_value->value.str.len = str->value.str.len;
		return_value->type = IS_STRING;
	}
}


void php3_hebrev(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_hebrev(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

void php3_hebrev_with_conversion(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_hebrev(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}


void php3_newline_to_br(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str;
	
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &str)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(str);
	
	_php3_char_to_str(str->value.str.val,str->value.str.len,'\n',"<br>\n",5,return_value);
}

void php3_setlocale(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *category, *locale;
	int cat;
	char *loc, *retval;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &category, &locale)==FAILURE)
		WRONG_PARAM_COUNT;
#if HAVE_SETLOCALE
	convert_to_string(category);
	convert_to_string(locale);
	if (!strcasecmp ("LC_ALL", category->value.str.val))
		cat = LC_ALL;
	else if (!strcasecmp ("LC_COLLATE", category->value.str.val))
		cat = LC_COLLATE;
	else if (!strcasecmp ("LC_CTYPE", category->value.str.val))
		cat = LC_CTYPE;
	else if (!strcasecmp ("LC_MONETARY", category->value.str.val))
		cat = LC_MONETARY;
	else if (!strcasecmp ("LC_NUMERIC", category->value.str.val))
		cat = LC_NUMERIC;
	else if (!strcasecmp ("LC_TIME", category->value.str.val))
		cat = LC_TIME;
	else {
		php3_error(E_WARNING,"Invalid locale category name %s, must be one of LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC or LC_TIME", category->value.str.val);
		RETURN_FALSE;
	}
	if (!strcmp ("0", locale->value.str.val))
		loc = NULL;
	else
		loc = locale->value.str.val;
	retval = setlocale (cat, loc);
	if (retval) {
		RETVAL_STRING(retval,1);
		return;
	}
#endif
	RETURN_FALSE;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
