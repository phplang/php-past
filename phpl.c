/*
 * PHP Tools
 *
 * Personal Home Page Tools.
 *
 * Copyright (C) 1995, Rasmus Lerdorf <rasmus@io.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * If you do use this software, please let me know.  I'd like to maintain
 * a list of URL's on my home page just to see what interesting things
 * people are doing with their home pages these days.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#if FILEH
#include <sys/file.h>
#endif
#if LOCKFH
#include <sys/lockf.h>
#endif
#include <time.h>
#include <ctype.h>
#include <pwd.h>
#include "config.h"
#include "version.h"
#include "wm.h"
#include "common.h"
#include "post.h"

#ifndef LOGDIR
#define LOGDIR	"logs/"		/* Directory in which log files should be placed */
#endif
#ifndef ACCDIR
#define ACCDIR  "logs/"
#endif
#ifndef NOACCESS
#define NOACCESS "NoAccess.html"
#endif
#ifndef STARTSEP
#define STARTSEP "<!--"
#endif
#ifndef ENDSEP
#define ENDSEP   "-->"
#endif
#ifndef STARTLEN
#define STARTLEN  4
#endif
#ifndef ENDLEN
#define ENDLEN    3
#endif

static char host[128];
static char user[128];
static char agent[128];
static char short_agent[64];
static char lasttime[32];
static char lastuser[128];
static char modtime[32];
static int cnt;
static int todays_cnt;
static int NoInfo;
static char password[64];
static char acc_password[64];
static char email_addr[128];
static char referer[128];
static char EmailForm[128];
static char PasswordForm[128];
static char raw_file[128];

static postdata *pdtop=NULL;

#if DEBUG
FILE *fperr;
#endif

static char *Month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug",
					"Sep","Oct","Nov","Dec"};

void LoadEnvVars(void) {
	char *s=NULL, *t=NULL;

	if(getenv("REMOTE_HOST")) strncpy(host,getenv("REMOTE_HOST"),127);
	else if(getenv("REMOTE_ADDR")) strncpy(host,getenv("REMOTE_ADDR"),127);
	else strcpy(host,"unknown");
	if(getenv("REMOTE_IDENT")) strncpy(user,getenv("REMOTE_IDENT"),127);
	else if(getenv("REMOTE_USER")) strncpy(user,getenv("REMOTE_USER"),127);
	else strcpy(user,"unknown");
	if(getenv("HTTP_USER_AGENT")) {
		strncpy(agent,getenv("HTTP_USER_AGENT"),127);
		s=strchr(agent,'/');
		t=strchr(agent,' ');
		if(s && t && s<t) {
			*t='\0';
			strcpy(short_agent,agent);
			*t=' ';
		} else if(s && t && t<s) {
			s=strchr(t+1,' ');
			if(s) {
				*s='\0';
				strcpy(short_agent,agent);
				*s=' ';
			} else strcpy(short_agent,agent);
		} else strcpy(short_agent,agent);
	} else strcpy(agent,"unknown");
	if((int)strlen(referer)>0) return;
	if(getenv("HTTP_REFERER")) strncpy(referer,getenv("HTTP_REFERER"),127);
	else strcpy(referer,"none");
}

