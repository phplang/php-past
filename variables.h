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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id: variables.h,v 1.57 2000/04/10 19:29:37 andi Exp $ */


#ifndef _VARIABLES_H
#define _VARIABLES_H

extern void clean_unassigned_variable_top(int delete_var INLINE_TLS);
extern int assign_to_variable(pval *result, pval *var, pval *expr, int (*func) (pval *, pval *, pval * INLINE_TLS) INLINE_TLS);
extern void assign_to_list(pval *result, pval *list, pval *expr INLINE_TLS);
/*not used ?? extern void assign_to_class_variable(pval *result, pval *classname, pval *varname, pval *expr, int (*func) (pval *, pval *, pval *) INLINE_TLS);*/
extern int get_regular_variable_contents(pval *result, pval *varname, int free_varname INLINE_TLS);
extern void get_array_variable(pval *result, pval *varname, pval *idx INLINE_TLS);
extern void get_next_array_variable(pval *result, pval *varname);
extern int incdec_variable(pval *result, pval *varname, int (*func) (pval *), int post INLINE_TLS);
/*not used ?? extern void incdec_class_variable(pval *result, pval *classname, pval *varname, int (*func) (pval *), int post);*/
extern void php3i_print_variable(pval *var INLINE_TLS);
extern int is_not_internal_function(pval *pvalue);
extern void array_assign_next(pval *result, pval *varname, pval *expr);
extern void add_array_pair_list(pval *result, pval *idx, pval *value, int initialize INLINE_TLS);
extern void declare_class_variable(pval *varname, pval *expr INLINE_TLS);
extern void get_object_property(pval *result, pval *classname, pval *varname INLINE_TLS);
extern void get_object_symtable(pval *result, pval *parent, pval *child INLINE_TLS);
extern void assign_new_object(pval *result, pval *classname, unsigned char implicit_ctor INLINE_TLS);

extern PHPAPI void php3tls_pval_destructor(pval *pvalue);
extern void pval_destructor(pval *pvalue INLINE_TLS);
extern PHPAPI int pval_copy_constructor(pval *pvalue);
#define PVAL_DESTRUCTOR (void (*)(void *)) pval_destructor
#define PVAL_COPY_CTOR (void (*)(void *)) pval_copy_constructor

/* Internal functions which have a YACC rule */
extern void php3_unset(pval *result, pval *var_ptr);
extern void php3_isset(pval *result, pval *var_ptr);
extern void php3_empty(pval *result, pval *var_ptr);

extern PHPAPI void var_reset(pval *var);
extern PHPAPI void var_uninit(pval *var);
extern PHPAPI HashTable *php3i_get_symbol_table(void);
extern PHPAPI HashTable *php3i_get_function_table(void);

extern void read_pointer_value(pval *result,pval *array_result INLINE_TLS);
extern inline void get_regular_variable_pointer(pval *result, pval *varname INLINE_TLS);

extern PHPAPI pval *php3i_long_pval(long);
extern PHPAPI pval *php3i_double_pval(double);
extern PHPAPI pval *php3i_string_pval(const char *);
extern PHPAPI char *php3i_pval_strdup(pval *);

#endif
