/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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
   | Authors: Kristian Koehntopp (kris@koehntopp.de)                      |
   +----------------------------------------------------------------------+
 */
 
/* $Id:  */


#include "php.h"
#include "internal_functions.h"
#if PHP_31
# include "ext/standard/php3_string.h"
#else
# include "php3_string.h"
#endif
#include "php3_posix.h"

#if HAVE_POSIX
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <unistd.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <errno.h>
#include "php3_list.h"

#define SAFE_STRING(s) ((s)?(s):"")

function_entry posix_functions[] = {
	{"posix_getpid",		php3_posix_getpid,	NULL},
	{"posix_getppid",		php3_posix_getppid,	NULL},
	{"posix_getpgrp",		php3_posix_getpgrp,	NULL},
	{"posix_getpgid",		php3_posix_getpgid,	NULL},
	{"posix_getsid",		php3_posix_getsid,	NULL},

	{"posix_getuid",		php3_posix_getuid,	NULL},
	{"posix_getgid",		php3_posix_getgid,	NULL},
	{"posix_geteuid",		php3_posix_geteuid,	NULL},
	{"posix_getegid",		php3_posix_getegid,	NULL},
	{"posix_getgroups",		php3_posix_getgroups,	NULL},

	{"posix_getrlimit",		php3_posix_getrlimit,	NULL},
	{"posix_uname",			php3_posix_uname,	NULL},

	{NULL, NULL, NULL}
};

php3_module_entry posix_module_entry = {
	"Posix", 
	posix_functions, 
	php3_minit_posix, 
	php3_mshutdown_posix, 
	NULL,
	NULL, 
	php3_info_posix, 
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &posix__module_entry; }
#endif

#if APACHE
extern void timeout(int sig);
#endif

int php3_minit_posix(INIT_FUNC_ARGS)
{
	return SUCCESS;
}


int php3_mshutdown_posix(void){
	return SUCCESS;
}


void php3_info_posix(void)
{
    php3_printf("$ Revision: $\n");
}


/* {{{ proto long posix_getpid(void) 
   Get the current process id */
void php3_posix_getpid(INTERNAL_FUNCTION_PARAMETERS)
{
	pid_t  pid;

	pid = getpid();
    return_value->type= IS_LONG;
	return_value->value.lval = pid;
}
/* }}} */

/* {{{ proto long posix_getppid(void) 
   Get the parent process id */
void php3_posix_getppid(INTERNAL_FUNCTION_PARAMETERS)
{
	pid_t  ppid;

	ppid = getppid();
    return_value->type= IS_LONG;
	return_value->value.lval = ppid;
}
/* }}} */

/* {{{ proto long posix_getpgrp(void) 
   Get the current process group */
void php3_posix_getpgrp(INTERNAL_FUNCTION_PARAMETERS)
{
	pid_t  pgrp;

	pgrp = getpgrp();
    return_value->type= IS_LONG;
	return_value->value.lval = pgrp;
}
/* }}} */

/* {{{ proto long posix_getpgid(void) 
   Get the process group id of the specified process */
