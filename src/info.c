/***[info.c]******************************************************[TAB=4]****\
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
/* $Id: info.c,v 1.43 1997/11/25 21:40:32 rasmus Exp $ */
#include "php.h"
#include "parse.h"
#include <stdlib.h>
#include <sys/param.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_GRP_H
#include <grp.h>
#endif
#include <time.h>
#if APACHE
#include "http_protocol.h"
#endif

extern char **environ;

void Info(void) {
	struct stat sb;
	char *path, *s;
#if HAVE_PWD_H
	struct passwd *pw;
#endif
#if HAVE_GRP_H
	struct group *gr;
#endif
	char buf[MAXPATHLEN+30];
	char **envp;
#if APACHE
	array_header *env_arr;
	table_entry *env;
	int i;
	VarTree *var;
#endif	
#ifndef WINNT
#ifndef WIN32
	FILE *fp;
#endif
#endif

	php_header(0,NULL);
	sprintf(buf,"<html><head><title>PHP/FI</title></head><body><h1>PHP/FI Version %s</h1>by Rasmus Lerdorf (<a href=\"mailto:rasmus@lerdorf.on.ca\">rasmus@lerdorf.on.ca</a>)<p>The PHP/FI Web Site is at <a href=\"http://php.iquest.net/\">http://php.iquest.net/</a><p>\n",PHP_VERSION);
	PUTS(buf);
	PUTS("This program is free software; you can redistribute it and/or modify\n");
	PUTS("it under the terms of the GNU General Public License as published by\n");
	PUTS("the Free Software Foundation; either version 2 of the License, or\n");
	PUTS("(at your option) any later version.<p>\n");
	PUTS("This program is distributed in the hope that it will be useful,\n");
	PUTS("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	PUTS("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	PUTS("GNU General Public License for more details.<p>\n");
	PUTS("You should have received a copy of the GNU General Public License\n");
	PUTS("along with this program; if not, write to the Free Software\n");
	PUTS("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.<p>\n");
#ifdef WINDOWS
#ifdef WIN32
	PUTS("<hr><b><i>Windows95/NT Version compiled with MS VC++ V5</i></b>\n");
#else
	PUTS("<hr><b><i>Windows95/NT Version compiled with cygnus-2.7.2-970404</i></b>\n");
#endif
#else
	PUTS("<hr><b><i>Unix version:</i></b> ");
	fp = popen("uname -a","r");
	if(fp) {
		while(fgets(buf,255,fp)) {
			PUTS(buf);	
		}
		pclose(fp);
	}
#endif
	
	PUTS("<hr><b><i>Environment:</i></b><pre>");
	envp = environ;
	while (*envp) {
	    PUTS(*envp++);
	    PUTS("\n");
	}
#ifndef APACHE
	{
		char *sn, *pi;
		sn = getenv("SCRIPT_NAME"); pi = getenv("PATH_INFO");
		if(sn && pi && !strcmp(sn,pi)) {
			pi = NULL;
		}
		sprintf(buf,"PHP_SELF=%s%s\n",sn?sn:"",pi?pi:"");
		PUTS(buf);	
	}
#endif
	PUTS("</pre>\n");
#if APACHE
	PUTS("<hr><b><i>Apache defined variables available to PHP/FI:</i></b><p>\n");
	env_arr = table_elts(php_rqst->subprocess_env);
	env = (table_entry *)env_arr->elts;
	for(i = 0; i < env_arr->nelts; ++i) {
		if(!env[i].key) continue;
		if(strcasecmp(env[i].key,"SCRIPT_NAME")) {
			sprintf(buf,"%s = %s<br>\n",env[i].key,env[i].val);
			PUTS(buf);
			if(!strcasecmp(env[i].key,"SCRIPT_FILENAME")) {
				sprintf(buf,"PATH_TRANSLATED = %s<br>\n",env[i].val);
				PUTS(buf);
			}
		}
    }
	sprintf(buf,"PATH_INFO = %s<br>\n",php_rqst->uri);  /* Faked by GetVar */
	PUTS(buf);
	sprintf(buf,"PHP_SELF = %s<br>\n",php_rqst->uri);  /* Faked by GetVar */
	PUTS(buf);
#if APACHE
	var = GetVar("PHP_AUTH_USER",0,0);
	if(var) {
		sprintf(buf,"PHP_AUTH_USER = %s<br>\n",var->strval);
		PUTS(buf);
	}
	var = GetVar("PHP_AUTH_PW",0,0);
	if(var) {
		sprintf(buf,"PHP_AUTH_PW = %s<br>\n",var->strval);
		PUTS(buf);
	}
	var = GetVar("PHP_AUTH_TYPE",0,0);
	if(var) {
		sprintf(buf,"PHP_AUTH_TYPE = %s<br>\n",var->strval);
		PUTS(buf);
	}
#endif
	PUTS("<hr><b><i>Request HTTP Headers:</i></b><p>\n");
	env_arr = table_elts(php_rqst->headers_in);
	env = (table_entry *)env_arr->elts;
	for(i = 0; i < env_arr->nelts; ++i) {
		if(!env[i].key) continue;
		sprintf(buf,"%s:%s<br>\n",env[i].key,env[i].val);
		PUTS(buf);
    }
	PUTS("<hr><b><i>Send HTTP Headers:</i></b><p>\n");
	env_arr = table_elts(php_rqst->headers_out);
	env = (table_entry *)env_arr->elts;
	for(i = 0; i < env_arr->nelts; ++i) {
		if(!env[i].key) continue;
		sprintf(buf,"%s:%s<br>\n",env[i].key,env[i].val);
		PUTS(buf);
    }
#endif
#if APACHE
	path = php_rqst->filename;
#else	
	path = getenv("PATH_TRANSLATED");
#endif
	if(path) {
#ifndef APACHE
		if(stat(path,&sb)!=-1) {
#endif
			PUTS("<hr><font size=+1>Information on <i>");
			PUTS(path);
			PUTS("</i></font><br>\n");
#if APACHE
			switch(php_rqst->finfo.st_mode&S_IFMT) {
#else
			switch(sb.st_mode&S_IFMT) {
#endif
			case S_IFIFO:
				PUTS("<b>Type:</b> <i>fifo special</i><br>\n");
				break;
        	case S_IFCHR:
				PUTS("<b>Type:</b> <i>character special</i><br>\n");
				break;
        	case S_IFDIR:
				PUTS("<b>Type:</b> <i>directory</i><br>\n");
				break;
        	case S_IFBLK:
				PUTS("<b>Type:</b> <i>block special</i><br>\n");
				break;
        	case S_IFREG:
				PUTS("<b>Type:</b> <i>ordinary file</i><br>\n");
				break;
			default:
				PUTS("<b>Type:</b> <i>unknown</i><br>\n");
				break;
			}
#if APACHE
			sprintf(buf,"<b>Permission Bits:</b> <i>%04lo</i><br>\n",(long)php_rqst->finfo.st_mode&07777);
#else
			sprintf(buf,"<b>Permission Bits:</b> <i>%04lo</i><br>\n",(long)sb.st_mode&07777);
#endif
			PUTS(buf);
#if APACHE
			sprintf(buf,"<b>Number of Links:</b> <i>%ld</i><br>\n",(long)php_rqst->finfo.st_nlink);
#else
			sprintf(buf,"<b>Number of Links:</b> <i>%ld</i><br>\n",(long)sb.st_nlink);
#endif
			PUTS(buf);
#ifdef HAVE_PWD_H
#if APACHE
			pw = getpwuid(php_rqst->finfo.st_uid);
#else
			pw = getpwuid(sb.st_uid);
#endif
			if(pw) {
				gr = getgrgid(pw->pw_gid);
				sprintf(buf,"<b>Owner:</b> <i>%s</i> <b>From Group:</b> <i>%s</i><br>\n",pw->pw_name,gr->gr_name);
				PUTS(buf);
			}
#endif

#ifdef HAVE_GRP_H
#if APACHE
			gr = getgrgid(php_rqst->finfo.st_gid);
#else
			gr = getgrgid(sb.st_gid);
#endif
			if(gr) {
				sprintf(buf,"<b>Group:</b> <i>%s</i><br>\n",gr->gr_name);
				PUTS(buf);
			}
#endif
			sprintf(buf,"<b>Size:</b> <i>%ld</i><br>\n",
#if APACHE
				(long)php_rqst->finfo.st_size);
#else
				(long)sb.st_size);
#endif
			PUTS(buf);
			sprintf(buf,"<b>Last Access:</b> <i>%s</i><br>",
#if APACHE
				ctime(&php_rqst->finfo.st_atime));
#else
				ctime(&sb.st_atime));
#endif
			PUTS(buf);
			sprintf(buf,"<b>Last Modification:</b> <i>%s</i><br>",
#if APACHE
				ctime(&php_rqst->finfo.st_mtime));
#else
				ctime(&sb.st_mtime));
#endif
			PUTS(buf);
			sprintf(buf,"<b>Last Status Change:</b> <i>%s</i><br>",
#if APACHE
				ctime(&php_rqst->finfo.st_ctime));
#else
				ctime(&sb.st_ctime));
#endif
			PUTS(buf);
#ifndef APACHE
		}
