/***[solid.c]*****************************************************[TAB=4]****\
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
* Solid Extensions                                                           *
*                                                                            *
* Copyright 1996 Jeroen van der Most (jvdmost@digiface.nl)                   *
* Based on pg95.c, written by Adam Sussman (asussman@vidya.com)              *
*                                                                            *
*  More information on Solid can be found on http://solid.digiface.nl/       *
*                                                                            *
*  There is no Solid code in this file.                                      *
*                                                                            *
\****************************************************************************/
 
#include "php.h"
#include <stdlib.h>
#ifdef HAVE_LIBSOLID
#include <cli0core.h>
#include <cli0ext1.h>
#endif
#include "parse.h"
#include <ctype.h>

#if APACHE
#ifndef DEBUG
#undef palloc
#endif
#endif

#ifdef HAVE_LIBSOLID
typedef struct {
	char name[32];
	char *value;
	long int vallen;
} ResultValue;

typedef struct SolidResult {
	HSTMT stmt;
	ResultValue *values;
	int numcols;
	int fetched;
} SolidResult;

typedef struct SolidResultList {
	SolidResult            *result;
	int                    index;
	struct SolidResultList *next;
	int conn_index;
} SolidResultList;

typedef struct SolidConnList {
	HDBC                 conn;
	int                  index;
	struct SolidConnList *next;
} SolidConnList;

static SolidResultList *solid_result_top=NULL;
static SolidConnList *solid_conn_top=NULL;
static int solid_ind=1;
static int solid_conn_ind=1;
static HENV henv;
#endif

void php_init_solid(void) {
#ifdef HAVE_LIBSOLID
	solid_result_top=NULL;
	solid_conn_top=NULL;
	solid_ind=2;
	solid_conn_ind=1;
	SQLAllocEnv(&henv);
#endif
}

/***************************************************
 *
 * List maintainers (SolidResultList and SolidConnList)
 *
 ***************************************************/

#ifdef HAVE_LIBSOLID
int solid_add_result(SolidResult *result, int conn_in) {
	SolidResultList *new;

	new = solid_result_top;
	if (!new) {
		new = emalloc(0,sizeof(SolidResultList));
		if (new == NULL) 
			Error("Out of memory");
		solid_result_top = new;
	} 
	else {
		while (new->next) 
			new=new->next;
		new->next = emalloc(0,sizeof(SolidResultList));
		if (new->next == NULL) {
			Error("Out of memory");
		}
		else {
			new = new->next;
		}
	}

	new->result = result;
	new->index  = solid_ind++;
	new->conn_index = conn_in;
	new->next   = NULL;
	return solid_ind-1;
}

SolidResult *solid_get_result(int count) {
	SolidResultList *new;

	if (count == 1) 
		return NULL;

	new = solid_result_top;
	while (new) {
		if (new->index == count) 
			return new->result;
		new = new->next;
	}
	return NULL;
}

void solid_del_result(int count) {
	SolidResultList *new, *prev, *next;

	if (count == 1) 
		return;

	prev = NULL;
	new = solid_result_top;
	while (new) {
		next = new->next;
		if (new->index == count) {
			SQLFreeStmt(new->result->stmt, SQL_DROP);
			if (prev) 
				prev->next = next;
			else 
				solid_result_top = next;
			break;
		}
		prev = new;
		new = next;
	}
}

int solid_add_conn(HDBC conn) {
	SolidConnList *new;

	new = solid_conn_top;
	if (!new) {
		new = emalloc(0,sizeof(SolidConnList));
		solid_conn_top = new;
	} 
	else {
		while (new->next) 
			new=new->next;
		new->next = emalloc(0,sizeof(SolidConnList));
		new = new->next;
	}

	new->conn  = conn;
	new->index = solid_conn_ind++;
	new->next  = NULL;
	return solid_conn_ind-1;
}

