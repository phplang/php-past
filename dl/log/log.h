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
   | Authors:                                                             |
   +----------------------------------------------------------------------+
 */

/* $Id: log.h,v 1.2 1997/12/31 15:56:04 rasmus Exp $ */

#ifndef _PHP_LOG_H
#define _PHP_LOG_H

/* this should be set in ../config.h eventually */
#define LOG_DIR "."
#define LOG_DB 1

/* when any of the page statistics are asked for, we load them
   all at once and stash them in this struct. */
typedef struct {
	char *logfile; /* either the filename converted to a logging name
			  or a user-specified "filename" from logas().
			  this is used as the actual filename in db
			  logging, as some field when sql logging */

	int filled;

	long total_count;
	long today_count;
	long last_access;
	long start_logging;
	char *last_host;
	char *last_email;
	char *last_ref;
	char *last_browser;	
} logging_stats;

extern logging_stats log_stats;

typedef struct {
#if MSQLLOGGING || MYSQLLOGGING
	char *sql_log_db;
	char *sql_log_host;
#else
	char *dbm_log_dir;
#endif
} logging_conf;

extern logging_conf log_conf;

extern void _php3_log(void);
extern char *_php3_filename_to_logfn(char *filename);
extern void _php3_load_log_info(void);

extern void _php3_log_db(void);
extern void _php3_load_log_info_db(void);

/* logging functions */
extern int php3_init_log(INITFUNCARG);
extern int php3_shutdown_log(void);
extern void php3_getlastaccess(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getlastbrowser(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getlastemail(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getlasthost(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getlastref(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getlogdir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getlogfile(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getloghost(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getstartlogging(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_gettoday(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_gettotal(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_logas(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _PHP_LOG_H */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
