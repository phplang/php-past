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
   | Authors:                                                             |
   +----------------------------------------------------------------------+
 */

/* $Id: log_db.c,v 1.6 1999/06/16 11:34:15 ssb Exp $ */

#include "phpdl.h"
#include "log.h"

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <sys/stat.h>

#include <time.h>
#include <errno.h>

#include "functions/pageinfo.h"
#include "functions/db.h"
#include "log.h"

void _php3_log_db() {

#if LOGGING
	char path[1024];
	char key[32];
	char buf[512];
	char *host = NULL;
	char *email, *ref, *browser;
	dbm_info *dbm;

#if NOLOGSUCCESSIVE
	/* this prevents logging multiple consecutive hits from the
	   same host */
	char *lasthost = NULL;
#endif

	struct stat sb;
	time_t t;
	int try = 0, retries = 0;
	int ret, es;
	char *lref = NULL, *s;

	/* verify that the logging directory exists */
	if (!log_stats.filled)
		_php3_load_log_info();

	/* SAFE_MODE concern: should make sure that the user is
	   logging somewhere safe */
	if (stat(log_conf.dbm_log_dir, &sb) == -1) {
		if (mkdir(log_conf.dbm_log_dir, 0755)==-1) {
			php3_error(E_WARNING, "Trying to create main log directory [%s]: %d [%s]",log_conf.dbm_log_dir,errno,strerror(errno));
			return;
		}
	}	

	/* see if the log directory exists for this page-owner's uid */
	sprintf(path, "%s/%ld", log_conf.dbm_log_dir, _php3_getuid());
	if (stat(path, &sb) == -1) {
		if (mkdir(path, 0755)==-1) {
			php3_error(E_WARNING, "Trying to create user log directory [%s]: %d [%s]",path,errno,strerror(errno));
			return;
		}
		log_stats.start_logging = time(NULL);
	} 

	/* construct the full path for the database */
        sprintf(path, "%s/%ld/%s.log", log_conf.dbm_log_dir, _php3_getuid(), log_stats.logfile);

	es = error_reporting;
	error_reporting = 0;
	dbm = _php3_dbmopen(path, "w");
	error_reporting = es;

	if (!dbm) {
		dbm = _php3_dbmopen(path, "n");
		if (!dbm) {
			php3_error(E_WARNING, "Unable to create %s", path);
			return;
		}
		log_stats.total_count = 0;
		log_stats.today_count = 0;

		t = time(NULL);
		sprintf(key,"%ld",(long)t);

		/* retry a few times for some reason */
		while(!_php3_dbminsert(dbm, "first", key)) {
			retries++;
			if (retries > 20) {
				retries = 0;
				break;
			}
		}
	}

	host = php_env.remote_host;
#if NOLOGSUCCESSIVE
	lasthost = getlasthost();
	if (strncmp(host, lasthost, 128)==0) {
		_php3_dbmclose(dbm);
		return;
	}
#endif
	email = php_env.email;
	ref = php_env.referrer;
	if (ref) {
		lref = estrdup(ref);
		s = strchr(lref,'&');
		if (s) *s='\0';
		if (strlen(lref)>128) lref[127]='\0';	
	}
	browser = php_env.useragent;	

	sprintf(buf,"%s%c%s%c%s%c%s",host?host:"",27,email?email:"",27,lref?lref:"",27,browser?browser:"");
	t = time(NULL);
	sprintf(key,"%ld",(long)t);
	ret=1;
	while(ret) {
		ret = _php3_dbminsert(dbm, key, buf);
		if (ret) {
			sprintf(key,"%ld%c",(long)t,' '+try++);
		}		
		retries++;
		if (retries>20) break;
	}
	log_stats.total_count++;
	log_stats.today_count++;
	sprintf(buf,"%s %ld %ld %ld %s%c%s%c%s%c%s",
		php_env.filename,
		(long)t,
		log_stats.total_count,
		log_stats.today_count,
		host ? host : "" ,
		27,
		email ? email : "",
		27,
		lref ? lref : "",
		27,
		browser ? browser : "");
	_php3_dbmreplace(dbm, "last", buf);
	_php3_dbmclose(dbm);
#endif
}

void _php3_load_log_info_db() {
#if LOGGING
	dbm_info *dbm;
	char *value;
	char *s=NULL, *ss;
	char path[1024];
	char key[32];
	int ret, es;
	time_t t;
	struct tm *tm1;
	int day1, day2;
	int retries=0;

	/* construct the full path for the database */
        sprintf(path, "%s/%ld/%s.log", log_conf.dbm_log_dir, _php3_getuid(), log_stats.logfile);

	es = error_reporting;
	error_reporting = 0;
	dbm = _php3_dbmopen(path, "r");
	error_reporting = es;

  	if (!dbm) {
		log_stats.filled = 1;
		log_stats.total_count = 0;
		log_stats.today_count = 0;
		log_stats.last_access = 0;
		log_stats.last_host = estrdup("none");
		log_stats.last_ref = estrdup("none");
		log_stats.last_browser = estrdup("none");
		log_stats.last_email = estrdup("none");
		log_stats.start_logging = time(NULL);
		return;
	}

	value = s = _php3_dbmfetch(dbm, "last");

	ss = (char *)strchr(s,' ');

	if (ss) {
		s = ss+1;
		ss=(char *)strchr(s,' ');
	}
	if (ss) {
		*ss='\0';
		log_stats.last_access = atol(s);
		s = ss+1;
		ss = (char *)strchr(s,' ');
		if (ss) {
			*ss='\0';
			log_stats.total_count = atol(s);
			s = ss+1;
			ss = (char *)strchr(s,' ');
			if (ss) {
				*ss='\0';
				log_stats.today_count = atol(s);	
				t = time(NULL);
				tm1 = localtime((time_t *)(&log_stats.last_access));
				day1 = tm1->tm_yday;
				tm1 = localtime(&t);
				day2 = tm1->tm_yday;
				if (day1 != day2) log_stats.today_count = 0;	
				s = ss+1;
				ss = (char *)strchr(s,27);
				if (ss) {
					*ss='\0';
					log_stats.last_host = (char *)estrdup(s);
					s = ss+1;
					ss = (char *)strchr(s,27);
					if (ss) {
						*ss='\0';
						if (*s) log_stats.last_email = (char *)estrdup(s);
						s = ss+1;
						ss = (char *)strchr(s,27);
						if (ss) {
							*ss='\0';
							if (*s) log_stats.last_ref = (char *)estrdup(s);
							s = ss+1;
							log_stats.last_browser = (char *)estrdup(s);
						}
					}
				}
			}
		}
	}
	if (value) efree(value);

	value = s = _php3_dbmfetch(dbm, "first");
	if (!s) {
		/* If there is no first entry in the dbm file, add one now.
		   This obviously won't be the correct first entry date, but
		   it is better than not having one at all. */
		t = time(NULL);
		sprintf(key,"%ld",(long)t);
		ret = 1;
		while (ret) {
			ret = _php3_dbminsert(dbm,"first",key);
			retries++;
			if (retries>20) {
				retries = 0;
				break;
			}
		}
		log_stats.start_logging = t;
	} else {
		log_stats.start_logging = atol(s);	
	}
	if (value) efree(value);

	log_stats.filled = 1;
	_php3_dbmclose(dbm);	
#endif
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
