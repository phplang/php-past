/***[msql.c]******************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996 Rasmus Lerdorf                                         *
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
/* $Id: msql.c,v 1.13 1996/05/23 12:11:28 rasmus Exp $ */
/* mSQL is Copyright (c) 1993-1995 David J. Hughes */

/* Note that there is no mSQL code in this file */

#include <php.h>
#include <stdlib.h>
#if HAVE_LIBMSQL
#include <msql.h>
#endif
#include <parse.h>
#include <ctype.h>

#if HAVE_LIBMSQL
typedef struct ResultList {
	m_result *result;
	int ind;
	struct ResultList *next;
} ResultList;
static int dbsock=-1;
static char CurrentDB[128];
static ResultList *result_top=NULL;
static char *CurrentHost=NULL;
static char *CurrentTcpPort=NULL;
static char *CurrentUnixPort=NULL;
static int msql_ind=1;
#endif

void php_init_msql(void) {
#if HAVE_LIBMSQL
	static char junk[1];
	CurrentTcpPort=&junk[0];
	CurrentUnixPort=&junk[0];
	dbsock=-1;
	result_top=NULL;
	junk[0]='\0';
	CurrentHost=NULL;
	CurrentDB[0]='\0';
	msql_ind=1;
#endif
}

#if HAVE_LIBMSQL
int add_result(m_result *result) {
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
	new->ind    = msql_ind++;
	new->next   = NULL;
	return(msql_ind-1);
}

m_result *get_result(int count) {
	ResultList *new;

	new = result_top;
	while(new) {
		if(new->ind == count) return(new->result);
		new=new->next;
	}	
	return(NULL);
}
#endif

void del_result(int count) {
#if HAVE_LIBMSQL
	ResultList *new, *prev, *next;

	prev=NULL;
	new = result_top;
	while(new) {
		next = new->next;
		if(new->ind == count) {
			msqlFreeResult(new->result);
			if(prev) prev->next = next;
			else result_top = next;
			break;
		}
		prev=new;
		new=next;
	}	
#endif
}

void MsqlClose(void) {
#if HAVE_LIBMSQL
	ResultList *new,*next;

	new = result_top;	
	while(new) {
		next = new->next;	
		msqlFreeResult(new->result);
		new = next;
	}
	result_top = NULL;
	if(dbsock>-1) msqlClose(dbsock);
	dbsock=-1;
	CurrentDB[0]='\0';
#endif
}
	
