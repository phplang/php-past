/***[db.c]********************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996,1997 Rasmus Lerdorf                                    *
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
/* $Id: db.c,v 1.22 1997/08/16 03:51:39 rasmus Exp $ */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "php.h"
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
#ifdef GDBM
#include <gdbm.h>
#else
#ifdef NDBM
#include <ndbm.h>
#endif
#endif
#include "parse.h"

static dbmStack *top = NULL;

void ListSupportedDBs(void) {
	char temp1[128];
	char temp[256];

	temp1[0]='\0';
	temp[0]='\0';
#ifdef GDBM
	sprintf(temp1,"%s<br>",gdbm_version);
	strcat(temp,temp1);
#else
#ifdef NDBM
	strcat(temp,"ndbm support enabled<br>\n");
#else
	strcat(temp,"flat file support enabled<br>\n");
#endif	
#endif	
#ifdef HAVE_LIBMSQL
	strcat(temp,"mSQL support enabled<br>\n");
#endif
#ifdef HAVE_LIBPQ
	strcat(temp,"Postgres95 support enabled<br>\n");
#endif
#ifdef HAVE_LIBMYSQL
	strcat(temp,"mysql support enabled<br>\n");
#endif
#ifdef HAVE_LIBSOLID
	strcat(temp,"Solid support enabled<br>\n");
#endif
#if NFS_HACK
	strcat(temp,"NFS hack in effect<br>\n");
#endif

	if(!*temp) strcat(temp,"No database support compiled into this version<br>\n");
	Push(temp,STRING);
}

void dbmPush(char *filename,char *lockfn,int lockfd, void *dbf) {
	dbmStack *new;

	new = emalloc(0,sizeof(dbmStack));
	new->next=top;
	new->filename = estrdup(0,filename);
	if(lockfn) new->lockfn = estrdup(0,lockfn);
	else new->lockfn=NULL;
	new->lockfd = lockfd;
	new->dbf = dbf;
	top = new;
}

void *dbmPop(void) {
	static dbmStack new;
	dbmStack *s;
#ifndef APACHE
	static int First=1;
#endif

#if APACHE
	new.filename = NULL;
	new.lockfn = NULL;
	if(!top) return(NULL);
#else
	if(First) {
		new.filename = NULL;
		new.lockfn = NULL;
	}
	if(!top) return(NULL);
	else new.lockfn=NULL;
	First=0;
#endif
	new.filename = estrdup(0,top->filename);
	if(top->lockfn) new.lockfn = estrdup(0,top->lockfn);
	new.lockfd = top->lockfd;
	new.dbf = top->dbf;
	s = top;
	top = top->next;
	return(&new);
}

/* Finds a node and pushes it to the top of the stack */
void *dbmFind(char *filename) {
	dbmStack *s, *p;

	p = top;
	s = top;

	while(s) {	
		if(!strcmp(filename,s->filename)) {
			if(s == top) return(s);
			if(s->next == NULL) {
				s->next = top->next;
				top->next = NULL;
				p->next = top;
				top = s;
				return(s);
			}
			if(s->next) {
				p->next = s->next;
				s->next = top;
				top = s;
				return(s);
			}
		}
		p = s;
		s=s->next;
	}
	return(NULL);
}

void dbmCloseAll(void) {
	dbmStack *s;
#if NFS_HACK

#else
	int lockfd=0;
#endif

	s = dbmPop();
	while(s) {
		if(s->lockfn) {
#if NFS_HACK

#else
#ifdef HAVE_LOCKF
			lockfd = open(s->lockfn,O_RDWR,0644);
			lockf(lockfd,F_ULOCK,0);
			close(lockfd);
#else
#ifdef HAVE_FLOCK
			lockfd = open(s->lockfn,O_RDWR,0644);
			flock(s->lockfd,LOCK_UN);
			close(lockfd);
#endif
#endif
#endif
#if NFS_HACK
			unlink(s->lockfn);
#endif
		}
#ifdef GDBM
		if(s->dbf) gdbm_close(s->dbf);
#else
#ifdef NDBM
		if(s->dbf) dbm_close(s->dbf);
#else
		if(s->dbf) fclose(s->dbf);
#endif
#endif
		s = dbmPop();
	}
}	

