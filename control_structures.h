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


/* $Id: control_structures.h,v 1.30 1998/01/17 21:23:25 andi Exp $ */


#ifndef _CONTROL_STRUCTURES_H
#define _CONTROL_STRUCTURES_H

#define EXECUTE 0
#define BEFORE_EXECUTE 1
#define DONT_EXECUTE 2

#define SWITCHED 0x1
#define NOT_FIRST_ITERATION 0x2

#define SHOULD_EXECUTE ((GLOBAL(ExecuteFlag)==EXECUTE && !GLOBAL(function_state).returned && GLOBAL(function_state).loop_change_type==DO_NOTHING)?1:0)

typedef struct {
	YYSTYPE expr;
	int offset;
} switch_expr;


typedef struct {
	char *strval;
	int strlen,lval,type;
	HashTable *ht;
} variable_tracker;

#ifndef THREAD_SAFE
extern TokenCacheManager token_cache_manager;
extern Stack css, input_source_stack, for_stack, function_state_stack, switch_stack,variable_unassign_stack;
extern int ExecuteFlag, Execute;
extern int php3_display_source;
extern int php3_preprocess;
extern FunctionState function_state;
extern unsigned int param_index;
extern char *class_name;
extern HashTable *class_symbol_table;
#endif

extern int init_resource_list(void);
extern int init_resource_plist(void);
extern void destroy_resource_list(void);
extern void destroy_resource_plist(void);

#endif
