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
   | Author: Stig Sæther Bakken <ssb@fast.no>                             |
   +----------------------------------------------------------------------+
 */

/* $Id: uniqid.c,v 1.41 2000/02/22 15:13:57 eschmid Exp $ */
#include "php.h"
#include "internal_functions.h"

#include <stdlib.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <string.h>
#include <errno.h>

#include <stdio.h>
#if MSVC5
#include "win32/time.h"
#else
#include <sys/time.h>
#endif

#include "php_lcg.h"
#include "uniqid.h"

#define MORE_ENTROPY (argc == 2 && flags->value.lval)

/* {{{ proto string uniqid(string prefix [, bool more_entropy])
   Generate a unique id */
PHP_FUNCTION(uniqid)
{
#ifdef HAVE_GETTIMEOFDAY
	pval *prefix, *flags;
	char uniqid[138];
	int sec, usec, argc;
	struct timeval tv;
	TLS_VARS;
	
	argc = ARG_COUNT(ht);
	if (argc < 1 || argc > 2 || getParameters(ht,argc,&prefix,&flags)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(prefix);
	if (argc == 2) {
		convert_to_long(flags);
	}

	/* Do some bounds checking since we are using a char array. */
	if (strlen(prefix->value.str.val) > 114) {
		php3_error(E_WARNING, "The prefix to uniqid should not be more than 114 characters.");
		return;
	}
#if HAVE_USLEEP && !(WIN32|WINNT)
	if (!MORE_ENTROPY) {
		usleep(1);
	}
#endif
	gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	sec = (int) tv.tv_sec;
	usec = (int) (tv.tv_usec % 1000000);

	/* The max value usec can have is 0xF423F, so we use only five hex
	 * digits for usecs:
	 */
	if (MORE_ENTROPY) {
		sprintf(uniqid, "%s%08x%05x%.8f", prefix->value.str.val, sec, usec, php_combined_lcg() * 10);
	} else {
		sprintf(uniqid, "%s%08x%05x", prefix->value.str.val, sec, usec);
	}

	RETURN_STRING(uniqid,1);
#endif
}
/* }}} */

function_entry uniqid_functions[] = {
	{"uniqid",		php3_uniqid,		NULL},
	{NULL, NULL, NULL}
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
