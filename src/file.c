/***[file.c]******************************************************[TAB=4]****\
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
/* $Id: file.c,v 1.80 1997/11/25 21:40:32 rasmus Exp $ */
#include "php.h"
#include <stdlib.h>
#include <sys/param.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_GRP_H
#include <grp.h>
#endif
#include <errno.h>
#include "parse.h"
#include <ctype.h>
#if APACHE
#include "http_protocol.h"
#include "http_request.h"
#endif
#if WINNT|WIN32
#include "win32/wfile.h"
#include "win32/param.h"
#endif

static char *CurrentFilename=NULL;
static char *CurrentStatFile=NULL;
#if WINNT|WIN32
static unsigned int CurrentStatLength=0;
#else
static int CurrentStatLength=0;
#endif
static char *CurrentPI=NULL;
static long CurrentFileSize=0L;
static struct stat gsb;
static int fgetss_state=0;
static char *IncludePath = NULL;
static char *AutoPrependFile = NULL;
static char *AutoAppendFile = NULL;

static FpStack *fp_top = NULL;

#if APACHE
void php_init_file(php_module_conf *conf) {
#else
void php_init_file(void) {
#endif
	CurrentFilename=NULL;
	CurrentStatFile=NULL;
	CurrentStatLength=0;
	CurrentPI=NULL;
	CurrentFileSize=0L;
	fp_top = NULL;
	fgetss_state=0;
#if APACHE
	IncludePath = conf->IncludePath;
	AutoPrependFile = conf->AutoPrependFile;
	AutoAppendFile = conf->AutoAppendFile;
#endif
	if (IncludePath == NULL) {
		char *path;
		path = getenv("PHP_INCLUDE_PATH");
		if(path) {
			IncludePath = estrdup(0, path);
		}
	}
	if (IncludePath == NULL) {
	    IncludePath = estrdup(0, INCLUDEPATH);
	}
	if (AutoPrependFile == NULL) {
	    char *file;
	    if ((file = getenv("PHP_AUTO_PREPEND_FILE"))) {
			AutoPrependFile = estrdup(0, file);
	    }
	}
	if (AutoAppendFile == NULL) {
	    char *file;
	    if ((file = getenv("PHP_AUTO_APPEND_FILE"))) {
		AutoAppendFile = estrdup(0, file);
	    }
	}
}

void StripLastSlash(char *str) {
	char *s;

	s = strrchr(str,'/');
	if(s) {
		*s='\0';
	}
}

/* 
 * Opens a file for parsing.  This function is overly complex and
 * could use a rewrite/rethink.
 */
int OpenFile(char *filename, int top, long *file_size) {
	char *fn, *pi, *sn, *fn2=NULL, *fn3=NULL;
	char *s=NULL, *ss=NULL;
	int ret=-1, careful=1;
	int fd;
	int err, len;
	char erbuf[100];
	regex_t re;
	regmatch_t subs[1];
#ifdef PHP_ROOT_DIR
	char temp[1024];
#endif

	if(!filename) {
#if APACHE
		pi = php_rqst->uri;
#else
		pi = getenv("PATH_INFO");
#endif
		if(pi) set_path_dir(pi);
#ifndef PHP_ROOT_DIR
#if APACHE
		fn = php_rqst->filename;
#else
#if WIN32
		fn = getenv("PATH_TRANSLATED"); 
#else
		fn = pi;
#endif
#endif
		if(!fn || (fn && !*fn)) { Info(); return(-1); }
#else
		if(!pi) { Info(); return(-1); }
		sprintf(temp,"%s%s",PHP_ROOT_DIR,pi);
		fn = temp;
#endif
	} else {
		fn = filename;
		pi = filename;
		careful=0;
	}
	/*
	 * To prevent someone from uploading a file and then running it through php
	 */
	if(!strncmp(fn,"phpfi",5)) {
		Error("You are explicitly not allowed to open any files that begin with &quot;phpfi&quot; for security reasons");
		return(-1);
	}
#ifdef PATTERN_RESTRICT
	err = regcomp(&re, PATTERN_RESTRICT, 0);
	if(err) {
		len = regerror(err, &re, erbuf, sizeof(erbuf));
		Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
		return(-1);
	}
#if DEBUG
	Debug("Checking pattern restriction: \"%s\" against \"%s\"\n",PATTERN_RESTRICT,fn);
#endif
	err = regexec(&re,fn,(size_t)1,subs,0);
	if(err && err!= REG_NOMATCH) {
		len = regerror(err, &re, erbuf, sizeof(erbuf));
		Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
		regfree(&re);
		return(-1);
	}
	if(err==REG_NOMATCH) {
		if(getenv("PATH_TRANSLATED")) {   /* ie. don't apply restriction when run from command line */
			Error("Sorry, you are not permitted to load that file through PHP/FI.");
			regfree(&re);
			return(-1);
		}
	}
	regfree(&re);
#endif
	/* Make sure the is no '..' in the path */
	if(top) {
		err = regcomp(&re, "\\.\\.", 0);
		if(err) {
			len = regerror(err, &re, erbuf, sizeof(erbuf));
			Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
			return(-1);
		}
		err = regexec(&re,fn,(size_t)1,subs,0);
		if(err && err!= REG_NOMATCH) {
			len = regerror(err, &re, erbuf, sizeof(erbuf));
			Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
			regfree(&re);
			return(-1);
		}
		if(err!=REG_NOMATCH) {
			Error("Sorry, you are not permitted to use '..' in your pathname.");
			regfree(&re);
			return(-1);
		}
		regfree(&re);
	}
	fn = (char *)estrdup(1,FixFilename(fn,top,&ret,careful));
	*file_size = (long)gsb.st_size;
	CurrentFileSize = (long)gsb.st_size;
	fd=ret;
	if(ret==-1) {
#if DEBUG
		Debug("Unable to find file [%s] - %s",fn,strerror(errno));
#endif
#ifdef VIRTUAL_PATH
		sn  = VIRTUAL_PATH;
#else
#if APACHE
		sn = php_rqst->filename;
#else
		sn  = getenv("SCRIPT_NAME");
#endif
#endif
		if(sn) {
			fn = emalloc(1,sizeof(char) * (strlen(pi)+strlen(sn)+3));
			strcpy(fn,sn);
			s = strrchr(fn,'/');
			if(s) *s='\0';
			if(*pi!='/') strcat(fn,"/");
			strcat(fn,pi);
			fn2 = (char *) estrdup(1,FixFilename(fn,1,&ret,careful));
			if(ret==-1) fn2 = NULL;
		} else {
			fd=-1;
			ret=-1;
		}
	}
	if(ret!=-1) {
		if((gsb.st_mode&S_IFMT) == S_IFDIR) {
			fn3 = emalloc(1,sizeof(char) * (strlen(fn) + 20));
			strcpy(fn3,fn);
			strcat(fn3,"/index.html");
			fd = stat(fn3,&gsb);
			if(fd==-1) {
				strcpy(fn3,fn);
				strcat(fn3,"/index.phtml");
				fd = stat(fn3,&gsb);
				if(fd==-1) {
					if(fn2) {
						strcpy(fn3,fn2);
						strcat(fn3,"/index.html");
						fd = stat(fn3,&gsb);
						if(fd==-1) {
							strcpy(fn3,fn2);
							strcat(fn3,"/index.phtml");
							fd = stat(fn3,&gsb);
							if(fd==-1) {
								*fn3='\0';
							}
						}
					}
				}
			}
		}
		if(fn3 && *fn3) {
#if DEBUG
			Debug("Opening fn3 [%s]\n",fn3);
#endif
			fd = open(fn3,O_RDONLY);
			if(fd==-1) {
				Error("(1)Unable to open <i>%s</i>",fn3);
				return(-1);
			}
		} else if(!fn2) {
#if DEBUG
			Debug("Opening fn [%s]\n",fn);
#endif
			fd = open(fn,O_RDONLY);
			if(fd==-1) {
				Error("(2)Unable to open <i>%s</i>",fn);
				return(-1);
			}
		} else {
#if DEBUG
			Debug("Opening fn2 [%s]\n",fn2);
#endif
			fd = open(fn2,O_RDONLY);
			if(fd==-1) {
				Error("(3)Unable to open <i>%s</i>",fn2);
				return(-1);
			}
		}
		if(top) SetStatInfo(&gsb);
		
#if ACCESS_CONTROL
		if(top) {
			if(CheckAccess(pi,gsb.st_uid)<0) return(-1);
		}
#endif
		if(!filename) CurrentPI = estrdup(0,pi);	
		if(CurrentFilename) CurrentFilename=NULL;
		if(filename) CurrentFilename = (char *)estrdup(0,filename);
		else {
			if(fn3 && *fn3) CurrentFilename = (char *)estrdup(0,fn3);
			else if(!fn2) CurrentFilename = (char *)estrdup(0,fn);
			else CurrentFilename = (char *)estrdup(0,fn2);
		}
	} else {
#if APACHE
		ss = php_rqst->uri;
#else
		ss = getenv("PATH_INFO");
#endif
#ifdef PHP_DOCUMENT_ROOT
		if(!ss) Error("(4)Unable to open: <i>%s</i> - is your DOCUMENT_ROOT of %s set right?",filename?filename:"null",PHP_DOCUMENT_ROOT);
		else Error("(5)Unable to open: <i>%s</i> - is your DOCUMENT_ROOT of %s set right?",ss,PHP_DOCUMENT_ROOT);
#else
		if(!ss) Error("(6)Unable to open: <i>%s</i>",filename?filename:"null");
		else Error("(7)Unable to open: <i>%s</i>",ss);
#endif
	}
	return(fd);
}

/*
 * if cd = 1, then current directory will be changed to
 * directory portion of the passed path and the PATH_DIR
 * environment variable will be set
 */
char *FixFilename(char *filename, int cd, int *ret, int careful) {
	static char temp[1024];
	char path[1024];
	char fn[512], user[128], *s;
	struct passwd *pw=NULL;
	int st=0;
	char o='\0';
	int l=0;

	s = strrchr(filename,'/');
	if(s) {
		strncpy(fn,s+1,sizeof(fn));
		fn[sizeof(fn) - 1]='\0';
		o=*s;
		*s='\0';
		if(strlen(filename)==0) {
			strcpy(path,"/");
		} else {
			strncpy(path,filename,sizeof(path));
			path[sizeof(path) - 1]='\0';
		}
		*s=o;
	} else {
#ifdef PHP_ROOT_DIR
		strncpy(path,PHP_ROOT_DIR,sizeof(path));	
		path[sizeof(path)-1]='\0';
#else
		path[0] = '\0';
#endif
		strncpy(fn,filename,sizeof(fn));
		fn[sizeof(fn)-1]='\0';
	}
	if(fn && *fn=='~') {
		strncpy(path,fn,sizeof(path));
		path[sizeof(path)-1]='\0';
		fn[0]='\0';
	}
	if(*path) {
		if(*path=='~') {
			s = strchr(path,'/');
			if(s) {
				o=*s;
				*s='\0';
			}
			strcpy(user,path+1); /* This strcpy is safe, path size is known */
			if(s) {
				*s=o;		
				strcpy(temp,s);
			} else temp[0]='\0';
#ifdef HAVE_PWD_H
			if(*user) {
				pw = getpwnam(user);	
				if(pw) {
				  const char* pd = 0;
#ifdef PHP_PUB_DIRNAME_ENV
				  pd = getenv(PHP_PUB_DIRNAME_ENV);
#endif
				  if (pd == 0) pd = PHP_PUB_DIRNAME;
				  strcpy (path,pw->pw_dir);
				  strcat (path,"/");
				  strncat (path, pd, sizeof(path) - strlen(path) - 1);
				  strncat (path, temp, sizeof(path) - strlen(path) - 1);
				}
			}
#endif
		} else if(*path=='/' && *(path+1)=='~') {
			s = strchr(path+1,'/');
			if(s) {
				o=*s;
				*s='\0';
			}
			/* path contents is known to be safe here */
			strcpy(user,path+2);
			if(s) {
				*s=o;		
				/* s is derived from path and thus also safe */
				strcpy(temp,s);
			} else temp[0]='\0';
#if HAVE_PWD_H
			if(*user) {
				pw = getpwnam(user);	
				if(pw) {
				  const char* pd = 0;
#ifdef PHP_PUB_DIRNAME_ENV
				  pd = getenv(PHP_PUB_DIRNAME_ENV);
#endif
				  if (pd == 0) pd = PHP_PUB_DIRNAME;
				  sprintf(path,"%s/%s%s",pw->pw_dir,pd,temp);
				  strcpy(path,pw->pw_dir);
				  strcat(path,"/");
				  strncat(path, pd, sizeof(path) - strlen(path) - 1);
				  strncat(path, temp, sizeof (path) - strlen(path) - 1);
				}
			}
#endif
		} else if(*path=='/') {
#ifdef PHP_DOCUMENT_ROOT
			if(strncmp(path,PHP_DOCUMENT_ROOT,strlen(PHP_DOCUMENT_ROOT))) {
				strcpy(temp,PHP_DOCUMENT_ROOT);
				if(strlen(path)>1) strcat(temp,path);
				strncpy(path,temp,sizeof(path));
				path[sizeof(path)-1]='\0';	
			}
#endif
		}
		temp[0]='\0';
		if(cd) {
		strncpy(temp,path,sizeof(temp));
retry:
#if DEBUG
			Debug("ChDir to %s\n",temp);
#endif
			if(chdir(temp)<0) {
#if DEBUG
				Debug("%d [%s]",errno,strerror(errno));
#endif
				s = strrchr(temp,'/');
				if(s) {
					*s='\0';
					goto retry;
				}
			}
		}
		if(*fn) {
			strncpy(temp,path,sizeof(temp));
			strcat(temp,"/");
			strncat(temp,fn,sizeof(temp)-strlen(path)-1);
			temp[sizeof(temp)-1]='\0';
			st = stat(temp,&gsb);
			if((st!=-1) && (gsb.st_mode&S_IFMT)==S_IFDIR) {
				strncpy(temp,path,sizeof(temp));
				strcat(temp,"/");
				strncat(temp,fn,sizeof(temp)-strlen(path)-1);
				strcat(temp,"/index.html");
				temp[sizeof(temp)-1]='\0';
				st = stat(temp,&gsb);
				if(st==-1) {
					strncpy(temp,path,sizeof(temp));
					strcat(temp,"/");
					strncat(temp,fn,sizeof(temp)-strlen(path)-1);
					strcat(temp,"/index.phtml");
					temp[sizeof(temp)-1]='\0';
					st = stat(temp,&gsb);
				}
				strcat(path,"/");
				strncat(path,fn,sizeof(path));
				path[sizeof(path)-1]='\0';
			} else if(st==-1) {
				l = strlen(temp);
				if(strlen(fn)>4) {
					if(!strcasecmp(&temp[l-4],"html")) {
						temp[l-1]='\0';	 /* silly .html -> .htm hack */
						st = stat(temp,&gsb);
					}
				}
			}
		} else {
			st = stat(path,&gsb);
			if((st!=-1) && (gsb.st_mode&S_IFMT)==S_IFDIR) {
				/* path is safe, this sprintf is fine */
				sprintf(temp,"%s/index.html",path);
				st = stat(temp,&gsb);
				if(st==-1) {
					/* this one is safe too */
					sprintf(temp,"%s/index.phtml",path);
					st = stat(temp,&gsb);
				}
			} else strcpy(temp,path); /* ditto */
		}
	} else {
		st = stat(fn,&gsb);
		if((st!=-1) && (gsb.st_mode&S_IFMT)==S_IFDIR) {
			strncpy(temp,fn,sizeof(temp)-12);
			strcat(temp,"/index.html");
			st = stat(temp,&gsb);
			if(st==-1) {
				strncpy(temp,fn,sizeof(temp)-13);
				strcat(temp,"/index.phtml");
				st = stat(temp,&gsb);
			}
		} else {
			strncpy(temp,fn,sizeof(temp));
			temp[sizeof(temp)-1]='\0';
		}
	}		
	*ret=st;	
	return(temp);
}

char *GetCurrentFilename(void) {
	char *r;
	
	if(!CurrentFilename) return("");
	r = strrchr(CurrentFilename,'/');
	if(r) return(r+1);
	else return(CurrentFilename);
}


void SetCurrentFilename(char *filename) {
	if(filename) CurrentFilename = estrdup(0,filename);
	else CurrentFilename=NULL;
}

long GetCurrentFileSize(void) {
	return(CurrentFileSize);
}

void SetCurrentFileSize(long file_size) {
	CurrentFileSize = file_size;
}

char *getfilename(char *path, int ext) {
	static char filename[64];
	char *s;

	s = strrchr(path,'/');
	if(s) {
		strncpy(filename,s,sizeof(filename));
	} else {
		strncpy(filename,path,sizeof(filename));
	}

	if(!ext) {
		s = strrchr(filename,'.');
		if(s) *s='\0';
	}
	return(filename);
}	

void ClearStatCache(void) {
	if(CurrentStatFile) *CurrentStatFile=0;
}

void FileFunc(int type) {
	Stack *s;
	static struct stat sb;
	char temp[64];

	s = Pop();
	if(!s) {
		switch(type) {
		case 0:
			Error("Stack error in fileperms");
			break;
		case 1:
			Error("Stack error in fileinode");
			break;
		case 2:
			Error("Stack error in filesize");
			break;
		case 3:
			Error("Stack error in fileowner");
			break;
		case 4:
			Error("Stack error in filegroup");
			break;
		case 5:
			Error("Stack error in fileatime");
			break;
		case 6:
			Error("Stack error in filemtime");
			break;
		case 7:
			Error("Stack error in filectime");
			break;
		case 8:
			Error("Stack error in filetype");
			break;
		}
		return;
	}
#if APACHE
    if(!CurrentStatFile) {
        CurrentStatFile = estrdup(0,php_rqst->filename);
        CurrentStatLength = strlen(php_rqst->filename);
		if(stat(CurrentStatFile,&sb)==-1) {
			*CurrentStatFile=0;
			Push("-1",LNUMBER);
			return;
		}
    }
#endif
	if (!CurrentStatFile || strcmp(s->strval,CurrentStatFile)) {
		if (strlen(s->strval) > CurrentStatLength) {
			CurrentStatFile = estrdup(0,s->strval);
			CurrentStatLength = strlen(s->strval);
		} else {
			strcpy(CurrentStatFile,s->strval);
		}
		if(stat(CurrentStatFile,&sb)==-1) {
			*CurrentStatFile=0;
			Push("-1",LNUMBER);
			return;
		}
	}
	switch(type) {
	case 0: /* fileperms */
		sprintf(temp,"%ld",(long)sb.st_mode);
		Push(temp,LNUMBER);
		break;
	case 1: /* fileinode */
		sprintf(temp,"%ld",(long)sb.st_ino);
		Push(temp,LNUMBER);
		break;
	case 2: /* filesize  */
		sprintf(temp,"%ld",(long)sb.st_size);
		Push(temp,LNUMBER);
		break;
	case 3: /* fileowner */
		sprintf(temp,"%ld",(long)sb.st_uid);
		Push(temp,LNUMBER);
		break;
	case 4: /* filegroup */
		sprintf(temp,"%ld",(long)sb.st_gid);
		Push(temp,LNUMBER);
		break;
	case 5: /* fileatime */
		sprintf(temp,"%ld",(long)sb.st_atime);
		Push(temp,LNUMBER);
		break;
	case 6: /* filemtime */
		sprintf(temp,"%ld",(long)sb.st_mtime);
		Push(temp,LNUMBER);
		break;
	case 7: /* filectime */
		sprintf(temp,"%ld",(long)sb.st_ctime);
		Push(temp,LNUMBER);
		break;
	case 8: /* filetype */
		switch(sb.st_mode&S_IFMT) {
		case S_IFIFO:
			Push("fifo",STRING);
			break;
		case S_IFCHR:
			Push("char",STRING);
			break;
		case S_IFDIR:
			Push("dir",STRING);
			break;
		case S_IFBLK:
			Push("block",STRING);
			break;
		case S_IFREG:
			lstat(CurrentStatFile,&sb);
			if((sb.st_mode&S_IFMT) == S_IFLNK)
				Push("link",STRING);
			else
				Push("file",STRING);
			break;	
		}
		break;
	}
}

void TempNam(void) {
	Stack *s;
	char *d;
	char *t;
	char p[64];

	s = Pop();
	if(!s) {
		Error("Stack error in tempnam");
		return;
	}
	strncpy(p,s->strval,sizeof(p));

	s = Pop();
	if(!s) {
		Error("Stack error in tempnam");
		return;
	}
	d = (char *) estrdup(1,s->strval);
	
	t = tempnam(d,p);
	Push(t,STRING);
}

void Unlink(void) {
	Stack *s;
	
	s = Pop();
	if(!s) {
		Error("Stack error in unlink");
		return;
	}
	if(!s->strval || (s->strval && !*(s->strval))) {
		Error("Invalid filename in unlink");
		Push("-1", LNUMBER);
		return;
	}
#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,1)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be unlinked.");
		Push("-1", LNUMBER);
		return;	
	}
#endif
	if (unlink(s->strval) == 0) {
		Push("0", LNUMBER);
	}
	else {
		Error("Unlink failed (%s)", strerror(errno));
		Push("-1", LNUMBER);
	}
}

