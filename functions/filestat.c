/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
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
   | Author:  Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id: filestat.c,v 1.95 2000/06/12 14:39:44 eschmid Exp $ */
#include "php.h"
#include "internal_functions.h"
#include "safe_mode.h"
#include "fopen-wrappers.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#ifdef OS2
#  define INCL_DOS
#  include <os2.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
# include <sys/statvfs.h>
#elif defined(HAVE_SYS_STATFS_H) && defined(HAVE_STATFS)
# include <sys/statfs.h>
#endif

#if HAVE_PWD_H
# if MSVC5
#  include "win32/pwd.h"
# else
#  include <pwd.h>
# endif
#endif

#ifdef HAVE_GRP_H
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

static char *CurrentStatFile=NULL;
# if MSVC5
static unsigned int CurrentStatLength=0;
# else
static int CurrentStatLength=0;
# endif
static struct stat sb;
#ifdef HAVE_SYMLINK
static struct stat lsb;
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode)	(((mode)&S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(mode)	(((mode)&S_IFMT) == S_IFREG)
#endif
#ifndef S_ISLNK
#define S_ISLNK(mode)	(((mode)&S_IFMT) == S_IFLNK)
#endif


int php3_init_filestat(INIT_FUNC_ARGS)
{
	TLS_VARS;
	
	GLOBAL(CurrentStatFile)=NULL;
	GLOBAL(CurrentStatLength)=0;
	return SUCCESS;
}

int php3_shutdown_filestat(void)
{
	TLS_VARS;
	
	if (GLOBAL(CurrentStatFile))
		efree (GLOBAL(CurrentStatFile));
	return SUCCESS;
}

/* {{{ proto bool diskfree(string path)
   Return number of kilobytes free in path */
void php3_diskfreespace(INTERNAL_FUNCTION_PARAMETERS)
{
#ifdef WINDOWS
	pval *path;
	double bytesfree;

	HINSTANCE kernel32;
	FARPROC gdfse;
	typedef BOOL (WINAPI *gdfse_func)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
	gdfse_func func;

	/* These are used by GetDiskFreeSpaceEx, if available. */
	ULARGE_INTEGER FreeBytesAvailableToCaller;
	ULARGE_INTEGER TotalNumberOfBytes;
  	ULARGE_INTEGER TotalNumberOfFreeBytes;

	/* These are used by GetDiskFreeSpace otherwise. */
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD NumberOfFreeClusters;
	DWORD TotalNumberOfClusters;

#else /* not - WINDOWS */
	pval *path;
#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	struct statvfs buf;
#elif defined(HAVE_SYS_STATFS_H) && defined(HAVE_STATFS)
	struct statfs buf;
#endif
	double bytesfree = 0;
	TLS_VARS;
#endif /* WINDOWS */

	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&path)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(path);

	if (_php3_check_open_basedir(path->value.str.val)) RETURN_FALSE;

#ifdef WINDOWS
	/* GetDiskFreeSpaceEx is only available in NT and Win95 post-OSR2,
	   so we have to jump through some hoops to see if the function
	   exists. */
	kernel32 = LoadLibrary("kernel32.dll");
	if (kernel32) {
		gdfse = GetProcAddress(kernel32, "GetDiskFreeSpaceExA");
		/* It's available, so we can call it. */
		if (gdfse) {
			func = (gdfse_func)gdfse;
			if (func(path->value.str.val,
				&FreeBytesAvailableToCaller,
				&TotalNumberOfBytes,
				&TotalNumberOfFreeBytes) == 0) RETURN_FALSE;

			/* i know - this is ugly, but i works (thies@digicol.de) */
			bytesfree  = FreeBytesAvailableToCaller.HighPart * 
				(double) (((unsigned long)1) << 31) * 2.0 +
				FreeBytesAvailableToCaller.LowPart;
		}
		/* If it's not available, we just use GetDiskFreeSpace */
		else {
			if (GetDiskFreeSpace(path->value.str.val,
				&SectorsPerCluster, &BytesPerSector,
				&NumberOfFreeClusters, &TotalNumberOfClusters) == 0) RETURN_FALSE;
			bytesfree = (double)NumberOfFreeClusters * (double)SectorsPerCluster * (double)BytesPerSector;
		}
	}
	else {
		php3_error(E_WARNING, "Unable to load kernel32.dll");
		RETURN_FALSE;
	}

#elif defined(OS2)
	{
		FSALLOCATE fsinfo;
  		char drive = path->value.str.val[0] & 95;
  		
		if (DosQueryFSInfo( drive ? drive - 64 : 0, FSIL_ALLOC, &fsinfo, sizeof( fsinfo ) ) == 0)
			bytesfree = (double)fsinfo.cbSector * fsinfo.cSectorUnit * fsinfo.cUnitAvail;
	}
#else /* WINDOWS, OS/2 */
#if defined(HAVE_SYS_STATVFS_H) && defined(HAVE_STATVFS)
	if (statvfs(path->value.str.val,&buf)) RETURN_FALSE;
	if (buf.f_frsize) {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_frsize));
	} else {
		bytesfree = (((double)buf.f_bavail) * ((double)buf.f_bsize));
	}
