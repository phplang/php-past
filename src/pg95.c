/***[pg95.c]******************************************************[TAB=4]****\
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
* Postgres95 Extensions                                                      *
*                                                                            *
* Written by Adam Sussman (asussman@vidya.com)                               *
*                                                                            *
*  More information about Postgres95 can be found at this URL:               *
*                                                                            *
*  http://epoch.cs.berkeley.edu:8000/postgres95/                             *
*                                                                            *
*  The Postgres95 database system is Copyright (c) 1994-6 Regents of the     *
*  University of California.                                                 *
*                                                                            *
*  There is no Postgres95 code or any other Unviersity of California         *
*  owned code in this file.                                                  *
*                                                                            *
\****************************************************************************/
/* Patches marked 'john:' by John Robinson <john@intelligent.co.uk> */

/* $Id: pg95.c,v 1.21 1997/06/12 18:59:40 rasmus Exp $ */

#include "php.h"
#include <stdlib.h>
#ifdef HAVE_LIBPQ
#include <libpq-fe.h>
#endif
#include "parse.h"
#include <ctype.h>

#if APACHE
#ifndef DEBUG
#undef palloc
#endif
#endif

#ifdef HAVE_LIBPQ
typedef struct pgResultList {
	PGresult	*result;
	int		index;
	struct		pgResultList *next;
} pgResultList;

typedef struct pgConnList {
	PGconn		*conn;
	int		index;
	struct		pgConnList *next;
} pgConnList;

static pgResultList	*pg_result_top=NULL;
static pgConnList	*pg_conn_top=NULL;
static int		pgTypeRes=0;
static int		pgLastOid=-1;
static int	pg95_ind=2;
static int	pg95_conn_ind=1;
#endif

void php_init_pg95(void) {
#ifdef HAVE_LIBPQ
	pg_result_top=NULL;
	pg_conn_top=NULL;
	pgTypeRes=0;
	pgLastOid=-1;
	pg95_ind=2;
	pg95_conn_ind=1;
#endif
}

/***************************************************
 *
 * List maintainers (pgResultList and pgConnList)
 *
 ***************************************************/

/* pgResultList */

#ifdef HAVE_LIBPQ
int pg_add_result(PGresult *result)
{
	/* result index 1 (pg95_ind) is reserved to indicate successfull
	 * non-tuple returning commands
	 */

	pgResultList	*new;

	new = pg_result_top;
	if (!new) {
		new = emalloc(0,sizeof(pgResultList));
		if (new == NULL) Error("Out of memory");
		pg_result_top = new;
	} else {
		while (new->next) new=new->next;
		new->next = emalloc(0,sizeof(pgResultList));
		if (new->next == NULL) {
			Error("Out of memory");
		} else {
			new = new->next;
		}
	}

	new->result	= result;
	new->index	= pg95_ind++;
	new->next	= NULL;
	return (pg95_ind-1);
}

PGresult *pg_get_result(int count)
{
	pgResultList	*new;

	if (count == 1) return NULL;

	new = pg_result_top;
	while (new) {
		if (new->index == count) return (new->result);
		new = new->next;
	}
	return (NULL);
}

void pg_del_result(int count)
{
	pgResultList	*new, *prev, *next;

	if (count == 1) return;
	prev = NULL;
	new = pg_result_top;
	while (new) {
		next = new->next;
		if (new->index == count) {
			PQclear(new->result);
			if (prev) prev->next = next;
			else pg_result_top = next;
			break;
		}
		prev = new;
		new = next;
	}
}

/* pgConnList */

int pg_add_conn(PGconn *conn)
{
	pgConnList	*new;

	new = pg_conn_top;
	if (!new) {
		new = emalloc(0,sizeof(pgConnList));
		if (new == NULL) Error("Out of memory");
		pg_conn_top = new;
	} else {
		while (new->next) new=new->next;
		new->next = emalloc(0,sizeof(pgConnList));
		if (new->next == NULL) {
			Error("Out of memory");
		} else {
			new = new->next;
		}
	}

	new->conn	= conn;
	new->index	= pg95_conn_ind++;
	new->next	= NULL;
	return (pg95_conn_ind-1);
}

