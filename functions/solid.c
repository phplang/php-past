/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Jeroen van der Most <jvdmost@digiface.nl>                   |
   |          Stig Sæther Bakken <ssb@fast.no>                            |
   +----------------------------------------------------------------------+
 */

/* $Id: solid.c,v 1.41 2000/09/30 17:32:44 sas Exp $ */

#include <stdlib.h>
#include <ctype.h>

#ifndef MSVC5
#include "config.h"
#include "build-defs.h"
#endif

#include "php.h"
#include "internal_functions.h"
#include "php3_list.h"
#include "php3_solid.h"


#if HAVE_SOLID && !HAVE_UODBC

THREAD_LS static HENV henv;

function_entry solid_functions[] = {
	{"solid_exec",			php3_solid_exec,			NULL},
	{"solid_fetchrow",		php3_solid_fetchrow,		NULL},
	{"solid_result",		php3_solid_result,			NULL},
	{"solid_freeresult",	php3_solid_freeresult,		NULL},
	{"solid_connect",		php3_solid_connect,			NULL},
	{"solid_close",			php3_solid_close,			NULL},
	{"solid_numrows",		php3_solid_numrows,			NULL},
	{"solid_numfields",		php3_solid_numfields,		NULL},
	{"solid_fieldname",		php3_solid_fieldname,		NULL},
	{"solid_fieldnum",		php3_solid_fieldnum,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry solid_module_entry = {
	"Solid",
	solid_functions,
	php3_minit_solid,
	NULL,
	NULL,
	NULL,
	php3_info_solid,
	0,
	0,
	0,
	NULL
};

#if COMPILE_DL
php3_module_entry  *get_module() { return &solid_module_entry; };
#endif

THREAD_LS solid_module php3_solid_module;


static void _free_solidres(SQLResult *res)
{
	if (res && res->values) {
		efree(res->values);
	}
	if (res) {
		efree(res);
	}
}


static void _close_solidconn(SQLConn *conn)
{
#if 0
	list_entry *le2;
	SQLResult *res;
	int i, nument = _php3_hash_num_elements(&list);

	for (i = 0; i < nument; i++) {
		if (_php3_hash_index_find(&list, i, (void **)&le2) == FAILURE ||
			le2->type != php3_solid_module.le_result) {
			continue;
		}
		res = (SQLResult *)le2->ptr;
		if (res->conn == conn) {
			res->conn = NULL;
		}
	}
#else
	/* XXX FIXME This is not completely safe, we may still have
	 * pointers to this SQLConn in some SQLResult that will crash us
	 * if accessed.  If results are freed before connections this
	 * won't be a problem.
	 */
	if (conn) {
		efree(conn);
	}
#endif
}


/* module startup function */
int php3_minit_solid(INIT_FUNC_ARGS)
{
    SQLAllocEnv(&henv);
	php3_solid_module.le_result = register_list_destructors(php3_solid_freeresult,NULL);
	php3_solid_module.le_result = register_list_destructors(_free_solidres,NULL);
	php3_solid_module.le_link = register_list_destructors(_close_solidconn,NULL);
	return SUCCESS;
}

void php3_info_solid(void)
{
	PUTS("Compile-time variables:<br>\n"
		 "    <tt>SOLID_INCLUDE=" PHP_SOLID_INCLUDE "</tt><br>\n"
		 "    <tt>SOLID_LIBS=" PHP_SOLID_LIBS "</tt>");
}

int php3_shutdown_solid(INTERNAL_FUNCTION_PARAMETERS)
{
	/* run php3_solid_closeall if we're not in "persistence mode"? */
	return SUCCESS;
}

/***************************************************
 *
 * List maintainers (SQLResult and SQLConn)
 *
 ***************************************************/

static int
solid_add_result(INTERNAL_FUNCTION_PARAMETERS, SQLResult *result, SQLConn* conn)
{
	int ind = php3_list_insert(result, php3_solid_module.le_result);

	result->index = ind;
	result->conn = conn;

	return ind;
}

static SQLResult *
solid_get_result(INTERNAL_FUNCTION_PARAMETERS, int ind)
{
	SQLResult *res;
	int type;

	res = php3_list_find(ind, &type);
	if (!res || type != php3_solid_module.le_result) {
		return NULL;
	}
	return res;
}

static void
solid_del_result(INTERNAL_FUNCTION_PARAMETERS, int ind)
{
	/* XXX FIXME we don't check that this is really an SQL result */
	php3_list_delete(ind);
}

static int
solid_add_conn(INTERNAL_FUNCTION_PARAMETERS, HDBC hdbc)
{
	SQLConn *new;
	int ind;

	new = emalloc(sizeof(SQLConn));
	ind = php3_list_insert(new, php3_solid_module.le_link);

	new->hdbc = hdbc;
	new->index = ind;

	return ind;
}

static SQLConn *
solid_get_conn(INTERNAL_FUNCTION_PARAMETERS, int ind)
{
	SQLConn *conn;
	int type;

	conn = php3_list_find(ind, &type);
	if (!conn || type != php3_solid_module.le_link) {
		return NULL;
	}
	return conn;
}

static void
solid_del_conn(INTERNAL_FUNCTION_PARAMETERS, int ind)
{
	/* XXX FIXME we don't check that this is really an SQL connection */
	php3_list_delete(ind);
}


/* Main User Functions */

void php3_solid_exec(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *CONN, *QUERY;
	char        *query;
	int         i, j;
	SQLResult  *result = NULL;
	SQLConn    *conn = NULL;
	HDBC        hdbc = SQL_NULL_HDBC;
	short       resultcols;
	SWORD       colnamelen; /* Not used */
	SDWORD      displaysize;
	RETCODE     rc;
	char        state[6];     /* Not used */
	SDWORD      error;        /* Not used */
	char        errormsg[255];
	SWORD       errormsgsize; /* Not used */
	SWORD       status;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &CONN, &QUERY) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(CONN);
	convert_to_string(QUERY);

	query = QUERY->value.str.val;

	conn = solid_get_conn(INTERNAL_FUNCTION_PARAM_PASSTHRU, CONN->value.lval);
	if (conn) {
		hdbc = conn->hdbc;
	}

	if (hdbc == NULL) {
		php3_error(E_WARNING, "Bad SQL connection number");
		RETURN_FALSE;
	}

    result = (SQLResult *)emalloc(sizeof(SQLResult));

	if (result == NULL) {
		php3_error(E_ERROR, "Out of memory");
		RETURN_FALSE;
	}

	status = SQLAllocStmt(hdbc, &result->stmt);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		php3_error(E_WARNING,
					"Solid: SQLAllocStmt returned %d", status);
		RETURN_FALSE;
	}

	if ((rc = SQLExecDirect(result->stmt, query, SQL_NTS)) != SQL_SUCCESS) {
		SQLError(henv, hdbc, result->stmt, state, &error, errormsg,
				 sizeof(errormsg)-1, &errormsgsize);
		php3_error(E_WARNING, "Bad response to SQL query %d %s (%s)",
					rc, state, errormsg);
		SQLFreeStmt(result->stmt, SQL_DROP);
		j = 0;
	}
	else {
		SQLNumResultCols(result->stmt, &resultcols);
		if (resultcols == 0) {
			j = 1;
		}
		else {
			result->numcols = resultcols;
			result->fetched = 0;
			result->values = (SQLResultValue *)emalloc(sizeof(SQLResultValue) * resultcols);
			for (i = 0; i < resultcols; i++) {
				SQLColAttributes(result->stmt, i+1, SQL_COLUMN_NAME,
								 result->values[i].name,
								 sizeof(result->values[i].name),
								 &colnamelen, NULL);
				SQLColAttributes(result->stmt, i+1, SQL_COLUMN_DISPLAY_SIZE,
								 NULL, 0, NULL, &displaysize);
				result->values[i].value = (char *)emalloc(displaysize + 1);
				if (result->values[i].value != NULL)
					/* Should we use SQL_C_BINARY here instead? */
					SQLBindCol(result->stmt, i+1, SQL_C_CHAR, 
							   result->values[i].value, displaysize+1,
							   &result->values[i].vallen);
				/* else, we try to do SQLGetData in
				   Solid_result() in case
				   this is a BLOB field - sopwith */
			}
		    j = solid_add_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, result, conn);
		}
	}

	RETVAL_LONG(j);
}