#endif
	}
	PUTS("<hr>\n");
	s=emalloc(1,MAXPATHLEN+1);
	if(getcwd(s,MAXPATHLEN)) {
		sprintf(buf,"<b>Working Directory:</b> <i>%s</i><br>\n",s);
		PUTS(buf);
	}
	path = GetIncludePath();
	if (path) {
		sprintf(buf,"<b>Include Path:</b> <i>%s</i><br>\n",path);
		PUTS(buf);
	}
#if ACCESS_CONTROL
	sprintf(buf,"<b>Access Control enabled using:</b> <i>%s</i><br>\n",getaccdir());
	PUTS(buf);
#endif
#if LOGGING
	sprintf(buf,"<b>Access Logging enabled using:</b> <i>%s</i><br>\n",getlogdir());
	PUTS(buf);
#endif
#if MSQLLOGGING
	sprintf(buf,"<b>Access Logging enabled using mSQL in db:</b> <i>%s</i> <b>host:</b><i>%s</i><br>\n",getlogdir(),getloghost());
	PUTS(buf);
#endif
#if MYSQLLOGGING
	sprintf(buf,"<b>Access Logging enabled using mySQL in db:</b> <i>%s</i> <b>host:</b><i>%s</i><br>\n",getlogdir(),getloghost());
	PUTS(buf);