PGconn *pg_get_conn(int count)
{
	pgConnList	*new;

	new = pg_conn_top;
	while (new) {
		if (new->index == count) return (new->conn);
		new = new->next;
	}
	return (NULL);
}

void pg_del_conn(PGconn* conn)
{
	pgConnList	*new, *prev, *next;

	prev = NULL;
	new = pg_conn_top;
	while (new) {
		next = new->next;
		if (new->conn == conn) {
			PQfinish(new->conn);
			if (prev) prev->next = next;
			else pg_conn_top = next;
			break;
		}
		prev = new;
		new = next;
	}
}

/* type lookup
 *
 * toid=-2 is a special case lookup where only
 *	 cacheing is performed
 *
 */
char* pg_type(PGconn* conn, int toid)
{
	PGresult	*result;
	int		i, num_tups;
	char		*field;
	char		*oid;
	char		ref_oid[16];
	int		oid_num, nam_num;


	/* we will cache the results of the typelist query the
	   first time we ask so we don't accrue overhead later */

	result = pg_get_result(pgTypeRes);
	if (result == NULL) {

		if (PQstatus(conn) == CONNECTION_BAD)
		{
			/* try to reset the connection */
			PQreset(conn);

			if (PQstatus(conn) == CONNECTION_BAD) {
				Error("Postgres95 database connection error (%s). Shutting down connection", PQerrorMessage(conn));
				pg_del_conn(conn);
				return NULL;
			}
		}

		result = PQexec(conn, "select oid,typname from pg_type");
		
		if ((result == NULL) || (PQresultStatus(result) != PGRES_TUPLES_OK)) {
			Error("Error getting postgres95 type list");
			return NULL;
		}

		pgTypeRes = pg_add_result(result);
	} 

	/* Request was for cache only */
	if (toid == -2) return NULL;

	/* lookup type */
	num_tups = PQntuples(result);
	oid_num = PQfnumber(result, "oid");
	nam_num = PQfnumber(result, "typname");

	sprintf(ref_oid, "%d", toid);

	for (i = 0; i < num_tups; i++)
	{
		oid = PQgetvalue(result, i, oid_num);
		field = PQgetvalue(result, i, nam_num);

		if ((oid == NULL) || (field == NULL)) return NULL;

		if (!strcmp(oid, ref_oid)) {
			return field;
		}
	}

	return NULL;
}
#endif

/* PHP house keeping function */

void PGcloseAll(void) {
#ifdef HAVE_LIBPQ
	pgResultList	*lnew, *lnext;
	pgConnList	*cnew, *cnext;

	lnew = pg_result_top; 
	while (lnew) {
		lnext = lnew->next;
		PQclear(lnew->result);
		lnew = lnext;
	}
	pg_result_top = NULL;

	cnew = pg_conn_top;
	while (cnew) {
		cnext = cnew->next;
		PQfinish(cnew->conn);
		cnew=cnext;
	}
	pg_conn_top = NULL;
#endif
}

/* Main User Functions */