#elif defined(HAVE_SYS_STATFS_H) && defined(HAVE_STATFS)
	if (statfs(path->value.str.val,&buf)) RETURN_FALSE;
	bytesfree = (((double)buf.f_bsize) * ((double)buf.f_bavail));
#endif
#endif /* WINDOWS */

	RETURN_DOUBLE(bytesfree);
}
/* }}} */

/* {{{ proto bool chgrp(string filename, mixed group)
   Change file group */
void php3_chgrp(INTERNAL_FUNCTION_PARAMETERS)
{
#ifndef WINDOWS
	pval *filename, *group;
	gid_t gid;
	struct group *gr=NULL;
#endif
	int ret;
	TLS_VARS;
	ret = 0;
#if !defined(WINDOWS) && !defined(OS2)

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&filename,&group)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	if (group->type == IS_STRING) {
		gr = getgrnam(group->value.str.val);
		if (!gr) {
			php3_error(E_WARNING, "unable to find gid for %s",
					   group->value.str.val);
			RETURN_FALSE;
		}
		gid = gr->gr_gid;
	} else {
		convert_to_long(group);
		gid = group->value.lval;
	}

/* #if PHP_SAFE_MODE */
	if (php3_ini.safe_mode &&(!_php3_checkuid(filename->value.str.val, 1))) {
		RETURN_FALSE;
	}
/* #endif */

	/* Check the basedir */
	if (_php3_check_open_basedir(filename->value.str.val)) RETURN_FALSE;

	ret = chown(filename->value.str.val, -1, gid);
	if (ret == -1) {
		php3_error(E_WARNING, "chgrp failed: %s", strerror(errno));
		RETURN_FALSE;
	}
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool chown(string filename, mixed user)
   Change file owner */
