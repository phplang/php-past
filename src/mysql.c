/***[mysql.c]*****************************************************[TAB=4]****\
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
/* $Id: mysql.c,v 1.25 1997/08/22 22:22:37 rasmus Exp $ */
/* mSQL is Copyright (c) 1993-1995 David J. Hughes */

/* Note that there is no mySQL code in this file */

#include "php.h"
#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_LIBMYSQL
#include <mysql.h>
#endif
#include "parse.h"
#include <ctype.h>

#ifndef HAVE_LIBMYSQL
#define MYSQL void
#endif
MYSQL *mysqlGetDbSock();

#ifdef HAVE_LIBMYSQL
typedef struct ResultList {
	MYSQL_RES *result;
	int ind;
	struct ResultList *next;
} ResultList;

static MYSQL *dbsock=NULL, mysql;
static char CurrentDB[128];
static ResultList *result_top=NULL;
static char *CurrentHost=NULL;
static char *CurrentUser=NULL;
static char *CurrentPassword=NULL;
static char *CurrentTcpPort=NULL;
static char *CurrentUnixPort=NULL;
static int mysql_ind=1;
#endif

void php_init_mysql(char *defaulthost) {
#ifdef HAVE_LIBMYSQL
#ifdef HAVE_PWD_H
	struct passwd *pwd;
#endif
	char * name;
	static char junk[1];
	CurrentTcpPort=&junk[0];
	CurrentUnixPort=&junk[0];
	dbsock=NULL;
	result_top=NULL;
	junk[0]='\0';
	/* set CurrentUser to current uid's login name */
	CurrentUser=NULL;
	name=NULL;
#ifdef HAVE_PWD_H
	pwd = getpwuid(getuid());
	CurrentUser= estrdup(0, pwd->pw_name);
#endif
#if PHP_MYSQL_GETLOGIN
#ifdef HAVE_GETLOGIN
	name = getlogin();
	if(name) {
		CurrentUser= estrdup(0, name);
	}
#endif
#ifdef HAVE_CUSERID
	if(!name) {
		name = cuserid(NULL);
		if(name) {
			CurrentUser= estrdup(0, name);
		}
	}
#endif
#endif
	CurrentHost=defaulthost;
	CurrentPassword=NULL;
	CurrentDB[0]='\0';
	mysql_ind=1;
#endif
}

#ifdef HAVE_LIBMYSQL
int mysql_add_result(MYSQL_RES *result) {
	ResultList *new;

	new = result_top;
	if(!new) {
		new = emalloc(0,sizeof(ResultList));
		result_top = new;
	} else {
		while(new->next) new=new->next;
		new->next = emalloc(0,sizeof(ResultList));
		new = new->next;
	}
	new->result = result;
	new->ind    = mysql_ind++;
	new->next   = NULL;
	return(mysql_ind-1);
}

MYSQL_RES *mysql_get_result(int count) {
	ResultList *new;

	new = result_top;
	while(new) {
		if(new->ind == count) return(new->result);
		new=new->next;
	}	
	return(NULL);
}
#endif

void mysql_del_result(int count) {
#ifdef HAVE_LIBMYSQL
	ResultList *new, *prev, *next;

	prev=NULL;
	new = result_top;
	while(new) {
		next = new->next;
		if(new->ind == count) {
			mysql_free_result(new->result);
			if(prev) prev->next = next;
			else result_top = next;
			break;
		}
		prev=new;
		new=next;
	}	
#endif
}

void MYsqlClose(void) {
#ifdef HAVE_LIBMYSQL
	ResultList *new,*next;

	new = result_top;	
	while(new) {
		next = new->next;	
		mysql_free_result(new->result);
		new = next;
	}
	result_top = NULL;
	if(dbsock != NULL) mysql_close(dbsock);
	dbsock=NULL;
	CurrentDB[0]='\0';
#endif
}
	
