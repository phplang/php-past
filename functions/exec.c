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
   | Author: Rasmus Lerdorf                                               |
   +----------------------------------------------------------------------+
 */
/* $Id: exec.c,v 1.57 1998/01/30 17:46:04 zeev Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include <stdio.h>
#include "parser.h"
#include <ctype.h>
#include "internal_functions.h"
#include "php3_string.h"
#include "safe_mode.h"
#include "head.h"
#include "exec.h"

#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

/*
 * If type==0, only last line of output is returned (exec)
 * If type==1, all lines will be printed and last lined returned (system)
 * If type==2, all lines will be saved to given array (exec with &$array)
 * If type==3, output will be printed binary, no lines will be saved or returned (passthru)
 *
 */
static int _Exec(int type, char *cmd, YYSTYPE *array, YYSTYPE *return_value)
{
	FILE *fp;
	char buf[EXEC_INPUT_BUF], *tmp=NULL;
	int t, l, ret, output=1;
	char *b, *c, *d=NULL;
	TLS_VARS;

	if (php3_ini.safe_mode) {
		l = strlen(cmd) + strlen(php3_ini.safe_mode_exec_dir) + 2;
		c = strchr(cmd, ' ');
		if (c) *c = '\0';
		if (strstr(cmd, "..")) {
			php3_error(E_WARNING, "No '..' components allowed in path");
			return -1;
		}
		b = strrchr(cmd, '/');
		d = emalloc(l);
		strncpy(d, php3_ini.safe_mode_exec_dir, l - 1);
		if (b) {
			strncat(d, b, l - 1);
			d[l - 1] = '\0';		/* watch out for overflows */
		} else {
			strcat(d, "/");
			strncat(d, cmd, l - 1);
			d[l - 1] = '\0';		/* watch out for overflows */
		}
		if (c) {
			*c = ' ';
			strncat(d, c, l - 1);
			d[l - 1] = '\0';
		}
		tmp = _php3_escapeshellcmd(d);
		efree(d);
		d = tmp;
#if WIN32|WINNT
		fp = popen(d, "rt");
#else
		fp = popen(d, "r");
#endif
		if (!fp) {
			php3_error(E_WARNING, "Unable to fork [%s]", d);
			efree(d);
			return -1;
		}
	} else { /* not safe_mode */
#if WIN32|WINNT
		fp = popen(cmd, "rt");
#else
		fp = popen(cmd, "r");
#endif
		if (!fp) {
			php3_error(E_WARNING, "Unable to fork [%s]", cmd);
			return -1;
		}
	}
	buf[0] = '\0';
	if (type == 1 || type == 3) {
		output=php3_header(0, NULL);
	}
	if (type==2) {
		array_init(array);
	}
	if (type != 3) {
		while (fgets(buf, EXEC_INPUT_BUF - 1, fp)) {
			if (type == 1) {
				if(output) PUTS(buf);
#if APACHE
#  if MODULE_MAGIC_NUMBER > 19970110
				if(output) rflush(GLOBAL(php3_rqst));
#  else
				if(output) bflush(GLOBAL(php3_rqst)->connection->client);
#  endif
#endif
#if CGI_BINARY
				fflush(stdout);
#endif
#if USE_SAPI
				GLOBAL(sapi_rqst)->flush(GLOBAL(sapi_rqst)->scid);
#endif
			}
			else if (type == 2) {
				YYSTYPE tmp;
			
				/* strip trailing whitespaces */	
				l = strlen(buf);
				t = l;
				while (l && isspace((int)buf[--l]));
				if (l < t) buf[l + 1] = '\0';
				tmp.strlen = strlen(buf);
				tmp.value.strval = estrndup(buf,tmp.strlen);
				tmp.type = IS_STRING;
				hash_next_index_insert(array->value.ht,(void *) &tmp, sizeof(YYSTYPE), NULL);
			}
		}

		/* strip trailing spaces */
		l = strlen(buf);
		t = l;
		while (l && isspace((int)buf[--l]));
		if (l < t) buf[l + 1] = '\0';

	} else {
		int b, i;

		while ((b = fread(buf, 1, sizeof(buf), fp)) > 0) {
			for (i = 0; i < b; i++)
				if(output) PUTC(buf[i]);
		}
	}

	/* Return last line from the shell command */
	if(php3_ini.magic_quotes_runtime && type!=3) {
		tmp = _php3_addslashes(buf, 0);
		RETVAL_STRING(tmp);
		efree(tmp);
	} else {
		RETVAL_STRING(buf);
	}
	
	ret = pclose(fp);
#if HAVE_SYS_WAIT_H
	if (WIFEXITED(ret)) {
		ret = WEXITSTATUS(ret);
	}
#endif

	if(d) efree(d);
	return ret;
}