void PGexec(void) {
#ifdef HAVE_LIBPQ
	Stack	*s;
	int	conn;
	ExecStatusType	stat;
	char	*query;
	int	j;
	char	temp[16];
	const char *tmpoid;
	PGresult *result=NULL;
	PGconn	 *curr_conn=NULL;

	/* get args */
	s = Pop();
	if (!s) {
		Error("Stack error in postgres95 expression");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) query = (char*) estrdup(1,s->strval);
	else {
		Error("No query string in postgres95 expression");
		Push("0", LNUMBER);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in postgres95 expression");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) {
		conn = s->intval;
	} else {
		conn = 0;
	}

	/* check db connection */
	curr_conn = pg_get_conn(conn);
	if (curr_conn == NULL) {
		Error("Bad postgres95 connection number");
		Push("0", LNUMBER);
		return;
	}

	if (PQstatus(curr_conn) == CONNECTION_BAD)
	{
		/* try to reset the connection */
		PQreset(curr_conn);

		if (PQstatus(curr_conn) == CONNECTION_BAD) {
			Error("Postgres95 database connection error (%s)", PQerrorMessage(curr_conn));
			pg_del_conn(curr_conn);
			Push("0", LNUMBER);
			return;
		}
	}
	StripDollarSlashes(query);  /* Postgres95 doesn't recognize \$ */
#if DEBUG
	Debug("Sending query: %s\n",query);
#endif
	result = PQexec(curr_conn, query);
	if (result == NULL) 
		stat = (ExecStatusType) PQstatus(curr_conn);
	else
		stat = PQresultStatus(result);

	if ((stat == PGRES_EMPTY_QUERY)  ||
	    (stat == PGRES_BAD_RESPONSE) ||
	    (stat == PGRES_NONFATAL_ERROR))
	{
		Error("Bad response to postgres95 query (%s)", PQerrorMessage(curr_conn));
		PQclear(result);
		j = 0;
	} else if (stat == PGRES_FATAL_ERROR)
	{
		Error("Fatal postgres95 error (%s). Shutting down connection.", PQerrorMessage(curr_conn));
		pg_del_conn(curr_conn);
		j = 0;
		PQclear(result);
	} else if (stat == PGRES_COMMAND_OK)
	{
		j = 1;

		/* set last oid if this was an insert */
		tmpoid = PQoidStatus(result);
		if (tmpoid == NULL)
			pgLastOid = -1;
		else
			sscanf(tmpoid, "%d", &pgLastOid);

		PQclear(result);
	} else {
		j = pg_add_result(result);
	}

	sprintf(temp, "%d", j);
	Push(temp,LNUMBER);
#else
	Pop();
	Pop();
	Push("0", LNUMBER);
	Error("No postgres95 support");
#endif
}

void PG_result(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	char		*field;
	char		*ftype;
	int		res_ind, tuple_ind;
	int		field_ind;
	int		type_oid;
	PGresult	*result;
	PGconn		*curr_conn;
	char*		value;
	char*		tmp;
	char*		ret;

	field_ind = -1;
	field = NULL;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_result");
		Push("", STRING);
		return;
	}
	if (s->strval)
		if (s->type == STRING)
#if PHP_PG_LOWER
			field = estrdup(1,_strtolower(s->strval)); /* lower case field names */
#else
			field = estrdup(1,s->strval); /* case-sensitive field names */
#endif
		else
			field_ind = s->intval;
		
	else {
		Error("No field argument in pg_result");
		Push("", STRING);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in pg_result");
		Push("", STRING);
		return;
	}
	if (s->strval) tuple_ind = s->intval;
	else tuple_ind = 0;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_result");
		Push("", STRING);
		return;
	}
	if (s->strval) res_ind = s->intval;
	else res_ind = 0;

	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}

	/* check result */
	result = pg_get_result(res_ind);
	if (result == NULL) {
		Error("Unable to find result index %s", res_ind);
		Push("", STRING);
		return;
	}

	curr_conn = result->conn;
	if ((curr_conn == NULL) && (pgTypeRes==0)) {
		Error("Unable to get connection for tuple typing in pg_result");
		Push("", STRING);
		return;
	}

	/* get field index if the field parameter was a string */
	if (field != NULL)
	{
		field_ind = PQfnumber(result, field);
		if (field_ind < 0) {
			Error("Field %s not found", field);
			Push("", STRING);
			return;
		}
	}

	/* get field type */
	type_oid = PQftype(result, field_ind);
	ftype = pg_type(curr_conn, type_oid);

	if (ftype == NULL) {
		Error ("Unable to type field %s", field);
		Push("", STRING);
		return;
	}

	/* get ASCII value of the field
	 *
	 * One day support for arrays and binary cursors
	 * will need to be added.
 	 *
	 */
	value = PQgetvalue(result, tuple_ind, field_ind);

	if (value == NULL)
	{
		Error("Error getting data value");
		Push("", STRING);
		return;
	}

	tmp = estrdup(1,value);
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	/* cover int, int2, int4, integer, oid, smallint */
	/* john: this used to accidentally pick up "point" because
	** strstr(ftype, "int") matches "point". Never mind, I've
	** refudged it so it doesn't any more
	** john: not bool, which should have been parsed from 't' or 'f'
	*/
	if ((strstr (ftype, "int") && strcmp(ftype, "point")) ||
	    strstr (ftype, "oid")) {

		Push(tmp, LNUMBER);
		return;
	}

	/* cover real, float, float4, float8 */
	if (strstr (ftype, "float")  ||
	    !strcmp(ftype, "real")) {

		Push(tmp, DNUMBER);
		return;
	}

	/* cover bpchar, bytea, char, char2, char4, char8, char16,
	 * name, text, varchar, abstime, date, reltime, time,
	 * tinterval
	 * john: and bool as 't' or 'f', which is what pg95 1.09 returns
	 */
	if ( strstr(ftype, "char")  ||
	    !strcmp(ftype, "bool")  ||
	    !strcmp(ftype, "name")  ||
	    !strcmp(ftype, "text")  ||
	     strstr(ftype, "time")  ||
	    !strcmp(ftype, "date")  ||
	    !strcmp(ftype, "tinterval")) {

		Push((ret=AddSlashes(tmp,1)), STRING);
		return;
	}

	/* get everything else */

	Push((ret=AddSlashes(tmp,1)), STRING);
	return;