void ReadLink(void) {
	Stack *s;
	int ret;
	char buf[256];

	s = Pop();
	if(!s) {
		Error("Stack error in ReadLink");
		return;
	}
	if(!s->strval || (s->strval && !*(s->strval))) {
		Error("Invalid path in ReadLink");
		Push("-1", LNUMBER);
		return;
	}
	ret = readlink(s->strval, buf , 256);
	if(ret==-1) {
		Error("ReadLink failed (%s)", strerror(errno));
		Push("-1",LNUMBER);
	}
	else {
		/*
		 * Append NULL to the end of the string
		 */
		buf[ret] = '\0';
		Push(buf,STRING);
	}
}

void LinkInfo(void) {
	Stack *s;
	struct stat sb;	
	int ret;
	char temp[64];

	s = Pop();
	if(!s) {
		Error("Stack error in LinkInfo");
		return;
	}
	if(!s->strval || (s->strval && !*(s->strval))) {
		Error("Invalid path in LinkInfo");
		Push("-1", LNUMBER);
		return;
	}
	ret = lstat(s->strval,&sb);
	if(ret==-1) {
		Error("LinkInfo failed (%s)", strerror(errno));
		Push("-1",LNUMBER);
	}
	else
	{
		sprintf(temp,"%ld",(long)sb.st_dev);
		Push(temp,LNUMBER);
	}
}
 
