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
   | Authors: Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   +----------------------------------------------------------------------+
 */

/* $Id: adabasd.c,v 1.58 2000/01/01 04:31:14 sas Exp $ */

/*
 * TODO/FIXME 
   - Add/StripSlashes ?
   - various error checking (malloc)
 */
#ifndef MSVC5
#include "config.h"
#include "build-defs.h"
#endif

#include "php.h"
#include "internal_functions.h"
#include "php3_string.h"
#include "functions/head.h"
#include "adabasd.h"

#if HAVE_ADABAS && !HAVE_UODBC

adabas_module php3_adabas_module;

function_entry adabas_functions[] = {
    {"ada_afetch", php3_Ada_aFetch, NULL},
    {"ada_autocommit", php3_Ada_autocommit, NULL},
    {"ada_close", php3_Ada_close, NULL},
    {"ada_closeall", php3_Ada_closeAll, NULL},
    {"ada_commit", php3_Ada_commit, NULL},
    {"ada_connect", php3_Ada_connect, NULL},
    {"ada_exec", php3_Ada_exec, NULL},
    {"ada_fetchrow", php3_Ada_fetchRow, NULL},
    {"ada_fieldlen", php3_Ada_fieldLen, NULL},
    {"ada_fieldname", php3_Ada_fieldName, NULL},
    {"ada_fieldtype", php3_Ada_fieldType, NULL},
    {"ada_freeresult", php3_Ada_freeResult, NULL},
    {"ada_numfields", php3_Ada_numFields, NULL},
    {"ada_numrows", php3_Ada_numRows, NULL},
    {"ada_result", php3_Ada_result, NULL},
    {"ada_resultall", php3_Ada_resultAll, NULL},
    {"ada_rollback", php3_Ada_rollback, NULL},
	{ NULL, NULL, NULL}
};

php3_module_entry adabas_module_entry = {
	        "Adabas", adabas_functions, php3_minit_adabas, php3_mshutdown_adabas, php3_rinit_adabas, NULL, php3_info_adabas, 
			STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
php3_module_entry *get_module() { return &adabas_module_entry; };
#endif

#include "php3_list.h"

void _free_ada_result(AdaResult *res)
{
	if (res){
		if (res->values) {
			register int i;
			
			for (i=0; i<res->numcols; i++) {
				efree(res->values[i].value);
			}
			efree(res->values);
			res->values = NULL;
		}
		/* this is probably taken care of by the Adabas library, when the connection is closed
		 * don't free it explicitly!
		 */
		if (res->stmt){
			/*SQLFreeStmt(res->stmt,SQL_DROP);*/
			res->stmt = NULL;
		}
		efree(res);
		res = NULL;
	}
}

void _close_ada_connection(HDBC conn)
{
	/* FIXME
	 * Closing a connection will fail if there are
	 * pending transactions
	 */
	SQLDisconnect(conn);
	SQLFreeConnect(conn);
	conn = NULL;
}

int php3_minit_adabas(INIT_FUNC_ARGS) 
{
	SQLAllocEnv(&php3_adabas_module.henv);
	
	cfg_get_string("adabas.defaultDB",&php3_adabas_module.defDB);
	cfg_get_string("adabas.defaultUser",&php3_adabas_module.defUser);
	cfg_get_string("adabas.defaultPW",&php3_adabas_module.defPW);
	php3_adabas_module.le_result = register_list_destructors(_free_ada_result, NULL);
	php3_adabas_module.le_conn = register_list_destructors(_close_ada_connection, NULL);
	return SUCCESS;
}

int php3_rinit_adabas(INIT_FUNC_ARGS)
{
	php3_adabas_module.defConn = 0;
	return SUCCESS;
}

int php3_mshutdown_adabas(void)
{
	SQLFreeEnv(php3_adabas_module.henv);
	return SUCCESS;
}

void php3_info_adabas(void)
{
	PUTS("Adabas D Support Active");
}

/*
 *
 * List management functions
 *
 */

int ada_add_result(HashTable *list,AdaResult *result)
{
	return php3_list_insert(result, php3_adabas_module.le_result);
}

AdaResult *ada_get_result(HashTable *list, int ind)
{
	AdaResult *res;
	int type;

	res = (AdaResult*)php3_list_find(ind, &type);
	if (!res || type != php3_adabas_module.le_result) {
		return NULL;
	}
	return res;
}

void ada_del_result(HashTable *list, int ind)
{
	AdaResult *res;
	int type;

	res = (AdaResult *)php3_list_find(ind, &type);
	if (!res || type != php3_adabas_module.le_result) {
		php3_error(E_WARNING,"Can't find result %d",ind);
		return;
	}
	php3_list_delete(ind);
}

int ada_add_conn(HashTable *list, HDBC conn)
{
	return php3_list_insert(conn, php3_adabas_module.le_conn);
}

void make_def_conn(HashTable *list)
{
	HDBC    new_conn;
	RETCODE rc;

	SQLAllocConnect(php3_adabas_module.henv, &new_conn);
	rc = SQLConnect(new_conn, php3_adabas_module.defDB, SQL_NTS, php3_adabas_module.defUser, 
					SQL_NTS, php3_adabas_module.defPW, SQL_NTS);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		ada_sql_error(new_conn, SQL_NULL_HSTMT, "SQLConnect"); 
	} else {
		php3_adabas_module.defConn = ada_add_conn(list, new_conn);
	}
}

