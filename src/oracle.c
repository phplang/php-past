/***[oracle.c]****************************************************[TAB=4]****\
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
*                                                                            *
* Oracle functions for PHP.                                                  *
*                                                                            *
* © Copyright (C) Guardian Networks AS 1997, Dmitry Povarov 1997             *
*                                                                            *
* Authors: Stig Sæther Bakken <ssb@guardian.no>                              *
*          Mitch Golden <mgolden@interport.net>                              *
*          Dmitry "Dizzy" Povarov <dizzy@glas.net>                           *
*                                                                            *
\****************************************************************************/

/*
 *  $Id: oracle.c,v 1.18 1997/12/19 12:23:04 dizzy Exp $
 *
 * Possible enhancements:
 *
 * - use MAGIC_QUOTES
 * - PL/SQL procedure definition
 * - set rollback options
 * - LONG and LONG RAW support
 * - array/table support
 *
 * Known bugs:
 *
 * - columns such as AVG not returned as float
 *
 */

/* PHP stuff */
#include "php.h"
#include "parse.h"

#if HAVE_LIBOCIC

/* Oracle stuff */
#include <oratypes.h>
#include <ocidfn.h>
#ifdef __STDC__
# include <ociapr.h>
#else
# include <ocikpr.h>
#endif

#include "oracle.h"
#include <string.h>
#include <stdlib.h>

/* #define DEBUG */

/* NULL return values processing policy: 
   0 - return empty string regardless of var type 
   1 - return empty string for strings and 0 for numbers */

#define NULL_POLICY 0

/*
 * private functions
 */

static oraConnection *ora_add_conn(void);
static oraConnection *ora_get_conn(int);
static oraConnection *ora_find_conn(const char *userid, const char *password,
									const char *sid);
static void ora_del_conn(int);

static oraCursor *ora_add_cursor(void);
static oraCursor *ora_get_cursor(int);
static void ora_del_cursor(int);

static char *ora_error(Cda_Def *);
static int ora_describe_define(oraCursor *);
static int do_logoff(oraConnection *);

static oraConnection ora_conn_array[MAX_CONNECTIONS];
static oraCursor ora_cursor_array[MAX_CURSORS];

#endif

/*
** PHP functions
*/

void
Ora_Logon() /* userid, password */
{
#if HAVE_LIBOCIC
	char userid[ORAUIDLEN+1], password[ORAPWLEN+1], sid[ORASIDLEN+1];
	char retval[16];
	oraConnection *conn;
	char *envsid;
	Stack *s;
	
#ifdef TRACE
	Error("Ora_Logon");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Logon");
		Push("-1", LNUMBER);
		return;
	}
	strncpy(password, s->strval, ORAPWLEN);
	password[ORAPWLEN] = '\0';

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Logon");
		Push("-1", LNUMBER);
		return;
	}
	strncpy(userid, s->strval, ORAUIDLEN);
	userid[ORAUIDLEN] = '\0';

	envsid = getenv("ORACLE_SID");
	if (envsid != NULL) {
		strncpy(sid, envsid, ORASIDLEN);
		sid[ORASIDLEN] = '\0';
	} else {
		sid[0] = '\0';
	}

	conn = ora_find_conn(userid, password, sid);

#ifdef TRACE
	if (conn == NULL) {
		Error("Didn't recycle connection.");
	} else {
		Error("Recycled connection %d", conn->ind);
	}
#endif

	if (conn == NULL) {
		conn = ora_add_conn();

		if (conn == NULL) {
			Error("Too many open connections.");
			Push("-1", LNUMBER);
			return;
		}

		if (orlon(&conn->lda, conn->hda, userid, -1, password, -1, 0)) {
			Error("Unable to connect to ORACLE (%s)", ora_error(&conn->lda));
			ora_del_conn(conn->ind);
			Push("-1", LNUMBER);
			return;
		}

		strcpy(conn->userid, userid);
		strcpy(conn->password, password);
		strcpy(conn->sid, sid);
	}

	conn->inuse = 1;
	conn->waserror = 0;

	sprintf(retval, "%d", conn->ind);
	Push(retval, LNUMBER);

#else
	Pop();
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif

#ifdef TRACE
	Error("Ora_Logon returns");
#endif
}


