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
   | Authors: Jim Winstead (jimw@adventure.com)                           |
   +----------------------------------------------------------------------+
 */

/* $Id: log.c,v 1.12 1999/06/16 11:34:15 ssb Exp $ */

#include "phpdl.h"
#include "log.h"

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <time.h>
#include <errno.h>

#include "functions/reg.h"
#include "functions/pageinfo.h"
#include "log.h"

logging_stats log_stats;
logging_conf log_conf;


function_entry log_functions[] = {
	/* logging support */
	{"getlastaccess", php3_getlastaccess, NULL},
	{"getlastbrowser", php3_getlastbrowser, NULL},
	{"getlastemail", php3_getlastemail, NULL},
	{"getlasthost", php3_getlasthost, NULL},
	{"getlastref", php3_getlastref, NULL},
	{"getlogdir", php3_getlogdir, NULL},
	{"getloghost", php3_getloghost, NULL},
	{"getstartlogging", php3_getstartlogging, NULL},
	{"gettoday", php3_gettoday, NULL},
	{"gettotal", php3_gettotal, NULL},
	{"logas", php3_logas, NULL},
	{NULL,NULL,NULL}
};

DLEXPORT int dl_initialize(void *handle)
{
	if (register_functions(log_functions,IS_INTERNAL_FUNCTION) == FAILURE) {
		return FAILURE;
	}
	return SUCCESS;
}

DLEXPORT int php3_init_log(INIT_FUNC_ARGS) {
	log_stats.filled = 0;
	log_stats.logfile = NULL;

#if LOG_MSQL || LOG_MYSQL
	/* FIXME: This is broken. Fix it when someone makes SQL logging go. */
	log_conf.sql_log_db = cfg_get_string("log.sql_db");
	log_conf.sql_log_host = cfg_get_string("log.sql_host");
#else
	cfg_get_string("log.dbm_dir", &(log_conf.dbm_log_dir));
#endif

	return SUCCESS;
}

static void _php3_flushlogstats(void) {
	if (log_stats.filled) {
		if (log_stats.last_host) efree(log_stats.last_host);
		if (log_stats.last_email) efree(log_stats.last_email);
		if (log_stats.last_ref) efree(log_stats.last_ref);
		if (log_stats.last_browser) efree(log_stats.last_browser);
	}
	log_stats.filled = 0;
}

DLEXPORT int php3_shutdown_log(void)
{
	_php3_flushlogstats();
	if (log_stats.logfile) {
		efree(log_stats.logfile);
		log_stats.logfile = NULL;
	}

#if LOG_MSQL || LOG_MYSQL
	efree(log_conf.sql_log_db);
	efree(log_conf.sql_log_host);
#else
	efree(log_conf.dbm_log_dir);
#endif

	return SUCCESS;
}

char *_php3_filename_to_logfn(char *filename) {
	char *lfn, *lp, *ret;

	lfn = estrdup(filename);
	lp = lfn;
	while(*lp == '/') lp++;
	if (*lp=='~') {
		while(*lp =='~') lp++;
		if (strchr(lp,'/')) {
			while(*lp!='/') lp++;
			lp++;
		}
	}
	ret = _php3_regreplace("/","_",lp, 0, 0);
	efree(lfn);
	return(ret);
}

static void _php3_log_setfilename(void) {
	/* construct the filename for the log */
	log_stats.logfile = _php3_filename_to_logfn(php_env.filename);
}


DLEXPORT void _php3_log(void) {
	/* construct the logfile name if it wasn't set by logas()
	   or because we already read the log once) */
	if (!log_stats.logfile)
		_php3_log_setfilename();

#if LOG_MYSQL
	_php3_log_mysql();
#endif
#if LOG_MSQL
	_php3_log_msql();
#endif
#if LOG_DBM
	_php3_log_db();
#endif
}

DLEXPORT void php3_getlogfile(INTERNAL_FUNCTION_PARAMETERS) {
	/* construct the logfile name if it wasn't set by logas()
	   or because we already read the log once) */
	if (!log_stats.logfile)
		_php3_log_setfilename();
	RETURN_STRING(log_stats.logfile);
}

DLEXPORT void php3_logas(INTERNAL_FUNCTION_PARAMETERS) {
	pval *as;
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &as) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(as);
	if (log_stats.logfile) efree(log_stats.logfile);
	log_stats.logfile = estrdup(as->value.str.val);
	_php3_flushlogstats();
	RETURN_STRING(as->value.str.val);
}

DLEXPORT void _php3_load_log_info(void) {
	/* construct the logfile name if it wasn't set by logas()
	   or because we already read the log once) */
	if (!log_stats.logfile)
		_php3_log_setfilename();
#if LOG_DBM
	_php3_load_log_info_db();
#endif
}

#define LOAD_LAST_LOG_INFO \
	if (!log_stats.filled) \
		_php3_load_log_info();

DLEXPORT void php3_getlasthost(INTERNAL_FUNCTION_PARAMETERS) {
	LOAD_LAST_LOG_INFO;
	if (log_stats.last_host) 
		RETURN_STRING(log_stats.last_host);
}

DLEXPORT void php3_getlastref(INTERNAL_FUNCTION_PARAMETERS) {
	LOAD_LAST_LOG_INFO;
	if (log_stats.last_ref) 
		RETURN_STRING(log_stats.last_ref);
}

DLEXPORT void php3_getlastemail(INTERNAL_FUNCTION_PARAMETERS) {
	LOAD_LAST_LOG_INFO;
	if (log_stats.last_email) 
		RETURN_STRING(log_stats.last_email);
}

DLEXPORT void php3_getlastbrowser(INTERNAL_FUNCTION_PARAMETERS) {
	LOAD_LAST_LOG_INFO;
	if (log_stats.last_browser) 
		RETURN_STRING(log_stats.last_browser);
}

DLEXPORT void php3_getlastaccess(INTERNAL_FUNCTION_PARAMETERS) {
	LOAD_LAST_LOG_INFO;
	RETURN_LONG(log_stats.last_access);
}

DLEXPORT void php3_getstartlogging(INTERNAL_FUNCTION_PARAMETERS) {
	LOAD_LAST_LOG_INFO;
	RETURN_LONG(log_stats.start_logging);
}

DLEXPORT void php3_gettotal(INTERNAL_FUNCTION_PARAMETERS) {
	LOAD_LAST_LOG_INFO;
	RETURN_LONG(log_stats.total_count);
}

DLEXPORT void php3_gettoday(INTERNAL_FUNCTION_PARAMETERS) {
	LOAD_LAST_LOG_INFO;
	RETURN_LONG(log_stats.today_count);
}

DLEXPORT static char *_php3_getlogdir(void) {
#if LOG_MSQL || LOG_MYSQL
	return log_conf.sql_log_db;
#else
#if LOG_DBM
	return log_conf.dbm_log_dir;
#else
	return NULL;
#endif
#endif
}

DLEXPORT void php3_getlogdir(INTERNAL_FUNCTION_PARAMETERS) {
	char *ret = _php3_getlogdir();
	RETURN_STRING(ret);
}

DLEXPORT static char *_php3_getloghost(void) {
#if LOG_MSQL || LOG_MYSQL 
	return log_conf.sql_log_host;
#else
	return NULL;
#endif
}

DLEXPORT void php3_getloghost(INTERNAL_FUNCTION_PARAMETERS) {
	char *ret = _php3_getloghost();
	RETURN_STRING(ret);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
