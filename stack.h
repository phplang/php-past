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


/* $Id: stack.h,v 1.18 2000/04/10 19:29:36 andi Exp $ */


#ifndef _STACK_H
#define _STACK_H

typedef struct {
	int top, max;
	void **elements;
} Stack;


#define STACK_BLOCK_SIZE 64

extern int php3i_stack_init(Stack *stack);
extern int php3i_stack_push(Stack *stack, void *element, int size);
extern int php3i_stack_top(Stack *stack, void **element);
extern int php3i_stack_del_top(Stack *stack);
extern int php3i_stack_int_top(Stack *stack);
extern int php3i_stack_is_empty(Stack *stack);
extern int php3i_stack_destroy(Stack *stack);

#endif
