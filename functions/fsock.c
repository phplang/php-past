/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
*/
/* $Id: fsock.c,v 1.76 1998/11/18 21:23:06 ssb Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"
#include "php3_list.h"
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

#define FREE_SOCK efree(sock)

#if WIN32|WINNT
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
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

/* 
   This function takes an optional third argument which should be
   passed by reference.  The error code from the connect call is written
   to this variable.
*/
/* {{{ proto int fsockopen(string hostname, int port [, int errno [, string errstr]])
   Open Internet or Unix domain socket connection */
void php3_fsockopen(INTERNAL_FUNCTION_PARAMETERS) {
	pval *args[4];
	int *sock=emalloc(sizeof(int));
	int id, socketd, arg_count=ARG_COUNT(ht);
	unsigned short portno;
	TLS_VARS;
	
	if (arg_count > 4 || arg_count < 2 || getParametersArray(ht,arg_count,args)==FAILURE) {
		FREE_SOCK;
		WRONG_PARAM_COUNT;
	}
	switch(arg_count) {
		case 4:
			if(!ParameterPassedByReference(ht,4)) {
				php3_error(E_WARNING,"error string argument to fsockopen not passed by reference");
			}
			pval_copy_constructor(args[3]);
			args[3]->value.str.val = empty_string;
			args[3]->value.str.len = 0;
			args[3]->type = IS_STRING;
			/* fall-through */
		case 3:
			if(!ParameterPassedByReference(ht,3)) {
				php3_error(E_WARNING,"error argument to fsockopen not passed by reference");
			}
			args[2]->type = IS_LONG;
			args[2]->value.lval = 0;
			break;
	}
	convert_to_string(args[0]);
	convert_to_long(args[1]);
	portno = (unsigned short) args[1]->value.lval;

	if (portno) {
		struct sockaddr_in server;

		memset(&server, 0, sizeof(server));
		socketd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketd == SOCK_ERR) {
			FREE_SOCK;
			RETURN_FALSE;
		}
	  
		server.sin_addr.s_addr = lookup_hostname(args[0]->value.str.val);
		server.sin_family = AF_INET;
		
		if (server.sin_addr.s_addr == -1) {
			FREE_SOCK;
			RETURN_FALSE;
		}
  
		server.sin_port = htons(portno);
  
		if (connect(socketd, (struct sockaddr *)&server, sizeof(server)) == SOCK_CONN_ERR) {
			FREE_SOCK;
			if(arg_count>2) args[2]->value.lval = errno;
			if(arg_count>3) {
				args[3]->value.str.val = estrdup(strerror(errno));
				args[3]->value.str.len = strlen(args[3]->value.str.val);
			}
			RETURN_FALSE;
		}
#if defined(AF_UNIX)
	} else {
		/* Unix domain socket.  s->strval is socket name. */
		struct  sockaddr_un unix_addr;
		socketd = socket(AF_UNIX,SOCK_STREAM,0);
		if (socketd == SOCK_ERR) {
			FREE_SOCK;
			RETURN_FALSE;
		}
	  
		memset(&unix_addr,(char)0,sizeof(unix_addr));
		unix_addr.sun_family = AF_UNIX;
		strcpy(unix_addr.sun_path, args[0]->value.str.val);

		if (connect(socketd, (struct sockaddr *) &unix_addr, sizeof(unix_addr)) == SOCK_CONN_ERR) {
			FREE_SOCK;
			if(arg_count>2) args[2]->value.lval = errno;
			if(arg_count>3) {
				args[3]->value.str.val = estrdup(strerror(errno));
				args[3]->value.str.len = strlen(args[3]->value.str.val);
			}
			RETURN_FALSE;
		}
#endif /* AF_UNIX */
	}

#if 0
	if ((fp = fdopen (socketd, "r+")) == NULL){
		RETURN_LONG(-6);  /* FIXME */
	}

#ifdef HAVE_SETVBUF  
	if ((setvbuf(fp, NULL, _IONBF, 0)) != 0){
		RETURN_LONG(-7);  /* FIXME */
	}
#endif
#endif

	*sock=socketd;
	id = php3_list_insert(sock,GLOBAL(wsa_fp));
	RETURN_LONG(id);
}
/* }}} */


int _php3_sock_fgets(char *buf, int maxlen, int socket)
{
	int retval;
	int count=0;
	
	while (count<maxlen && (retval=recv(socket,buf,1,0))!=SOCK_RECV_ERR && retval!=EWOULDBLOCK && retval!=0) {
		if (*buf=='\n') {
			buf++;
			count++;
			break;
		}
		buf++;
		count++;
	}
	
	*buf = '\0';
	return count;
}


int _php3_sock_fread(char *buf, int maxlen, int socket)
{
	int retval;
	int count=0;
	
	while (count<maxlen && (retval=recv(socket,buf,1,0))!=SOCK_RECV_ERR && retval!=EWOULDBLOCK && retval!=0) {
		buf++;
		count++;
	}
	
	*buf = '\0';
	return count;
}
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
