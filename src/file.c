/***[file.c]******************************************************[TAB=4]****\
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
/* $Id: file.c,v 1.29 1996/09/04 02:16:51 rasmus Exp $ */
#include "php.h"
#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include "regexpr.h"
#include "parse.h"
#include <ctype.h>
#if APACHE
#include "http_protocol.h"
#include "http_request.h"
#endif

static char *CurrentFilename=NULL;
static char *CurrentStatFile=NULL;
static char *CurrentPI=NULL;
static long CurrentFileSize=0L;
static struct stat gsb;
static int fgetss_state=0;

static FpStack *fp_top = NULL;

void php_init_file(void) {
	CurrentFilename=NULL;
	CurrentStatFile=NULL;
	CurrentPI=NULL;
	CurrentFileSize=0L;
	fp_top = NULL;
	fgetss_state=0;
}

void StripLastSlash(char *str) {
	char *s;

	s = strrchr(str,'/');
	if(s) {
		*s='\0';
	}
}

int OpenFile(char *filename, int top, long *file_size) {
	char *fn, *pi, *sn, *fn2=NULL, *fn3=NULL;
	char *s=NULL, *ss=NULL;
	int ret=-1;
	int no_httpd=0, include=0;
	int fd;
#ifdef PATTERN_RESTRICT
	int pret;
	char *cp;
	struct re_pattern_buffer exp;
	struct re_registers regs;
	char fastmap[256];
#endif
#ifdef PHP_ROOT_DIR
	char temp[1024];
#endif

#if DEBUG	
	Debug("OpenFile called with %s\n",filename?filename:"null");
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
		fn = getenv("PATH_TRANSLATED");
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
		if(top) no_httpd=1;
		else include=1;
	}
	/*
	 * To prevent someone from uploading a file and then running it through php
	 */
	if(!strncmp(fn,"phpfi",5)) {
		Error("You are explicitly not allowed to open any files that begin with &quot;phpfi&quot; for security reasons");
		return(-1);
	}
#ifdef PATTERN_RESTRICT
	if(fn[strlen(fn)-1]!='/') {
		exp.allocated = 0;
		exp.buffer = 0;
		exp.translate = NULL;
		exp.fastmap = fastmap;
		cp = php_re_compile_pattern(PATTERN_RESTRICT,strlen(PATTERN_RESTRICT),&exp);
		if(cp) {
			Error("Regular Expression error in PATTERN_RESTRICT: %s",cp);
			return(-1);
		}
		php_re_compile_fastmap(&exp);
#if DEBUG
		Debug("Checking pattern restriction: \"%s\" against \"%s\"\n",PATTERN_RESTRICT,fn);
#endif
		if((pret = php_re_match(&exp,fn,strlen(fn),0,&regs))<0) {
			Error("Sorry, you are not permitted to load that file through PHP/FI.",pret);
			return(-1);
		}
	}
#endif

	fn = (char *)estrdup(1,FixFilename(fn,top,&ret));
	*file_size = (long)gsb.st_size;
	CurrentFileSize = (long)gsb.st_size;
#if DEBUG
	Debug("OpenFile: fn = [%s]\n",fn);
#endif
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
			fn = emalloc(1,sizeof(char) * (strlen(pi)+strlen(sn)+1));
			strcpy(fn,sn);
			s = strrchr(fn,'/');
			if(s) *s='\0';
			strcat(fn,pi);
			fn2 = (char *) estrdup(1,FixFilename(fn,1,&ret));
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
			fd = open(fn3,O_RDONLY);
			if(fd==-1) {
				Error("Unable to open <i>%s</i>",fn3);
				return(-1);
			}
		} else if(!fn2) {
			fd = open(fn,O_RDONLY);
			if(fd==-1) {
				Error("Unable to open <i>%s</i>",fn);
				return(-1);
			}
		} else {
			fd = open(fn2,O_RDONLY);
			if(fd==-1) {
				Error("Unable to open <i>%s</i>",fn2);
				return(-1);
			}
		}
		if(top) SetStatInfo(&gsb);
		
#if ACCESS_CONTROL
		if(!no_httpd && !include) {
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
		Error("Unable to open: <i>%s</i>",ss?ss:"null");
	}
#if DEBUG
	if(fd>0) Debug("CurrentFilename is [%s]\n",CurrentFilename);
