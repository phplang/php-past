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
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   |          Zeev Suraski <bourbon@nevision.net.il>                      |
   +----------------------------------------------------------------------+
 */


/* $Id: string.c,v 1.93 1998/01/28 21:50:18 andi Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include <stdio.h>
#include "parser.h"
#include "internal_functions.h"
#include "reg.h"
#include "post.h"
#include "php3_string.h"
#if HAVE_SETLOCALE
#include <locale.h>
#endif

void php3_strlen(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	RETVAL_LONG(str->strlen);
}


void php3_chop(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str;
	register int i;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	if (str->type == IS_STRING) {
		int len = str->strlen;
		char *c = str->value.strval;
		for (i = len - 1; i >= 0; i--) {
			if (c[i] == ' ' || c[i] == '\n' || c[i] == '\r' ||
				c[i] == '\t' || c[i] == '\v') {
				len--;
			} else {
				break;
			}
		}
		RETVAL_STRINGL(c, len);
		return;
	}
	RETURN_FALSE;
}


void php3_explode(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str, *delim;
	char *work_str, *p1, *p2;
	int i = 0;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &delim, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	convert_to_string(delim);

	if (str->type == IS_STRING && delim->type == IS_STRING) {
		if (array_init(return_value) == FAILURE) {
			return;
		}
		work_str = p1 = estrndup(str->value.strval,str->strlen);
		p2 = strstr(p1, delim->value.strval);
		if (p2 == NULL) {
			add_index_string(return_value, i++, p1, 1);
		}
		else do {
			p2[0] = 0;
			add_index_string(return_value, i++, p1, 1);
			p1 = p2 + delim->strlen;
		} while ((p2 = strstr(p1, delim->value.strval)) && p2 != work_str);
		if (p1 != work_str) {
			add_index_string(return_value, i++, p1, 1);
		}
		efree(work_str);
	}
}


void php3_implode(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg1, *delim, *tmp, arr;
	int len = 0, count = 0;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &delim) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	arr = *arg1;
	convert_to_string(delim);
	if (arr.type != IS_ARRAY || delim->type != IS_STRING) {
		php3_error(E_WARNING, "Bad arguments to %s()", GLOBAL(function_state).function_name);
		return;
	}
	yystype_copy_constructor(arg1);

	/* convert everything to strings, and calculate length */
	hash_internal_pointer_reset(arr.value.ht);
	while (hash_get_current_data(arr.value.ht, (void **) &tmp) == SUCCESS) {
		convert_to_string(tmp);
		if (tmp->type == IS_STRING) {
			len += tmp->strlen;
			len += delim->strlen;
			count++;
		}
		hash_move_forward(arr.value.ht);
	}
	len = len - delim->strlen;

	/* do it */
	return_value->value.strval = (char *) emalloc(len + 1);
	return_value->value.strval[0] = 0;
	hash_internal_pointer_reset(arr.value.ht);
	while (hash_get_current_data(arr.value.ht, (void **) &tmp) == SUCCESS) {
		if (tmp->type == IS_STRING) {
			count--;
			strcat(return_value->value.strval, tmp->value.strval);
			if (count > 0) {
				strcat(return_value->value.strval, delim->value.strval);
			}
		}
		hash_move_forward(arr.value.ht);
	}
	return_value->type = IS_STRING;
	return_value->strlen = len;
	hash_destroy(arr.value.ht);
	efree(arr.value.ht);
}

#ifndef THREAD_SAFE
char *strtok_string = NULL;
#endif

void php3_strtok(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str, *tok;
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
	token = tok->value.strval;

	if (argc == 2) {
		convert_to_string(str);

		if (GLOBAL(strtok_string)) {
			efree(GLOBAL(strtok_string));
			GLOBAL(strtok_string) = NULL;
		}
		GLOBAL(strtok_string) = estrndup(str->value.strval,str->strlen);
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
		RETVAL_STRING(STATIC(strtok_pos1));
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
		ch = toupper(*c);
		*c++ = ch;
	}
	return (s);
}


void php3_strtoupper(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	char *ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	ret = _php3_strtoupper(arg->value.strval);
	RETVAL_STRING(ret);
}


PHPAPI char *_php3_strtolower(char *s)
{
	register int ch;
	char *c;

	c = s;
	while (*c) {
		ch = tolower(*c);
		*c++ = ch;
	}
	return (s);
}


void php3_strtolower(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str;
	char *ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	ret = _php3_strtolower(str->value.strval);
	RETVAL_STRING(ret);
}

void php3_basename(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str;
	char *ret, *c;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	ret = estrdup(str->value.strval);
	c = ret + strlen(ret) -1;	
	while(*c=='/') c--;
	*(c+1)='\0';	
	c = strrchr(ret,'/');
	if(c) {
		RETVAL_STRING(c+1);
	} else {
		RETVAL_STRING(str->value.strval);
	}
	efree(ret);
}

