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


/* $Id: stack.h,v 1.10 1998/01/17 21:23:32 andi Exp $ */


#ifndef _STACK_H
#define _STACK_H

typedef struct {
	int top, max;
	void **elements;
} Stack;


#define STACK_BLOCK_SIZE 64

extern int stack_init(Stack *stack);
extern int stack_push(Stack *stack, void *element, int size);
extern int stack_top(Stack *stack, void **element);
extern int stack_del_top(Stack *stack);
extern int stack_int_top(Stack *stack);
extern int stack_is_empty(Stack *stack);
extern int stack_destroy(Stack *stack);

#endif