#endif
	return(fd);
}

/*
 * if cd = 1, then current directory will be changed to
 * directory portion of the passed path and the PATH_DIR
 * environment variable will be set
 */
char *FixFilename(char *filename, int cd, int *ret) {
	static char temp[1024];
	char path[1024];
	char fn[128], user[128], *s;
	struct passwd *pw=NULL;
	int st=0;
	char o='\0';
	int l=0;

	s = strrchr(filename,'/');
	if(s) {
		strcpy(fn,s+1);
		o=*s;
		*s='\0';
		strcpy(path,filename);
		*s=o;
	} else {
#ifdef PHP_ROOT_DIR
		strcpy(path,PHP_ROOT_DIR);	
#else
		path[0] = '\0';
#endif
		strcpy(fn,filename);
	}
	if(fn && *fn=='~') {
		strcpy(path,fn);
		fn[0]='\0';
	}
	if(*path) {
		if(*path=='~') {
			s = strchr(path,'/');
			if(s) {
				o=*s;
				*s='\0';
			}
			strcpy(user,path+1);
			if(s) {
				*s=o;		
				strcpy(temp,s);
			} else temp[0]='\0';
			if(*user) {
				pw = getpwnam(user);	
				if(pw) sprintf(path,"%s/%s%s",pw->pw_dir,PHP_PUB_DIRNAME,temp);
			}
		} else if(*path=='/' && *(path+1)=='~') {
			s = strchr(path+1,'/');
			if(s) {
				o=*s;
				*s='\0';
			}
			strcpy(user,path+2);
			if(s) {
				*s=o;		
				strcpy(temp,s);
			} else temp[0]='\0';
			if(*user) {
				pw = getpwnam(user);	
				if(pw) sprintf(path,"%s/%s%s",pw->pw_dir,PHP_PUB_DIRNAME,temp);
			}
		}
		temp[0]='\0';
		if(cd) {
			if(chdir(path)<0) {
#if DEBUG
				Debug("%d [%s]",errno,strerror(errno));
#endif
			}
		}
		if(*fn) {
			sprintf(temp,"%s/%s",path,fn);
			st = stat(temp,&gsb);
			if((st!=-1) && (gsb.st_mode&S_IFMT)==S_IFDIR) {
				sprintf(temp,"%s/%s/index.html",path,fn);
				st = stat(temp,&gsb);
				if(st==-1) {
					sprintf(temp,"%s/%s/index.phtml",path,fn);
					st = stat(temp,&gsb);
				}
				sprintf(path,"%s/%s",path,fn);
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
				sprintf(temp,"%s/index.html",path);
				st = stat(temp,&gsb);
				if(st==-1) {
					sprintf(temp,"%s/index.phtml",path);
					st = stat(temp,&gsb);
				}
			} else strcpy(temp,path);
		}
	} else {
		st = stat(fn,&gsb);
		if((st!=-1) && (gsb.st_mode&S_IFMT)==S_IFDIR) {
			sprintf(temp,"%s/index.html",fn);
			st = stat(temp,&gsb);
			if(st==-1) {
				sprintf(temp,"%s/index.phtml",fn);
				st = stat(temp,&gsb);
			}
		} else strcpy(temp,fn);
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
#if DEBUG
	Debug("Setting CurrentFilename to [%s]\n",filename);
#endif
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
		strcpy(filename,s);
	} else {
		strcpy(filename,path);
	}

	if(!ext) {
		s = strrchr(filename,'.');
		if(s) *s='\0';
	}
	return(filename);
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
		}
		return;
	}
#if APACHE
	if(!CurrentStatFile) CurrentStatFile = estrdup(0,php_rqst->filename);
#endif
	if(!CurrentStatFile || (CurrentStatFile && strcmp(s->strval,CurrentStatFile))) {
		CurrentStatFile = estrdup(0,s->strval);
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
	}
}

void TempNam(void) {
	Stack *s;
	char *d;
	char *t;
	char p[32];

	s = Pop();
	if(!s) {
		Error("Stack error in tmpname");
		return;
	}
	strncpy(p,s->strval,31);

	s = Pop();
	if(!s) {
		Error("Stack error in tmpname");
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
		return;
	}
	unlink(s->strval);
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
		return;
	}
	ret = readlink(s->strval, buf , 256);
	if(ret==-1) 
		Push("-1",LNUMBER);
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
		return;
	}
	ret = lstat(s->strval,&sb);
	if(ret==-1) 
		Push("-1",LNUMBER);
	else
	{
		sprintf(temp,"%ld",(long)sb.st_dev);
		Push(temp,LNUMBER);
	}
}
 