void Msql(void) {
#if HAVE_LIBMSQL
	Stack *s;
	char *query=NULL;
#ifndef APACHE
	static int First=1;
	char junk[1];
#endif
	int j;
	char temp[16];
	m_result *result=NULL;
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
		Error("Stack error in msql expression");
		Push("-1",LNUMBER);
		return;
	}
	if(s->strval) query = (char *)estrdup(1,s->strval);
	else {
		Error("No query string in msql expression");
		Push("-1",LNUMBER);
		return;
	}
	s = Pop();
	if(!s) {
		Error("Stack error in msql expression");
		Push("-1",LNUMBER);
		return;
	}
	if(!s->strval) {
		Error("No database argument in msql expression");
		Push("-1",LNUMBER);
		return;
	}
	if(dbsock==-1) {
		dbsock = msqlConnect(CurrentHost);
		if(dbsock<0) {
			Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
			Push("-1",LNUMBER);
			return;
		}
		CurrentTcpPort = getenv("MSQL_TCP_PORT");
		CurrentUnixPort = getenv("MSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MSQL_TCP_PORT");
		unixPort = getenv("MSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = msqlConnect(CurrentHost);
#if DEBUG
			Debug("After connect\n");
#endif
			if(dbsock<0) {
				Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
				Push("-1",LNUMBER);
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}
	if(strcmp(CurrentDB,s->strval)) {
		if(msqlSelectDB(dbsock,s->strval)<0) {
			Error("Unable to select mSQL database (%s)",msqlErrMsg);
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
	if(msqlQuery(dbsock,query)<0) {
#if APACHE
		unblock_alarms();
#endif
		Error("Unable to perform query (%s)",msqlErrMsg);
		Push("-1",LNUMBER);
		return;
	}
#if APACHE
	unblock_alarms();
#endif
	result = msqlStoreResult();	
	if(result) j = add_result(result);
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
	Error("No mSQL support");
#endif
} 

void MsqlConnect(void) {
#if HAVE_LIBMSQL
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_connect");
		Push("-1",LNUMBER);
		return;
	}
	MsqlClose();
	if(strlen(s->strval)==0 || !strcasecmp(s->strval,"localhost")) CurrentHost=NULL;
	else {
		CurrentHost = (char *)estrdup(0,s->strval);
	}
#else
	Pop();
	Push("-1",LNUMBER);
	Error("No mSQL support");
#endif	
}

void MsqlResult(void) {
#if HAVE_LIBMSQL
	Stack *s;
	int i,j;
	char fieldname[128];
	char tablename[128];
	m_row record;
	m_result *result;
	m_field *msql_field;
	int ind, result_ind, done=0;
	char *tmp=NULL, *ret;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_result");
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
		Error("no field argument in msql_result");
		return;
	}
	
	s = Pop();
	if(!s) {
		Error("Stack error in msql_result");
		return;
	}
	if(s->strval) {
		ind = s->intval;
	} else {
		ind = 0;
	}

	s = Pop();
	if(!s) {
		Error("Stack error in msql_result");
		return;
	}
	if(s->strval) {
		result_ind = s->intval;
	} else {
		Error("Invalid result index in msql_result");
		return;
	}	
	result = get_result(result_ind);
	if(!result) {
		Error("Unable to find result index %d",result_ind);
		return;
	}
	i = msqlNumRows(result);
	j = msqlNumFields(result);
	if(i==0 || j==0 || ind>=i) {
		Push("",STRING);
		return;
	}
	msqlDataSeek(result,ind);
	record = msqlFetchRow(result);
	i=0;
	while(i<j && !done) {
		msqlFieldSeek(result,i);
		msql_field = msqlFetchField(result);
		if(!strcmp(msql_field->name,fieldname)) {
			if(strlen(tablename)==0 || (strlen(tablename) > 0 && strcmp(msql_field->table,tablename)==0)) {
				switch(msql_field->type) {
				case INT_TYPE:
					if(record[i]) {
						Push(record[i],LNUMBER);
					} else Push("",STRING);
					break;
				case CHAR_TYPE:
					if(record[i]) {
						tmp = estrdup(1,record[i]);
						Push((ret=AddSlashes(tmp,1)),STRING);
					} else Push("",STRING);
					break;
				case REAL_TYPE:
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
	Error("No mSQL support");
#endif
}

void MsqlFreeResult(void) {
#if HAVE_LIBMSQL 
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_result");
		return;
	}
	if(s->strval) {
		del_result(s->intval);
	} else {
		Error("Invalid result index in msql_freeresult");
		return;
	}	
#else
	Pop();
	Error("No mSQL support");
#endif
}

void MsqlNumRows(void) {
#if HAVE_LIBMSQL
	Stack *s;
	char temp[16];
	m_result *result;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_numrows");
		return;
	}
	if(s->strval) {
		result = get_result(s->intval);	
		if(!result) {
			Error("Unable to find result index %d",s->intval);
			return;
		}
		sprintf(temp,"%d",msqlNumRows(result));
	} else {
		Error("Invalid result index in msql_numrows");
		return;
	}	
	Push(temp,LNUMBER);
#else
	Pop();
	Error("No mSQL support");
#endif
}

void MsqlNumFields(void) {
#if HAVE_LIBMSQL
	Stack *s;
	char temp[16];
	m_result *result;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_numfields");
		return;
	}
	if(s->strval) {
		result = get_result(s->intval);	
		if(!result) {
			Error("Unable to find result index %d",s->intval);
			return;
		}
		sprintf(temp,"%d",msqlNumFields(result));
	} else {
		Error("Invalid result index in msql_numfields");
		return;
	}	
	Push(temp,LNUMBER);
#else
	Pop();
	Error("No mSQL support");
#endif
}

/* checks a field
 *
 * type = 1   pushes fieldname
 * type = 2   pushes fieldtype
 * type = 3   pushes fieldlen
 * type = 4   pushes fieldflags
 */
void MsqlField(int type) {
#if HAVE_LIBMSQL
	Stack *s;
	m_result *result=NULL;
	int field_ind=0;
	m_field *msql_field;
	char temp[64];
	int flg=0;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_fields expression");
		return;
	}
	if(s->strval) {
		field_ind = s->intval;
	} else {
		Error("field index error in msql_fields expression");
		return;
	}
	s = Pop();
	if(!s) {
		Error("Stack error in msql_fields expression");
		return;
	}
	if(s->strval) {
		result = get_result(s->intval);	
		if(!result) {
			Error("Unable to find result index %d",s->intval);
			return;
		}
	} else {
		Error("Invalid result index in msql_numrows");
		return;
	}	
	msqlFieldSeek(result,field_ind);
	msql_field = msqlFetchField(result);
	switch(type) {
	case 1:
		Push(msql_field->name,STRING);	
		break;
	case 2:
		switch(msql_field->type) {
		case INT_TYPE:
			Push("int",STRING);
			break;
		case CHAR_TYPE:
			Push("char",STRING);
			break;
		case REAL_TYPE:
			Push("real",STRING);
			break;
		case IDENT_TYPE:
			Push("ident",STRING);
			break;
		case NULL_TYPE:
			Push("null",STRING);
			break;
		default:
			Push("unknown",STRING);
			break;
		}
		break;
	case 3:
		sprintf(temp,"%d",msql_field->length);
		Push(temp,LNUMBER);
		break;
	case 4:
		flg = msql_field->flags;
		temp[0]='\0';
		if(flg & PRI_KEY_FLAG) strcat(temp,"primary key");
		if(flg & NOT_NULL_FLAG) {
			if(strlen(temp))
				strcat(temp," not null");
			else
				strcat(temp,"not null");
		}
		Push(temp,STRING);
		break;
	}
#else
	Pop();
	Error("No mSQL support");
#endif
} 

void MsqlRegCase(void) {
	Stack *st;
    char *s,*t;
    static char *buf=NULL;

	st = Pop();
	if(!st) {
		Error("Stack error in msql_regcase");
		return;
	}
	if(st->strval && *st->strval) {
		buf=emalloc(1,sizeof(char)*(4*strlen(st->strval)+1));
		s=st->strval;
		t=buf;
		while(*s) {
			if(tolower(*s) == toupper(*s)) {
				*t++=*s++;
				continue;
			}
			*t++='[';
			*t++=toupper(*s);
			*t++=tolower(*s++);
			*t++=']';
		}
		*t='\0';
		Push(buf,STRING);
	} else {
		Push("",STRING);
	}
}


void MsqlTableName(void){
#if HAVE_LIBMSQL
	Stack *s;
	int res_index=0;
	int tb_index=0;
	m_result *res=NULL;
	m_row	  row;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_tablename expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid table index expression in msqlTablename");
		return;
	}
	tb_index=s->intval;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_tablename expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid m_result expression in msqlTablename");
		return;
	}
	res_index=s->intval;

	res=get_result(res_index);
	msqlDataSeek(res, tb_index);
	row=msqlFetchRow(res);
	if (row) {
		Push((char *)row[0], STRING);
		return;
	}
	Push("", STRING);
