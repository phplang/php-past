/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   +----------------------------------------------------------------------+
 */

/* This file is based on the Adabas D extension.
 * Adabas D will no longer be supported as separate module.
 */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#ifndef MSVC5
#include "config.h"
#include "build-defs.h"
#endif
#include <fcntl.h>

#include "parser.h"
#include "internal_functions.h"
#include "php3_string.h"
#include "php3_unified_odbc.h"
#include "head.h"

#if HAVE_UODBC

#ifdef THREAD_SAFE

void *uodbc_mutex;
DWORD UODBCTls;
static int numthreads=0;

typedef struct uodbc_global_struct {
	uodbc_module php3_uodbc_module;
} uodbc_global_struct;

#define UODBC_GLOBAL(a) uodbc_globals->a

#define UODBC_TLS_VARS \
	uodbc_global_struct *uodbc_globals = TlsGetValue(UODBCTls); 

#else
uodbc_module php3_uodbc_module;
#define UODBC_GLOBAL(a) a
#define UODBC_TLS_VARS
#endif

function_entry uodbc_functions[] = {
	{"odbc_autocommit",	php3_uodbc_autocommit,	NULL},
	{"odbc_close",		php3_uodbc_close,		NULL},
	{"odbc_close_all",	php3_uodbc_close_all,	NULL},
	{"odbc_commit",		php3_uodbc_commit,		NULL},
	{"odbc_connect",	php3_uodbc_connect,		NULL},
	{"odbc_pconnect",	php3_uodbc_pconnect,	NULL},
	{"odbc_cursor",		php3_uodbc_cursor,		NULL},
	{"odbc_do",			php3_uodbc_do,			NULL},
	{"odbc_exec",		php3_uodbc_do,			NULL},
	{"odbc_prepare",	php3_uodbc_prepare,		NULL},
	{"odbc_execute",	php3_uodbc_execute,		NULL},
	{"odbc_fetch_row", 	php3_uodbc_fetch_row,	NULL},
	{"odbc_fetch_into",	php3_uodbc_fetch_into,	NULL},
	{"odbc_field_len", 	php3_uodbc_field_len,	NULL},
	{"odbc_field_name",	php3_uodbc_field_name,	NULL},
	{"odbc_field_type",	php3_uodbc_field_type,	NULL},
	{"odbc_free_result",php3_uodbc_free_result,	NULL},
	{"odbc_num_fields",	php3_uodbc_num_fields,	NULL},
	{"odbc_num_rows",	php3_uodbc_num_rows,	NULL},
	{"odbc_result",		php3_uodbc_result,		NULL},
	{"odbc_result_all",	php3_uodbc_result_all,	NULL},
	{"odbc_rollback",	php3_uodbc_rollback,	NULL},
/* for backwards compatibility with the older odbc module*/
	{"sqlconnect",		php3_uodbc_connect,		NULL},
	{"sqldisconnect",	php3_uodbc_close,		NULL},
	{"sqlfetch",		php3_uodbc_fetch_row,	NULL},
	{"sqlexecdirect",	php3_uodbc_do,			NULL},
	{"sqlgetdata",		php3_uodbc_result,		NULL},
	{"sqlfree",			php3_uodbc_free_result,	NULL},
	{"sqlrowcount",		php3_uodbc_num_rows,	NULL},
	{ NULL, NULL, NULL }
};

php3_module_entry uodbc_module_entry = {
    "uODBC", uodbc_functions, php3_minit_uodbc, php3_mshutdown_uodbc,
    php3_rinit_uodbc, NULL, php3_info_uodbc, 0, 0, 0, NULL
};


#if COMPILE_DL
php3_module_entry *get_module() { return &uodbc_module_entry; };
#if (WIN32|WINNT) && defined(THREAD_SAFE)

/*NOTE: You should have an odbc.def file where you
export DllMain*/
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
		if((UODBCTls=TlsAlloc())==0xFFFFFFFF){
			return 0;
		}
		break;    
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if(!TlsFree(UODBCTls)){
			return 0;
		}
		break;
    }
    return 1;
}
#endif
#endif

#include "list.h"

static void _free_uodbc_result(uodbc_result *res)
{
	int i;
	
	if(res){
		if(res->values) {
			for(i = 0; i < res->numcols; i++)
				efree(res->values[i].value);
			/*for(i = 0; i < result->numparams; i++)
				efree(res-> */

			efree(res->values);
			res->values = NULL;
		}
		if(res->stmt){
			SQLFreeStmt(res->stmt,SQL_DROP);
			res->stmt = NULL;
		}
		efree(res);
	}
}

static int _results_cleanup(list_entry *le)
{
	UODBC_TLS_VARS;

	if(le->type == UODBC_GLOBAL(php3_uodbc_module).le_result){
		uodbc_connection *conn = ((uodbc_result *) le->ptr)->conn_ptr;
		if(!conn->open && ((uodbc_result *) le->ptr)->stmt){
			SQLFreeStmt(((uodbc_result *) le->ptr)->stmt,SQL_DROP);
			((uodbc_result *) le->ptr)->stmt = NULL;
		}
	}
	return 0;
}

static void _close_uodbc_connection(uodbc_connection *conn)
{
	/* FIXME
	 * Closing a connection will fail if there are
	 * pending transactions
	 */
	UODBC_TLS_VARS;

	conn->open = 0;
	hash_apply(UODBC_GLOBAL(php3_uodbc_module).resource_list,
				(int (*)(void *))_results_cleanup);
	SQLDisconnect(conn->hdbc);
	SQLFreeConnect(conn->hdbc);
	efree(conn);
	UODBC_GLOBAL(php3_uodbc_module).num_links--;
}


static void _close_uodbc_pconnection(uodbc_connection *conn)
{
	UODBC_TLS_VARS;


	conn->open = 0;
	hash_apply(UODBC_GLOBAL(php3_uodbc_module).resource_plist,
				(int (*)(void *))_results_cleanup);

	SQLDisconnect(conn->hdbc);
	SQLFreeConnect(conn->hdbc);
	free(conn);

	UODBC_GLOBAL(php3_uodbc_module).num_links--;
	UODBC_GLOBAL(php3_uodbc_module).num_persistent--;
}


