/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Zeev Suraski <bourbon@netvision.net.il                      |
   +----------------------------------------------------------------------+
 */
/* $Id: rand.c,v 1.26 1998/08/26 19:44:42 zeev Exp $ */

#include <stdlib.h>

#include "php.h"
#include "internal_functions.h"
#include "phpmath.h"

#ifndef RAND_MAX
#define RAND_MAX	2^^15
#endif

void php3_srand(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;

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
	pval *p_min=NULL, *p_max=NULL;
	
	switch (ARG_COUNT(ht)) {
		case 0:
			break;
		case 2:
			if (getParameters(ht, 2, &p_min, &p_max)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(p_min);
			convert_to_long(p_max);
			if (p_max->value.lval-p_min->value.lval <= 0) {
				php3_error(E_WARNING,"rand():  Invalid range:  %ld..%ld", p_min->value.lval, p_max->value.lval);
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
			
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

	if (p_min && p_max) { /* implement range */
		return_value->value.lval = (return_value->value.lval%(p_max->value.lval-p_min->value.lval+1))+p_min->value.lval;
	}
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