void dbmOpen(void) {
	Stack *s;
	char *filename;
	char *mode;
	char temp[256];
	int ret;

	s = Pop();
	if(!s) {
		Error("Stack error in dbmopen");
		return;
	}
	mode = estrdup(0,s->strval);

	s = Pop();
	if(!s) {
		Error("Stack error in dbmopen");
		return;
	}
	filename = estrdup(0,s->strval);

	ret = _dbmOpen(filename,mode,1);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}

int _dbmOpen(char *filename, char *mode, int docroot) {
	char *fn;
	int ret,lock=0;
	char *lockfn=NULL;
	int lockfd=0;
#if NFS_HACK
	int last_try=0;
	struct stat sb;
	int retries=0;
#endif
#ifdef GDBM 
	GDBM_FILE dbf;
	int imode=0;
#else 
#ifdef NDBM 
	DBM *dbf;
	int imode=0;
#else 
	FILE *dbf;
	char imode[8];
#endif 
#endif 

	switch(*mode) {
		case 'w': 
#ifdef GDBM
			imode=GDBM_WRITER; 
#else 
#ifdef NDBM 
			imode=O_RDWR;
#else 
			strcpy(imode,"r+b");
#endif  
#endif 
			lock=1;
			break;
		case 'c': 
#ifdef GDBM
			imode=GDBM_WRCREAT;
#else
#ifdef NDBM 
			imode=O_RDWR | O_APPEND | O_CREAT;
#else
			strcpy(imode,"a+b");
#endif 
#endif 
			lock=1;
			break;
		case 'n': 
#ifdef GDBM 
			imode=GDBM_NEWDB;
#else
#ifdef NDBM 
			imode=O_RDWR | O_CREAT | O_TRUNC;
#else
			strcpy(imode,"w+b");
#endif 
#endif 
			lock=1;
			break;
		default: 
#ifdef GDBM 
			imode=GDBM_READER;
#else
#ifdef NDBM 
			imode=O_RDONLY;
#else
			strcpy(imode,"r");
#endif 
#endif 
			lock=0;
			break;
	}
	if(docroot) fn = FixFilename(filename,0,&ret,0);
	else fn=filename;	
#ifdef GDBM 
	if(lock) {
		lockfn = emalloc(1,strlen(fn)+10);
		strcpy(lockfn,fn);
		strcat(lockfn,".lck");
#if NFS_HACK 
		while((last_try = stat(lockfn,&sb))==0) {
			retries++;
			sleep(1);
			if(retries>30) break;
		}	
		if(last_try!=0) {
			lockfd = open(lockfn,O_RDWR|O_CREAT,0644);
			close(lockfd);
		} else {
			Error("File appears to be locked [%s]\n",lockfn);
			return(-1);
		}
#else /* NFS_HACK */
		lockfd = open(lockfn,O_RDWR|O_CREAT,0644);
		if(lockfd) {
#ifdef HAVE_LOCKF 
			lockf(lockfd,F_LOCK,0);
#else
#ifdef HAVE_FLOCK 
			flock(lockfd,LOCK_EX);
#endif 
#endif 
			close(lockfd);
		} else {
			Error("Unable to establish lock: %s",fn);
		}
#endif /* else NFS_HACK */
	}
	dbf = gdbm_open(fn,512,imode,0666,0);
#else
#ifdef NDBM 
	if(lock) {
		lockfn = emalloc(1,strlen(fn)+10);
		strcpy(lockfn,fn);
		strcat(lockfn,".lck");
#if NFS_HACK 
		while((last_try = stat(lockfn,&sb))==0) {
			retries++;
#if DEBUG
			Debug("Retry #%d last_try=%d\n",retries,last_try);
#endif
			sleep(1);
			if(retries>30) break;
		}	
		if(last_try!=0) {
			lockfd = open(lockfn,O_RDWR|O_CREAT,0644);
			close(lockfd);
		} else {
			Error("File appears to be locked [%s]\n",lockfn);
			return(-1);
		}
#else
		lockfd = open(lockfn,O_RDWR|O_CREAT,0644);
		if(lockfd) {
#ifdef HAVE_LOCKF 
			lockf(lockfd,F_LOCK,0);
#else
#ifdef HAVE_FLOCK 
			flock(lockfd,LOCK_EX);
#endif 
#endif 
			close(lockfd);
		} else {
			Error("Unable to establish lock: %s",fn);
		}
#endif /* NFS_HACK */
	}
	dbf = dbm_open(fn,imode,0666);
#else /* flat file */
#if NFS_HACK 
	if(lock) {
		lockfn = emalloc(1,strlen(fn)+10);
		strcpy(lockfn,fn);
		strcat(lockfn,".lck");
		while((last_try = stat(lockfn,&sb))==0) {
			retries++;
#if DEBUG
			Debug("Retry #%d last_try=%d\n",retries,last_try);
#endif
			sleep(1);
			if(retries>30) break;
		}	
		if(last_try!=0) {
			lockfd = open(lockfn,O_RDWR|O_CREAT,0644);
			close(lockfd);
		} else {
			Error("File appears to be locked [%s]\n",lockfn);
			return(-1);
		}
	}
#endif /* NFS_HACK */
	dbf = fopen(fn,imode);
	if(dbf) {
		setvbuf(dbf,NULL,_IONBF,0);
		if(lock) {
			lockfd = fileno(dbf);
#if NFS_HACK 
				
#else			
#ifdef HAVE_LOCKF 
			lockf(fileno(dbf),F_LOCK,0);
#else
#ifdef HAVE_FLOCK 
			flock(fileno(dbf),LOCK_EX);
#endif 
#endif 
#endif /* NFS_HACK */
		}
	}	
#endif /* NDBM */
#endif /* GDBM */
	if(dbf) {
		dbmPush(filename,lockfn,lockfd,dbf);
		ret = 0;
	} else {
#ifdef GDBM 
		Error("dbmopen: %d [%s], %d [%s]",gdbm_errno,gdbm_strerror(gdbm_errno),errno,strerror(errno));
		if(gdbm_errno) ret = gdbm_errno;
		else if(errno) ret = errno;
		else ret = -1;
#else 
#ifdef NDBM 
#if DEBUG
		Debug("dbmOpen (ndbm): errno = %d [%s]\n",errno,strerror(errno));
#endif
		if(errno) ret=errno;
		else ret = -1;
#else
#if DEBUG
		Debug("dbmOpen (flat file): errno = %d [%s]\n",errno,strerror(errno));
#endif
		if(errno) ret=errno;
		else ret = -1;
#endif 
#endif 	
#if NFS_HACK
		if(lockfn) {
			unlink(lockfn);
		}
#endif
	}
	return(ret);
}

