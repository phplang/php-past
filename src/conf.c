/***[conf.c]******************************************************[TAB=4]****\
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
/* $Id: conf.c,v 1.18 1997/08/16 03:51:38 rasmus Exp $ */
#include "php.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif
#if APACHE
#include "http_protocol.h"
#endif

#if ACCESS_CONTROL
/*
 * Configuration
 *
 * If path was provided, check ownership and open appropriate config file
 *
 */
#if APACHE
void Configuration(void) {
#else
void Configuration(int argc, char **argv) {
#endif
	int ret,i,rule,es;
	char *path, *s, *key, *sn=NULL, *pi=NULL,*defRule=NULL;
	char temp[1024];
	char buf[512];
	struct stat sb;
	struct passwd *pw;
	VarTree *var;
	AccessInfo *actop, *ac;

#ifdef PHP_ROOT_DIR
	char php_root_temp[1024];

#if APACHE
	s = php_rqst->uri;
#else
	s = getenv("PATH_INFO");
#endif
	if(s) {
		sprintf(php_root_temp, "%s%s",PHP_ROOT_DIR,s);
		path = php_root_temp;
	}
	else path = NULL;
#else
#if APACHE
	path = php_rqst->filename;
#else
	path = getenv("PATH_TRANSLATED");
#endif
#endif

	if(path) {
		if(stat(path,&sb)==-1) {
			Error("Unable to find: %s",path);
			return;
		}	
		sprintf(temp,"%s/%ld-cfg",ACCESS_DIR,(long)sb.st_uid);
	} else sprintf(temp,"%s/0-cfg",ACCESS_DIR);

	es = ErrorPrintState(0);		
	ret = _dbmOpen(temp,"r",0);
	ErrorPrintState(es);		
	if(ret > 0) {
		ret = _dbmOpen(temp,"n",0);
#if DEBUG
		Debug("Creating new configuration file [%s]\n",temp);
#endif
		if(ret) return;
		_dbmInsert(temp,"cfg-file",temp);
		pw = getpwuid(sb.st_uid);
#ifdef HAVE_CRYPT
#if BROKEN_CONFIG_PW
		_dbmInsert(temp,"cfg-passwd",(char *)crypt("php","xy"));
#else
		if(pw && pw->pw_name)
			_dbmInsert(temp,"cfg-passwd",(char *)crypt(pw->pw_name,"xy"));
		else
			_dbmInsert(temp,"cfg-passwd",(char *)crypt("php","xy"));
#endif
#else
#if BROKEN_CONFIG_PW
		_dbmInsert(temp,"cfg-passwd","php");
#else
		if(pw && pw->pw_name)
			_dbmInsert(temp,"cfg-passwd",pw->pw_name);
		else
			_dbmInsert(temp,"cfg-passwd","php");
#endif
#endif
		_dbmInsert(temp,"cfg-user",pw->pw_name);
		_dbmInsert(temp,"cfg-accessALL","pub\033L\033dom.*");
		_dbmInsert(temp,"cfg-email-URL"," ");
		_dbmInsert(temp,"cfg-ban-URL"," ");
		_dbmInsert(temp,"cfg-passwd-URL"," ");
		_dbmClose(temp);
		ret = _dbmOpen(temp,"r",0);
		if(ret) return;
	}

	php_header(0,NULL);
	var = GetVar("cfg-passwd",NULL,0);
#ifdef HAVE_CRYPT
	if(!var || (var && strcmp((char *)crypt(var->strval,"xy"),_dbmFetch(temp,"cfg-passwd")))) {
#else
	if(!var || (var && strcmp(var->strval,_dbmFetch(temp,"cfg-passwd")))) {
#endif
		if(var) {
			PUTS("<html><head><title>Incorrect Configuration Password</title></head>\n");
			PUTS("<body><center><h1>Incorrect Configuration Password</h1></center>\n");
			PUTS("Please re-enter your configuration password:\n");
		} else {
			PUTS("<html><head><title>PHP/FI Configuration Password</title></head>\n");
			PUTS("<body><center><h1>PHP/FI Configuration Password</h1></center>\n");
			PUTS("Please enter your configuration password:\n");
		}
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
		PUTS("<form action=\"");
		PUTS(sn);
		PUTS("?config\" method=\"POST\">\n");
#else
		pi = getenv("PATH_INFO");
		printf("<form action=\"%s%s?config\" method=\"POST\">\n",sn?sn:argv[0],pi?pi:"");
#endif
		PUTS("<input type=\"password\" name=\"cfg-passwd\"><input type=\"submit\" value=\" Ok \"></form>");
		PUTS("</body></html>\n");
		fflush(stdout);
		_dbmClose(temp);
		return;
	}
	if(path) AddFile(temp, path);
	ChkPostVars(temp);
	var = GetVar("cfg-passwd",NULL,0);
	AddRule(temp);
	PostToAccessStr(temp);
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
	PUTS("<html><head><title>PHP/FI Configuration Screen</title></head>\n");
	PUTS("<body><center><h1>PHP/FI Configuration Screen</h1></center>\n");
#ifndef APACHE
	fflush(stdout);
#endif
#if APACHE
	PUTS("<b>Configuration file:</b> <i>");
	PUTS(_dbmFetch(temp,"cfg-file"));
	PUTS("</i><br>\n<b>User:</b> <i>");
	PUTS(_dbmFetch(temp,"cfg-user"));
	PUTS("</i><br>\n<center><form action=\"");
	PUTS(sn);
	PUTS("?config\" method=\"POST\">\n<input type=\"password\" name=\"chg-passwd\" value=\"");
	PUTS(var->strval);
	PUTS("\">\n<input type=\"hidden\" name=\"cfg-passwd\" value=\"");
	PUTS(var->strval);
	PUTS("\">\n");
#else
	printf("<b>Configuration file:</b> <i>%s</i><br>\n",_dbmFetch(temp,"cfg-file"));
	printf("<b>User:</b> <i>%s</i><br>\n",_dbmFetch(temp,"cfg-user"));
	printf("<center><form action=\"%s%s?config\" method=\"POST\">\n",sn?sn:argv[0],pi?pi:"");
	printf("<input type=\"password\" name=\"chg-passwd\" value=\"%s\">\n",var->strval);
	printf("<input type=\"hidden\" name=\"cfg-passwd\" value=\"%s\">\n",var->strval);
#endif
	PUTS("<input type=\"submit\" value=\"Change Password\"></form></center><hr>\n");
#ifndef APACHE
	fflush(stdout);
#endif
	s = _dbmFetch(temp,"cfg-accessALL");
	if(s) defRule = (char *)estrdup(0,s);
	actop = StrtoAccess(s);
	i=0;
	key=NULL;
	while(i==0 || (key && *key)) {
		if(i==0 || (key && !strncmp(key,"cfg-access-",11))) {
			ac = actop;
			PUTS("<table border=1 cellpadding=2>\n");
#if APACHE
			PUTS("<form action=\"");
			PUTS(sn);
			PUTS("?config\" method=\"POST\">\n<input type=\"hidden\" name=\"cfg-passwd\" value=\"");
			PUTS(var->strval);
			PUTS("\">\n");
#else
			printf("<form action=\"%s%s?config\" method=\"POST\">\n",sn?sn:argv[0],pi?pi:"");
			printf("<input type=\"hidden\" name=\"cfg-passwd\" value=\"%s\">\n",var->strval);
#endif
			if(i>0) {
#if APACHE
				PUTS("<tr><th colspan=3>");
				PUTS(&key[11]);
				PUTS("</th><input type=\"hidden\" name=\"file\" value=\"");
				PUTS(&key[11]);
				PUTS("\">\n");
#else
				printf("<tr><th colspan=3>%s</th>",&key[11]);
				printf("<input type=\"hidden\" name=\"file\" value=\"%s\">\n",&key[11]);
#endif
				s = _dbmFetch(temp,key);
				actop = StrtoAccess(s);
				ac= actop;
				if(ac->def) PUTS("<td colspan=2 align=\"center\"><select name=\"def\"><option SELECTED>public\n<option>private\n</select></td></tr>");
				else PUTS("<td align=\"center\"><select name=\"def\"><option SELECTED>private\n<option>public\n</select></td></tr>");
			} else {
				PUTS("<tr><th colspan=3>Default</th>");
				PUTS("<input type=\"hidden\" name=\"file\" value=\"Default\">\n");
				if(ac->def) PUTS("<td colspan=2 align=\"center\"><b>Access:</b> <select name=\"def\"><option SELECTED>public\n<option>private\n</select></td></tr>");
				else PUTS("<td colspan=2 align=\"center\"><b>Access:</b> <select name=\"def\"><option SELECTED>private\n<option>public\n</select></td></tr>");
				PUTS("<tr><th>Function</th><th>Password</th><th>Type</th><th>Pattern</th><th>del</th></tr>\n");
			}
#ifndef APACHE
			fflush(stdout);
#endif
			rule=0;
			while(ac) {	
				PUTS("<tr>");
				if(ac->mode & 1) { 
					sprintf(buf,"<td align=\"center\"><select name=\"mode[%d]\"><option SELECTED>E-mail Req.\n<option>Allow\n<option>Ban\n<option>Password\n<option>No Logging\n<option>Logging\n<option>Show Info\n<option>Hide Info\n</select></td> ",rule);
					PUTS(buf);
				}	
				if(ac->mode & 2) { 
					sprintf(buf,"<td align=\"center\"><select name=\"mode[%d]\"><option SELECTED>Allow\n<option>E-Mail Req.\n<option>Ban\n<option>Password\n<option>No Logging\n<option>Logging\n<option>Show Info\n<option>Hide Info\n</select></td> ",rule);
					PUTS(buf);
				}	
				if(ac->mode & 4) {
					sprintf(buf,"<td align=\"center\"><select name=\"mode[%d]\"><option SELECTED>Ban\n<option>E-Mail Req.\n<option>Allow\n<option>Password\n<option>No Logging\n<option>Logging\n<option>Show Info\n<option>Hide Info\n</select></td> ",rule);
					PUTS(buf);
				}	
				if(ac->mode & 8) {
					sprintf(buf,"<td align=\"center\"><select name=\"mode[%d]\"><option SELECTED>Password\n<option>E-Mail Req.\n<option>Allow\n<option>Ban\n<option>No Logging\n<option>Logging\n<option>Show Info\n<option>Hide Info\n</select></td> ",rule);
					PUTS(buf);
				}	
				if(ac->mode & 16) {
					sprintf(buf,"<td align=\"center\"><select name=\"mode[%d]\"><option SELECTED>No Logging\n<option>E-Mail Req.\n<option>Allow\n<option>Ban\n<option>Password\n<option>Logging\n<option>Show Info\n<option>Hide Info\n</select></td> ",rule);
					PUTS(buf);
				}	
				if(ac->mode & 32) {
					sprintf(buf,"<td align=\"center\"><select name=\"mode[%d]\"><option SELECTED>Logging\n<option>E-Mail Req.\n<option>Allow\n<option>Ban\n<option>Password\n<option>No Logging\n<option>Show Info\n<option>Hide Info\n</select></td> ",rule);
					PUTS(buf);
				}	
				if(ac->mode & 64) {
					sprintf(buf,"<td align=\"center\"><select name=\"mode[%d]\"><option SELECTED>Show Info\n<option>E-mail Req.\n<option>Allow\n<option>Ban\n<option>Password\n<option>Logging\n<option>No Logging\n<option>Hide Info\n</select></td> ",rule);
					PUTS(buf);
				}	
				if(ac->mode & 128) {
					sprintf(buf,"<td align=\"center\"><select name=\"mode[%d]\"><option SELECTED>Hide Info\n<option>E-mail Req.\n<option>Allow\n<option>Ban\n<option>Password\n<option>Logging\n<option>No Logging\n<option>Show Info\n</select></td> ",rule);
					PUTS(buf);
				}	
				if(ac->password) {
					if(!strcmp(ac->password," ")) 
						sprintf(buf,"<td align=\"center\"><input type=\"text\" name=\"password[%d]\" value=\"\"></td>\n",rule);
					else
						sprintf(buf,"<td align=\"center\"><input type=\"text\" name=\"password[%d]\" value=\"%s\"></td>\n",rule,ac->password);
					PUTS(buf);
				} else {
					sprintf(buf,"<td align=\"center\"><input type=\"text\" name=\"password[%d]\" value=\"\"></td>\n",rule);
					PUTS(buf);
				}

				if(ac->type==0) 
					sprintf(buf,"<td align=\"center\"><select name=\"type[%d]\"><option SELECTED>Domain\n<option>E-Mail\n<option>RefDoc\n<option>Browser\n</select></td>",rule);
				else if(ac->type==1) 
					sprintf(buf,"<td align=\"center\"><select name=\"type[%d]\"><option SELECTED>E-Mail\n<option>Domain\n<option>RefDoc\n<option>Browser\n</select></td>",rule);
				else if(ac->type==2) 
					sprintf(buf,"<td align=\"center\"><select name=\"type[%d]\"><option SELECTED>RefDoc\n<option>Domain\n<option>E-Mail\n<option>Browser\n</select></td>",rule);
				else if(ac->type==3) 
					sprintf(buf,"<td align=\"center\"><select name=\"type[%d]\"><option SELECTED>Browser\n<option>Domain\n<option>E-Mail\n<option>RefDoc\n</select></td>",rule);
				PUTS(buf);
				sprintf(buf,"<td align=\"center\"><input type=\"text\" name=\"patt[%d]\" value=\"%s\"></td>\n",rule,ac->patt);
				PUTS(buf);
				if(!(i==0 && rule==0)) {
					sprintf(buf,"<td align=\"center\"><input type=\"checkbox\" name=\"del%d\"></td></tr>\n\n",rule);
					PUTS(buf);
				}
#ifndef APACHE
				fflush(stdout);
#endif
				ac = ac->next;
				rule++;
			}	
			PUTS("<tr><td colspan=3 align=\"center\"><input type=\"submit\" value=\"Submit Changes\"></td></form>\n");
#if APACHE
			sprintf(buf,"<form action=\"%s?config\" method=\"POST\">\n",sn);
#else
			sprintf(buf,"<form action=\"%s%s?config\" method=\"POST\">\n",sn?sn:argv[0],pi?pi:"");
#endif
			PUTS(buf);
			sprintf(buf,"<input type=\"hidden\" name=\"cfg-passwd\" value=\"%s\">\n",var->strval);
			PUTS(buf);
			sprintf(buf,"<input type=\"hidden\" name=\"addrule\" value=\"%s\">\n",(i>0)?&key[11]:"Default");
			PUTS(buf);
			PUTS("<td colspan=2 align=\"center\"><input type=\"submit\" value=\"Add Rule\"></td></tr></form></table>\n");
#ifndef APACHE
			fflush(stdout);
#endif
		}
		if(i==0) key = _dbmFirstKey(temp);
		else {
			key = _dbmNextKey(temp,key);
		}
		i++;
	}	

	PUTS("<hr><i>Below you may configure URL's to be displayed whenever the stated condition occurs.  If left blank, a built-in PHP/FI screen will be used.</i><p>\n");
#if APACHE
	sprintf(buf,"<form action=\"%s?config\" method=\"POST\">\n",sn);
#else
	sprintf(buf,"<form action=\"%s%s?config\" method=\"POST\">\n",sn?sn:argv[0],pi?pi:"");
#endif
	PUTS(buf);
	sprintf(buf,"<input type=\"hidden\" name=\"cfg-passwd\" value=\"%s\">\n",var->strval);
	PUTS(buf);

	s = _dbmFetch(temp,"cfg-email-URL");
	if(!s || (s && strlen(s)<2))
		PUTS("E-Mail Address request form: <input type=\"text\" name=\"cfg-email-URL\" value=\"\" size=60 maxlength=512><br>\n");
	else if(s) {
		sprintf(buf,"E-Mail Address request form: <input type=\"text\" name=\"cfg-email-URL\" value=\"%s\" size=60 maxlength=512><br>\n",s);
		PUTS(buf);
	}

	s = _dbmFetch(temp,"cfg-passwd-URL");
	if(!s || (s && strlen(s)<2))
		PUTS("Password request form: <input type=\"text\" name=\"cfg-passwd-URL\" value=\"\" size=60 maxlength=512><br>\n"); 
	else if(s) {
		sprintf(buf,"Password request form: <input type=\"text\" name=\"cfg-passwd-URL\" value=\"%s\" size=60 maxlength=512><br>\n",s);
		PUTS(buf);
	}

	s = _dbmFetch(temp,"cfg-ban-URL");
	if(!s || (s && strlen(s)<2))
		PUTS("Page to show a banned user: <input type=\"text\" name=\"cfg-ban-URL\" value=\"\" size=60 maxlength=512><br>\n");
	else if(s) {
		sprintf(buf,"Page to show a banned user: <input type=\"text\" name=\"cfg-ban-URL\" value=\"%s\" size=60 maxlength=512><br>\n",s);
		PUTS(buf);
	}

	PUTS("<input type=\"submit\" value=\"Submit Changes\">\n");
	PUTS("<hr><font size=-2><i>PHP/FI</i></font></body></html>\n");
#ifndef APACHE
	fflush(stdout);
#endif
	_dbmClose(temp);
}

#endif /* ACCESS_CONTROL */

