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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
/* $Id: safe_mode.c,v 1.16 1998/01/29 22:33:41 shane Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#if HAVE_PWD_H
#if MSVC5
#include "win32/pwd.h"
#include "win32/param.h"
#else
#include <pwd.h>
#include <sys/param.h>
#endif
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include "functions/pageinfo.h"
#include "safe_mode.h"

/*
 * _php3_checkuid
 *
 * This function has four modes:
 * 
 * 0 - return invalid (0) if file does not exist
 * 1 - return valid (1)  if file does not exist
 * 2 - if file does not exist, check directory
 * 3 - only check directory (needed for mkdir)
 */
int _php3_checkuid(const char *fn, int mode) {
	struct stat sb;
	int ret;
	long uid=0L, duid=0L;
	char *s;

	if(!fn) return(0); /* path must be provided */

	/* 
	 * If given filepath is a URL, allow - safe mode stuff
	 * related to URL's is checked in individual functions
     */	
	if(!strncasecmp(fn,"http://",7)) {
		return(1);
	}
		
	if(mode<3) {
		ret = stat(fn,&sb);
		if(ret<0 && mode < 2) return(mode);
		if(ret>-1) {
			uid=sb.st_uid;
			if(uid==_php3_getuid()) return(1);
		}
	}
	s = strrchr(fn,'/');

	/* This loop gets rid of trailing slashes which could otherwise be
	 * used to confuse the function.
	 */
	while(s && *(s+1)=='\0' && s>fn) {
		s='\0';
		s = strrchr(fn,'/');
	}

	if(s) {
		*s='\0';
		ret = stat(fn,&sb);
		*s='/';
		if(ret<0) return(0);
		duid = sb.st_uid;
	} else {
		s = emalloc(MAXPATHLEN+1);
		if(!getcwd(s,MAXPATHLEN)) return(0);
		ret = stat(s,&sb);
		efree(s);
		if(ret<0) return(0);
		duid = sb.st_uid;
	}
	if(duid == _php3_getuid()) return(1);
	else return(0);
}


PHPAPI char *_php3_get_current_user()
{
#if CGI_BINARY || USE_SAPI
	struct stat statbuf;
#endif
	struct passwd *pwd;
	int uid;
	TLS_VARS;

	if (GLOBAL(request_info).current_user) {
		return GLOBAL(request_info).current_user;
	}

	/* FIXME: I need to have this somehow handled if
	USE_SAPI is defined, because cgi will also be
	interfaced in USE_SAPI */
#if CGI_BINARY || USE_SAPI
	if (!GLOBAL(request_info).filename || (stat(GLOBAL(request_info).filename,&statbuf)==-1)) {
		return empty_string;
	}
	uid = statbuf.st_uid;
#endif
#if APACHE
	uid = GLOBAL(php3_rqst)->finfo.st_uid;
#endif

	if ((pwd=getpwuid(uid))==NULL) {
		return empty_string;
	}
	GLOBAL(request_info).current_user_length = strlen(pwd->pw_name);
	GLOBAL(request_info).current_user = estrndup(pwd->pw_name,GLOBAL(request_info).current_user_length);
	
	return GLOBAL(request_info).current_user;		
}	