void dbmClose(void) {
	Stack *s;
	char temp[16];
	int ret;

	s = Pop();
	if(!s) {
		Error("Stack error in dbmclose");
		return;
	}
	ret = _dbmClose(s->strval);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}

int _dbmClose(char *filename) {
	dbmStack *st;
	int ret;
	char *fn;
#ifdef GDBM
	GDBM_FILE dbf;
	int lockfd;
#else
#ifdef NDBM
	DBM *dbf;
	int lockfd;
#else
	FILE *dbf;
#endif
#endif

	st = (dbmStack *)dbmFind(filename);
	if(st) dbf = st->dbf;
	else dbf=(void *)0;
	if(!dbf) {
		Error("Unable to close %s",filename);
		return(-1);
	} else ret = 0;
	fn = emalloc(1,sizeof(char) * (strlen(st->filename)+10));
	strcpy(fn,st->filename);
	strcat(fn,".lck");
#ifdef GDBM
#if NFS_HACK
	unlink(fn);
#else
	if(st->lockfn) {
#ifdef HAVE_LOCKF
		lockfd = open(st->lockfn,O_RDWR,0644);
		lockf(lockfd,F_ULOCK,0);
		close(lockfd);
#else
#ifdef HAVE_FLOCK
		lockfd = open(st->lockfn,O_RDWR,0644);
		flock(lockfd,LOCK_UN);
		close(lockfd);
#endif
#endif
	}
#endif
	if(dbf) gdbm_close(dbf);
#else
#ifdef NDBM
#if NFS_HACK
	unlink(fn);
#else
	if(st->lockfn) {
#ifdef HAVE_LOCKF
		lockfd = open(st->lockfn,O_RDWR,0644);
		lockf(lockfd,F_ULOCK,0);
		close(lockfd);
#else
#ifdef HAVE_FLOCK
		lockfd = open(st->lockfn,O_RDWR,0644);
		flock(lockfd,LOCK_UN);
		close(lockfd);
#endif
#endif
	}
#endif
	if(dbf) dbm_close(dbf);
#else
#if NFS_HACK
	unlink(fn);
	if(dbf) {
#else
	if(dbf) {
#ifdef HAVE_LOCKF
		lockf(fileno(dbf),F_ULOCK,0);
#else
#ifdef HAVE_FLOCK
		flock(fileno(dbf),LOCK_UN);
#endif
#endif
#endif
		fclose(dbf);
	}
#endif
#endif
	dbmPop();
	return(ret);
}	

/*
 * ret = -1 means that database was opened for read-only
 * ret = 0  success
 * ret = 1  key already exists - nothing done
 */
void dbmInsert(void) {
	Stack *s;
	int ret;
	char temp[16];
	static char *keystr, *contentstr;

	s = Pop();
	if(!s) {
		Error("Stack error in dbmreplace");
		return;
	}
	contentstr = estrdup(1,s->strval);

	s = Pop();
	if(!s) {
		Error("Stack error in dbmreplace");
		return;
	}
	keystr = estrdup(1,s->strval);

	s = Pop();
	if(!s) {
		Error("Stack error in dbmreplace");
		return;
	}
	ret = _dbmInsert(s->strval,keystr,contentstr);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}

int _dbmInsert(char *filename, char *keystr, char *contentstr) {
#ifndef APACHE
	static int First=1;
#endif
	static datum key, content;
	int ret;
	dbmStack *st;
#ifdef GDBM
	GDBM_FILE dbf;
#else
#ifdef NDBM
	DBM *dbf;
#else
	FILE *dbf;
#endif
#endif
#if DEBUG
	Debug("_dbmInsert called with [%s] [%s] [%s]\n",filename,keystr,contentstr);
#endif
#if APACHE
	key.dptr = NULL;
	content.dptr = NULL;
#else
	if(First) {
		key.dptr = NULL;
		content.dptr = NULL;
	}
	First=0;
#endif
	StripSlashes(contentstr);
	StripSlashes(keystr);
	content.dptr = estrdup(1,contentstr);
	content.dsize = strlen(contentstr);
	key.dptr = estrdup(1,keystr);
#ifdef GDBM_FIX
	key.dsize = strlen(keystr)+1;
#else
	key.dsize = strlen(keystr);
#endif

	st = (dbmStack *)dbmFind(filename);
	if(!st) {
		Error("Unable to find dbf pointer");
		return(1);
	}
	dbf = st->dbf;
	if(!dbf) {
		Error("Unable to locate dbm file");
		return(1);
	}
#ifdef GDBM
	ret = gdbm_store(dbf, key, content, GDBM_INSERT);
#else
#ifdef NDBM
	ret = dbm_store(dbf, key, content, DBM_INSERT);
#else
	fseek(dbf,0L,SEEK_END);
	fprintf(dbf,"%d\n",key.dsize);
	fflush(dbf);
	ret = write(fileno(dbf),key.dptr,key.dsize);
	fprintf(dbf,"%d\n",content.dsize-1);
	fflush(dbf);
	ret = write(fileno(dbf),content.dptr,content.dsize-1);
	if(ret>0) ret=0;
#endif
#endif
	return(ret);	
}	

#ifndef GDBM
#ifndef NDBM

static long CurrentFlatFilePos = 0L;

void flatfilewipekey(FILE *dbf, void *key, int size) {
	char *buf;
	int num;
	int buf_size=1024;
	long pos;

	rewind(dbf);
	buf = emalloc(1,(buf_size+1)*sizeof(char));
	while(!feof(dbf)) {
		if(!fgets(buf,15,dbf)) break;
		num = atoi(buf);
		if(num > buf_size) {
			buf_size+=num;
			buf = emalloc(1,(buf_size+1)*sizeof(char));
		}
		pos = ftell(dbf);
		num=read(fileno(dbf),buf,num);
		if(num<0) break;
		*(buf+num) = '\0';
		if(size == num) {
			if(!memcmp(buf,key,size)) {
				fseek(dbf,pos,SEEK_SET);
				fputc(0,dbf);
				fflush(dbf);
				fseek(dbf,0L,SEEK_END);
				break;
			}
		}	
		if(!fgets(buf,15,dbf)) break;
		num = atoi(buf);
		if(num > buf_size) {
			buf_size+=num;
			buf = emalloc(1,(buf_size+1)*sizeof(char));
		}
		num=read(fileno(dbf),buf,num);
		if(num<0) break;
		*(buf+num) = '\0';
	}
}	

int flatfilefindkey(FILE *dbf, void *key, int size) {
	char *buf;
	int num;
	int buf_size=1024;
	int ret=0;

	rewind(dbf);
	buf = emalloc(1,(buf_size+1)*sizeof(char));
	while(!feof(dbf)) {
		if(!fgets(buf,15,dbf)) break;
		num = atoi(buf);
		if(num > buf_size) {
			buf_size+=num;
			buf = emalloc(1,(buf_size+1)*sizeof(char));
		}
		num=read(fileno(dbf),buf,num);
		if(num<0) break;
		*(buf+num) = '\0';
		if(size == num) {
			if(!memcmp(buf,key,size)) {
				ret = 1;
				break;
			}
		}	
		if(!fgets(buf,15,dbf)) break;
		num = atoi(buf);
		if(num > buf_size) {
			buf_size+=num;
			buf = emalloc(1,(buf_size+1)*sizeof(char));
		}
		num=read(fileno(dbf),buf,num);
		if(num<0) break;
		*(buf+num) = '\0';
	}
	return(ret);
}	

datum flatfilefirstkey(FILE *dbf) {
	static datum buf;
	int num;
	int buf_size=1024;

	rewind(dbf);
	buf.dptr = emalloc(1,(buf_size+1)*sizeof(char));
	while(!feof(dbf)) {
		if(!fgets(buf.dptr,15,dbf)) break;
		num = atoi(buf.dptr);
		if(num > buf_size) {
			buf_size+=num;
			buf.dptr = emalloc(1,(buf_size+1)*sizeof(char));
		}
		num=read(fileno(dbf),buf.dptr,num);
		if(num<0) break;
		buf.dsize = num;
		if(*(buf.dptr)!=0) {
			CurrentFlatFilePos = ftell(dbf);
			return(buf);
		}
		if(!fgets(buf.dptr,15,dbf)) break;
		num = atoi(buf.dptr);
		if(num > buf_size) {
			buf_size+=num;
			buf.dptr = emalloc(1,(buf_size+1)*sizeof(char));
		}
		num=read(fileno(dbf),buf.dptr,num);
		if(num<0) break;
	}
	buf.dptr = NULL;
	return(buf);
}	

datum flatfilenextkey(FILE *dbf) {
	static datum buf;
	int num;
	int buf_size=1024;

	fseek(dbf,CurrentFlatFilePos,SEEK_SET);
	buf.dptr = emalloc(1,(buf_size+1)*sizeof(char));
	while(!feof(dbf)) {
		if(!fgets(buf.dptr,15,dbf)) break;
		num = atoi(buf.dptr);
		if(num > buf_size) {
			buf_size+=num;
			buf.dptr = emalloc(1,(buf_size+1)*sizeof(char));
		}
		num=read(fileno(dbf),buf.dptr,num);
		if(num<0) break;
		if(!fgets(buf.dptr,15,dbf)) break;
		num = atoi(buf.dptr);
		if(num > buf_size) {
			buf_size+=num;
			buf.dptr = emalloc(1,(buf_size+1)*sizeof(char));
		}
		num=read(fileno(dbf),buf.dptr,num);
		if(num<0) break;
		buf.dsize = num;
		if(*(buf.dptr)!=0) {
			CurrentFlatFilePos = ftell(dbf);
			return(buf);
		}
	}
	buf.dptr = NULL;
	return(buf);
}	
#endif
#endif

void dbmReplace(void) {
	Stack *s;
	int ret;
	char temp[16];
	char *contentstr, *keystr;

	s = Pop();
	if(!s) {
		Error("Stack error in dbmreplace");
		return;
	}
	contentstr = estrdup(1,s->strval);

	s = Pop();
	if(!s) {
		Error("Stack error in dbmreplace");
		return;
	}
	keystr = estrdup(1,s->strval);

	s = Pop();
	if(!s) {
		Error("Stack error in dbmreplace");
		return;
	}
	ret = _dbmReplace(s->strval,keystr,contentstr);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}

int _dbmReplace(char *filename, char *keystr, char *contentstr) {
#ifndef APACHE
	static int First=1;
#endif
	static datum key, content;
#ifdef GDBM
	GDBM_FILE dbf;
#else
#ifdef NDBM
	DBM *dbf;
#else
	FILE *dbf;
#endif
#endif
	dbmStack *st;
	int ret;

#if DEBUG
	Debug("_dbmReplace called with [%s] [%s] [%s]\n",filename,keystr,contentstr);
#endif
#if APACHE
	key.dptr = NULL;
	content.dptr = NULL;
#else
	if(First) {
		key.dptr = NULL;
		content.dptr = NULL;
	}
	First=0;
#endif

	content.dptr = estrdup(1,contentstr);
	content.dsize = strlen(contentstr);  
	key.dptr = estrdup(1,keystr);
#ifdef GDBM_FIX
	key.dsize = strlen(keystr)+1;
#else
	key.dsize = strlen(keystr);
#endif

	st = (dbmStack *)dbmFind(filename);
	if(!st || (st && !st->dbf)) {
		Error("Unable to locate dbm file");
		return(-1);
	}
	dbf = st->dbf;
#ifdef GDBM
	ret = gdbm_store(dbf, key, content, GDBM_REPLACE);
#else
#ifdef NDBM
	ret = dbm_store(dbf, key, content, DBM_REPLACE);
#else
	flatfilewipekey(dbf,key.dptr,key.dsize);
	fprintf(dbf,"%d\n",key.dsize);
	fflush(dbf);
	ret = write(fileno(dbf),key.dptr,key.dsize);
	fprintf(dbf,"%d\n",content.dsize);
	ret = write(fileno(dbf),content.dptr,content.dsize);
	if(ret>0) ret=0;
#endif
#endif
	return(ret);
}	

void dbmFetch(void) {
	Stack *s;
	char *keystr;

	s = Pop();
	if(!s) {
		Error("Stack error in dbmfetch");
		return;
	}
	keystr = estrdup(1,s->strval);

	s = Pop();
	if(!s) {
		Error("Stack error in dbmfetch");
		return;
	}
	Push(_dbmFetch(s->strval,keystr),STRING);
}

char *_dbmFetch(char *filename, char *keystr) {
	dbmStack *st;
	static char temp[1] = { '\0' };
	static datum key, content;
#ifndef APACHE
	static int First=1;
#endif
	static char *ret=NULL;
#ifdef GDBM
	GDBM_FILE dbf;
#else
#ifdef NDBM
	DBM *dbf;
#else
	FILE *dbf;
	int num=0, buf_size=1024;
	char *buf;	
#endif
#endif

#if APACHE	
	key.dptr = NULL;
#else
	if(First) {
		key.dptr = NULL;
	} else First=0;
#endif	

	key.dptr = estrdup(1,keystr);
#ifdef GDBM_FIX
	key.dsize = strlen(keystr)+1;
#else
	key.dsize = strlen(keystr);
#endif

	st = (dbmStack *)dbmFind(filename);
	if(!st) {
#if DEBUG
		Debug("Unable to find dbm pointer on stack\n");
#endif
		return(NULL);
	}
	dbf = st->dbf;
	if(!dbf) {
		Error("Unable to locate dbm file");
		return(NULL);
	}

#ifdef GDBM
	content = gdbm_fetch(dbf,key);	
#else
#ifdef NDBM
	content = dbm_fetch(dbf,key);
#else
	if(flatfilefindkey(dbf,key.dptr,key.dsize)) {
		buf = emalloc(1,(buf_size+1) * sizeof(char));
		if(fgets(buf,15,dbf)) {
			num = atoi(buf);
			if(num > buf_size) {
				buf_size+=num;
				buf = emalloc(1,(buf_size+1)*sizeof(char));
			}
			read(fileno(dbf),buf,num);
			content.dptr = buf;
			content.dsize = num;
		}
	}		
#endif
#endif

	if(ret != temp) ret = temp;
	if(content.dptr) {
		ret = emalloc(1,sizeof(char) * (content.dsize+1));
		memcpy(ret,content.dptr,content.dsize);
		*(ret+content.dsize) = '\0';
#ifdef GDBM /* GDBM uses malloc to allocate the content blocks, so we need to free it */
		free(content.dptr);
#else
#ifndef NDBM /* the internal flat-file format uses malloc as well */
		free(content.dptr);
#endif
#endif
	}
	else ret=temp;
#if DEBUG
	Debug("_dbmFetch called with key: [%s], returning [%s]\n",key.dptr?key.dptr:"null",ret?ret:"null");
#endif
	if(ret!=temp) return((ret=AddSlashes(ret,1)));
	else return(ret);
}

void dbmExists(void) {
	Stack *s;
	char temp[4];
	int ret;
	char *keystr;

	s = Pop();
	if(!s) {
		Error("Stack error in dbmexists");
		return;
	}
	keystr = estrdup(1,s->strval);

	s = Pop();
	if(!s) {
		Error("Stack error in dbmexists");
		return;
	}
	ret = _dbmExists(s->strval, keystr);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}

int _dbmExists(char *filename, char *keystr) {
	dbmStack *st;
	static datum key;
#ifndef APACHE
	static int First=1;
#endif
	int ret=0;
#ifdef GDBM
	GDBM_FILE dbf;
#else
#ifdef NDBM
	DBM *dbf;
	datum content;
#else
	FILE *dbf;
#endif
#endif

#if APACHE
	key.dptr = NULL;
#else
	if(First) {
		key.dptr = NULL;
	} else First = 0;
#endif

	key.dptr = estrdup(1,keystr);
#ifdef GDBM_FIX
	key.dsize = strlen(keystr)+1;
#else
	key.dsize = strlen(keystr);
#endif

	st = (dbmStack *)dbmFind(filename);	
	dbf = st->dbf;
	if(!dbf) {
		Error("Unable to locate dbm file");
		return(-1);
	}
#ifdef GDBM
	ret = gdbm_exists(dbf,key);
#else
#ifdef NDBM
	content = dbm_fetch(dbf,key);
	if(content.dptr) ret = 1;
	else ret = 0;
#else
	if(flatfilefindkey(dbf,key.dptr,key.dsize)) ret = 1;
#endif
#endif
	return(ret);
}
		
void dbmDelete(void) {
	Stack *s;
	char temp[4];
	int ret;
	char *keystr;

	s = Pop();
	if(!s) {
		Error("Stack error in dbmdelete");
		return;
	}
	keystr = estrdup(1,s->strval);

	s = Pop();
	if(!s) {
		Error("Stack error in dbmdelete");
		return;
	}
	ret = _dbmDelete(s->strval,keystr);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}

int _dbmDelete(char *filename, char *keystr) {
	dbmStack *st;
	static datum key;
#ifndef APACHE
	static int First=1;
#endif
	int ret=0;
#ifdef GDBM
	GDBM_FILE dbf;
#else
#ifdef NDBM
	DBM *dbf;
#else
	FILE *dbf;
#endif
#endif

#if DEBUG
	Debug("_dbmDelete called with [%s] [%s]\n",filename,keystr);
#endif
#if APACHE
	key.dptr = NULL;
#else
	if(First) {
		key.dptr = NULL;
	} else First = 0;
#endif

	key.dptr = estrdup(1,keystr);
#ifdef GDBM_FIX
	key.dsize = strlen(keystr)+1;
#else
	key.dsize = strlen(keystr);
#endif

	st = (dbmStack *)dbmFind(filename);	
	dbf = st->dbf;
	if(!dbf) {
		Error("Unable to locate dbm file");
		return(-1);
	}
#ifdef GDBM
	ret = gdbm_delete(dbf,key);
#else
#ifdef NDBM
	ret = dbm_delete(dbf,key);
#else
	flatfilewipekey(dbf,key.dptr,key.dsize);
#endif
#endif
	return(ret);
}

void dbmFirstKey(void) {
	Stack *s;
	char *ret;
	s = Pop();

	if(!s) {
		Error("Stack error in dbmfirstkey");
		return;
	}
	ret = _dbmFirstKey(s->strval);
	if(!ret) Push("",STRING);
	else Push(ret,STRING);
}

char *_dbmFirstKey(char *filename) {
	dbmStack *st;
#ifndef APACHE
	static int First=1;
#endif
	static datum ret;
	static char *retstr;
#ifdef GDBM
	GDBM_FILE dbf;
#else
#ifdef NDBM
	DBM *dbf;
#else
	FILE *dbf;
#endif
#endif

#if APACHE
	ret.dptr = NULL;
	retstr = NULL;
#else
	if(First) {
		ret.dptr = NULL;
		retstr = NULL;
	} else First = 0;
#endif
	st = (dbmStack *)dbmFind(filename);	
	if(!st) {
		Error("Unable to locate dbm file");
		return(NULL);
	}
	dbf = st->dbf;
	if(!dbf) {
		Error("Unable to locate dbm file");
		return(NULL);
	}
#ifdef GDBM
	ret = gdbm_firstkey(dbf);
#else
#ifdef NDBM
	ret = dbm_firstkey(dbf);
#else
	ret = flatfilefirstkey(dbf);
#endif
#endif
	if(!ret.dptr) return(NULL);
	retstr = emalloc(1,(1+ret.dsize)*sizeof(char));
	memcpy(retstr,ret.dptr,ret.dsize);	
	retstr[ret.dsize] = '\0';
	return(retstr);
}

void dbmNextKey(void) {
	Stack *s;
	char *ret;
	char *keystr;

	s = Pop();
	if(!s) {
		Error("Stack error in dbmdelete");
		return;
	}
	keystr = estrdup(1,s->strval);
#if DEBUG
	Debug("dbmNextKey: keystr = [%s]\n",keystr);
#endif

	s = Pop();
	if(!s) {
		Error("Stack error in dbmdelete");
		return;
	}
	ret = _dbmNextKey(s->strval,keystr);
	if(!ret) Push("",STRING);
	else Push(ret,STRING);
}

char *_dbmNextKey(char *filename, char *keystr) {
	dbmStack *st;
	static datum key;
#ifndef APACHE
	static int First=1;
#endif
	static datum ret;
	static char *retstr;
#ifdef GDBM
	GDBM_FILE dbf;
#else
#ifdef NDBM
	DBM *dbf;
#else 
	FILE *dbf;
#endif
#endif

#if APACHE
	key.dptr = NULL;
	ret.dptr = NULL;
	retstr = NULL;
#else
	if(First) {
		key.dptr = NULL;
		ret.dptr = NULL;
		retstr = NULL;
	} else First = 0;
#endif

	key.dptr = estrdup(1,keystr);
#ifdef GDBM_FIX
	key.dsize = strlen(keystr)+1;
#else
	key.dsize = strlen(keystr);
#endif

	st = (dbmStack *)dbmFind(filename);	
	if(!st) {
		Error("Unable to locate dbm pointer for [%s]",filename);
		return(NULL);
	}
	dbf = st->dbf;
	if(!dbf) {
		Error("Unable to locate dbm file");
		return(NULL);
	}
#ifdef GDBM
	ret = gdbm_nextkey(dbf,key);
#else
#ifdef NDBM
	ret = dbm_nextkey(dbf);
#else
	ret = flatfilenextkey(dbf);
#endif
#endif
	if(!ret.dptr) return(NULL);
	retstr = emalloc(1,(1+ret.dsize) * sizeof(char));
	memcpy(retstr,ret.dptr,ret.dsize);
	retstr[ret.dsize] = '\0';
	return(retstr);
}

void php_init_db(void) {
	top = NULL;
#ifndef GDBM
#ifndef NDBM
	CurrentFlatFilePos = 0L;
#endif
#endif
}
