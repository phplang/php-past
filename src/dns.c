/***[dns.c]*******************************************************[TAB=4]****\
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
/* $Id: dns.c,v 1.9 1997/09/13 16:14:26 shane Exp $ */
#include "php.h"
#include "parse.h"
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if WINNT|WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

void GetHostByAddr(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in echo\n");
		return;
	}
	Push((char *)_GetHostByAddr(s->strval),STRING);
}


const char *_GetHostByAddr(char *ip) {
	unsigned long addr;
	static struct hostent *hp;

#if DEBUG
	Debug("_GetHostByAddr called with [%s]\n",ip);
#endif
	if((int)(addr = inet_addr(ip)) == -1) {
#if DEBUG
		Debug("address not in a.b.c.d form");
#endif
		return(ip);
	}
	hp = gethostbyaddr((char *)&addr, sizeof (addr), AF_INET);
	if(!hp) {
#if DEBUG
		Debug("Unable to resolve %s\n",ip);
#endif
		return(ip);
	}
#if DEBUG
	Debug("_GetHostByAddr returning [%s]\n",hp->h_name);
#endif
	return(hp->h_name);
}

void GetHostByName(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in echo\n");
		return;
	}
	Push(_GetHostByName(s->strval),STRING);
}

char *_GetHostByName(char *name) {
	static struct hostent *hp;
	static struct in_addr in;

	hp = gethostbyname(name);
	if(!hp) {
		Error("Unable to resolve %s\n",name);
		return(name);
	}
	memcpy(&in.s_addr, *(hp->h_addr_list), sizeof (in.s_addr));
	return(inet_ntoa(in));
}
