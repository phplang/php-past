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
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#if DIRECT
#include <sys/dir.h>
#else 
#include <dirent.h>
#endif
#include <string.h>
#include "config.h"
#include "common.h"
#include "version.h"

#ifndef LOGDIR
#define LOGDIR  "logs/"
#endif

#if DEBUG
FILE *fperr;
#endif

typedef struct ref_struct {
	char *ref_url;
	struct ref_struct *next;
} ref_struct;

void ShowStats(int fancy) {
	DIR *d;
#if DIRECT
	struct direct *de;
#else
	struct dirent *de;
#endif
	char buf[256];
	char fn[128];
	FILE *fp;
	char *s, *t;
	char logdir[256];

#if DEBUG
	fprintf(fperr,"ShowStats called.  Fancy=%d\n",fancy);
	fflush(fperr);
#endif

	if(LOGDIR[0]!='/') FixFilename(LOGDIR,logdir);
	else strcpy(logdir,LOGDIR);
	s=logdir+strlen(logdir)-1;
	if(*s != '/') strcat(logdir,"/");

#if DEBUG
	fprintf(fperr,"About to open logdir [%s]\n",logdir);
	fflush(fperr);
#endif

	d=opendir(logdir);
	if(!d) {
		puts("<html><head>PHPLMON Error</head><body>");
		printf("Unable to open log directory (%s)\n<p></body></html>",logdir);	
		return;
	}
	puts("<html><head><title>PHPL Page Stats</title></head>");
	puts("<body>");
	if(fancy) {
		puts("<center><table border> <caption>PHPL Page Stats</caption>");
		puts("<tr> <th>HTML Page</th> <th>Total</th> <th>Today</th> <th>Last Access</th> <th>By</th> </tr>");
	} else {
		puts("<pre><u>  <i><b>HTML Page</b></i>\t  <i><b>Total</b></i>  <i><b>Today</b></i>   <i><b>Last Access</b></i>    <i><b>By</b></i></u>");
	}
	while((de=readdir(d))) {
		if(CheckExtension(de->d_name,".cnt")) {
			strcpy(fn,de->d_name);
			sprintf(buf,"%s%s",logdir,de->d_name);
#if DEBUG
			fprintf(fperr,"Opening [%s]\n",buf);
			fflush(fperr);
#endif
			fp=fopen(buf,"r");
			if(!fp) continue;
			s=strrchr(fn,'.');
			if(s) *s='\0';
			if(fancy) puts("<tr>");
			if(fgets(buf,255,fp)) {

#if DEBUG
				fprintf(fperr,"Read [%s]\n",buf);
				fflush(fperr);
#endif

				t = buf+strlen(buf)-1;
				if(*t=='\n' || *t=='\r') *t='\0';
				t=strchr(buf,' ');
				if(t) *t='\0';

#if DEBUG	
				fprintf(fperr,"About to print\n");
				fflush(fperr);
#endif

				if(fancy) {
					if(t) printf("<td><a href=\"phplmon.cgi?ref_show+%s.html\">%s.html</a></td> <td align=\"right\"><a href=\"phplview.cgi?%s%s.log+dheb\">%s</a></td> <td align=\"right\"><a href=\"phplview.cgi?%s%s.log+tdheb\">%s</a></td> ",
						fn,fn,logdir,fn,buf,logdir,fn,t+1);
					else printf("<td><a href=\"phplmon.cgi?ref_show+%s.html\">%s.html</a></td> <td align=\"right\"><a href=\"phplview.cgi?%s%s.log+dheb\">%s</a></td> <td align=\"right\">0</td> ",fn,fn,logdir,fn,buf);
				} else {
					if(t) printf("<a href=\"phplmon.cgi?ref_show+%s\">%-16s</a><a href=\"phplview.cgi?%s%s.log+adheb\">%4s</a> <a href=\"phplview.cgi?%s%s.log+atdheb\">%4s</a> ",fn,fn,logdir,fn,buf,logdir,fn,t+1);
					else printf("<a href=\"phplmon.cgi?ref_show+%s\">%-16s</a><a href=\"phplview.cgi?%s%s.log+adheb\">%4s</a>    0 ",fn,fn,logdir,fn,buf);
				}
				if(fgets(buf,255,fp)) {
					t = buf+strlen(buf)-1;
					if(*t=='\n' || *t=='\r') *t='\0';
					if(fancy) printf("<td>%s</td> ",buf);
					else printf("%-15s ",buf);
					if(fgets(buf,255,fp)) {
						t = buf+strlen(buf)-1;
						if(*t=='\n' || *t=='\r') *t='\0';
						if(fancy) printf("<td><i>%s</i></td>\n",buf);
						else printf("<i>%-15s</i>\n",buf);
					}
				}
			}
			fclose(fp);
			if(fancy) puts("</tr>");
		}
	}
	if(fancy) puts("</table></center>");
	else puts("</pre>");
	printf("<hr><font size=-1><i><a href=\"%s\">PHP Tools</a> Copyright &copy 1995 Rasmus Lerdorf</i></font><br>\n",PHPURL);
	puts("</body></html>");
	fflush(stdout);
	closedir(d);
} /* ShowStats */

