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


/* $Id: variables.h,v 1.39 1998/02/20 04:03:58 zeev Exp $ */


#ifndef _VARIABLES_H
#define _VARIABLES_H

extern void clean_unassigned_variable_top(int delete_var INLINE_TLS);
extern int assign_to_variable(YYSTYPE *result, YYSTYPE *var, YYSTYPE *expr, int (*func) (YYSTYPE *, YYSTYPE *, YYSTYPE * INLINE_TLS) INLINE_TLS);
extern void assign_to_list(YYSTYPE *result, YYSTYPE *list, YYSTYPE *expr INLINE_TLS);
/*not used ?? extern void assign_to_class_variable(YYSTYPE *result, YYSTYPE *classname, YYSTYPE *varname, YYSTYPE *expr, int (*func) (YYSTYPE *, YYSTYPE *, YYSTYPE *) INLINE_TLS);*/
extern int get_regular_variable_contents(YYSTYPE *result, YYSTYPE *varname, int free_varname INLINE_TLS);
extern void get_array_variable(YYSTYPE *result, YYSTYPE *varname, YYSTYPE *idx INLINE_TLS);
extern void get_next_array_variable(YYSTYPE *result, YYSTYPE *varname);
extern int incdec_variable(YYSTYPE *result, YYSTYPE *varname, int (*func) (YYSTYPE *), int post INLINE_TLS);
/*not used ?? extern void incdec_class_variable(YYSTYPE *result, YYSTYPE *classname, YYSTYPE *varname, int (*func) (YYSTYPE *), int post);*/
extern void print_variable(YYSTYPE *var INLINE_TLS);
extern int is_not_internal_function(YYSTYPE *yystype);
extern void array_assign_next(YYSTYPE *result, YYSTYPE *varname, YYSTYPE *expr);
extern void add_array_pair_list(YYSTYPE *result, YYSTYPE *idx, YYSTYPE *value, int initialize INLINE_TLS);
extern void declare_class_variable(YYSTYPE *varname, YYSTYPE *expr INLINE_TLS);
extern void get_object_property(YYSTYPE *result, YYSTYPE *classname, YYSTYPE *varname INLINE_TLS);
extern void get_object_symtable(YYSTYPE *result, YYSTYPE *parent, YYSTYPE *child INLINE_TLS);
extern void assign_new_object(YYSTYPE *result, YYSTYPE *classname INLINE_TLS);

extern void yystype_destructor(YYSTYPE *yystype INLINE_TLS);
extern int yystype_copy_constructor(YYSTYPE *yystype);
#define YYSTYPE_DESTRUCTOR (void (*)(void *)) yystype_destructor
#define YYSTYPE_COPY_CTOR (void (*)(void *)) yystype_copy_constructor

/* Internal functions which have a YACC rule */
extern void php3_unset(YYSTYPE *result, YYSTYPE *var_ptr);
extern void php3_isset(YYSTYPE *result, YYSTYPE *var_ptr);
extern void php3_empty(YYSTYPE *result, YYSTYPE *var_ptr);

extern PHPAPI void var_reset(YYSTYPE *var);
extern PHPAPI void var_uninit(YYSTYPE *var);

extern void read_pointer_value(YYSTYPE *result,YYSTYPE *array_result INLINE_TLS);
extern inline void get_regular_variable_pointer(YYSTYPE *result, YYSTYPE *varname INLINE_TLS);

#endif
