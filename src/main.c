/***[main.c]******************************************************[TAB=4]****\
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
/* $Id: main.c,v 1.50 1997/06/16 14:11:43 rasmus Exp $ */
#include <stdlib.h>
#include "php.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SETLOCALE
#include <locale.h>
#endif
#if APACHE
#include "http_protocol.h"
#endif

#if APACHE
request_rec *php_rqst;
#endif

#ifndef APACHE
int main(int argc, char **argv) {
	int fd;
	char *s;
	int no_httpd=0;
	long file_size;

#if PHPFASTCGI
	while(FCGI_Accept() >= 0) {
#endif
	s = getenv("PATH_TRANSLATED");
	if(!s) no_httpd=1;

#ifdef HAVE_SETLOCALE
	setlocale(LC_ALL,"");
#endif

	/* Init all components */
	php_init_pool();
	php_init_log();
	php_init_acc();
	php_init_yacc();
	php_init_lex();
	php_init_error();
	php_init_stack();
	php_init_symbol_tree();
	php_init_switch();
	php_init_db();
	php_init_while();
	php_init_msql(NULL);
	php_init_mysql(NULL);
	php_init_pg95();
	php_init_solid();
	php_init_file();
	php_init_head();
	php_init_dir();
#ifdef HAVE_LIBGD
	php_init_gd();
#endif
	php_init_cond();

	if(argc>1) {
		if(!strcasecmp(argv[argc-1],"info")) {
			Info();
			exit(0);
#if ACCESS_CONTROL
		} else if(!strcasecmp(argv[argc-1],"config")) {
			s = getenv("REQUEST_METHOD");
			if(s && !strcasecmp(s,"post")) TreatData(0);  /* POST Data */
			Configuration(argc, argv);	
			exit(0);
#endif
		}
		if(!getenv("QUERY_STRING")) {
			{
				char *astr=NULL; 
				int ai, al=1;

				for(ai=1;ai<argc;ai++) al+=strlen(argv[ai])+1;
				astr = malloc(al+14); /* This space can't be freed anyway, so let it leak */
				strcpy(astr,"QUERY_STRING=");	
				for(ai=1;ai<argc;ai++) {
					strcat(astr,php_urlencode(argv[ai]));
					if(ai<argc-1) strcat(astr,"+");
				}
				putenv(astr);
			}
		}
	}
	s = getenv("REQUEST_METHOD");
	if(s && !strcasecmp(s,"post")) TreatData(0);  /* POST Data */
	TreatData(2); /* Cookie Data */
	TreatData(1); /* GET Data */

	if(no_httpd && argv[1]) 
		if(argv[2] && !strcmp(argv[1],"-q")) {
			NoHeader();	
			fd=OpenFile(argv[2],1,&file_size);
		} else fd=OpenFile(argv[1],1,&file_size);
	else 
		fd=OpenFile(NULL,1,&file_size);
	if(fd==-1) { fflush(stdout); return(-1); }
	ParserInit(fd,file_size,no_httpd,NULL);	
	yyparse();
	Exit(1);
	php_pool_free(1);
	php_pool_free(2);
	php_pool_free(0);
#if PHPFASTCGI
	}
#endif
	fflush(stdout);
	return(0);
}
#else

/* Apache module entry point called from mod_php.c */
int apache_php_module_main(request_rec *r, php_module_conf *conf, int fd) {
	char *last_arg,*s;

#ifdef HAVE_SETLOCALE
	setlocale(LC_ALL,"");
#endif
	php_rqst = r;

	/* 
	   The following init calls set the static variables in the
	   various sections to sane values.  Things will hopefully be rewritten 
	   at some point to not need this as it is completely unthreadable!
	*/
	php_init_pool(conf);
	php_init_log(conf);
	php_init_acc(conf);
	php_init_yacc();
	php_init_lex();
	php_init_error();
	php_init_stack();
	php_init_symbol_tree();
	php_init_switch();
	php_init_db();
	php_init_while();
#ifdef HAVE_LIBMSQL
	php_init_msql(conf->SQLLogHost);
#endif

#ifdef HAVE_SYBASE
    php_init_sybsql();
#endif

#ifdef HAVE_LIBMYSQL
	php_init_mysql(conf->SQLLogHost);
#endif
#ifdef HAVE_LIBPQ
	php_init_pg95();
#endif
#ifdef HAVE_LIBSOLID
	php_init_solid();
#endif
	php_init_file(conf);
	php_init_head();
	php_init_dir();
	php_init_mime(conf);
#ifdef HAVE_LIBGD
	php_init_gd();
#endif
	php_init_cond();
	
	TreatHeaders();  /* Check to see if there are any special HTTP headers */

	if(r->args) {
		last_arg = strrchr(r->args,'&');
		if(!last_arg) last_arg = r->args;	
		if(conf->Debug && !strcasecmp(last_arg,"info")) {
			Info();
			return 0;
#if ACCESS_CONTROL
		} else if(!strcasecmp(last_arg,"config")) {
			s = r->method;
			if(s && !strcasecmp(s,"post")) TreatData(0);  /* POST Data */
			Configuration(); 
			return 0;
#endif
		}
	}
	SetCurrentFilename(r->filename);
	SetCurrentFileSize(r->finfo.st_size);
	SetCurrentPI(r->uri);
	SetCurrentPD(r->uri);
	SetStatInfo(&(r->finfo));

	s = r->method;
	if(s && !strcasecmp(s,"post")) TreatData(0);  /* POST Data */
	TreatData(2); /* Cookie Data */
	TreatData(1); /* GET Data */
#if ACCESS_CONTROL
	ParserInit(fd,r->finfo.st_size,0,NULL);	
	if(CheckAccess(r->uri,r->finfo.st_uid)>-1) {
		yyparse();
	}
#else
	ParserInit(fd,r->finfo.st_size,0,NULL);	
	yyparse();
#endif
	Exit(1); 
	return(OK);
}
#endif