/*
 * Reads reference url's from a .ref file previously opened
 */
struct ref_struct *ReadRefs(FILE *fp) {
	char buf[256];
	struct ref_struct *r=NULL;
	static struct ref_struct *ref;
	int first_time=1;
	char *s=NULL;

	ref=NULL;
	while(!feof(fp)) {
		if(fgets(buf,255,fp)) {
			s=buf+strlen(buf)-1;
			if(*s=='\n') *s='\0';
			if(first_time) {
				r=(struct ref_struct *)malloc(sizeof(struct ref_struct));
				ref=r;
				first_time=0;
			} else {
				r->next=(struct ref_struct *)malloc(sizeof(struct ref_struct));
				r=r->next;
			}
			if(!r) continue;
			r->ref_url = malloc(sizeof(char)*(strlen(buf)+1));
			if(!r->ref_url) continue;
#if DEBUG
			fprintf(fperr,"Adding url to linked list: [%s]\n",buf);
			fflush(fperr);
#endif
			strcpy(r->ref_url,buf);
			r->next=NULL;
		}
	}
	return(ref);	
} /* ReadRefs */
			
int ScanRefs(FILE *fp, struct ref_struct **ref) {
	struct ref_struct *r=NULL;
	struct ref_struct *last=NULL;
	char *s, *t;
	char buf[512];
	int refs_changed=0;
	int match=0;

	while(!feof(fp)) {
		r=*ref;
		match=0;
		if(fgets(buf,511,fp)) {
			s=buf+strlen(buf)-1;
			if(*s=='\n') *s='\0';
			s=strrchr(buf,27);
			if(s) {
				t=strchr(s,' ');
				if(t) {
					if(!strcmp(t+1,"none")) continue;
					if(strncasecmp(t+1,"http",4)) continue;
					if(!*ref) {
						*ref=(struct ref_struct *)malloc(sizeof(struct ref_struct));
						if(*ref) {
							r=*ref;
							(*ref)->ref_url=malloc(sizeof(char)*(strlen(t+1)+1));
							s=strrchr(t+1,'+');
							if(s) *s='\0';
							if((*ref)->ref_url) {
								strcpy((*ref)->ref_url,t+1);
								refs_changed=1;
#if DEBUG
								fprintf(fperr,"Adding [%s] to linked list\n",t+1);
								fflush(fperr);
#endif
							}
						}
					} else {
						s=strrchr(t+1,'+');
						if(s) *s='\0';
						r=*ref;
						while(r) {
							if(strcmp(r->ref_url,t+1)) {
								last=r;
								r=r->next;
								match=0;
								continue;
							} else {
								last=NULL;
								match=1;
								break;
							}
						}
						if(!match && last) {
							last->next=(struct ref_struct *)malloc(sizeof(ref_struct));
							if(last->next) {
								last->next->ref_url=malloc(sizeof(char)*(strlen(t+1)+1));
								if(last->next->ref_url) {
									strcpy(last->next->ref_url,t+1);
#if DEBUG
									fprintf(fperr,"Adding [%s] to linked list\n",t+1);
									fflush(fperr);
#endif
									refs_changed=1;
									last->next->next=NULL;
								}
							}
						}
					}
				}
			}
		}
	} /* while */
	return(refs_changed);
} /* ScanRefs */