void LogEntry(char *fn, int logit) {
	FILE *fp;
	char buf[64];
	char cntbuf[128];
	char filename[128];
	char *s, *ss=NULL, *t=NULL;
	struct tm *tm1;
	time_t tmnow;
	struct stat sb;

	cnt=-1;
	if(fn && strlen(fn)) {
		strcpy(filename,fn);
	} else return;

#if DEBUG
	fprintf(fperr,"LogEntry called with filename [%s]\n",filename);
	fflush(fperr);
#endif

	if(stat(filename,&sb)!=-1) {
		tm1=localtime(&sb.st_mtime);
		sprintf(modtime,"%s %d, %d at %d:%02d",Month[tm1->tm_mon],tm1->tm_mday,1900+tm1->tm_year,tm1->tm_hour,tm1->tm_min);
	}
	tmnow=time(NULL);
	tm1=localtime(&tmnow);
	s=strrchr(filename,'.');
	if(s) *s='\0';
	if((s=strrchr(filename,'/'))) s++;
	else s=filename;	
	if(logit) {
		if(LOGDIR[0]!='/') FixFilename(LOGDIR,buf);
		else {
			strcpy(buf,LOGDIR);
		}
		ss=buf+strlen(buf)-1;
		if(*ss !='/') strcat(buf,"/");
		strcat(buf,s);
		strcat(buf,".log");

#if DEBUG
		fprintf(fperr,"Opening logfile [%s]\n",buf);
		fflush(fperr);
#endif

		fp=fopen(buf,"a");
		if(!fp) {
			puts("<html><head><title>PHPL Error</title></head><body>");
			puts("<center><h1>PHPL Error</h1></center>");
			printf("Unable to open logfile: %s for append\n",buf);
			puts("</body></html>");
			return;
		}
#if FLOCK
		flock(fileno(fp),LOCK_EX); /* Lock file */
#endif
#if LOCKF
		lockf(fileno(fp),F_LOCK,1024);
#endif
		if(!strlen(email_addr) || (strlen(email_addr)&&!strchr(email_addr,'@'))) 
			fprintf(fp,"%s %02d %02d:%02d %s %s%cnone %s\n",Month[tm1->tm_mon],tm1->tm_mday,tm1->tm_hour, tm1->tm_min,host,agent,27,referer);
		else fprintf(fp,"%s %02d %02d:%02d %s %s%c%s %s\n",Month[tm1->tm_mon],tm1->tm_mday,tm1->tm_hour, tm1->tm_min,host,agent,27,email_addr,referer);
		fflush(fp);
#if FLOCK
		flock(fileno(fp),LOCK_UN); /* Unlock file */
#endif
#if LOCKF
		lockf(fileno(fp),F_ULOCK,1024);
#endif
		fclose(fp);
	}
	if(LOGDIR[0]!='/') FixFilename(LOGDIR,buf);
	else strcpy(buf,LOGDIR);
	ss=buf+strlen(buf)-1;
	if(*ss !='/') strcat(buf,"/");
	strcat(buf,s);
	strcat(buf,".cnt");

#if DEBUG
	fprintf(fperr,"Opening cnt file [%s]\n",buf);
	fflush(fperr);
#endif

	if(logit) fp=fopen(buf,"r+");
	else {
		fp=fopen(buf,"r");
	}	
	if(!fp) {
		fp=fopen(buf,"w");
		cnt=0;
		if(!fp) {
			puts("<html><head><title>PHPL Error</title></head><body>");
			puts("<center><h1>PHPL Error</h1></center>");
			printf("Unable to open count file: %s for write\n",buf);
			puts("</body></html>");
			return;
		}
	}
#if FLOCK
	if(logit) flock(fileno(fp),LOCK_EX);
#endif
#if LOCKF
	if(logit) lockf(fileno(fp),F_LOCK,1024);
#endif
	if(cnt==-1) {

#if DEBUG
		fprintf(fperr,"cnt=-1\n");
		fflush(fperr);
#endif

		if(fgets(cntbuf,127,fp)) {
			s=strchr(cntbuf,' ');
			if(s) *s='\0';
			cnt=atoi(cntbuf);
			if(s) todays_cnt=atoi(s+1);
		} else {
			cnt=0;
			todays_cnt=0;
		}

#if DEBUG
		fprintf(fperr,"cnt=%d todays_cnt=%d\n",cnt,todays_cnt);
		fflush(fperr);
#endif

		if(fgets(cntbuf,127,fp)) {
			strncpy(lasttime,cntbuf,strlen(cntbuf)-1);
			s=strrchr(lasttime,' ');
			if(s) {
				*s='\0';
				t=strrchr(lasttime,' ');	
				*s=' ';
			}
			if(s && t) {
				if(strncmp(Month[tm1->tm_mon],t+1,3) || atoi(s+1)!=tm1->tm_mday) {
					todays_cnt=0;
				}
			}
		} else strcpy(lasttime,"Never");
		if(fgets(cntbuf,127,fp)) strncpy(lastuser,cntbuf,strlen(cntbuf)-1);
		else strcpy(lastuser,"Nobody");
		rewind(fp);
	} else {
		strcpy(lasttime,"Never");
		strcpy(lastuser,"Nobody");
	}

	if(logit) {
		fprintf(fp,"%d %d\n",++cnt, ++todays_cnt);
		fprintf(fp,"%d:%02d on %s %02d\n",tm1->tm_hour,tm1->tm_min,Month[tm1->tm_mon],tm1->tm_mday);
		if(!strlen(email_addr) || (strlen(email_addr)&&!strchr(email_addr,'@'))) 
			fprintf(fp,"%s\n",host);
		else 
			fprintf(fp,"%s\n",email_addr);
		fflush(fp);
#if FLOCK
		flock(fileno(fp),LOCK_UN);
#endif
#if LOCKF	
		lockf(fileno(fp),F_ULOCK,1024);
#endif
		fclose(fp);	
	}
} /* LogEntry */

/*
 * CheckAccess
 *
 * Returns -1 if access file wasn't found
 *          0 if user is allowed access
 *          1 if user is denied access
 *          2 if user is allowed access but should not be logged
 */
