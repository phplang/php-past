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
   | Authors: Stig Bakken                                                 |
   +----------------------------------------------------------------------+
 */
/* $Id: crypt.c,v 1.27 1997/12/31 15:56:20 rasmus Exp $ */

#include "parser.h"
#include "internal_functions.h"

#if HAVE_CRYPT

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_CRYPT_H
#include <crypt.h>
#endif
#if TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#if MSVC5
#include <process.h>
#endif

#include "functions/php3_string.h"

function_entry crypt_functions[] = {
	{"crypt",	php3_crypt,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry crypt_module_entry = {
	"Crypt", crypt_functions, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, NULL
};

void php3_crypt(INTERNAL_FUNCTION_PARAMETERS)
{
	char salt[4];
	int arg_count = ARG_COUNT(ht);
	YYSTYPE *arg1, *arg2;
	static char seedchars[] =
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

	if (arg_count < 1 || arg_count > 2 ||
		getParameters(ht, arg_count, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);

	salt[0] = '\0';
	if (arg_count == 2) {
		convert_to_string(arg2);
		strncpy(salt, arg2->value.strval, 2);
	}
	if (!salt[0]) {
		srand(time(0) * getpid());
		salt[0] = seedchars[rand() % 64];
		salt[1] = seedchars[rand() % 64];
	}
	salt[2] = '\0';

	return_value->value.strval = (char *) crypt(arg1->value.strval, salt);
	return_value->strlen = strlen(return_value->value.strval);	/* can be optimized away to 13? */
	return_value->type = IS_STRING;
	yystype_copy_constructor(return_value);
}

#endif



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
