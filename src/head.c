/***[head.c]******************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996 Rasmus Lerdorf                                         *
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
/* $Id: head.c,v 1.11 1996/05/22 18:47:56 rasmus Exp $ */
#include <php.h>
#include <parse.h>
#if APACHE
#include "http_protocol.h"
#endif

static int HeaderPrinted=0;
static int PrintHeader=1;

void php_init_head(void) {
	HeaderPrinted=0;
	PrintHeader=1;
}

void Header(void) {
	Stack *s;
#if APACHE
	char *r;
#endif

#ifndef APACHE
	HeaderPrinted=1;
#endif
	s = Pop();
	if(!s) {
		Error("Stack error in echo\n");
		return;
	}
#if APACHE
	if(HeaderPrinted==1) {
#if DEBUG
		Debug("Woops, call to Header - already gone\n");
#endif
		return; /* too late, already sent */
	}
	ParseEscapes(s->strval);
	/*
	 * Not entirely sure this is the right way to support the header
	 * command in the Apache module.  Comments?
	 */
	r = strchr(s->strval,':');
	if(r) {
		*r='\0';
		table_set(php_rqst->headers_out,s->strval,r+1);
		if(!strcasecmp(s->strval,"location")) php_rqst->status = REDIRECT;
#if DEBUG
		Debug("Redirecting to: %s\n",s->strval);
#endif
		*r=':';
		HeaderPrinted = 2;
	}		
#else
	ParseEscapes(s->strval);
	PUTS(s->strval);
	PUTS("\015\012");
#endif
}

void php_header(int type,char *str) {
#if APACHE
    if((PrintHeader && !HeaderPrinted) || (PrintHeader && HeaderPrinted==2)) { 
        if(HeaderPrinted!=2) php_rqst->content_type = "text/html";
		if(type==1) {
			table_set(php_rqst->headers_out,"Location",str);
			php_rqst->status = REDIRECT;
		}
        HeaderPrinted = 1;
#if DEBUG
		Debug("Sending header\n");
#endif
        send_http_header(php_rqst);
    }
#else
    if(PrintHeader && !HeaderPrinted) { 
		if(type==1) {
			fputs("Location: ",stdout);
			fputs(str,stdout);
			fputs("\015\012\015\012",stdout);
		} else {
	        fputs("Content-type: text/html\015\012\015\012",stdout); 
		}
        HeaderPrinted = 1;
    }
#endif
}