HDBC ada_get_conn(HashTable *list, int ind)
{
	HDBC conn;
	int type;

	if (ind == 0){
    	if (php3_adabas_module.defConn == 0){
			make_def_conn(list);
		}
		ind = php3_adabas_module.defConn;
	}

	conn = (HDBC)php3_list_find(ind, &type);
	if (!conn || type != php3_adabas_module.le_conn) {
		return NULL;
	}
	return conn;
}

void ada_del_conn(HashTable *list, int ind)
{
	HDBC conn;
	int type;

	conn = (HDBC)php3_list_find(ind, &type);
	if (!conn || type != php3_adabas_module.le_conn) {
		return;
	}
	php3_list_delete(ind);	
}

void ada_sql_error(HDBC conn, HSTMT stmt, char *func)
{
    char	state[6];     /* Not used */
	SDWORD	error;        /* Not used */
	char	errormsg[255];
	SWORD	errormsgsize; /* Not used */
	
	SQLError(php3_adabas_module.henv, conn, stmt, state, &error, errormsg,
				sizeof(errormsg)-1, &errormsgsize);
	
	if (func)
		php3_error(E_WARNING, "Function %s", func);
	php3_error(E_WARNING, "SQL error: %s, SQL state %s", errormsg, state);	
}

/* Main User Functions */

void php3_Ada_closeAll(INTERNAL_FUNCTION_PARAMETERS)
{
	void *ptr;
	int type;
	int i, nument = _php3_hash_next_free_element(list);

	for (i = 1; i < nument; i++) {
		ptr = php3_list_find(i, &type);
		if (!ptr || type != php3_adabas_module.le_result){
			continue;
		}
		php3_list_delete(i);
	}

	for (i = 1; i < nument; i++) {
		ptr = php3_list_find(i, &type);
		if (!ptr || type != php3_adabas_module.le_conn){
			continue;
		}
		php3_list_delete(i);
	}
}

