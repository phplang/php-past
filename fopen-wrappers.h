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
   | Authors: Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */
/* $Id: fopen-wrappers.h,v 1.9 1998/03/01 21:29:11 jaakko Exp $ */

#ifndef _FOPEN_WRAPPERS_H
#define _FOPEN_WRAPPERS_H

#define IGNORE_PATH	0
#define USE_PATH	1
#define IGNORE_URL	2
/* There's no USE_URL. */
#if WIN32|WINNT
# define IGNORE_URL_WIN 2
#else
# define IGNORE_URL_WIN 0
#endif
#define ENFORCE_SAFE_MODE 4

/* 
   The SOCK_ARG defines are used to expand the definition
   of the fopen() wrapper so that we can support URL referneces
   in Windows, while not adding unused args into Unix.
*/

/* Defines to handle differences between Unix and Windows sockets */

#if WIN32|WINNT
# define SOCK_VARS int issock = 0; int socketd = 0;
# define SOCK_PARG ,&issock,&socketd
# define SOCK_ARG ,issock,socketd
# define SOCK_ARG_IN ,int *issock,int *socketd
# define SOCK_ERR ==INVALID_SOCKET
# define SOCK_CONN_ERR ==SOCKET_ERROR
# define SOCK_WRITE(d,s) send(s,d,strlen(d),0)
# define SOCK_WRITEL(d,l,s) send(s,d,l,0)
# define SOCK_FGETC(c,s) recv(s,(char *)&c,1,0)
# define SOCK_FGETS(b,l,s) recv(s,b,l,0)
# define SOCK_FCLOSE(s) closesocket(s)
# define SOCK_FP *socketd
# define SOCK_FPC *socketd
# define SOCK_FTP socketd
# define SOCKETD *socketd
# ifndef THREAD_SAFE
extern int wsa_fp; /* a list for open sockets */
# endif
#else
# define SOCK_VARS
# define SOCK_ARG
# define SOCK_PARG
# define SOCK_ARG_IN
# define SOCK_ERR <0
# define SOCK_CONN_ERR <0
# define SOCK_WRITE(d,s) fputs(d,s)
# define SOCK_WRITEL(d,s) fwrite(d,l,1,s)
# define SOCK_FGETC(c,s) c=fgetc(s)
# define SOCK_FGETS(b,l,s) fgets(b,l,s)
# define SOCK_FCLOSE(s) fclose(s)
# define SOCK_FP fp
# define SOCK_FPC fpc
# define SOCK_FTP fpc
# define SOCKETD socketd
#endif

extern PHPAPI FILE *php3_fopen_wrapper(char *filename, char *mode, int options SOCK_ARG_IN);

extern FILE *php3_fopen_for_parser(void);

extern PHPAPI FILE *php3_fopen_with_path(char *filename, char *mode, char *path, char **opened_path);

extern PHPAPI int php3_isurl(char *path);
extern PHPAPI char *php3_strip_url_passwd(char *path);
extern PHPAPI int php3_write(void *buf, int size);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
