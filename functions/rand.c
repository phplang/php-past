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
   +----------------------------------------------------------------------+
 */
/* $Id: rand.c,v 1.20 1997/12/31 15:56:47 rasmus Exp $ */

#include <stdlib.h>

#include "parser.h"
#include "internal_functions.h"
#include "phpmath.h"

#ifndef RAND_MAX
#define RAND_MAX	2^^15
#endif

void php3_srand(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);
#ifdef HAVE_SRAND48
	srand48((unsigned int) arg->value.lval);
#else
#ifdef HAVE_SRANDOM
	srandom((unsigned int) arg->value.lval);
#else
	srand((unsigned int) arg->value.lval);
#endif
#endif
}

void php3_rand(INTERNAL_FUNCTION_PARAMETERS)
{
	return_value->type = IS_LONG;
#ifdef HAVE_LRAND48
	return_value->value.lval = lrand48();
#else
#ifdef HAVE_RANDOM
	return_value->value.lval = random();
#else
	return_value->value.lval = rand();
#endif
#endif
}

void php3_getrandmax(INTERNAL_FUNCTION_PARAMETERS)
{
	return_value->type = IS_LONG;
#if HAVE_LRAND48
	return_value->value.lval = 2147483647;	/* 2^^31 */
#else
	return_value->value.lval = RAND_MAX;
#endif
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
