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
* © Copyright (C) Guardian Networks AS 1997                                  *
* Authors: Stig Sæther Bakken <ssb@guardian.no>                              *
*                                                                            *
*                                                                            *
\****************************************************************************/

/*
 *  $Id: oracle.c,v 1.6 1997/06/12 06:20:35 cvswrite Exp $
 *
 * TODO:
 * - use MAGIC_QUOTES
 * - bind variables
 * - configure support
 * - PL/SQL procedure definition
 * - set rollback options
 * - LONG and LONG RAW support
 * - array/table support
 * - ocan
 * - choose database
 *
 *
 * My Makefile mods needed:
 *
 * ORALIBDIR=/app/home/dba/oracle/product/7.1.4/lib
 *
 * include -DHAVE_LIBOCIC=1 in CPPFLAGS
 *
 * include -L$(ORALIBDIR) in LDFLAGS
 *
 * include -lora -locic -lcv6 -lnlsrtl -lcore -lsqlnet $(ORALIBDIR)/osntab.o
 * in LIBS or STATICLIBS
 *
 * In addition you need these header files:
 *
 *   ocidfn.h
 *   oratypes.h
 *   ocikpr.h
 *   ociapr.h
 *
 */

#if HAVE_LIBOCIC
/* Oracle stuff */
# include <oratypes.h>
# include <ocidfn.h>
# ifdef __STDC__
#  include <ociapr.h>
# else
#  include <ocikpr.h>
# endif
#endif

/* PHP stuff */
# include "oracle.h"
# include "php.h"
# include "parse.h"

#if HAVE_LIBOCIC

static oraConnection *ora_conn_top = (void *)NULL;
static oraCursor *ora_cursor_top = (void *)NULL;

static int ora_conn_index = 0;
static int ora_cursor_index = 0;
static int ora_numwidth = 38;

#endif

/*
** PHP functions
*/

void
Ora_Logon() /* userid, password */
{
#if HAVE_LIBOCIC
	char *userid, *password;
	char retval[16];
	oraConnection *conn;
	Stack *s;
	
	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Logon");
		Push("0", LNUMBER);
		return;
	}
	password = (char *)estrdup(1, s->strval);

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Logon");
		Push("0", LNUMBER);
		return;
	}
	userid = s->strval;

	conn = ora_add_conn();

	

	if (orlon(&conn->lda, conn->hda, userid, -1, password, -1, 0)) {
		Error("Unable to connect to ORACLE (%s)", ora_error(&conn->lda));
		Push("-1", LNUMBER);
		ora_del_conn(conn->ind);
	}
	else {
		sprintf(retval, "%d", conn->ind);
		Push(retval, LNUMBER);
	}
#else
	Pop();
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
}


void
Ora_Logoff() /* conn_index */
{
#if HAVE_LIBOCIC
	int index;
	oraConnection *conn;
	Stack *s;

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Logoff");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) {
		index = s->intval;
	}
	else {
		index = 0;
	}	

	conn = ora_get_conn(index);

	if (conn == NULL) {
		Error("No such connection id (%d)", index);
		return;
	}

	ologof(&conn->lda);

	ora_del_conn(index);

	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
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

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Open");
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

    if (oopen(&cursor->cda, &conn->lda, (text *)0, -1, -1, (text *)0, -1)) {
		Error("Unable to open new cursor (%s)", ora_error(&cursor->cda));
		Push("-1", LNUMBER);
		ora_del_cursor(cursor->ind);
		return;
	}

	sprintf(retval, "%d", cursor->ind);
	Push(retval, LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
}


void
Ora_Close() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraCursor *cursor;
	int cursor_ind;

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Close");
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
}


void
Ora_CommitOff() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraConnection *conn;
	int conn_ind;

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
		Error("Unable to turn off auto-commit (%s)", ora_error(&conn->lda));
		Push("-1", LNUMBER);
		return;
	}

	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
}


void
Ora_CommitOn() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraConnection *conn;
	int conn_ind;

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_CommitOn");
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
		Error("Unable to turn on auto-commit (%s)", ora_error(&conn->lda));
		Push("-1", LNUMBER);
		return;
	}

	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
}


void
Ora_Commit() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraConnection *conn;
	int conn_ind;

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Commit");
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
		Error("Unable to commit transaction (%s)", ora_error(&conn->lda));
		Push("-1", LNUMBER);
		return;
	}

	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
}


