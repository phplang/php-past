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
#include <sys/types.h>
#include <time.h>
#include "config.h"
#include "common.h"
#include "post.h"
#include "version.h"

#ifndef LOGDIR
#define LOGDIR "logs/"
#endif

static char *Month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

#if DEBUG
	FILE *fperr;
#endif

int main(int argc, char *argv[]) {
	FILE *fp;
	char buf[512];
	char fn[128];
	char *s, *t, *ss, *tt;
	char *r=NULL, *e=NULL;
	int fancy=1;
	int days=0;
	time_t tmnow;
	struct tm *tm1;
	int show_ref=0;
	int show_host=0;
	int show_agent=0;
	int show_date=0;
	int show_email=0;
	char *res=NULL;

	setuid(geteuid());

#if DEBUG
	fperr=fopen("phplview.err","w");
	fprintf(fperr,"phplview starting\n");
	fflush(fperr);
#endif

	if(argc<2) return(0);
	if(strlen(argv[1])==0) return(0);
	if(!strcasecmp(argv[1],"version")) {
		puts("Content-type: text/html");
		puts("");
		ShowVersion();
		return(0);
	}
	if(!strcasecmp(argv[1],"env")) {
		puts("Content-type: text/html");
		puts("");
		ShowEnv();
		return(0);
	}
	if(!strcasecmp(getenv("REQUEST_METHOD"),"post")) {
		res=getpost();
		if(CheckResult(res)) {
			parse_url((unsigned char *)res);
			s=strtok(res,"&");
			while(s) {
				t=strchr(s,'=');
				if(t) *t='\0';
				if(!strcmp(s,"show_date")) show_date=1;
				if(!strcmp(s,"show_ref")) show_ref=1;
				if(!strcmp(s,"show_host")) show_host=1;
				if(!strcmp(s,"show_agent")) show_agent=1;
				if(!strcmp(s,"show_email")) show_email=1;
				if(!strcmp(s,"text_only")) fancy=0;
				if(!strcmp(s,"today_only")) days=1;
				s=strtok(NULL,"&");
			}
		}
	}
	if(argc>2) {
		if(strchr(argv[2],'a')) fancy=0;
		if(strchr(argv[2],'b')) show_agent=1;
		if(strchr(argv[2],'d')) show_date=1;
		if(strchr(argv[2],'e')) show_email=1;
		if(strchr(argv[2],'h')) show_host=1;
		if(strchr(argv[2],'r')) show_ref=1;
		if(strchr(argv[2],'t')) days=1;
	}

#if DEBUG
	fprintf(fperr,"arg1=[%s], arg2=[%s]\n",argv[1],argv[2]);
	fflush(fperr);
#endif

	if(argv[1][0]=='/') strcpy(buf,argv[1]);
	else if(FixFilename(argv[1],buf)<0) {
		puts("<html><head><title>PHPLVIEW Error</title></head>");
		puts("<body>Do not use '..' in filenames</body></html>");
		return(0);
	}

#if DEBUG
	fprintf(fperr,"Trying to open [%s] for reading\n",buf);
	fflush(fperr);
#endif

	fp=fopen(buf,"r");
	if(!fp) return(0);
	puts("Content-type: text/html");
	puts("");
	strcpy(fn,argv[1]);
	s=strrchr(fn,'.');
	if(s) *s='\0';
	s=strrchr(fn,'/');
	printf("<html><head><title>Log for %s.html</title></head><body>\n",(s)?s+1:fn);
	if(fancy) {
		puts("<font size=2><center><table border>");
		if(days==0) printf("<caption>Full Log for %s.html</caption>\n",(s)?s+1:fn);
		else if(days==1) printf("<caption>Log for %s.html for today</caption>\n",(s)?s+1:fn);
		if(show_date) puts("<th>Data/Time</th>");
		if(show_host) puts("<th>Domain</th>");
		if(show_email) puts("<th>E-Mail</th>");
		if(show_ref) puts("<th>From URL</th>");
		if(show_agent) puts("<th>Browser</th>");
		puts("</tr>");
	} else {
		if(days==0) printf("<h1>Full Log for %s.html</h1>\n",(s)?s+1:fn);
		else if(days==1) printf("<h1>Log for %s.html for today</h1>\n",(s)?s+1:fn);
		printf("<hr><pre><u>");
		if(show_date) printf("<i><b>Date   Time</b></i>    ");
		if(show_host) printf("<i><b>%-36s</b></i>","Domain");
		if(show_email) printf("<i><b>%-36s</b></i>","E-Mail");
		if(show_ref) printf("<i><b>%-36s</b></i>","From URL");
		if(show_agent) printf("<i><b>Browser</b></i>");
		printf("</u>\n");
	}
	fflush(stdout);
	while(!feof(fp)) {
		if(fgets(buf,511,fp)) {
			t=buf+strlen(buf)-1;
			if(*t=='\n' || *t=='\r') *t='\0';
			s=strchr(buf,' ');
			s=strchr(s+1,' ');
			s=strchr(s+1,' ');
			t=strchr(s+1,' ');
			e=strchr(t+1,27);
			if(e) { *e='\0'; r=strchr(e+1,' '); }
			if(r) *r='\0';
			if(s) *s='\0';
			if(t) *t='\0';
			if(days==0) {
				if(fancy) {
					printf("<tr>");
					if(show_date) printf("<td><b>%s</b></td> ",buf);
					if(show_host) printf("<td><b>%s</b></td> ",s+1);
					if(show_email) {
						if(e && strcmp(e+1,"none")) printf("<td><a href=\"mailto:%s\">%s</a></td> ",e+1,e+1);
						else printf("<td><b>none</b></td> ");
					}
					if(show_ref) {
						if(r && strcmp(r+1,"none")) printf("<td><a href=\"%s\">%s</a></td> ",r+1,r+1);
						else printf("<td>none</td> ");
					}
					if(show_agent) printf("<td><b>%s</b></td>",t+1);
					printf("</tr>\n");
				} else {
					if(show_date) printf("<b>%-15s</b>",buf);
					if(show_host) printf("<i>%-36s</i>",s+1);
					if(show_email) {
						if(e && strcmp(e+1,"none")) printf("<a href=\"mailto:%s\">%-36s</a>",e+1,e+1);
						else printf("<i>%-36s</i>","none");
					}
					if(show_ref) {
						if(r && strcmp(r+1,"none")) printf("<a href=\"%s\">%-36s</a>",r+1,r+1);
						else printf("<i>%-36s</i>","none");
					}
					if(show_agent) printf("%s",t+1);
					printf("\n");
				}
			} else {
				tmnow=time(NULL);
				tm1=localtime(&tmnow);
				ss=strchr(buf,' ');
				if(!ss) continue;
				*ss='\0';
				tt=ss+1;
				if(strcmp(buf,Month[tm1->tm_mon])) continue;
				*ss=' ';
				ss=strchr(tt,' ');
				if(!ss) continue;
				*ss='\0';
				if(tm1->tm_mday != atoi(tt)) continue;
				*ss=' ';
				if(fancy) {
					printf("<tr>");
					if(show_date) printf("<td><b>%s</b></td> ",buf);
					if(show_host) printf("<td><b>%s</b></td> ",s+1);
					if(show_email) {
						if(e && strcmp(e+1,"none")) printf("<td><a href=\"mailto:%s\">%s</a></td> ",e+1,e+1);
						else printf("<td><b>none</b></td> ");
					}
					if(show_ref) {
						if(r && strcmp(r+1,"none")) printf("<td><a href=\"%s\">%s</a></td> ",r+1,r+1);
						else printf("<td>none</td> ");
					}
					if(show_agent) printf("<td><b>%s</b></td>",t+1);
					printf("</tr>\n");
				} else {
					if(show_date) printf("<b>%-15s</b>",buf);
					if(show_host) printf("<i>%-36s</i>",s+1);
					if(show_email) {
						if(e && strcmp(e+1,"none")) printf("<a href=\"mailto:%s\">%-36s</a>",e+1,e+1);
						else printf("<i>%-36s</i>","none");
					}
					if(show_ref) {
						if(r && strcmp(r+1,"none")) printf("<a href=\"%s\">%-36s</a>",r+1,r+1);
						else printf("<i>%-36s</i>","none");
					}
					if(show_agent) printf("%s",t+1);
					printf("\n");
				}
			}
		}	
	}
	fclose(fp);
	if(fancy) puts("</table></center></font><hr>");
	else puts("</pre><hr>");
	printf("<form action=\"phplview.cgi?%s\" method=\"POST\">\n",argv[1]);
	puts("Redisplay this page with the following columns:<br>\n");
	printf("<input type=\"checkbox\" name=\"show_date\" %s>\n",(show_date?"checked":""));
	puts("Date and Time<br>");
	printf("<input type=\"checkbox\" name=\"show_host\" %s>\n",(show_host?"checked":""));
	puts("Domain Name<br>");
	printf("<input type=\"checkbox\" name=\"show_email\" %s>\n",(show_email?"checked":""));
	puts("Email Address<br>");
	printf("<input type=\"checkbox\" name=\"show_ref\" %s>\n",(show_ref?"checked":""));
	puts("Referring Document<br>");
	printf("<input type=\"checkbox\" name=\"show_agent\" %s>\n",(show_agent?"checked":""));
	puts("Browser<br>");
	printf("<input type=\"checkbox\" name=\"today_only\" %s>\n",(days?"checked":""));
	puts("Today Only<br>");
	printf("<input type=\"checkbox\" name=\"text_only\" %s>\n",(fancy?"":"checked"));
	puts("Text Only<br>");
	puts("<input type=\"submit\" value=\"Redisplay\">");
	printf("<hr><font size=-1><i><a href=\"%s\">PHP Tools</a> Copyright &copy 1995 Rasmus Lerdorf</i></font><br>\n",PHPURL);
	puts("</body></html>");

#if DEBUG
	fclose(fperr);
#endif

	return(0);
}