void php3_chown(INTERNAL_FUNCTION_PARAMETERS)
{
#if !defined(WINDOWS) && !defined(OS2)
	pval *filename, *user;
	int ret;
	uid_t uid;
	struct passwd *pw = NULL;
	TLS_VARS;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&filename,&user)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	if (user->type == IS_STRING) {
		pw = getpwnam(user->value.str.val);
		if (!pw) {
			php3_error(E_WARNING, "unable to find uid for %s",
					   user->value.str.val);
			RETURN_FALSE;
		}
		uid = pw->pw_uid;
	} else {
		convert_to_long(user);
		uid = user->value.lval;
	}

	if (php3_ini.safe_mode &&(!_php3_checkuid(filename->value.str.val, 1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (_php3_check_open_basedir(filename->value.str.val)) RETURN_FALSE;

	ret = chown(filename->value.str.val, uid, -1);
	if (ret == -1) {
		php3_error(E_WARNING, "chown failed: %s", strerror(errno));
		RETURN_FALSE;
	}
#else
	TLS_VARS;
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool chmod(string filename, int mode)
   Change file mode */
void php3_chmod(INTERNAL_FUNCTION_PARAMETERS) {
	pval *filename, *mode;
	int ret;
	TLS_VARS;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&filename,&mode)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);
	convert_to_long(mode);

	if (php3_ini.safe_mode &&(!_php3_checkuid(filename->value.str.val, 1))) {
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (_php3_check_open_basedir(filename->value.str.val)) RETURN_FALSE;

	ret = chmod(filename->value.str.val, mode->value.lval);
	if (ret == -1) {
		php3_error(E_WARNING, "chmod failed: %s", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool touch(string filename [, int time])
   Set modification time of file */
void php3_touch(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_UTIME
	pval *filename, *filetime;
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

	if (php3_ini.safe_mode &&(!_php3_checkuid(filename->value.str.val, 1))) {
		if (newtime) efree(newtime);
		RETURN_FALSE;
	}

	/* Check the basedir */
	if (_php3_check_open_basedir(filename->value.str.val)) RETURN_FALSE;

	/* create the file if it doesn't exist already */
	ret = stat(filename->value.str.val, &sb);
	if (ret == -1) {
		file = fopen(filename->value.str.val, "w");
		if (file == NULL) {
			php3_error(E_WARNING, "unable to create file %s because %s", filename->value.str.val, strerror(errno));
			if (newtime) efree(newtime);
			RETURN_FALSE;
		}
		fclose(file);
	}

	ret = utime(filename->value.str.val, newtime);
	if (newtime) efree(newtime);
	if (ret == -1) {
		php3_error(E_WARNING, "utime failed: %s", strerror(errno));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
#endif
}
/* }}} */

/* {{{ proto void clearstatcache(void)
   Clear file stat cache */
void php3_clearstatcache(INTERNAL_FUNCTION_PARAMETERS) {
	TLS_VARS;
	
	if (GLOBAL(CurrentStatFile)) {
		efree(GLOBAL(CurrentStatFile));
		GLOBAL(CurrentStatFile) = NULL;
	}
}
/* }}} */

static void _php3_stat(const char *filename, int type, pval *return_value)
{
	struct stat *stat_sb = &GLOBAL(sb);
	TLS_VARS;

	if (!GLOBAL(CurrentStatFile) || strcmp(filename,GLOBAL(CurrentStatFile))) {
		if (!GLOBAL(CurrentStatFile)
			|| strlen(filename) > GLOBAL(CurrentStatLength)) {
			if (GLOBAL(CurrentStatFile)) efree(GLOBAL(CurrentStatFile));
			GLOBAL(CurrentStatLength) = strlen(filename);
			GLOBAL(CurrentStatFile) = estrndup(filename,GLOBAL(CurrentStatLength));
		} else {
			strcpy(GLOBAL(CurrentStatFile),filename);
		}
#ifdef HAVE_SYMLINK
		GLOBAL(lsb).st_mode = 0; /* mark lstat buf invalid */
#endif
		if (stat(GLOBAL(CurrentStatFile),&GLOBAL(sb))==-1) {
			if (type != 15 || errno != ENOENT) { /* fileexists() test must print no error */
				php3_error(E_NOTICE,"stat failed for %s (errno=%d - %s)",GLOBAL(CurrentStatFile),errno,strerror(errno));
			}
			efree(GLOBAL(CurrentStatFile));
			GLOBAL(CurrentStatFile)=NULL;
			RETURN_FALSE;
		}
	}

#ifdef HAVE_SYMLINK
	if (8 == type /* filetype */
		|| 14 == type /* is link */
		|| 16 == type) { /* lstat */

		/* do lstat if the buffer is empty */

		if (!GLOBAL(lsb).st_mode) {
			if (lstat(GLOBAL(CurrentStatFile),&GLOBAL(lsb)) == -1) {
				php3_error(E_NOTICE,"lstat failed for %s (errno=%d - %s)",GLOBAL(CurrentStatFile),errno,strerror(errno));
				RETURN_FALSE;
			}
		}
	}
#endif

	switch(type) {
	case 0: /* fileperms */
		RETURN_LONG((long)GLOBAL(sb).st_mode);
	case 1: /* fileinode */
		RETURN_LONG((long)GLOBAL(sb).st_ino);
	case 2: /* filesize  */
		RETURN_LONG((long)GLOBAL(sb).st_size);
	case 3: /* fileowner */
		RETURN_LONG((long)GLOBAL(sb).st_uid);
	case 4: /* filegroup */
		RETURN_LONG((long)GLOBAL(sb).st_gid);
	case 5: /* fileatime */
		RETURN_LONG((long)GLOBAL(sb).st_atime);
	case 6: /* filemtime */
		RETURN_LONG((long)GLOBAL(sb).st_mtime);
	case 7: /* filectime */
		RETURN_LONG((long)GLOBAL(sb).st_ctime);
	case 8: /* filetype */
#ifdef HAVE_SYMLINK
		if (S_ISLNK(GLOBAL(lsb).st_mode)) {
			RETURN_STRING("link",1);
		}
#endif
		switch(GLOBAL(sb).st_mode&S_IFMT) {
		case S_IFIFO: RETURN_STRING("fifo",1);
		case S_IFCHR: RETURN_STRING("char",1);
		case S_IFDIR: RETURN_STRING("dir",1);
#ifdef S_IFBLK		
		case S_IFBLK: RETURN_STRING("block",1);
#endif
		case S_IFREG: RETURN_STRING("file",1);
		}
		php3_error(E_WARNING,"Unknown file type (%d)",GLOBAL(sb).st_mode&S_IFMT);
		RETURN_STRING("unknown",1);
	case 9: /*is writable*/
		RETURN_LONG((GLOBAL(sb).st_mode&S_IWRITE)!=0);
	case 10: /*is readable*/
		RETURN_LONG((GLOBAL(sb).st_mode&S_IREAD)!=0);
	case 11: /*is executable*/
		RETURN_LONG((GLOBAL(sb).st_mode&S_IEXEC)!=0 && !S_ISDIR(GLOBAL(sb).st_mode));
	case 12: /*is file*/
		RETURN_LONG(S_ISREG(GLOBAL(sb).st_mode));
	case 13: /*is dir*/
		RETURN_LONG(S_ISDIR(GLOBAL(sb).st_mode));
	case 14: /*is link*/
#ifdef HAVE_SYMLINK
		RETURN_LONG(S_ISLNK(GLOBAL(lsb).st_mode));
#else
		RETURN_FALSE;
#endif
	case 15: /*file exists*/
		RETURN_TRUE; /* the false case was done earlier */
	case 16: /* lstat */
#ifdef HAVE_SYMLINK
		stat_sb = &GLOBAL(lsb);
#endif
		/* FALLTHROUGH */
	case 17: /* stat */
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_next_index_long(return_value, stat_sb->st_dev);
		add_next_index_long(return_value, stat_sb->st_ino);
		add_next_index_long(return_value, stat_sb->st_mode);
		add_next_index_long(return_value, stat_sb->st_nlink);
		add_next_index_long(return_value, stat_sb->st_uid);
		add_next_index_long(return_value, stat_sb->st_gid);
#ifdef HAVE_ST_BLKSIZE
		add_next_index_long(return_value, stat_sb->st_rdev);
#else
		add_next_index_long(return_value, -1);
#endif
		add_next_index_long(return_value, stat_sb->st_size);
		add_next_index_long(return_value, stat_sb->st_atime);
		add_next_index_long(return_value, stat_sb->st_mtime);
		add_next_index_long(return_value, stat_sb->st_ctime);
#ifdef HAVE_ST_BLKSIZE
		add_next_index_long(return_value, stat_sb->st_blksize);
#else
		add_next_index_long(return_value, -1);
#endif
#ifdef HAVE_ST_BLOCKS
		add_next_index_long(return_value, stat_sb->st_blocks);
#else
		add_next_index_long(return_value, -1);
#endif
		return;
	}
	php3_error(E_WARNING, "didn't understand stat call");
	RETURN_FALSE;
}

/* another quickie macro to make defining similar functions easier */
#define FileFunction(name, funcnum) \
void name(INTERNAL_FUNCTION_PARAMETERS) { \
	pval *filename; \
	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&filename) == FAILURE) { \
		WRONG_PARAM_COUNT; \
	} \
	convert_to_string(filename); \
	_php3_stat(filename->value.str.val, funcnum, return_value); \
}

/* {{{ proto int fileperms(string filename)
   Get file permissions */
FileFunction(php3_fileperms,0)
/* }}} */

/* {{{ proto int fileinode(string filename)
   Get file inode */
FileFunction(php3_fileinode,1)
/* }}} */

/* {{{ proto int filesize(string filename)
   Get file size */
FileFunction(php3_filesize, 2)
/* }}} */

/* {{{ proto int fileowner(string filename)
   Get file owner */
FileFunction(php3_fileowner,3)
/* }}} */

/* {{{ proto int filegroup(string filename)
   Get file group */
FileFunction(php3_filegroup,4)
/* }}} */

/* {{{ proto int fileatime(string filename)
   Get last access time of file */
FileFunction(php3_fileatime,5)
/* }}} */

/* {{{ proto int filemtime(string filename)
   Get last modification time of file */
FileFunction(php3_filemtime,6)
/* }}} */

/* {{{ proto int filectime(string filename)
   Get inode modification time of file */
FileFunction(php3_filectime,7)
/* }}} */

/* {{{ proto string filetype(string filename)
   Get file type */
FileFunction(php3_filetype, 8)
/* }}} */

/* {{{ proto int is_writeable(string filename)
   Returns true if file can be written to */
FileFunction(php3_iswritable, 9)
/* }}} */

/* {{{ proto int is_readable(string filename)
   Returns true if file can be read */
FileFunction(php3_isreadable,10)
/* }}} */

/* {{{ proto int is_executable(string filename)
   Returns true if file is executable */
FileFunction(php3_isexec,11)
/* }}} */

/* {{{ proto int is_file(string filename)
   Returns true if file is a regular file */
FileFunction(php3_isfile,12)
/* }}} */

/* {{{ proto int is_dir(string filename)
   Returns true if file is directory */
FileFunction(php3_isdir,13)
/* }}} */

/* {{{ proto int is_link(string filename)
   Returns true if file is symbolic link */
FileFunction(php3_islink,14)
/* }}} */

/* {{{ proto bool file_exists(string filename)
   Returns true if filename exists */
FileFunction(php3_fileexists,15)
/* }}} */

/* {{{ proto array lstat(string filename)
   Give information about a file or symbolic link */
FileFunction(php3_lstat,16)
/* }}} */

/* {{{ proto array stat(string filename)
   Give information about a file */
FileFunction(php3_stat,17)
/* }}} */

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
	{"file_exists",	php3_fileexists,	NULL},
	{"is_writeable",php3_iswritable,	NULL},
	{"is_readable",	php3_isreadable,	NULL},
	{"is_executable",php3_isexec,		NULL},
	{"is_file",		php3_isfile,		NULL},
	{"is_dir",		php3_isdir,			NULL},
	{"is_link",		php3_islink,		NULL},
	{"stat",		php3_stat,			NULL},
	{"lstat",		php3_lstat,			NULL},
	{"chown",		php3_chown,			NULL},
	{"chgrp",		php3_chgrp,			NULL},
	{"chmod",		php3_chmod,			NULL},
	{"touch",		php3_touch,			NULL},
	{"diskfreespace",php3_diskfreespace,NULL},
	{NULL, NULL, NULL}
};


php3_module_entry php3_filestat_module_entry = {
	"PHP_filestat", php3_filestat_functions, NULL, NULL, php3_init_filestat, php3_shutdown_filestat, NULL, STANDARD_MODULE_PROPERTIES
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