int php3_minit_uodbc(INITFUNCARG) 
{
#ifdef THREAD_SAFE
	uodbc_global_struct *uodbc_globals;
#if !COMPILE_DL
#if WIN32|WINNT
	CREATE_MUTEX(uodbc_mutex,"UODBC_TLS");
#endif
#endif
#if !COMPILE_DL
	SET_MUTEX(uodbc_mutex);
	numthreads++;
	if(numthreads==1){
	if((UODBCTls=TlsAlloc())==0xFFFFFFFF){
		FREE_MUTEX(uodbc_mutex);
		return 0;
	}}
	FREE_MUTEX(uodbc_mutex);
#endif
	uodbc_globals =
		(uodbc_global_struct *) LocalAlloc(LPTR, sizeof(uodbc_global_struct)); 
	TlsSetValue(UODBCTls, (void *) uodbc_globals);
#endif
	SQLAllocEnv(&UODBC_GLOBAL(php3_uodbc_module).henv);
	
	cfg_get_string("uodbc.default_db",
				   &UODBC_GLOBAL(php3_uodbc_module).defDB);
	cfg_get_string("uodbc.default_user",
				   &UODBC_GLOBAL(php3_uodbc_module).defUser);
	cfg_get_string("uodbc.default_pw",
				   &UODBC_GLOBAL(php3_uodbc_module).defPW);
	if (cfg_get_long("uodbc.allow_persistent",
					 &UODBC_GLOBAL(php3_uodbc_module).allow_persistent)
		== FAILURE) {
		UODBC_GLOBAL(php3_uodbc_module).allow_persistent = -1;
	}
	if (cfg_get_long("uodbc.max_persistent",
					 &UODBC_GLOBAL(php3_uodbc_module).max_persistent)
		== FAILURE) {
		UODBC_GLOBAL(php3_uodbc_module).max_persistent = -1;
	}
	if (cfg_get_long("uodbc.max_links",
					 &UODBC_GLOBAL(php3_uodbc_module).max_links)
		== FAILURE) {
		UODBC_GLOBAL(php3_uodbc_module).max_links = -1;
	}

	UODBC_GLOBAL(php3_uodbc_module).num_persistent = 0;

	UODBC_GLOBAL(php3_uodbc_module).le_result =
		register_list_destructors(_free_uodbc_result, NULL);
	UODBC_GLOBAL(php3_uodbc_module).le_conn =
		register_list_destructors(_close_uodbc_connection, NULL);
	UODBC_GLOBAL(php3_uodbc_module).le_pconn =
		register_list_destructors(NULL, _close_uodbc_pconnection);

	return SUCCESS;
}


int php3_rinit_uodbc(INITFUNCARG)
{
	UODBC_TLS_VARS;

	UODBC_GLOBAL(php3_uodbc_module).defConn = -1;
	UODBC_GLOBAL(php3_uodbc_module).num_links = 
			UODBC_GLOBAL(php3_uodbc_module).num_persistent;

	return SUCCESS;
}

int php3_mshutdown_uodbc(void)
{
#ifdef THREAD_SAFE
	uodbc_global_struct *uodbc_globals;
	uodbc_globals = TlsGetValue(UODBCTls); 
#endif
	SQLFreeEnv(UODBC_GLOBAL(php3_uodbc_module).henv);
#ifdef THREAD_SAFE
	if (uodbc_globals != 0) {
		LocalFree((HLOCAL) uodbc_globals);
	}
#if !COMPILE_DL
	SET_MUTEX(uodbc_mutex);
	numthreads--;
	if (!numthreads) {
		if (!TlsFree(UODBCTls)) {
			FREE_MUTEX(uodbc_mutex);
			return 0;
		}
	}
	FREE_MUTEX(uodbc_mutex);
#endif
#endif
	return SUCCESS;
}


void php3_info_uodbc(void)
{
#if !defined(COMPILE_DL) && defined(THREAD_SAFE)
	TLS_VARS;
#endif
#if HAVE_SOLID
	PUTS("Unified ODBC Support active (compiled with Solid)");
#elif HAVE_ADABAS
	PUTS("Unified ODBC Support active (compiled with Adabas D)");
#elif HAVE_IODBC && !(WIN32|WINNT)
	PUTS("Unified ODBC Support active (compiled with iODBC)");
#elif WIN32|WINNT
	PUTS("Unified ODBC Support active (compiled with win32 ODBC)");
#else
	PUTS("Unified ODBC Support active (compiled with unknown library)");
#endif
}


/*
 * List management functions
 */

int uodbc_add_result(HashTable *list,uodbc_result *result)
{
	UODBC_TLS_VARS;
	return php3_list_insert(result, UODBC_GLOBAL(php3_uodbc_module).le_result);
}

uodbc_result *uodbc_get_result(HashTable *list, int ind)
{
	uodbc_result *res;
	int type;
	UODBC_TLS_VARS;

	res = (uodbc_result*)php3_list_find(ind, &type);
	if (!res || type != UODBC_GLOBAL(php3_uodbc_module).le_result) {
		return NULL;
	}
	return res;
}

void uodbc_del_result(HashTable *list, int ind)
{
	uodbc_result *res;
	int type;
	UODBC_TLS_VARS;

	res = (uodbc_result *)php3_list_find(ind, &type);
	if (!res || type != UODBC_GLOBAL(php3_uodbc_module).le_result) {
		php3_error(E_WARNING,"Can't find result %d",ind);
		return;
	}
	php3_list_delete(ind);
}

#if 0
int uodbc_add_conn(HashTable *list, HDBC conn)
{
	UODBC_TLS_VARS;
	return php3_list_insert(conn, UODBC_GLOBAL(php3_uodbc_module).le_conn);
}

void uodbc_make_def_conn(HashTable *list)
{
	HDBC    new_conn;
	RETCODE rc;
	UODBC_TLS_VARS;

	SQLAllocConnect(UODBC_GLOBAL(php3_uodbc_module).henv, &new_conn);
	rc = SQLConnect(new_conn, UODBC_GLOBAL(php3_uodbc_module).defDB, SQL_NTS, UODBC_GLOBAL(php3_uodbc_module).defUser, 
					SQL_NTS, UODBC_GLOBAL(php3_uodbc_module).defPW, SQL_NTS);

	if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		uodbc_sql_error(new_conn, SQL_NULL_HSTMT, "SQLConnect"); 
	}else{
		UODBC_GLOBAL(php3_uodbc_module).defConn = uodbc_add_conn(list, new_conn);
	}
}
#endif