void ShowRefs(char *filename) {
	char logdir[256];
	char fn[256];
	char buf[512];
	FILE *fp;
	char *s;

	if(LOGDIR[0]!='/') FixFilename(LOGDIR,logdir);
	else strcpy(logdir,LOGDIR);
	s=logdir+strlen(logdir)-1;
	if(*s != '/') strcat(logdir,"/");
	strcpy(fn,filename);
	s=strrchr(fn,'.');
	if(s) *s='\0';
	sprintf(buf,"%s%s.ref",logdir,fn);

#if DEBUG
	fprintf(fperr,"Opening [%s] for reading\n",buf);
	fflush(fperr);
#endif

	fp=fopen(buf,"r");
	if(!fp) return;
	strcpy(buf,filename);
	s=strrchr(buf,'.');
	if(s) *s='\0';
	printf("<h2>Reference URL's for %s.html</h2>\n<ul>\n",buf);
	while(!feof(fp)) {
		if(fgets(buf,511,fp)) {
			s=buf+strlen(buf)-1;
			if(*s=='\n') *s='\0';
			printf("<li><a href=\"%s\">%s</a><br>\n",buf,buf);
		}
	}
	fclose(fp);
	puts("</ul><hr>");
}

void ShowAllRefs(void) {
	DIR *d;
#if DIRECT
	struct direct *de;
#else
	struct dirent *de;
#endif
	char logdir[256];
	char *s;

	if(LOGDIR[0]!='/') FixFilename(LOGDIR,logdir);
	else strcpy(logdir,LOGDIR);
	s=logdir+strlen(logdir)-1;
	if(*s != '/') strcat(logdir,"/");

	d=opendir(logdir);
	if(!d) {
		puts("PHPLMON Error<p>");
		printf("Unable to open log directory (%s)\n<p>",logdir);	
		return;
	}
	while((de=readdir(d))) {
		if(CheckExtension(de->d_name,".ref")) {
			ShowRefs(de->d_name);
		}
	}
	closedir(d);
}

void UpdateRefs(char *filename) {
	FILE *fp;
	char buf[256];
	char fn[256];
	char logdir[256];
	char *s;
	int refs_changed=0;
	struct ref_struct *ref=NULL;
	struct ref_struct *r;
	struct ref_struct *last;

	if(LOGDIR[0]!='/') FixFilename(LOGDIR,logdir);
	else strcpy(logdir,LOGDIR);
	s=logdir+strlen(logdir)-1;
	if(*s != '/') strcat(logdir,"/");
	strcpy(fn,filename);
	s=strrchr(fn,'.');
	if(s) *s='\0';
	sprintf(buf,"%s%s.ref",logdir,fn);
	fp=fopen(buf,"r");
	if(fp) {
		ref=ReadRefs(fp);
		fclose(fp);
	}
	sprintf(buf,"%s%s.log",logdir,fn);
#if DEBUG
	fprintf(fperr,"Opening [%s]\n",buf);
	fflush(fperr);
#endif
			
	fp=fopen(buf,"r");
	if(!fp) goto done;
	refs_changed=ScanRefs(fp,&ref);
	fclose(fp);
	if(refs_changed) {
		sprintf(buf,"%s%s.ref",logdir,fn);
#if DEBUG
		fprintf(fperr,"Opening [%s]\n",buf);
		fflush(fperr);
#endif
		fp=fopen(buf,"w");
		if(fp) {
			r=ref;
			while(r) {
				fprintf(fp,"%s\n",r->ref_url);
				r=r->next;
			}
			fclose(fp);
		}
	}	
done:
	if(ref) {
		r=ref;
		while(r) {
			last=r;
			r=r->next;
			free(last->ref_url);
			free(last);
		}
		free(ref);
		ref=NULL;
		last=NULL;	
	}
} /* UpdateRefs */