void
Ora_Logoff() /* conn_index */
{
#if HAVE_LIBOCIC
	int conn_ind;
	oraConnection *conn;
	Stack *s;

#ifdef TRACE
	Error("Ora_Logoff");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Logoff");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) {
		conn_ind = s->intval;
	}
	else {
		conn_ind = 0;
	}	

	conn = ora_get_conn(conn_ind);

	if (conn == NULL) {
		Push("-1", LNUMBER);
		Error("No such connection id (%d)", conn_ind);
		return;
	}

	if (conn->inuse == 0) {
		Push("-1", LNUMBER);
		Error("Connection id %d not in use on this page.", conn_ind);
		return;
	}

	if (do_logoff(conn)) {
		Push("-1", LNUMBER);
		Error("Ora_Logoff had an error on connection %d.", conn_ind);
	}
	else {
		Push("0", LNUMBER);
	}

#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif

#ifdef TRACE
	Error("Ora_Logoff end");
#endif
}


void
Ora_Open() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraConnection *conn;
	oraCursor *cursor;
	int conn_ind;
	char retval[16];

#ifdef TRACE
	Error("Ora_Open");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Open");
		Push("-1", LNUMBER);
		return;
	}

	conn_ind = s->intval;
	conn = ora_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Invalid connection index %d", conn_ind);
		Push("-1", LNUMBER);
		return;
	}

	cursor = ora_add_cursor();

	if (cursor == NULL) {
		Error("Too many cursors on one page (max %d).", MAX_CURSORS);
		Push("-1", LNUMBER);
		return;
	}

    if (oopen(&cursor->cda, &conn->lda, (text *)0, -1, -1, (text *)0, -1)) {
		Error("Unable to open new cursor (%s)", ora_error(&cursor->cda));
		Push("-1", LNUMBER);
		ora_del_cursor(cursor->ind);
		return;
	}

	cursor->conn_ind = conn_ind;

	sprintf(retval, "%d", cursor->ind);
	Push(retval, LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif

#ifdef TRACE
	Error("Ora_Open end");
#endif
}


void
Ora_Close() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraCursor *cursor;
	int cursor_ind;

#ifdef TRACE
	Error("Ora_Close");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Close");
		Push("-1", LNUMBER);
		return;
	}

	cursor_ind = s->intval;

	cursor = ora_get_cursor(cursor_ind);

	if (cursor == NULL) {
		Error("Invalid cursor index %d", cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

	if (oclose(&cursor->cda)) {
		Error("Unable to close cursor (%s)", ora_error(&cursor->cda));
		Push("-1", LNUMBER);
		return;
	}

	ora_del_cursor(cursor->ind);
	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
#ifdef TRACE
	Error("Ora_Close end");
#endif
}


void
Ora_CommitOff() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraConnection *conn;
	int conn_ind;

#ifdef TRACE
	Error("Ora_CommitOff");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_CommitOff");
		return;
	}

	conn_ind = s->intval;
	conn = ora_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Invalid connection index %d", conn_ind);
		Push("-1", LNUMBER);
		return;
	}

	if (ocof(&conn->lda)) {
		Error("Unable to turn off auto-commit on connection %d (%s)",
			  conn_ind, ora_error(&conn->lda));
		Push("-1", LNUMBER);
		return;
	}

	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
#ifdef TRACE
	Error("Ora_CommitOff end");
#endif
}


void
Ora_CommitOn() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraConnection *conn;
	int conn_ind;

#ifdef TRACE
	Error("Ora_CommitOn");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_CommitOn");
		Push("-1", LNUMBER);
		return;
	}

	conn_ind = s->intval;
	conn = ora_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Invalid connection index %d", conn_ind);
		Push("-1", LNUMBER);
		return;
	}

	if (ocon(&conn->lda)) {
		Error("Unable to turn on auto-commit on connection %d (%s)",
			  conn_ind, ora_error(&conn->lda));
		Push("-1", LNUMBER);
		return;
	}

	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
#ifdef TRACE
	Error("Ora_CommitOn end");
#endif

}


void
Ora_Commit() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraConnection *conn;
	int conn_ind;

#ifdef TRACE
	Error("Ora_Commit");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Commit");
		Push("-1", LNUMBER);
		return;
	}

	conn_ind = s->intval;
	conn = ora_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Invalid connection index %d", conn_ind);
		Push("-1", LNUMBER);
		return;
	}

	if (ocom(&conn->lda)) {
		Error("Unable to commit transaction on connection %d (%s)",
			  conn_ind, ora_error(&conn->lda));
		Push("-1", LNUMBER);
		return;
	}

	conn->waserror = 0;

	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
#ifdef TRACE
	Error("Ora_Commit end");
#endif
}


void
Ora_Rollback() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraConnection *conn;
	int conn_ind;