HDBC solid_get_conn(int count) {
	SolidConnList *new;

	new = solid_conn_top;
	while (new) {
		if (new->index == count) 
			return new->conn;
		new = new->next;
	}
	return NULL;
}

void solid_del_conn(HDBC conn) {
	SolidConnList *new, *prev, *next;

	prev = NULL;
	new = solid_conn_top;
	while (new) {
		next = new->next;
		if (new->conn == conn) {
			SQLDisconnect(new->conn);
			SQLFreeConnect(new->conn);
			if (prev) 
				prev->next = next;
			else 
				solid_conn_top = next;
			break;
		}
		prev = new;
		new = next;
	}
}
#endif /* HAVE_LIBSOLID */

/* PHP house keeping function */

void Solid_closeAll(void) {
#ifdef HAVE_LIBSOLID
	SolidResultList *lnew, *lnext;
	SolidConnList   *cnew, *cnext;

	lnew = solid_result_top; 
	while (lnew) {
		lnext = lnew->next;
		SQLFreeStmt(lnew->result->stmt, SQL_DROP);
		lnew = lnext;
	}
	solid_result_top = NULL;

	cnew = solid_conn_top;
	while (cnew) {
		cnext = cnew->next;
		if(cnew->conn) {
			SQLDisconnect(cnew->conn);
			SQLFreeConnect(cnew->conn);
		}
		cnew=cnext;
	}
	solid_conn_top = NULL;
#endif
}

/* Main User Functions */

