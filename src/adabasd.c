/***[adabasd.c]***************************************************[TAB=4]****\
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

/****************************************************************************\
* Extensions for ADABAS D                                                    *
*                                                                            *
* Copyright 1997 Andreas Karajannis                                          *
* Based on solid.c by Jeroen van der Most (jvdmost@digiface.nl)              *
* Based on pg95.c, written by Adam Sussman (asussman@vidya.com)              *
*                                                                            *
*                                                                            *
*                                                                            *
\****************************************************************************/
 
#include "php.h"
#include <stdlib.h>
#ifdef HAVE_LIBADABAS
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>
#endif
#include "parse.h"
#include <ctype.h>

#if APACHE
#ifndef DEBUG
#undef palloc
#endif
#include "http_protocol.h"
#include "http_request.h"
#endif

#ifdef HAVE_LIBADABAS

typedef struct {
	char name[32];
	char *value;
	long int vallen;
	int passthru;
} ResultValue;

typedef struct AdaResult {
	HSTMT stmt;
	ResultValue *values;
	int numcols;
	int fetched;
} AdaResult;

typedef struct AdaResultList {
	AdaResult            *result;
	int                  index;
	struct AdaResultList *next;
	int conn_index;
} AdaResultList;

typedef struct AdaConnList {
	HDBC               conn;
	int                index;
	struct AdaConnList *next;
} AdaConnList;

static AdaResultList *ada_result_top=NULL;
static AdaConnList *ada_conn_top=NULL;
static int ada_ind=1;
static int ada_conn_ind=1;
static int ada_def_conn=0;
static HENV henv;
char *defDB;
char *defUser;
char *defPW;
#endif

#ifdef HAVE_LIBADABAS
void php_init_adabas(char *DB, char *User, char *PW) 
{
	if(DB)
		defDB = (char*) estrdup(1,DB);
	else 
		defDB = NULL;
	if(User) 
		defUser = (char*) estrdup(1,User);
	else 
		defUser = NULL;
	if(PW) 
		defPW = (char*) estrdup(1,PW);
	else 
		defUser = NULL;

	ada_result_top = NULL;
	ada_conn_top = NULL;
	ada_ind = 2;
	ada_conn_ind = 1;
	ada_def_conn = 0;
	SQLAllocEnv(&henv);
}
#endif

/***************************************************
 *
 * List maintainers (AdaResultList and AdaConnList)
 *
 ***************************************************/

#ifdef HAVE_LIBADABAS
int ada_add_result(AdaResult *result, int conn_in)
{
	AdaResultList *new;

	new = ada_result_top;
	if(!new){
		new = emalloc(0,sizeof(AdaResultList));
		if(new == NULL) 
			Error("Out of memory");
		ada_result_top = new;
	} 
	else{
		while(new->next) 
			new=new->next;
		new->next = emalloc(0,sizeof(AdaResultList));
		if(new->next == NULL)
			Error("Out of memory");
		else
			new = new->next;
	}

	new->result = result;
	new->index  = ada_ind++;
	new->conn_index = conn_in;
	new->next   = NULL;
	return ada_ind - 1;
}

AdaResult *ada_get_result(int count)
{
	AdaResultList *new;

	if(count == 1) 
		return NULL;

	new = ada_result_top;
	while(new){
		if(new->index == count) 
			return new->result;
		new = new->next;
	}
	return NULL;
}

void ada_del_result(int count)
{
	AdaResultList *new, *prev, *next;

	if(count == 1) 
		return;

	prev = NULL;
	new = ada_result_top;
	while(new){
		next = new->next;
		if(new->index == count){ 
			SQLFreeStmt(new->result->stmt, SQL_DROP);
			if(prev) 
				prev->next = next;
			else 
				ada_result_top = next;
			break;
		}
		prev = new;
		new = next;
	}
}