#else
	Pop();
	Pop();
	Pop();
	Push("", STRING);
	Error("No Postgres95 support");
#endif
}

void PGfreeResult(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_freeresult");
		return;
	}
	if (s->strval) {
		pg_del_result(s->intval);
	} else {
		Error("Invalid result index in pg_freeresult");
		return;
	}
#else
	Pop();
	Error("No Postgres95 support");
#endif
}

void PGconnect(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	PGconn		*new_conn;
	char		*host=NULL;
	char		*port=NULL;
	char		*options=NULL;
	char		*tty=NULL;
	char		*db=NULL;
	char		*temp;
	int		j;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) db = estrdup(1,s->strval);
	else {
		Error("No database name supplied in pg_connect");
		Push("0", LNUMBER);
		return;
	}
	
	s = Pop();
	if (!s) {
		Error("Stack error in pg_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) tty = estrdup(1,s->strval);
	else {
		Error("No tty name supplied in pg_connect");
		Push("0", LNUMBER);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in pg_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) options = estrdup(1,s->strval);
	else {
		Error("No options string supplied in pg_connect");
		Push("0", LNUMBER);
		return;
	}
	
	s = Pop();
	if (!s) {
		Error("Stack error in pg_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) port = estrdup(1,s->strval);
	else {
		Error("No port number supplied in pg_connect");
		Push("0", LNUMBER);
		return;
	}
	
	s = Pop();
	if (!s) {
		Error("Stack error in pg_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) host = estrdup(1,s->strval);
	else {
		Error("No host name/address supplied in pg_connect");
		Push("0", LNUMBER);
		return;
	}
	
	new_conn = PQsetdb(host, port, options, tty, db);
	if ((new_conn == NULL) || (PQstatus(new_conn) == CONNECTION_BAD)) {

		Error("Could not connect to database (%s)", PQerrorMessage(new_conn));
		temp = (char*) emalloc(1,2);
		sprintf(temp, "0");
	}
	else
	{
		j = pg_add_conn(new_conn);
		temp = (char*) emalloc(1,(j%10)+3);
		sprintf(temp, "%d", j);

		/* get and cache the type table */
		if (pgTypeRes == 0) 
			pg_type(new_conn, -2);
			
	}
	Push(temp, LNUMBER);
#else
	Pop();
	Pop();
	Pop();
	Pop();
	Pop();
	Error("No postgres95 support");
#endif
}

void PGclose(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		conn_ind;
	PGconn		*conn;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_close");
		return;
	}
	if (s->strval) conn_ind = s->intval;
	else conn_ind = 0;

	conn = pg_get_conn(conn_ind);

	pg_del_conn(conn);
#else
	Pop();
	Error("No postgres95 support");
#endif
}

void PGnumRows(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		res_ind;
	PGresult	*result;
	char		temp[16];

	s = Pop();
	if (!s) {
		Error("Stack error in pg_numrows");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) res_ind = s->intval;
	else res_ind = 0;

	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = pg_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in pg_numrows");
		Push("-1", LNUMBER);
		return;
	}

	sprintf(temp, "%d", PQntuples(result));
	Push(temp, LNUMBER);
#else
	Pop();
	Error("No postgres95 support");
#endif
}


void PGnumFields(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		res_ind;
	PGresult	*result;
	char		temp[16];

	s = Pop();
	if (!s) {
		Error("Stack error in pg_numfields");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) res_ind = s->intval;
	else res_ind = 0;

	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = pg_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in pg_numfields");
		Push("-1", LNUMBER);
		return;
	}

	sprintf(temp, "%d", PQnfields(result));
	Push(temp, LNUMBER);
#else
	Pop();
	Error("No postgres95 support");
#endif
}

void PGfieldName(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		res_ind;
	int		field_ind;
	PGresult	*result;
	char		*tmp;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldname");
		Push("", STRING);
		return;
	}
	if (s->strval) field_ind = s->intval;
	else {
		Error("No field index given in pg_fieldname");
		Push("", STRING);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldname");
		Push("", STRING);
		return;
	}
	if (s->strval) res_ind = s->intval;
	else res_ind = 0;

	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}
	result = pg_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in pg_fieldname");
		Push("", STRING);
		return;
	}

	if (field_ind >= PQnfields(result))
	{
		Error("Field index larger than number of fields");
		Push("", STRING);
		return;
	}

	tmp = estrdup(1,PQfname(result, field_ind));
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	Push(tmp, STRING);
#else
	Pop();
	Pop();
	Error("No postgres95 support");
