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
#include <fcntl.h>
#if FILEH
#include <sys/file.h>
#endif
#if LOCKFH
#include <sys/lockf.h>
#endif
#include "html_common.h"
#include "post.h"
#include "common.h"

#ifndef FORMDIR
#define FORMDIR "forms/"
#endif

#if DEBUG
FILE *fperr;
#endif

void PrintFormType(int type, char *field, char *text) {
	switch(type) {
	case 'M':
		if(field) printf("<b>%s:</b> <a href=\"mailto:%s\">%s</a><br>\n",field,text,text);
		else printf("<a href=\"mailto:%s\">%s</a><br>\n",text,text);
		break;
	case 'm':
		if(field) printf("<b>%s:</b> <a href=\"mailto:%s\">%s</a> \n",field,text,text);
		else printf("<a href=\"mailto:%s\">%s</a> \n",text,text);
		break;
	case 'L':
		if(field) printf("<b>%s:</b> <a href=\"%s\">%s</a><br>\n",field,text,text);
		else printf("<a href=\"%s\">%s</a><br>\n",text,text);
		break;
	case 'l':
		if(field) printf("<b>%s:</b> <a href=\"%s\">%s</a> \n",field,text,text);
		else printf("<a href=\"%s\">%s</a> \n",text,text);
		break;
	case 'T':
		if(field) printf("<b>%s:</b> %s<br>",field,text);
		else printf("%s<br>",text);
		break;
	case 't':
		if(field) printf("<b>%s:</b> %s ",field,text);
		else printf("%s ",text);
		break;
	case 'I':
		if(field) printf("<b>%s:</b> <i>%s</i><br>",field,text);
		else printf("<i>%s</i><br>",text);
		break;
	case 'i':
		if(field) printf("<b>%s:</b> <i>%s</i> ",field,text);
		else printf("<i>%s</i> ",text);
		break;
	case 'B':
		if(field) printf("<b>%s:</b> <b>%s</b><br>",field,text);
		else printf("<b>%s</b><br>",text);
		break;
	case 'b':
		if(field) printf("<b>%s:</b> <b>%s</b> ",field,text);
		else printf("<b>%s</b> ",text);
		break;
	}
}