#endif
#ifdef HAVE_LIBMSQL
	PUTS("<b>mSQL support enabled</b><br>\n");
#endif
#ifdef HAVE_LIBMYSQL
	PUTS("<b>mysql support enabled</b><br>\n");
#endif
#ifdef HAVE_LIBPQ
	PUTS("<B>Postgres95 support enabled</b><br>\n");
#endif
#ifdef HAVE_LIBSOLID
	PUTS("<B>Solid support enabled</b><br>\n");
#endif
#ifdef ILLUSTRA
	PUTS("<B>Illustra/Universal Server support enabled</b><br>\n");
#endif
#ifdef HAVE_SYBASE
	PUTS("<B>Sybase support enabled</b><br>\n");
#endif
#ifdef HAVE_ODBC
	PUTS("<B>ODBC support enabled</B><br>\n");
#endif
#ifdef HAVE_LIBADABAS
	PUTS("<B>Adabas D support enabled</B><br>\n");
#endif /*HAVE_LIBADABAS*/
#ifdef GDBM
	PUTS("<b>GDBM support enabled</b><br>\n");
#else
#ifdef NDBM
	PUTS("<b>NDBM support enabled</b><br>\n");
#else
	PUTS("<b>FlatFile support enabled</b><br>\n");
#endif
#endif
#if APACHE
	PUTS("<b>Apache module support enabled</b><br>\n");
#endif
#ifdef HAVE_LIBGD
	PUTS("<b>GD support enabled</b><br>\n");
#endif
#if PHPFASTCGI
	PUTS("<b>FastCGI support enabled</b><br>\n");
#endif
#if PHP_SAFE_MODE
	PUTS("<b>SAFE MODE Enabled!</b><br>\n");
#endif
	PUTS("</body></html>");
}

void ShowPageInfo(void) {
	char *email,*host=NULL;
	struct tm *tm1;
	time_t t;
	int tot=0, tod=0;
	char buf[256];
	char *days[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
	char *months[] = {	"January","February","March","April","May","June","July",
						"August","September","October","November","December" };

	tot = gettotal();
	tod = gettoday();
	t = getstartlogging();
	if(t<1) t = time(NULL);
	tm1 = localtime(&t);
	sprintf(buf,"<hr size=3 width=400 align=\"center\"><center><i>%d total hit%s since %s %s %d.  %d hit%s today.\n<br>",tot,(tot==1)?"":"s",days[tm1->tm_wday],months[tm1->tm_mon],tm1->tm_mday,tod,(tod==1)?"":"s");
	PUTS(buf);
	email = getlastemailaddr();
	if(!email || (email && strlen(email)<3)) host = getlasthost();
	t = getlastaccess();	
	tm1 = localtime(&t);
	if(email && strlen(email)>2) {
		if(!t || t==-1)
			PUTS("<i>No Previous Access to this page<br>\n");
		else {
				sprintf(buf,"<i>Last access on %s %s %d at %d:%02d:%02d by %s<br>\n",
				  days[tm1->tm_wday],months[tm1->tm_mon],tm1->tm_mday,tm1->tm_hour,
				  tm1->tm_min,tm1->tm_sec,email);
				PUTS(buf);
			}				
	} else {
		if(!t || t==-1)
			PUTS("No Previous Access to this page<br>\n");
		else {
			sprintf(buf,"Last access on %s %s %d at %d:%02d:%02d from %s<br>\n",
				days[tm1->tm_wday],months[tm1->tm_mon],tm1->tm_mday,tm1->tm_hour,
				tm1->tm_min,tm1->tm_sec,host?host:"(null)");
			PUTS(buf);
		}
	}
	t = getlastmod();
	tm1 = localtime(&t);
	sprintf(buf,"Page was last updated on %s %s %d, %d at %d:%02d:%02d</i></center>\n",days[tm1->tm_wday],months[tm1->tm_mon],tm1->tm_mday,1900+tm1->tm_year,tm1->tm_hour,tm1->tm_min,tm1->tm_sec);
	PUTS(buf);
}

void PHPVersion(void) {
	Push(PHP_VERSION,STRING);
}