void MYsql(void) {
#ifdef HAVE_LIBMYSQL
	Stack *s;
	char *query=NULL;
#ifndef APACHE
	static int First=1;
	char junk[1];
#endif
	int j;
	char temp[16];
	MYSQL_RES *result=NULL;
	char *tcpPort, *unixPort;

#ifndef APACHE
	if(First) {
		CurrentDB[0] = '\0';
		junk[0]='\0';
		CurrentTcpPort=&junk[0];
		CurrentUnixPort=&junk[0];
		First=0;
	}
#endif

	s = Pop();
	if(!s) {
		Error("Stack error in mysql expression");
		Push("-1",LNUMBER);
		return;
	}
	if(s->strval) query = (char *)estrdup(1,s->strval);
	else {
		Error("No query string in mysql expression");
		Push("-1",LNUMBER);
		return;
	}
	s = Pop();
	if(!s) {
		Error("Stack error in mysql expression");
		Push("-1",LNUMBER);
		return;
	}
	if(!s->strval) {
		Error("No database argument in mysql expression");
		Push("-1",LNUMBER);
		return;
	}
	if(dbsock==NULL) {
		dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
		if(!dbsock) {
			Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
			Push("-1",LNUMBER);
			return;
		}
		CurrentTcpPort = getenv("MYSQL_TCP_PORT");
		CurrentUnixPort = getenv("MYSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MYSQL_TCP_PORT");
		unixPort = getenv("MYSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MYsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0);
#if DEBUG
			Debug("After connect\n");
#endif
			if(!dbsock) {
				Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
				Push("-1",LNUMBER);
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}
	if(strcmp(CurrentDB,s->strval)) {
		if(mysql_select_db(dbsock,s->strval)<0) {
			Error("Unable to select mySQL database (%s)",mysql_error(&mysql));
			Push("-1",LNUMBER);
			return;
		}
		strcpy(CurrentDB,s->strval);
	}

#if DEBUG
	Debug("Sending query: %s\n",query);
#endif
#if APACHE
	block_alarms();
#endif
	if(mysql_query(dbsock,query)<0) {
#if APACHE
		unblock_alarms();
#endif
		Error("Unable to perform query (%s)",mysql_error(&mysql));
		Push("-1",LNUMBER);
		return;
	}
#if APACHE
	unblock_alarms();
#endif
	result = mysql_store_result(dbsock);
	if(result) j = mysql_add_result(result);
	else {
		if(!strncasecmp(query,"insert",6) || !strncasecmp(query,"update",6) || !strncasecmp(query,"create",6) || !strncasecmp(query,"drop",4) || !strncasecmp(query,"delete",6))
			j=0;
		else
			j=-1;
	}
	sprintf(temp,"%d",j);
	Push(temp,LNUMBER);
#else
	Pop();
	Pop();
	Push("0",LNUMBER);
	Error("No mySQL support");
#endif
} 

void MYsqlConnect(int t) {
#ifdef HAVE_LIBMYSQL
	Stack *s;

	if(t>1) {
		s = Pop();
		if(!s) {
			Error("Stack error in mysql_connect");
			Push("-1",LNUMBER);
			return;
		}
		if(strlen(s->strval)==0) CurrentPassword=NULL;
		else {
#if DEBUG
			Debug("Setting password to: %s\n",s->strval);
#endif
			CurrentPassword = (char *)estrdup(0,s->strval);
		}
	}
	if(t>0) {
		s = Pop();
		if(!s) {
			Error("Stack error in mysql_connect");
			Push("-1",LNUMBER);
			return;
		}
		if(strlen(s->strval)==0) CurrentUser=NULL;
		else {
#if DEBUG
			Debug("CurrentUser set to %s\n",s->strval);
#endif
			CurrentUser = (char *)estrdup(0,s->strval);
		}
	}
	s = Pop();
	if(!s) {
		Error("Stack error in mysql_connect");
		Push("-1",LNUMBER);
		return;
	}
	MYsqlClose();
	if(strlen(s->strval)==0 || !strcasecmp(s->strval,"localhost")) CurrentHost=NULL;
	else {
		CurrentHost = (char *)estrdup(0,s->strval);
	}
#else
	Pop();
	Push("-1",LNUMBER);
	Error("No mySQL support");
#endif	
}

void MYsqlResult(void) {
#ifdef HAVE_LIBMYSQL
	Stack *s;
	int i,j;
	char fieldname[128];
	char tablename[128];
	MYSQL_ROW record;
	MYSQL_RES *result;
	MYSQL_FIELD *mysql_field;
	int ind, result_ind, done=0;
	char *tmp=NULL, *ret;

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_result");
		return;
	}
	tablename[0]='\0';
	if(s->strval) {
		if((tmp=strchr(s->strval,'.'))) {
			*tmp='\0';
			strcpy(tablename,s->strval);
			strcpy(fieldname,tmp+1);
			*(s->strval) = '.';
		} else {
			strcpy(fieldname,s->strval);
		}
	} else {
		Error("no field argument in mysql_result");
		return;
	}
	
	s = Pop();
	if(!s) {
		Error("Stack error in mysql_result");
		return;
	}
	if(s->strval) {
		ind = s->intval;
	} else {
		ind = 0;
	}

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_result");
		return;
	}
	if(s->strval) {
		result_ind = s->intval;
	} else {
		Error("Invalid result index in mysql_result");
		return;
	}	
	result = mysql_get_result(result_ind);
	if(!result) {
		Error("Unable to find result index %d",result_ind);
		Push("", STRING);
		return;
	}
	i = mysql_num_rows(result);
	j = mysql_num_fields(result);
	if(i==0 || j==0 || ind>=i) {
		Push("",STRING);
		return;
	}
	mysql_data_seek(result,ind);
	record = mysql_fetch_row(result);
	i=0;
	while(i<j && !done) {			/* modified to include mysql field_type */
		mysql_field_seek(result,i);
		mysql_field = mysql_fetch_field(result);
		if(!strcmp(mysql_field->name,fieldname)) {
			if(strlen(tablename)==0 || (strlen(tablename) > 0 && strcmp(mysql_field->table,tablename)==0)) {
				switch(mysql_field->type) {
				case FIELD_TYPE_SHORT:		/* Return with the correct PHP data type */
				case FIELD_TYPE_LONG:
				case FIELD_TYPE_LONGLONG:
				case FIELD_TYPE_INT24:
					if(record[i]) {
						Push(record[i],LNUMBER);
					} else Push("",STRING);
					break;
				case FIELD_TYPE_CHAR:
				case FIELD_TYPE_TIME:
				case FIELD_TYPE_STRING:
				case FIELD_TYPE_VAR_STRING:
				case FIELD_TYPE_BLOB:
				case FIELD_TYPE_TINY_BLOB:
				case FIELD_TYPE_MEDIUM_BLOB:
				case FIELD_TYPE_LONG_BLOB:
					if(record[i]) {
						tmp = estrdup(1,record[i]);
						Push((ret=AddSlashes(tmp,1)),STRING);
					} else Push("",STRING);
					break;
				case FIELD_TYPE_DECIMAL:
				case FIELD_TYPE_FLOAT:
				case FIELD_TYPE_DOUBLE:
					if(record[i]) Push(record[i],DNUMBER);
					else Push("",STRING);
					break;
				default:
					if(record[i]) {
						tmp = estrdup(1,record[i]);
						Push((ret=AddSlashes(tmp,1)),STRING);
					} else Push("",STRING);
					break;
				}
				done=1;
			}
		}
		i++;
	}
#else
	Pop();
	Pop();
	Push("",STRING);
	Error("No mySQL support");
#endif
}