void php3_solid_fetchrow(INTERNAL_FUNCTION_PARAMETERS)
{
	pval     *res;
	int         res_ind;
	SQLResult *result;
	RETCODE     rc;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &res) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(res);
	res_ind = res->value.lval;

	/* check result */
	result = solid_get_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, res_ind);
	if (result == NULL) {
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}

	rc = SQLFetch(result->stmt);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}
	result->fetched++;
	RETVAL_TRUE;
}


void php3_solid_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval     *res, *col;
	char        *field;
	int         res_ind;
	int         field_ind;
	SQLResult *result;
	int         i;
	RETCODE     rc;

	field_ind = -1;
	field = NULL;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &res, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(res);

	if (col->type == IS_STRING) {
		field = col->value.str.val;
	} else {
		field_ind = col->value.lval;
	}

	res_ind = res->value.lval;

	/* check result */
	result = solid_get_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, res_ind);
	if (result == NULL) {
		php3_error(E_WARNING, "Unable to find result index %d",
					res_ind);
		RETURN_FALSE;
	}

	/* get field index if the field parameter was a string */
	if (field != NULL) {
		for (i = 0; i < result->numcols; i++) {
			if (strcasecmp(result->values[i].name, field) == 0) {
				field_ind = i;
				break;
			}
		}
		if (field_ind < 0) {
			php3_error(E_WARNING, "Field %s not found", field);
			RETURN_FALSE;
		}
	} else {
		/* check for limits of field_ind if the field parameter was an int */
		if (field_ind >= result->numcols || field_ind <0) {
			php3_error(E_WARNING, "Field index is larger "
						"than the number of fields");
			RETURN_FALSE;
		}
	}

    if (result->fetched == 0) {
		/* User forgot to call solid_fetchrow(), let's do it here */
		rc = SQLFetch(result->stmt);
		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			RETURN_FALSE;
		}
		result->fetched++;
	}

	if (result->values[field_ind].vallen == SQL_NULL_DATA) {
		RETURN_FALSE; /* FIXME: better value for null ? */
	} else {
		char c, *realval = &c;
		SDWORD fieldsize = 12345;

		if (result->values[field_ind].value != NULL) {
			RETURN_STRING(result->values[field_ind].value,1);
		}
		/* All this ugly code here is the fault of
		 * sopwith@redhat.com :) If there
		 * is no value pointer set up where we do
		 * the SQLBindCol, then we know that we
		 * should at least try to get the size of the
		 * field as we go - i.e. for BLOB fields
		 * & etc.
		 */
		
		/* This first SQLGetData is used just to
		 * find out the exact size of the field - 
		 * that is why we only get one char
		 * (it doesn't seem to want to get 0 chars)
		 */
		if (SQLGetData(result->stmt, field_ind + 1, SQL_C_CHAR,
					   realval, 1, &fieldsize) == SQL_ERROR) {
			char msgbuf[512];
			UCHAR szSqlState;
			SDWORD pfNativeError;
			SWORD foo;
			HDBC conn = SQL_NULL_HDBC;

			if (result->conn) {
				conn = result->conn->hdbc;
			}

			SQLError(henv, conn, result->stmt, &szSqlState,
					 &pfNativeError, msgbuf, sizeof(msgbuf)-1,
					 &foo);
			php3_error(E_WARNING, "Initial SQLGetData failed");
			php3_error(E_WARNING, "%s", msgbuf);
		} else if (fieldsize > 0) {
			realval = emalloc(fieldsize + 2);
			realval[0] = '\0';
			if (realval == NULL ||
				SQLGetData(result->stmt,
						   field_ind + 1, SQL_C_CHAR,
						   realval, fieldsize + 1,
						   &fieldsize) == SQL_ERROR) {
				php3_error(E_WARNING, "Out of memory");
			} else {
				RETURN_STRING(realval,1);
			}
		}
	}
	RETVAL_FALSE;
}