void UpdateAllRefs() {
	DIR *d;
#if DIRECT
	struct direct *de;
#else
	struct dirent *de;
#endif
	char *s;
	char logdir[256];

	if(LOGDIR[0]!='/') FixFilename(LOGDIR,logdir);
	else strcpy(logdir,LOGDIR);
	s=logdir+strlen(logdir)-1;
	if(*s != '/') strcat(logdir,"/");

#if DEBUG
	fprintf(fperr,"About to open logdir [%s]\n",logdir);
	fflush(fperr);
#endif

	d=opendir(logdir);
	if(!d) {
		puts("<html><head>PHPLMON Error</head><body>");
		printf("Unable to open log directory (%s)\n<p></body></html>",logdir);	
		return;
	}
	while((de=readdir(d))) {
		if(CheckExtension(de->d_name,".log")) {
			UpdateRefs(de->d_name);
		}
	}
	closedir(d);
} /* UpdateAllRefs */

int main(int argc, char *argv[]) {
	int fancy;
	int ref;
	int show=0;

#if DEBUG
	fperr=fopen("phplmon.err","w");
	if(!fperr) {
		puts("Content-type: text/html");
		puts("");
		puts("<html><head>PHPLMON Error</head><body>");
		puts("Unable to open error file</body></html>");
		return(-1);
	}
#endif

	setuid(geteuid());
	if(argc>1 && !strcasecmp(argv[1],"version")) {
		puts("Content-type: text/html");
		puts("");
		ShowVersion();
		return(0);
	} 
	if(argc>1 && !strcasecmp(argv[1],"env")) {
		puts("Content-type: text/html");
		puts("");
		ShowEnv();
		return(0);
	}
	if(argc>1 && !strcasecmp(argv[1],"text")) fancy=0;
	else fancy=1;
	
	if(argc>1 && !strcasecmp(argv[1],"ref_update")) {
		if(argc>2 && strlen(argv[2])) ref=2;
		else ref=1;
	}
	else ref=0;

	if(argc>1 && !strcasecmp(argv[1],"ref_show")) {
		if(argc>2 && strlen(argv[2])) show=2;
		else show=1;
	}
	else show=0;


	puts("Content-type: text/html");
	puts("");
	if(ref==1) {
		UpdateAllRefs();
		puts("<html><head><title>Finished Updating All URL Ref Pages</title></head>");
		puts("<body><center><h1>Finished Updating All URL Ref Pages</h1></center>");
		ShowAllRefs();
		printf("<hr><font size=-1><i><a href=\"%s\">PHP Tools</a> Copyright &copy 1995 Rasmus Lerdorf</i></font><br>\n",PHPURL);
		puts("</body></html>");
	} else if(ref==2) {
		UpdateRefs(argv[2]);
		puts("<html><head><title>Finished Updating Ref Page</title></head>");
		puts("<body><center><h1>Finished Updating Ref Page</h1></center>");
		ShowRefs(argv[2]);
		printf("<hr><font size=-1><i><a href=\"%s\">PHP Tools</a> Copyright &copy 1995 Rasmus Lerdorf</i></font><br>\n",PHPURL);
		puts("</body></html>");
	} else if(show==1) {
		puts("<html><head><title>Referring URL List for All Pages</title></head>");
		puts("<body><center><h1>Referring URL List for All Pages</h1></center>");
		ShowAllRefs();
		printf("<a href=\"phplmon.cgi?ref_update\">Update Reference URL's for All pages</a><br>\n");
		printf("<hr><font size=-1><i><a href=\"%s\">PHP Tools</a> Copyright &copy 1995 Rasmus Lerdorf</i></font><br>\n",PHPURL);
		puts("</body></html>");
	} else if(show==2) {
		puts("<html><head><title>Referring URL List</title></head>");
		puts("<body><center><h1>Referring URL List</h1></center>");
		ShowRefs(argv[2]);
		printf("<a href=\"phplmon.cgi?ref_show\">Show Reference URL's for All pages</a><br>\n");
		printf("<a href=\"phplmon.cgi?ref_update+%s\">Update Reference URL's for this page</a><br>\n",argv[2]);
		printf("<a href=\"phplmon.cgi?ref_update\">Update Reference URL's for All pages</a><br>\n");
		printf("<hr><font size=-1><i><a href=\"%s\">PHP Tools</a> Copyright &copy 1995 Rasmus Lerdorf</i></font><br>\n",PHPURL);
		puts("</body></html>");
	}
	else ShowStats(fancy);
#if DEBUG
	fclose(fperr);
#endif
	return(0);
}
