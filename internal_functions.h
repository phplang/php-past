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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: internal_functions.h,v 1.117 1998/01/23 01:29:39 zeev Exp $ */


#ifndef _INTERNAL_FUNCTIONS_H
#define _INTERNAL_FUNCTIONS_H

#include "mod_php3.h"
#include "modules.h"

extern unsigned char first_arg_force_ref[];
extern unsigned char first_arg_allow_ref[];
extern unsigned char second_arg_force_ref[];
extern unsigned char second_arg_allow_ref[];

extern PHPAPI int getParameters(HashTable *ht, int param_count,...);
extern PHPAPI int getParametersArray(HashTable *ht, int param_count, YYSTYPE **argument_array);
extern PHPAPI int getThis(YYSTYPE **this);
extern PHPAPI int ParameterPassedByReference(HashTable *ht, uint n);
extern PHPAPI int register_functions(function_entry *functions);
extern PHPAPI void unregister_functions(function_entry *functions, int count);
extern PHPAPI int register_module(php3_module_entry *module_entry);

extern PHPAPI void wrong_param_count(void);

#define WRONG_PARAM_COUNT { wrong_param_count(); return; }
#define WRONG_PARAM_COUNT_WITH_RETVAL(ret) { wrong_param_count(); return ret; }
#define ARG_COUNT(ht) (ht->nNextFreeElement)

#define BYREF_NONE 0
#define BYREF_FORCE 1
#define BYREF_ALLOW 2

#if !MSVC5
#define DLEXPORT
#endif

extern inline PHPAPI int array_init(YYSTYPE *arg);
extern inline PHPAPI int object_init(YYSTYPE *arg);
extern inline PHPAPI int add_assoc_long(YYSTYPE *arg, char *key, long n);
extern inline PHPAPI int add_assoc_double(YYSTYPE *arg, char *key, double d);
extern inline PHPAPI int add_assoc_string(YYSTYPE *arg, char *key, char *str, int duplicate);
extern inline PHPAPI int add_assoc_stringl(YYSTYPE *arg, char *key, char *str, uint length, int duplicate);
extern inline PHPAPI int add_assoc_function(YYSTYPE *arg, char *key,void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS));
extern inline PHPAPI int add_index_long(YYSTYPE *arg, uint idx, long n);
extern inline PHPAPI int add_index_double(YYSTYPE *arg, uint idx, double d);
extern inline PHPAPI int add_index_string(YYSTYPE *arg, uint idx, char *str, int duplicate);
extern inline PHPAPI int add_index_stringl(YYSTYPE *arg, uint idx, char *str, uint length, int duplicate);
extern inline PHPAPI int add_next_index_long(YYSTYPE *arg, long n);
extern inline PHPAPI int add_next_index_double(YYSTYPE *arg, double d);
extern inline PHPAPI int add_next_index_string(YYSTYPE *arg, char *str, int duplicate);
extern inline PHPAPI int add_next_index_stringl(YYSTYPE *arg, char *str, uint length, int duplicate);

extern inline PHPAPI int add_get_assoc_string(YYSTYPE *arg, char *key, char *str, void **dest, int duplicate);
extern inline PHPAPI int add_get_assoc_stringl(YYSTYPE *arg, char *key, char *str, uint length, void **dest, int duplicate);
extern inline PHPAPI int add_get_index_string(YYSTYPE *arg, uint idx, char *str, void **dest, int duplicate);
extern inline PHPAPI int add_get_index_stringl(YYSTYPE *arg, uint idx, char *str, uint length, void **dest, int duplicate);

#define add_property_long(arg,key,n)  add_assoc_long((arg),(key),(n))
#define add_property_double(arg,key,d)  add_assoc_double((arg),(key),(n))
#define add_property_string(arg,key,str,dup)  add_assoc_string((arg),(key),(str),(dup))
#define add_property_stringl(arg,key,str,length,dup)  add_assoc_stringl((arg),(key),(str),(length),(dup))
#define add_method(arg,key,method)	add_assoc_function((arg),(key),(method))

#endif							/* _INTERNAL_FUNCTIONS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