#endif
}

void PGfieldType(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		res_ind;
	int		field_ind;
	int		type_oid;
	char*		ftype;
	PGresult	*result;
	char		*tmp;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldtype");
		Push("", STRING);
		return;
	}
	if (s->strval) field_ind = s->intval;
	else {
		Error("No field index given in pg_fieldtype");
		Push("", STRING);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldtype");
		Push("", STRING);
		return;
	}
	if (s->strval) res_ind = s->intval;
	else res_ind = 0;

	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}
	result = pg_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in pg_fieldtype");
		Push("", STRING);
		return;
	}

	if (field_ind >= PQnfields(result))
	{
		Error("Field index is larger than the number of fields");
		Push("", STRING);
		return;
	}

	type_oid = PQftype(result, field_ind);

	/* NULL because if we don't already have the type table cached,
	 * we are in trouble anyway.
	 */
	ftype = pg_type(NULL, type_oid);

	tmp = estrdup(1,ftype);
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	Push(tmp, STRING);
#else
	Pop();
	Pop();
	Error("No postgres95 support");
#endif
}

void PGfieldNum(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		res_ind;
	char		*fname;
	PGresult	*result;
	char		tmp[16];

	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldnum");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) fname = estrdup(1,s->strval);
	else {
		Error("No field name given in pg_fieldnum");
		Push("-1", LNUMBER);
		return;
	}
	
	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldnum");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) res_ind = s->intval;
	else res_ind = 0;

	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = pg_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in pg_fieldnum");
		Push("-1", LNUMBER);
		return;
	}

	sprintf(tmp, "%d", PQfnumber(result, fname));

	Push(tmp, LNUMBER);
#else
	Pop();
	Pop();
	Error("No Postgres95 support");
#endif
}


void PGfieldPrtLen(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		res_ind;
	int		tuple_ind;
	PGresult	*result;
	char		*field_name;
	int		field_ind;
	int		sz;
	char		tmp[16];

	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldprtlen");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) field_name = estrdup(1,s->strval);
	else {
		Error("No field name given in pg_fieldprtlen");
		Push("-1", LNUMBER);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldprtlen");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) tuple_ind = s->intval;
	else {
		Error("No tuple index given in pg_fieldprtlen");
		Push("-1", LNUMBER);
		return;
	}
	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldprtlen");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) res_ind = s->intval;
	else res_ind = 0;

	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = pg_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in pg_fieldprtlen");
		return;
	}

	field_ind = PQfnumber(result, field_name);
	if (field_ind < 0) {
		Error("Field %s not found", field_name);
		Push("-1", LNUMBER);
		return;
	}
	sz = PQgetlength(result, tuple_ind, field_ind);
	sprintf(tmp, "%d", sz);

	Push(tmp, LNUMBER);
#else
	Pop();
	Pop();
	Pop();
	Error("No postgres95 support");
#endif
}

