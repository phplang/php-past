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


/* $Id: stack.c,v 1.12 1998/01/17 21:23:31 andi Exp $ */


#include "parser.h"

int stack_init(Stack *stack)
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

int stack_push(Stack *stack, void *element, int size)
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


int stack_top(Stack *stack, void **element)
{
	if (stack->top > 0) {
		*element = stack->elements[stack->top - 1];
		return SUCCESS;
	} else {
		*element = NULL;
		return FAILURE;
	}
}


int stack_del_top(Stack *stack)
{
	if (stack->top > 0) {
		efree(stack->elements[--stack->top]);
	}
	return SUCCESS;
}

int stack_int_top(Stack *stack)
{
	int *e;

	if (stack_top(stack, (void **) &e) == FAILURE) {
		return FAILURE;			/* this must be a negative number, since negative numbers can't be address numbers */
	} else {
		return *e;
	}
}


int stack_is_empty(Stack *stack)
{
	if (stack->top == 0) {
		return 1;
	} else {
		return 0;
	}
}


int stack_destroy(Stack *stack)
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
