/***[log.c]*******************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996 Rasmus Lerdorf                                         *
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
/* $Id: log.c,v 1.21 1996/09/19 04:49:59 rasmus Exp $ */
#include "php.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_FILE_H
#ifndef HAVE_LOCKF
#ifdef HAVE_FLOCK
#include <sys/file.h>
#endif
#endif
#endif
#include <time.h>
#include <errno.h>
#ifdef HAVE_LIBMSQL
#include <msql.h>
#endif
#include "parse.h"

static long total_count=-1;
static long today_count=-1;
static long last_access=-1;
static long last_modified=-1;
static long start_logging=-1;
static char *last_host=NULL;
static char *last_email=NULL;
static char *last_ref=NULL;
static char *last_browser=NULL;
static char *logfile=NULL;
static long MyUid=-1;
static long MyInode=-1;
#if LOGGING
static char lbuf[1024];
static char *dbmLogDir;
#endif
#if MSQLLOGGING
static char *MsqlLogDB;
#endif

#if APACHE
void php_init_log(php_module_conf *conf) {
#else
void php_init_log(void) {
#endif
	total_count=-1;
	today_count=-1;
	last_access=-1;
	last_modified=-1;
	start_logging=-1;
	last_host=NULL;
	last_email=NULL;
	last_ref=NULL;
	last_browser=NULL;
	logfile=NULL;
	MyUid=-1;
	MyInode=-1;
#if LOGGING
	lbuf[0]='\0';
#endif
#if APACHE
#if MSQLLOGGING
	if(conf->MsqlLogDB) MsqlLogDB = conf->MsqlLogDB;
	else MsqlLogDB = estrdup(0,MSQLLOGDB);
#endif
#if LOGGING
	if(conf->dbmLogDir) dbmLogDir = conf->dbmLogDir;
	else dbmLogDir = estrdup(0,LOG_DIR);
#endif
#else
#if MSQLLOGGING
	MsqlLogDB = estrdup(0,MSQLLOGDB);
#endif
#if LOGGING
	dbmLogDir = estrdup(0,LOG_DIR);
#endif
#endif
}

void SetStatInfo(struct stat *sb) {
	last_modified = sb->st_mtime;
	MyUid = sb->st_uid;
	MyInode = sb->st_ino;
}

char *filename_to_logfn(char *filename) {
	char *lfn, *lp, *ret;

	lfn = estrdup(1,filename);
	lp = lfn;
	while(*lp == '/') lp++;
	if(*lp=='~') {
		while(*lp =='~') lp++;
		if(strchr(lp,'/')) {
			while(*lp!='/') lp++;
			lp++;
		}
	}
	lp = _RegReplace("/","_",lp);
	ret = estrdup(1,lp);
	return(ret);
}

void Log(char *filename) {
#if LOGGING
	static char temp[1024];
	char key[32];
	char buf[512];
	char *host,*email,*ref,*browser;
	struct stat sb;
	time_t t;
	int try=0,retries=0;
	int ret, es;
	char *lref=NULL, *s;
	char *lfn;

	if(stat(dbmLogDir,&sb)==-1) {
		if(mkdir(dbmLogDir,0755)==-1) {
			Error("Trying to create main log directory [%s]: %d [%s]",dbmLogDir,errno,strerror(errno));
			return;
		}
	}	
	sprintf(temp,"%s/%ld",dbmLogDir,MyUid);
	if(stat(temp,&sb)==-1) {
		if(mkdir(temp,0755)==-1) {
			Error("Trying to create user log directory [%s]: %d [%s]",temp,errno,strerror(errno));
			return;
		}
		start_logging=time(NULL);
	} 
	lfn = filename_to_logfn(filename);
	sprintf(temp,"%s/%ld/%s.log",dbmLogDir,MyUid,lfn);
	logfile = temp;	
#if DEBUG
	Debug("Trying to open: %s\n",temp);
#endif
    es = ErrorPrintState(0);
    ret = _dbmOpen(temp,"w");
    ErrorPrintState(es);
    if(ret > 0) {
#if DEBUG
		Debug("Creating new dbm logging file\n");
#endif
        ret = _dbmOpen(temp,"n");
		if(ret) {
			Error("Unable to create %s",temp);
			return;
		}
		total_count = 0;
		today_count = 0;

		t = time(NULL);
		sprintf(key,"%ld",(long)t);
		ret=1;
		while(ret) {
			ret = _dbmInsert(temp,"first",key);
			retries++;
			if(retries>20) {
				retries = 0;
				break;
			}
		}
	}

	if(total_count < 0) {
		loadlastinfo(temp,filename);
	}
	host = getremotehostname();	
	email = getemailaddr();
	ref = getrefdoc();
	if(ref) {
		lref = estrdup(1,ref);
		s = strchr(lref,'&');
		if(s) *s='\0';
		if(strlen(lref)>128) lref[127]='\0';	
	}
	browser = getbrowser();	

	sprintf(buf,"%s%c%s%c%s%c%s",host?host:"",27,email?email:"",27,lref?lref:"",27,browser?browser:"");
	t = time(NULL);
	sprintf(key,"%ld",(long)t);
	ret=1;
	while(ret) {
		ret = _dbmInsert(temp,key,buf);
		if(ret) {
			sprintf(key,"%ld%c",(long)t,' '+try++);
		}		
		retries++;
		if(retries>20) break;
	}
	total_count++;
	today_count++;
	sprintf(buf,"%s %ld %ld %ld %s%c%s%c%s%c%s",filename,(long)t,total_count,today_count,host?host:"",27,email?email:"",27,lref?lref:"",27,browser?browser:"");
	_dbmReplace(temp,"last",buf);
	_dbmClose(temp);
#endif
}

void loadlastinfo(char *dbmfile,char *filename) {
#if LOGGING
	char *s=NULL, *ts=NULL, *ss, *lfn;
	char temp[512];
	char key[32];
	int ret, es;
	time_t t;
	struct tm *tm1;
	int day1, day2;
	int free_s=0;
	int retries=0;

	if(!dbmfile) {
		if(lbuf[0]=='\0') {
			if(!filename) {
#if APACHE
				filename = php_rqst->uri;
#else
				filename = getenv("PATH_INFO");
#endif
			}
			lfn = filename_to_logfn(filename);
			sprintf(temp,"%s/%ld/%s.log",dbmLogDir,MyUid,lfn);
			logfile = temp;	
   			es = ErrorPrintState(0);
   			ret = _dbmOpen(temp,"r");
   			ErrorPrintState(es);
   			if(ret > 0) {
				total_count = 0;
				today_count = 0;
				last_access = 0;
				last_host = estrdup(0,"none");
				last_ref = estrdup(0,"none");
				last_browser = estrdup(0,"none");
				last_email = estrdup(0,"none");
				start_logging = time(NULL);
				return;
			}
			s = _dbmFetch(temp,"last");
			strcpy(lbuf,s);
		} else {
			s = (char *)estrdup(1,lbuf);
			ts = s;
			free_s=1;		
		}
	} else {
		if(lbuf[0]=='\0') {
			s = _dbmFetch(dbmfile,"last");
			strcpy(lbuf,s);
		} else {
			s = (char *)estrdup(1,lbuf);
			ts=s;
			free_s=1;
		}
	}
	ss = (char *)strchr(s,' ');
	if(ss) {
		s = ss+1;
		ss=(char *)strchr(s,' ');
	}
	if(ss) {
		*ss='\0';
		last_access = atol(s);
		s = ss+1;
		ss = (char *)strchr(s,' ');
		if(ss) {
			*ss='\0';
			total_count = atol(s);
			s = ss+1;
			ss = (char *)strchr(s,' ');
			if(ss) {
				*ss='\0';
				today_count = atol(s);	
				t = time(NULL);
				tm1 = localtime((time_t *)(&last_access));
				day1 = tm1->tm_yday;
				tm1 = localtime(&t);
				day2 = tm1->tm_yday;
				if(day1 != day2) today_count = 0;	
				s = ss+1;
				ss = (char *)strchr(s,27);
				if(ss) {
					*ss='\0';
					last_host = (char *)estrdup(0,s);
					s = ss+1;
					ss = (char *)strchr(s,27);
					if(ss) {
						*ss='\0';
						if(*s) last_email = (char *)estrdup(0,s);
						s = ss+1;
						ss = (char *)strchr(s,27);
						if(ss) {
							*ss='\0';
							if(*s) last_ref = (char *)estrdup(0,s);
							s = ss+1;
							last_browser = (char *)estrdup(0,s);
						}
					}
				}
			}
		}
	}
	if(start_logging==-1) {
		if(!dbmfile) s = _dbmFetch(temp,"first");
		else s = _dbmFetch(dbmfile,"first");
		if(!s) {
			/* If there is no first entry in the dbm file, add one now.
			   This obviously won't be the correct first entry date, but
			   it is better than not having one at all. */
			t = time(NULL);
			sprintf(key,"%ld",(long)t);
			ret=1;
			while(ret) {
				ret = _dbmInsert(temp,"first",key);
				retries++;
				if(retries>20) {
					retries = 0;
					break;
				}
			}
			start_logging = t;
		} else start_logging = atol(s);	
	}
	if(!dbmfile && !free_s) _dbmClose(temp);	
#endif
}

char *getlastemailaddr(void) {
#if MSQLLOGGING
	if(!last_email) msqlloadlastinfo(NULL);
#else
	if(!last_email) loadlastinfo(NULL,NULL);
#endif
	return(last_email);
}

char *getlastbrowser(void) {
#if MSQLLOGGING
	if(!last_browser) msqlloadlastinfo(NULL);
#else
	if(!last_browser) loadlastinfo(NULL,NULL);
#endif
	return(last_browser);
}

char *getlasthost(void) {
#if MSQLLOGGING
	if(!last_host) msqlloadlastinfo(NULL);
#else
	if(!last_host) loadlastinfo(NULL,NULL);
#endif
	return(last_host);
}

char *getlastref(void) {
#if MSQLLOGGING
	if(!last_ref) msqlloadlastinfo(NULL);
#else
	if(!last_ref) loadlastinfo(NULL,NULL);
#endif
	return(last_ref);
}

char *getlogfile(void) {
	return(logfile);
}

time_t getstartlogging(void) {
#if MSQLLOGGING
	if(start_logging==-1) msqlloadlastinfo(NULL);
#else
	if(start_logging==-1) loadlastinfo(NULL,NULL);
#endif
	return(start_logging);
}

time_t getlastaccess(void) {
#if MSQLLOGGING
	if(last_access==-1) msqlloadlastinfo(NULL);
#else
	if(last_access==-1) loadlastinfo(NULL,NULL);
#endif
	return(last_access);
}

time_t getlastmod(void) {
#if MSQLLOGGING
	if(last_modified==-1) msqlloadlastinfo(NULL);
#else
	if(last_modified==-1) loadlastinfo(NULL,NULL);
#endif
	return(last_modified);
}

int gettotal(void) {
#if MSQLLOGGING
	if(total_count==-1) msqlloadlastinfo(NULL);
#else
	if(total_count==-1) loadlastinfo(NULL,NULL);
#endif
	if(total_count==-1) total_count=0;
	return(total_count);
}

int gettoday(void) {
#if MSQLLOGGING
	if(today_count==-1) msqlloadlastinfo(NULL);
#else
	if(today_count==-1) loadlastinfo(NULL,NULL);
#endif
	if(today_count==-1) today_count=0;
	return(today_count);
}

void GetLogFile(void) {
	if(!logfile) Push("",STRING);
	else Push(logfile,STRING);
}

void GetLastHost(void) {
#if MSQLLOGGING
	if(!last_host) msqlloadlastinfo(NULL);
#else
	if(!last_host) loadlastinfo(NULL,NULL);
#endif
	if(!last_host) Push("",STRING);
	else Push(last_host,STRING);
}

void GetLastRef(void) {
#if MSQLLOGGING
	if(!last_ref) msqlloadlastinfo(NULL);
#else
	if(!last_ref) loadlastinfo(NULL,NULL);
#endif
	if(!last_ref) Push("",STRING);
	else Push(last_ref,STRING);
}

void GetLastEmail(void) {
#if MSQLLOGGING
	if(!last_email) msqlloadlastinfo(NULL);
#else
	if(!last_email) loadlastinfo(NULL,NULL);
#endif
	if(!last_email) Push("",STRING);
	else {
		Push(last_email,STRING);
	}	
}

void GetLastBrowser(void) {
#if MSQLLOGGING
	if(!last_browser) msqlloadlastinfo(NULL);
#else
	if(!last_browser) loadlastinfo(NULL,NULL);
#endif
	if(!last_browser) Push("",STRING);
	else Push(last_browser,STRING);
}

void GetLastAccess(void) {
	char temp[32];

#if MSQLLOGGING
	if(last_access==-1) msqlloadlastinfo(NULL);
#else
	if(last_access==-1) loadlastinfo(NULL,NULL);
#endif
	if(last_access==-1) Push("0",STRING);
	else {
		sprintf(temp,"%ld",last_access);
		Push(temp,STRING);
	}
}

void GetStartLogging(void) {
	char temp[32];

#if MSQLLOGGING
	if(start_logging==-1) msqlloadlastinfo(NULL);
#else
	if(start_logging==-1) loadlastinfo(NULL,NULL);
#endif
	if(start_logging==-1) Push("0",STRING);
	else {
		sprintf(temp,"%ld",start_logging);
		Push(temp,STRING);
	}
}

void GetLastMod(void) {
	char temp[32];

#if MSQLLOGGING
	if(last_modified==-1) msqlloadlastinfo(NULL);
#else
	if(last_modified==-1) loadlastinfo(NULL,NULL);
#endif
	if(last_modified==-1) Push("0",STRING);
	else {
		sprintf(temp,"%ld",last_modified);
		Push(temp,STRING);
	}
}

void GetTotal(void) {
	char temp[32];

#if MSQLLOGGING
	if(total_count==-1) msqlloadlastinfo(NULL);
#else
	if(total_count==-1) loadlastinfo(NULL,NULL);
#endif
	if(total_count==-1) Push("0",STRING);
	else {
		sprintf(temp,"%ld",total_count);
		Push(temp,STRING);
	}
}

void GetToday(void) {
	char temp[32];

#if MSQLLOGGING
	if(today_count==-1) msqlloadlastinfo(NULL);
#else
	if(today_count==-1) loadlastinfo(NULL,NULL);
#endif
	if(today_count==-1) Push("0",STRING);
	else {
		sprintf(temp,"%ld",today_count);
		Push(temp,STRING);
	}
}

char *getlogdir(void) {
#if LOGGING
	if(!dbmLogDir) return(LOG_DIR);
	else return(dbmLogDir);
#endif
#if MSQLLOGGING
	if(!MsqlLogDB) return(MSQLLOGDB);
	else return(MsqlLogDB);
#endif
	return NULL;
}

void GetLogDir(void) {
	Push(getlogdir(),STRING);
}

void GetMyUid(void) {
#ifndef APACHE
	struct stat sb;
#endif
	char *path;
	char temp[32];

	if(MyUid==-1) {
#if APACHE
		path = php_rqst->filename;
#else
		path = getenv("PATH_TRANSLATED");
#endif
		if(path) {
#if APACHE
			MyUid = php_rqst->finfo.st_uid;
#else
			if(stat(path,&sb)==-1) {
				Error("Unable to find: %s",path);
				return;
			}	
			MyUid = sb.st_uid;
#endif
		}
	}
	sprintf(temp,"%ld",MyUid);
	Push(temp,LNUMBER);
}

long getmyuid(void) {
#ifndef APACHE
	struct stat sb;
#endif
	char *path;

	if(MyUid==-1) {
#if APACHE
		path = php_rqst->filename;
#else
		path = getenv("PATH_TRANSLATED");
#endif
		if(path) {
#if APACHE
			MyUid = php_rqst->finfo.st_uid;
			last_modified = php_rqst->finfo.st_mtime;
#else
			if(stat(path,&sb)==-1) {
				Error("Unable to find: %s",path);
				return(-1L);
			}	
			MyUid = sb.st_uid;
			last_modified = sb.st_mtime;
#endif
		}
	} 
	return(MyUid);
}

void GetMyPid(void) {
	char temp[16];
	
	sprintf(temp,"%ld",(long)getpid());
	Push(temp,LNUMBER);
}

void GetMyInode(void) {
#ifndef APACHE
	struct stat sb;
#endif
	char *path;
	char temp[32];

	if(MyInode==-1) {
#if APACHE
		path = php_rqst->filename;
#else
		path = getenv("PATH_TRANSLATED");
#endif
		if(path) {
#if APACHE
			MyInode = php_rqst->finfo.st_ino;
#else
			if(stat(path,&sb)==-1) {
				Error("4. Unable to find: %s",path);
				return;
			}	
			MyInode = sb.st_ino;
#endif
		}
	}
	sprintf(temp,"%ld",MyInode);
	Push(temp,LNUMBER);
}

void MsqlLog(char *filename) {
#if MSQLLOGGING
	char query[512];
	long uid;
	int dbsock, lockfd;
	time_t t, la;	
	struct tm *tm1;
	int day1, day2;
	char *s,*fn, *pfn,*host,*email,*lref,*ref,*browser;
	char lockfn[64];
	m_result *result;
	m_row record;
	char hs=0,es=0,ls=0,bs=0;

#if DEBUG
	Debug("MsqlLog called for file: [%s]\n",filename?filename:"null");
#endif

	dbsock = msqlGetDbSock();
	if(dbsock==-1) {
		dbsock = msqlConnect(NULL);
		if(dbsock<0) {
			Error("Unable to connect to msql daemon");
			return;
		}
	}
	if(msqlSelectDB(dbsock,MsqlLogDB)<0) {
		msqlClose(dbsock);
		Error("Unable to select msql logging database (%s) - %s",MsqlLogDB,msqlErrMsg);
		return;
	}
	msqlSetCurrent(dbsock,MsqlLogDB);

	uid = getmyuid();
	t = time(NULL);

	host = getremotehostname();	
	email = getemailaddr();
	ref = getrefdoc();
	lref=NULL;
	if(ref) {
		lref = estrdup(1,ref);
		s = strchr(lref,'&');
		if(s) *s='\0';
		if(strlen(lref)>128) lref[127]='\0';	
	}
	browser = getbrowser();	
#if DEBUG
	Debug("MsqlLog: filename = [%s]\n",filename?filename:"null");
#endif
	if(!filename || (filename && strlen(filename)<1)) fn = GetCurrentFilename();
	else fn = filename;

	if(fn && strlen(fn)>63) {
		fn += strlen(fn)-63;
	}
	if(host && strlen(host)>63) {
		hs = host[63];
		host[63]='\0';	
	}	
	if(email && strlen(email)>63) {
		es = email[63];
		email[63] = '\0';
	}
	if(lref && strlen(lref)>63) {
		ls = lref[63];
		lref[63]='\0';
	}
	if(browser && strlen(browser)>63) {
		bs = browser[63];
		browser[63]='\0';
	}
	sprintf(query,"insert into log%ld values (%ld,'%s','%s','%s','%s','%s')",
		uid,t,fn,host?host:"",email?email:"",lref?lref:"",browser?browser:"");
#if DEBUG
	Debug("Sending query: %s\n",query);
#endif	
	if(msqlQuery(dbsock,query)<0) {
#if DEBUG
		Debug("Query failed! (%s)\n",msqlErrMsg);
#endif
	}

	/* Atomically read and increment total and today counters */		
	sprintf(query,"select total,today,timestamp from last%ld where filename='%s'",uid,fn);
	pfn = filename_to_logfn(filename);
	sprintf(lockfn,"%s/%s%ld.lck",MSQLLOGTMP,pfn,uid);
	/* Lock database */
	lockfd = open(lockfn,O_RDWR|O_CREAT,0644);
	if(lockfd) {
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
	if(msqlQuery(dbsock,query)<0) {
#if DEBUG
		Debug("Query failed! (%s)\n",msqlErrMsg);
#endif
	} else {
		result = msqlStoreResult();
		if(result && msqlNumRows(result)>0) {
			msqlDataSeek(result,0);
			record = msqlFetchRow(result);
			if(record) {
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
			if(msqlQuery(dbsock,query)<0) {
#if DEBUG
				Debug("Query failed! (%s)\n",msqlErrMsg);
#endif
			}
		}
	}
	tm1 = localtime((time_t *)(&la));
	day1 = tm1->tm_yday;
	tm1 = localtime(&t);
	day2 = tm1->tm_yday;
	if(day1 != day2) today_count = 0;	
	total_count++;
	today_count++;
	sprintf(query,
		"update last%ld set timestamp=%ld,total=%ld,today=%ld,host='%s',email='%s',lref='%s',browser='%s' where filename='%s'",
		uid,t,total_count,today_count,host?host:"",email?email:"",lref?lref:"",browser?browser:"",fn);
#if DEBUG
	Debug("Sending query: %s\n",query);
#endif
	if(msqlQuery(dbsock,query)<0) {
#if DEBUG
		Debug("Query failed! (%s)\n",msqlErrMsg);
#endif
	}
#ifdef HAVE_LOCKF
	lockf(lockfd,F_ULOCK,0);
	close(lockfd);
#else
#ifdef HAVE_FLOCK
	flock(lockfd,LOCK_UN);
	close(lockfd);
#endif
#endif
	unlink(lockfn);
	if(hs) host[63]=hs;
	if(es) email[63]=es;
	if(ls) lref[63]=ls;
	if(bs) browser[63]=bs;
#endif
}

void msqlloadlastinfo(char *filename) {
#if MSQLLOGGING
	char *lfn;
	char query[128];
	long uid, la;
	int dbsock;
	m_result *result;
	m_row record;
	time_t t;	
	struct tm *tm1;
	int day1, day2;

	uid = getmyuid();
	if(!filename) {
#if APACHE
		filename = php_rqst->uri;
#else
		filename = getenv("PATH_INFO");
#endif
		if(!filename || (filename && strlen(filename)<1)) lfn=GetCurrentFilename();
		else lfn = filename;
	} else {
		if(!filename || (filename && strlen(filename)<1)) lfn=GetCurrentFilename();
		else lfn = filename;
	}

	dbsock = msqlGetDbSock();
	if(dbsock==-1) {
		dbsock = msqlConnect(NULL);
		if(dbsock<0) {
			Error("Unable to connect to msql daemon");
			return;
		}
	}
	if(msqlSelectDB(dbsock,MsqlLogDB)<0) {
		msqlClose(dbsock);
		Error("Unable to select msql logging database (%s)",MsqlLogDB);
		return;
	}
	msqlSetCurrent(dbsock,MsqlLogDB);
	
	sprintf(query,"select * from last%ld where filename='%s'",uid,lfn);
#if DEBUG
	Debug("msqlloadlastinfo: Sending query: %s\n",query);
#endif
	if(msqlQuery(dbsock,query)<0) {
#if DEBUG
		Debug("Query failed! (%s)\n",msqlErrMsg);
#endif
	} else {
		result = msqlStoreResult();
		if(result && msqlNumRows(result)>0) {
			msqlDataSeek(result,0);
			record = msqlFetchRow(result);
			last_access = atol(record[1]);
			la = last_access;
			total_count = atol(record[2]);	
			today_count = atol(record[3]);	
			t = time(NULL);
			tm1 = localtime((time_t *)(&la));
			day1 = tm1->tm_yday;
			tm1 = localtime(&t);
			day2 = tm1->tm_yday;
			if(day1 != day2) today_count = 0;	
			start_logging = atol(record[4]);
			last_host = estrdup(0,record[5]);
			last_email = estrdup(0,record[6]);
			last_ref = estrdup(0,record[7]);
			last_browser = estrdup(0,record[8]);
			msqlFreeResult(result);
		}
	}	
#endif
}
