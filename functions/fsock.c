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
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
*/
/* $Id: fsock.c,v 1.57 1998/02/01 21:33:09 shane Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "list.h"
#include "internal_functions.h"
#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "fsock.h"

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if MSVC5
#include <winsock.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#if MSVC5
#undef AF_UNIX
#endif
#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include <string.h>
#include <errno.h>

#include "base64.h"
#include "file.h"
#include "post.h"
#include "url.h"

#ifndef THREAD_SAFE
extern int le_fp;
#endif

#if WIN32|WINNT
#define FREE_SOCK efree(sock)
#else
#define FREE_SOCK
#include "build-defs.h"
#endif

/*
 * Converts a host name to an IP address.
 */
int lookup_hostname(const char *addr)
{
	uint r;
	struct hostent *host_info;

	r = inet_addr(addr);
	if (r == (uint) - 1) {
		host_info = gethostbyname(addr);
		if (host_info == 0) {
			/* Error: unknown host */
			return -1;
		}
		memcpy((char *) &r, host_info->h_addr, host_info->h_length);
	}
	return r;
}


/* FIXME: do something about the non-standard return codes from this function */
void php3_fsockopen(INTERNAL_FUNCTION_PARAMETERS) {
	YYSTYPE *arg1, *arg2;
#if (WIN32|WINNT)
	int *sock=emalloc(sizeof(int));
#else
	FILE *fp;
#endif
	int id, socketd;
	unsigned short portno;
	TLS_VARS;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&arg1,&arg2)==FAILURE) {
		FREE_SOCK;
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
	convert_to_long(arg2);
	portno = (unsigned short) arg2->value.lval;

	if(portno) {
		struct sockaddr_in server;

		memset(&server, 0, sizeof(server));
		socketd = socket(AF_INET, SOCK_STREAM, 0);
		if(socketd SOCK_ERR) {
			FREE_SOCK;
			RETURN_LONG(-3); /* FIXME */
		}
	  
		server.sin_addr.s_addr = lookup_hostname(arg1->value.strval);
		server.sin_family = AF_INET;
		
		if(server.sin_addr.s_addr == -1) {
			FREE_SOCK;
			RETURN_LONG(-4); /* FIXME */
		}
  
		server.sin_port = htons(portno);
  
		if(connect(socketd, (struct sockaddr *)&server, sizeof(server)) SOCK_CONN_ERR) {
			FREE_SOCK;
			RETURN_LONG(-5); /* FIXME */
		}
#if defined(AF_UNIX)
	} else {
		/* Unix domain socket.  s->strval is socket name. */
		struct  sockaddr_un unix_addr;
		socketd = socket(AF_UNIX,SOCK_STREAM,0);
		if (socketd SOCK_ERR) {
			FREE_SOCK;
			RETURN_LONG(-3);  /* FIXME */
		}
	  
		memset(&unix_addr,(char)0,sizeof(unix_addr));
		unix_addr.sun_family = AF_UNIX;
		strcpy(unix_addr.sun_path, arg1->value.strval);

		if (connect(socketd, (struct sockaddr *) &unix_addr, sizeof(unix_addr)) SOCK_CONN_ERR) {
			FREE_SOCK;
			RETURN_LONG(-5);  /* FIXME */
		}
#endif /* AF_UNIX */
	}

#if !(WIN32|WINNT)
	if ((fp = fdopen (socketd, "r+")) == NULL){
		RETURN_LONG(-6);  /* FIXME */
	}

#ifdef HAVE_SETVBUF  
	if ((setvbuf(fp, NULL, _IONBF, 0)) != 0){
		RETURN_LONG(-7);  /* FIXME */
	}
#endif
#endif

#if WIN32|WINNT
	*sock=socketd;
	id = php3_list_insert(sock,GLOBAL(wsa_fp));
#else
	id = php3_list_insert(fp,GLOBAL(le_fp));
#endif
	RETURN_LONG(id);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