#ifdef TRACE
	Error("Ora_Rollback");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Rollback");
		Push("-1", LNUMBER);
		return;
	}

	conn_ind = s->intval;
	conn = ora_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Invalid connection index %d", conn_ind);
		Push("-1", LNUMBER);
		return;
	}

	if (orol(&conn->lda)) {
		Error("Unable to roll back transaction (%s)", ora_error(&conn->lda));
		Push("-1", LNUMBER);
		return;
	}

	conn->waserror = 0;

	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
#ifdef TRACE
	Error("Ora_Rollback end");
#endif
}


void
Ora_Parse(int flag) /* cursor_index, sql_statement [, defer] */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraCursor *cursor;
	oraConnection *conn;
	int cursor_ind;
	sword defer = 0;
	text *query;

#ifdef TRACE
	Error("Ora_Parse");
#endif

	if (flag) {
		s = Pop();
		if (!s) {
			Error("Stack error in Ora_Parse");
			Push("-1", LNUMBER);
			return;
		}
		if (s->intval) {
			defer = DEFER_PARSE;
		}
	}

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Parse");
		Push("-1", LNUMBER);
		return;
	}
	query = (text *)estrdup(0, s->strval);
	ParseEscapes(query);

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Parse");
		Push("-1", LNUMBER);
		return;
	}
	cursor_ind = s->intval;

	if (query == NULL) {
		Error("Out of memory");
		Push("-1", LNUMBER);
		return;
	}

	cursor = ora_get_cursor(cursor_ind);
	if (cursor == NULL) {
		Error("Invalid cursor index %d", cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

	conn = ora_get_conn(cursor->conn_ind);
	if (conn == NULL) {
		Error("Internal error in Ora_Parse: no connection for cursor %d",
			  cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

	cursor->currentQuery = query;

	if (oparse(&cursor->cda, query, (sb4)-1, defer, VERSION_7)) {
		Error("Ora_Parse failed query: (%s)  error: (%s)",
			  query, ora_error(&cursor->cda));
		conn->waserror = 1;
		Push("-1", LNUMBER);
		return;
	}

        /* -Dz-: Clean cursor's binding area at each re-parsing */
        ora_no_bindings(cursor);

	conn->waserror = 0;
	
	Push("0", LNUMBER);
#else
	Pop();
	Pop();
	if (flag) {
		Pop();
	}
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
#ifdef TRACE
	Error("Ora_Parse end");
#endif
}

/* Returns the number of SELECT-list items for a select, number of
** affected rows for UPDATE/INSERT/DELETE, 0 for another successful
** statement or -1 on error.
*/
void
Ora_Exec() /* cursor_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraCursor *cursor;
	oraConnection *conn;
	int cursor_ind, ncol;
	ub2 sqlfunc;
	char retval[16];
	char *t;

#ifdef TRACE
	Error("Ora_Exec");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Exec");
		Push("-1", LNUMBER);
		return;
	}

	cursor_ind = s->intval;
	cursor = ora_get_cursor(cursor_ind);
	if (cursor == NULL) {
		Error("Invalid cursor index %d", cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

	conn = ora_get_conn(cursor->conn_ind);
	if (conn == NULL) {
		Error("Internal error in Ora_Exec: no connection for cursor %d",
			  cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

	sqlfunc = cursor->cda.ft;
	if (sqlfunc == FT_SELECT) {
		ncol = ora_describe_define(cursor);
		if (ncol < 0) {
#ifdef TRACE
			Error("Ora_Exec error return (describe_define)");
#endif
			Push("-1", LNUMBER);
			return;
		}
	} else {
		ncol = 0;
	}

	ora_bind_in(cursor); /* -Dz-: Get data from PHP vars */

	if (oexec(&cursor->cda)) {
		t = cursor->currentQuery;
		if (t == NULL) {
			t = "";
		}
		conn->waserror = 1;
		Error("Ora_Exec failed query: (%s) error: (%s)",
			  t, ora_error(&cursor->cda));
		Push("-1", LNUMBER);
		return;
	}

	ora_bind_out(cursor);
	conn->waserror = 0;

	sprintf(retval, "%d", ncol);
	Push(retval, LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
#ifdef TRACE
	Error("Ora_Exec end");
#endif
}


void
Ora_Fetch() /* cursor_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	int cursor_ind;
	oraCursor *cursor;
	sword err;

#ifdef TRACE
	Error("Ora_Fetch");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Fetch");
		Push("-1", LNUMBER);
		return;
	}

	/* Find the cursor */
	cursor_ind = s->intval;
	cursor = ora_get_cursor(cursor_ind);
	if (cursor == NULL) {
		Error("Invalid cursor index %d", cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

	/* Get data from Oracle */
	err = ofetch(&cursor->cda);
	if (err) {
		if (cursor->cda.rc == NO_DATA_FOUND) {
			Push("0", LNUMBER);
			return;
		}
		Error("Ora_Fetch failed (%s)", ora_error(&cursor->cda));
		Push("-1", LNUMBER);
		return;
	}

	cursor->curr_column = cursor->column_top;

	Push("1", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
#ifdef TRACE
	Error("Ora_Fetch end");
#endif
}


void
Ora_GetColumn() /* cursor_index, column_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	int cursor_ind, column_ind;
	oraCursor *cursor;
	oraColumn *column;
	sb2 type;

#ifdef TRACE
	Error("Ora_GetColumn");
#endif

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_GetColumn");
		Push("-1", LNUMBER);
		return;
	}
	column_ind = s->intval;

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_GetColumn");
		Push("-1", LNUMBER);
		return;
	}
	cursor_ind = s->intval;

	/* Find the cursor */
	cursor = ora_get_cursor(cursor_ind);
	if (cursor == NULL) {
		Error("Invalid cursor index %d", cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

	if (column_ind < 0 || column_ind >= cursor->ncols) {
		Error("Invalid column index %d (max %d)", column_ind, cursor->ncols);
		Push("-1", LNUMBER);
		return;
	}

	column = cursor->columns[column_ind];

	if (column == NULL) {
		Error("Empty column %d", cursor_ind);
		Push("-1", LNUMBER);
		return;
	}


#ifdef DEBUG
	Debug("Oracle: column(%d)->dbtype=%d\n", column_ind, column->dbtype);
#endif

	type = column->dbtype;

#ifdef DEBUG
	{
		char *t;
		if(column->buf!=NULL) t=column->buf;
		else t="NULL";
		Error("column->flt_buf '%f'  column->int_buf '%d'  column->buf '%s",
			  column->flt_buf, column->int_buf, t);
	}
#endif

    if (column->col_retcode != 0 && column->col_retcode != STRING_TRUNCATED) {
        /* Some error fetching column.  The most common is 1405, a NULL
		 * was retreived.  1406 is ASCII or string buffer data was
		 * truncated. The converted data from the database did not fit
		 * into the buffer.  Since we allocated the buffer to be large
		 * enough, this should not occur.  Anyway, we probably want to
		 * return what we did get, in that case
		 */
        Push("", STRING);
    }
	else if (type == FLOAT_TYPE) {
		char retval[65];

	        if (column->indp == -1) /* Check for NULL */
	            #if NULL_POLICY
	                column->flt_buf = 0; /* -Dz- */
	            #else
	                { Push("",STRING); return;}
	            #endif
	            
		sprintf(retval, "%64.32f", column->flt_buf);
#ifdef DEBUG
		Error("Ora_GetColumn returns float '%s'", retval);
#endif
		Push(retval, DNUMBER);
	}
	else if (type == INT_TYPE) {
		char retval[16];

	        if (column->indp == -1) /* Check for NULL */
	            #if NULL_POLICY
	                column->int_buf = 0; /* -Dz- */
	            #else
	                { Push("",STRING); return;}
	            #endif

		sprintf(retval, "%d", column->int_buf);
#ifdef DEBUG
		Error("Ora_GetColumn returns int '%s'", retval);
#endif
		Push(retval, LNUMBER);
	}
	else if (type == STRING_TYPE || type == VARCHAR2_TYPE) {
#ifdef DEBUG
		Error("Ora_GetColumn returns string '%s'", column->buf);
#endif
	        if (column->indp == -1)  column->buf = strdup(""); /* -Dz- */
		Push(AddSlashes(column->buf, 0), STRING);
	}
	else {
		Error("Ora_GetColumn found invalid type (%d) in column %d", type,
			  column_ind);
		Push("-1", LNUMBER);
		return;
	}
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
#ifdef TRACE
	Error("Ora_GetColumn end");
#endif
}


void
Ora_Bind(void) /* cursor_index, php_var_name, sql_var_name, var_len */
{
#if HAVE_LIBOCIC
	Stack        *s;
	oraCursor    *cursor;
	VarTree      *php_var;
	int          cursor_ind;
	char         *php_var_name;
	char         *sql_var_name;
	int          var_type;
	int          var_len;

	/* -Dz-: "Safe" binding technology: a new look :)
	 *
	 * 1) "Cursor" object contains a pointer to binding list
	 *
	 * 2) Each binding list entry conststs of the following fields:
	 *
	 *    a) PHP variable name
	 *    b) PL/SQL variable name (or placeholder number?)
	 *    c) Variable type
	 *    d) Variable length
	 *    e) Memory space which oracle will consider as C variable to bind
	 *
	 * 3) Ora_Bind adds entry to binding list and does binding itself
	 *
	 * 4) Ora_Exec makes the following steps:
	 *         
	 *    a) copies values from PHP variables to space in binding list entry
	 *    b) starts SQL query or PL/SQL block execution
	 *    c) copies values from bindig list entries to PHP variables
	 *
	 * 5) Note: Ora_Bind should be used after Ora_Parse but before Ora_Exec,
	 *    because Ora_Parse destroys binding list.
	 */

	/* Extract var_len from stack */

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Bind");
        Push("-1", LNUMBER);
		return;
	}
	var_len = s->intval;
      
	/* Extract sql_var_name from stack */

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Bind");
        Push("-1", LNUMBER);
		return;
	}
	if (s->strval) {
		sql_var_name = estrdup(0, s->strval);
	}

	/* Extract php_var_name from stack */

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Bind");
        Push("-1", LNUMBER);
		return;
	}   
	if (s->strval) {
		php_var_name = estrdup(0, s->strval);
	}
   
	/* Extract cursor_ind from stack */

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Bind");
        Push("-1", LNUMBER);
		return;
	}
	cursor_ind = s->intval;
   
	/* Obtain some required things from PHP & Oracle */

	cursor = ora_get_cursor(cursor_ind);
	if (cursor == NULL) {
        Error("Invalid cursor index %d", cursor_ind);
        Push("-1", LNUMBER);
        return;
	}

	php_var = GetVar(php_var_name, NULL, 0);
	if (php_var == NULL) {
        Error("Undeclared PHP variable %s", php_var_name);
        Push("-1", LNUMBER);
        return;
	}

	if ( ! ora_new_bind(cursor, php_var_name, sql_var_name, 
						VARCHAR2_TYPE, var_len)) {
        Push("-1", LNUMBER);
	}           

	Push("0", LNUMBER); /* Bind successfully */
#else
	Pop();
	Pop();
	Pop();
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
}



/* Called when page is done */
void
OraCloseAll(void)
{
#if HAVE_LIBOCIC
	int i;
	oraConnection *conn;
      
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		conn = ora_conn_array + i;
		if (conn->ind >= 0 && conn->inuse != 0) {
			do_logoff(ora_conn_array+i);
		}
	}
#endif
}

#if HAVE_LIBOCIC

/* Called at beginning of page. */
void
php_init_oracle(void)
{
	/* Keep track of whether this is the first time the module is being
	   initialized.  Remember, the server may be called upon to produce
	   more than one page. */
	static int runbefore = 0;
	int i;

	if (runbefore) {
		/* There should be nothing open from last time, but
		 * just in case there is,
		 * do again what we do when the module exits!
		 */
		OraCloseAll();
	} else {
		runbefore = 1;
		/* Mark all connections free */
		for (i = 0; i < MAX_CONNECTIONS; i++) {
			ora_del_conn(i);
		}

		/* Mark all cursors free */
		for(i=0; i<MAX_CURSORS; i++) {
			ora_del_cursor(i);
		}
	}
}


/*
** Functions internal to this module.
*/

static oraConnection *
ora_add_conn()
{
	int i;
      
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if(ora_conn_array[i].ind < 0) {
			ora_conn_array[i].ind = i;
			return ora_conn_array + i;
		}
	}
	return NULL;
}