int CheckAccess(char *fn, int *email_req, int *password_req) {
	FILE *fp;
	char *r,*s,*ss=NULL,*t;
	char buf[128];
	char filename[1024];
	int Access=0;
	int Match=0;
	int DefAccess=0;  /* Default is to allow if not specified */
	int old_em=0;
	int old_pw=0;

#if DEBUG
	fprintf(fperr,"In CheckAccess: NoInfo=%d\n",NoInfo);
	fflush(fperr);
#endif

	strcpy(filename,fn);
	s=strrchr(filename,'.');
	if(s) *s='\0';
	if((s=strrchr(filename,'/'))) s++;
	else s=filename;	
	if(ACCDIR[0]!='/') FixFilename(ACCDIR,buf);
	else strcpy(buf,ACCDIR);
	ss=buf+strlen(buf)-1;
	if(*ss!='/') strcat(buf,"/");
	strcat(buf,s);
	strcat(buf,".acc");
#if DEBUG
	fprintf(fperr,"Checking access file [%s]\n",buf);
	fflush(fperr);
#endif
	fp=fopen(buf,"r");
	if(!fp) return(-1);
	while(!feof(fp)) {
		Match=0;
		Access=0;
		if(fgets(buf,127,fp)) {
			if(buf[0]=='#') continue;  /* ignore comments */
			s=strchr(buf,' '); /* accept both space and tab as separator */
			t=strchr(buf,'\t');
			if(!s && t) s=t;	
			if(s && t && t<s) t=s;
			if(!s) continue;
			*s='\0';
			if(!strcasecmp(buf,"public")) { DefAccess=0; continue; }
			if(!strcasecmp(buf,"private")) { DefAccess=1; continue; }
			if(!strcasecmp(buf,"allow")) Access=0;
			else if(!strcasecmp(buf,"ban")) Access=1;
			else if(!strcasecmp(buf,"nolog")) Access=2;
			else if(!strcasecmp(buf,"noinfo") && NoInfo != 2) NoInfo=1;
			else if(!strcasecmp(buf,"info")) NoInfo+=3;
			else if(!strcasecmp(buf,"passwordform")) {
				strcpy(PasswordForm,s+1);
				t=PasswordForm+strlen(PasswordForm)-1;
				if(*t=='\n') *t='\0';
#if DEBUG
				fprintf(fperr,"PasswordForm=[%s]\n",PasswordForm);
				fflush(fperr);
#endif
			} else if(!strcasecmp(buf,"emailform")) {
				strcpy(EmailForm,s+1);
				t=EmailForm+strlen(EmailForm)-1;
				if(*t=='\n') *t='\0';
#if DEBUG
				fprintf(fperr,"EmailForm=[%s]\n",EmailForm);
				fflush(fperr);
#endif
			} else if(!strcasecmp(buf,"email")&&*email_req==0) {
				*email_req=1;
			} else if(!strcasecmp(buf,"noemail")) {
				old_em=*email_req;
				*email_req=4;
			} else if(!strcasecmp(buf,"password")&&*password_req==0) {
				r=s+1;
				while(*r==' ' || *r=='\t') r++;
				if(strlen(r)==0) continue;
				s=strchr(r,' '); /* accept both space and tab as separator */
				t=strchr(r,'\t');
				if(!s && t) s=t;	
				if(s && t && t<s) t=s;
				if(!s) continue;
				*s='\0';
				strcpy(acc_password,r);	
				*password_req=1;
			} else if(!strcasecmp(buf,"nopassword")) {
				old_pw=*password_req;
				*password_req=4;
			}
			r=s+1;
			/* Strip leading spaces/tabs */
			while(*r==' ' || *r=='\t') r++;
			if(strlen(r)==0) continue;
			s=r;
			s+=strlen(r)-1;
			while(*s==' ' || *s=='\t' || *s=='\n' || *s=='\r') s--;
			*(s+1)='\0';	
			if(!strncasecmp(r,"mail:",5)) {
				r+=5;
				if(strlen(r)==0) continue;
				if(email_addr && strlen(email_addr))
					strcpy(filename,email_addr);
				else continue;
			} else if(!strncasecmp(r,"browser:",8)) {
				r+=8;
				if(strlen(r)==0) continue;
				if(agent && strlen(agent))
					strcpy(filename,agent);
				else continue;
			} else if(!strncasecmp(r,"referer:",8)) {
				r+=8;
				if(strlen(r)==0) continue;
				if(referer && strlen(referer)) 
					strcpy(filename,referer);
				else continue;
			} else sprintf(filename,"%s@%s",user,host);
#if DEBUG
			fprintf(fperr,"Matching [%s] [%s]\n",filename,r);
			fflush(fperr);
#endif
			if(wild_match(r,filename)!=0) {
#if DEBUG
				fprintf(fperr,"Got Match\n");
				fflush(fperr);
#endif
				if(*email_req==1) { *email_req=2; continue; }
				if(*email_req==4) { *email_req=3; continue; }
				if(*password_req==1) { *password_req=2; continue; }
				if(*password_req==4) { *password_req=3; continue; }
				if(NoInfo!=1 && NoInfo!=3 && NoInfo!=5) { Match=1; break; }
				else if(NoInfo==1) NoInfo=2;
				else NoInfo=0;
			} else {
				if(NoInfo==1 || NoInfo==3) NoInfo=0;
				if(*password_req==1) *password_req=0;
				if(*password_req==4) *password_req=old_pw;
				if(*email_req==1) *email_req=0;
				if(*email_req==4) *email_req=old_em;
#if DEBUG
				fprintf(fperr,"No Match\n");
				fflush(fperr);
#endif
			}
		}	
	}
	fclose(fp);

#if DEBUG
	fprintf(fperr,"Leaving CheckAccess: NoInfo=%d em=%d pw=%d\n",NoInfo,*email_req,*password_req);
	fflush(fperr);
#endif

	if(Match) return(Access);
	else return(DefAccess);
}