uodbc_connection *uodbc_get_conn(HashTable *list, int ind)
{
	uodbc_connection *conn = NULL;
	int type;
	HashTable *plist;
	UODBC_TLS_VARS;
	
	plist = UODBC_GLOBAL(php3_uodbc_module).resource_plist;

	conn = (uodbc_connection *)php3_list_find(ind, &type);
	if(conn && (type == UODBC_GLOBAL(php3_uodbc_module).le_conn ||
				type == UODBC_GLOBAL(php3_uodbc_module).le_pconn))
		return conn;

	conn = (uodbc_connection *)php3_plist_find(ind, &type);
	if(conn && (type == UODBC_GLOBAL(php3_uodbc_module).le_conn ||
				type == UODBC_GLOBAL(php3_uodbc_module).le_pconn))
		return conn;

	php3_error(E_WARNING,"Bad ODBC connection number (%d)",ind);
	return NULL;
}

void uodbc_sql_error(HDBC conn, HSTMT stmt, char *func)
{
    char	state[6];     /* Not used */
	SDWORD	error;        /* Not used */
	char	errormsg[255];
	SWORD	errormsgsize; /* Not used */
	UODBC_TLS_VARS;
	
	SQLError(UODBC_GLOBAL(php3_uodbc_module).henv, conn, stmt, state, &error, errormsg,
				sizeof(errormsg)-1, &errormsgsize);
	
	if(func)
		php3_error(E_WARNING, "Function %s", func);
	php3_error(E_WARNING, "SQL error: %s, SQL state %s", errormsg, state);	
}

/* Main User Functions */

void php3_uodbc_close_all(INTERNAL_FUNCTION_PARAMETERS)
{
	void *ptr;
	int type;
	int i, nument = hash_next_free_element(list);
	UODBC_TLS_VARS;

	for (i = 1; i < nument; i++) {
		ptr = php3_list_find(i, &type);
		if(ptr && (type == UODBC_GLOBAL(php3_uodbc_module).le_conn ||
				   type == UODBC_GLOBAL(php3_uodbc_module).le_pconn)){
			php3_list_delete(i);
		}
	}
}

int uodbc_bindcols(uodbc_result *result)
{
    int i;
    SWORD       colnamelen; /* Not used */
	SDWORD      displaysize,coltype;
	
    result->values = (uodbc_result_value *)emalloc(sizeof(uodbc_result_value)*result->numcols);

    if(result->values == NULL){
        php3_error(E_WARNING, "Out of memory");
        SQLFreeStmt(result->stmt, SQL_DROP);
        return 0;
    }
    
    for(i = 0; i < result->numcols; i++){
        SQLColAttributes(result->stmt, (UWORD)(i+1), SQL_COLUMN_NAME,
                         result->values[i].name,
                         sizeof(result->values[i].name),
                         &colnamelen,
                         0);
        SQLColAttributes(result->stmt, (UWORD)(i+1), SQL_COLUMN_DISPLAY_SIZE,
                         NULL,
                         0,
                         NULL,
                         &displaysize);
         /* Decision, whether a column will be output directly or returned into a
            PHP variable is based on two attributes:
            - Anything longer than 4096 Bytes (SQL_COLUMN_DISPLAY_SIZE)
            - and/or of datatype binary (SQL_BINARY,SQL_VARBINARY,SQL_LONGVARBINARY)
            will be sent direct to the client.
            Hint: SQL_BINARY and SQL_VARBINARY can be returned to PHP with the SQL
            function HEX(), e.g. SELECT HEX(SYSKEY) SYSKEY FROM MYTABLE.
            */
        if(displaysize > 4096){
            displaysize = 4096;
            result->values[i].passthru = 1;
        } else {
            result->values[i].passthru = 0;
        }

        SQLColAttributes(result->stmt, (UWORD)(i+1), SQL_COLUMN_TYPE,
                         NULL, 0, NULL, &coltype);
        
        switch(coltype){
            case SQL_BINARY:
            case SQL_VARBINARY:
            case SQL_LONGVARBINARY:
                result->values[i].passthru = 1;
                result->values[i].value = (char *)emalloc(displaysize);
                SQLBindCol(result->stmt, (UWORD)(i+1), SQL_C_BINARY,result->values[i].value,
                           displaysize, &result->values[i].vallen);
					break;
            default:
                result->values[i].value = (char *)emalloc(displaysize + 1);
                SQLBindCol(result->stmt, (UWORD)(i+1), SQL_C_CHAR, result->values[i].value,
                           displaysize + 1, &result->values[i].vallen);
                break;
        }
    }
    return 1;
}