void SymLink(void) {
#ifdef HAVE_SYMLINK
	Stack *s;
	char *new;
	int ret;
	char temp[4];

	s = Pop();
	if(!s) {
		Error("Stack error in symlink");
		return;
	}
	if(!s->strval || (s->strval && !*(s->strval))) {
		Error("Invalid filename in symlink");
		Push("-1", LNUMBER);
		return;
	}
	new = (char *) estrdup(1,s->strval);
	s = Pop();
	if(!s) {
		Error("Stack error in symlink");
		return;
	}
	if(!s->strval || (s->strval && !*(s->strval))) {
		Error("Invalid filename in symlink");
		Push("-1", LNUMBER);
		return;
	}

#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,2)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be linked.");
		Push("-1", LNUMBER);
		return;	
	}
#endif

	ret = symlink(s->strval, new);	
	if(ret==-1) {
		Error("SymLink failed (%s)", strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
#else
	Pop();
	Pop();
	Error("SymLink not available on this system");
	Push("0", LNUMBER);
#endif
}

void Link(void) {
#ifdef HAVE_LINK
	Stack *s;
	char *new;
	int ret;
	char temp[4];

	s = Pop();
	if(!s) {
		Error("Stack error in link");
		return;
	}
	if(!s->strval || (s->strval && !*(s->strval))) {
		Error("Invalid filename in link");
		Push("-1", LNUMBER);
		return;
	}
	new = (char *) estrdup(1,s->strval);
	s = Pop();
	if(!s) {
		Error("Stack error in link");
		return;
	}
	if(!s->strval || (s->strval && !*(s->strval))) {
		Error("Invalid filename in link");
		Push("-1", LNUMBER);
		return;
	}
#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,2)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be linked.");
		Push("-1", LNUMBER);
		return;	
	}
#endif
	ret = link(s->strval, new);	
	if(ret==-1) {
		Error("Link failed (%s)", strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
#else
	Pop();
	Pop();
	Error("Link not available on this system");
	Push("0", LNUMBER);
#endif
}

void Rename(void) {
	Stack *s;
	char *new;
	int ret;
	char temp[4];

	s = Pop();
	if(!s) {
		Error("Stack error in rename");
		return;
	}
	if(!s->strval || (s->strval && !*(s->strval))) {
		Error("Invalid filename in rename");
		Push("-1", LNUMBER);
		return;
	}
	new = (char *) estrdup(1,s->strval);
	s = Pop();
	if(!s) {
		Error("Stack error in rename");
		return;
	}
	if(!s->strval || (s->strval && !*(s->strval))) {
		Error("Invalid filename in rename");
		Push("-1", LNUMBER);
		return;
	}
#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,2)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be renamed.");
		Push("-1", LNUMBER);
		return;	
	}
#endif
	ret = rename(s->strval, new);	
	if(ret==-1) {
		Error("Rename failed (%s)", strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}

void Sleep(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in sleep");
		return;
	}
	sleep(s->intval);
}

void USleep(void) {
#ifdef HAVE_USLEEP
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in usleep");
		return;
	}
	usleep(s->intval);
#else
	Pop();
	Error("USleep not available on this system");
#endif
}

/*
 * Push a file pointer onto internal file identifier stack
 *
 * Arguments:
 *
 * FILE *fp - file pointer
 * char *fn - filename or host
 * int type - type of file pointer (needed for FpCloseAll)
 *            0 = regular file
 *            1 = socket
 *            2 = pipe
 */
int FpPush(FILE *fp, char *fn, int type) {
	FpStack *new = emalloc(0,sizeof(FpStack));

	new->fp = fp;
	new->filename = (char *)estrdup(0,fn);
	new->id = fileno(fp);
	new->type = type;
	new->next = fp_top;
	fp_top = new;
	return(new->id);
}

FILE *FpFind(int id) {
	FpStack *f;

	f = fp_top;
	while(f) {
		if(f->id == id) return(f->fp);
		f=f->next;
	}
	return(NULL);
}	

void FpDel(int id) {
	FpStack *e,*f;

	f = fp_top;
	e = f;
	while(f) {
		if(f->id == id) {
			if(f==fp_top) {
				fp_top=f->next;
				return;
			} else {
				e->next = f->next;
				return;
			}
		}
		e = f;
		f = f->next;
	}
}

void FpCloseAll(void) {
	FpStack *f;

	f = fp_top;
	while(f) {
		switch(f->type) {
		case 0:
		case 1:
			fclose(f->fp);
			break;
		case 2:
			pclose(f->fp);
			break;
		}	
		f = f->next;
	}
	fp_top = NULL;
}

void Fopen(void) {
	Stack *s;
	char temp[8];
	FILE *fp;
	int id;
	char *p;

	s = Pop();
	if(!s) {
		Error("Stack error in fopen");
		return;
	}
	if(!*(s->strval)) {
		Push("-1",LNUMBER);
		return;
	}
	p = estrdup(1,s->strval);
	s = Pop();
	if(!s) {
		Error("Stack error in fopen");
		return;
	}
	if(!*(s->strval)) {
		Push("-1",LNUMBER);
		return;
	}
#if DEBUG
	Debug("Opening [%s] with mode [%s]\n",s->strval,p);
#endif
	StripSlashes(s->strval);

#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,2)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be opened.");
		Push("-1",LNUMBER);
		return;
	}
#endif

	fp = fopen(s->strval,p);
	if(!fp) {
		Error("fopen(\"%s\",\"%s\") - %s",s->strval,p,strerror(errno));
		Push("-1",LNUMBER);
		return;
	}
	fgetss_state=0;
	id = FpPush(fp,s->strval,0);
	sprintf(temp,"%d",id);	
	Push(temp,LNUMBER);
}	

