/***[error.c]*****************************************************[TAB=4]****\
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
/* $Id: error.c,v 1.25 1997/08/13 23:01:40 rasmus Exp $ */
#include "php.h"
#include "parse.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdarg.h>		/* ANSI C, variable length args */
#if APACHE
#include "http_protocol.h"
#endif

#if DEBUG
static FILE *fpdebug = NULL;
#endif
static int PrintErrors=1;

extern int yylex_linenumber;

void php_init_error(void) {
#if DEBUG
	fpdebug = NULL;
#endif
	PrintErrors=1;
}

void CloseDebug(void) {
#if DEBUG
	if(fpdebug) fclose(fpdebug);
	fpdebug=NULL;
#endif
}

void Debug(char *fmt,...) {
#if DEBUG
	va_list ap;

	if(!fpdebug) {
		fpdebug = fopen(DEBUG_FILE,"w");
		chmod(DEBUG_FILE,0666);
		if(!fpdebug) {
			Error("Unable to open debug output file: %s",DEBUG_FILE);
			Exit(-1);
			exit(-1); /* Fatal - unfortunately */
		}
	}
	fprintf(fpdebug,"%d:",yylex_linenumber);
	va_start(ap, fmt);
	vfprintf(fpdebug,fmt,ap);
	va_end(ap);
	fflush(fpdebug);
#endif
}

void Error(char *fmt,...) {
	char msg[1024];		/* temporary string */
	char buf[1024];
	va_list ap;
	char *line=NULL;
	char *s;
	int pos=0, i=0, length=0;

	va_start(ap, fmt);
	vsprintf(msg, fmt, ap);
	va_end(ap);

	if(PrintErrors) {
#if DEBUG
		Debug("calling php_header from Error()\n");
#endif
		php_header(0,NULL);
	}
#if DEBUG
	if(!fpdebug) {
		fpdebug = fopen(DEBUG_FILE,"w");
		if(!fpdebug) {
			PUTS("Unable to open debug output file: ");
			PUTS(DEBUG_FILE);
			PUTS("\n");
			Exit(-1);
		}
	}
	fputs(msg,fpdebug);	
	fflush(fpdebug);
#endif
	Push((char *)msg,STRING);
	SetVar((char *)"phperrmsg",0,0);

	if(PrintErrors) {
		s = GetCurrentFilename();
		if(s && strlen(s))
			sprintf(buf,"<b><i>%s:</i> %s on line %d</b><br>\n",GetCurrentFilename(),msg,yylex_linenumber);
		else 
			sprintf(buf,"<b>%s</b><br>\n",msg);
		PUTS(buf);
		line = GetCurrentLexLine(&pos, &length);
		s = (char *)line;
		if(s && length) PUTS("<tt>");
#ifndef APACHE
		fflush(stdout);
#endif
		while(*s && i<length) {
			if(i==pos-1) {
				PUTS("<b><blink>");
			}
			switch(*s) {
			case '&':
				PUTS("&amp;");
				break;
			case '<':
				PUTS("&lt;");
				break;
			case '>':
				PUTS("&gt;");
				break;
			case '\"':
				PUTS("&quot;");
				break;
			default:
#if APACHE
				PUTC(*s);
#else
				fputc(*s,stdout);
#endif
			}
			s++;
			i++;
		}
		if(s && length) PUTS("</blink></b></tt><br>");
#ifndef APACHE
		fflush(stdout);
#endif
	}
}

/* Sets current error reporting state and returns previous setting */
int ErrorPrintState(int state) {
	int ret;

	ret = PrintErrors;
	PrintErrors=state;
	return(ret);
}	

void SetErrorReporting(void) {
	Stack *s;
	char temp[16];
	int ret;

	s = Pop();
	if(!s) {
		Error("Stack error in seterrorreporting");
		return;
	}
	ret = ErrorPrintState(s->intval);
	sprintf(temp,"%d",ret);
	Push((char *)temp,LNUMBER);
}
