/***[syslog.c]****************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996,1997 Rasmus Lerdorf                                    *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License as published by      *
*  the Free Software Foundation; either version 2 of the License, or         *
*  (at your option) any later version.                                       *
*                                                                            *
*  This program is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU General Public License for more details.                              *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with this program; if not, write to the Free Software               *
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
*                                                                            *
\****************************************************************************/
/*
 * Contributed by Stig Bakken - Guardian Networks AS <ssb@guardian.no>
 *
 * $Source: /repository/phpfi/src/syslog.c,v $
 * $Id: syslog.c,v 1.3 1997/11/15 07:59:20 rasmus Exp $
 *
 */

#include "php.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

#include <string.h>
#include <errno.h>
#include "parse.h"

#include <stdio.h>

#if HAVE_SYSLOG_H
static void set_int_var(char *name, int value)
{
	char buf[10];
	sprintf(buf, "%d", value);
	Push(buf,LNUMBER);
	SetVar(name,0,0);
}
#endif

void php_init_syslog(void)
{
#if HAVE_SYSLOG_H
	set_int_var("LOG_EMERG", LOG_EMERG);
	set_int_var("LOG_ALERT", LOG_ALERT);
	set_int_var("LOG_CRIT", LOG_CRIT);
	set_int_var("LOG_ERR", LOG_ERR);
	set_int_var("LOG_WARNING", LOG_WARNING);
	set_int_var("LOG_NOTICE", LOG_NOTICE);
	set_int_var("LOG_INFO", LOG_INFO);
	set_int_var("LOG_DEBUG", LOG_DEBUG);
	set_int_var("LOG_KERN", LOG_KERN);
	set_int_var("LOG_USER", LOG_USER);
	set_int_var("LOG_MAIL", LOG_MAIL);
	set_int_var("LOG_DAEMON", LOG_DAEMON);
	set_int_var("LOG_AUTH", LOG_AUTH);
	set_int_var("LOG_SYSLOG", LOG_SYSLOG);
	set_int_var("LOG_LPR", LOG_LPR);
	set_int_var("LOG_NEWS", LOG_NEWS);
	set_int_var("LOG_UUCP", LOG_UUCP);
#ifdef LOG_CRON
/* apparently some systems do not have LOG_CRON */
	set_int_var("LOG_CRON", LOG_CRON);
#endif
#ifdef LOG_AUTHPRIV
/* AIX doesn't have LOG_AUTHPRIV */
	set_int_var("LOG_AUTHPRIV", LOG_AUTHPRIV);
#endif
	set_int_var("LOG_LOCAL0", LOG_LOCAL0);
	set_int_var("LOG_LOCAL1", LOG_LOCAL1);
	set_int_var("LOG_LOCAL2", LOG_LOCAL2);
	set_int_var("LOG_LOCAL3", LOG_LOCAL3);
	set_int_var("LOG_LOCAL4", LOG_LOCAL4);
	set_int_var("LOG_LOCAL5", LOG_LOCAL5);
	set_int_var("LOG_LOCAL6", LOG_LOCAL6);
	set_int_var("LOG_LOCAL7", LOG_LOCAL7);
	set_int_var("LOG_PID", LOG_PID);
	set_int_var("LOG_CONS", LOG_CONS);
	set_int_var("LOG_ODELAY", LOG_ODELAY);
	set_int_var("LOG_NDELAY", LOG_NDELAY);
	set_int_var("LOG_NOWAIT", LOG_NOWAIT);
#ifdef LOG_PERROR
/* AIX doesn't have LOG_PERROR */
	set_int_var("LOG_PERROR", LOG_PERROR);
#endif
#endif
}

/*
** OpenLog("nettopp", $LOG_PID, $LOG_LOCAL1);
** Syslog($LOG_EMERG, "help me!")
** CloseLog();
*/
void OpenLog(void)
{
#ifdef HAVE_SYSLOG_H
	char *ident;
	int option, facility;
	Stack *s;

	s = Pop();
	if (!s) {
		Error("stack error in OpenLog function");
		return;
	}
	facility = s->intval;

	s = Pop();
	if (!s) {
		Error("stack error in OpenLog function");
		return;
	}
	option = s->intval;

	s = Pop();
	if (!s) {
		Error("stack error in OpenLog function");
		return;
	}
	ident = s->strval;

	openlog(ident, option, facility);
#else
    Error("OpenLog function not available");	
#endif
    Push("0",STRING);
}

void CloseLog(void)
{
#ifdef HAVE_SYSLOG_H
	closelog();
#else
    Error("CloseLog function not available");	
#endif
    Push("0",STRING);
}

void Syslog(void)
{
#ifdef HAVE_SYSLOG_H
	int priority;
	char *message;
	Stack *s;

	s = Pop();
	if (!s) {
		Error("stack error in Syslog function");
		return;
	}
	message = s->strval;

	s = Pop();
	if (!s) {
		Error("stack error in Syslog function");
		return;
	}
	priority = s->intval;

	/*
	 * CAVEAT: if the message contains patterns such as "%s",
	 * this will cause problems.
	 */
	syslog(priority, message);
#else
    Error("Syslog function not available");	
#endif
    Push("0",STRING);
}