void php3_solid_freeresult(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *res;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &res) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(res);
	solid_del_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, res->value.lval);
}


void php3_solid_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *dsn_arg, *uid_arg, *pwd_arg;
	HDBC    new_conn;
	char    *dsn=NULL;
	char    *uid=NULL;
	char    *pwd=NULL;
	int     j;
	RETCODE rc;
	char    state[6];
	SDWORD  error;
	char    errormsg[255];
	SWORD   errormsgsize;

	if (ARG_COUNT(ht) != 3 ||
		getParameters(ht, 3, &dsn_arg, &uid_arg, &pwd_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(dsn_arg);
	convert_to_string(uid_arg);
	convert_to_string(pwd_arg);

	dsn = dsn_arg->value.str.val;
	uid = uid_arg->value.str.val;
	pwd = pwd_arg->value.str.val;

	SQLAllocConnect(henv, &new_conn);
	SQLSetConnectOption(new_conn, SQL_TRANSLATE_OPTION,
						SQL_SOLID_XLATOPT_ANSI);
	rc = SQLConnect(new_conn, dsn, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		rc = SQLError(henv, new_conn, SQL_NULL_HSTMT, state, &error, errormsg,
					  sizeof(errormsg) - 1, &errormsgsize);
		if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO) {
			php3_error(E_WARNING,
						"Could not connect to database \"%s\" (%s)", dsn,
						errormsg);
		}
		else {
			php3_error(E_WARNING,
						"Could not connect to database \"%s\"", dsn);
		}
		RETVAL_FALSE;
	}
	else {
		j = solid_add_conn(INTERNAL_FUNCTION_PARAM_PASSTHRU, new_conn);
		RETVAL_LONG(j);
	}
}