/*
 * ShowWrongPassword
 *
 * Tell user they got the password wrong
 */
void ShowWrongPassword(void) {
	puts("<html><head><title>Wrong Password!</title></head>");
	puts("<body><center><h1>Wrong Password!</h1></center>");
	puts("Sorry, the password you entered is not correct.  Please go");
	puts("back and try again.<p>");
	puts("</body></html>");
}

/*
 * ShowEmailPasswordForm
 *
 * Prompt user for both an email address and a password
 */
void ShowEmailPasswordForm(char *cmd, char *filename) {
	puts("<html><head><title>Email Address and Password Required</title></head>");
	puts("<body><center><h1>Email and Password Required</h1></center>");
	puts("This document is protected by a password.  You must know this");
	puts("password in order to continue.  You are also asked to enter your");
	puts("email address.  If either of these fields are not filled out, you");
	puts("will not be granted access.<p>");
	printf("<form action=\"%s?%s\" method=\"POST\">\n",cmd,filename);
	printf("<input type=\"hidden\" name=\"http_referer\" value=\"%s\">\n",referer);
	printf("<center><tt>Email Address</tt><br><input type=\"text\" name=\"email\" size=40 value=\"%s\"></center><p>\n",email_addr);
	puts("<center><tt>Password</tt><br><input type=\"password\" name=\"password\"></center><p>");
	puts("<center><input type=\"submit\" value=\" SEND \"></center><hr>");
	printf("<font size=-1><i><a href=\"%s\">PHP Tools</a> Copyright &copy 1995 Rasmus Lerdorf</i></font><br>\n",PHPURL);
	puts("</body></html>");
}

/*
 * ShowEmailForm
 *
 * Prompt user for an email address
 */
void ShowEmailForm(char *cmd, char *filename) {
#if DEBUG
	fprintf(fperr,"About to print Email Address Request Form\n");
	fprintf(fperr,"cmd = [%s], Filename = [%s]\n",cmd, filename);
	fflush(fperr);
#endif
	puts("<html><head><title>Email Address Required</title></head>");
	puts("<body><center><h1>Email Address Required</h1></center>");
	puts("In order to continue, you are asked to provide your e-mail address.");
	puts("The information is used for logging purposes only.");
	puts("Please enter it in the field below.<p>");
	printf("<form action=\"%s?%s\" method=\"POST\">\n",cmd,filename);
	printf("<input type=\"hidden\" name=\"http_referer\" value=\"%s\">\n",referer);
	puts("<center><tt>Email Address</tt><br><input type=\"text\" size=40 name=\"email\"></center><p>");
	puts("<center><input type=\"submit\" value=\" SEND \"></center><hr>");
	puts("In order to avoid having to fill in this field in the future");
	puts("you can append your e-mail address to the URL you are trying to");
	puts("access right now.  Put a '+' followed by your e-mail address at the");
	puts("end of the URL and you will be able to skip this window.<hr>");
	printf("<font size=-1><i><a href=\"%s\">PHP Tools</a> Copyright &copy 1995 Rasmus Lerdorf</i></font><br>\n",PHPURL);
	puts("</body></html>");
}

/*
 * ShowPasswordForm
 *
 * Prompt user for both an email address and a password
 */
void ShowPasswordForm(char *cmd, char *filename) {
	puts("<html><head><title>Password Required</title></head>");
	puts("<body><center><h1>Password Required</h1></center>");
	puts("This document is protected by a password.  You must know this");
	puts("password in order to continue.<p>");
	printf("<form action=\"%s?%s\" method=\"POST\">\n",cmd,filename);
	printf("<input type=\"hidden\" name=\"http_referer\" value=\"%s\">\n",referer);
	puts("<center><tt>Password</tt><br><input type=\"password\" name=\"password\"></center><p>");
	puts("<center><input type=\"submit\" value=\" SEND \"></center><hr>");
	printf("<font size=-1><i><a href=\"%s\">PHP Tools</a> Copyright &copy 1995 Rasmus Lerdorf</i></font><br>\n",PHPURL);
	puts("</body></html>");
}

