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


/* $Id: control_structures.h,v 1.46 2000/04/10 19:29:36 andi Exp $ */


#ifndef _CONTROL_STRUCTURES_H
#define _CONTROL_STRUCTURES_H

#include "php3_list.h"

#define EXECUTE 0
#define BEFORE_EXECUTE 1
#define DONT_EXECUTE 2

#define SWITCHED 0x1
#define NOT_FIRST_ITERATION 0x2

#define SHOULD_EXECUTE ((GLOBAL(ExecuteFlag)==EXECUTE && !GLOBAL(function_state).returned && GLOBAL(function_state).loop_change_type==DO_NOTHING)?1:0)

typedef struct {
	pval expr;
	int offset;
	int Execute;
} switch_expr;


typedef struct {
	char *strval;
	int strlen,lval,type;
	HashTable *ht;
} variable_tracker;

extern TokenCacheManager token_cache_manager;
extern Stack css, input_source_stack, for_stack, function_state_stack, switch_stack,variable_unassign_stack;
extern int ExecuteFlag, Execute;
extern int php3_display_source;
extern int php3_preprocess;
extern FunctionState function_state;
extern unsigned int param_index;
extern char *class_name;
extern HashTable *class_symbol_table;

extern int init_resource_list(void);
extern int init_resource_plist(void);
extern void destroy_resource_list(void);
extern void destroy_resource_plist(void);
extern int clean_module_resource(list_entry *, int *);
extern inline void start_display_source(int start_in_php INLINE_TLS);

#endif