void Fclose(void) {
	Stack *s;
	int id;
	FILE *fp;

	s = Pop();
	if(!s) {
		Error("Stack error in fclose");
		return;
	}
	id = s->intval;

	fp = FpFind(id);
	if(!fp) {
		Error("Unable to find file identifier %d",id);
		Push("-1", LNUMBER);
		return;
	}
	fclose(fp);
	FpDel(id);
	Push("0", LNUMBER);
}

void Popen(void) {
	Stack *s;
	char temp[8];
	FILE *fp;
	int id;
	char *p;
#if PHP_SAFE_MODE
	char *b, buf[1024];
#endif

	s = Pop();
	if(!s) {
		Error("Stack error in popen");
		return;
	}
	if(!*(s->strval)) {
		Push("-1",LNUMBER);
		return;
	}
	p = estrdup(1,s->strval);
	s = Pop();
	if(!s) {
		Error("Stack error in popen");
		return;
	}
	if(!*(s->strval)) {
		Push("-1",LNUMBER);
		return;
	}
#if DEBUG
	Debug("Opening pipe to [%s] with mode [%s]\n",s->strval,p);
#endif
#if PHP_SAFE_MODE
	b = strrchr(s->strval,'/');
	if(b) {
		sprintf(buf,"%s%s",PHP_SAFE_MODE_EXEC_DIR,b);
	} else {
		sprintf(buf,"%s/%s",PHP_SAFE_MODE_EXEC_DIR,s->strval);
	}
	fp = popen(buf,p);
	if(!fp) {
		Error("popen(\"%s\",\"%s\") - %s",buf,p,strerror(errno));
		Push("-1",LNUMBER);
		return;
	}
#else
	fp = popen(s->strval,p);
	if(!fp) {
		Error("popen(\"%s\",\"%s\") - %s",s->strval,p,strerror(errno));
		Push("-1",LNUMBER);
		return;
	}
#endif
	id = FpPush(fp,s->strval,2);
	sprintf(temp,"%d",id);	
	Push(temp,LNUMBER);
}

