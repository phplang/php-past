/***[acc.c]*******************************************************[TAB=4]****\
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
/* $Id: acc.c,v 1.28 1997/08/16 03:51:37 rasmus Exp $ */
#include "php.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif
#include <ctype.h>
#include <errno.h>
#include "parse.h"
#if APACHE
#include "http_protocol.h"
#endif

static char *RemoteHostName=NULL;
static char *EmailAddr=NULL;
static char *RefDoc=NULL;
static char *Browser=NULL;
#if defined(LOGGING) || defined(MSQLLOGGING) || defined(MYSQLLOGGING)
static int Logging=1;
#endif
static int ShowInfo=1;


static AccessInfo *top=NULL;
static char *AccessDir=NULL;

#if APACHE
void php_init_acc(php_module_conf *conf) {
#else
void php_init_acc(void) {
#endif
	RemoteHostName=NULL;
	EmailAddr=NULL;
	RefDoc=NULL;
	Browser=NULL;
#if defined(LOGGING) || defined(MSQLLOGGING) || defined(MYSQLLOGGING)
#if APACHE
	Logging=conf->Logging;
#else
	Logging=1;
#endif
#endif
#if APACHE
	ShowInfo=conf->ShowInfo;
#else
	ShowInfo=1;
#endif
	top=NULL;
#if APACHE
	if(conf->AccessDir) AccessDir = estrdup(0,conf->AccessDir);
#if ACCESS_CONTROL
	else AccessDir = estrdup(0,ACCESS_DIR);
#else
	else AccessDir = NULL;
#endif
#else
#if ACCESS_CONTROL
	AccessDir = estrdup(0,ACCESS_DIR);
#else
	AccessDir = NULL;
#endif
#endif
}

#if ACCESS_CONTROL

AccessInfo *StrtoAccess(char *str) {
	AccessInfo *new, *last;
	char *s, *password=NULL;
	int state;
	int def=0, mode=0;

	while(top) {
		new = top->next;
		top = new;
	}
	last = NULL;
	top = NULL;
	s = strtok(str,"\033");
	state=0;
	while(s) {
		if(state==1 && *s>'Z') state=3;
		switch(state) {
		case 0: /* Get default mode */
			if(!strcmp(s,"pri")) def=0;
			else def=1; 
			state=1;
			break;
		case 1: /* Get mode bits */
			mode=0;
			if(strchr(s,'A')) mode |= 2;
			if(strchr(s,'B')) mode |= 4;
			if(strchr(s,'L')) mode |= 32;
			if(strchr(s,'E')) mode |= 1;
			if(strchr(s,'N')) mode |= 16;
			if(strchr(s,'S')) mode |= 64;
			if(strchr(s,'H')) mode |= 128;
			if(strchr(s,'P')) { 
				mode |= 8; 
				state=2; 
			}
			if(state==1) state=3;
			break;
		case 2: /* Get password */
			if(s && *s) {
				password = (char *) estrdup(0,s);
			} else {
				password = emalloc(0,sizeof(char));
				*password='\0';
			}
			state = 3;
			break;
		case 3: /* Get pattern */
			new = emalloc(0,sizeof(AccessInfo));
			new->password = password;
			new->mode = mode;
			new->def = def;
			new->next = NULL;	
			if(!strncmp(s,"dom",3)) new->type = 0;
			else if(!strncmp(s,"mai",3)) new->type = 1;
			else if(!strncmp(s,"ref",3)) new->type = 2;
			else if(!strncmp(s,"bro",3)) new->type = 3;
			else new->type = 0;
			new->patt = (char *)estrdup(0,&s[3]);
			StripSlashes(new->patt);
			if(!top) {
				top=new;
			} else {
				last->next = new;
			}
			last = new;
			state=1;
			break;
		}
		s = strtok(NULL,"\033");
	}
	return(top);
}

void AddRule(char *db) {
	VarTree *var;
	char temp[1024];
	char temp2[1024];
	char *s, *t;
	int ret;

	var = GetVar("addrule",NULL,0);
	if(!var) return;
	_dbmClose(db);
	ret = _dbmOpen(db,"w",0);
	if(ret) return;
	if(!strcmp(var->strval,"Default")) strcpy(temp,"cfg-accessALL");
	else sprintf(temp,"cfg-access-%s",var->strval);
	s = _dbmFetch(db,temp);	
	temp2[0] = '\0';
	if(s) {
		strcpy(temp2,s);
	} else {
		strcpy(temp2,"pub");
	}
	s = _dbmFetch(db,"cfg-accessALL");
	if(s) {
		strcat(temp2,"\033");
		t = strchr(s+4,'\033');
		if(t) {
			t = strchr(t+1,'\033');	
			if(t) *t='\0';
		}
		strcat(temp2,s+4);
	}
	_dbmReplace(db,temp,temp2);
	_dbmClose(db);
	_dbmOpen(db,"r",0);
}
		
void AddFile(char *db, char *path) {
	char temp[1024];
	char *pi, *s;
	int ret;

#if APACHE
	pi = php_rqst->uri;
#else
	pi = getenv("PATH_INFO");
#endif
	if(!pi) return;
	sprintf(temp,"cfg-access-%s",pi);
	s = _dbmFetch(db,temp);
	if(s && strlen(s)>2) return;
	_dbmClose(db);
	ret = _dbmOpen(db,"w",0);
	if(ret) return;

	s = _dbmFetch(db,"cfg-accessALL");
	if(s) _dbmInsert(db,temp,s);	
	else _dbmInsert(db,temp,"pub\033L\033dom.*");
	_dbmClose(db);
	_dbmOpen(db,"r",0);
}

void ChkPostVars(char *db) {
	VarTree *var;
	int ret, op=0;

	var = GetVar("cfg-email-URL",NULL,0);
	if(var) {
		_dbmClose(db);
		ret = _dbmOpen(db,"w",0);
		if(ret) return;
		op = 1;
		_dbmReplace(db,"cfg-email-URL",var->strval);
		Push(var->strval,STRING);
		SetVar("cfg-email-URL",0,0);
	}
	var = GetVar("cfg-ban-URL",NULL,0);
	if(var) {
		if(!op) {
			_dbmClose(db);
			ret = _dbmOpen(db,"w",0);
			if(ret) return;
			op = 1;
		}
		_dbmReplace(db,"cfg-ban-URL",var->strval);
		Push(var->strval,STRING);
		SetVar("cfg-ban-URL",0,0);
	}
	var = GetVar("cfg-passwd-URL",NULL,0);
	if(var) {
		if(!op) {
			_dbmClose(db);
			ret = _dbmOpen(db,"w",0);
			if(ret) return;
			op = 1;
		}
		_dbmReplace(db,"cfg-passwd-URL",var->strval);
		Push(var->strval,STRING);
		SetVar("cfg-passwd-URL",0,0);
	}
	var = GetVar("chg-passwd",NULL,0);
	if(var) {
		if(!op) {
			_dbmClose(db);
			ret = _dbmOpen(db,"w",0);
			if(ret) return;
			op = 1;
		}
#ifdef HAVE_CRYPT
		_dbmReplace(db,"cfg-passwd",(char *)crypt(var->strval,"xy"));
#else
		_dbmReplace(db,"cfg-passwd",var->strval);
#endif
		Push(var->strval,STRING);
		SetVar("cfg-passwd",0,0);
	}
	if(op) {
		_dbmClose(db);
		_dbmOpen(db,"r",0);
	}
}

void PostToAccessStr(char *db) {
	VarTree *var, *del=NULL;
	char ind[8];
	char temp[1024];
	static char temp2[512];
	int ret;
	int i, count;
	char small[16];

	var = GetVar("file",NULL,0);
	if(!var) return;
	_dbmClose(db);
	ret = _dbmOpen(db,"w",0);
	if(ret) return;
	if(!strcmp(var->strval,"Default")) strcpy(temp,"cfg-accessALL");
	else sprintf(temp,"cfg-access-%s",var->strval);

	var = GetVar("def",NULL,0);
	if(!var) return;
	if(!strcmp(var->strval,"public")) strcpy(temp2,"pub\033");
	else strcpy(temp2,"pri\033");

	var = GetVar("mode",NULL,0);
	count = var->count;
	for(i=0;i<count;i++) {
		sprintf(small,"del%d",i);
		del = GetVar(small,NULL,0);
		if(del && strchr(del->strval,'n')) continue;
		if(i>0) strcat(temp2,"\033");
		sprintf(ind,"%d",i);
		var = GetVar("mode",ind,1);
		sprintf(small,"%c",*(var->strval));
		strcat(temp2,small);
		if(*(var->strval) == 'P') {
			var = GetVar("password",ind,1);
			strcat(temp2,"\033");
			if(var && *(var->strval)) {
				strcat(temp2,var->strval);
			} else {
				strcat(temp2," ");
			}
		}
		var = GetVar("type",ind,1);
		if(var && var->strval) { 
			StripSlashes(var->strval);
			switch((var->strval)[0]) {
				case 'D':
					strcat(temp2,"\033dom");
					break;
				case 'E':
					strcat(temp2,"\033mai");
					break;
				case 'R':
					strcat(temp2,"\033ref");
					break;
				case 'B':
					strcat(temp2,"\033bro");
					break;
			}
		}
		var = GetVar("patt",ind,1);
		if(var && var->strval) {
			StripSlashes(var->strval);
			strcat(temp2,var->strval);
		}
	}
	if(strlen(temp2)<5) {
		_dbmDelete(db,temp);
	} else {
		_dbmReplace(db,temp,temp2);
	}
	_dbmClose(db);
	_dbmOpen(db,"r",0);
}

int CheckAccess(char *filename, long uid) {
	VarTree *var;
	char *s, *ss;
	char db[512], temp[512];
	AccessInfo *actop, *ac;
	struct stat sb;
	int ret, allow=0;
	static char *email_URL=NULL, *passwd_URL=NULL, *ban_URL=NULL;
	int es, retu=0;
	regex_t re;
	regmatch_t subs[1];
	char erbuf[150];
	int err, len;

	if(stat(AccessDir,&sb)==-1) {
		if(mkdir(AccessDir,0755)==-1) {
			Error("Trying to create access directory [%s]: %d [%s]",AccessDir,errno,strerror(errno));
			return(0);
		}
	}
	sprintf(db,"%s/%ld-cfg",AccessDir,uid);

	es = ErrorPrintState(0);
	ret = _dbmOpen(db,"r",0);
	ErrorPrintState(es);
	if(ret) return(0);

	sprintf(temp,"cfg-access-%s",filename);	
	s = _dbmFetch(db,temp);
	if(!s || (s && !*s)) {
		s = _dbmFetch(db,"cfg-accessALL");
		if(!s || (s && !*s)) {
#if DEBUG
			Debug("Unable to read config string for this file\n");
#endif
			_dbmClose(db);
			return(0);
		}
	}
	actop = StrtoAccess(s);
	ac = actop;
	if(!ac) return(0);

	s = _dbmFetch(db,"cfg-email-URL");
	if(s) email_URL = (char *) estrdup(0,s);
	s = _dbmFetch(db,"cfg-passwd-URL");
	if(s) passwd_URL = (char *) estrdup(0,s);
	s = _dbmFetch(db,"cfg-ban-URL");
	if(s) ban_URL = (char *) estrdup(0,s);
	_dbmClose(db);

	while(ac) {
		err = regcomp(&re, ac->patt, REG_EXTENDED | REG_NOSUB);
		if(err) {
			len = regerror(err, &re, erbuf, sizeof(erbuf));
			Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
			continue;	
		}
		switch(ac->type) {
		case 0: /* domain */
			ss = getremotehostname();
			if(ss) {
				s = (char *) estrdup(0,ss);
			} else s=NULL;
			_strtolower(s);
			break;
		case 1: /* e-mail address */
			ss = getemailaddr();
			if(ss) {
				s = (char *) estrdup(0,ss);
			} else s=NULL;
			break;
		case 2: /* referring doc */
			s = getrefdoc();
			break;
		case 3: /* browser */
			s = getbrowser();
			break;
		}
		if(!s) { ac=ac->next; continue; }
		err = regexec(&re, s, 1, subs, 0);
		if(err && err!=REG_NOMATCH) {
			len = regerror(err, &re, erbuf, sizeof(erbuf));
			Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
			regfree(&re);
			return(0);
		}

		if(err==REG_NOMATCH) { 
			ac=ac->next; 
			s=NULL;
			regfree(&re);
			continue; 
		}
		switch(ac->mode) {
		case 1: /* E-Mail */
			if(!getemailaddr()) {
				ShowEmailPage(email_URL);
				retu = -1;
			}
			break;
		case 2: /* Allow */
			allow++;
			break;
		case 4: /* Ban */
			allow--;
			break;
		case 8: /* Password */
			var = GetVar("PASSWORD",NULL,0);
			if(!var || (var && strcmp(var->strval,ac->password))) {
				ShowPasswordPage(passwd_URL);
				retu = -1;
			}
			break;	
		case 16: /* NoLogging */
#if defined(LOGGING) || defined(MSQLLOGGING) || defined(MYSQLLOGGING)
			Logging=0;
#endif
			break;
		case 32: /* Logging */
#if defined(LOGGING) || defined(MSQLLOGGING) || defined(MYSQLLOGGING)
			Logging=1;
#endif
			break;
		case 64: /* Info */
			ShowInfo=1;
			break;
		case 128: /* NoInfo */
			ShowInfo=0;
			break;
		}
		ac = ac->next;
		s=NULL;
		regfree(&re);
	}	
	if(actop->def==0) allow--;	
	if(allow<0) { ShowBanPage(ban_URL); return(-1); }
	return(retu);
}

void ShowBanPage(char *url) {
	if(url && strlen(url) > 5) {
		php_header(1,url);
		Exit(0);
		return;
	}
	php_header(0,NULL);
	PUTS("<html><head><title>No Access</title></head>\n");
	PUTS("<body><h1>Sorry - You do not have access to this page</h1></body></html>\n");
	ShowInfo=0;
	Exit(0);
}

void ShowEmailPage(char *url) {
	VarTree *var;
	char *sn, *pi;
#ifdef VIRTUAL_PATH
	sn = VIRTUAL_PATH;
#else
#if APACHE
	sn = php_rqst->uri;
#else
	sn = getenv("SCRIPT_NAME");
#endif
#endif
#if APACHE
	pi = sn;
#else
	pi = getenv("PATH_INFO");
#endif
	if(url && strlen(url) > 5) {
		php_header(1,url);
		Exit(0);
		return;
	}
	php_header(0,NULL);
	PUTS("<html><head><title>E-Mail Address Required</title></head>\n");
	PUTS("<body><h1>E-Mail Address Required</h1>Please provide your e-mail address to continue\n");
#if APACHE
	PUTS("<center><form action=\"");
	PUTS(sn);
	PUTS("\" method=\"POST\">\n");
#else
	printf("<center><form action=\"%s%s\" method=\"POST\">\n",sn,pi?pi:"");
#endif
	PUTS("<input type=\"text\" name=\"EMAIL_ADDR\">\n");
	var = GetVar("PASSWORD",NULL,0);
	if(var && *(var->strval)) {
#if APACHE
		PUTS("<input type=\"hidden\" name=\"PASSWORD\" value=\"");
		PUTS(var->strval);
		PUTS("\">");
#else
		printf("<input type=\"hidden\" name=\"PASSWORD\" value=\"%s\">\n",var->strval);
#endif
	}
	PUTS("<input type=\"submit\" value=\" Submit \"></form></center>\n");
	PUTS("</body></html>\n");
	ShowInfo=0;
	Exit(0);
	return;
}

void ShowPasswordPage(char *url) {
	VarTree *var;
	char *sn, *pi;

#ifdef VIRTUAL_PATH
	sn = VIRTUAL_PATH;
#else
#if APACHE
	sn = php_rqst->uri;
#else
	sn = getenv("SCRIPT_NAME");
#endif
#endif
#if APACHE
	pi = sn;
#else
	pi = getenv("PATH_INFO");
#endif
	if(url && strlen(url) > 5) {
		php_header(1,url);
		Exit(0);
		return;
	}
	php_header(0,NULL);
	PUTS("<html><head><title>Password Required</title></head>\n");
	PUTS("<body><h1>Password Required</h1>Please enter the password:\n"); 
#if APACHE
	PUTS("<center><form action=\"");
	PUTS(sn);
	PUTS("\" method=\"POST\">\n");
#else
	printf("<center><form action=\"%s%s\" method=\"POST\">\n",sn,pi?pi:"");
#endif
	PUTS("<input type=\"password\" name=\"PASSWORD\">\n");
	var = GetVar("EMAIL_ADDR",NULL,0);
	if(var && *(var->strval)) {
#if APACHE
		PUTS("<input type=\"hidden\" name=\"EMAIL_ADDR\" value=\"");
		PUTS(var->strval);
		PUTS("\">\n");
#else
		printf("<input type=\"hidden\" name=\"EMAIL_ADDR\" value=\"%s\">\n",var->strval);
#endif
	}
	PUTS("<input type=\"submit\" value=\" Submit \"></form></center>\n");
	PUTS("</body></html>\n");
	ShowInfo=0;
	Exit(0);
	return;
}

#endif  /* ACCESS_CONTROL */

char *getremotehostname(void) {
	char *s;

	if(!RemoteHostName) {
#if APACHE
		s = table_get(php_rqst->subprocess_env,"REMOTE_HOST");
#else
		s = getenv("REMOTE_HOST");
#endif
		if(s) {
			if(isdigit(*s)) s = (char *)_GetHostByAddr(s);
			RemoteHostName = s;
		} else {
#if APACHE
			s = table_get(php_rqst->subprocess_env,"REMOTE_ADDR");
#else
			s = getenv("REMOTE_ADDR");
#endif
			if(s) {
				if(isdigit(*s)) s = (char *)_GetHostByAddr(s);
				RemoteHostName = s;
			}
		}
	}
	return(RemoteHostName);
}

char *getemailaddr(void) {
	static VarTree *var;

	if(!EmailAddr) {	
		var = GetVar("EMAIL_ADDR",NULL,0);
		if(!var) EmailAddr=NULL;
		else EmailAddr = var->strval;
	}
	return(EmailAddr);
}

char *getrefdoc(void) {
	if(!RefDoc) {
#if APACHE
		RefDoc = table_get(php_rqst->subprocess_env,"HTTP_REFERER");
#else
		RefDoc = getenv("HTTP_REFERER");
#endif
	}
	return(RefDoc);
}	

char *getbrowser(void) {
	if(!Browser) {
#if APACHE
		Browser = table_get(php_rqst->subprocess_env,"HTTP_USER_AGENT");
#else
		Browser = getenv("HTTP_USER_AGENT");
#endif
	}
	return(Browser);
}

int getlogging(void) {
#if defined(LOGGING) || defined(MSQLLOGGING) || defined(MYSQLLOGGING)
	return(Logging);
#else
	return(0);
#endif
}

void setlogging(int val) {
#if defined(LOGGING) || defined(MSQLLOGGING) || defined(MYSQLLOGGING)
	Logging = val;
#endif
}

void SetLogging(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in setlogging()");
		return;
	}
	setlogging(s->intval);
}

int getshowinfo(void) {
	return(ShowInfo);
}

void setshowinfo(int val) {
	ShowInfo = val;
}

void SetShowInfo(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in setshowinfo()");
		return;
	}
	setshowinfo(s->intval);
}

char *getaccdir(void) {
#if ACCESS_CONTROL
	if(AccessDir) return(AccessDir);
	else return(ACCESS_DIR);
#else
	return NULL;
#endif
}

void GetAccDir(void) {
#if ACCESS_CONTROL
	Push(getaccdir(),STRING);
#endif
}