/* 
 * ShowFile
 *
 * Function returns -1 if file could not be opened and -2 if access rule
 * denied user access.
 */
int ShowFile(char *filename, char *cmd, char *org_file, int checkaccess) {
	int retall, ret;
	char buf[1024];
	char *buf2=NULL;
	FILE *fp;
	char *s, *ss, *t, *tt;
	int email_all_req=0, password_all_req=0, email_req=0, password_req=0;
	postdata *pdcur;
	int bufsize=0;
	char temp[16];

	LoadEnvVars();
	NoInfo=0;
#if DEBUG
	fprintf(fperr,"org_file=[%s]\n",org_file);
	fflush(fperr);
#endif
	if(!checkaccess) goto skip;
	EmailForm[0]='\0';
	PasswordForm[0]='\0';
	if((retall=CheckAccess("All",&email_all_req,&password_all_req))==1) return(-2);

	if((ret=CheckAccess(filename,&email_req,&password_req))==1) {
		return(-2);
	}

	if(((email_all_req || email_req)&&email_req!=3) && ((password_all_req || password_req)&&password_req!=3)) {
		if(strlen(password) && strcmp(acc_password,password)) {
			ShowWrongPassword();
			return(0);
		}
		if(!strlen(email_addr) && !strlen(password)) {
			ShowEmailPasswordForm(cmd,org_file);
			return(0);
		} else if(!strlen(password)) {
			if(strlen(PasswordForm)) ShowFile(PasswordForm,cmd,org_file,0);
			else ShowPasswordForm(cmd,org_file);
			return(0);
		}
	} else if((email_all_req || email_req)&&email_req!=3) {
		if(!strlen(email_addr)) {
			if(strlen(EmailForm)) ShowFile(EmailForm,cmd,org_file,0);
			else ShowEmailForm(cmd,org_file);
			return(0);
		}
	} else if((password_all_req || password_req)&&password_req!=3) {
		if(strlen(password) && strcmp(acc_password,password)) {
			ShowWrongPassword();
			return(0);
		}
		if(!strlen(password)) {
			if(strlen(PasswordForm)) ShowFile(PasswordForm,cmd,org_file,0);
			else ShowPasswordForm(cmd,org_file);
			return(0);
		}
	}
	if( (retall==0 && ret==-1) || (retall==-1 && ret==0) || (retall==0 && ret==0) || (retall==-1 && ret==-1) ) {
		LogEntry(filename,1);
	}
	else LogEntry(filename,0);

skip:

#if DEBUG
	fprintf(fperr,"About to open [%s]\n",filename);
	fflush(fperr);
#endif

	if(!(fp=fopen(filename,"r"))) {

#if DEBUG
		fprintf(fperr,"Unable to open [%s]\n",filename);
		fflush(fperr);
#endif

		return(-1);
	}

#if DEBUG
	fprintf(fperr,"Displaying File...\n");
	fflush(fperr);
#endif
	if(CheckExtension(filename,".txt")) {
		puts("<pre>");
	}
	while(!feof(fp)) {
		if(!fgets(buf,511,fp)) break;
		tt=buf;
start:
		if((s=strstr(tt,STARTSEP))) {
#if DEBUG
			fprintf(fperr,"Found start of substitution\n");
			fflush(fperr);
#endif
			t=strstr(s,ENDSEP);
			if(!t) {
				fputs(tt,stdout);
				continue;
			}
			*t='\0';
			if(s-buf>0) {
				*s='\0';
				fputs(tt,stdout);
			}
			s+=STARTLEN;

			if(!strncasecmp(s,"$total",6)) {
				printf("%d",cnt);
			} else if(!strncasecmp(s,"$today",6)) {
				printf("%d",todays_cnt);
			} else if(!strncasecmp(s,"$lasttime",9)) {
				printf("%s",lasttime);
			} else if(!strncasecmp(s,"$lastuser",9)) {
				printf("%s",lastuser);
			} else if(!strncasecmp(s,"$modtime",8)) {
				printf("%s",modtime);
			} else if(!strncasecmp(s,"$version",8)) {
				printf("%s",VERSION);
			} else if(!strncasecmp(s,"$email",6)) {
				printf("%s",email_addr);
			} else if(!strncasecmp(s,"$plusemail",6)) {
				if(email_addr && strlen(email_addr)) printf("+%s",email_addr);
			} else if(!strncasecmp(s,"$http_referer",13)) {
				printf("%s",referer);
			} else if(!strncasecmp(s,"$browser",8)) {
				printf("%s",agent);
			} else if(!strncasecmp(s,"$short_browser",14)) {
				printf("%s",short_agent);
			} else if(!strncasecmp(s,"$filename",9)) {
				printf("%s",raw_file);
			}
			/* If there is post data present, check for substitutions */
			else if(pdtop) {
#if DEBUG
				fprintf(fperr,"Data present, scanning for subs\n");
				fflush(fperr);
#endif
				buf2=malloc(1024*sizeof(char));
				bufsize=1024;
				strcpy(buf2,s);
				pdcur=pdtop;
				ss=strchr(s,' ');
				if(ss) *ss='\0';
				while(pdcur) {
#if DEBUG
					fprintf(fperr,"Calling SubVar with [%s] [%s] [%s]\n",pdcur->var,pdcur->val,buf2);
					fflush(fperr);
#endif
					buf2=SubVar('$',pdcur->var,pdcur->val,buf2,&bufsize);
#if DEBUG
					fprintf(fperr,"New buf is [%s]\n",buf2);
					fflush(fperr);
#endif
					pdcur=pdcur->next;
				}
				if(ss) *ss=' ';
#if !defined(NOSYSTEM)
				if(s[0]!='!') 
#endif
					fputs(buf2,stdout);
			}
#if !defined(NOSYSTEM)
			if(s[0]=='!') {
				fflush(stdout);
				if(!buf2) {
					buf2=malloc(1024*sizeof(char));
					bufsize=1024;
					strcpy(buf2,s);
				}
				if(strchr(buf2,'$')) {
					sprintf(temp,"%d",cnt);
					buf2=SubVar('$',"total",temp,buf2,&bufsize);
					sprintf(temp,"%d",todays_cnt);
					buf2=SubVar('$',"today",temp,buf2,&bufsize);
					buf2=SubVar('$',"lasttime",lasttime,buf2,&bufsize);
					buf2=SubVar('$',"lastuser",lastuser,buf2,&bufsize);
					buf2=SubVar('$',"modtime",modtime,buf2,&bufsize);
					buf2=SubVar('$',"version",VERSION,buf2,&bufsize);
					buf2=SubVar('$',"email",email_addr,buf2,&bufsize);
					if(email_addr && strlen(email_addr)) sprintf(temp,"+%s",email_addr);
					else temp[0]='\0';
					buf2=SubVar('$',"plusemail",temp,buf2,&bufsize);
					buf2=SubVar('$',"http_referer",referer,buf2,&bufsize);
					buf2=SubVar('$',"browser",agent,buf2,&bufsize);
					buf2=SubVar('$',"short_browser",short_agent,buf2,&bufsize);
					buf2=SubVar('$',"filename",raw_file,buf2,&bufsize);
				}
				system(&buf2[1]);
				fflush(stdout);
			} 
#endif
			if(buf2) {
				free(buf2);
				buf2=NULL;
			}
			if(strlen(t+ENDLEN)) {
				tt=t+ENDLEN;
				goto start;  /* Ok, it's ugly, but effective in this case */
			}
		} else fputs(tt,stdout);
	}
	if(CheckExtension(filename,".txt")) {
		puts("</pre>");
	}
	fflush(stdout);
	fclose(fp);	
	if(!NoInfo && checkaccess) {
		printf("<hr size=3 width=400 align=\"center\"><font size=2><center>Last access at %s by %s\n<br>",lasttime,lastuser);
		if(todays_cnt==1) printf("This page has been accessed a total of <a href=\"phplmon.cgi\"><b>%d</b></a> times now! <b>%d</b> time today.<br>",cnt,todays_cnt);
		else printf("This page has been accessed a total of <a href=\"phplmon.cgi\"><b>%d</b></a> times now! <b>%d</b> times today.<br>",cnt,todays_cnt);
		printf("Page was last updated on %s.</center></font>",modtime);
		fflush(stdout);
	}
	return(0);
}	