static oraConnection *
ora_get_conn(int ind)
{
	if( ind >= 0 && ind < MAX_CONNECTIONS && ora_conn_array[ind].ind >= 0) {
        return(ora_conn_array + ind);
	} else {
        return NULL;
	}
}


/* find a connection with the right userid, password and sid that's not
   already being used */
static oraConnection *
ora_find_conn(const char *userid, const char *password,
			  const char *sid)
{
#ifndef ORA_NOT_PERSISTENT
	int i;
	oraConnection *conn;

	for (i = 0; i < MAX_CONNECTIONS; i++) {
		conn = ora_conn_array+i;
		if (strcmp(conn->userid, userid) == 0 &&
			strcmp(conn->password, password) == 0 &&
			strcmp(conn->sid, sid) == 0 &&
			conn->inuse == 0) {
			return(conn);
		}
	}
#endif
	return(NULL);
}


static void
ora_del_conn(int ind)
{
	if (ind >= 0 && ind < MAX_CONNECTIONS) {
		ora_conn_array[ind].ind = -1;
		ora_conn_array[ind].inuse = 0;
		ora_conn_array[ind].waserror = 0;
		ora_conn_array[ind].userid[0]   = '\0';
		ora_conn_array[ind].password[0] = '\0';
		ora_conn_array[ind].sid[0]      = '\0';
	}
}


