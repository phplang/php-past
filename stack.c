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


/* $Id: stack.c,v 1.21 2000/04/10 19:29:36 andi Exp $ */


#include "php.h"

int php3i_stack_init(Stack *stack)
{
	stack->top = 0;
	stack->elements = (void **) emalloc(sizeof(void **) * STACK_BLOCK_SIZE);
	if (!stack->elements) {
		return FAILURE;
	} else {
		stack->max = STACK_BLOCK_SIZE;
		return SUCCESS;
	}
}

int php3i_stack_push(Stack *stack, void *element, int size)
{
	if (stack->top >= stack->max) {		/* we need to allocate more memory */
		stack->elements = (void **) erealloc(stack->elements,
				   (sizeof(void **) * (stack->max += STACK_BLOCK_SIZE)));
		if (!stack->elements) {
			return FAILURE;
		}
	}
	stack->elements[stack->top] = (void *) emalloc(size);
	memcpy(stack->elements[stack->top], element, size);
	return stack->top++;
}


int php3i_stack_top(Stack *stack, void **element)
{
	if (stack->top > 0) {
		*element = stack->elements[stack->top - 1];
		return SUCCESS;
	} else {
		*element = NULL;
		return FAILURE;
	}
}


int php3i_stack_del_top(Stack *stack)
{
	if (stack->top > 0) {
		efree(stack->elements[--stack->top]);
	}
	return SUCCESS;
}

int php3i_stack_int_top(Stack *stack)
{
	int *e;

	if (php3i_stack_top(stack, (void **) &e) == FAILURE) {
		return FAILURE;			/* this must be a negative number, since negative numbers can't be address numbers */
	} else {
		return *e;
	}
}


int php3i_stack_is_empty(Stack *stack)
{
	if (stack->top == 0) {
		return 1;
	} else {
		return 0;
	}
}


int php3i_stack_destroy(Stack *stack)
{
	register int i;

	for (i = 0; i < stack->top; i++) {
		efree(stack->elements[i]);
	}

	if (stack->elements) {
		efree(stack->elements);
	}
	return SUCCESS;
}