void php3_Ada_exec(INTERNAL_FUNCTION_PARAMETERS)
{
	pval 	*arg1, *arg2;
	int         conn;
	char        *query;
	int         i;
	AdaResult   *result=NULL;
	HDBC        curr_conn=NULL;
	short       resultcols;
	SWORD       colnamelen; /* Not used */
	SDWORD      displaysize, coltype;
	UDWORD		scrollopts;
	RETCODE     rc;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	conn = arg1->value.lval;
	query = arg2->value.str.val;
	
	curr_conn = ada_get_conn(list, conn);
	if (curr_conn == NULL){
		php3_error(E_WARNING, "Bad Adabas connection number");
		RETURN_ZERO;
	}

	_php3_stripslashes(query,NULL);
	
	result = (AdaResult *)emalloc(sizeof(AdaResult));
	if (result == NULL){
		php3_error(E_WARNING, "Out of memory");
		RETURN_ZERO;
	}

	rc = SQLAllocStmt(curr_conn, &(result->stmt));
	if (rc == SQL_INVALID_HANDLE){
		php3_error(E_WARNING, "SQLAllocStmt error 'Invalid Handle'");
		RETURN_ZERO;
	}

	if (rc == SQL_ERROR){
		ada_sql_error(curr_conn, SQL_NULL_HSTMT, "SQLAllocStmt");
		RETURN_ZERO;
	}

	/* Set scrolling cursor here, works only if supp. by the driver */	
	rc = SQLGetInfo(curr_conn, SQL_FETCH_DIRECTION,  &scrollopts, sizeof(scrollopts), NULL);
	if (rc == SQL_SUCCESS){
		if ((result->fetch_abs = (scrollopts & SQL_FD_FETCH_ABSOLUTE))){
			if (SQLSetStmtOption(result->stmt, SQL_CURSOR_TYPE, SQL_CURSOR_DYNAMIC)
				!= SQL_SUCCESS){
				ada_sql_error(curr_conn, result->stmt, " SQLSetStmtOption");
				SQLFreeStmt(result->stmt, SQL_DROP);
				RETURN_ZERO;
			}
		}
	} else {
		result->fetch_abs = 0;
	}
	/* End of scrolling cursor */

	if ((rc = SQLExecDirect(result->stmt, query, SQL_NTS)) != SQL_SUCCESS){
		ada_sql_error(curr_conn, result->stmt, "SQLExecDirect"); 
		SQLFreeStmt(result->stmt, SQL_DROP);
		RETURN_ZERO;
	}	
	SQLNumResultCols(result->stmt, &resultcols);
	/* For insert, update etc. cols == 0 */
	if ((result->numcols = resultcols) > 0){
		result->values = (ResultValue *)emalloc(sizeof(ResultValue)*resultcols);

		if (result->values == NULL){
			php3_error(E_WARNING, "Out of memory");
			SQLFreeStmt(result->stmt, SQL_DROP);
			RETURN_ZERO;
		}

		for(i = 0; i < resultcols; i++){
			SQLColAttributes(result->stmt, i+1, SQL_COLUMN_NAME,
							result->values[i].name, sizeof(result->values[i].name),
							&colnamelen, NULL);
			SQLColAttributes(result->stmt, i+1, SQL_COLUMN_DISPLAY_SIZE,
								NULL, 0, NULL, &displaysize);
			/* Decision, whether a column will be output directly or returned into a 
			   PHP variable is based on two attributes:
			   - Anything longer than 4096 Bytes (SQL_COLUMN_DISPLAY_SIZE)
			   - and/or of datatype binary (SQL_BINARY,SQL_VARBINARY,SQL_LONGVARBINARY)
			   will be sent direct to the client.
			   Hint: SQL_BINARY and SQL_VARBINARY can be returned to PHP with the SQL
			   function HEX(), e.g. SELECT HEX(SYSKEY) SYSKEY FROM MYTABLE.
			*/
			if (displaysize > 4096){
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
					result->values[i].value = (char *)emalloc(displaysize);
					SQLBindCol(result->stmt, i+1, SQL_C_BINARY,result->values[i].value,
								displaysize, &result->values[i].vallen);
					break;
				default:
					result->values[i].value = (char *)emalloc(displaysize + 1);
					SQLBindCol(result->stmt, i+1, SQL_C_CHAR, result->values[i].value,
								displaysize + 1, &result->values[i].vallen);
					break;
			}
		}
	} else {
		result->values = NULL;
		result->stmt = NULL;
	}
	result->hdbc = curr_conn;
	result->fetched = 0;
	RETURN_LONG(ada_add_result(list, result));
}