static oraCursor *
ora_add_cursor()
{
	int i;
      
	for (i = 0; i < MAX_CONNECTIONS; i++) {
		if (ora_cursor_array[i].ind < 0) {
			ora_cursor_array[i].ind = i;
			return ora_cursor_array + i;
		}
	}
	return NULL;
}


static oraCursor *
ora_get_cursor(int ind)
{
	if (ind >= 0 && ind < MAX_CONNECTIONS && ora_cursor_array[ind].ind >= 0) {
        return ora_cursor_array + ind;
	} else {
		return(NULL);
	}
}


static void
ora_del_cursor(int ind) {
	if (ind >= 0 && ind < MAX_CURSORS) {
		/* if not NULL, really this stuff should be e-freed */
		ora_cursor_array[ind].currentQuery = NULL;
		ora_cursor_array[ind].column_top = NULL;
		ora_cursor_array[ind].curr_column = NULL;
		ora_cursor_array[ind].columns = NULL;
		ora_cursor_array[ind].ncols = 0;
		ora_cursor_array[ind].ind = -1;
		ora_cursor_array[ind].conn_ind = -1;
	}
}
  

static char *
ora_error(Cda_Def *cda)
{
    sword n, l;
    text *errmsg;

	errmsg = (text *)emalloc(2, 512);
    n = oerhms(cda, cda->rc, errmsg, 400);
	
	/* remove the last newline */
	l = strlen(errmsg);
	if (l < 400 && errmsg[l-1] == '\n') {
		errmsg[l-1] = '\0';
		l--;
	}

    if (cda->fc > 0) {
		strncat(errmsg, " -- while processing OCI function ", 100);
		strcat(errmsg, ora_func_tab[cda->fc]);
	}
	return (char *)errmsg;
}

	
static sword
ora_describe_define(oraCursor *cursor)
{
    sword col = 0, len;
	int columns_left = 1;
	int i;
	oraColumn *column, *last_column = NULL;
	oraColumn *tmpcol;
	ub1 *ptr;
	Cda_Def *cda = &cursor->cda;

	cursor->column_top = NULL;

	while (columns_left) {

		/* Allocate a column structure */
		column = (oraColumn *)emalloc(0, sizeof(oraColumn));
		if (column == NULL) {
			Error("ora_describe_define: Out of memory no. 1");
			return -1;
		}
		column->cbufl = ORANAMELEN;

		/* Fetch a select-list item description */
		if (odescr(cda, col + 1, &column->dbsize, &column->dbtype,
				   &column->cbuf[0], &column->cbufl, &column->dsize,
				   &column->prec, &column->scale, &column->nullok))
		{
            if (cda->rc == VAR_NOT_IN_LIST) {
				columns_left = 0;
#ifdef DEBUG
				Debug("Oracle: no columns left to describe\n");
#endif
                break;
			}
            else {
                Error(ora_error(cda));
                return -1;
            }
        }
#ifdef DEBUG
		Debug("Oracle: column %d name=%s type=%d scale=%d prec=%d\n",
			  col, column->cbuf, column->dbtype, column->scale, column->prec);
#endif

		/* Determine the data type and length */
		
        if (column->dbtype == NUMBER_TYPE) {
			column->dbsize = ORANUMWIDTH;
			if (column->scale != 0) {
				/* Handle NUMBER with scale as float. */
				ptr = (ub1 *) &column->flt_buf;
				len = (sword) sizeof(float);
				column->dbtype = FLOAT_TYPE;
			}
			else {
				ptr = (ub1 *) &column->int_buf;
				len = (sword) sizeof(sword);
				column->dbtype = INT_TYPE;
			}
		}
		else {
			if (column->dbtype == DATE_TYPE) {
				column->dbsize = 10;
			}
			else if (column->dbtype == ROWID_TYPE) {
				column->dbsize = 18;
			}
			if (column->dbsize > ORABUFLEN) {
				column->dbsize = ORABUFLEN;
			}
			len = column->dbsize + 1;
			ptr = column->buf = (ub1 *)emalloc(0, len);
			if (ptr == NULL) {
				Error("ora_describe_define: Out of memory no. 2");
				return -1;
			}
			column->dbtype = STRING_TYPE;
        }

#ifdef DEBUG
		Debug("Oracle: column->dbtype=%d\n", column->dbtype);
#endif
		/* Define an output variable for the select-list item */
        if (odefin(cda, col + 1, ptr, len, column->dbtype,
                   -1, &column->indp, (text *) 0, -1, -1,
                   &column->col_retlen, &column->col_retcode))
        {
            Error(ora_error(cda));
            return -1;
        }

		/* Chain this column into the cursor's column list */
		column->next = NULL;
		if (last_column == NULL) {
			cursor->column_top = column;
		}
		else {
			last_column->next = column;
		}
		last_column = column;

		col++;
    }

	if (col > 0) {
		/* Allocate an array of columns in the cursor for easy indexing. */
		cursor->columns = emalloc(0, sizeof(oraColumn *) * col);
		if (cursor->columns == NULL) {
			Error("ora_describe_define: Out of memory no. 3");
			return -1;
		}

		tmpcol = cursor->column_top;
		for (i = 0; i < col; i++) {
			cursor->columns[i] = tmpcol;
			tmpcol = tmpcol->next;
		}
	}

	cursor->ncols = col;
    return col;
}