int main(int argc, char *argv[]) {
	char buf[1024];
	char lastline[1024];
	char fn[256];
	char *s, *ss, *t, *res=NULL;
	FILE *fp;
	int i;
	int prev=0;
	int dynamic;
	int FormData;
	int nodata=0;
	char divider[64];
	int divi=0;

	setuid(geteuid());

#if DEBUG
	fperr=fopen("phpf.err","w");
	fprintf(fperr,"PHPF Starting\n");
	fflush(fperr);
#endif

	if(argc==1) {
		puts("Content-type: text/html");
		puts("");
		puts("<html><head><title>PHPF Usage</title></head><body>");
		puts("<center><h1>PHPF Usage</h1></center>");
		puts("Call <i>phpf.cgi</i> with a url like <tt>http://host@domain/~user/phpf.cgi?arg</tt> where arg is one of:<br>");
		puts("<pre>\t<tt>form+dest_url</tt></pre>");
		puts("</body></html>");
#if DEBUG
		fclose(fperr);
#endif
		return(0);
	}
	if(!strcasecmp(argv[1],"version")) {
		puts("Content-type: text/html");
		puts("");
		ShowVersion();
#if DEBUG
		fclose(fperr);
#endif
		return(0);
	}
	if(!strcasecmp(argv[1],"env")) {
		puts("Content-type: text/html");
		puts("");
		ShowEnv();
#if DEBUG
		fclose(fperr);
#endif
		return(0);
	}

	if(argc>2 && !strcasecmp(argv[1],"form") && !strcasecmp(argv[2],"debug")) {
		html_head("Post Results");
		res=getpost();
		if(CheckResult(res)) printf("[%s]<p>",res);
		fflush(stdout);
		puts("<pre>");
		system("env");
		fflush(stdout);
		puts("</pre>");
		puts("</body></html>");
		free(res);
#if DEBUG
		fclose(fperr);
#endif
		return(0);
	}
	dynamic=1;
	if(!strcasecmp(argv[1],"form") || !(dynamic=strcasecmp(argv[1],"dynamic"))) {
		if(dynamic==0) dynamic=1;
		else if(dynamic==1) dynamic=0;

		if(!dynamic) {
			if(FORMDIR[0]!='/') FixFilename(FORMDIR,buf);
			else strcpy(buf,FORMDIR);
			ss=buf+strlen(buf)-1;
			if(*ss!='/') strcat(buf,"/");
			if(argc>3) {
				if(FixFilename(argv[3],buf)<0) {
					html_error("PHPF Error","Don't use '..' in filenames");
					return(0);
				}
			} else if(getenv("HTTP_REFERER")) {
				strcpy(fn,getenv("HTTP_REFERER"));
				s=strrchr(fn,'/');
				ss=strrchr(fn,'?');
				if(!s && ss) s=ss;
				else if(s && ss && ss>s) s=ss;
				if(s) s++;
				t=strrchr(fn,'.');
				if(t) *t='\0';	
				strcat(buf,(s)?s:fn);
				strcat(buf,".res");
			} else {
				strcat(buf,"form.res");
			}
			fp=fopen(buf,"a");
			if(!fp) {
				html_error("PHPF Error","Unable to open results file!");
				return(0);
			}
#if FLOCK
			flock(fileno(fp),LOCK_EX);
#endif
#if LOCKF
			lockf(fileno(fp),F_LOCK,1024);
#endif
		} else {
			if(argc<3) {
				return(0);
			}
			if(FixFilename(argv[2],buf)<0) {
				html_error("PHPF Error","Don't use '..' in filenames");
				return(0);
			}
			fp=fopen(buf,"r+");
			if(!fp) {
				html_error("PHPF Error","Unable to open Form file");
				return(0);
			}
#if FLOCK
			flock(fileno(fp),LOCK_EX);
#endif
#if LOCKF
			lockf(fileno(fp),F_LOCK,1024);
#endif
			FormData=0;
			puts("Content-type: text/html");
			puts("");
			divider[0]='\0';
			divi=0;
			while(!feof(fp)) {
				if(fgets(buf,1023,fp)) {
					s=buf+strlen(buf)-1;
					if(*s=='\n') *s='\0';
					if(!FormData) {
						if(!strstr(buf,"</body>")) puts(buf);
						else {
							strcpy(lastline,buf);
							FormData=1;
							continue;
						}
					} else {
						s=strchr(buf,'=');
						if(!s) {
							PrintFormType(prev,NULL,buf);
							continue;
						}
						if((int)strlen(s)<2) continue;
						*s='\0';
						if(strlen(divider) && !strcasecmp(&argv[(divi<argc-1?divi+1:0)][1],buf)) puts(divider);
						for(i=3;i<argc;i++) {
							if(strlen(divider)==0 && (argv[i][0]=='x' || argv[i][0]=='X')) {
								strcpy(divider,&argv[i][1]);
								CleanString(divider);
								divi=i;
								continue;
							}
							if(!strcasecmp(buf,&argv[i][1])) {
								PrintFormType(argv[i][0],buf,s+1);
								prev=argv[i][0];
							}
						}
					}	
				}
			}
		}
		if(!strcasecmp(getenv("REQUEST_METHOD"),"post")) {
			res=getpost();
			divider[0]='\0';
			divi=0;
			if(CheckResult(res)) {
				parse_url((unsigned char *)res);
				s=strtok(res,"&");
				while(s) {
					fprintf(fp,"%s\n",s);
					t=strchr(s,'=');
					if(!t) {
						PrintFormType(prev,NULL,s);
						s=strtok(NULL,"&");
						continue;
					}
					if((int)strlen(t)<2) {
						s=strtok(NULL,"&");
						continue;
					}
					*t='\0';
					if(strlen(divider) && !strcasecmp(&argv[(divi<argc-1?divi+1:0)][1],buf)) puts(divider);
					for(i=3;i<argc;i++) {
						if(strlen(divider)==0 && (argv[i][0]=='x' || argv[i][0]=='X')) {
							strcpy(divider,&argv[i][1]);
							CleanString(divider);
							divi=i;
							continue;
						}
						if(!strcasecmp(s,&argv[i][1])) {
							PrintFormType(argv[i][0],s,t+1);
							prev=argv[i][0];
						}
					}
					s=strtok(NULL,"&");
				}
				nodata=0;
			} else { nodata=1; }
		} else { nodata=1; }
		fflush(fp);
#if FLOCK
		flock(fileno(fp),LOCK_UN);
#endif
#if LOCKF
		lockf(fileno(fp),F_LOCK,1024);
#endif
		fclose(fp);
		free(res);
		if(!dynamic) {
			if(argc>2) {
				strcpy(buf,argv[2]);
				parse_url((unsigned char *)buf);
				CleanString(buf);
				printf("Location: %s\n\n",buf);
			} else {
				html_head("Form Submittal");
				puts("<body><h2>Thank You!</h2></body></html>");
			}
		} else {
			puts(lastline);
		}
		fflush(stdout);
#if DEBUG
		fclose(fperr);
#endif
		return(0);
	}
	if(!strcasecmp(argv[1],"show")) {
		if(argc<4) {
			html_head("PHPT Error");
			puts("<body>Missing arguments.  Format is phpf.cgi?show+file+arg1+arg2+...");
			puts("</body></html>");
#if DEBUG
			fclose(fperr);
#endif
			return(0);
		}
		FixFilename(argv[2],buf);
		fp=fopen(buf,"r");
		if(!fp) {
			html_head("PHPF Error");
			puts("<body><h2>Unable to open results file.</h2>");
			puts("Apparently nobody has filled in this form before.  Why");
			puts("don't you go ahead and be the first?<p>");
			puts("</body></html>");
#if DEBUG
			fclose(fperr);
#endif
			return(0);
		}
		puts("Content-type: text/html");
		puts("");
		strcpy(buf,argv[2]);
		s=strrchr(buf,'.');
		if(s) *s='\0';
		s=strrchr(buf,'/');
		printf("<html><head><title>%s Results</title></head>\n",s?s+1:buf);
		printf("<body><center><h2>%s Results</h2></center>\n",s?s+1:buf); 
		divider[0]='\0';
		divi=0;
		while(!feof(fp)) {
			if(fgets(buf,1023,fp)) {
				s=buf+strlen(buf)-1;
				if(*s=='\n') *s='\0';
				s=strchr(buf,'=');
				if(!s) {
					PrintFormType(prev,NULL,buf);
					continue;
				}
				if((int)strlen(s)<2) continue;
				*s='\0';
				if(strlen(divider) && !strcasecmp(&argv[(divi<argc-1?divi+1:0)][1],buf)) puts(divider);
				for(i=3;i<argc;i++) {
					if(strlen(divider)==0 && (argv[i][0]=='x' || argv[i][0]=='X')) {
						strcpy(divider,&argv[i][1]);
						CleanString(divider);
						divi=i;
						continue;
					}
					if(!strcasecmp(buf,&argv[i][1])) {
						PrintFormType(argv[i][0],buf,s+1);
						prev=argv[i][0];
					}
				}
			}
		}
		puts("</body></html>");
		fflush(stdout);
		fclose(fp);
	}
#if DEBUG
	fclose(fperr);
#endif
	return(0);
}