void Pclose(void) {
	Stack *s;
	int id,ret;
	char temp[8];
	FILE *fp;

	s = Pop();
	if(!s) {
		Error("Stack error in pclose");
		return;
	}
	id = s->intval;

	fp = FpFind(id);
	if(!fp) {
		Error("Unable to find file identifier %d",id);
		Push("-1", LNUMBER);
		return;
	}
	ret=pclose(fp);
	FpDel(id);
	sprintf(temp,"%d",ret);	
	Push(temp, LNUMBER);
}

void Feof(void) {
	Stack *s;
	FILE *fp;
	int id;

	s = Pop();
	if(!s) {
		Error("Stack error in feof");
		return;
	}
	id = s->intval;
	fp = FpFind(id);
	if(!fp) {
		Error("Unable to find file identifier %d",id);
		Push("-1", LNUMBER);
		return;
	}
	if(feof(fp))
		Push("1",LNUMBER);
	else
		Push("0",LNUMBER);
}

void Fgets(void) {
	Stack *s;
	FILE *fp;
	int id;
	int len;
	char *buf;

	s = Pop();
	if(!s) {
		Error("Stack error in fgets");
		return;
	}
	len = s->intval;

	s = Pop();
	if(!s) {
		Error("Stack error in fgets");
		return;
	}
	id = s->intval;

	fp = FpFind(id);
	if(!fp) {
		Error("Unable to find file identifier %d",id);
		Push("", STRING);
		return;
	}
	buf = emalloc(1,sizeof(char) * (len + 1));
	if(!fgets(buf,len,fp)) {
		Push("",STRING);
		return;
	}
	Push((buf=AddSlashes(buf,1)),STRING);
}

