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
   |          Stig Sæther Bakken <ssb@fast.no>                            |
   +----------------------------------------------------------------------+
 */


/* $Id: php3_string.h,v 1.48 2000/05/23 19:34:47 hholzgra Exp $ */

#ifndef _PHPSTRING_H
#define _PHPSTRING_H

extern char *strtok_string;
extern char *locale_string;

PHP_FUNCTION(bin2hex);
extern void php3_strlen(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strcmp(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strspn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strcspn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strcasecmp(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_str_replace(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_chop(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_trim(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ltrim(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_similar_text(INTERNAL_FUNCTION_PARAMETERS);
extern void soundex(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_levenshtein(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_explode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_implode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtok(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtoupper(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtolower(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_basename(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dirname(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strstr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strpos(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strrpos(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strrchr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_substr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_quotemeta(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ucfirst(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ucwords(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strrev(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hebrev(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hebrev_with_conversion(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_user_sprintf(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_user_printf(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_addslashes(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_stripslashes(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_chr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ord(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_newline_to_br(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_setlocale(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_stristr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_chunk_split(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strip_tags(INTERNAL_FUNCTION_PARAMETERS);

#if HAVE_CRYPT
extern php3_module_entry crypt_module_entry;
#define crypt_module_ptr &crypt_module_entry
extern void php3_crypt(INTERNAL_FUNCTION_PARAMETERS);
extern int php3_minit_crypt(INIT_FUNC_ARGS);
#else
#define crypt_module_ptr NULL
#endif

extern PHPAPI char *_php3_strtoupper(char *s);
extern PHPAPI char *_php3_strtolower(char *s);
#if 0
extern PHPAPI char *_StrTr(char *string, char *str_from, char *str_to);
#endif
extern PHPAPI char *_php3_strtr(char *string, int len, char *str_from, char *str_to, int trlen);
extern PHPAPI char *_php3_addslashes(char *string, int length, int *new_length, int freeit);
extern PHPAPI void _php3_stripslashes(char *string, int *len);
extern PHPAPI void _php3_dirname(char *str, int len);
extern PHPAPI char *php3i_stristr(unsigned char *s, unsigned char *t);
extern PHPAPI void _php3_trim(pval *str, pval * return_value);
extern PHPAPI void _php3_strip_tags(char *rbuf, int len, int state, char *allow);

#if 0
extern PHPAPI char *_php3_str_to_str(char *haystack, int length, 
	char *needle, int needle_len, char *str, int str_len, int *_new_length);
#endif
extern PHPAPI void _php3_char_to_str(char *str,uint len,char from,char *to,int to_len,pval *result);

extern PHPAPI void _php3_implode(pval *delim, pval *arr, pval *return_value);
extern PHPAPI void _php3_explode(pval *delim, pval *str, pval *return_value); 

#endif /* _PHPSTRING_H */