void MYsqlFreeResult(void) {
#ifdef HAVE_LIBMYSQL 
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_result");
		return;
	}
	if(s->strval) {
		mysql_del_result(s->intval);
	} else {
		Error("Invalid result index in mysql_freeresult");
		return;
	}	
#else
	Pop();
	Error("No mySQL support");
#endif
}

void MYsqlNumRows(void) {
#ifdef HAVE_LIBMYSQL
	Stack *s;
	char temp[16];
	MYSQL_RES *result;

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_numrows");
		return;
	}
	if(s->strval) {
		result = mysql_get_result(s->intval);
		if(!result) {
			Error("Unable to find result index %d",s->intval);
			Push("-1", LNUMBER);
			return;
		}
		sprintf(temp,"%ld",mysql_num_rows(result));
	} else {
		Error("Invalid result index in mysql_numrows");
		Push("-1", LNUMBER);
		return;
	}	
	Push(temp,LNUMBER);
#else
	Pop();
	Push("-1", LNUMBER);
	Error("No mySQL support");
#endif
}

void MYsqlNumFields(void) {
#ifdef HAVE_LIBMYSQL
	Stack *s;
	char temp[16];
	MYSQL_RES *result;

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_numfields");
		return;
	}
	if(s->strval) {
		result = mysql_get_result(s->intval);
		if(!result) {
			Error("Unable to find result index %d",s->intval);
			Push("-1", LNUMBER);
			return;
		}
		sprintf(temp,"%d",mysql_num_fields(result));
	} else {
		Error("Invalid result index in mysql_numfields");
		Push("-1", LNUMBER);
		return;
	}	
	Push(temp,LNUMBER);