/* Strip any HTML tags while reading */
void Fgetss(void) {
	Stack *s;
	FILE *fp;
	int id;
	int len, br;
	char *buf, *p, *rbuf, *rp, c, lc;

	s = Pop();
	if(!s) {
		Error("Stack error in fgetss");
		return;
	}
	len = s->intval;

	s = Pop();
	if(!s) {
		Error("Stack error in fgetss");
		return;
	}
	id = s->intval;

	fp = FpFind(id);
	if(!fp) {
		Error("Unable to find file identifier %d",id);
		Push("", STRING);
		return;
	}
	buf = emalloc(1,sizeof(char) * (len + 1));
	if(!fgets(buf,len,fp)) {
		Push("",STRING);
		return;
	}
	rbuf=estrdup(1,buf);
	c = *buf;
	lc=(char)0;
	p = buf;
	rp=rbuf;
	br=0;
	while(c) {
		switch(c) {
		case '<':
			if(fgetss_state==0) {
				lc='<';
				fgetss_state=1;
			}
			break;
		case '(':
			if(fgetss_state==2) {
				if(lc!='\"') {
					lc='(';
					br++;
				}
			} else if(fgetss_state==0) {
				*(rp++) = c;
			}
			break;	
		case ')':
			if(fgetss_state==2) {
				if(lc!='\"') {
					lc=')';
					br--;
				}
			} else if(fgetss_state==0) {
				*(rp++) = c;
			}
			break;	
		case '>':
			if(fgetss_state==1) {
				lc='>';
				fgetss_state=0;
			} else if(fgetss_state==2) {
				if(!br && lc!='\"') fgetss_state=0;
			}
			break;
		case '\"':
			if(fgetss_state==2) {
				if(lc=='\"') lc=(char)0;
				else if(lc!='\\') lc='\"';
			} else if(fgetss_state==0) {
				*(rp++) = c;
			}
			break;
		case '?':
			if(fgetss_state==1) {
				br=0;
				fgetss_state=2;
				break;
			}
			/* fall-through */
		default:
			if(fgetss_state==0) {
				*(rp++) = c;
			}	
		}
		c=*(++p);
	}	
	*rp='\0';
	Push((AddSlashes(rbuf,1)),STRING);
}

void Fputs(void) {
	Stack *s;
	FILE *fp;
	int ret,id;
	char *buf;
	char temp[8];

	s = Pop();
	if(!s) {
		Error("Stack error in fputs");
		return;
	}
	if(!*s->strval) {
		return;
	}
	buf = (char *)estrdup(1,s->strval);
	s = Pop();
	if(!s) {
		Error("Stack error in fputs");
		return;
	}
	id = s->intval;	
	fp = FpFind(id);
	if(!fp) {
		Error("Unable to find file identifier %d",id);
		Push("", STRING);
		return;
	}
	ParseEscapes(buf);
	StripSlashes(buf);
	ret = fputs(buf,fp);
	sprintf(temp,"%d",ret);
	Push(temp,STRING);
}	

void Rewind(void) {
	Stack *s;
	int id;
	FILE *fp;

	s = Pop();
	if(!s) {
		Error("Stack error in rewind");
		return;
	}
	id = s->intval;	
	fp = FpFind(id);
	if(!fp) {
		Error("Unable to find file identifier %d",id);
		Push("-1", LNUMBER);
		return;
	}
	rewind(fp);
	Push("0", LNUMBER);
}

void Ftell(void) {
	Stack *s;
	int id;
	char temp[16];
	long pos;
	FILE *fp;

	s = Pop();
	if(!s) {
		Error("Stack error in ftell");
		return;
	}
	id = s->intval;	
	fp = FpFind(id);
	if(!fp) {
		Error("Unable to find file identifier %d",id);
		Push("-1", LNUMBER);
		return;
	}
	pos = ftell(fp);
	sprintf(temp,"%ld",pos);
	Push(temp,LNUMBER);
}

void Fseek(void) {
	Stack *s;
	int ret,id;
	long pos;
	char temp[8];
	FILE *fp;

	s = Pop();
	if(!s) {
		Error("Stack error in fseek");
		return;
	}
	pos = s->intval;
	s = Pop();
	if(!s) {
		Error("Stack error in fseek");
		return;
	}
	id = s->intval;
	fp = FpFind(id);
	if(!fp) {
		Error("Unable to find file identifier %d",id);
		Push("-1", LNUMBER);
		return;
	}
	ret = fseek(fp,pos,SEEK_SET);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}