static int
do_logoff(oraConnection *conn)
{
	int i, retval;
	int conn_ind;
        
	conn_ind = conn->ind;

#ifdef TRACE
	Error("do_logoff(%d)", conn_ind);
#endif

#ifndef ORA_NOT_PERSISTENT

	/* Persistent connections.  Close all cursors and commit work. */

	for (i = 0; i < MAX_CURSORS; i++) {
		/* Close all cursors */
		if (ora_cursor_array[i].conn_ind == conn_ind) {
#ifdef TRACE
			Error("Closing cursor %d", i);
#endif
			oclose(&(ora_cursor_array[i].cda));
			ora_del_cursor(i);
		}
	}

	if (conn->waserror == 0) {
#ifdef TRACE
		Error("Commit connection %d", conn->ind);
#endif
		if (ocom(&conn->lda)) {
			Error("Couldn't commit connection %d", conn_ind);
			if (ologof(&conn->lda)) {
				Error("Couldn't disconnect connection %d", conn_ind);
			}
			retval = -1;
		} else {
			retval = 0;
		}
	} else {
#ifdef TRACE
		Error("Rollback connection %d", conn->ind);
#endif
		if (orol(&conn->lda)) {
			Error("Couldn't rollback connection %d", conn_ind);
			if (ologof(&conn->lda)) {
				Error("Couldn't disconnect connection %d", conn_ind);
			}
			retval = -1;
		} else {
			retval = 0;
		}
	}

	conn->inuse = 0;

#else

	/* Not Persistent, just logoff */

	if (ologof(&conn->lda)) {
		Error("Couldn't disconnect connection %d", conn_ind);
		retval = -1;
	} else {
		retval = 0;
	}

	ora_del_conn(conn_ind);

#endif

#ifdef TRACE
	Error("do_logoff returns");
#endif
	return(retval);
}


