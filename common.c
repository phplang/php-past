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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "config.h"
#include "version.h"

#if DEBUG
extern FILE *fperr;
#endif

void ShowVersion(void) {
	puts("<html><head><title>PHP Tools Version Information</title></head><body><hr width=400>");
	printf("<font size=+2><center>PHP Tools Version %s</center></font><br>\n",VERSION);
	printf("<font size=+2><center>by <a href=\"mailto:%s\">%s</a></center></font><br>\n",EMAIL,AUTHOR);
	printf("<font size=+1><center><a href=\"%s\">PHP Tools Home Page</a></center></font><hr width=400>\n",PHPURL);
	puts("Copyright &copy 1995 Rasmus Lerdorf<br><br>");
    puts("This program is free software; you can redistribute it and/or modify");
    puts("it under the terms of the GNU General Public License as published by");
    puts("the Free Software Foundation.<p>");
    puts("This program is distributed in the hope that it will be useful,");
    puts("but WITHOUT ANY WARRANTY; without even the implied warranty of");
    puts("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
    puts("GNU General Public License for more details.<p>");
    puts("You should have received a copy of the GNU General Public License");
    puts("along with this program; if not, write to the Free Software");
    puts("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.<hr>");

	puts("<h2>System Info</h2>");
	fflush(stdout);
	system("uname -a");
	fflush(stdout);
	puts("<hr width=400></body></html>");
}

void ShowEnv(void) {
	struct passwd *pw;
	char buf[256];

	pw=getpwuid(geteuid());
	puts("<html><head><title>HTTP Environment</title></head><body>");
	puts("<center><h1>HTTP Environment</h1></center><hr width=400>");
	printf("<pre>");
	fflush(stdout);
	system("env");
	printf("</pre>");
#if defined(ROOTDIR) && defined(HTML_DIR)
	sprintf(buf,"%s/%s",ROOTDIR, HTML_DIR);
#else
#	ifdef ROOTDIR
		strcpy(buf,ROOTDIR);
#	endif
#	ifdef HTML_DIR
		sprintf(buf,"%s/%s",pw->pw_dir, HTML_DIR);
#	endif
#	if !defined(ROOTDIR) && !defined(HTML_DIR)
		sprintf(buf,"%s/public_html",pw->pw_dir);
#	endif
#endif
	printf("\nBase HTML directory = <tt>%s</tt>\n<p>",buf);
	printf("The following have been defined in config.h for this system:\n<p><tt>");
#ifdef ROOTDIR
	printf("ROOTDIR=%s\n<br>",ROOTDIR);
#endif

#ifdef LOGDIR
	printf("LOGDIR=%s\n<br>",LOGDIR);
#endif
#ifdef ACCDIR
	printf("ACCDIR=%s\n<br>",ACCDIR);
#endif
#ifdef HTML_DIR
	printf("HTML_DIR=%s\n<br>",HTML_DIR);
#endif
#ifdef NOACCESS
	printf("NOACCESS=%s\n<br>",NOACCESS);
#endif
	printf("</tt><br>Directory where the PHP Tools binaries live = \n<tt>");
	fflush(stdout);
	system("pwd");
	fflush(stdout);
	printf("</tt><br><pre>");
	fflush(stdout);
	system("ls -la");
	printf("</pre>");
	fflush(stdout);
	puts("<hr width=400></body></html>");
}

int CheckExtension(char *filename, char *ext) {
    char *s;

    if(!filename) return(0);
    if(strlen(filename)==0) return(0);
    s=strrchr(filename,'.');
    if(!s) return(0);
    if(!strcmp(s,ext)) return(1);
    return(0);
}

void CleanString(char *buf) {
	char *s, *t;
	
	s=buf;
	if(!s) return;
	while(*s!='\0') {
		if(*s=='\\') {
			t=s;
			while(*t!='\0') {
				*t=*(t+1);
				t++;
			}
			s--;
		}
		s++;
	}
} /* CleanString */

int FixFilename(char *file, char *buf) {
	struct passwd *pw=NULL;
	char *s;
	char *t=NULL;

	if(strstr(file,"..")) return(-1);
	CleanString(file);
#if DEBUG
	fprintf(fperr,"Checking filename [%s]\n",file);
	fflush(fperr);
#endif
	if(file[0]=='~' && file[1]!='/') {
		t=strchr(file,'/');
		if(t) {
			*t='\0';
			pw=getpwnam(&file[1]);
			*t='/';
			t=t+1;
		}
	} else if(file[0]=='/' && file[1]=='~') {
		t=strchr(&file[1],'/');
		if(t) {
			*t='\0';
			pw=getpwnam(&file[2]);
			*t='/';
			t=t+1;
		}
	} 
	if(!t) pw=getpwuid(geteuid());
	
#if defined(ROOTDIR) && defined(HTML_DIR)
	sprintf(buf,"%s/%s",ROOTDIR, HTML_DIR);
#else
#	ifdef ROOTDIR
		strcpy(buf,ROOTDIR);
#	endif
#	ifdef HTML_DIR
		sprintf(buf,"%s/%s",pw->pw_dir, HTML_DIR);
#	endif
#	if !defined(ROOTDIR) && !defined(HTML_DIR)
		sprintf(buf,"%s/public_html",pw->pw_dir);
#	endif
#endif

	s=buf+strlen(buf)-1;
	if(*s!='/') strcat(buf,"/");

#if DEBUG
	fprintf(fperr,"Base Directory is [%s]\n",buf);
	fflush(fperr);
#endif

	if(t) s=t;
	else s=file;
	while(*s=='/') s++;
	strcat(buf,s);
	return(0);
}

