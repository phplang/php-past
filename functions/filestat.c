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
   | Author:  Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id: filestat.c,v 1.55 1998/01/25 22:25:23 ssb Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "safe_mode.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#if HAVE_PWD_H
# if MSVC5
#  include "win32/pwd.h"
# else
#  include <pwd.h>
# endif
#endif

#if HAVE_GRP_H
# if MSVC5
#  include "win32/grp.h"
# else
#  include <grp.h>
# endif
#endif

#if HAVE_UTIME
# if MSVC5
#  include <sys/utime.h>
# else
#  include <utime.h>
# endif
#endif

#include "php3_filestat.h"

#ifndef THREAD_SAFE
static char *CurrentStatFile=NULL;
# if MSVC5
static unsigned int CurrentStatLength=0;
# else
static int CurrentStatLength=0;
# endif
static struct stat sb;
#endif


int php3_init_filestat(INITFUNCARG) {
	TLS_VARS;
	
	GLOBAL(CurrentStatFile)=NULL;
	GLOBAL(CurrentStatLength)=0;
	return SUCCESS;
}

int php3_shutdown_filestat(void) {
	TLS_VARS;
	
	if (GLOBAL(CurrentStatFile))
		efree (GLOBAL(CurrentStatFile));
	return SUCCESS;
}

void php3_chgrp(INTERNAL_FUNCTION_PARAMETERS) {
#ifndef WINDOWS
	YYSTYPE *filename, *group;
	gid_t gid;
	struct group *gr=NULL;
#endif
	int ret;
	TLS_VARS;
	ret = 0;
#ifndef WINDOWS

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&filename,&group)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	if (group->type == IS_STRING) {
		gr = getgrnam(group->value.strval);
		if (!gr) {
			php3_error(E_WARNING, "unable to find gid for %s", group->value.strval);
			return;
		}
		gid = gr->gr_gid;
	} else {
		convert_to_long(group);
		gid = group->value.lval;
	}

/* #if PHP_SAFE_MODE */
	if (php3_ini.safe_mode &&(!_php3_checkuid(filename->value.strval, 1))) {
		php3_error(E_WARNING, "SAFE MODE restriction in effect. Invalid owner of file to be changed.");
		return;
	}
/* #endif */

	ret = chown(filename->value.strval, -1, gid);
	if (ret == -1) {
		php3_error(E_WARNING, "chgrp failed: %s", strerror(errno));
	}
#endif
	RETURN_LONG(ret);
}

void php3_chown(INTERNAL_FUNCTION_PARAMETERS) {
	int ret;
#ifndef WINDOWS

	YYSTYPE *filename, *user;
	uid_t uid;
	struct passwd *pw = NULL;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&filename,&user)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	if (user->type == IS_STRING) {
		pw = getpwnam(user->value.strval);
		if (!pw) {
			php3_error(E_WARNING, "unable to find uid for %s", user->value.strval);
			return;
		}
		uid = pw->pw_uid;
	} else {
		convert_to_long(user);
		uid = user->value.lval;
	}

	if (php3_ini.safe_mode &&(!_php3_checkuid(filename->value.strval, 1))) {
		php3_error(E_WARNING, "SAFE MODE restriction in effect. Invalid owner of file to be changed.");
		return;
	}

	ret = chown(filename->value.strval, uid, -1);
	if (ret == -1) {
		php3_error(E_WARNING, "chown failed: %s", strerror(errno));
	}
#else
	TLS_VARS;
	ret = 0;
#endif
	RETURN_LONG(ret);
}

void php3_chmod(INTERNAL_FUNCTION_PARAMETERS) {
	YYSTYPE *filename, *mode;
	int ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&filename,&mode)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	convert_to_long(mode);

	if (php3_ini.safe_mode &&(!_php3_checkuid(filename->value.strval, 1))) {
		php3_error(E_WARNING, "SAFE MODE restriction in effect. Invalid owner of file to be changed.");
		return;
	}

	ret = chmod(filename->value.strval, mode->value.lval);
	if (ret == -1) {
		php3_error(E_WARNING, "chmod failed: %s", strerror(errno));
	}
	RETURN_LONG(ret);
}

