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
   +----------------------------------------------------------------------+
 */


/* $Id: php3_string.h,v 1.8 1998/02/20 15:17:24 zeev Exp $ */

#ifndef _PHPSTRING_H
#define _PHPSTRING_H

#ifndef THREAD_SAFE
extern char *strtok_string;
#endif

extern void php3_strlen(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_chop(INTERNAL_FUNCTION_PARAMETERS);
extern void soundex(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_explode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_implode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtok(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtoupper(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtolower(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_basename(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dirname(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strstr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strrchr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_substr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_quotemeta(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ucfirst(INTERNAL_FUNCTION_PARAMETERS);
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

#if HAVE_CRYPT
extern php3_module_entry crypt_module_entry;
#define crypt_module_ptr &crypt_module_entry
extern void php3_crypt(INTERNAL_FUNCTION_PARAMETERS);
#else
#define crypt_module_ptr NULL
#endif

extern PHPAPI char *_php3_strtoupper(char *s);
extern PHPAPI char *_php3_strtolower(char *s);
extern char *_StrTr(char *string, char *str_from, char *str_to);
extern PHPAPI char *_php3_addslashes(char *string, int freeit);
extern PHPAPI void _php3_stripslashes(char *string);
extern PHPAPI void _php3_dirname(char *str);

#endif /* _PHPSTRING_H */