void PGfieldSize(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		res_ind;
	PGresult	*result;
	char		*field_name;
	int		field_ind;
	int		sz;
	char		tmp[16];

	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldsize");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) field_name = estrdup(1,s->strval);
	else {
		Error("No field name given in pg_fieldsize");
		Push("-1", LNUMBER);
		return;
	}

	s = Pop();
	if (!s) {
		Error("Stack error in pg_fieldsize");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) res_ind = s->intval;
	else res_ind = 0;

	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	result = pg_get_result(res_ind);
	if (result == NULL) {
		Error("Bad result index in pg_fieldsize");
		Push("-1", LNUMBER);
		return;
	}

	field_ind = PQfnumber(result, field_name);
	if (field_ind < 0) {
		Error("Field %s not found", field_name);
		Push("-1", LNUMBER);
		return;
	}
	sz = PQfsize(result, field_ind);
	if (sz == -1) sz = 0;
	sprintf(tmp, "%d", sz);

	Push(tmp, LNUMBER);
#else
	Pop();
	Pop();
	Error("No postgres95 support");
#endif
}


void PGhost(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		conn_ind;
	PGconn		*conn;
	char		*tmp;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_host");
		Push("", STRING);
		return;
	}
	if (s->strval) conn_ind = s->intval;
	else conn_ind = 0;

	conn = pg_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Bad connection index in pg_host");
		Push("", STRING);
		return;
	}

	tmp = estrdup(1,PQhost(conn));
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	Push(tmp, STRING);

#else
	Pop();
	Error("No postgres95 support");
#endif
}


void PGdbName(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		conn_ind;
	PGconn		*conn;
	char		*tmp;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_dbname");
		Push("", STRING);
		return;
	}
	if (s->strval) conn_ind = s->intval;
	else conn_ind = 0;

	conn = pg_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Bad connection index in pg_dbName");
		Push("", STRING);
		return;
	}

	tmp = estrdup(1,PQdb(conn));
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	Push(tmp, STRING);

#else
	Pop();
	Error("No postgres95 support");
#endif
}

void PGoptions(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		conn_ind;
	PGconn		*conn;
	char		*tmp;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_options");
		Push("", STRING);
		return;
	}
	if (s->strval) conn_ind = s->intval;
	else conn_ind = 0;

	conn = pg_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Bad connection index in pg_options");
		Push("", STRING);
		return;
	}

	tmp = estrdup(1,PQoptions(conn));
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	Push(tmp, STRING);

#else
	Pop();
	Error("No postgres95 support");
#endif
}

void PGport(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		conn_ind;
	PGconn		*conn;
	char		*tmp;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_port");
		Push("", STRING);
		return;
	}
	if (s->strval) conn_ind = s->intval;
	else conn_ind = 0;

	conn = pg_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Bad connection index in pg_port");
		Push("", STRING);
		return;
	}

	tmp = estrdup(1,PQport(conn));
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	Push(tmp, STRING);

#else
	Pop();
	Error("No postgres95 support");
#endif
}

void PGtty(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		conn_ind;
	PGconn		*conn;
	char		*tmp;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_tty");
		Push("", STRING);
		return;
	}
	if (s->strval) conn_ind = s->intval;
	else conn_ind = 0;

	conn = pg_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Bad connection index in pg_tty");
		Push("", STRING);
		return;
	}

	tmp = estrdup(1,PQtty(conn));
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	Push(tmp, STRING);

#else
	Pop();
	Error("No postgres95 support");
#endif
}


void PGgetlastoid(void) {
#ifdef HAVE_LIBPQ
	char		tmp[16];

	sprintf(tmp, "%d", pgLastOid);
	Push(tmp, LNUMBER);

#else
	Error("No postgres95 support");
#endif
}


void PGerrorMessage(void) {
#ifdef HAVE_LIBPQ
	Stack		*s;
	int		conn_ind;
	PGconn		*conn;
	char		*tmp;

	s = Pop();
	if (!s) {
		Error("Stack error in pg_errorMessage");
		Push("", STRING);
		return;
	}
	if (s->strval) conn_ind = s->intval;
	else conn_ind = 0;

	conn = pg_get_conn(conn_ind);
	if (conn == NULL) {
		Error("Bad connection index in pg_errorMessge");
		Push("", STRING);
		return;
	}

	tmp = estrdup(1,PQerrorMessage(conn));
	if (tmp == NULL) {
		Error("Out of memory");
		Push("", STRING);
		return;
	}

	Push(tmp, STRING);

#else
	Pop();
	Error("No postgres95 support");
#endif
}