/* -Dz-: Bind PHP variables to oracle SQL statements & PL/SQL blocks */

static void ora_no_bindings(oraCursor* cursor)
{
    oraBindVar* ob;
    
    /* -Dz-: Free bind variables itself and related list entries */

    while(cursor->ora_bind_top != NULL)
    {
        if (cursor->ora_bind_top->space != NULL)    /* Free fields */
            free(cursor->ora_bind_top->space); 
        if (cursor->ora_bind_top->sql_name != NULL)
            free(cursor->ora_bind_top->sql_name);
        if (cursor->ora_bind_top->php_name != NULL)
            free(cursor->ora_bind_top->php_name);

        ob = cursor->ora_bind_top;                  /* Free entry  */
        cursor->ora_bind_top = cursor->ora_bind_top->next;
        free(ob);
    }

}


/* Declare new binding descriptor */

static oraBindVar* ora_new_bind(oraCursor* cursor,
                                char* php_var_name,
                                char* sql_var_name,
                                int   var_type,
                                int   var_len)
{
    oraBindVar* new_bind;
    Cda_Def *cda = &cursor->cda;

    new_bind = malloc(sizeof(oraBindVar));
    if (new_bind != NULL)
    {
        new_bind->next = cursor->ora_bind_top;
        
        new_bind->type     = var_type;
        new_bind->len      = var_len+1;
        new_bind->sql_name = strdup(sql_var_name);
        new_bind->php_name = strdup(php_var_name);
        new_bind->space    = malloc(var_len+1);
        
        if (new_bind->space && new_bind->sql_name && new_bind->php_name) {
            cursor->ora_bind_top = new_bind;
        } else {
            /* Something was unsuccessfull */
            
            free(new_bind->sql_name);
            free(new_bind->php_name);
            free(new_bind->space);
            free(new_bind);
            new_bind = NULL;
            Error("Memory allocation error in Ora_Bind");
            return (new_bind);
        }
    } 

    if (obndra(cda,                        /* Cursor CDA area               */
              (text* )new_bind->sql_name,  /* PL/SQL variable               */
              -1,                          /* PL/SQL var name length        */
              (ub1* )new_bind->space,      /* Pointer to program variable   */
              (sword)new_bind->len,        /* Size of program variable      */
              new_bind->type,              /* External Oracle variable type */
              -1,
              (sb2*) 0,
              (ub2*) 0,
              (ub2*) 0,
              (ub4)  0,
              (ub4*) 0,
              (text*) 0, 
              -1,
              -1 ))
    {
            Error("Error binding PHP variable %s to PL/SQL variable %s",
                   new_bind->php_name, new_bind->sql_name);

            cursor->ora_bind_top = new_bind->next;
                   
            free(new_bind->sql_name);
            free(new_bind->php_name);
            free(new_bind->space);
            free(new_bind);
            new_bind = NULL;
    }

    return (new_bind);
}