#else
	Pop();
	Error("No mySQL support");
	Push("-1", LNUMBER);
#endif
}

/* checks a field
 *
 * type = 1   pushes fieldname
 * type = 2   pushes fieldtype
 * type = 3   pushes fieldlen
 * type = 4   pushes fieldflags
 */
void MYsqlField(int type) {
#ifdef HAVE_LIBMYSQL
	Stack *s;
	MYSQL_RES *result=NULL;
	int field_ind=0;
	MYSQL_FIELD *mysql_field;
	char temp[64];
	int flg=0;

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_fields expression");
		return;
	}
	if(s->strval) {
		field_ind = s->intval;
	} else {
		Error("field index error in mysql_fields expression");
		return;
	}
	s = Pop();
	if(!s) {
		Error("Stack error in mysql_fields expression");
		return;
	}
	if(s->strval) {
		result = mysql_get_result(s->intval);
		if(!result) {
			Error("Unable to find result index %d",s->intval);
			return;
		}
	} else {
		Error("Invalid result index in mysql_numrows");
		return;
	}	
	mysql_field_seek(result,field_ind);
	mysql_field = mysql_fetch_field(result);
	switch(type) {
	case 1:
		Push(mysql_field->name,STRING);	
		break;
	case 2:					/* modified to include mysql field_type */
		switch(mysql_field->type) {
		case FIELD_TYPE_DECIMAL:
			Push("decimal",STRING);
			break;
		case FIELD_TYPE_CHAR:
			Push("char",STRING);
			break;
		case FIELD_TYPE_SHORT:
			Push("small int",STRING);
			break;
		case FIELD_TYPE_LONG:
			Push("long",STRING);
			break;
		case FIELD_TYPE_FLOAT:
			Push("float",STRING);
			break;		
		case FIELD_TYPE_DOUBLE:
			Push("double",STRING);
			break;
		case FIELD_TYPE_NULL:
			Push("null",STRING);
			break;		
		case FIELD_TYPE_TIME:
			Push("timestamp",STRING);
			break;
		case FIELD_TYPE_LONGLONG:
			Push("big int",STRING);
			break;		
		case FIELD_TYPE_INT24:
			Push("medium int",STRING);
			break;
		case FIELD_TYPE_TINY_BLOB:
			Push("tiny blob",STRING);
			break;		
		case FIELD_TYPE_MEDIUM_BLOB:
			Push("medium blob",STRING);
			break;
		case FIELD_TYPE_LONG_BLOB:
			Push("long blob",STRING);
			break;		
		case FIELD_TYPE_BLOB:
			Push("blob",STRING);
			break;
		case FIELD_TYPE_VAR_STRING:
			Push("var string",STRING);
			break;		
		case FIELD_TYPE_STRING:
			Push("string",STRING);
			break;	
		default:
			Push("unknown",STRING);
			break;
		}
		break;
	case 3:
		sprintf(temp,"%d",mysql_field->length);
		Push(temp,LNUMBER);
		break;
	case 4:
		flg = mysql_field->flags;	/* modifed to cater for mysql field_flags */
		temp[0]='\0';
		if (flg & PRI_KEY_FLAG) strcat(temp,"primary key ");
		if (flg & MULTIPLE_KEY_FLAG) strcat(temp,"key ");
		if (flg & UNIQUE_KEY_FLAG) strcat(temp,"unique ");
		if (flg & BLOB_FLAG) strcat(temp,"blob ");
		if (flg & UNSIGNED_FLAG) strcat(temp,"unsigned ");
		if (flg & ZEROFILL_FLAG) strcat(temp,"zerofill ");
		if (flg & NOT_NULL_FLAG) strcat(temp,"not null");
		Push(temp,STRING);
		break;
	}
