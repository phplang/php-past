/***[mail.c]******************************************************[TAB=4]****\
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
#include "php.h"
#include <stdlib.h>
#include "parse.h"
#include <ctype.h>

#if APACHE
#ifndef DEBUG
#undef palloc
#endif
#endif

void Mail(int flag) {
#if defined(PHPSENDMAIL) && !defined(WINNT)
	Stack *s;
	char  temp[16];
	char  *message;
	char  *subject;
	char  *to;
	char  *headers = NULL;
	FILE  *sendmail;

	/* get args */

	if (flag) {
		s = Pop(); /* (optional) Extra headers */
		if (!s) {
			Error("Stack error in mail");
			return;
		}
		headers = (char *)estrdup(1,s->strval);
		ParseEscapes(headers);
	}

	s = Pop(); /* Message text */
	if (!s) {
		Error("Stack error in mail");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval)  {
		message = (char*) estrdup(1,s->strval);
		ParseEscapes(message);
	}
	else {
		Error("No message string in mail command");
		Push("0", LNUMBER);
		return;
	}

	s = Pop(); /* Message Subject */
	if (!s) {
		Error("Stack error in mail");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) 
		subject = (char*) estrdup(1,s->strval);
	else {
		Error("No subject field in mail command");
		Push("0", LNUMBER);
		return;
	}

	s = Pop(); /* Message Destination */
	if (!s) {
		Error("Stack error in mail");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) {
		to = (char*) estrdup(1,s->strval);
	} else {
		Error("No to field in mail command");
		Push("0", LNUMBER);
		return;
	}

    sendmail = popen(SENDMAIL, "w");
	if (sendmail) {
		fprintf(sendmail, "To: %s\n", to);
		fprintf(sendmail, "Subject: %s\n", subject);
		if (headers != NULL) {
			fprintf(sendmail, "%s\n", headers);
		}
		fprintf(sendmail, "\n%s\n.\n", message);
        sprintf(temp, "%d", pclose(sendmail));
	    Push(temp,LNUMBER);
	}
	else {
	    Push("-1",LNUMBER);
	}
#else
	if (flag) {
	    Pop();
	}
	Pop();
	Pop();
	Pop();
	Error("Mail function not supported on this system");
	Push("0", LNUMBER);
#endif
}

/*
 * Local variables:
 * tab-width: 4
 * End:
 */
