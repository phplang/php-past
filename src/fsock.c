/***[fsock.c]*****************************************************[TAB=4]****\
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
 * Contributed by Paul Panotzki - Bunyip Information Systems
 *                                                         
 */
/* $Id: fsock.c,v 1.14 1997/09/18 20:31:15 shane Exp $ */
#include "php.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if WINNT|WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#endif
#if WINNT|WIN32
#undef AF_UNIX
#endif
#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include <string.h>
#include <errno.h>
#include "parse.h"

void FSockOpen(void) {
	Stack *s;
	char temp[8];
	FILE *fp;
	int id, socketd;
#ifdef WIN32
	unsigned short portno;
	struct hostent FAR *hostp;
	struct hostent FAR * FAR PASCAL gethostbyname();
#else
	int portno;
	struct hostent *hostp, *gethostbyname();
#endif

	s = Pop();
	if(!s) {
		Error("Stack error in fsockopen");
		return;
	}
#if !defined(AF_UNIX)
	if(!(s->intval)) {
	  Push("-2",LNUMBER);
	  return;
	}
#endif

#if WINNT|WIN32
	/* don't know if this is a good thing */
	portno = (unsigned short) s->intval;
#else
	portno = s->intval;
#endif

	s = Pop();
	if(!s) {
		Error("Stack error in fsockopen");
		return;
	}
	if(!*(s->strval)) {
		Push("-1",LNUMBER);
		return;
	}
  
	if (portno) {
	  struct sockaddr_in server;
	  socketd = socket(AF_INET, SOCK_STREAM, 0);
	  if(socketd < 0) {
	    Push("-3",LNUMBER);
	    return;
	  }
  
	  server.sin_family = AF_INET;
	  hostp = gethostbyname(s->strval);
	  if(hostp == 0) {
	    Push("-4",LNUMBER);
	    return;
	  }
  
	  memcpy(&server.sin_addr, hostp->h_addr, hostp->h_length);
	  server.sin_port = htons(portno);
  
	  if(connect(socketd, (struct sockaddr *)&server, sizeof(server)) < 0) {
	    Push("-5",LNUMBER);
	    return;
	  }
#if defined(AF_UNIX)
	} else {
	  /* Unix domain socket.  s->strval is socket name. */
	  struct  sockaddr_un unix_addr;
	  socketd = socket(AF_UNIX,SOCK_STREAM,0);
	  if (socketd < 0) {
	    Push("-3",LNUMBER);
	    return;
	  }
	  
	  memset(&unix_addr,(char)0,sizeof(unix_addr));
	  unix_addr.sun_family = AF_UNIX;
	  strcpy(unix_addr.sun_path, s->strval);

	  if (connect(socketd, (struct sockaddr *) &unix_addr,
		      sizeof(unix_addr)) < 0) {
	    Push("-5",LNUMBER);
	    return;
	  }
#endif /* AF_UNIX */
	}
	
	if ((fp = fdopen (socketd, "r+")) == NULL){
		Push("-6",LNUMBER);
		return;
	}

#ifdef HAVE_SETVBUF  
	if ((setvbuf(fp, NULL, _IONBF, 0)) != 0){
		Push("-7",LNUMBER);
		return;
	}
#endif
 
	id = FpPush(fp,s->strval,1);
	sprintf(temp,"%d",id);	
	Push(temp,LNUMBER);
}	