#else
	Pop();
	Error("No mySQL support");
	Push("", STRING);
#endif
} 

void MYsqlTableName(void){
#ifdef HAVE_LIBMYSQL
	Stack *s;
	int res_index=0;
	int tb_index=0;
	MYSQL_RES *res=NULL;
	MYSQL_ROW	  row;

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_tablename expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid table index expression in mysqlTablename");
		return;
	}
	tb_index=s->intval;

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_tablename expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid MYSQL_RES expression in mysqlTablename");
		return;
	}
	res_index=s->intval;

	res=mysql_get_result(res_index);
	if(!res) {
		Error("Unable to find result index %d",res_index);
		Push("", STRING);
		return;
	}
	mysql_data_seek(res, tb_index);
	row=mysql_fetch_row(res);
	if (row) {
		Push((char *)row[0], STRING);
		return;
	}
	Push("", STRING);
#else
	Pop();
	Pop();
	Push("", STRING);
	Error("No mySQL support");
#endif	
}

void MYsqlListTables(void) {
#ifdef HAVE_LIBMYSQL
	char* dbname;
	Stack *s;
	MYSQL_RES *res=NULL;
	int tb_res; 
#ifndef APACHE
	static int First=1;
	char junk[1];
#endif
	char *tcpPort, *unixPort;
	char temp[16];

#ifndef APACHE
	if(First) {
		CurrentDB[0] = '\0';
		junk[0]='\0';
		CurrentTcpPort=&junk[0];
		CurrentUnixPort=&junk[0];
		First=0;
	}
#endif

	s = Pop();
	if(!s) {
		Error("Stack error in mysqllisttables expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid dbname expression in mysqllisttables");
		return;
	}
	else dbname=(char*)estrdup(1,s->strval);

	if(dbsock==NULL) {
		dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
		if(!dbsock) {
			Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
			Push("-1", LNUMBER);
			return;
		}
		CurrentTcpPort = getenv("MYSQL_TCP_PORT");
		CurrentUnixPort = getenv("MYSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MYSQL_TCP_PORT");
		unixPort = getenv("MYSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MYsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
#if DEBUG
			Debug("After connect\n");
#endif
			if(!dbsock) {
				Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
				Push("-1", LNUMBER);
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}

	if(mysql_select_db(dbsock,dbname)<0){
		Error("Unable to select mySQL table (%s)", mysql_error(&mysql));
		Push("-1", LNUMBER);
		return;
	}

	res = mysql_list_tables(dbsock, 0);
	if (res) tb_res=mysql_add_result(res);
	else {
		Error("Unable to find any table in %s", dbname); 
		Push("-1", LNUMBER);
		return;
	}

	sprintf(temp, "%d", tb_res);
	Push(temp, LNUMBER);

#else
	Pop();
	Push("0", LNUMBER);
	Error("No mySQL support");
#endif
}

MYSQL *mysqlGetDbSock() {
#ifdef HAVE_LIBMYSQL
	return(dbsock);
#else
	return(0);
#endif
}

MYSQL *mysqlGetPtr() {
#ifdef HAVE_LIBMYSQL
	return(&mysql);
#else
	return(0);
#endif
}

void mysqlSetCurrent(MYSQL *sock, char *newdb) {
#ifdef HAVE_LIBMYSQL
	CurrentTcpPort = getenv("MYSQL_TCP_PORT");
	CurrentUnixPort = getenv("MYSQL_UNIX_PORT");
	dbsock = sock;
	strcpy(CurrentDB, newdb);
#endif
}

void MYsqlCreateDB(void) {
#ifdef HAVE_LIBMYSQL
	char* dbname;
	Stack *s;
#ifndef APACHE
	static int First=1;
	char junk[1];
#endif
	char *tcpPort, *unixPort;

#ifndef APACHE
	if(First) {
		CurrentDB[0] = '\0';
		junk[0]='\0';
		CurrentTcpPort=&junk[0];
		CurrentUnixPort=&junk[0];
		First=0;
	}
#endif

	s = Pop();
	if(!s) {
		Error("Stack error in mysqllisttables expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid dbname expression in mysqllisttables");
		return;
	}
	dbname=(char*)estrdup(1,s->strval);

	if(dbsock==NULL) {
		dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
		if(!dbsock) {
			Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
			return;
		}
		CurrentTcpPort = getenv("MYSQL_TCP_PORT");
		CurrentUnixPort = getenv("MYSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MYSQL_TCP_PORT");
		unixPort = getenv("MYSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MYsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
#if DEBUG
			Debug("After connect\n");
#endif
			if(!dbsock) {
				Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}
 
	if(mysql_create_db(dbsock,dbname)<0) {
		Error("Unable to create mySQL Data Base (%s)", mysql_error(&mysql));
		return;
	}
#else
	Pop();
	Error("No mySQL support");
#endif
}

void MYsqlDropDB(void) {
#ifdef HAVE_LIBMYSQL
	char *dbname;
	Stack *s;
#ifndef APACHE
	static int First=1;
	char junk[1];
#endif
	char *tcpPort, *unixPort;

#ifndef APACHE
	if(First) {
		CurrentDB[0] = '\0';
		junk[0]='\0';
		CurrentTcpPort=&junk[0];
		CurrentUnixPort=&junk[0];
		First=0;
	}
#endif 

	s = Pop();
	if(!s) {
		Error("Stack error in mysqllisttables expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid dbname expression in mysqllisttables");
		return;
	}
	dbname=(char*)estrdup(1,s->strval);
 
	if(dbsock==NULL) {
		dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
		if(!dbsock) {
			Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
			return;
		}
		CurrentTcpPort = getenv("MYSQL_TCP_PORT");
		CurrentUnixPort = getenv("MYSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MYSQL_TCP_PORT");
		unixPort = getenv("MYSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MYsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
#if DEBUG
			Debug("After connect\n");
#endif
			if(!dbsock) {
				Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}

	if(mysql_drop_db(dbsock,dbname)<0){
		Error("Unable to drop mySQL Data Base (%s)", mysql_error(&mysql));
		return;
	}
#else
	Pop();
	Error("No mySQL support");
#endif
}

void MYsqlListDBs(void) {
#ifdef HAVE_LIBMYSQL
	char *hostname;
	MYSQL_RES *res=NULL;
	int db_res; 
#ifndef APACHE
	static int First=1;
	char junk[1];
#endif
	char *tcpPort, *unixPort;
	char temp[16];

#ifndef APACHE
	if(First) {
		CurrentDB[0] = '\0';
		junk[0]='\0';
		CurrentTcpPort=&junk[0];
		CurrentUnixPort=&junk[0];
		First=0;
	}
#endif
 
	if(dbsock==NULL) {
		dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
		if(!dbsock) {
			Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
			Push("-1", LNUMBER);
			return;
		}
		CurrentTcpPort = getenv("MYSQL_TCP_PORT");
		CurrentUnixPort = getenv("MYSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MYSQL_TCP_PORT");
		unixPort = getenv("MYSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MYsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
#if DEBUG
			Debug("After connect\n");
#endif
			if(!dbsock) {
				Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
				Push("-1", LNUMBER);
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}
	hostname=(char*)estrdup(1,mysql_get_host_info(dbsock));
	res = mysql_list_dbs(dbsock, 0);
	if (res) db_res=mysql_add_result(res);
	else {
		Error("Unable to find any data bases in host: %s", hostname); 
		Push("-1", LNUMBER);
		return;
	}

	sprintf(temp, "%d", db_res);
	Push(temp, LNUMBER);

#else
	Push("0", LNUMBER);
	Error("No mySQL support");
#endif
}

void MYsqlDBName(void){
#ifdef HAVE_LIBMYSQL
	Stack *s;
	int res_index=0;
	int db_index=0;
	MYSQL_RES *res=NULL;
	MYSQL_ROW     row;

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_tablename expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid table index expression in mysqlTablename");
		return;
	}
	if(s->intval == -1) {
		Error("Your table index is -1 : An error must have occurred");
		return;
	}
	db_index=s->intval;

	s = Pop();
	if(!s) {
		Error("Stack error in mysql_tablename expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid MYSQL_RES expression in mysqlTablename");
		return;
	}
	res_index=s->intval;

	res=mysql_get_result(res_index);
	if(!res) {
		Error("Unable to find result index %d",res_index);
		Push("", STRING);
		return;
	}
	mysql_data_seek(res, db_index);
	row=mysql_fetch_row(res);
	if (row) {
		Push((char *)row[0], STRING);
		return;
	}
	Push("", STRING);
#else
	Pop();
	Pop();
	Push("", STRING);
	Error("No mySQL support");
#endif
}

void MYsqlListFields(void) {
#ifdef HAVE_LIBMYSQL
	Stack *s;
	char *tablename=NULL;
#ifndef APACHE
	static int First=1;
	char junk[1];
#endif
	int j;
	char temp[16];
	MYSQL_RES *result=NULL;
	char *tcpPort, *unixPort;

#ifndef APACHE
	if(First) {
		CurrentDB[0] = '\0';
		junk[0]='\0';
		CurrentTcpPort=&junk[0];
		CurrentUnixPort=&junk[0];
		First=0;
	}
#endif

	s = Pop();
	if(!s) {
		Error("Stack error in mysql expression");
		Push("-1",LNUMBER);
		return;
	}
	if(s->strval) tablename = (char *)estrdup(1,s->strval);
	else {
		Error("No tablename in mysql_listfields expression");
		Push("-1",LNUMBER);
		return;
	}
	s = Pop();
	if(!s) {
		Error("Stack error in mysql expression");
		Push("-1",LNUMBER);
		return;
	}
	if(!s->strval) {
		Error("No database argument in mysql_listfields expression");
		Push("-1",LNUMBER);
		return;
	}
	if(dbsock==NULL) {	
		dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
		if(!dbsock) {	
			Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
			Push("-1",LNUMBER);
			return;
		}
		CurrentTcpPort = getenv("MYSQL_TCP_PORT");
		CurrentUnixPort = getenv("MYSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MYSQL_TCP_PORT");
		unixPort = getenv("MYSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MYsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = mysql_connect(&mysql, CurrentHost, CurrentUser?CurrentUser:0, CurrentPassword?CurrentPassword:0); 
#if DEBUG
			Debug("After connect\n");
#endif
			if(!dbsock) {
				Error("Unable to connect to mySQL socket (%s)",mysql_error(&mysql));
				Push("-1",LNUMBER);
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}
	if(strcmp(CurrentDB,s->strval)) {
		if(mysql_select_db(dbsock,s->strval)<0) {
			Error("Unable to select mySQL database (%s)",mysql_error(&mysql));
			Push("-1",LNUMBER);
			return;
		}
		strcpy(CurrentDB,s->strval);
	}

#if DEBUG
	Debug("Listing fields for table: %s\n",tablename);
#endif
#if APACHE
	block_alarms();
#endif
	result=mysql_list_fields(dbsock,tablename, 0);
	if(!result || mysql_num_fields(result)<1) {
#if APACHE
		unblock_alarms();
#endif
		Error("Unable to perform mysql_list_fields for table: %s",tablename);
		Push("-1",LNUMBER);
		return;
	}
#if APACHE
	unblock_alarms();
#endif
	if(result) j = mysql_add_result(result);
	else {
	        j=-1;
	}
	sprintf(temp,"%d",j);
	Push(temp,LNUMBER);
#else
	Pop();
	Pop();
	Push("0",LNUMBER);
	Error("No mySQL support");
#endif
}  /* MsqlListFields */

void MYsqlInsertId(void) {
#ifdef HAVE_LIBMYSQL
	char temp[16];
	MYSQL *res;
	
	res = mysqlGetDbSock();
	if(res != NULL) 
		sprintf(temp,"%ld",mysql_insert_id(res));
	else {
		Error("Invalid result index in mysql_insert_id");
		Push("-1", LNUMBER);
		return;
	}	
	Push(temp,LNUMBER);
#else
	Error("No mySQL support");
	Push("0", LNUMBER);
#endif
}

void MYsqlAffectedRows(void) {
#ifdef HAVE_LIBMYSQL
	char temp[16];
	MYSQL *res;

	res = mysqlGetDbSock();
	if(res) {
		sprintf(temp,"%ld",mysql_affected_rows(res));
	} else {
		Error("Invalid database identifier in mysql_affected_rows");
		Push("-1", LNUMBER);
		return;
	}	
	Push(temp,LNUMBER);
#else
	Error("No mySQL support");
	Push("0", LNUMBER);
#endif
}