int ada_add_conn(HDBC conn)
{
	AdaConnList *new;

	new = ada_conn_top;
	if(!new){
		new = emalloc(0,sizeof(AdaConnList));
		ada_conn_top = new;
	} 
	else{
		while(new->next) 
			new=new->next;
		new->next = emalloc(0,sizeof(AdaConnList));
		new = new->next;
	}

	new->conn  = conn;
	new->index = ada_conn_ind++;
	new->next  = NULL;
	return ada_conn_ind - 1;
}

void make_def_conn(void)
{
	HDBC    new_conn;
	RETCODE rc;
	char    state[6];
	SDWORD  error;
	char    errormsg[255];
	SWORD   errormsgsize;

	SQLAllocConnect(henv, &new_conn);
	rc = SQLConnect(new_conn, defDB, SQL_NTS, defUser, SQL_NTS, defPW, SQL_NTS);

	if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		SQLError(henv, new_conn, SQL_NULL_HSTMT, state, &error, errormsg,
				sizeof(errormsg)-1, &errormsgsize);
		Error("Could not connect to database (%s)", errormsg);
	}else{
		ada_def_conn = ada_add_conn(new_conn);
	}

}

HDBC ada_get_conn(int count)
{
	AdaConnList *new;

	if(count == 0){
		if(ada_def_conn == 0){
			make_def_conn();
		}
	count = ada_def_conn;
	}

	new = ada_conn_top;

	while(new){
		if(new->index == count) 
			return new->conn;
		new = new->next;
	}
	return NULL;
}

void ada_del_conn(HDBC conn)
{
	AdaConnList *new, *prev, *next;

	prev = NULL;
	new = ada_conn_top;
	while (new) {
		next = new->next;
		if (new->conn == conn) {
			SQLDisconnect(new->conn);
			SQLFreeConnect(new->conn);
			if (prev) 
				prev->next = next;
			else 
				ada_conn_top = next;
			break;
		}
		prev = new;
		new = next;
	}
}

void ada_sql_error(HDBC conn, HSTMT stmt, char *func)
{
	char    state[6];     /* Not used */
	SDWORD  error;        /* Not used */
	char    errormsg[255];
	SWORD   errormsgsize; /* Not used */

	SQLError(henv, conn, stmt, state, &error, errormsg,
			sizeof(errormsg)-1, &errormsgsize);

	if(func)
		Error("Function %s SQL error: %s, SQL state %s", func, errormsg, state);
	else
		Error("SQL error: %s, SQL state %s", errormsg, state);
}

#endif /* HAVE_LIBADABAS */

/* PHP house keeping function */

void Ada_closeAll(void)
{
#ifdef HAVE_LIBADABAS
	AdaResultList *lnew, *lnext;
	AdaConnList   *cnew, *cnext;

	lnew = ada_result_top; 
	while(lnew){
		lnext = lnew->next;
		SQLFreeStmt(lnew->result->stmt, SQL_DROP);
		lnew = lnext;
	}
	ada_result_top = NULL;

	cnew = ada_conn_top;
	while(cnew){
		cnext = cnew->next;
		if(cnew->conn){
			SQLDisconnect(cnew->conn);
			SQLFreeConnect(cnew->conn);
		}
		cnew=cnext;
	}
	ada_conn_top = NULL;
#endif
}

/* Main User Functions */