void php3_touch(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_UTIME
	YYSTYPE *filename, *filetime;
	int ret;
	struct stat sb;
	FILE *file;
	struct utimbuf *newtime = NULL;
	int ac = ARG_COUNT(ht);
	TLS_VARS;
	
	if (ac == 1 && getParameters(ht,1,&filename) != FAILURE) {
#ifndef HAVE_UTIME_NULL
		newtime = (struct utimbuf *)emalloc(sizeof(struct utimbuf));
		if (!newtime) {
			php3_error(E_WARNING, "unable to emalloc memory for changing time");
			return;
		}
		newtime->actime = time(NULL);
		newtime->modtime = newtime->actime;
#endif
	} else if (ac == 2 && getParameters(ht,2,&filename,&filetime) != FAILURE) {
		newtime = (struct utimbuf *)emalloc(sizeof(struct utimbuf));
		if (!newtime) {
			php3_error(E_WARNING, "unable to emalloc memory for changing time");
			return;
		}
		convert_to_long(filetime);
		newtime->actime = filetime->value.lval;
		newtime->modtime = filetime->value.lval;
	} else {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

	if (php3_ini.safe_mode &&(!_php3_checkuid(filename->value.strval, 1))) {
		php3_error(E_WARNING, "SAFE MODE restriction in effect. Invalid owner of file to be changed.");
		if (newtime) efree(newtime);
		RETURN_FALSE;
	}

	/* create the file if it doesn't exist already */
	ret = stat(filename->value.strval, &sb);
	if (ret == -1) {
		file = fopen(filename->value.strval, "w");
		if (file == NULL) {
			php3_error(E_WARNING, "unable to create file %s because %s", filename->value.strval, strerror(errno));
			if (newtime) efree(newtime);
			RETURN_FALSE;
		}
		fclose(file);
	}

	ret = utime(filename->value.strval, newtime);
	if (newtime) efree(newtime);
	if (ret == -1) {
		php3_error(E_WARNING, "utime failed: %s", strerror(errno));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
#endif
}

void php3_clearstatcache(INTERNAL_FUNCTION_PARAMETERS) {
	TLS_VARS;
	
	if(GLOBAL(CurrentStatFile)) {
		efree(GLOBAL(CurrentStatFile));
		GLOBAL(CurrentStatFile) = NULL;
	}
}

void php3_stat(INTERNAL_FUNCTION_PARAMETERS) {
	YYSTYPE *filename;
	TLS_VARS;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

#if APACHE
	if(!GLOBAL(CurrentStatFile)) {
		GLOBAL(CurrentStatLength) = strlen(GLOBAL(php3_rqst)->filename);
		GLOBAL(CurrentStatFile) = estrndup(GLOBAL(php3_rqst)->filename,GLOBAL(CurrentStatLength));
			if(stat(GLOBAL(CurrentStatFile),&GLOBAL(sb))==-1) {
				GLOBAL(CurrentStatFile)=NULL;
				return;
			}
    	}
#endif

	if (!GLOBAL(CurrentStatFile) || strcmp(filename->value.strval,GLOBAL(CurrentStatFile))) {
		if (strlen(filename->value.strval) >GLOBAL(CurrentStatLength)) {
			if (GLOBAL(CurrentStatFile)) efree(GLOBAL(CurrentStatFile));
			GLOBAL(CurrentStatFile) = estrndup(filename->value.strval,filename->strlen);
			GLOBAL(CurrentStatLength) = strlen(filename->value.strval);
		} else {
			strcpy(GLOBAL(CurrentStatFile),filename->value.strval);
		}
		if(stat(GLOBAL(CurrentStatFile),&GLOBAL(sb))==-1) {
			efree(GLOBAL(CurrentStatFile));
			GLOBAL(CurrentStatFile)=NULL;
			GLOBAL(CurrentStatLength)=0;
			RETURN_FALSE;
		}
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_next_index_long(return_value, GLOBAL(sb).st_dev);
	add_next_index_long(return_value, GLOBAL(sb).st_ino);
	add_next_index_long(return_value, GLOBAL(sb).st_mode);
	add_next_index_long(return_value, GLOBAL(sb).st_nlink);
	add_next_index_long(return_value, GLOBAL(sb).st_uid);
	add_next_index_long(return_value, GLOBAL(sb).st_gid);
#ifdef HAVE_ST_BLKSIZE
	add_next_index_long(return_value, GLOBAL(sb).st_rdev);
#else
	add_next_index_long(return_value, -1);
#endif
	add_next_index_long(return_value, GLOBAL(sb).st_size);
	add_next_index_long(return_value, GLOBAL(sb).st_atime);
	add_next_index_long(return_value, GLOBAL(sb).st_mtime);
	add_next_index_long(return_value, GLOBAL(sb).st_ctime);
#if HAVE_ST_BLKSIZE
	add_next_index_long(return_value, GLOBAL(sb).st_blksize);
#else
	add_next_index_long(return_value, -1);
#endif
#if HAVE_ST_BLOCKS
	add_next_index_long(return_value, GLOBAL(sb).st_blocks);
#else
	add_next_index_long(return_value, -1);
#endif
	return;
}

static void _php3_stat(const char *filename, int type, YYSTYPE *return_value)
{
	TLS_VARS;
	
#if APACHE
	if(!GLOBAL(CurrentStatFile)) {
		GLOBAL(CurrentStatLength) = strlen(GLOBAL(php3_rqst)->filename);
		GLOBAL(CurrentStatFile) = estrndup(GLOBAL(php3_rqst)->filename,GLOBAL(CurrentStatLength));
		if(stat(GLOBAL(CurrentStatFile),&GLOBAL(sb))==-1) {
			efree(GLOBAL(CurrentStatFile));
			GLOBAL(CurrentStatFile)=NULL;
			RETURN_FALSE;
		}
    }
#endif

	if (!GLOBAL(CurrentStatFile) || strcmp(filename,GLOBAL(CurrentStatFile))) {
		if (strlen(filename) > GLOBAL(CurrentStatLength)) {
			if(GLOBAL(CurrentStatFile)) efree(GLOBAL(CurrentStatFile));
			GLOBAL(CurrentStatLength) = strlen(filename);
			GLOBAL(CurrentStatFile) = estrndup(filename,GLOBAL(CurrentStatLength));
		} else {
			strcpy(GLOBAL(CurrentStatFile),filename);
		}
		if(stat(GLOBAL(CurrentStatFile),&GLOBAL(sb))==-1) {
			php3_error(E_NOTICE,"stat failed for %s (errno=%d - %s)",GLOBAL(CurrentStatFile),errno,strerror(errno));
			efree(GLOBAL(CurrentStatFile));
			GLOBAL(CurrentStatFile)=NULL;
			GLOBAL(CurrentStatLength)=0;
			RETURN_FALSE;
		}
	}
	switch(type) {
	case 0: /* fileperms */
		RETURN_LONG((long)GLOBAL(sb).st_mode);
		break;
	case 1: /* fileinode */
		RETURN_LONG((long)GLOBAL(sb).st_ino);
		break;
	case 2: /* filesize  */
		RETURN_LONG((long)GLOBAL(sb).st_size);
		break;
	case 3: /* fileowner */
		RETURN_LONG((long)GLOBAL(sb).st_uid);
		break;
	case 4: /* filegroup */
		RETURN_LONG((long)GLOBAL(sb).st_gid);
		break;
	case 5: /* fileatime */
		RETURN_LONG((long)GLOBAL(sb).st_atime);
		break;
	case 6: /* filemtime */
		RETURN_LONG((long)GLOBAL(sb).st_mtime);
		break;
	case 7: /* filectime */
		RETURN_LONG((long)GLOBAL(sb).st_ctime);
		break;
	case 8: /* filetype */
		switch(GLOBAL(sb).st_mode&S_IFMT) {
		case S_IFIFO:
			RETURN_STRING("fifo");
			break;
		case S_IFCHR:
			RETURN_STRING("char");
			break;
		case S_IFDIR:
			RETURN_STRING("dir");
			break;
		case S_IFBLK:
			RETURN_STRING("block");
			break;
		case S_IFREG:
			lstat(GLOBAL(CurrentStatFile),&GLOBAL(sb));
			if((GLOBAL(sb).st_mode&S_IFMT) == S_IFLNK) {
				RETURN_STRING("link");
			} else {
				RETURN_STRING("file");
			}
			break;	
		default:
			php3_error(E_WARNING,"Unknown file type (%d)",GLOBAL(sb).st_mode&S_IFMT);
			RETURN_STRING("unknown");
			break;
		}
		break;
	default:
		php3_error(E_WARNING, "didn't understand stat call");
		RETURN_FALSE;
		break;
	}
}

/* another quickie macro to make defining similar functions easier */
#define FileFunction(name, val) \
void name(INTERNAL_FUNCTION_PARAMETERS) { \
	YYSTYPE *filename; \
	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&filename) == FAILURE) { \
		WRONG_PARAM_COUNT; \
	} \
	convert_to_string(filename); \
	_php3_stat(filename->value.strval, val, return_value); \
}

FileFunction(php3_fileperms,0)
FileFunction(php3_fileinode,1)
FileFunction(php3_filesize, 2)
FileFunction(php3_fileowner,3)
FileFunction(php3_filegroup,4)
FileFunction(php3_fileatime,5)
FileFunction(php3_filemtime,6)
FileFunction(php3_filectime,7)
FileFunction(php3_filetype, 8)


function_entry php3_filestat_functions[] = {
	{"fileatime",	php3_fileatime,		NULL},
	{"filectime",	php3_filectime,		NULL},
	{"filegroup",	php3_filegroup,		NULL},
	{"fileinode",	php3_fileinode,		NULL},
	{"filemtime",	php3_filemtime,		NULL},
	{"fileowner",	php3_fileowner,		NULL},
	{"fileperms",	php3_fileperms,		NULL},
	{"filesize",	php3_filesize,		NULL},
	{"filetype",	php3_filetype,		NULL},
	{"stat",		php3_stat,			NULL},
	{"chown",		php3_chown,			NULL},
	{"chgrp",		php3_chgrp,			NULL},
	{"chmod",		php3_chmod,			NULL},
	{"touch",		php3_touch,			NULL},
	{NULL, NULL, NULL}
};


php3_module_entry php3_filestat_module_entry = {
	"PHP_filestat", php3_filestat_functions, NULL, NULL, php3_init_filestat, php3_shutdown_filestat, NULL, 0, 0, 0, NULL
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