char *GetCurrentPI(void) {
	return(CurrentPI);
}

void SetCurrentPI(char *pi) {
	if(pi)
		CurrentPI = estrdup(0,pi);
	else 
		CurrentPI = NULL;
}

char *GetIncludePath(void) {
    return(IncludePath);
}

void SetIncludePath(char *path) {
	if (path)
		IncludePath = estrdup(0, path);
	else
		IncludePath = NULL;
}

char *GetAutoPrependFile(void) {
    return(AutoPrependFile);
}

char *GetAutoAppendFile(void) {
    return(AutoAppendFile);
}

#if APACHE
void SetCurrentPD(char *pd) {
	char *s;
	int l=0;
	char *env;

#ifdef PHP_ROOT_DIR
	l = sizeof(char)*(strlen(pd) + strlen(PHP_ROOT_DIR) + 2);
	env = emalloc(0,l);
#else
	l = sizeof(char)*(strlen(pd)+2);
	env = emalloc(0,l);
#endif
	s = strrchr(pd,'/');
#ifdef PHP_ROOT_DIR
	if(!s) strncpy(env,PHP_ROOT_DIR,l);
#else
	if(!s) strcpy(env,"/");
#endif
	else {
		*s='\0';
#ifdef PHP_ROOT_DIR
		strcpy(env,PHP_ROOT_DIR);
		strncat(env,pd,l);
#else
		strncpy(env,pd,l);
#endif
		*s='/';
	}
#if DEBUG
	Debug("Setting PATH_DIR to %s\n",env);
#endif
	if(pd)
		table_set(php_rqst->subprocess_env,"PATH_DIR",env);
}
#endif

void ChMod(void) {
	Stack *s;
	int ret,mode;
	char temp[8];

	OctDec();	
	s = Pop();
	if(!s) {
		Error("Stack error in chmod()");
		return;
	}
	mode = s->intval;
	s = Pop();
	if(!s) {
		Error("Stack error in chmod()");
		return;
	}
#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,1)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be changed.");
		Push("-1",LNUMBER);
		return;
	}