void php3_uodbc_prepare(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE     *arg1, *arg2;
	int         conn;
	char        *query;
	uodbc_result   *result=NULL;
	uodbc_connection *curr_conn=NULL;
	RETCODE rc;

	if(getParameters(ht, 2, &arg1, &arg2) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	conn = arg1->value.lval;
	query = arg2->value.strval;

	curr_conn = uodbc_get_conn(list, conn);
	if(curr_conn == NULL){
		php3_error(E_WARNING, "Bad ODBC connection number (%d)", conn);
		RETURN_ZERO;
	}

	_php3_stripslashes(query);

	result = (uodbc_result *)emalloc(sizeof(uodbc_result));
	if(result == NULL){
		php3_error(E_WARNING, "Out of memory");
		RETURN_ZERO;
	}
	
	result->numparams = 0;
	
	rc = SQLAllocStmt(curr_conn->hdbc, &(result->stmt));
	if(rc == SQL_INVALID_HANDLE){
		efree(result);
		php3_error(E_WARNING, "SQLAllocStmt error 'Invalid Handle' in php3_uodbc_prepare");
		RETURN_ZERO;
	}

	if(rc == SQL_ERROR){
		uodbc_sql_error(curr_conn->hdbc, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_ZERO;
	}

	if((rc = SQLPrepare(result->stmt, query, SQL_NTS)) != SQL_SUCCESS){
		uodbc_sql_error(curr_conn->hdbc, result->stmt, "SQLPrepare");
		SQLFreeStmt(result->stmt, SQL_DROP);
		RETURN_ZERO;
	}
	
	SQLNumParams(result->stmt, &(result->numparams));
    SQLNumResultCols(result->stmt, &(result->numcols));

	if(result->numcols > 0){
        if(!uodbc_bindcols(result)){
			efree(result);
            RETURN_ZERO;
		}
	}else{
		result->values = NULL;
	}
	result->conn_ptr = curr_conn;
	result->fetched = 0;
	RETURN_LONG(uodbc_add_result(list, result));	
}

/*
 * Execute prepared SQL statement. Supports only input parameters.
 */

void php3_uodbc_execute(INTERNAL_FUNCTION_PARAMETERS)
{ 
    YYSTYPE *arg1, *arg2, arr, *tmp;
    typedef struct params_t{
		SDWORD vallen;
		int fp;
	} params_t;
	params_t *params = NULL;
	char *filename;
   	SWORD sqltype, scale, nullable;
	UDWORD precision;
   	uodbc_result   *result=NULL;
	int res_ind, numArgs, i, ne;
	RETCODE rc;
	
	numArgs = ARG_COUNT(ht);
	if(numArgs == 1){
		if(getParameters(ht, 1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	}else{
		if(getParameters(ht, 2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;

        arr = *arg2;
        if(arr.type != IS_ARRAY) {
            php3_error(E_WARNING, "No array passed to odbc_execute()");
            return;
        }
    }
    
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	result = uodbc_get_result(list, res_ind);
	if (result == NULL) {
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}
	
	if (result->numparams > 0 && numArgs == 1) {
		php3_error(E_WARNING, "No parameters to SQL statement given");
		RETURN_FALSE;
	}

    if(result->numparams > 0){
		if((ne = hash_num_elements(arr.value.ht)) < result->numparams){
			php3_error(E_WARNING,"Not enough parameters (%d should be %d) given", ne, result->numparams);
			RETURN_FALSE;
		}

        yystype_copy_constructor(arg2);
        hash_internal_pointer_reset(arr.value.ht);
        params = (params_t *)emalloc(sizeof(params_t) * result->numparams);
		
		for(i = 1; i <= result->numparams; i++){
            if(hash_get_current_data(arr.value.ht, (void **) &tmp) == FAILURE) {
                php3_error(E_WARNING,"Error getting parameter");
                SQLFreeStmt(result->stmt,SQL_RESET_PARAMS);
				efree(params);
                RETURN_FALSE;
            }
            convert_to_string(tmp);
			if(tmp->type != IS_STRING){
				php3_error(E_WARNING,"Error converting parameter");
				SQLFreeStmt(result->stmt,SQL_RESET_PARAMS);
				hash_destroy(arr.value.ht);
				efree(arr.value.ht);
				efree(params);
				RETURN_FALSE;
			}
			
            SQLDescribeParam(result->stmt,(UWORD)i,&sqltype,&precision,&scale,&nullable);
			params[i-1].vallen = tmp->strlen;
			params[i-1].fp = -1;

			if(sqltype == SQL_VARBINARY || sqltype == SQL_LONGVARBINARY){
				if(tmp->value.strval[0] != '\''){
					php3_error(E_WARNING,"No Filename for binary parameter");
					SQLFreeStmt(result->stmt,SQL_RESET_PARAMS);
					hash_destroy(arr.value.ht);
					efree(arr.value.ht);
					efree(params);
					RETURN_FALSE;
				}

				filename = &tmp->value.strval[1];
				filename[tmp->strlen - 2] = '\0';

                if((params[i-1].fp = open(filename,O_RDONLY)) == -1){
					php3_error(E_WARNING,"Can´t open file %s",filename);
					SQLFreeStmt(result->stmt,SQL_RESET_PARAMS);
					for(i = 0; i < result->numparams; i++){
						if(params[i].fp != -1)
							close(params[i].fp);
					}
					hash_destroy(arr.value.ht);
					efree(arr.value.ht);
					efree(params);
					RETURN_FALSE;
					}

				params[i-1].vallen = SQL_LEN_DATA_AT_EXEC(0);

				rc = SQLBindParameter(result->stmt, (UWORD)i, SQL_PARAM_INPUT,
										SQL_C_BINARY, sqltype, precision, scale,
										(void *)params[i-1].fp, 0, &params[i-1].vallen);
			}else{
				rc = SQLBindParameter(result->stmt, (UWORD)i, SQL_PARAM_INPUT,
										SQL_C_CHAR, sqltype, precision, scale,
										tmp->value.strval, 0, &params[i-1].vallen);
			}
			hash_move_forward(arr.value.ht);
		}
	}	
	rc = SQLExecute(result->stmt);

	if(rc == SQL_NEED_DATA){
		char buf[4096];
		int fp, nbytes;
		while(rc == SQL_NEED_DATA){
			rc = SQLParamData(result->stmt, (PTR FAR *)&fp);
			if(rc == SQL_NEED_DATA){
				while((nbytes = read(fp, &buf, 4096)) > 0)
					SQLPutData(result->stmt,(UCHAR FAR*) &buf, nbytes);
			}
		}
	}else{
		if(rc != SQL_SUCCESS){
			uodbc_sql_error(result->conn_ptr->hdbc, result->stmt, "SQLExecute");
			efree(arr.value.ht);
			efree(params);
			RETVAL_FALSE;
		}
	}	
	
	if(result->numparams > 0){
		SQLFreeStmt(result->stmt, SQL_RESET_PARAMS);
		for(i = 0; i < result->numparams; i++){
			if(params[i].fp != -1)
				close(params[i].fp);
		}
		hash_destroy(arr.value.ht);
		efree(arr.value.ht);
		efree(params);
	}

	if(rc == SQL_SUCCESS){
		RETVAL_TRUE;
	}
}

/* odbc_cursor simply returns a cursor name for the given stmt
 * Adabas automagically generates cursor names, other drivers may not
 */

void php3_uodbc_cursor(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE     *arg1;
	int			res_ind;
	SWORD		len, max_len;
	char		*cursorname;
   	uodbc_result *result;
	RETCODE 	rc;

	if(getParameters(ht, 1, &arg1) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	result = uodbc_get_result(list, res_ind);
	if (result == NULL) {
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}
	rc = SQLGetInfo(result->conn_ptr->hdbc,SQL_MAX_CURSOR_NAME_LEN,
					(void *)&max_len,0,&len);
	if(rc != SQL_SUCCESS){
		RETURN_FALSE;
	}
	
	if(max_len > 0){
		cursorname = emalloc(max_len + 1);
		if (cursorname == NULL){
			php3_error(E_WARNING,"Out of memory");
			RETURN_FALSE;
		}
		rc = SQLGetCursorName(result->stmt,cursorname,(SWORD)max_len,&len);
		if(rc != SQL_SUCCESS){
			char    state[6];     /* Not used */
	 		SDWORD  error;        /* Not used */
			char    errormsg[255];
			SWORD   errormsgsize; /* Not used */
			UODBC_TLS_VARS;

			SQLError(UODBC_GLOBAL(php3_uodbc_module).henv, result->conn_ptr->hdbc,
						result->stmt, state, &error, errormsg,
						sizeof(errormsg)-1, &errormsgsize);
			if(!strncmp(state,"S1015",5)){
				sprintf(cursorname,"php3_curs_%d", (int)result->stmt);
				if(SQLSetCursorName(result->stmt,cursorname,SQL_NTS) != SQL_SUCCESS){
					uodbc_sql_error(result->conn_ptr->hdbc,result->stmt,
										"SQLSetCursorName");
					RETVAL_FALSE;
				}else{
					RETVAL_STRING(cursorname);
				}
			}else{
				php3_error(E_WARNING, "SQL error: %s, SQL state %s", errormsg, state);
				RETVAL_FALSE;
			}
		}else{
			RETVAL_STRING(cursorname);
		}
		efree(cursorname);
	}else{
		RETVAL_FALSE;
	}
}

void php3_uodbc_do(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE 	*arg1, *arg2;
	int         conn;
	char        *query;
	uodbc_result   *result=NULL;
	uodbc_connection *curr_conn=NULL;
	RETCODE     rc;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      scrollopts;
#endif

	if(getParameters(ht, 2, &arg1, &arg2) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_string(arg2);
	conn = arg1->value.lval;
	query = arg2->value.strval;
	
	curr_conn = uodbc_get_conn(list, conn);
	if(curr_conn == NULL){
		php3_error(E_WARNING, "Bad ODBC connection number (%d)", conn);
		RETURN_ZERO;
	}

	_php3_stripslashes(query);
	
	result = (uodbc_result *)emalloc(sizeof(uodbc_result));
	if(result == NULL){
		php3_error(E_WARNING, "Out of memory");
		RETURN_ZERO;
	}

	rc = SQLAllocStmt(curr_conn->hdbc, &(result->stmt));
	if(rc == SQL_INVALID_HANDLE){
		php3_error(E_WARNING, "SQLAllocStmt error 'Invalid Handle' in php3_uodbc_do");
		efree(result);
		RETURN_ZERO;
	}

	if(rc == SQL_ERROR){
		uodbc_sql_error(curr_conn->hdbc, SQL_NULL_HSTMT, "SQLAllocStmt");
		efree(result);
		RETURN_ZERO;
	}
	
#if HAVE_SQL_EXTENDED_FETCH
	/* Solid doesn't have ExtendedFetch, if DriverManager is used, get Info,
	   whether Driver supports ExtendedFetch */
	rc = SQLGetInfo(curr_conn->hdbc, SQL_FETCH_DIRECTION, (void *) &scrollopts, sizeof(scrollopts), NULL);
	if(rc == SQL_SUCCESS){
		if((result->fetch_abs = (scrollopts & SQL_FD_FETCH_ABSOLUTE))){
			/* Try to set CURSOR_TYPE to dynamic. Driver will replace this with other
			   type if not possible.
			 */
			if(SQLSetStmtOption(result->stmt, SQL_CURSOR_TYPE, SQL_CURSOR_DYNAMIC)
				== SQL_ERROR){
				uodbc_sql_error(curr_conn->hdbc, result->stmt, " SQLSetStmtOption");
				SQLFreeStmt(result->stmt, SQL_DROP);
				efree(result);
				RETURN_ZERO;
			}
		}
	}else{
		result->fetch_abs = 0;
	}
#endif

	rc = SQLExecDirect(result->stmt, query, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		/* XXX FIXME we should really check out SQLSTATE with SQLError
		 * in case rc is SQL_SUCCESS_WITH_INFO here.
		 */
		uodbc_sql_error(curr_conn->hdbc, result->stmt, "SQLExecDirect"); 
		SQLFreeStmt(result->stmt, SQL_DROP);
		efree(result);
		RETURN_ZERO;
	}

	SQLNumResultCols(result->stmt, &(result->numcols));
	
	/* For insert, update etc. cols == 0 */
	if(result->numcols > 0){
        if(!uodbc_bindcols(result)){
			efree(result);
            RETURN_ZERO;
		}
	}else{
		result->values = NULL;
	}
	result->conn_ptr = curr_conn;
	result->fetched = 0;
	RETURN_LONG(uodbc_add_result(list, result));
}

void php3_uodbc_fetch_into(INTERNAL_FUNCTION_PARAMETERS)
{
	int         res_ind, numArgs, i;
	uodbc_result   *result;
	RETCODE     rc;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      crow;
	UWORD       RowStatus[1];
	SDWORD      rownum = -1;
	YYSTYPE     *arg1, *arg2, *arr, tmp;
	
	numArgs = ARG_COUNT(ht);

	switch(numArgs){
		case 2:
			if(getParameters(ht, 2, &arg1, &arr) == FAILURE)
				WRONG_PARAM_COUNT;
			break;
		case 3:
			if(getParameters(ht, 3, &arg1, &arg2, &arr) == FAILURE)
				WRONG_PARAM_COUNT;
			convert_to_long(arg2);
			rownum = arg2->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (!ParameterPassedByReference(ht, numArgs)){
		php3_error(E_WARNING, "Array not passed by reference in call to odbc_fetch_into()");
		return;
	}
#else
	YYSTYPE     *arg1, *arr, tmp;

	numArgs = ARG_COUNT(ht);

	if (numArgs != 2 || getParameters(ht, 2, &arg1, &arr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (!ParameterPassedByReference(ht, numArgs)){
		php3_error(E_WARNING, "Array not passed by reference in call to odbc_fetch_into()");
		return;
	}
#endif				
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;

	/* check result */
	result = uodbc_get_result(list, res_ind);
	if(result == NULL){
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}

	if(result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if(arr->type != IS_ARRAY){
		if (array_init(arr) == FAILURE){
			php3_error(E_WARNING, "Can't convert to type Array");
			return;
		}
	}

#if HAVE_SQL_EXTENDED_FETCH
	if(result->fetch_abs){
		if(rownum > 0)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		else
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	}else
#endif
		
		rc = SQLFetch(result->stmt);

	if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
		RETURN_FALSE;

#if HAVE_SQL_EXTENDED_FETCH
	if(rownum > 0 && result->fetch_abs)
		result->fetched = rownum;
	else
#endif
		result->fetched++;

	for(i = 0; i < result->numcols; i++){
		if(result->values[i].value != NULL &&
			result->values[i].vallen != SQL_NO_TOTAL &&
			result->values[i].passthru == 0){
			tmp.type = IS_STRING;
			tmp.strlen = strlen(result->values[i].value);
			tmp.value.strval = estrndup(result->values[i].value,tmp.strlen);
		}else{
			tmp.type = IS_STRING;
			tmp.strlen = 0;
			tmp.value.strval = empty_string;
		}
		hash_index_update(arr->value.ht, i, (void *) &tmp, sizeof(YYSTYPE), NULL);
	}
	RETURN_LONG(result->numcols);	
}

void php3_uodbc_fetch_row(INTERNAL_FUNCTION_PARAMETERS)
{
	int         res_ind, numArgs;
	SDWORD      rownum = 1;
	uodbc_result   *result;
	RETCODE     rc;
	YYSTYPE		*arg1, *arg2;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      crow;
	UWORD       RowStatus[1];
#endif

	numArgs = ARG_COUNT(ht);
	if(numArgs ==  1){
		if(getParameters(ht, 1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	}else{
		if(getParameters(ht, 2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
		convert_to_long(arg2);
		rownum = arg2->value.lval;
	}
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	result = uodbc_get_result(list, res_ind);
	if (result == NULL) {
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}
	
	if (result->numcols == 0) {
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

#if HAVE_SQL_EXTENDED_FETCH
    if(result->fetch_abs){
		if(numArgs > 1)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_ABSOLUTE,rownum,&crow,RowStatus);
		else
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	}else
#endif
		rc = SQLFetch(result->stmt);

	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		RETURN_FALSE;
	}
	
	if (numArgs > 1) {
		result->fetched = rownum;
	} else {
		result->fetched++;
	}
	
	RETURN_TRUE;
}	

void php3_uodbc_result(INTERNAL_FUNCTION_PARAMETERS)
{
	char        *field;
	int         res_ind;
	int         field_ind;
	uodbc_result   *result;
	int         i;
	RETCODE     rc;
	YYSTYPE		*arg1, *arg2;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      crow;
	UWORD       RowStatus[1];
#endif
#if !defined(COMPILE_DL) && defined(THREAD_SAFE)
	TLS_VARS;
#endif

	field_ind = -1;
	field = NULL;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2 , &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(arg1);
	res_ind = arg1->value.lval;

	if(arg2->type == IS_STRING)
		field = arg2->value.strval;
	else{
		convert_to_long(arg2);
		field_ind = arg2->value.lval - 1;
	}
	
	/* check result */
	result = uodbc_get_result(list, res_ind);
	if(result == NULL){
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}
	
	if((result->numcols == 0)){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
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
			php3_error(E_WARNING, "Field %s not found", field);
			RETURN_FALSE;
		}
	}else{
		/* check for limits of field_ind if the field parameter was an int */
		if(field_ind >= result->numcols || field_ind < 0){
			php3_error(E_WARNING, "Field index is larger than the number of fields");
			RETURN_FALSE;
		}
	}

	if(result->fetched == 0){
		/* User forgot to call odbc_fetchrow(), let's do it here */
#if HAVE_SQL_EXTENDED_FETCH
		if(result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		else
#endif
			rc = SQLFetch(result->stmt);

		if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
			RETURN_FALSE;
		
		result->fetched++;
	}
       
	if (result->values[field_ind].vallen == SQL_NULL_DATA){
		RETURN_FALSE
	} else {
		if (result->values[field_ind].value != NULL &&
			result->values[field_ind].vallen != SQL_NO_TOTAL &&
			result->values[field_ind].passthru == 0){
			RETURN_STRING(result->values[field_ind].value);	
		} else {
			char buf[4096];
			SDWORD fieldsize = 4096;

			/* Make sure that the Header is sent */ 
			if (!php3_header(0, NULL))
				RETURN_TRUE;  /* don't output anything on a HEAD request */

			if (result->values[field_ind].vallen != SQL_NO_TOTAL) {
				fieldsize = result->values[field_ind].vallen;
			}
			/* Output what's already in the field */
			PHPWRITE(result->values[field_ind].value, fieldsize);
			
			if (result->values[field_ind].vallen != SQL_NO_TOTAL) {
				RETURN_TRUE;
			}

			/* Call SQLGetData until SQL_SUCCESS is returned */
			while (1) {
				rc = SQLGetData(result->stmt, (UWORD)(field_ind + 1), SQL_C_BINARY,
								buf, 4096, &fieldsize);

				if (rc == SQL_ERROR) {
					uodbc_sql_error(result->conn_ptr->hdbc, result->stmt, "SQLGetData");
					RETURN_FALSE;
				}

				if (rc == SQL_SUCCESS_WITH_INFO) {
					fieldsize = 4096;
				}
				PHPWRITE(buf, fieldsize);
				
				if (rc == SQL_SUCCESS) { /* no more data avail */
					break;
				}
			}
			RETURN_TRUE;
		}
	}
}

void php3_uodbc_result_all(INTERNAL_FUNCTION_PARAMETERS)
{
	int         res_ind, numArgs;
	uodbc_result   *result;
	int         i;
	RETCODE     rc;
	YYSTYPE     *arg1, *arg2;
#if HAVE_SQL_EXTENDED_FETCH
	UDWORD      crow;
	UWORD       RowStatus[1];
#endif

	numArgs = ARG_COUNT(ht);
	if(numArgs ==  1){
		if(getParameters(ht, 1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	}else{
		if(getParameters(ht, 2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
	}
				
	convert_to_long(arg1);
	res_ind = arg1->value.lval;
	
	/* check result */
	result = uodbc_get_result(list, res_ind);
	if(result == NULL){
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}
	
	if(result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
#if HAVE_SQL_EXTENDED_FETCH
	if(result->fetch_abs)
		rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
	else
#endif	
		rc = SQLFetch(result->stmt);

	if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		php3_printf("<h2>No rows found</h2>\n");
		RETURN_LONG(0);
	}
	
	/* Start table tag */
	if(numArgs == 1){
		php3_printf("<table><tr>");
	}else{
		convert_to_string(arg2);	
		php3_printf("<table %s ><tr>",arg2->value.strval); 
	}
	
	for(i = 0; i < result->numcols; i++)
		php3_printf("<th>%s</th>", result->values[i].name);

	php3_printf("</tr>\n");

	while(rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO){
		result->fetched++;
		php3_printf("<tr>");
		for(i = 0; i < result->numcols; i++){
			if(result->values[i].vallen == SQL_NULL_DATA)
				php3_printf("<td> </td>");
			else{
				if(result->values[i].passthru == 0)
					php3_printf("<td>%s</td>", result->values[i].value);
					/* _php3_addslashes(result->values[i].value,0)); */
							
				else 
					php3_printf("<td>Unsupp. data type</td>");
			}
		}
		php3_printf("</tr>\n");

#if HAVE_SQL_EXTENDED_FETCH
		if(result->fetch_abs)
			rc = SQLExtendedFetch(result->stmt,SQL_FETCH_NEXT,1,&crow,RowStatus);
		else
#endif
			rc = SQLFetch(result->stmt);		
	}
	php3_printf("</table>\n");
	RETURN_LONG(result->fetched);
}

void php3_uodbc_free_result(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg1;
	
	if( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	uodbc_del_result(list, arg1->value.lval);
	RETURN_TRUE;
}

void php3_uodbc_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_uodbc_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

void php3_uodbc_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_uodbc_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

/* Persistent connections: two list-types le_pconn, le_conn and a plist
 * where hashed connection info is stored together with index pointer to
 * the actual link of type le_pconn in the list. Only persistent 
 * connections get hashed up. Normal connections use existing pconnections.
 * Maybe this has to change with regard to transactions on pconnections?
 * Possibly set autocommit to on on request shutdown.
 */
void php3_uodbc_do_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	char    *db = NULL;
	char    *uid = NULL;
	char    *pwd = NULL;
	YYSTYPE *arg1, *arg2, *arg3;
	uodbc_connection *db_conn;
	RETCODE rc;
	list_entry le, *index_ptr;
	char *hashed_details;
	int hashed_len, len, id;
	UODBC_TLS_VARS;

	UODBC_GLOBAL(php3_uodbc_module).resource_list = list;
	UODBC_GLOBAL(php3_uodbc_module).resource_plist = plist;
	
	if(getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(arg1);
	convert_to_string(arg2);
	convert_to_string(arg3);

	db = arg1->value.strval;
	uid = arg2->value.strval;
	pwd = arg3->value.strval;

	if (!UODBC_GLOBAL(php3_uodbc_module).allow_persistent) {
		persistent = 0;
	}

	if (UODBC_GLOBAL(php3_uodbc_module).max_links != -1 &&
		UODBC_GLOBAL(php3_uodbc_module).num_links >=
		UODBC_GLOBAL(php3_uodbc_module).max_links) {
		php3_error(E_WARNING, "uODBC: Too many open links (%d)",
		UODBC_GLOBAL(php3_uodbc_module).num_links);
		RETURN_FALSE;
	}

	/* the user requested a persistent connection */
	if (persistent && 
			UODBC_GLOBAL(php3_uodbc_module).max_persistent != -1 &&
			UODBC_GLOBAL(php3_uodbc_module).num_persistent >=
			UODBC_GLOBAL(php3_uodbc_module).max_persistent) {
		php3_error(E_WARNING,"uODBC: Too many open persistent links (%d)",
					UODBC_GLOBAL(php3_uodbc_module).num_persistent);
		RETURN_FALSE;
	}

	len = strlen(db) + strlen(uid) + strlen(pwd) + 9; 
	hashed_details = emalloc(len);

	if (hashed_details == NULL) {
		php3_error(E_WARNING, "Out of memory");
		RETURN_FALSE;
	}

	hashed_len = sprintf(hashed_details, "uodbc_%s_%s_%s", db, uid, pwd);

	/* try to find if we already have this link in our persistent list,
	 * no matter if it is to be persistent or not
	 */

	if (hash_find(plist, hashed_details, hashed_len + 1,
		  (void **) &index_ptr) == FAILURE) {
		/* the link is not in the persistent list */
		list_entry new_le, new_index_ptr;

		if(persistent)
			db_conn = (uodbc_connection *)malloc(sizeof(uodbc_connection));
		else
			db_conn = (uodbc_connection *)emalloc(sizeof(uodbc_connection));

		SQLAllocConnect(UODBC_GLOBAL(php3_uodbc_module).henv, &db_conn->hdbc);
		rc = SQLConnect(db_conn->hdbc, db, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);

		if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
			uodbc_sql_error(db_conn->hdbc, SQL_NULL_HSTMT, "SQLConnect");
			SQLFreeConnect(db_conn->hdbc);
			if(persistent)
				free(db_conn);
			else
				efree(db_conn);
			RETURN_FALSE;
		}
		db_conn->open = 1;
		if(persistent){
			new_le.type = UODBC_GLOBAL(php3_uodbc_module).le_pconn;
			new_le.ptr = db_conn;
			return_value->value.lval = 
				php3_plist_insert(db_conn, UODBC_GLOBAL(php3_uodbc_module).le_pconn);
			new_index_ptr.ptr = (void *) return_value->value.lval;
			new_index_ptr.type = le_index_ptr;
			if (hash_update(plist,hashed_details,hashed_len + 1,(void *) &new_index_ptr,
					sizeof(list_entry),NULL)==FAILURE) {
				SQLDisconnect(db_conn->hdbc);
				SQLFreeConnect(db_conn->hdbc);
				free(db_conn);
				efree(hashed_details);
				RETURN_FALSE;
			}
			UODBC_GLOBAL(php3_uodbc_module).num_persistent++;
		}else{
			/* non persistent, simply add to list */
			return_value->value.lval = php3_list_insert(db_conn, UODBC_GLOBAL(php3_uodbc_module).le_conn);
		}

		UODBC_GLOBAL(php3_uodbc_module).num_links++;

	} else {
		void *ptr;
		int type;
		/* the link is already in the persistent list */
		if (le.type != le_index_ptr) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		id = (int) index_ptr->ptr;
		ptr = php3_plist_find(id, &type);

		/* FIXME test if the connection is dead */
		if (ptr && (type ==  UODBC_GLOBAL(php3_uodbc_module).le_conn ||
					type == UODBC_GLOBAL(php3_uodbc_module).le_pconn)){
			return_value->value.lval = id;
		}else{
			efree(hashed_details);
			RETURN_FALSE;
		}
	}
	efree(hashed_details);
	return_value->type = IS_LONG;
}

void php3_uodbc_close(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg1;
	HDBC conn;
	int type, ind;
	UODBC_TLS_VARS;

    if(getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	ind = (int)arg1->value.lval;
	conn = (HDBC)php3_list_find(ind, &type);
	if (!conn ||
		(type != UODBC_GLOBAL(php3_uodbc_module).le_conn &&
		 type != UODBC_GLOBAL(php3_uodbc_module).le_pconn)) {
		return;
	}
	php3_list_delete(ind);
}

void php3_uodbc_num_rows(INTERNAL_FUNCTION_PARAMETERS)
{
	uodbc_result   *result;
	SDWORD      rows;
	YYSTYPE 	*arg1;
	
	if( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}            

	convert_to_long(arg1);

	result = uodbc_get_result(list, arg1->value.lval);
	if(result == NULL){
		php3_error(E_WARNING, "Bad result index in odbc_numrows");
		RETURN_FALSE;
	}

	SQLRowCount(result->stmt, &rows);
	RETURN_LONG(rows);
}

void php3_uodbc_num_fields(INTERNAL_FUNCTION_PARAMETERS)
{
	uodbc_result   *result;
	YYSTYPE     *arg1;

 	if( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}                            
 
    convert_to_long(arg1);
	 
	result = uodbc_get_result(list, arg1->value.lval);
	if(result == NULL){
		php3_error(E_WARNING, "Bad result index in odbc_numfields");
		RETURN_FALSE;
	}
	RETURN_LONG(result->numcols);
}

void php3_uodbc_field_name(INTERNAL_FUNCTION_PARAMETERS)
{
	uodbc_result       *result;
	YYSTYPE     *arg1, *arg2;
	
	if(getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
		
	convert_to_long(arg1);
	convert_to_long(arg2);
	
    result = uodbc_get_result(list, arg1->value.lval);
	if(result == NULL){
		php3_error(E_WARNING, "Bad result index");
		RETURN_FALSE;
	}
	
	if(result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if(arg2->value.lval > result->numcols){
		php3_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}
	
	if(arg2->value.lval < 1){
		php3_error(E_WARNING, "Field numbering starts at 1");
		RETURN_FALSE;
	}
	
	RETURN_STRING(result->values[arg2->value.lval - 1].name)
}

void php3_uodbc_field_type(INTERNAL_FUNCTION_PARAMETERS)
{
	uodbc_result	*result;
	char    	tmp[32];
	SWORD   	tmplen;
	YYSTYPE     *arg1, *arg2;

	if(getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);

	result = uodbc_get_result(list, arg1->value.lval);
	if(result == NULL){
		php3_error(E_WARNING, "Bad result index");
		RETURN_FALSE;
	}               

	if(result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}

	if(arg2->value.lval > result->numcols){
		php3_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}

	SQLColAttributes(result->stmt, (UWORD)arg2->value.lval,
					 SQL_COLUMN_TYPE_NAME, tmp, 31, &tmplen, NULL);
	RETURN_STRING(tmp)
}

void php3_uodbc_field_len(INTERNAL_FUNCTION_PARAMETERS)
{
	uodbc_result       *result;
	SDWORD  len;
	YYSTYPE     *arg1, *arg2;

	if(getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	
	result = uodbc_get_result(list, arg1->value.lval);

	if(result == NULL){
		php3_error(E_WARNING, "Bad result index");
		RETURN_FALSE;
	}                                                                

	if(result->numcols == 0){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	
	if(arg2->value.lval > result->numcols){
		php3_error(E_WARNING, "Field index larger than number of fields");
		RETURN_FALSE;
	}
	SQLColAttributes(result->stmt, (UWORD)arg2->value.lval, 
					 SQL_COLUMN_PRECISION, NULL, 0, NULL, &len);
	
	RETURN_LONG(len)
}

#if 0 /* XXX not used anywhere -jaakko */
void php3_uodbc_field_num(INTERNAL_FUNCTION_PARAMETERS)
{
	int         field_ind;
	char        *fname;
	uodbc_result *result;
	int         i;
	YYSTYPE     *arg1, *arg2;

	if(getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	fname = arg2->value.strval;
	
	if(arg1->value.lval == 1){
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	result = uodbc_get_result(list, arg1->value.lval);
	if(result == NULL){
		php3_error(E_WARNING, "Bad result index");
		RETURN_FALSE;
	}
	
	field_ind = -1;
	for(i = 0; i < result->numcols; i++){
		if(strcasecmp(result->values[i].name, fname) == 0)
			field_ind = i + 1;
		}
	if(field_ind == -1)
		RETURN_FALSE;
	RETURN_LONG(field_ind);
}
#endif /* XXX */

void php3_uodbc_autocommit(INTERNAL_FUNCTION_PARAMETERS)
{
	uodbc_connection *curr_conn;
	RETCODE rc;
	YYSTYPE *arg1, *arg2;

 	if( getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}                            
 
    convert_to_long(arg1);
	convert_to_long(arg2);

	curr_conn = uodbc_get_conn(list, arg1->value.lval);
	if(curr_conn == NULL){
		php3_error(E_WARNING, "Bad ODBC connection number (%d)", arg1->value.lval);
		RETURN_FALSE;
	}

	rc = SQLSetConnectOption(curr_conn->hdbc, SQL_AUTOCOMMIT, (arg2->value.lval)?
								SQL_AUTOCOMMIT_ON:SQL_AUTOCOMMIT_OFF);	
	if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		uodbc_sql_error(curr_conn->hdbc, SQL_NULL_HSTMT, "Autocommit");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

void php3_uodbc_transact(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	uodbc_connection *curr_conn;
	RETCODE rc;
	YYSTYPE *arg1;
	UODBC_TLS_VARS;

 	if( getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}                            
 
    convert_to_long(arg1);

	curr_conn = uodbc_get_conn(list, arg1->value.lval);
	if(curr_conn == NULL){
		php3_error(E_WARNING, "Bad ODBC connection number (%d)", arg1->value.lval);
		RETURN_FALSE;
	}

	rc = SQLTransact(UODBC_GLOBAL(php3_uodbc_module).henv, curr_conn->hdbc, (UWORD)((type)?SQL_COMMIT:SQL_ROLLBACK));
	if(rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO){
		uodbc_sql_error(curr_conn->hdbc, SQL_NULL_HSTMT, "SQLTransact");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}

void php3_uodbc_commit(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_uodbc_transact(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

void php3_uodbc_rollback(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_uodbc_transact(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

#endif /* HAVE_UODBC */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