#else
	Pop();
	Pop();
	Push("", STRING);
	Error("No mSQL support");
#endif	
}

void MsqlListTables(void) {
#if HAVE_LIBMSQL
	char* dbname;
	Stack *s;
	m_result *res=NULL;
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
		Error("Stack error in msqllisttables expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid dbname expression in msqllisttables");
		return;
	}
	else dbname=(char*)estrdup(1,s->strval);

	if(dbsock==-1) {
		dbsock = msqlConnect(CurrentHost);
		if(dbsock<0) {
			Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
			return;
		}
		CurrentTcpPort = getenv("MSQL_TCP_PORT");
		CurrentUnixPort = getenv("MSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MSQL_TCP_PORT");
		unixPort = getenv("MSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = msqlConnect(CurrentHost);
#if DEBUG
			Debug("After connect\n");
#endif
			if(dbsock<0) {
				Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}

	if(msqlSelectDB(dbsock,dbname)<0){
		Error("Unable to select mSQL table (%s)", msqlErrMsg);
		return;
	}

	res = msqlListTables(dbsock);
	if (res) tb_res=add_result(res);
	else {
		Error("Unable to find any table in %s", dbname); 
		return;
	}

	sprintf(temp, "%d", tb_res);
	Push(temp, LNUMBER);

#else
	Pop();
	Push("0", LNUMBER);
	Error("No mSQL support");
#endif
}

int msqlGetDbSock() {
#if HAVE_LIBMSQL
	return(dbsock);
#else
	return(0);
#endif
}

void msqlSetCurrent(int sock, char *newdb) {
#if HAVE_LIBMSQL
	CurrentTcpPort = getenv("MSQL_TCP_PORT");
	CurrentUnixPort = getenv("MSQL_UNIX_PORT");
	dbsock = sock;
	strcpy(CurrentDB,newdb);
#endif
}

void MsqlCreateDB(void) {
#if HAVE_LIBMSQL
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
		Error("Stack error in msqllisttables expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid dbname expression in msqllisttables");
		return;
	}
	dbname=(char*)estrdup(1,s->strval);

	if(dbsock==-1) {
		dbsock = msqlConnect(CurrentHost);
		if(dbsock<0) {
			Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
			return;
		}
		CurrentTcpPort = getenv("MSQL_TCP_PORT");
		CurrentUnixPort = getenv("MSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MSQL_TCP_PORT");
		unixPort = getenv("MSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MsqlClose();

#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = msqlConnect(CurrentHost);
#if DEBUG
			Debug("After connect\n");
#endif
			if(dbsock<0) {
				Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}
 
	if(msqlCreateDB(dbsock,dbname)<0){
		Error("Unable to create mSQL Data Base (%s)", msqlErrMsg);
		return;
	}
#else
	Pop();
	Error("No mSQL support");
#endif
}

void MsqlDropDB(void) {
#if HAVE_LIBMSQL
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
		Error("Stack error in msqllisttables expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid dbname expression in msqllisttables");
		return;
	}
	dbname=(char*)estrdup(1,s->strval);
 
	if(dbsock==-1) {
		dbsock = msqlConnect(CurrentHost);
		if(dbsock<0) {
			Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
			return;
		}
		CurrentTcpPort = getenv("MSQL_TCP_PORT");
		CurrentUnixPort = getenv("MSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MSQL_TCP_PORT");
		unixPort = getenv("MSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = msqlConnect(CurrentHost);
#if DEBUG
			Debug("After connect\n");
#endif
			if(dbsock<0) {
				Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}

	if(msqlDropDB(dbsock,dbname)<0){
		Error("Unable to drop mSQL Data Base (%s)", msqlErrMsg);
		return;
	}
#else
	Pop();
	Error("No mSQL support");
#endif
}

void MsqlListDBs(void) {
#if HAVE_LIBMSQL
	char *hostname;
	m_result *res=NULL;
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
 
	if(dbsock==-1) {
		dbsock = msqlConnect(CurrentHost);
		if(dbsock<0) {
			Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
			return;
		}
		CurrentTcpPort = getenv("MSQL_TCP_PORT");
		CurrentUnixPort = getenv("MSQL_UNIX_PORT");
	} else {
#if DEBUG
		Debug("Open socket found, checking to see if still valid\n");
#endif
		tcpPort = getenv("MSQL_TCP_PORT");
		unixPort = getenv("MSQL_UNIX_PORT");
		if((tcpPort && strcmp(tcpPort,CurrentTcpPort)) || (unixPort && strcmp(unixPort,CurrentUnixPort))) {
#if DEBUG
			Debug("Not valid, need to reopen\n");
#endif
			MsqlClose();
#if DEBUG
			Debug("About to connect\n");
#endif
			dbsock = msqlConnect(CurrentHost);
#if DEBUG
			Debug("After connect\n");
#endif
			if(dbsock<0) {
				Error("Unable to connect to mSQL socket (%s)",msqlErrMsg);
				return;
			}
			CurrentTcpPort = tcpPort;
			CurrentUnixPort = unixPort;
		}
	}
	hostname=(char*)estrdup(1,msqlGetHostInfo());
	res = msqlListDBs(dbsock);
	if (res) db_res=add_result(res);
	else {
		Error("Unable to find any data bases in host: %s", hostname); 
		return;
	}

	sprintf(temp, "%d", db_res);
	Push(temp, LNUMBER);

#else
	Push("0", LNUMBER);
	Error("No mSQL support");
#endif
}

void MsqlDBName(void){
#if HAVE_LIBMSQL
	Stack *s;
	int res_index=0;
	int db_index=0;
	m_result *res=NULL;
	m_row     row;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_tablename expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid table index expression in msqlTablename");
		return;
	}
	db_index=s->intval;

	s = Pop();
	if(!s) {
		Error("Stack error in msql_tablename expression");
		return;
	}
	if(!s->strval) {
		Error("Invalid m_result expression in msqlTablename");
		return;
	}
	res_index=s->intval;

	res=get_result(res_index);
	msqlDataSeek(res, db_index);
	row=msqlFetchRow(res);
	if (row) {
		Push((char *)row[0], STRING);
		return;
	}
	Push("", STRING);
#else
	Pop();
	Pop();
	Push("", STRING);
	Error("No mSQL support");
#endif
}