PHPAPI void _php3_dirname(char *str) {
	register char *c;

	c = str + strlen(str) -1;	
	while(*c=='/') c--;       /* strip trailing /'s */
	*(c+1)='\0';	
	c = strrchr(str,'/');
	if(c) *c='\0';
}

void php3_dirname(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str;
	char *ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str)) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	ret = estrdup(str->value.strval);
	_php3_dirname(ret);
	RETVAL_STRING(ret);
	efree(ret);
}

void php3_strstr(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *haystack, *needle;
	char *found = NULL;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &haystack, &needle) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(haystack);

	if (needle->type == IS_STRING) {
		found = strstr(haystack->value.strval, needle->value.strval);
	} else {
		char b[2];
		convert_to_long(needle);
		sprintf(b, "%c", (char) needle->value.lval);
		found = strstr(haystack->value.strval, b);
	}


	if (found) {
		RETVAL_STRING(found);
	} else {
		RETVAL_FALSE;
	}
}


void php3_strrchr(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *haystack, *needle;
	char *found = NULL;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &haystack, &needle) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(haystack);

	if (needle->type == IS_STRING) {
		found = strrchr(haystack->value.strval, *needle->value.strval);
	} else {

		convert_to_long(needle);
		found = strrchr(haystack->value.strval, needle->value.lval);
	}


	if (found) {
		RETVAL_STRING(found);
	} else {
		RETVAL_FALSE;
	}
}


/* args s,m,n */
void php3_substr(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *string, *from, *len;
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
		l = string->strlen;
	} else {
		convert_to_long(len);
		l = len->value.lval;
	}

	/* if "from" position is negative, count start position from the end
	 * of the string
	 */
	if (f < 0) {
		f = string->strlen + f;
		if (f < 0) {
			f = 0;
		}
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string
	 */
	if (l < 0) {
		l = (string->strlen - f) + l;
		if (l < 0) {
			l = 0;
		}
	}

	if (f >= (int)string->strlen) {
		RETURN_FALSE;
	}

	if ((f + l) < (int)string->strlen) {
		string->value.strval[f + l] = '\0';
	}
	RETVAL_STRING(string->value.strval + f);
}


void php3_quotemeta(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	register int x, y;
	char *str, *old;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	old = arg->value.strval;

	if (!*old) {
		RETURN_FALSE;
	}
	str = emalloc(2 * strlen(old) + 1);
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
	RETVAL_STRING(str);
	efree(str);
}


void php3_ord(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	RETVAL_LONG(str->value.strval[0]);
}


void php3_chr(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *num;
	char temp[2];
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(num);
	temp[0] = (char) num->value.lval;
	temp[1] = '\0';
	RETVAL_STRINGL(temp, 1);
}


void php3_ucfirst(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	if (!*arg->value.strval) {
		RETURN_FALSE;
	}
	*arg->value.strval = toupper(*arg->value.strval);
	RETVAL_STRING(arg->value.strval);
}


void php3_strtr(INTERNAL_FUNCTION_PARAMETERS)
{								/* strtr(STRING,FROM,TO) */
	YYSTYPE *str, *from, *to;
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

	string = (unsigned char*) str->value.strval;
	str_from = (unsigned char*) from->value.strval;
	str_to = (unsigned char*) to->value.strval;

	len1 = strlen(str_from);
	len2 = strlen(str_to);

	if (len1 > len2) {
		str_from[len2] = '\0';
		len1 = len2;
	}
	for (i = 0; i < 256; xlat[i] = i, i++);

	for (i = 0; i < len1; i++) {
		xlat[(unsigned char) str_from[i]] = str_to[i];
	}

	for (i = 0; i < str->strlen; i++) {
		string[i] = xlat[(unsigned char) string[i]];
	}

	RETVAL_STRING((char *)string);
}



void php3_strrev(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str;
	int i,len;
	char c;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &str)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(str);
	
	len = str->strlen;
	
	for (i=0; i<len-1-i; i++) {
		c=str->value.strval[i];
		str->value.strval[i] = str->value.strval[len-1-i];
		str->value.strval[len-1-i]=c;
	}

	*return_value = *str;
	yystype_copy_constructor(return_value);
}