void php3_posix_getpgid(INTERNAL_FUNCTION_PARAMETERS)
{
#if HAVE_GETPGID
	pid_t  pgid;
	pval  *pid;

	if (getParameters(ht, 1, &pid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(pid);
	pgid = getpgid(pid->value.lval);
	if (pgid < 0) {
		php3_error(E_WARNING, "posix_getpgid(%d) failed with '%s'", 
			pid->value.lval,
			strerror(errno));
		RETURN_FALSE;
	}

	return_value->type= IS_LONG;
	return_value->value.lval = pgid;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto long posix_getsid(void) 
   get process group ID of session leader */
void php3_posix_getsid(INTERNAL_FUNCTION_PARAMETERS)
{
#if HAVE_GETSID
	pid_t  sid;
	pval  *pid;

	if (getParameters(ht, 1, &pid)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(pid);
	sid = getsid(pid->value.lval);
	if (sid < 0) {
		php3_error(E_WARNING, "posix_getsid(%d) failed with '%s'", 
			pid->value.lval,
			strerror(errno));
		RETURN_FALSE;
	}

	return_value->type= IS_LONG;
	return_value->value.lval = sid;
#else
	RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto long posix_getuid(void) 
   Get the current user id */
void php3_posix_getuid(INTERNAL_FUNCTION_PARAMETERS)
{
	uid_t  uid;

	uid = getuid();
	return_value->type= IS_LONG;
	return_value->value.lval = uid;
}
/* }}} */

/* {{{ proto long posix_getgid(void) 
   Get the current group id */
void php3_posix_getgid(INTERNAL_FUNCTION_PARAMETERS)
{
	gid_t  gid;

	gid = getgid();
	return_value->type= IS_LONG;
	return_value->value.lval = gid;
}
/* }}} */

/* {{{ proto long posix_geteuid(void) 
   Get the current effective user id */
void php3_posix_geteuid(INTERNAL_FUNCTION_PARAMETERS)
{
	uid_t  uid;

	uid = geteuid();
	return_value->type= IS_LONG;
	return_value->value.lval = uid;
}
/* }}} */

/* {{{ proto long posix_getegid(void) 
   Get the current effective group id */
void php3_posix_getegid(INTERNAL_FUNCTION_PARAMETERS)
{
	gid_t  gid;

	gid = getegid();
	return_value->type= IS_LONG;
	return_value->value.lval = gid;
}
/* }}} */

/* {{{ proto long posix_getgroups(void) 
   get supplementary group access list IDs */
void php3_posix_getgroups(INTERNAL_FUNCTION_PARAMETERS)
{
	gid_t  gidlist[NGROUPS_MAX];
	int    result;
	int    i;

	result = getgroups(NGROUPS_MAX, gidlist);
	if (result < 0) {
		php3_error(E_WARNING, "posix_getgroups() failed with '%s'",
			strerror(errno));
		RETURN_FALSE;
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	for (i=0; i<result; i++) {
		add_next_index_long(return_value,gidlist[i]);
	}
}
/* }}} */

static int posix_addlimit(int limit, char *name, pval *return_value) {
	int result;
	struct rlimit rl;
	char hard[80];
	char soft[80];

	snprintf(hard, 80, "hard %s", name);
	snprintf(soft, 80, "soft %s", name);

	result = getrlimit(limit, &rl);
	if (result < 0) {
		php3_error(E_WARNING, "posix_getrlimit failed to getrlimit(RLIMIT_CORE with '%s'", strerror(errno));
		return FAILURE;
	}

	if (rl.rlim_cur == RLIM_INFINITY)
		add_assoc_string(return_value,soft,"unlimited", 9);
	else
		add_assoc_long(return_value,soft,rl.rlim_cur);

	if (rl.rlim_max == RLIM_INFINITY)
		add_assoc_string(return_value,hard,"unlimited", 9);
	else
		add_assoc_long(return_value,hard,rl.rlim_max);

	return SUCCESS;
}

struct limitlist {
	int limit;
	char *name;
} limits[] = {
#ifdef RLIMIT_CORE
	{ RLIMIT_CORE,	"core" },
#endif

#ifdef RLIMIT_DATA
	{ RLIMIT_DATA,	"data" },
#endif

#ifdef RLIMIT_STACK
	{ RLIMIT_STACK,	"stack" },
#endif

#ifdef RLIMIT_VMEM
	{ RLIMIT_VMEM, "virtualmem" },
#endif

#ifdef RLIMIT_AS
	{ RLIMIT_AS, "totalmem" },
#endif

#ifdef RLIMIT_RSS
	{ RLIMIT_RSS, "rss" },
#endif

#ifdef RLIMIT_NPROC
	{ RLIMIT_NPROC, "maxproc" },
#endif

#ifdef RLIMIT_MEMLOCK
	{ RLIMIT_MEMLOCK, "memlock" },
#endif

#ifdef RLIMIT_CPU
	{ RLIMIT_CPU,	"cpu" },
#endif

#ifdef RLIMIT_FSIZE
	{ RLIMIT_FSIZE, "filesize" },
#endif

#ifdef RLIMIT_NOFILE
	{ RLIMIT_NOFILE, "openfiles" },
#endif

#ifdef RLIMIT_OFILE
	{ RLIMIT_OFILE, "openfiles" },
#endif

	{ 0, NULL }
};

/* {{{ proto long posix_getrlimit(void) 
   get system resource consumption limits */

void php3_posix_getrlimit(INTERNAL_FUNCTION_PARAMETERS)
{
	struct limitlist *l = NULL;

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	for (l=limits; l->name; l++) {
		if (posix_addlimit(l->limit, l->name, return_value) == FAILURE)
			RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto long posix_uname(void) 
   get additional information about the current operating system */

void php3_posix_uname(INTERNAL_FUNCTION_PARAMETERS)
{
	struct utsname u;

	uname(&u);

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	add_assoc_string(return_value, "sysname",  u.sysname,  strlen(u.sysname));
	add_assoc_string(return_value, "nodename", u.nodename, strlen(u.nodename));
    add_assoc_string(return_value, "release",  u.release,  strlen(u.release));
    add_assoc_string(return_value, "version",  u.version,  strlen(u.version));
    add_assoc_string(return_value, "machine",  u.machine,  strlen(u.machine));
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