void Solid_exec(void) {
#ifdef HAVE_LIBSOLID
	Stack       *s;
	int         conn;
	char        *query;
	int         i, j;
	char        temp[16];
	SolidResult *result=NULL;
	HDBC        curr_conn=NULL;
	short       resultcols;
	SWORD       colnamelen; /* Not used */
	SDWORD      displaysize;
	RETCODE     rc;
	char        state[6];     /* Not used */
	SDWORD      error;        /* Not used */
	char        errormsg[255];
	SWORD       errormsgsize; /* Not used */

	/* get args */
	s = Pop();
	if (!s) {
		Error("Stack error in Solid expression");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) 
		query = (char*) estrdup(1,s->strval);
	else {
		Error("No query string in Solid expression");
		Push("0", LNUMBER);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in Solid expression");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval)
		conn = s->intval;
	else
		conn = 0;

	curr_conn = solid_get_conn(conn);
	if (curr_conn == NULL) {
		Error("Bad Solid connection number");
		Push("0", LNUMBER);
		return;
	}

	StripDollarSlashes(query);
#if DEBUG
	Debug("Sending query: %s\n",query);
#endif
    result = (SolidResult *)emalloc(0, sizeof(SolidResult));
	if (result == NULL) {
		Error("Out of memory");
		Push("0", LNUMBER);
		return;
	}
	if (SQLAllocStmt(curr_conn, &(result->stmt))==-2)
		Error("SQLAllocStmt returned -2");
	if ((rc = SQLExecDirect(result->stmt, query, SQL_NTS)) != SQL_SUCCESS) {
		SQLError(henv, curr_conn, result->stmt, state, &error, errormsg, sizeof(errormsg)-1, &errormsgsize);
		Error("Bad response to Solid query %d %s (%s)", rc, state, errormsg);
		SQLFreeStmt(result->stmt, SQL_DROP);
		j = 0;
	}
	else {
		SQLNumResultCols(result->stmt, &resultcols);
		if (resultcols == 0)
			j = 1;
		else {
			result->numcols = resultcols;
			result->fetched = 0;
			result->values = (ResultValue *)emalloc(0, sizeof(ResultValue)*resultcols);
			for (i = 0; i < resultcols; i++) {
				SQLColAttributes(result->stmt, i+1, SQL_COLUMN_NAME,
					result->values[i].name, sizeof(result->values[i].name),
					&colnamelen, NULL);
				SQLColAttributes(result->stmt, i+1, SQL_COLUMN_DISPLAY_SIZE,
					NULL, 0, NULL, &displaysize);
				result->values[i].value = (char *)emalloc(0, displaysize + 1);
				if (result->values[i].value != NULL)
				/* Should we use SQL_C_BINARY here instead? */
				SQLBindCol(result->stmt, i+1, SQL_C_CHAR, 
					result->values[i].value, displaysize+1,
					&result->values[i].vallen);
				/* else, we try to do SQLGetData in
				   Solid_result() in case
				   this is a BLOB field - sopwith */
			}
		    j = solid_add_result(result,conn);
		}
	}

	sprintf(temp, "%d", j);
	Push(temp,LNUMBER);
#else
	Pop();
	Pop();
	Push("0", LNUMBER);
	Error("No Solid support");
#endif
}

void Solid_fetchRow(void) {
#ifdef HAVE_LIBSOLID
	Stack       *s;
	int         res_ind;
	SolidResult *result;
	RETCODE     rc;

	s = Pop();
	if (!s) {
		Error("Stack error in solid_fetchrow");
		Push("", STRING);
		return;
	}
	if (s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if (res_ind == 1) {
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}

	/* check result */
	result = solid_get_result(res_ind);
	if (result == NULL) {
		Error("Unable to find result index %s", res_ind);
		Push("", STRING);
		return;
	}

	rc = SQLFetch(result->stmt);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		Push("0", LNUMBER);
		return;
	}
	result->fetched++;
	Push("1", LNUMBER);
	return;

#else
	Pop();
	Push("", STRING);
	Error("No Solid support");
#endif
}

void Solid_result(void) {
#ifdef HAVE_LIBSOLID
	Stack       *s;
	char        *field;
	int         res_ind;
	int         field_ind;
	SolidResult *result;
	int         i;
	RETCODE     rc;

	field_ind = -1;
	field = NULL;

	s = Pop();
	if (!s) {
		Error("Stack error in solid_result");
		Push("", STRING);
		return;
	}
	if (s->strval)
		if (s->type == STRING)
			field = estrdup(1,s->strval);
		else
			field_ind = s->intval;
	else {
		Error("No field argument in solid_result");
		Push("", STRING);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in solid_result");
		Push("", STRING);
		return;
	}
	if (s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if (res_ind == 1) {
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}

	/* check result */
	result = solid_get_result(res_ind);
	if (result == NULL) {
		Error("Unable to find result index %d", res_ind);
		Push("", STRING);
		return;
	}

	/* get field index if the field parameter was a string */
	if (field != NULL) {
		for (i=0; i<result->numcols; i++) {
			if (strcasecmp(result->values[i].name, field)==0) {
				field_ind = i;
				break;
			}
		}
		if (field_ind < 0) {
			Error("Field %s not found", field);
			Push("", STRING);
			return;
		}
      } else {
      /* check for limits of field_ind if the field parameter was an int */
              if (field_ind >= result->numcols || field_ind <0) {
                      Error("Field index is larger than the number of fields");
                      Push("", STRING);
                      return;
              }
	}

    if (result->fetched==0) {
		/* User forgot to call solid_fetchrow(), let's do it here */
		rc = SQLFetch(result->stmt);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			Push("", STRING);
			return;
		}
		result->fetched++;
	}

	if (result->values[field_ind].vallen == SQL_NULL_DATA)
		Push("", STRING); /* FIXME: better value for null ? */
	else {
		if(result->values[field_ind].value != NULL)
			Push(AddSlashes(result->values[field_ind].value,0), STRING);
		else {
			/* All this ugly code here is the fault of
			   sopwith@redhat.com :) If there
			   is no value pointer set up where we do
			   the SQLBindCol, then we know that we
			   should at least try to get the size of the
			   field as we go - i.e. for BLOB fields
			   & etc. */
			char c, *realval = &c;
			SDWORD fieldsize = 12345;
			/* This first SQLGetData is used just to
			   find out the exact size of the field - 
			   that is why we only get one char
			   (it doesn't seem to want to get 0 chars) */
			if(SQLGetData(result->stmt, field_ind + 1, SQL_C_CHAR,
				realval, 1, &fieldsize) == SQL_ERROR) {
				char msgbuf[512];
				UCHAR szSqlState;
				SDWORD pfNativeError;
				SWORD foo;
				SQLError(henv, solid_conn_top->conn,
					result->stmt, &szSqlState,
					&pfNativeError, msgbuf, 511,
					&foo);
				Error("Initial SQLGetData failed");
				Error(msgbuf);
                       } else if(fieldsize > 0) {
				realval = emalloc(0, fieldsize + 2);
                               realval[0] = '\0';
				if(realval == NULL
					|| SQLGetData(result->stmt,
						field_ind + 1, SQL_C_CHAR,
                                               realval, fieldsize + 1,
						&fieldsize) == SQL_ERROR) {
					Error("Out of memory");
					Push("<Out of memory>", STRING);
				} else
					Push(AddSlashes(realval, 0), STRING);
                       } else
                               Push("", STRING);
		}
	}
	return;

#else
	Pop();
	Pop();
	Push("", STRING);
	Error("No Solid support");
#endif
}

void Solid_freeResult(void) {
#ifdef HAVE_LIBSOLID
	Stack        *s;

	s = Pop();
	if (!s) {
		Error("Stack error in solid_freeresult");
		return;
	}
	if (s->strval) {
		solid_del_result(s->intval);
	} 
	else {
		Error("Invalid result index in solid_freeresult");
		return;
	}
#else
	Pop();
	Error("No Solid support");
#endif
}

void Solid_connect(void) {
#ifdef HAVE_LIBSOLID
	Stack   *s;
	HDBC    new_conn;
	char    *db=NULL;
	char    *uid=NULL;
	char    *pwd=NULL;
	char    *temp;
	int     j;
	RETCODE rc;
	char    state[6];
	SDWORD  error;
	char    errormsg[255];
	SWORD   errormsgsize;

	s = Pop();
	if (!s) {
		Error("Stack error in solid_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval)
		pwd = estrdup(1,s->strval);
	else {
		Error("No password supplied in solid_connect");
		Push("0", LNUMBER);
		return;
	}
	
	s = Pop();
	if (!s) {
		Error("Stack error in solid_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) 
		uid = estrdup(1,s->strval);
	else {
		Error("No user name supplied in solid_connect");
		Push("0", LNUMBER);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in solid_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) 
		db = estrdup(1,s->strval);
	else {
		Error("No database name supplied in solid_connect");
		Push("0", LNUMBER);
		return;
	}
	
	SQLAllocConnect(henv, &new_conn);
	rc = SQLConnect(new_conn, db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		SQLError(henv, new_conn, SQL_NULL_HSTMT, state, &error, errormsg, sizeof(errormsg)-1, &errormsgsize);
		Error("Could not connect to database (%s)", errormsg);
		temp = (char*) emalloc(1,2);
		sprintf(temp, "0");
	}
	else {
		j = solid_add_conn(new_conn);
		temp = (char*) emalloc(1,11);
		sprintf(temp, "%d", j);
	}
	Push(temp, LNUMBER);
#else
	Pop();
	Pop();
	Pop();
	Error("No Solid support");
#endif
}

void Solid_close(void) {
#ifdef HAVE_LIBSOLID
	Stack *s;
	int   conn_ind;
	HDBC  conn;
	SolidResultList *lnew, *lnext;

	s = Pop();
	if (!s) {
		Error("Stack error in solid_close");
		return;
	}
	if (s->strval) 
		conn_ind = s->intval;
	else 
		conn_ind = 0;

	conn = solid_get_conn(conn_ind);
	if(conn) {
		/* This next bit of code is needed to make sure that we delete any
		 * result pointers associated with the connection we are about to
		 * close.  If we don't do this, the Solid client library will SEGV
		 * on us if we later let SolidCloseAll() attempt to free these
		 * result pointers by calling SQLFreeStmt().  I still think this
		 * is a problem that should be fixed on the Solid side, but this should
		 * suffice for now.
		 */
		lnew = solid_result_top; 
		while(lnew) {
			lnext = lnew->next;
			if(lnew->conn_index == conn_ind) solid_del_result(lnew->index);
			lnew = lnext;
		}
		solid_del_conn(conn);
	}
#else
	Pop();
	Error("No Solid support");
#endif
}

void Solid_numRows(void) {
#ifdef HAVE_LIBSOLID
	Stack       *s;
	int         res_ind;
	SolidResult *result;
	SDWORD      rows;
	char        temp[16];

	s = Pop();
	if (!s) {
		Error("Stack error in solid_numrows");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) 
		res_ind = s->intval;
	else
		res_ind = 0;

	if (res_ind == 1) {
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = solid_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in solid_numrows");
		Push("-1", LNUMBER);
		return;
	}

    /*SQLNumResultCols(result->stmt, &cols);*/
	SQLRowCount(result->stmt, &rows);
	sprintf(temp, "%ld", rows);
	Push(temp, LNUMBER);
#else
	Pop();
	Error("No Solid support");
#endif
}


void Solid_numFields(void) {
#ifdef HAVE_LIBSOLID
	Stack       *s;
	int         res_ind;
	SolidResult *result;
	char        temp[16];

	s = Pop();
	if (!s) {
		Error("Stack error in solid_numfields");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if (res_ind == 1) {
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = solid_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in solid_numfields");
		Push("-1", LNUMBER);
		return;
	}

	sprintf(temp, "%d", result->numcols);
	Push(temp, LNUMBER);
#else
	Pop();
	Error("No Solid support");
#endif
}

void Solid_fieldName(void) {
#ifdef HAVE_LIBSOLID
	Stack       *s;
	int         res_ind;
	int         field_ind;
	SolidResult *result;
	char        *tmp;

	s = Pop();
	if (!s) {
		Error("Stack error in solid_fieldname");
		Push("", STRING);
		return;
	}
	if (s->strval)
		field_ind = s->intval;
	else {
		Error("No field index given in solid_fieldname");
		Push("", STRING);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in solid_fieldname");
		Push("", STRING);
		return;
	}
	if (s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if (res_ind == 1) {
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}
	result = solid_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in solid_fieldname");
		Push("", STRING);
		return;
	}

	if (field_ind >= result->numcols) {
		Error("Field index larger than number of fields");
		Push("", STRING);
		return;
	}

	tmp = estrdup(1,result->values[field_ind].name);
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	Push(tmp, STRING);
#else
	Pop();
	Pop();
	Error("No Solid support");
#endif
}

void Solid_fieldNum(void) {
#ifdef HAVE_LIBSOLID
	Stack       *s;
	int         res_ind;
	int         field_ind;
	char        *fname;
	SolidResult *result;
	char        tmp[16];
	int         i;

	s = Pop();
	if (!s) {
		Error("Stack error in solid_fieldnum");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) 
		fname = estrdup(1,s->strval);
	else {
		Error("No field name given in solid_fieldnum");
		Push("-1", LNUMBER);
		return;
	}
	
	s = Pop();
	if (!s) {
		Error("Stack error in solid_fieldnum");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) 
		res_ind = s->intval;
	else 
		res_ind = 0;

	if (res_ind == 1) {
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = solid_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in solid_fieldnum");
		Push("-1", LNUMBER);
		return;
	}

	field_ind = -1;
	for (i=0; i<result->numcols; i++) {
		if (strcasecmp(result->values[i].name, fname)==0)
			field_ind = i;
	}
	sprintf(tmp, "%d", field_ind);
	Push(tmp, LNUMBER);
#else
	Pop();
	Pop();
	Error("No Solid support");
#endif
}