/* Get binding descriptor for PHP variable */

static oraBindVar* ora_get_bind(oraCursor* cursor,
                                char* php_var_name)
{
    oraBindVar* ob;
    
    for (ob = cursor->ora_bind_top;
         ob != NULL;
         ob = ob->next)
    {
        if (!strcasecmp(ob->php_name, php_var_name))
            return (ob);
    }

    return (NULL);        
}


/* Erase binding descriptor */

static int ora_del_bind(void)
{
   /* -Dz-: Just now I'm not sure what it is required */
} 

/* Copy data from PHP variables to bind holders */
static int ora_bind_in(oraCursor* cursor)
{
    oraBindVar* ob;
    VarTree*    php_var;
    
    for (ob = cursor->ora_bind_top;
         ob != NULL;
         ob = ob->next)
    {

        php_var = GetVar(ob->php_name, NULL, 0);
        if (php_var == NULL)
        {
            Error("Undeclared PHP variable %s", ob->php_name);
            return;
        }

        /* Copy values from PHP variable to bind data structure */

	strncpy(ob->space, php_var->strval, ob->len);
	*((char*)ob->space + ob->len) = '\0';

    }
}

/* Copy data from bind holders to PHP */

static int ora_bind_out(oraCursor* cursor)
{
    oraBindVar* ob;
    VarTree*    php_var;
    
    for (ob = cursor->ora_bind_top;
         ob != NULL;
         ob = ob->next)
    {
        php_var = GetVar(ob->php_name, NULL, 0);
        if (php_var == NULL)
        {
            Error("Undeclared PHP variable %s", ob->php_name);
            return;
        }

        /* Copy values to PHP variable from bind data structure */

	*((char*)ob->space + ob->len) = '\0';
	php_var->strval = strdup(ob->space);       

    }
}

#endif

/*
 * Names:
 *
 * obndra    Ora_BindArray
 * obndrn    Ora_BindNumber
 * obndrv    Ora_BindName
 * obreak    Ora_Break
 * ocan      Ora_Cancel
 * oclose    Ora_Close
 * ocof      Ora_CommitOff
 * ocom      Ora_Commit
 * ocon      Ora_CommitOn
 * odefin    Ora_Define
 * odescr    Ora_Describe
 * odessp    Ora_DescribeProc
 * oerhms    Ora_Error
 * oexec     Ora_Exec
 * oexfet    Ora_ExecFetch
 * oexn      Ora_ExecArray
 * ofen      Ora_FetchArray
 * ofetch    Ora_Fetch
 * oflng     Ora_FetchLong
 * ologof    Ora_Logoff
 * oopoen    Ora_Open
 * oopt      Ora_Options
 * oparse    Ora_Parse
 * orlon     Ora_Logon
 * orol      Ora_Rollback
 *(sqlld2    Ora_SQL_Ld2)
 *(sqllda    Ora_SQL_Lda)
 * 
 * Will implement:
 * ===============
 * Ora_Bind
 * Ora_Break
 * Ora_Cancel
 * Ora_Close
 * Ora_CommitOff
 * Ora_CommitOn
 * Ora_Commit
 * Ora_Define
 * Ora_DescProc
 * Ora_Error
 * Ora_Exec
 * Ora_ExecFetch
 * Ora_Fetch
 * Ora_FetchLong
 * Ora_Logon
 * Ora_Logoff
 * Ora_Open
 * Ora_Options
 * Ora_Parse
 * Ora_Rollback
 * 
 */

/*
 * Local variables:
 * tab-width: 4
 * End:
 */
