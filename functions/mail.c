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
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id: mail.c,v 1.43 1998/02/03 19:40:00 shane Exp $ */

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#ifndef MSVC5
#include "build-defs.h"
#endif
#include "internal_functions.h"
#include "mail.h"

#if APACHE
#  ifndef DEBUG
#  undef palloc
#  endif
#endif

#if HAVE_SENDMAIL
#if MSVC5
#include "win32/sendmail.h"
#endif

function_entry mail_functions[] = {
	{"mail",		php3_mail,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry mail_module_entry = {
	"Sendmail", mail_functions, NULL, NULL, NULL, NULL, php3_info_mail, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &odbc_module_entry; }
#endif

void php3_mail(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *argv[4];
	char *to=NULL, *message=NULL, *headers=NULL, *subject=NULL;
	int argc;
	TLS_VARS;
	
	argc = ARG_COUNT(ht);
	if (argc < 3 || argc > 4 || getParametersArray(ht, argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	/* To: */
	convert_to_string(argv[0]);
	if (argv[0]->value.strval) {
		to = (char *) estrndup(argv[0]->value.strval,argv[0]->strlen);
	} else {
		php3_error(E_WARNING, "No to field in mail command");
		RETURN_FALSE;
	}

	/* Subject: */
	convert_to_string(argv[1]);
	if (argv[1]->value.strval) {
		subject = (char *) estrndup(argv[1]->value.strval,argv[1]->strlen);
	} else {
		php3_error(E_WARNING, "No subject field in mail command");
		if(to) efree(to);
		RETURN_FALSE;
	}

	/* message body */
	convert_to_string(argv[2]);
	if (argv[2]->value.strval) {
		message = (char *) estrndup(argv[2]->value.strval,argv[2]->strlen);
	} else {
		/* this is not really an error, so it is allowed. */
		php3_error(E_WARNING, "No message string in mail command");
		message = NULL;
	}

	if (argc == 4) {			/* other headers */
		convert_to_string(argv[3]);
		headers = (char *) estrndup(argv[3]->value.strval,argv[3]->strlen);
	}
	if(_php3_mail(to, subject, message, headers)){
		RETURN_TRUE;
	}else{
		RETURN_FALSE;
	}
}

int _php3_mail(char *to, char *subject, char *message, char *headers){
#if MSVC5
	int tsm_err;
	char *tsm_err_msg;
#else
	FILE *sendmail;
	int ret;
#endif
	TLS_VARS;

#if MSVC5
	if(TSendMail(php3_ini.smtp,&tsm_err,headers,subject,to,message) != SUCCESS){
		tsm_err_msg = (char *) estrdup(GetSMErrorText(tsm_err));
		php3_error(E_WARNING, tsm_err_msg);
		efree(tsm_err_msg);
		if(message) efree(message);
		if(subject) efree(subject);
		if(to) efree(to);
		if(headers) efree(headers);
		return 0;
	}else{
		if(message) efree(message);
		if(subject) efree(subject);
		if(to) efree(to);
		if(headers) efree(headers);
	}
#else
	sendmail = popen(PHP_PROG_SENDMAIL " -t", "w");

	if (sendmail) {
		fprintf(sendmail, "To: %s\n", to);
		fprintf(sendmail, "Subject: %s\n", subject);
		if (headers != NULL) {
			fprintf(sendmail, "%s\n", headers);
		}
		fprintf(sendmail, "\n%s\n.\n", message);
		ret = pclose(sendmail);
		if(message) efree(message);
		if(subject) efree(subject);
		if(to) efree(to);
		if(headers) efree(headers);
		if (ret == -1) {
			return 0;
		} else {
			return 1;
		}
	} else {
		php3_error(E_WARNING, "Could not execute mail delivery program");
		if(message) efree(message);
		if(subject) efree(subject);
		if(to) efree(to);
		if(headers) efree(headers);
		return 0;
	}
#endif
	return 1;
}

void php3_info_mail(void)
{
	TLS_VARS;
#if MSVC5
	PUTS("Internal Sendmail support for Windows 4");
#else
	php3_printf("Path to sendmail: <tt>%s</tt>", PHP_PROG_SENDMAIL);
#endif
}

#else

void php3_mail(INTERNAL_FUNCTION_PARAMETERS) {}
void php3_info_mail() {}

#endif


/*
 * Local variables:
 * tab-width: 4
 * End:
 */