void
Ora_Rollback() /* conn_index */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraConnection *conn;
	int conn_ind;

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Rollback");
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

	Push("0", LNUMBER);
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
#endif
}


void
Ora_Parse(int flag) /* cursor_index, sql_statement [, defer] */
{
#if HAVE_LIBOCIC
	Stack *s;
	oraCursor *cursor;
	int cursor_ind;
	sword defer = 0;
	text *query;

	if (flag) {
		s = Pop();
		if (!s) {
			Error("Stack error in Ora_Parse");
			return;
		}
		if (s->intval) {
			defer = DEFER_PARSE;
		}
	}

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Parse");
		return;
	}
	query = (text *)estrdup(0, s->strval);

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Parse");
		return;
	}
	cursor_ind = s->intval;

	if (query == NULL) {
		Error("Invalid query");
		Push("-1", LNUMBER);
		return;
	}

	cursor = ora_get_cursor(cursor_ind);
	if (cursor == NULL) {
		Error("Invalid cursor index %d", cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

	cursor->currentQuery = query;

	if (oparse(&cursor->cda, query, (sb4)-1, defer, VERSION_7)) {
		Error("Ora_Parse failed (%s)", ora_error(&cursor->cda));
		Push("-1", LNUMBER);
		return;
	}

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
	int cursor_ind, ncol;
	ub2 sqlfunc;

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Exec");
		return;
	}

	cursor_ind = s->intval;
	cursor = ora_get_cursor(cursor_ind);
	if (cursor == NULL) {
		Error("Invalid cursor index %d", cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

	ncol = ora_describe_define(cursor);
	if (ncol < 0) {
		Push("-1", LNUMBER);
		return;
	}

	if (oexec(&cursor->cda)) {
		Error("Ora_Exec failed (%s)", ora_error(&cursor->cda));
		Push("-1", LNUMBER);
		return;
	}

	sqlfunc = cursor->cda.ft;
	if (sqlfunc == FT_INSERT || sqlfunc == FT_SELECT ||
		sqlfunc == FT_UPDATE || sqlfunc == FT_DELETE)
	{
		char retval[16];
		sprintf(retval, "%d", ncol);
		Push(retval, LNUMBER);
	}
	else {
		Push("0", LNUMBER);
	}
#else
	Pop();
	Error("no Oracle support");
	Push("-1", LNUMBER);
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

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Fetch");
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

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Fetch");
		return;
	}
	column_ind = s->intval;

	s = Pop();
	if (!s) {
		Error("Stack error in Ora_Fetch");
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

	column = cursor->columns[column_ind];

	if (column_ind < 0 || column_ind >= cursor->ncols) {
		Error("Invalid column index %d (max %d)", cursor_ind, cursor->ncols);
		Push("-1", LNUMBER);
		return;
	}

	if (column == NULL) {
		Error("Empty column %d", cursor_ind);
		Push("-1", LNUMBER);
		return;
	}

#ifdef DEBUG
	Debug("Oracle: column(%d)->dbtype=%d\n", column_ind, column->dbtype);
#endif
	type = column->dbtype;

	if (type == FLOAT_TYPE) {
		char retval[65];
		sprintf(retval, "%31.32f", column->flt_buf);
		Push(retval, DNUMBER);
	}
	else if (type == INT_TYPE) {
		char retval[16];
		sprintf(retval, "%d", column->int_buf);
		Push(retval, LNUMBER);
	}
	else if (type == STRING_TYPE || type == VARCHAR2_TYPE) {
		Push(column->buf, STRING);
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
}

#if HAVE_LIBOCIC

void
php_init_oracle(void)
{
	ora_closeall();

	ora_conn_index = 0;
	ora_cursor_index = 0;

	ora_conn_top = NULL;
	ora_cursor_top = NULL;
}

/*
** Functions internal to this module.
*/

static oraConnection *
ora_add_conn()
{
	oraConnection *new, *conn;

	if (ora_conn_top == NULL) {
		new = emalloc(0, sizeof(oraConnection));
		if (!new) {
			Error("Out of memory");
			return NULL;
		}
		ora_conn_top = new;
		new->prev = NULL;
	}
	else {
		conn = ora_conn_top;
		while (conn->next) {
			conn = conn->next;
		}
		new = emalloc(0, sizeof(oraConnection));
		if (!new) {
			Error("Out of memory");
			return NULL;
		}
		new->prev = conn;
		conn->next = new;
	}

	new->ind = ++ora_conn_index;
	new->next = NULL;

	return new;
}

static oraConnection *
ora_get_conn(int ind)
{
	oraConnection *conn;

	conn = ora_conn_top;

	while (conn) {
		if (conn->ind == ind) {
			return conn;
		}
		conn = conn->next;
	}

	return NULL;
}

static void
ora_del_conn(int ind) {
	oraConnection *conn, *before, *after;

	conn = ora_get_conn(ind);

	if (conn == NULL) {
		return;
	}

	before = conn->prev;
	after = conn->next;

	if (before == NULL) {
		if (after == NULL) {
			ora_conn_top = NULL;
		}
		else {
			ora_conn_top = after;
			ora_conn_top->prev = NULL;
		}
	}
	else {
		before->next = after;
		after->prev = before;
	}
}

static oraCursor *
ora_add_cursor()
{
	oraCursor *new, *cursor;

	if (ora_cursor_top == NULL) {
		/* ...there are no cursors allocated already */
		new = emalloc(0, sizeof(oraCursor));
		if (!new) {
			Error("Out of memory");
			return NULL;
		}
		ora_cursor_top = new;
		new->prev = NULL;
	}
	else {
		cursor = ora_cursor_top;
		while (cursor && cursor->next) {
			cursor = cursor->next;
		}
		new = emalloc(0, sizeof(oraCursor));
		if (new == NULL) {
			Error("Out of memory");
			return NULL;
		}
		new->prev = cursor;
		cursor->next = new;
	}

	new->ind = ++ora_cursor_index;
	new->next  = NULL;
	new->currentQuery = NULL;

	return new;
}

static oraCursor *
ora_get_cursor(int ind)
{
	oraCursor *cursor;

	cursor = ora_cursor_top;

	while (cursor) {
		if (cursor->ind == ind) {
			return cursor;
		}
		cursor = cursor->next;
	}

	return NULL;
}

static void
ora_del_cursor(int ind) {
	oraCursor *cursor, *before, *after;

	cursor = ora_get_cursor(ind);

	if (cursor == NULL) {
		return;
	}

	before = cursor->prev;
	after = cursor->next;

	if (before == NULL) {
		if (after == NULL) {
			ora_cursor_top = NULL;
		}
		else {
			ora_cursor_top = after;
			ora_cursor_top->prev = NULL;
		}
	}
	else {
		before->next = after;
		after->prev = before;
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
	oraColumn *column, *last_column = NULL;
	ub1 *ptr;
	Cda_Def *cda = &cursor->cda;

	cursor->column_top = NULL;

	while (columns_left) {

		/* Allocate a column structure */
		column = (oraColumn *)emalloc(0, sizeof(oraColumn));
		if (column == NULL) {
			Error("Out of memory");
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
		Debug("Oracle: column %d name=%s type=%d\n", col, column->cbuf,
			  column->dbtype);
#endif

		/* Determine the data type and length */
        if (column->dbtype == NUMBER_TYPE) {
			column->dbsize = ora_numwidth;
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
				column->dbsize = 9;
			}
			else if (column->dbtype == ROWID_TYPE) {
				column->dbsize = 18;
			}
			if (column->dbsize > ORABUFLEN) {
				column->dbsize = ORABUFLEN;
			}
			len = column->dbsize > ORABUFLEN ? ORABUFLEN : column->dbsize+1;
			ptr = column->buf = (ub1 *)emalloc(0, len);
			if (ptr == NULL) {
				Error("Out of memory");
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

	/* Allocate an array of columns in the cursor for easy indexing. */
	cursor->columns = emalloc(0, sizeof(oraColumn *) * col);
	if (cursor->columns == NULL) {
		Error("Out of memory");
		return -1;
	}
	else {
		int i;
		oraColumn *tmpcol = cursor->column_top;
		for (i = 0; i < col; i++) {
			cursor->columns[i] = tmpcol;
			tmpcol = tmpcol->next;
		}
	}

	cursor->ncols = col;
    return col;
}

static void
ora_closeall()
{
	oraCursor *cursor, *next_cursor;
	oraConnection *conn, *next_conn;

	cursor = ora_cursor_top;
	conn = ora_conn_top;

	while (cursor) {
		next_cursor = cursor->next;
		ora_free_cursor(cursor);
		cursor = next_cursor;
	}

	while (conn) {
		next_conn = conn->next;
		ora_free_conn(conn);
		conn = next_conn;
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
