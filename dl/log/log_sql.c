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

/* $Id: log_sql.c,v 1.6 1999/06/16 11:34:15 ssb Exp $ */

#include "phpdl.h"

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_SYS_FILE_H
# ifndef HAVE_LOCKF
#  ifdef HAVE_FLOCK
#   include <sys/file.h>
#  endif
# endif
#endif

#include <time.h>
#include <errno.h>

#if HAVE_MSQL
#include <msql.h>
#endif

#if HAVE_MYSQL
#include <mysql.h>
#endif

#include "functions/reg.h"
#include "functions/pageinfo.h"
#include "log.h"

void SQLLog(char *filename) {
#if defined(MSQLLOGGING) || defined(MYSQLLOGGING)
	char query[512];
	long uid;
#if MSQLLOGGING
	int dbsock, lockfd;
	m_result *result;
	m_row record;
	char lockfn[64];
	char *pfn;
#endif
#if MYSQLLOGGING
	MYSQL *dbsock,*mysql;
	MYSQL_RES *result=NULL;
	MYSQL_ROW record;
#endif
	time_t t, la;	
	struct tm *tm1;
	int day1, day2;
	char *s,*fn,*host,*email,*lref,*ref,*browser;
#if NOLOGSUCCESSIVE
	char *lasthost;
#endif
	char hs=0,es=0,ls=0,bs=0;

#if DEBUG
	Debug("SQLLog called for file: [%s]\n",filename?filename:"null");
#endif

#if MSQLLOGGING 
	dbsock = msqlGetDbSock();
	if (dbsock==-1) {
#endif
#if MYSQLLOGGING
	dbsock = mysqlGetDbSock();
	mysql = mysqlGetPtr();
	if (dbsock==NULL) {
#endif
		if (SQLLogHost && strcasecmp(SQLLogHost,"localhost")) 
#if MSQLLOGGING
			dbsock = msqlConnect(SQLLogHost);
#endif
#if MYSQLLOGGING
			dbsock = mysql_connect(mysql,SQLLogHost,0,0);
#endif
		else
#if MSQLLOGGING
			dbsock = msqlConnect(NULL);
#endif
#if MYSQLLOGGING
			dbsock = mysql_connect(mysql,"localhost",0,0);
#endif
#if MSQLLOGGING
		if (dbsock<0) {
#endif
#if MYSQLLOGGING
		if (dbsock==NULL) {
#endif
			php3_error(E_WARNING, "Unable to connect to sql daemon");
			return;
		}
	}
#if MSQLLOGGING
	if (msqlSelectDB(dbsock,SQLLogDB)<0) {
		msqlClose(dbsock);
		php3_error(E_WARNING, "Unable to select msql logging database (%s) - %s",SQLLogDB,msqlErrMsg);
		return;
	}
	msqlSetCurrent(dbsock,SQLLogDB);
#endif
#if MYSQLLOGGING
	if (mysql_select_db(dbsock,SQLLogDB)<0) {
		mysql_close(dbsock);
		php3_error(E_WARNING, "Unable to select mysql logging database (%s)",SQLLogDB);
		return;
	}
	mysqlSetCurrent(dbsock,SQLLogDB);
#endif

	uid = getmyuid();
	t = time(NULL);

	host = getremotehostname();	
#if NOLOGSUCCESSIVE
	lasthost = getlasthost();
	if (strncmp(host?host:"",lasthost?lasthost:"",128)==0) {
		return;
	}
#endif
	email = getemailaddr();
	ref = getrefdoc();
	lref=NULL;
	if (ref) {
		lref = estrdup(1,ref);
		s = strchr(lref,'&');
		if (s) *s='\0';
		if (strlen(lref)>128) lref[127]='\0';	
	}
	browser = getbrowser();	
	if (forcelogfile) { 
		fn = forcelogfile; 
	}
	else if (!filename || (filename && strlen(filename)<1)) fn = GetCurrentFilename();
	else fn = filename;

	if (fn && strlen(fn)>63) {
		fn += strlen(fn)-63;
	}
	if (host && strlen(host)>63) {
		hs = host[63];
		host[63]='\0';	
	}	
	if (email && strlen(email)>63) {
		es = email[63];
		email[63] = '\0';
	}
	if (lref && strlen(lref)>63) {
		ls = lref[63];
		lref[63]='\0';
	}
	if (browser && strlen(browser)>63) {
		bs = browser[63];
		browser[63]='\0';
	}
	sprintf(query,"insert into log%ld values (%ld,'%s','%s','%s','%s','%s')",
		uid,t,fn,host?host:"",email?email:"",lref?lref:"",browser?browser:"");
#if DEBUG
	Debug("Sending query: %s\n",query);
#endif	
#if MSQLLOGGING
	if (msqlQuery(dbsock,query)<0) {
#endif
#if MYSQLLOGGING
	if (mysql_query(dbsock,query)<0) {
#if DEBUG
		Debug("Query failed! (%s)\n",mysql_error(mysql));
#endif
#endif
	}

	/* Atomically read and increment total and today counters */		
#if MSQLLOGGING
	sprintf(query,"select total,today,timestamp from last%ld where filename='%s'",uid,fn);
	pfn = filename_to_logfn(filename);
	sprintf(lockfn,"%s/%s%ld.lck",SQLLOGTMP,pfn,uid);
	/* Lock database */
	lockfd = open(lockfn,O_RDWR|O_CREAT,0644);
	if (lockfd) {
#ifdef HAVE_LOCKF
		lockf(lockfd,F_LOCK,0);
#else
#ifdef HAVE_FLOCK
		flock(lockfd,LOCK_EX);
#endif
#endif
	} 
#if DEBUG
	else {
		Debug("Lock failed\n");
	}
#endif

#if DEBUG
	Debug("Sending query: %s\n",query);
#endif
	if (msqlQuery(dbsock,query)<0) {
#if DEBUG
		Debug("Query failed! (%s)\n",msqlErrMsg);
#endif
	} else {
		result = msqlStoreResult();
		if (result && msqlNumRows(result)>0) {
			msqlDataSeek(result,0);
			record = msqlFetchRow(result);
			if (record) {
				total_count = atol(record[0]);
				today_count = atol(record[1]);
				la = atol(record[2]);
			} else {
				total_count = 0;
				today_count = 0;
				la = 0;
			}
			msqlFreeResult(result);
		} else {
			total_count = 0;
			today_count = 0;
			la = 0;
			sprintf(query,"insert into last%ld values ('%s',%ld,0,0,%ld,'%s','%s','%s','%s')",
				uid,fn,t,t,host?host:"",email?email:"",lref?lref:"",browser?browser:"");
#if DEBUG
			Debug("Sending query: %s\n",query);
#endif
			if (msqlQuery(dbsock,query)<0) {
#if DEBUG
				Debug("Query failed! (%s)\n",msqlErrMsg);
#endif
			}
		}
	}
#endif
#if MYSQLLOGGING
	sprintf(query,"select total,today,timestmp from last%ld where filename='%s'",uid,fn);
#if DEBUG
        Debug("Sending query: %s\n",query);
#endif
	if (mysql_query(dbsock,query)<0){
#if DEBUG
                Debug("Query failed! (%s)\n",mysql_error(mysql));
#endif
	} else {
		result = mysql_store_result(dbsock);
		if (result && mysql_num_rows(result)>0){
			mysql_data_seek(result,0);
			record = mysql_fetch_row(result);
			if (record) {
				total_count = atol(record[0]);
                                today_count = atol(record[1]);
                                la = atol(record[2]);
                        } else {
                                total_count = 0;
                                today_count = 0;
                                la = 0;
                        }
			mysql_free_result(result);
		} else {
			total_count = 0;
                        today_count = 0;
                        la = 0;
                        sprintf(query,"insert into last%ld values ('%s',%ld,0,0,%ld,'%s','%s','%s','%s')",
                                uid,fn,t,t,host?host:"",email?email:"",lref?lref:"",browser?browser:"");
#if DEBUG
                        Debug("Sending query: %s\n",query);
#endif
			if (mysql_query(dbsock,query)<0) {
#if DEBUG
                Debug("Query failed! (%s)\n",mysql_error(mysql));
#endif
			}
		}
	}
#endif
	tm1 = localtime((time_t *)(&la));
	day1 = tm1->tm_yday;
	tm1 = localtime(&t);
	day2 = tm1->tm_yday;
	if (day1 != day2) today_count = 0;	
#if MSQLLOGGING
	total_count++;
	today_count++;
	sprintf(query,
		"update last%ld set timestamp=%ld,total=%ld,today=%ld,host='%s',email='%s',lref='%s',browser='%s' where filename='%s'",
		uid,t,total_count,today_count,host?host:"",email?email:"",lref?lref:"",browser?browser:"",fn);
#endif
#if MYSQLLOGGING
	total_count++;
        today_count++;
	sprintf(query,
		"update last%ld set timestmp=%ld,total=%ld,today=%ld,host='%s',email='%s',lref='%s',browser='%s' where filename='%s'",
		uid,t,total_count,today_count,host?host:"",email?email:"",lref?lref:"",browser?browser:"",fn);
	
#endif
#if DEBUG
	Debug("Sending query: %s\n",query);
#endif
#if MSQLLOGGING
	if (msqlQuery(dbsock,query)<0) {
#endif
#if MYSQLLOGGING
	if (mysql_query(dbsock,query)<0) {
#if DEBUG
	Debug("Sending query: %s\n",query);
#endif 
		
#endif
	}
#ifdef HAVE_LOCKF
#if MSQLLOGGING
	lockf(lockfd,F_ULOCK,0);
	close(lockfd);
#endif
#else
#ifdef HAVE_FLOCK
#if MSQLLOGGING
	flock(lockfd,LOCK_UN);
	close(lockfd);
#endif
#endif
#endif
#if MSQLLOGGING
	unlink(lockfn);
#endif
	if (hs) host[63]=hs;
	if (es) email[63]=es;
	if (ls) lref[63]=ls;
	if (bs) browser[63]=bs;
#endif
}

void sqlloadlastinfo(char *filename) {
#if defined(MSQLLOGGING) || defined(MYSQLLOGGING)
	char *lfn;
	char query[128];
	long uid, la;
#if MSQLLOGGING
	int dbsock;
	m_result *result;
	m_row record;
#endif
#if MYSQLLOGGING
	MYSQL *dbsock,*mysql;
	MYSQL_RES *result=NULL;
	MYSQL_ROW record;
#endif
	time_t t;	
	struct tm *tm1;
	int day1, day2;

	uid = getmyuid();
	if (!filename) {
#if APACHE
		filename = php_rqst->uri;
#else
		filename = getenv("PATH_INFO");
#endif
		if (!filename || (filename && strlen(filename)<1)) lfn=GetCurrentFilename();
		else lfn = filename;
	} else {
		if (!filename || (filename && strlen(filename)<1)) lfn=GetCurrentFilename();
		else lfn = filename;
	}

#if MSQLLOGGING
	dbsock = msqlGetDbSock();
	if (dbsock==-1) {
#endif
#if MYSQLLOGGING
	dbsock = mysqlGetDbSock();
	mysql = mysqlGetPtr();
	if (dbsock==NULL) {
#endif
		if (SQLLogHost && strcasecmp(SQLLogHost,"localhost")) 
#if MSQLLOGGING
			dbsock = msqlConnect(SQLLogHost);
#endif
#if MYSQLLOGGING
			dbsock = mysql_connect(mysql,SQLLogHost,0,0);
#endif
		else
#if MSQLLOGGING
			dbsock = msqlConnect(NULL);
#endif
#if MYSQLLOGGING
			dbsock = mysql_connect(mysql,"localhost",0,0);
#endif
#if MSQLLOGGING
		if (dbsock<0) {
#endif
#if MYSQLLOGGING
		if (dbsock==NULL) {
#endif
			php3_error(E_WARNING, "Unable to connect to sql daemon");
			return;
		}
	}
#if MSQLLOGGING
	if (msqlSelectDB(dbsock,SQLLogDB)<0) {
		msqlClose(dbsock);
#endif
#if MYSQLLOGGING
	if (mysql_select_db(dbsock,SQLLogDB)<0) {
		mysql_close(dbsock);
#endif
		php3_error(E_WARNING, "Unable to select sql logging database (%s)",SQLLogDB);
		return;
	}
#if MSQLLOGGING
	msqlSetCurrent(dbsock,SQLLogDB);
#endif
#if MYSQLLOGGING
	mysqlSetCurrent(dbsock,SQLLogDB);
#endif
	
	sprintf(query,"select * from last%ld where filename='%s'",uid,lfn);
#if DEBUG
	Debug("sqlloadlastinfo: Sending query: %s\n",query);
#endif
#if MSQLLOGGING
	if (msqlQuery(dbsock,query)<0) {
#endif
#if MYSQLLOGGING
	if (mysql_query(dbsock,query)<0) {
#endif
#if DEBUG
#if MSQLLOGGING
		Debug("Query failed! (%s)\n",msqlErrMsg);
#endif
#if MYSQLLOGGING
		Debug("Query failed! (%s)\n",mysql_error(mysql));
#endif
#endif
	} else {
#if MSQLLOGGING
		result = msqlStoreResult();
#endif
#if MYSQLLOGGING
		result = mysql_store_result(dbsock);
#endif
#if MSQLLOGGING
		if (result && msqlNumRows(result)>0) {
			msqlDataSeek(result,0);
			record = msqlFetchRow(result);
#endif
#if MYSQLLOGGING
		if (result && mysql_num_rows(result)>0) {
			mysql_data_seek(result,0);
			record = mysql_fetch_row(result);
#endif
			last_access = atol(record[1]);
			la = last_access;
			total_count = atol(record[2]);	
			today_count = atol(record[3]);	
			t = time(NULL);
			tm1 = localtime((time_t *)(&la));
			day1 = tm1->tm_yday;
			tm1 = localtime(&t);
			day2 = tm1->tm_yday;
			if (day1 != day2) today_count = 0;	
			start_logging = atol(record[4]);
			last_host = estrdup(0,record[5]);
			last_email = estrdup(0,record[6]);
			last_ref = estrdup(0,record[7]);
			last_browser = estrdup(0,record[8]);
#if MSQLLOGGING
			msqlFreeResult(result);
#endif
#if MYSQLLOGGING
			mysql_free_result(result);
#endif
		}
	}	
#endif
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