void php3_solid_close(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *conn_arg;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &conn_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(conn_arg);

	solid_del_conn(INTERNAL_FUNCTION_PARAM_PASSTHRU, conn_arg->value.lval);
}

void php3_solid_numrows(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *res_arg;
	SQLResult *result;
	SDWORD      rows;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &res_arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(res_arg);
	result = solid_get_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, res_arg->value.lval);

	if (result == NULL) {
		php3_error(E_WARNING, "Bad result index in solid_numrows");
		RETURN_FALSE;
	}

    /*SQLNumResultCols(result->stmt, &cols);*/
	SQLRowCount(result->stmt, &rows);

	RETVAL_LONG(rows);
}


void php3_solid_numfields(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *res_arg;
	SQLResult *result;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &res_arg)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(res_arg);
	result = solid_get_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, res_arg->value.lval);

	if (result == NULL) {
		php3_error(E_WARNING, "Bad result index in solid_numfields");
		RETURN_FALSE;
	}

	RETVAL_LONG(result->numcols);
}

void php3_solid_fieldname(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *res_arg, *field_arg;
	int         res_ind;
	int         field_ind;
	SQLResult *result;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &res_arg, &field_arg) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(res_arg);
	convert_to_long(field_arg);

	field_ind = field_arg->value.lval;
	res_ind = res_arg->value.lval;

	result = solid_get_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, res_ind);
	if (result == NULL) {
		php3_error(E_WARNING, "Bad result index in solid_fieldname");
		RETURN_FALSE;
	}

	if (field_ind >= result->numcols || field_ind < 0) {
		php3_error(E_WARNING, "Invalid field index");
		RETURN_FALSE;
	}

	RETVAL_STRING(result->values[field_ind].name,1);
}

void php3_solid_fieldnum(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *res_arg, *field_arg;
	int         field_ind;
	char        *fname;
	SQLResult *result;
	int         i;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &res_arg, &field_arg) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(res_arg);
	convert_to_string(field_arg);

	fname = field_arg->value.str.val;
	result = solid_get_result(INTERNAL_FUNCTION_PARAM_PASSTHRU, res_arg->value.lval);

	if (result == NULL) {
		php3_error(E_WARNING, "Bad result index in solid_fieldnum");
		RETURN_FALSE;
	}

	field_ind = -1;
	for (i = 0; i < result->numcols; i++) {
		if (strcasecmp(result->values[i].name, fname) == 0) {
			field_ind = i;
			break;
		}
	}
	RETVAL_LONG(field_ind);
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