int main(int argc, char *argv[]) {
	int ret, retall=0;
	char filebuf[512];
	char filebuf2[512];
	char *s, *t, *res=NULL;
	int email_all_req=0, password_all_req=0, email_req=0, password_req=0;
	postdata *pdcur;
	int first_time=1;

#if DEBUG
	fperr=fopen("phpl.err","w");
	fprintf(fperr,"phpl starting\n");
	fflush(fperr);
#endif

	email_addr[0]='\0';
	password[0]='\0';

	setuid(geteuid());

	if(!strcasecmp(getenv("REQUEST_METHOD"),"post")) {
		res=getpost();

#if DEBUG
		fprintf(fperr,"Post Data = [%s]\n",res);
		if(argc>1) fprintf(fperr,"arg1 = [%s]\n",argv[1]);
		if(argc>2) fprintf(fperr,"arg2 = [%s]\n",argv[2]);
		if(argc>3) fprintf(fperr,"arg3 = [%s]\n",argv[3]);
		fflush(fperr);
#endif

		if(CheckResult(res)) {
			pdcur=(struct _postdata *)malloc(sizeof(struct _postdata));
			pdtop=pdcur;
			first_time=1;
			parse_url((unsigned char *)res);
			s=strtok(res,"&");
			while(s) {
				t=strchr(s,'=');
				if(!t) continue;
				*t='\0';
				if(!first_time) {
					pdcur->next=(struct _postdata *)malloc(sizeof(struct _postdata));
					if(!pdcur->next) break;
					pdcur=pdcur->next;
				}
				if(s) strcpy(pdcur->var,s);
				if(s+1) strcpy(pdcur->val,t+1);
				if(!strcasecmp(s,"email")) {
					if(strchr(t+1,' ') || !strchr(t+1,'@')) email_addr[0]='\0';
					else strncpy(email_addr,t+1,127);
				}
				if(!strcasecmp(s,"password")) strncpy(password,t+1,63);
				if(!strcasecmp(s,"http_referer")) strncpy(referer,t+1,127);
#if DEBUG
				fprintf(fperr,"var=[%s], val=[%s]\n",pdcur->var,pdcur->val);
				fflush(fperr);
#endif
				s=strtok(NULL,"&");
				pdcur->next=NULL;
				first_time=0;
			}
#if DEBUG
			fprintf(fperr,"Linked list contains:\n");
			pdcur=pdtop;
			while(pdcur) {
				fprintf(fperr,"var=[%s], val=[%s]\n",pdcur->var,pdcur->val);
				fflush(fperr);
				pdcur=pdcur->next;
			}
#endif
		}
	}
	if(argv[1] && strstr(argv[1],"://")) {
#if DEBUG
		fprintf(fperr,"External Link Detected\n");
		fflush(fperr);
#endif
		if(argc<3) {
			puts("Content-type: text/html");
			puts("");
			puts("<html><head><title>PHPL Error</title></head><body>");
			puts("<center><h1>PHPL Error</h1></center>");
			puts("When tracking access to off-site pages, you need to provide");
			puts("a log filename. ie. <br><br><font size=2><tt>");
			puts("&lta href=\"/~user/phpl.cgi?http://machine.dom+logs/machine.log\"&gt");
			puts("</font></tt><br><br>");
			puts("This would log the access in the file <i>logs/machine.log</i>");
			puts("And the re-direct the user to the URL <i>http://machine.dom</i>.");
			puts("</body></html>");
			return(0);
		}
		LoadEnvVars();
		if(argc>3) {
			if(strchr(argv[3],' ') || !strchr(argv[3],'@')) email_addr[0]='\0';
			else strcpy(email_addr,argv[3]);
		}
		strcpy(filebuf,argv[2]);
		NoInfo=0;

#if DEBUG
		fprintf(fperr,"Checking Access File [%s] for External Link\n",filebuf);
		fflush(fperr);
#endif
		
		retall=CheckAccess("All",&email_all_req,&password_all_req);
		ret=CheckAccess(filebuf,&email_req,&password_req);
		if(retall==1 || ret==1) {
#if DEBUG
			fprintf(fperr,"Access Denied to external link\n");
			fflush(fperr);
#endif
			ShowFile(NOACCESS,argv[0],argv[1],0);
			return(0);
		}
#if DEBUG
		fprintf(fperr,"Checking for e-mail and password requirements for external link\n");
		fflush(fperr);
#endif
		if(argc>2) {
			sprintf(filebuf2,"%s+%s",argv[1],argv[2]);
			CleanString(filebuf2);
		}
		if(argc>3) {
			sprintf(filebuf2,"%s+%s+%s",argv[1],argv[2],argv[3]);
			CleanString(filebuf2);
		}
#if DEBUG
		fprintf(fperr,"Check 1\n");
		fflush(fperr);
#endif
		if(((email_all_req || email_req)&&email_req!=3) && ((password_all_req || password_req)&&password_req!=3)) {
			if(strlen(password) && strcmp(acc_password,password)) {
				puts("Content-type: text/html");
				puts("");
				ShowWrongPassword();
				return(0);
			}
			if(!strlen(email_addr) && !strlen(password)) {
				puts("Content-type: text/html");
				puts("");
				ShowEmailPasswordForm(argv[0],filebuf2);
				return(0);
			} else if(!strlen(password)) {
				puts("Content-type: text/html");
				puts("");
				ShowPasswordForm(argv[0],filebuf2);
				return(0);
			}
		} else if((email_all_req || email_req)&&email_req!=3) {
#if DEBUG
			fprintf(fperr,"E-Mail Address Required for external link\nCurrent address is [%s]\n",email_addr);
			fprintf(fperr,"filebuf2 is [%s]\n",filebuf2);
			fflush(fperr);
#endif
			if(!strlen(email_addr)) {
				puts("Content-type: text/html");
				puts("");
				ShowEmailForm(argv[0],filebuf2);
				return(0);
			}
#if DEBUG
			fprintf(fperr,"Check 2\n");
			fflush(fperr);
#endif
		} else if((password_all_req || password_req)&&password_req!=3) {
			if(strlen(password) && strcmp(acc_password,password)) {
				puts("Content-type: text/html");
				puts("");
				ShowWrongPassword();
				return(0);
			}
			if(!strlen(password)) {
				puts("Content-type: text/html");
				puts("");
				ShowPasswordForm(argv[0],filebuf2);
				return(0);
			}
		}
#if DEBUG
		fprintf(fperr,"Checking whether external link access should be logged for this user\n");
		fflush(fperr);
#endif
		if( (retall==0 && ret==-1) || (retall==-1 && ret==0) || (retall==0 && ret==0) || (retall==-1 && ret==-1)) {
			LogEntry(filebuf,1);
		}
		else LogEntry(filebuf,0);
#if DEBUG
		fprintf(fperr,"Cleaning up URL string [%s]\n",argv[1]);
		fflush(fperr);
#endif
		strcpy(filebuf,argv[1]);
		CleanString(filebuf);

#if DEBUG
		fprintf(fperr,"Sending Location line: [%s]\n",filebuf);
		fflush(fperr);
#endif

		printf("Location: %s\n\n",filebuf);
		return(0);
	} else {
		if(argc>2) {
			if(strchr(argv[2],' ') || !strchr(argv[2],'@')) email_addr[0]='\0';
			else strcpy(email_addr,argv[2]);
		}
	}
	puts("Content-type: text/html");
	puts("");
	if(!argv[1]) {
		puts("<html><head><title>PHPL Error</title></head><body>");
		puts("<center><h1>PHPL Error</h1></center>");
		puts("You need to provide a filename argument to <i>phpl.cgi</i>.<p>");
		puts("Syntax is:<br><br>");
		puts("<font size=2><tt>phpl.cgi?path/filename</tt></font><p>");
		puts("</body></html>");
		return(0);
	}
	if(CheckExtension(argv[1],".acc")) {
		puts("<html><head><title>Illegal Operation</title></head>");
		puts("<body>You are not allowed to load a file with the <i>.acc</i>");
		puts("extension through the PHP Tools.<p></body></html>");
		return(0);
	}
	if(!strcasecmp(argv[1],"version")) {
		ShowVersion();
		return(0);
	}
	if(!strcasecmp(argv[1],"env")) {
		ShowEnv();
		return(0);
	}
	if(strstr(argv[1],"..")) {
		puts("<html><head><title>Illegal Operation</title></head>");
		puts("<body>You are not allowed to use '..' components in file paths.");
		puts("<p></body></html>");
		return(0);
	}
#if DEBUG
	fprintf(fperr,"Calling FixFilename with [%s]\n",argv[1]);
	fflush(fperr);
#endif
	FixFilename(argv[1],filebuf);
	strcpy(raw_file,argv[1]);
	CleanString(raw_file);
#if DEBUG
	fprintf(fperr,"Fixed filename is [%s]\n",filebuf);
	fflush(fperr);
#endif
	fflush(stdout);

#if DEBUG
	fprintf(fperr,"filebuf=[%s]\n",filebuf);
#endif
	if(argc>2) {
		sprintf(filebuf2,"%s+%s",argv[1],argv[2]);
		CleanString(filebuf2);
	} else if(argc>1) {
		strcpy(filebuf2,argv[1]);
		CleanString(filebuf2);
	}
	if((ret=ShowFile(filebuf,argv[0],filebuf2,1))==-1) {
		puts("<html><head><title>PHPL Error</title></head><body>");
		puts("<center><h1>PHPL Error</h1></center>");
		printf("The file <b><tt>%s</tt></b> could not be opened for reading!<p>\n",filebuf);
		puts("Make sure the file exists and that the <i>phpl.cgi</i> program has");
		puts("permission to read it.  If the base directory appears");
		puts("incorrect, recompile <i>phpl.cgi</i> with the correct base");
		puts("directory defined in the ROOTDIR variable.<p>");
		puts("Or, if the base directory includes <b>/tmp</b> chances are you");
		puts("have forgotten to set the setuid bit on the PHP binaries.");
		puts("Execute the following command: <tt>chmod u+s php*.cgi</tt><p>");
		puts("</body></html>");	
	} else if(ret==-2) {
		ShowFile(NOACCESS,argv[0],argv[1],0);
	}
#if DEBUG
	fclose(fperr);
#endif

	/* Free the post data linked list if present */
	if(pdtop) {
		pdcur=pdtop;
		while(pdcur) {
			pdtop=pdcur->next;
			free(pdcur);
			pdcur=pdtop;
		}
	}
	return(0);
}