/* be careful, this edits the string in-place */
PHPAPI void _php3_stripslashes(char *string)
{
	char *s, *t;
	int l;

	l = strlen(string);
	s = string;
	t = string;
	while (l > 0 && *t != '\0') {
		if (*t == '\\') {
			t++;				/* skip the slash */
			l--;
			if (l > 0 && *t != '\0') {
				*s++ = *t++;	/* preserve the next character */
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
	if (s != t)
		*s = '\0';
}


void php3_addslashes(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	return_value->value.strval = _php3_addslashes(str->value.strval,0);
	return_value->strlen = strlen(return_value->value.strval);
	return_value->type = IS_STRING;
}

void php3_stripslashes(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *str;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	/* let RETVAL do the estrdup() */
	RETVAL_STRING(str->value.strval);
	_php3_stripslashes(return_value->value.strval);
	/* but don't forget to recalc the strlen! */
	return_value->strlen = strlen(return_value->value.strval);
}

#ifndef HAVE_STRERROR
char *strerror(int errnum) {
	extern int sys_nerr;
	extern char *sys_errlist[];
#ifndef THREAD_SAFE
	static char str_ebuf[40];
#endif

	if((unsigned int)errnum < sys_nerr) return(sys_errlist[errnum]);
	(void)sprintf(STATIC(str_ebuf), "Unknown error: %d", errnum);
	return(STATIC(str_ebuf));
}
#endif


PHPAPI char *_php3_addslashes(char *str, int should_free)
{
	int string_length = strlen(str);
	char *new_str = (char *) emalloc(string_length*2+1); /* maximum string length, worst case situation */
	register char *source,*target;
	
	for (source=str,target=new_str; *source; source++) {
		switch(*source) {
			case '\'':
				if (php3_ini.magic_quotes_sybase) {
					*target = '\'';
					target++;
					*target = '\'';
					target++;
					break;
				}
				/* break is missing *intentionally* */
			case '\"':
			case '\\':
				*target = '\\';
				target++;
				/* break is missing *intentionally* */
			default:
				*target = *source;
				target++;
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

static void _php3_char_to_str(char *str,uint len,char from,char *to,int to_len,YYSTYPE *result)
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
		result->value.strval = estrndup(str,len);
		result->strlen = len;
		return;
	}
	
	result->strlen = len+char_count*to_len+1;
	result->value.strval = target = (char *) emalloc(result->strlen);
	
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


static void _php3_hebrev(INTERNAL_FUNCTION_PARAMETERS,int convert_newlines)
{
	YYSTYPE *str,*max_chars_per_line;
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
	
	if (str->strlen==0) {
		RETURN_FALSE;
	}

	tmp = str->value.strval;
	block_start=block_end=0;
	block_ended=0;

	heb_str = (char *) emalloc(str->strlen+1);
	target = heb_str+str->strlen;
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
			while((isheb((int)*(tmp+1)) || _isblank((int)*(tmp+1)) || ispunct((int)*(tmp+1)) || (int)*(tmp+1)=='\n' ) && block_end<str->strlen-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			for (i=block_start; i<=block_end; i++) {
				*target = str->value.strval[i];
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
			while(!isheb(*(tmp+1)) && (int)*(tmp+1)!='\n' && block_end<str->strlen-1) {
				tmp++;
				block_end++;
				block_length++;
			}
			while ((_isblank((int)*tmp) || ispunct((int)*tmp)) && *tmp!='/' && *tmp!='-' && block_end>block_start) {
				tmp--;
				block_end--;
			}
			for (i=block_end; i>=block_start; i--) {
				*target = str->value.strval[i];
				target--;
			}
			block_type = _HEB_BLOCK_TYPE_HEB;
		}
		block_start=block_end+1;
	} while(block_end<str->strlen-1);


	broken_str = (char *) emalloc(str->strlen+1);
	begin=end=str->strlen-1;
	target = broken_str;
		
	while (1) {
		char_count=0;
		while (!max_chars || char_count<max_chars) {
			char_count++;
			begin--;
			if (begin==0 || _isnewline(heb_str[begin])) {
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

		if (begin==0) {
			*target = 0;
			break;
		}
		begin--;
		end=begin;
	}
	efree(heb_str);

	if (convert_newlines) {
		_php3_char_to_str(broken_str,str->strlen,'\n',"<br>\n",5,return_value);
		efree(broken_str);
	} else {
		return_value->value.strval = broken_str;
		return_value->strlen = str->strlen;
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
	YYSTYPE *str;
	
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &str)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(str);
	
	_php3_char_to_str(str->value.strval,str->strlen,'\n',"<br>\n",5,return_value);
}

void php3_setlocale(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *category, *locale;
	int cat;
	char *loc, *retval;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &category, &locale)==FAILURE)
		WRONG_PARAM_COUNT;
#if HAVE_SETLOCALE
	convert_to_string(category);
	convert_to_string(locale);
	if (!strcasecmp ("LC_ALL", category->value.strval))
		cat = LC_ALL;
	else if (!strcasecmp ("LC_COLLATE", category->value.strval))
		cat = LC_COLLATE;
	else if (!strcasecmp ("LC_CTYPE", category->value.strval))
		cat = LC_CTYPE;
	else if (!strcasecmp ("LC_MONETARY", category->value.strval))
		cat = LC_MONETARY;
	else if (!strcasecmp ("LC_NUMERIC", category->value.strval))
		cat = LC_NUMERIC;
	else if (!strcasecmp ("LC_TIME", category->value.strval))
		cat = LC_TIME;
	else {
		php3_error(E_WARNING,"Invalid locale category name %s, must be one of LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY, LC_NUMERIC or LC_TIME", category->value.strval);
		RETURN_FALSE;
	}
	if (!strcmp ("0", locale->value.strval))
		loc = NULL;
	else
		loc = locale->value.strval;
	retval = setlocale (cat, loc);
	if (retval) {
		RETVAL_STRING(retval);
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