void php3_Ada_aFetch(INTERNAL_FUNCTION_PARAMETERS)
{
	int         res_ind, numArgs, i;
	SDWORD      rownum = -1;
	AdaResult   *result;
	RETCODE     rc;
	UDWORD      crow;
	UWORD       RowStatus[1];
	pval     *arg1, *arg2, *arr, tmp;

	numArgs = ARG_COUNT(ht);
	
	switch(numArgs)
	{
		case 2:
			if (getParameters(ht, 2, &arg1, &arr) == FAILURE)
				WRONG_PARAM_COUNT;
			break;
		case 3:
			if (getParameters(ht, 3, &arg1, &arg2, &arr) == FAILURE)
				WRONG_PARAM_COUNT;
			convert_to_long(arg2);
			rownum = arg2->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (!ParameterPassedByReference(ht, numArgs)){
		php3_error(E_WARNING, "Array not passed by reference in call to ada_afetch()");
		return;
	}
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;

	/* check result */
	result = ada_get_result(list, res_ind);
	if (result == NULL){
		php3_error(E_WARNING, "Unable to find result index %s", res_ind);
		RETURN_FALSE;
	}

	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (arr->type != IS_ARRAY){
		if (array_init(arr) == FAILURE){
			php3_error(E_WARNING, "Can't convert to type Array");
			return;
		}
	}

	if (result->fetch_abs){
		if (rownum > 0)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		else
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	} else {
		rc = SQLFetch(result->stmt);
	}
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		RETURN_FALSE;

	if (rownum > 0 && result->fetch_abs)
		result->fetched = rownum;
	else
		result->fetched++;

	for(i = 0; i < result->numcols; i++){
		if (result->values[i].value != NULL &&
			result->values[i].vallen != SQL_NO_TOTAL &&
			result->values[i].passthru == 0){
			tmp.type = IS_STRING;
			tmp.value.str.len = strlen(result->values[i].value);
			tmp.value.str.val = estrndup(result->values[i].value,tmp.value.str.len);
		} else {
			tmp.type = IS_STRING;
			tmp.value.str.len = 0;
			tmp.value.str.val = empty_string;
		}
		_php3_hash_index_update(arr->value.ht, i, (void *) &tmp, sizeof(pval), NULL);
	}
	RETURN_LONG(result->numcols);	
}

void php3_Ada_fetchRow(INTERNAL_FUNCTION_PARAMETERS)
{
	int         res_ind, numArgs;
	SDWORD      rownum = 1;
	AdaResult   *result;
	RETCODE     rc;
	UDWORD      crow;
	UWORD       RowStatus[1];
	pval		*arg1, *arg2;
	
	numArgs = ARG_COUNT(ht);
	if (numArgs ==  1){
		if (getParameters(ht, 1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long(arg2);
		rownum = arg2->value.lval;
	}
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	result = ada_get_result(list, res_ind);
	if (result == NULL){
		php3_error(E_WARNING, "Unable to find result index %s", res_ind);
		RETURN_FALSE;
	}
	
	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if (result->fetch_abs){
		if (numArgs > 1)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		else
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	} else {
		rc = SQLFetch(result->stmt);
	}
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		RETURN_FALSE;
	
	if (numArgs > 1)
		result->fetched = rownum;
	else 
		result->fetched++;
	
	RETURN_TRUE;
}	

void php3_Ada_result(INTERNAL_FUNCTION_PARAMETERS)
{
	char        *field;
	int         res_ind;
	int         field_ind;
	AdaResult   *result;
	int         i;
	RETCODE     rc;
	UDWORD      crow;
	UWORD       RowStatus[1];
	pval		*arg1, *arg2;

	field_ind = -1;
	field = NULL;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2 , &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;

	if (arg2->type == IS_STRING)
		field = arg2->value.str.val;
	else{
		convert_to_long(arg2);
		field_ind = arg2->value.lval - 1;
	}
	
	/* check result */
	result = ada_get_result(list, res_ind);
	if (result == NULL){
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}
	
	if ((result->numcols == 0)){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	/* get field index if the field parameter was a string */
	if (field != NULL){
		for(i = 0; i < result->numcols; i++){
			if (!strcasecmp(result->values[i].name, field)){
				field_ind = i;
				break;
			}
		}

		if (field_ind < 0){
			php3_error(E_WARNING, "Field %s not found", field);
			RETURN_FALSE;
		}
	} else {
		/* check for limits of field_ind if the field parameter was an int */
		if (field_ind >= result->numcols || field_ind < 0){
			php3_error(E_WARNING, "Field index is larger than the number of fields");
			RETURN_FALSE;
		}
	}

	if (result->fetched == 0){
		/* User forgot to call ada_fetchrow(), let's do it here */
		if (result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		else
			rc = SQLFetch(result->stmt);

		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
			RETURN_FALSE;
		
		result->fetched++;
	}
       
	if (result->values[field_ind].vallen == SQL_NULL_DATA){
		RETURN_FALSE
	} else {
		if (result->values[field_ind].value != NULL &&
			result->values[field_ind].vallen != SQL_NO_TOTAL && 
			result->values[field_ind].passthru == 0){
			RETURN_STRING(result->values[field_ind].value,1);	
		} else {
			char buf[4096];
			SDWORD fieldsize = 4096;

			/* Make sure that the Header is sent */ 
			if (!php3_header())
			   	RETURN_TRUE;  /* don't output anything on a HEAD request */

			if (result->values[field_ind].vallen != SQL_NO_TOTAL){
				fieldsize = result->values[field_ind].vallen;
			}
			/* Output what's already in the field */
			PHPWRITE(result->values[field_ind].value, fieldsize );
			
			if (result->values[field_ind].vallen != SQL_NO_TOTAL){
				RETURN_TRUE;
			}

			/* Call SQLGetData until SQL_SUCCESS is returned */
			while(1){
				rc = SQLGetData(result->stmt, field_ind + 1, SQL_C_BINARY,
								buf, 4096, &fieldsize);

				if (rc == SQL_ERROR){ 
					ada_sql_error(result->hdbc, result->stmt, "SQLGetData");
					RETURN_FALSE;
				}

				if (rc == SQL_SUCCESS_WITH_INFO)
					fieldsize = 4096;
				PHPWRITE(buf, fieldsize);
				
				if (rc == SQL_SUCCESS) /* no more data avail */
					break;
			}
			RETURN_TRUE;
		}
	}
}

void php3_Ada_resultAll(INTERNAL_FUNCTION_PARAMETERS)
{
	int         res_ind, numArgs;
	AdaResult   *result;
	int         i;
	RETCODE     rc;
	UDWORD      crow;
	UWORD       RowStatus[1];
	pval     *arg1, *arg2;

	numArgs = ARG_COUNT(ht);
	if (numArgs ==  1){
		if (getParameters(ht, 1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		if (getParameters(ht, 2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
	}
				
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	result = ada_get_result(list, res_ind);
	if (result == NULL){
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}
	
	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (result->fetch_abs)
		rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	else
		rc = SQLFetch(result->stmt);
	
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		php3_printf("<h2>No rows found</h2>\n");
		RETURN_LONG(0);
	}
	
	/* Start table tag */
	if (numArgs == 1){
		php3_printf("<table><tr>");
	} else {
		convert_to_string(arg2);	
		php3_printf("<table %s ><tr>",arg2->value.str.val); 
	}
	
	for(i = 0; i < result->numcols; i++)
		php3_printf("<th>%s</th>", result->values[i].name);

	php3_printf("</tr>\n");

	while(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO){
		result->fetched++;
		php3_printf("<tr>");
		for(i = 0; i < result->numcols; i++){
			if (result->values[i].vallen == SQL_NULL_DATA)
				php3_printf("<td>&nbsp;</td>");
			else{
				if (result->values[i].passthru == 0)
					php3_printf("<td>%s</td>", result->values[i].value);
				else 
					php3_printf("<td>Unsupp. data type</td>");
			}
		}
		php3_printf("</tr>\n");

		if (result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		else
			rc = SQLFetch(result->stmt);		
	}
	php3_printf("</table>\n");
	RETURN_LONG(result->fetched);
}

void php3_Ada_freeResult(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1;
	
	if ( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	ada_del_result(list, arg1->value.lval);
	RETURN_TRUE;
}

void php3_Ada_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	HDBC    new_conn;
	char    *db = NULL;
	char    *uid = NULL;
	char    *pwd = NULL;
	RETCODE rc;
	pval *arg1, *arg2, *arg3;
	
	if (getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;
	
	convert_to_string(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);

	db = arg1->value.str.val;
	uid = arg2->value.str.val;
	pwd = arg3->value.str.val;

	SQLAllocConnect(php3_adabas_module.henv, &new_conn);
	
	rc = SQLConnect(new_conn, db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		ada_sql_error(new_conn, SQL_NULL_HSTMT, "SQLConnect");
		RETURN_FALSE;
	} else {
		RETURN_LONG(ada_add_conn(list, new_conn));
	}
}

void php3_Ada_close(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1;

    if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	ada_del_conn(list, arg1->value.lval);
}

void php3_Ada_numRows(INTERNAL_FUNCTION_PARAMETERS)
{
	AdaResult   *result;
	SDWORD      rows;
	pval 	*arg1;
	
	if ( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}            

	convert_to_long(arg1);

	result = ada_get_result(list, arg1->value.lval);
	if (result == NULL){
		php3_error(E_WARNING, "Bad result index in ada_numrows");
		RETURN_FALSE;
	}

	SQLRowCount(result->stmt, &rows);
	RETURN_LONG(rows);
}

void php3_Ada_numFields(INTERNAL_FUNCTION_PARAMETERS)
{
	AdaResult   *result;
	pval     *arg1;

 	if ( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}                            
 
    convert_to_long(arg1);
	 
	result = ada_get_result(list, arg1->value.lval);
	if (result == NULL){
		php3_error(E_WARNING, "Bad result index in ada_numfields");
		RETURN_FALSE;
	}
	RETURN_LONG(result->numcols);
}

void php3_Ada_fieldName(INTERNAL_FUNCTION_PARAMETERS)
{
	AdaResult       *result;
	pval     *arg1, *arg2;
	
	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_long(arg1);
	convert_to_long(arg2);
	
    result = ada_get_result(list, arg1->value.lval);
	if (result == NULL){
		php3_error(E_WARNING, "Bad result index");
		RETURN_FALSE;
	}
	
	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (arg2->value.lval > result->numcols){
		php3_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}
	
	if (arg2->value.lval < 1){
		php3_error(E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}
	
	RETURN_STRING(result->values[arg2->value.lval - 1].name,1)
}

void php3_Ada_fieldType(INTERNAL_FUNCTION_PARAMETERS)
{
	AdaResult	*result;
	char    	tmp[32];
	SWORD   	tmplen;
	pval     *arg1, *arg2;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);

	result = ada_get_result(list, arg1->value.lval);
	if (result == NULL){
		php3_error(E_WARNING, "Bad result index");
		RETURN_FALSE;
	}               

	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if (arg2->value.lval > result->numcols){
		php3_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	SQLColAttributes(result->stmt,arg2->value.lval, SQL_COLUMN_TYPE_NAME,
						tmp,31,&tmplen, NULL);
	RETURN_STRING(tmp,1);
}

void php3_Ada_fieldLen(INTERNAL_FUNCTION_PARAMETERS)
{
	AdaResult       *result;
	SDWORD  len;
	pval     *arg1, *arg2;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	
	result = ada_get_result(list, arg1->value.lval);

	if (result == NULL){
		php3_error(E_WARNING, "Bad result index");
		RETURN_FALSE;
	}                                                                

	if (result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if (arg2->value.lval > result->numcols){
		php3_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}
	SQLColAttributes(result->stmt, arg2->value.lval, SQL_COLUMN_PRECISION,
					NULL,0,NULL,&len);
	
	RETURN_LONG(len)
}

#if 0 /* XXX not used anywhere -jaakko */
void php3_Ada_fieldNum(INTERNAL_FUNCTION_PARAMETERS)
{
	int         field_ind;
	char        *fname;
	AdaResult   *result;
	int         i;
	pval     *arg1, *arg2;

	if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	fname = arg2->value.str.val;
	
	if (arg1->value.lval == 1){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	result = ada_get_result(list, arg1->value.lval);
	if (result == NULL){
		php3_error(E_WARNING, "Bad result index");
		RETURN_FALSE;
	}
	
	field_ind = -1;
	for(i = 0; i < result->numcols; i++){
		if (strcasecmp(result->values[i].name, fname) == 0)
			field_ind = i + 1;
		}
	if (field_ind == -1)
		RETURN_FALSE;
	RETURN_LONG(field_ind);
}
#endif /* XXX */

void php3_Ada_autocommit(INTERNAL_FUNCTION_PARAMETERS)
{
	HDBC	curr_conn;
	RETCODE rc;
	pval *arg1, *arg2;

 	if ( getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}                            
 
    convert_to_long(arg1);
	convert_to_long(arg2);

	curr_conn = ada_get_conn(list, arg1->value.lval);
	if (curr_conn == NULL){
		php3_error(E_WARNING, "Bad Adabas connection number");
		RETURN_FALSE;
	}

	rc = SQLSetConnectOption(curr_conn, SQL_AUTOCOMMIT, (arg2->value.lval)?
								SQL_AUTOCOMMIT_ON:SQL_AUTOCOMMIT_OFF);	
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		ada_sql_error(curr_conn, SQL_NULL_HSTMT, "Autocommit");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

void php3_Ada_transact(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	HDBC	curr_conn;
	RETCODE rc;
	pval *arg1;

 	if ( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}                            
 
    convert_to_long(arg1);

	curr_conn = ada_get_conn(list, arg1->value.lval);
	if (curr_conn == NULL){
		php3_error(E_WARNING, "Bad Adabas connection number");
		RETURN_FALSE;
	}

	rc = SQLTransact(php3_adabas_module.henv, curr_conn, (type)?SQL_COMMIT:SQL_ROLLBACK);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		ada_sql_error(curr_conn, SQL_NULL_HSTMT, "SQLTransact");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

void php3_Ada_commit(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_Ada_transact(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

void php3_Ada_rollback(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_Ada_transact(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

#endif /* HAVE_ADABAS */