void SymLink(void) {
#if HAVE_SYMLINK
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
		return;
	}
	ret = symlink(s->strval, new);	
	if(ret==-1) {
		Error("%d [%s]",errno, strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
#else
	Pop();
	Pop();
	Error("SymLink not available on this system");
#endif
}

void Link(void) {
#if HAVE_LINK
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
		return;
	}
	ret = link(s->strval, new);	
	if(ret==-1) {
		Error("%d [%s]",errno, strerror(errno));
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
#else
	Pop();
	Pop();
	Error("Link not available on this system");
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
		return;
	}
	ret = rename(s->strval, new);	
	if(ret==-1) {
		Error("%d [%s]",errno, strerror(errno));
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
#if HAVE_USLEEP
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
		return;
	}
	fclose(fp);
	FpDel(id);
}

void Popen(void) {
	Stack *s;
	char temp[8];
	FILE *fp;
	int id;
	char *p;

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
	fp = popen(s->strval,p);
	if(!fp) {
		Error("popen(\"%s\",\"%s\") - %s",s->strval,p,strerror(errno));
		Push("-1",LNUMBER);
		return;
	}
	id = FpPush(fp,s->strval,2);
	sprintf(temp,"%d",id);	
	Push(temp,LNUMBER);
}

void Pclose(void) {
	Stack *s;
	int id;
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
		return;
	}
	pclose(fp);
	FpDel(id);
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
		return;
	}
	if(feof(fp)) Push("1",LNUMBER);
	else Push("0",LNUMBER);
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
		return;
	}
	rewind(fp);
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

#if APACHE
void SetCurrentPD(char *pd) {
#if DEBUG
	Debug("Setting PATH_DIR to %s\n",pd);
#endif
	if(pd)
		table_set(php_rqst->subprocess_env,"PATH_DIR",pd);
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
	ret = chmod(s->strval,mode);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}	

void ChOwn(void) {
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
		return;
	}
	s = Pop();	
	if(!s) {
		Error("Stack error in chown()");
		return;
	}
	ret = chown(s->strval,pw->pw_uid,-1);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}	

void ChGrp(void) {
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
		return;
	}
	s = Pop();	
	if(!s) {
		Error("Stack error in chown()");
		return;
	}
	ret = chown(s->strval,-1,gr->gr_gid);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
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
	ret = rmdir(s->strval);
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
}	

void File(void) {
	Stack *s;
	FILE *fp;
	char buf[2048];
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
	fp = fopen(s->strval,"r");
	if(!fp) {
		Error("file(\"%s\") - %s",s->strval,strerror(errno));
		Push("-1",LNUMBER);
		return;
	}
	var = GetVar("__filetmp__",NULL,0);
	if(var) deletearray(var);
	while(fgets(buf,2047,fp)) {
		l = strlen(buf);
		t = l;
		while(isspace(buf[--l])); 
		if(l<t) buf[l+1]='\0';	
		Push(AddSlashes(buf,0),STRING);
		SetVar("__filetmp__",1,0);
	}
	Push("__filetmp__",VAR);
	fclose(fp);
}	

void set_path_dir(char *pi) {
#ifndef APACHE
	char *buf = emalloc(0,sizeof(char) * (strlen(pi)+12));
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
	Debug("Setting PATH_DIR to %s\n",env);
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
		if (rr) destroy_sub_req (rr);
		return;
	}

	if (rr->status != 200) {
		Error("Unable to include file: %s", file);
		if (rr) destroy_sub_req (rr);
		return;
	}

	/* Cannot include another PHP file because of global conflicts */
	if (rr->content_type &&
		!strcmp(rr->content_type, "application/x-httpd-php")) {
		Error("Cannot include a PHP/FI file "
			"(use <code>&lt;?include \"%s\"&gt;</code> instead)", file);
		if (rr) destroy_sub_req (rr);
		return;
	}

	if (run_sub_req(rr))
		Error("Unable to include file: %s", file);

	if (rr) destroy_sub_req (rr);
}
#endif