void Ada_exec(void)
{
#ifdef HAVE_LIBADABAS
	Stack       *s;
	int         conn;
	char        *query;
	int         i, j;
	char        temp[16];
	AdaResult   *result=NULL;
	HDBC        curr_conn=NULL;
	short       resultcols;
	SWORD       colnamelen; /* Not used */
	SDWORD      displaysize, coltype;
	RETCODE     rc;

	/* get args */
	s = Pop();
	if(!s){
		Error("Stack error in ada_exec");
		Push("0", LNUMBER);
		return;
	}
	if(s->strval) 
		query = (char*) estrdup(1,s->strval);
	else{
		Error("No query string in ada_exec");
		Push("0", LNUMBER);
		return;
	}

	s = Pop();
	if(!s){
		Error("Stack error in ada_exec");
		Push("0", LNUMBER);
		return;
	}
	if(s->strval)
		conn = s->intval;
	else
		conn = 0;

	curr_conn = ada_get_conn(conn);
	if(curr_conn == NULL){
		Error("Bad Adabas connection number");
		Push("0", LNUMBER);
		return;
	}

	StripDollarSlashes(query);
#if DEBUG
	Debug("Sending query: %s\n",query);
#endif
   	result = (AdaResult *)emalloc(0, sizeof(AdaResult));
	if(result == NULL){
		Error("Out of memory");
		Push("0", LNUMBER);
		return;
	}
	rc = SQLAllocStmt(curr_conn, &(result->stmt));
	if(rc == SQL_INVALID_HANDLE){
		Error("SQLAllocStmt returned error 'Invalid Handle'");
		Push("0", LNUMBER);
		return;
	}
	if(rc == SQL_ERROR){
		ada_sql_error(curr_conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		Push("0", LNUMBER);
        return;
	}

/* Set scrolling cursor here, works only if supp. by the driver */
	if((rc = SQLSetStmtOption(result->stmt, SQL_CURSOR_TYPE, SQL_CURSOR_DYNAMIC))
		!= SQL_SUCCESS){
		ada_sql_error(curr_conn, result->stmt, " SQLSetStmtOption");
		SQLFreeStmt(result->stmt, SQL_DROP);
		Push("0", LNUMBER);
		return;
	}
/* End of scrolling cursor */
	
	if((rc = SQLExecDirect(result->stmt, query, SQL_NTS)) != SQL_SUCCESS){
		ada_sql_error(curr_conn, result->stmt, "SQLExecDirect");
		SQLFreeStmt(result->stmt, SQL_DROP);
		j = 0;
	}else{
		SQLNumResultCols(result->stmt, &resultcols);
		if(resultcols == 0)
			j = 1;
		else{
			result->numcols = resultcols;
			result->fetched = 0;
			result->values = (ResultValue *)emalloc(0, sizeof(ResultValue)*resultcols);
			
			if(result->values == NULL){
				Error("Out of memory");
				Push("0", LNUMBER);
			    return;
			}
					
			for(i = 0; i < resultcols; i++){
				SQLColAttributes(result->stmt, i+1, SQL_COLUMN_NAME,
					result->values[i].name, sizeof(result->values[i].name),
					&colnamelen, NULL);
				SQLColAttributes(result->stmt, i+1, SQL_COLUMN_DISPLAY_SIZE,
					NULL, 0, NULL, &displaysize);
#if DEBUG
				Debug("%s : %ld\n",result->values[i].name,displaysize);
#endif
				/* Decision, whether a column will be output directly or returned into a 
				 * PHP variable is based on two attributes:
				 * - Anything longer than 4096 Bytes (SQL_COLUMN_DISPLAY_SIZE)
				 * - and/or of datatype binary (SQL_BINARY,SQL_VARBINARY,SQL_LONGVARBINARY)
				 * will be sent direct to the client.
				 * Hint: SQL_BINARY and SQL_VARBINARY can be returned to PHP with the SQL
				 * function HEX(), e.g. SELECT HEX(SYSKEY) SYSKEY FROM MYTABLE.
				 */
			   	if(displaysize > 4096){
					displaysize = 4096;
					result->values[i].passthru = 1;
				} else {
					result->values[i].passthru = 0;
				}
				SQLColAttributes(result->stmt, i+1, SQL_COLUMN_TYPE,
									NULL, 0, NULL, &coltype);

				switch(coltype){
					case SQL_BINARY:
					case SQL_VARBINARY:
					case SQL_LONGVARBINARY:
						result->values[i].passthru = 1;
						result->values[i].value = (char *)emalloc(0,displaysize);
						SQLBindCol(result->stmt, i+1, SQL_C_BINARY,result->values[i].value,
									displaysize, &result->values[i].vallen);
						break;
					default:
						result->values[i].value = (char *)emalloc(0,displaysize + 1);
						SQLBindCol(result->stmt, i+1, SQL_C_CHAR, result->values[i].value,
									displaysize + 1, &result->values[i].vallen);
						break;
				}
			}
			
		j = ada_add_result(result,conn);
		}
	}

	sprintf(temp, "%d", j);
	Push(temp,LNUMBER);
#else
	Pop();
	Pop();
	Push("0", LNUMBER);
	Error("No Adabas support");
#endif
}

void Ada_fetchRow(int mode)
{
#ifdef HAVE_LIBADABAS
	Stack       *s;
	int         res_ind;
	SDWORD      rownum = 1;
	AdaResult   *result;
	RETCODE     rc;
	UDWORD	    crow;
	UWORD	    RowStatus[1];
			
	if(mode){
		s = Pop();
		if(!s){
			Error("Stack error in ada_fetchrow");
			Push("",STRING);
			return;
		}
		if(s->strval)
			rownum = s->intval;
	}

	s = Pop();
	if(!s){
		Error("Stack error in ada_fetchrow");
		Push("", STRING);
		return;
	}
	if(s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if(res_ind == 1){
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}

	/* check result */
	result = ada_get_result(res_ind);
	if(result == NULL){
		Error("Unable to find result index %s", res_ind);
		Push("", STRING);
		return;
	}

	if(mode)
		rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
	else
		rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	
	if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		Push("0", LNUMBER);
		return;
	}

	if(mode) 
		result->fetched = rownum;
	else 
		result->fetched++;
	
	Push("1", LNUMBER);
	return;

#else
	Pop();
	Push("", STRING);
	Error("No Adabas support");
#endif
}

void Ada_result(void)
{
#ifdef HAVE_LIBADABAS
	Stack       *s;
	char        *field;
	int         res_ind;
	int         field_ind;
	AdaResult   *result;
	int         i;
	RETCODE     rc;
	UDWORD	    crow;
	UWORD	    RowStatus[1];

	field_ind = -1;
	field = NULL;

	s = Pop();
	if(!s){
		Error("Stack error in ada_result");
		Push("", STRING);
		return;
	}
	if(s->strval)
		if(s->type == STRING)
			field = estrdup(1,s->strval);
		else
			field_ind = s->intval;
	else{
		Error("No field argument in ada_result");
		Push("", STRING);
		return;
	}

	s = Pop();
	if (!s){
		Error("Stack error in ada_result");
		Push("", STRING);
		return;
	}
	if(s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if(res_ind == 1){
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}

	/* check result */
	result = ada_get_result(res_ind);
	if(result == NULL){
		Error("Unable to find result index %d", res_ind);
		Push("", STRING);
		return;
	}

	/* get field index if the field parameter was a string */
	if(field != NULL){
		for(i = 0; i < result->numcols; i++){
			if(!strcasecmp(result->values[i].name, field)){
				field_ind = i;
				break;
			}
		}
		
		if(field_ind < 0){
			Error("Field %s not found", field);
			Push("", STRING);
			return;
		}
    }
	else{
      		/* check for limits of field_ind if the field parameter was an int */
            if(field_ind >= result->numcols || field_ind < 0){
             	Error("Field index is larger than the number of fields");
                Push("", STRING);
                return;
			}
		}

    if(result->fetched == 0){
		/* User forgot to call ada_fetchrow(), let's do it here */
		rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
			Push("", STRING);
			return;
		}
		result->fetched++;
	}
	
	if(result->values[field_ind].vallen == SQL_NULL_DATA)
		Push("", STRING); /* FIXME: better value for null ? */
	else{
		if(result->values[field_ind].value != NULL &&
				result->values[field_ind].vallen != SQL_NO_TOTAL &&
				result->values[field_ind].passthru == 0){
			Push(AddSlashes(result->values[field_ind].value,0), STRING);
		}else{
			char buf[4096];
			SDWORD fieldsize = 4096;
		
			/* Make sure that the Header is sent */	
			php_header(0, NULL);
			
			if(result->values[field_ind].vallen != SQL_NO_TOTAL){
				fieldsize = result->values[field_ind].vallen;
			}
			/* Output what's already in the field */
			for(i = 0;i < fieldsize - 1; i++)
				PUTC(result->values[field_ind].value[i]);
			/* Do we have more data available? */
			if(result->values[field_ind].vallen != SQL_NO_TOTAL){
				return;
			}
			
			/* Call SQLGetData until SQL_SUCCESS is returned */
			while(1){
				rc = SQLGetData(result->stmt, field_ind + 1, SQL_C_BINARY,
						buf, 4096, &fieldsize);

			   	if(rc == SQL_ERROR){
					ada_sql_error(ada_conn_top->conn, result->stmt, "SQLGetData");	
					break;
				}
				
				if(rc == SQL_SUCCESS_WITH_INFO)
				   	fieldsize = 4096;	

				for(i = 0; i < fieldsize - 1; i++)
					PUTC(buf[i]);

				if(rc == SQL_SUCCESS)
					break;
			}
		}
	}
	return;

#else
	Pop();
	Pop();
	Push("", STRING);
	Error("No Adabas support");
#endif
}

void Ada_resultAll(int mode)
{
#ifdef HAVE_LIBADABAS
	Stack       *s;
	char        tmp[256];
	int         res_ind;
	AdaResult   *result;
	int         i;
	RETCODE     rc;
	UDWORD	    crow;
	UWORD	    RowStatus[1];

	memset(tmp,'\0',256);
	if(mode){
		s = Pop();
		if (!s) {
			Error("Stack error in ada_result_all");
			Push("-1",LNUMBER);
			return;
		}
		if (s->strval)
			sprintf(tmp,"<table %s ><tr>",s->strval);
		else {
			Error("No format argument in ada_result_all");
			Push("-1",LNUMBER);
			return;
		}
	}
	else strcpy(tmp,"<table><tr>");

	s = Pop();
	if(!s){
		Error("Stack error in ada_result_all");
		Push("-1",LNUMBER);
		return;
	}
	if(s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if(res_ind == 1){
		Error("No tuples available at this result index");
		Push("-1",LNUMBER);
		return;
	}

	/* check result */
	result = ada_get_result(res_ind);
	if(result == NULL){
		Error("Unable to find result index %d", res_ind);
		Push("-1",LNUMBER);
		return;
	}

	rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		Echo("<h2>No rows found</h2",0);
	        Push("0",LNUMBER);
                return;
	}

 	Echo(tmp,0); /* Start table tag */

	for(i = 0; i < result->numcols; i++){
		sprintf(tmp,"<th>%s</th>", result->values[i].name);
		Echo (tmp,0);
	}

	Echo("</tr>\n",0);

	while(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO){
		result->fetched++;
		Echo("<tr>",0);
		for(i = 0; i < result->numcols; i++){
			if(result->values[i].vallen == SQL_NULL_DATA)
				sprintf(tmp,"<td> </td>");
			else{
				if(result->values[i].passthru == 0)
					sprintf(tmp,"<td>%s</td>",
						AddSlashes(result->values[i].value,0));
				else 
					sprintf(tmp,"<td>Unsupp. data type</td>");
			}
			Echo(tmp,0);
		}
		Echo("</tr>\n",0);
		rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	}
	Echo("</table>\n",0);
  	sprintf(tmp,"%d",result->fetched);
  	Push(tmp,LNUMBER);
  	return;

#else
	Pop();
	Pop();
	Push("", STRING);
	Error("No Adabas support");
#endif
}

void Ada_freeResult(void)
{
#ifdef HAVE_LIBADABAS
	Stack	*s;

	s = Pop();
	if(!s){
		Error("Stack error in ada_freeresult");
		return;
	}
	if(s->strval){
		ada_del_result(s->intval);
	} 
	else{
		Error("Invalid result index in ada_freeresult");
		return;
	}
#else
	Pop();
	Error("No Adabas support");
#endif
}

void Ada_connect(void)
{
#ifdef HAVE_LIBADABAS
	Stack   *s;
	HDBC    new_conn;
	char    *db=NULL;
	char    *uid=NULL;
	char    *pwd=NULL;
	char    *temp;
	int     j;
	RETCODE rc;

	s = Pop();
	if(!s){
		Error("Stack error in ada_connect");
		Push("0", LNUMBER);
		return;
	}
	if(s->strval)
		pwd = estrdup(1,s->strval);
	else{
		Error("No password supplied in ada_connect");
		Push("0", LNUMBER);
		return;
	}
	
	s = Pop();
	if(!s){
		Error("Stack error in ada_connect");
		Push("0", LNUMBER);
		return;
	}
	if(s->strval) 
		uid = estrdup(1,s->strval);
	else{
		Error("No user name supplied in ada_connect");
		Push("0", LNUMBER);
		return;
	}

	s = Pop();
	if(!s){
		Error("Stack error in ada_connect");
		Push("0", LNUMBER);
		return;
	}
	if(s->strval) 
		db = estrdup(1,s->strval);
	else{
		Error("No database name supplied in ada_connect");
		Push("0", LNUMBER);
		return;
	}
	
	SQLAllocConnect(henv, &new_conn);

	rc = SQLConnect(new_conn, db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);

    if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		ada_sql_error(new_conn, SQL_NULL_HSTMT, "SQLConnect");
		temp = (char*) emalloc(1,2);
		sprintf(temp, "0");
	}else{
		j = ada_add_conn(new_conn);
		temp = (char*) emalloc(1,11);
		sprintf(temp, "%d", j);
	}
	Push(temp, LNUMBER);

#else
	Pop();
	Pop();
	Pop();
	Error("No Adabas support");
#endif
}