void php3_exec(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg1, *arg2, *arg3;
	int arg_count = ARG_COUNT(ht);
	int ret;

	if (arg_count > 3 || getParameters(ht, arg_count, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (arg_count) {
		case 1:
			ret = _Exec(0, arg1->value.strval, NULL, return_value);
			break;
		case 2:
			if (!ParameterPassedByReference(ht,2)) {
				php3_error(E_WARNING,"Array argument to exec() not passed by reference");
			}
			ret = _Exec(2, arg1->value.strval, arg2, return_value);
			break;
		case 3:
			if (!ParameterPassedByReference(ht,2)) {
				php3_error(E_WARNING,"Array argument to exec() not passed by reference");
			}
			if (!ParameterPassedByReference(ht,3)) {
				php3_error(E_WARNING,"return_status argument to exec() not passed by reference");
			}
			ret = _Exec(2, arg1->value.strval, arg2, return_value);
			arg3->type = IS_LONG;
			arg3->value.lval=ret;
			break;
	}
}

void php3_system(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg1, *arg2;
	int arg_count = ARG_COUNT(ht);
	int ret;

	if (arg_count > 2 || getParameters(ht, arg_count, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (arg_count) {
		case 1:
			ret = _Exec(1, arg1->value.strval, NULL, return_value);
			break;
		case 2:
			if (!ParameterPassedByReference(ht,2)) {
				php3_error(E_WARNING,"return_status argument to system() not passed by reference");
			}
			ret = _Exec(1, arg1->value.strval, NULL, return_value);
			arg2->type = IS_LONG;
			arg2->value.lval=ret;
			break;
	}
}

void php3_passthru(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg1, *arg2;
	int arg_count = ARG_COUNT(ht);
	int ret;

	if (arg_count > 2 || getParameters(ht, arg_count, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (arg_count) {
		case 1:
			ret = _Exec(3, arg1->value.strval, NULL, return_value);
			break;
		case 2:
			if (!ParameterPassedByReference(ht,2)) {
				php3_error(E_WARNING,"return_status argument to system() not passed by reference");
			}
			ret = _Exec(3, arg1->value.strval, NULL, return_value);
			arg2->type = IS_LONG;
			arg2->value.lval=ret;
			break;
	}
}

static int php3_ind(char *s, char c)
{
	register int x;

	for (x = 0; s[x]; x++)
		if (s[x] == c)
			return x;

	return -1;
}

/* Escape all chars that could possibly be used to
   break out of a shell command

   This function emalloc's a string and returns the pointer.
   Remember to efree it when done with it.

   *NOT* safe for binary strings
*/
char * _php3_escapeshellcmd(char *str) {
	register int x, y, l;
	char *cmd;

	l = strlen(str);
	cmd = emalloc(2 * l + 1);
	strcpy(cmd, str);
	for (x = 0; cmd[x]; x++) {
		if (php3_ind("&;`'\"|*?~<>^()[]{}$\\\x0A\xFF", cmd[x]) != -1) {
			for (y = l + 1; y > x; y--)
				cmd[y] = cmd[y - 1];
			l++;				/* length has been increased */
			cmd[x] = '\\';
			x++;				/* skip the character */
		}
	}
	return cmd;
}

void php3_escapeshellcmd(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg1;
	char *cmd;
	TLS_VARS;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	cmd = _php3_escapeshellcmd(arg1->value.strval);

	RETVAL_STRING(cmd);
	efree(cmd);
}
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
