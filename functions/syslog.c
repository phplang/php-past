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
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */

/* $Id: syslog.c,v 1.26 1998/02/03 18:54:28 shane Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "internal_functions.h"

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if MSVC5
#include "win32/syslog.h"
#else
#include <syslog.h>
#endif

#include <string.h>
#include <errno.h>

#include <stdio.h>
#include "functions/php3_syslog.h"

int php3_init_syslog(INITFUNCARG)
{
	TLS_VARS;
	
	/* error levels */
	SET_VAR_LONG("LOG_EMERG", LOG_EMERG); /* system unusable */
	SET_VAR_LONG("LOG_ALERT", LOG_ALERT); /* immediate action required */
	SET_VAR_LONG("LOG_CRIT", LOG_CRIT); /* critical conditions */
	SET_VAR_LONG("LOG_ERR", LOG_ERR); 
	SET_VAR_LONG("LOG_WARNING", LOG_WARNING);
	SET_VAR_LONG("LOG_NOTICE", LOG_NOTICE);
	SET_VAR_LONG("LOG_INFO", LOG_INFO);
	SET_VAR_LONG("LOG_DEBUG", LOG_DEBUG);
	/* facility: type of program logging the message */
	SET_VAR_LONG("LOG_KERN", LOG_KERN);
	SET_VAR_LONG("LOG_USER", LOG_USER); /* generic user level */
	SET_VAR_LONG("LOG_MAIL", LOG_MAIL); /* log to email */
	SET_VAR_LONG("LOG_DAEMON", LOG_DAEMON); /* other system daemons */
	SET_VAR_LONG("LOG_AUTH", LOG_AUTH);
	SET_VAR_LONG("LOG_SYSLOG", LOG_SYSLOG);
	SET_VAR_LONG("LOG_LPR", LOG_LPR);
#ifdef LOG_NEWS
	/* No LOG_NEWS on HP-UX */
	SET_VAR_LONG("LOG_NEWS", LOG_NEWS); /* usenet new */
#endif
#ifdef LOG_UUCP
	/* No LOG_UUCP on HP-UX */
	SET_VAR_LONG("LOG_UUCP", LOG_UUCP);
#endif
#ifdef LOG_CRON
/* apparently some systems don't have this one */
	SET_VAR_LONG("LOG_CRON", LOG_CRON);
#endif
#ifdef LOG_AUTHPRIV
/* AIX doesn't have LOG_AUTHPRIV */
	SET_VAR_LONG("LOG_AUTHPRIV", LOG_AUTHPRIV);
#endif
#ifndef MSVC5
	SET_VAR_LONG("LOG_LOCAL0", LOG_LOCAL0);
	SET_VAR_LONG("LOG_LOCAL1", LOG_LOCAL1);
	SET_VAR_LONG("LOG_LOCAL2", LOG_LOCAL2);
	SET_VAR_LONG("LOG_LOCAL3", LOG_LOCAL3);
	SET_VAR_LONG("LOG_LOCAL4", LOG_LOCAL4);
	SET_VAR_LONG("LOG_LOCAL5", LOG_LOCAL5);
	SET_VAR_LONG("LOG_LOCAL6", LOG_LOCAL6);
	SET_VAR_LONG("LOG_LOCAL7", LOG_LOCAL7);
#endif
	/* options */
	SET_VAR_LONG("LOG_PID", LOG_PID);
	SET_VAR_LONG("LOG_CONS", LOG_CONS);
	SET_VAR_LONG("LOG_ODELAY", LOG_ODELAY);
	SET_VAR_LONG("LOG_NDELAY", LOG_NDELAY);
	SET_VAR_LONG("LOG_NOWAIT", LOG_NOWAIT);
#ifdef LOG_PERROR
/* AIX doesn't have LOG_PERROR */
	SET_VAR_LONG("LOG_PERROR", LOG_PERROR); /*log to stderr*/
#endif
	return SUCCESS;
}

/*
   ** OpenLog("nettopp", $LOG_PID, $LOG_LOCAL1);
   ** Syslog($LOG_EMERG, "help me!")
   ** CloseLog();
 */

void php3_openlog(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *ident, *option, *facility;
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &ident, &option, &facility) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(ident);
	convert_to_long(option);
	convert_to_long(facility);

	openlog(ident->value.strval, option->value.lval, facility->value.lval);
}

void php3_closelog(INTERNAL_FUNCTION_PARAMETERS)
{
	closelog();
}

void php3_syslog(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *priority, *message;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &priority, &message) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(priority);
	convert_to_string(message);

	/*
	 * CAVEAT: if the message contains patterns such as "%s",
	 * this will cause problems.
	 */

	syslog(priority->value.lval, message->value.strval);
	RETURN_TRUE;
}

function_entry syslog_functions[] = {
	{"openlog",		php3_openlog,		NULL},
	{"syslog",		php3_syslog,		NULL},
	{"closelog",	php3_closelog,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry syslog_module_entry = {
	"Syslog", syslog_functions, NULL, NULL, php3_init_syslog, NULL, NULL, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &syslog_module_entry; }
#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