void Ada_close(void)
{
#ifdef HAVE_LIBADABAS
	Stack *s;
	int   conn_ind;
	HDBC  conn;
	AdaResultList *lnew, *lnext;

	s = Pop();
	if(!s){
		Error("Stack error in ada_close");
		return;
	}
	if(s->strval) 
		conn_ind = s->intval;
	else 
		conn_ind = 0;

	conn = ada_get_conn(conn_ind);
	if(conn){
		/* This next bit of code is needed to make sure that we delete any
		 * result pointers associated with the connection we are about to
		 * close.  If we don't do this, the Solid client library will SEGV
		 * on us if we later let AdaCloseAll() attempt to free these
		 * result pointers by calling SQLFreeStmt().  I still think this
		 * is a problem that should be fixed on the Ada side, but this should
		 * suffice for now.
		 */
		lnew = ada_result_top; 
		while(lnew){
			lnext = lnew->next;
			if(lnew->conn_index == conn_ind)
			   	ada_del_result(lnew->index);
			lnew = lnext;
		}
		ada_del_conn(conn);
	}
#else
	Pop();
	Error("No Adabas support");
#endif
}

void Ada_numRows(void)
{
#ifdef HAVE_LIBADABAS
	Stack       *s;
	int         res_ind;
	AdaResult   *result;
	SDWORD      rows;
	char        temp[16];

	s = Pop();
	if(!s){
		Error("Stack error in ada_numrows");
		Push("-1", LNUMBER);
		return;
	}
	if(s->strval) 
		res_ind = s->intval;
	else
		res_ind = 0;

	if(res_ind == 1){
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = ada_get_result(res_ind);
	if(result == NULL){
		Error("Bad result index in ada_numrows");
		Push("-1", LNUMBER);
		return;
	}

	SQLRowCount(result->stmt, &rows);
	sprintf(temp, "%ld", rows);
	Push(temp, LNUMBER);
#else
	Pop();
	Error("No Adabas support");
#endif
}


void Ada_numFields(void)
{
#ifdef HAVE_LIBADABAS
	Stack       *s;
	int         res_ind;
	AdaResult   *result;
	char        temp[16];

	s = Pop();
	if(!s){
		Error("Stack error in ada_numfields");
		Push("-1", LNUMBER);
		return;
	}
	if(s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if(res_ind == 1){
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = ada_get_result(res_ind);
	if(result == NULL){
		Error("Bad result index in ada_numfields");
		Push("-1", LNUMBER);
		return;
	}

	sprintf(temp, "%d", result->numcols);
	Push(temp, LNUMBER);
#else
	Pop();
	Error("No Adabas support");
#endif
}

void Ada_Field(int mode)
{
#ifdef HAVE_LIBADABAS
	Stack	*s;
	int	res_ind;
	int	field_ind;
	AdaResult	*result;
	char	tmp[32];
	SWORD	tmplen;
	SDWORD	len;
	
	s = Pop();
	if(!s){
		Error("Stack error in ada_field*");
		Push("", STRING);
		return;
	}
	if(s->strval)
		field_ind = s->intval;
	else{
		Error("No field index given in ada_field*");
		Push("", STRING);
		return;
	}

	s = Pop();
	if(!s){
		Error("Stack error in ada_field*");
		Push("", STRING);
		return;
	}
	if(s->strval)
		res_ind = s->intval;
	else
		res_ind = 0;

	if(res_ind == 1) {
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}	
	result = ada_get_result(res_ind);
	if(result == NULL){
		Error("Bad result index in ada_field*");
		Push("", STRING);
		return;
	}

	if(field_ind >= result->numcols){
		Error("Field index larger than number of fields");
		Push("", STRING);
		return;
	}

	switch(mode){
		case 1: /* name */
			strcpy(tmp,result->values[field_ind].name);
			break;
		case 2: /* type */
			SQLColAttributes(result->stmt, field_ind+1, SQL_COLUMN_TYPE_NAME,
					tmp,31,&tmplen, NULL); 
			break;
		case 3: /* len */
			SQLColAttributes(result->stmt, field_ind+1, SQL_COLUMN_PRECISION,
					NULL,0,NULL,&len);
			sprintf(tmp,"%ld",len);
			break;
	}

if(mode == 3)
		Push(tmp,LNUMBER);
	else
		Push(tmp, STRING);
#else
	Pop();
	Pop();
	Error("No Adabas support");
#endif
}

void Ada_fieldNum(void)
{
#ifdef HAVE_LIBADABAS
	Stack       *s;
	int         res_ind;
	int         field_ind;
	char        *fname;
	AdaResult   *result;
	char        tmp[16];
	int         i;

	s = Pop();
	if(!s){
		Error("Stack error in ada_fieldnum");
		Push("-1", LNUMBER);
		return;
	}
	if(s->strval) 
		fname = estrdup(1,s->strval);
	else{
		Error("No field name given in ada_fieldnum");
		Push("-1", LNUMBER);
		return;
	}
	
	s = Pop();
	if(!s){
		Error("Stack error in ada_fieldnum");
		Push("-1", LNUMBER);
		return;
	}
	if(s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if(res_ind == 1) {
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = ada_get_result(res_ind);
	if(result == NULL){
		Error("Bad result index in ada_fieldnum");
		Push("-1", LNUMBER);
		return;
	}

	field_ind = -1;
	for(i=0; i<result->numcols; i++){
		if(strcasecmp(result->values[i].name, fname)==0)
			field_ind = i;
	}
	sprintf(tmp, "%d", field_ind);
	Push(tmp, LNUMBER);
#else
	Pop();
	Pop();
	Error("No Adabas support");
#endif
}