#endif
	ret = chmod(s->strval,mode);
	if (ret < 0) {
		Error("ChMod failed (%s)", strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}	

void ChOwn(void) {
#ifndef WIN32
	Stack *s;
	int ret;
	char temp[8];
	struct passwd *pw=NULL;

	s = Pop();
	if(!s) {
		Error("Stack error in chown()");
		return;
	}
	pw = getpwnam(s->strval);
	if(!pw) {
		Error("Unable to find entry for %s in passwd file",s->strval);
		Push("-1", LNUMBER);
		return;
	}
	s = Pop();	
	if(!s) {
		Error("Stack error in chown()");
		return;
	}
#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,1)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be changed.");
		Push("-1",LNUMBER);
		return;
	}
#endif
	ret = chown(s->strval,pw->pw_uid,-1);
	if (ret < 0) {
		Error("ChOwn failed (%s)", strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
#else
	Error("ChOwn not available under win32!");
#endif
}	

void ChGrp(void) {
#ifndef WIN32
	Stack *s;
	int ret;
	char temp[8];
	struct group *gr=NULL;

	s = Pop();
	if(!s) {
		Error("Stack error in chown()");
		return;
	}
	if(s->intval != -1) gr = getgrnam(s->strval);
	if(!gr) {
		Error("Unable to find entry for %s in groups file",s->strval);
		Push("-1", LNUMBER);
		return;
	}
	s = Pop();	
	if(!s) {
		Error("Stack error in chown()");
		return;
	}
#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,1)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be changed.");
		Push("-1",LNUMBER);
		return;
	}
#endif
	ret = chown(s->strval,-1,gr->gr_gid);
	if (ret < 0) {
		Error("ChGrp failed (%s)", strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
#else
	Error("ChGrp not available under win32!");
#endif
}

void MkDir(void) {
	Stack *s;
	int ret,mode;
	char temp[8];

	OctDec();	
	s = Pop();
	if(!s) {
		Error("Stack error in mkdir()");
		return;
	}
	mode = s->intval;
	s = Pop();
	if(!s) {
		Error("Stack error in mkdir()");
		return;
	}
	ret = mkdir(s->strval,mode);
	if (ret < 0) {
		Error("MkDir failed (%s)", strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}	

void RmDir(void) {
	Stack *s;
	int ret;
	char temp[8];

	s = Pop();
	if(!s) {
		Error("Stack error in rmdir()");
		return;
	}
#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,1)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of directory to be removed.");
		Push("-1",LNUMBER);
		return;
	}
#endif
	ret = rmdir(s->strval);
	if (ret < 0) {
		Error("RmDir failed (%s)", strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}	

void PHPFile(void) {
	Stack *s;
	FILE *fp;
	char buf[8192];
	VarTree *var;
	int l,t;

	s = Pop();
	if(!s) {
		Error("Stack error in file");
		return;
	}
	if(!*(s->strval)) {
		Push("-1",LNUMBER);
		return;
	}

#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,1)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be read.");
		Push("-1",LNUMBER);
		return;
	}
#endif
		
	fp = fopen(s->strval,"r");
	if(!fp) {
		Error("file(\"%s\") - %s",s->strval,strerror(errno));
		Push("-1",LNUMBER);
		return;
	}
	var = GetVar("__filetmp__",NULL,0);
	if(var) deletearray(var);
	while(fgets(buf,8191,fp)) {
#if DEBUG
		Debug("File() read line \"%s\"\n",buf);
#endif
		l = strlen(buf);
		t = l;
		while(l>0 && isspace(buf[--l])); 
		if(l<t) buf[l+1]='\0';	
		Push(AddSlashes(buf,0),STRING);
		SetVar("__filetmp__",1,0);
	}
	Push("__filetmp__",VAR);
	fclose(fp);
}	

void set_path_dir(char *pi) {
#ifndef APACHE
	char *buf = malloc(sizeof(char) * (strlen(pi)+12));
#endif
#ifdef PHP_ROOT_DIR
	char *env = emalloc(0,sizeof(char) * (strlen(pi) + strlen(PHP_ROOT_DIR) + 2));
#else
	char *env = emalloc(0,sizeof(char) * (strlen(pi)+2));
#endif
	char *s;

	s = strrchr(pi,'/');
#ifdef PHP_ROOT_DIR
	if(!s) strcpy(env,PHP_ROOT_DIR);
#else
	if(!s) strcpy(env,"/");
#endif
	else {
		*s='\0';
#ifdef PHP_ROOT_DIR
		sprintf(env,"%s%s",PHP_ROOT_DIR,pi);
#else
		strcpy(env,pi);
#endif
		*s='/';
	}
#if APACHE
#if DEBUG
	Debug("1. Setting PATH_DIR to %s\n",env);
#endif
	table_set(php_rqst->subprocess_env,"PATH_DIR",env);
#else
	sprintf(buf,"PATH_DIR=%s",env);
	putenv(buf);
#endif
}

/* This function is equivilent to <!--#include virtual...-->
 * in mod_include. It does an Apache sub-request. It is useful
 * for including CGI scripts or .shtml files, or anything else
 * that you'd parse through Apache (for .phtml files, you'd probably
 * want to use <?Include>. This only works when PHP is compiled
 * as an Apache module, since it uses the Apache API for doing
 * sub requests.
 */

#if APACHE
void Virtual(void) {
	Stack *s;
	char *file;
	request_rec *rr = NULL;

	s = Pop();
	if (!s) {
		Error("Stack error in Virtual");
		return;
	}

	file = s->strval;

	if (!(rr = sub_req_lookup_uri (file, php_rqst))) {
		Error("Unable to include file: %s", file);
		if (rr)
			destroy_sub_req (rr);
		Push("-1", LNUMBER);
		return;
	}

	if (rr->status != 200) {
		Error("Unable to include file: %s", file);
		if (rr)
			destroy_sub_req (rr);
		Push("-1", LNUMBER);
		return;
	}

	/* Cannot include another PHP file because of global conflicts */
	if (rr->content_type &&
		!strcmp(rr->content_type, "application/x-httpd-php")) {
		Error("Cannot include a PHP/FI file "
			  "(use <code>&lt;?include \"%s\"&gt;</code> instead)", file);
		if (rr)
			destroy_sub_req (rr);
		Push("-1", LNUMBER);
		return;
	}

	if (run_sub_req(rr)) {
		Error("Unable to include file: %s", file);
		Push("-1", LNUMBER);
	}
	else {
		Push("0", LNUMBER);
	}

	if (rr)
		destroy_sub_req (rr);
}
#endif

/*
 * Read a file and write the ouput to stdout
 */
void ReadFile(void) {
	Stack *s;
	char buf[8192],temp[8];
	FILE *fp;
	int b,i, size;

	s = Pop();
	if(!s) {
		Error("Stack error in ReadFile");
		return;
	}
	if(!*(s->strval)) {
		Push("-1",LNUMBER);
		return;
	}
#if DEBUG
	Debug("Opening [%s]\n",s->strval);
#endif
	StripSlashes(s->strval);

#if PHP_SAFE_MODE
	if(!CheckUid(s->strval,1)) {
		Error("SAFE MODE Restriction in effect.  Invalid owner of file to be read.");
		Push("-1",LNUMBER);
		return;
	}
#endif
	fp = fopen(s->strval,"r");
	if(!fp) {
		Error("ReadFile(\"%s\") - %s",s->strval,strerror(errno));
		Push("-1",LNUMBER);
		return;
	}
	size= 0;
	php_header(0,NULL);
	while((b = fread(buf, 1, sizeof(buf), fp)) > 0) {
		for(i = 0; i < b; i++)
			PUTC(buf [i]);
		size += b ;
	}
	fclose(fp);
	sprintf(temp,"%d",size);	
	Push(temp,LNUMBER);
}	

/*
 * Return or change the umask.
 */
void FileUmask(int args) {
    char buf[16];
    int oldumask;
	Stack *s;

    oldumask = umask(077);

	if (args == 0) {
		umask(oldumask);
    }
    else {
		OctDec();
		s = Pop();
		if (!s) {
			umask(oldumask); /* In case of errors the umask should
							  * be changed back. */
			Error("Stack error in Umask");
			return;
		}
		umask(s->intval);
    }

    sprintf(buf, "%o", oldumask);
	Push(buf, LNUMBER);
}

/*
 * Read to EOF on a file descriptor and write the output to stdout.
 */
void FPassThru(void) {
	Stack *s;
	FILE *fp;
	char buf[8192], temp[8];
	int id, size, b, i;

	s = Pop();
	if (!s) {
		Error("Stack error in FPassThru");
		Push("-1", LNUMBER);
		return;
	}
	id = s->intval;
	fp = FpFind(id);
	if (!fp) {
		Error("Unable to find file identifier %d",id);
		Push("-1", LNUMBER);
		return;
	}
	
	size = 0;
	php_header(0,NULL);
	while((b = fread(buf, 1, sizeof(buf), fp)) > 0) {
		for(i = 0; i < b; i++)
			PUTC(buf [i]);
		size += b ;
	}
	fclose(fp);
	sprintf(temp,"%d",size);	
	Push(temp,LNUMBER);
}

/*
 * CheckUid
 *
 * This function has three modes:
 * 
 * 0 - return invalid (0) if file does not exist
 * 1 - return valid (1)  if file does not exist
 * 2 - if file does not exist, check directory
 */
int CheckUid(char *fn, int mode) {
	struct stat sb;
	int ret;
	long uid=0L, duid=0L;
	char *s;

	if(!fn) return(0); /* path must be provided */

	ret = stat(fn,&sb);
	if(ret<0 && mode < 2) return(mode);
	if(ret>-1) {
		uid=sb.st_uid;
		if(uid==getmyuid()) return(1);
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
		s=emalloc(1,MAXPATHLEN+1);
		if(!getcwd(s,MAXPATHLEN)) return(0);
		ret = stat(s,&sb);
		if(ret<0) return(0);
		duid = sb.st_uid;
	}
	if(duid == getmyuid()) return(1);
	else return(0);
}

/*
 * Local variables:
 * tab-width: 4
 * End:
 */
