/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   |                                                                      |
   | Initial work sponsored by Thies Arntzen <thies@digicol.de> of        |
   | Digital Collections, http://www.digicol.de/                          |
   +----------------------------------------------------------------------+
 */

/* $Id: oci8.c,v 1.19 1998/10/01 18:24:13 ssb Exp $ */

#include "php.h"
#include "internal_functions.h"
#include "php3_oci8.h"

#if HAVE_OCI8

# include "php3_list.h"
# include "build-defs.h"
# include "snprintf.h"
# include "head.h"

/* These {{{ and }}} lines are folding marks for Emacs, in case you
 * wondered.
 */

/* TODO list:
 *
 * - Error mode (print or shut up?)
 * - LOB writing, reading and manipulation
 * - returning refcursors as statement handles
 * - ability to refer to columns by name as well as number
 * - OCIPasswordChange()
 * - Prefetching control
 *
 */

/* {{{ thread safety stuff */

# ifdef THREAD_SAFE
#  define OCI8_GLOBAL(a) oci8_globals->a
#  define OCI8_TLS_VARS \
	oci8_global_struct *oci8_globals = TlsGetValue(OCI8Tls); 

void *oci8_mutex;
DWORD OCI8Tls;
static int numthreads=0;

typedef struct oci8_global_struct {
	oci8_module php3_oci8_module;
} oci8_global_struct;

# else /* !defined(THREAD_SAFE) */
#  define OCI8_GLOBAL(a) a
#  define OCI8_TLS_VARS

oci8_module php3_oci8_module;

# endif /* defined(THREAD_SAFE) */

/* }}} */
/* {{{ dynamically loadable module stuff */

# if COMPILE_DL

php3_module_entry *get_module() { return &oci8_module_entry; };

#  if (WIN32|WINNT) && defined(THREAD_SAFE)

/* NOTE: You should have an oci8.def file where you export DllMain */
BOOL WINAPI DllMain(HANDLE hModule, DWORD  ul_reason_for_call, 
					LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			if ((OCI8Tls = TlsAlloc()) == 0xFFFFFFFF){
				return 0;
			}
			break;    
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			if (!TlsFree(OCI8Tls)) {
				return 0;
			}
			break;
    }
    return 1;
}

#  endif /* thread safe on Windows */
# endif /* COMPILE_DL */

/* }}} */
/* {{{ startup/shutdown/info/internal function prototypes */

int php3_minit_oci8(INIT_FUNC_ARGS);
int php3_rinit_oci8(INIT_FUNC_ARGS);
int php3_mshutdown_oci8(void);
int php3_rshutdown_oci8(void);
void php3_info_oci8(void);

static ub4 oci8_error(OCIError *err_p, char *what, sword status);
static int oci8_ping(oci8_connection *conn);
static void oci8_debug(const char *format,...);
static void oci8_close_conn(oci8_connection *connection);
static void oci8_free_stmt(oci8_statement *statement);

static oci8_connection *oci8_get_conn(int, const char *, HashTable *, HashTable *);
static oci8_statement *oci8_get_stmt(int, const char *, HashTable *);
static oci8_out_column *oci8_get_col(oci8_statement *, int, char *);
static pval *oci8_make_pval(oci8_out_column *, char *);
static int oci8_parse(oci8_connection *, text *, ub4, HashTable *);
static int oci8_execute(oci8_statement *, char *);
static int oci8_fetch(oci8_statement *, ub4, char *);

/* bind callback functions */
static sb4 oci8_bind_in_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 *, ub1 *, dvoid **);
static sb4 oci8_bind_out_callback(dvoid *, OCIBind *, ub4, ub4, dvoid **, ub4 **, ub1 *, dvoid **, ub2 **);

/* }}} */
/* {{{ extension function prototypes */

void php3_oci8_bindbyname(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_columnisnull(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_columnname(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_columnsize(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_columntype(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_execute(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_fetch(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_fetchinto(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_freestatement(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_internaldebug(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_logoff(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_logon(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_numcols(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_parse(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_result(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_serverversion(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci8_statementtype(INTERNAL_FUNCTION_PARAMETERS);

/* }}} */
/* {{{ extension definition structures */

static unsigned char refs_aaf[] = {3, BYREF_ALLOW, BYREF_ALLOW, BYREF_FORCE};

function_entry oci8_functions[] = {
	{"ocibindbyname",    php3_oci8_bindbyname,    refs_aaf},
	{"ocicolumnisnull",  php3_oci8_columnisnull,  NULL},
	{"ocicolumnname",    php3_oci8_columnname,    NULL},
	{"ocicolumnsize",    php3_oci8_columnsize,    NULL},
	{"ocicolumntype",    php3_oci8_columntype,    NULL},
	{"ociexecute",       php3_oci8_execute,       NULL},
	{"ocifetch",         php3_oci8_fetch,         NULL},
	{"ocifetchinto",     php3_oci8_fetchinto,     second_arg_force_ref},
	{"ocifreestatement", php3_oci8_freestatement, NULL},
	{"ociinternaldebug", php3_oci8_internaldebug, NULL},
	{"ocinumcols",       php3_oci8_numcols,       NULL},
	{"ociparse",         php3_oci8_parse,         NULL},
	{"ociresult",        php3_oci8_result,        NULL},
	{"ociserverversion", php3_oci8_serverversion, NULL},
	{"ocistatementtype", php3_oci8_statementtype, NULL},
    {"ocilogoff",        php3_oci8_logoff,        NULL},
    {"ocilogon",         php3_oci8_logon,         NULL},
/*    {"ociplogon",        php3_oci8_plogon,      	NULL},*/
    {NULL,               NULL,                    NULL}
};

php3_module_entry oci8_module_entry = {
    "OCI8",                /* extension name */
    oci8_functions,        /* extension function list */
    php3_minit_oci8,       /* extension-wide startup function */
    php3_mshutdown_oci8,   /* extension-wide shutdown function */
    php3_rinit_oci8,       /* per-request startup function */
    php3_rshutdown_oci8,   /* per-request shutdown function */
    php3_info_oci8,        /* information function */
    STANDARD_MODULE_PROPERTIES
};

/* }}} */
/* {{{ startup, shutdown and info functions */

int php3_minit_oci8(INIT_FUNC_ARGS)
{
#ifdef THREAD_SAFE
	oci8_global_struct *oci8_globals;
# if !COMPILE_DL
#  if WIN32|WINNT
	CREATE_MUTEX(oci8_mutex,"OCI8_TLS");
#  endif
	SET_MUTEX(oci8_mutex);
	numthreads++;
	if (numthreads == 1) {
		if ((OCI8Tls = TlsAlloc()) == 0xFFFFFFFF){
			FREE_MUTEX(oci8_mutex);
			return 0;
		}
	}
	FREE_MUTEX(oci8_mutex);
# endif /* !COMPILE_DL */
	oci8_globals =
		(oci8_global_struct *) LocalAlloc(LPTR, sizeof(oci8_global_struct)); 
	TlsSetValue(OCI8Tls, (void *) oci8_globals);
#endif /* THREAD_SAFE */

	if (cfg_get_long("oci8.allow_persistent",
					 &OCI8_GLOBAL(php3_oci8_module).allow_persistent)
		== FAILURE) {
	  OCI8_GLOBAL(php3_oci8_module).allow_persistent = -1;
	}
	if (cfg_get_long("oci8.max_persistent",
					 &OCI8_GLOBAL(php3_oci8_module).max_persistent)
	    == FAILURE) {
		OCI8_GLOBAL(php3_oci8_module).max_persistent = -1;
	}
	if (cfg_get_long("oci8.max_links",
					 &OCI8_GLOBAL(php3_oci8_module).max_links)
	    == FAILURE) {
		OCI8_GLOBAL(php3_oci8_module).max_links = -1;
	}
	
	OCI8_GLOBAL(php3_oci8_module).num_persistent = 0;
	
	OCI8_GLOBAL(php3_oci8_module).le_conn =
		register_list_destructors(oci8_close_conn, NULL);

	OCI8_GLOBAL(php3_oci8_module).le_stmt =
		register_list_destructors(oci8_free_stmt, NULL);

	if (cfg_get_long("oci8.max_bind_data_size",
					 &OCI8_GLOBAL(php3_oci8_module).max_bind_data_size) == FAILURE) {
		OCI8_GLOBAL(php3_oci8_module).max_bind_data_size = OCI8_MAX_DATA_SIZE;
	}

	if (cfg_get_long("oci8.debug_mode",
					 &OCI8_GLOBAL(php3_oci8_module).debug_mode) == FAILURE) {
		OCI8_GLOBAL(php3_oci8_module).debug_mode = 0;
	}

	return SUCCESS;
}


int php3_rinit_oci8(INIT_FUNC_ARGS)
{
	OCI8_TLS_VARS;
	
	OCI8_GLOBAL(php3_oci8_module).num_links = 
		OCI8_GLOBAL(php3_oci8_module).num_persistent;

    return SUCCESS;
}


int php3_mshutdown_oci8()
{
#ifdef THREAD_SAFE
	oci8_global_struct *oci8_globals;
	oci8_globals = TlsGetValue(OCI8Tls); 
	if (oci8_globals != 0) {
		LocalFree((HLOCAL) oci8_globals);
	}
#if !COMPILE_DL
	SET_MUTEX(oci8_mutex);
	numthreads--;
	if (!numthreads) {
		if (!TlsFree(OCI8Tls)) {
			FREE_MUTEX(oci8_mutex);
			return 0;
		}
	}
	FREE_MUTEX(oci8_mutex);
#endif
#endif
	return SUCCESS;
}


int php3_rshutdown_oci8()
{
    return SUCCESS;
}


void php3_info_oci8()
{
	php3_printf("Oracle version: %s<br>\n"
			    "Compile-time ORACLE_HOME: %s<br>\n"
			    "Libraries used: %s",
			    PHP_ORACLE_VERSION, PHP_ORACLE_HOME, PHP_ORACLE_LIBS);
}

/* }}} */
/* {{{ extension-internal functions */

/* {{{ debug malloc/realloc/free */

#if 0
CONST dvoid *mymalloc(dvoid *ctx, size_t size)
{
    dvoid *ret;
    oci8_debug("mymalloc(%d)\n", size);
	rflush(GLOBAL(php3_rqst));
	ret = (dvoid *)emalloc(size);
    return ret;
}

CONST dvoid *myrealloc(dvoid *ctx, dvoid *ptr, size_t size)
{
    dvoid *ret;
    oci8_debug("myrealloc(%08x, %d)\n", ptr, size);
	rflush(GLOBAL(php3_rqst));
	ret = (dvoid *)erealloc(ptr, size);
    return ptr;
}

CONST void myfree(dvoid *ctx, dvoid *ptr)
{
    oci8_debug("myfree(%08x)\n", ptr);
	rflush(GLOBAL(php3_rqst));
    efree(ptr);
}
#endif

/* }}} */
/* {{{ oci8_free_column() */

static int
oci8_free_column(oci8_out_column *column)
{
	if (column->data) {
		efree(column->data);
	}
	if (column->value) {
		efree(column->value);
	}
	return 0;
}

/* }}} */
/* {{{ oci8_free_stmt() */

static void
oci8_free_stmt(oci8_statement *statement)
{
	OCI8_TLS_VARS;

	oci8_debug("Freeing statement handle for \"%s\".", statement->last_query);
	OCIHandleFree(statement->pStmt, OCI_HTYPE_STMT);
	OCIHandleFree(statement->pError, OCI_HTYPE_ERROR);
	if (statement->last_query) {
		efree(statement->last_query);
	}
	if (statement->columns) {
		_php3_hash_destroy(statement->columns);
		efree(statement->columns);
	}
	if (statement->binds) {
		_php3_hash_destroy(statement->binds);
		efree(statement->binds);
	}
	efree(statement);
}

/* }}} */
/* {{{ oci8_close_conn() */

static void
oci8_close_conn(oci8_connection *connection)
{
	OCI8_TLS_VARS;
  
	connection->open = 0;
	oci8_error(connection->pError, "OCILogoff",
					 OCILogoff(connection->pServiceContext,
							   connection->pError));
	OCI8_GLOBAL(php3_oci8_module).num_links--;
	efree(connection);
}

/* }}} */
/* {{{ oci8_error() */

static ub4
oci8_error(OCIError *err_p, char *what, sword status)
{
    text errbuf[512];
    ub4 errcode = 0;

    switch (status) {
	case OCI_SUCCESS:
	    break;
	case OCI_SUCCESS_WITH_INFO:
	    php3_error(E_WARNING, "%s: OCI_SUCCESS_WITH_INFO", what);
	    break;
	case OCI_NEED_DATA:
	    php3_error(E_WARNING, "%s: OCI_NEED_DATA", what);
	    break;
	case OCI_NO_DATA:
	    php3_error(E_WARNING, "%s: OCI_NO_DATA", what);
	    break;
	case OCI_ERROR:
	    OCIErrorGet(err_p, (ub4)1, NULL, &errcode, errbuf,
					(ub4)sizeof(errbuf), (ub4)OCI_HTYPE_ERROR);
	    php3_error(E_WARNING, "%s: %s", what, errbuf);
	    break;
	case OCI_INVALID_HANDLE:
	    php3_error(E_WARNING, "%s: OCI_INVALID_HANDLE", what);
	    break;
	case OCI_STILL_EXECUTING:
	    php3_error(E_WARNING, "%s: OCI_STILL_EXECUTING", what);
	    break;
	case OCI_CONTINUE:
	    php3_error(E_WARNING, "%s: OCI_CONTINUE", what);
	    break;
	default:
	    break;
    }
    return errcode;
}

/* }}} */
/* {{{ oci8_ping() */

/* test if a connection is still alive and return 1 if it is */
static int oci8_ping(oci8_connection *conn)
{
    /* XXX FIXME not yet implemented */
    return 1;
}

/* }}} */
/* {{{ oci8_debug() */

static void oci8_debug(const char *format,...)
{
	OCI8_TLS_VARS;

    if (OCI8_GLOBAL(php3_oci8_module).debug_mode) {
		char buffer[1024];
		va_list args;

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer)-1, format, args);
		va_end(args);
		buffer[sizeof(buffer)-1] = '\0';
		if (php3_header()) {
			php3_printf("OCIDebug: %s<br>\n", buffer);
		}
	}
}

/* }}} */
/* {{{ oci8_get_conn() */

static oci8_connection *
oci8_get_conn(int conn_ind, const char *func, HashTable *list, HashTable *plist)
{
	int type;
	oci8_connection *connection;
	OCI8_TLS_VARS;

	connection = (oci8_connection *)php3_list_find(conn_ind, &type);
	if (!connection || !OCI8_CONN_TYPE(type)) {
		php3_error(E_WARNING, "%s: invalid connection %d", func, conn_ind);
		return (oci8_connection *)NULL;
	}
	return connection;
}

/* }}} */
/* {{{ oci8_get_stmt() */

static oci8_statement *
oci8_get_stmt(int stmt_ind, const char *func, HashTable *list)
{
	int type;
	oci8_statement *statement;
	OCI8_TLS_VARS;

	statement = (oci8_statement *)php3_list_find(stmt_ind, &type);
	if (!statement || !OCI8_STMT_TYPE(type)) {
		php3_error(E_WARNING, "%s: invalid statement %d", func, stmt_ind);
		return (oci8_statement *)NULL;
	}
	return statement;
}

/* }}} */
/* {{{ oci8_get_col() */

static oci8_out_column *
oci8_get_col(oci8_statement *statement, int col, char *func)
{
	oci8_out_column *outcol = NULL;
	OCI8_TLS_VARS;

	if (_php3_hash_index_find(statement->columns, col, (void **)&outcol) == FAILURE) {
		php3_error(E_WARNING, "%s: invalid column %d", func, col);
		return NULL;
	}
	return outcol;
}

/* }}} */
/* {{{ oci8_make_pval() */

static pval *
oci8_make_pval(oci8_out_column *column, char *func)
{
	pval *value = ecalloc(1,sizeof(pval));
	size_t size;

	switch (column->indicator) {
		case -1: /* NULL value, return an empty string */
			var_reset(value);
			return value;
		case -2: /* truncated value */
			oci8_debug("%s: truncated column", func);
			size = column->size;
			break;
		case 0: /* intact value */
			size = column->storage_size;
			break;
		default:
			var_reset(value);
			return value;
	}
	value->type = IS_STRING;
	value->value.str.len = size;
	value->value.str.val = emalloc(size);
	memcpy(value->value.str.val, column->data, size);
	column->value = value;
	return value;
}

/* }}} */
/* {{{ oci8_parse() */

static int
oci8_parse(oci8_connection *connection, text *query, ub4 len, HashTable *list)
{
	oci8_statement *statement;
	sword error;
	OCI8_TLS_VARS;

	statement = ecalloc(1,sizeof(oci8_statement));
    OCIHandleAlloc(connection->pEnv, (dvoid **)&statement->pStmt,
				   OCI_HTYPE_STMT, 0, NULL);
    OCIHandleAlloc(connection->pEnv, (dvoid **)&statement->pError,
				   OCI_HTYPE_ERROR, 0, NULL);
    error = oci8_error(statement->pError, "OCIParse",
					   OCIStmtPrepare(statement->pStmt, connection->pError,
									  query, len,
									  OCI_NTV_SYNTAX, OCI_DEFAULT));
	if (error) {
		return 0;
	}

	statement->last_query = estrdup(query);
	statement->conn = connection;
	return php3_list_insert(statement, OCI8_GLOBAL(php3_oci8_module).le_stmt);
}

/* }}} */
/* {{{ oci8_execute() */

static int
oci8_execute(oci8_statement *statement, char *func)
{
	oci8_out_column *outcol;
	OCIParam *param;
	int counter;
	sword error, def_error;
	ub2 define_type;
	ub2 stmttype = 0;
	ub4 iters, mode;
	OCI8_TLS_VARS;

	OCIAttrGet((dvoid *)statement->pStmt, OCI_HTYPE_STMT,
			   (ub2 *)&stmttype, (ub4 *)0, OCI_ATTR_STMT_TYPE,
			   statement->pError);

	if (stmttype == OCI_STMT_SELECT) {
		iters = 0;
	} else {
		iters = 1;
	}
	mode = OCI_COMMIT_ON_SUCCESS;
	oci8_debug("oci8_execute: stmttype=%d mode=%d", stmttype, mode);
	error = OCIStmtExecute(statement->conn->pServiceContext,
						   statement->pStmt, statement->pError,
						   iters, 0, NULL, NULL, mode);
	if (error) {
		oci8_debug("oci8_execute error 1");
		oci8_error(statement->pError, func, error);
		return 0;
	}

	statement->columns = emalloc(sizeof(HashTable));
	if (!statement->columns ||
		_php3_hash_init(statement->columns, 13, NULL,
						(void (*)(void *))oci8_free_column, 0) == FAILURE) {
		/* out of memory */
		return 0;
	}

	if (stmttype == OCI_STMT_SELECT) {
		OCIHandleAlloc(statement->conn->pEnv, (dvoid **)&param,
					   OCI_DTYPE_PARAM, 0, NULL);
		counter = 1;
		error = OCIParamGet(statement->pStmt, OCI_HTYPE_STMT,
							statement->pError, (dvoid *)&param, counter);
		oci8_error(statement->pError, "oci8_execute2", error);
		while (error == OCI_SUCCESS) {
			outcol = ecalloc(1,sizeof(oci8_out_column));
			if (_php3_hash_index_update(statement->columns, counter, outcol,
										sizeof(oci8_out_column), NULL) == FAILURE) {
				efree(outcol);
				efree(statement->columns);
				/* out of memory */
				return 0;
			} else {
				oci8_debug("added info about column %d", counter);
			}
			efree(outcol);
			if (_php3_hash_index_find(statement->columns, counter,
									  (void **)&outcol) == FAILURE ||
				outcol == NULL) {
				php3_error(E_WARNING, "%s: inserted column lost?!", func);
				return 0;
			}
			outcol->name = NULL;

			/* get data type */
			/* XXX can get OCI_ATTR_TYPE_NAME instead */
			oci8_error(statement->pError, "OCIAttrGet(param,type)",
					   OCIAttrGet((dvoid *)param, OCI_DTYPE_PARAM,
								  (dvoid *)&outcol->type, (ub4 *)0,
								  OCI_ATTR_DATA_TYPE, statement->pError));
			/* get data size */
			oci8_error(statement->pError, "OCIAttrGet(param,size)",
					   OCIAttrGet((dvoid *)param, OCI_DTYPE_PARAM,
								  (dvoid *)&outcol->storage_size, (dvoid *)0,
								  OCI_ATTR_DATA_SIZE, statement->pError));
			/* get column name */
			oci8_error(statement->pError, "OCIAttrGet(param,name)",
					   OCIAttrGet((dvoid *)param, OCI_DTYPE_PARAM,
								  (dvoid **)&outcol->name,
								  (ub4 *)&outcol->name_len,
								  (ub4)OCI_ATTR_NAME, statement->pError));
		
			/* Remember the size Oracle told us, we have to increase the
			 * storage size for some types.
			 */
			outcol->size = outcol->storage_size;

			/* "Scalar" data types are stored directly in strings,
			 * while others, like LOBs and RAW data need to be
			 * handled specially.
			 */
			switch (outcol->type) {
				case SQLT_BIN:   /* RAW */
				case SQLT_LBI:   /* LONG RAW */
				case SQLT_BLOB:  /* binary LOB */
				case SQLT_CLOB:  /* character LOB */
				case SQLT_BFILE: /* binary file LOB */
				case SQLT_RSET:  /* ref. cursor */
					outcol->is_opaque = 1;
					define_type = outcol->type;
					break;
				default:
					outcol->is_opaque = 0;
					define_type = SQLT_STR;
					outcol->storage_size++; /* add one for string terminator */
					break;
			}
			outcol->data = (text *)emalloc(outcol->storage_size);
			if (outcol->data == NULL) {
				efree(outcol);
				efree(statement->columns);
				/* out of memory */
				return 0;
			}

			def_error = OCIDefineByPos(statement->pStmt,
									   (OCIDefine **)&outcol->pDefine,
									   statement->pError, counter,
									   (dvoid *)outcol->data,
									   outcol->storage_size, define_type,
									   (dvoid *)&outcol->indicator, (ub2 *)0,
									   (ub2 *)&outcol->retcode, OCI_DEFAULT);

			if (def_error) {
				oci8_debug("oci8_execute error 2");
				oci8_error(statement->pError, func, def_error);
				efree(outcol->data);
				efree(outcol);
				efree(statement->columns);
				return 0;
			}
			statement->ncolumns = counter;
			counter++;
			error = OCIParamGet(statement->pStmt, OCI_HTYPE_STMT,
								statement->pError, (dvoid *)&param, counter);
		}
	}
	return 1;
}

/* }}} */
/* {{{ oci8_fetch() */

static int
oci8_fetch(oci8_statement *statement, ub4 nrows, char *func)
{
	sword error;
	OCI8_TLS_VARS;

	error = OCIStmtFetch(statement->pStmt, statement->pError, nrows,
						 OCI_FETCH_NEXT, OCI_DEFAULT);
	oci8_debug("OCIStmtFetch returned %d", error);
	if (error == OCI_NO_DATA) {
		return 0;
	}
	if (error == OCI_SUCCESS_WITH_INFO || error == OCI_SUCCESS) {
		return 1;
	}
	oci8_error(statement->pError, func, error);
	return 1;
}

/* }}} */
/* {{{ oci8_bind_in_callback() */

static sb4
oci8_bind_in_callback(dvoid *ictxp, /* context pointer */
					  OCIBind *bindp, /* bind handle */
					  ub4 iter, /* 0-based execute iteration value */
					  ub4 index, /* index of current array for PL/SQL or
									row index for SQL */
					  dvoid **bufpp, /* pointer to data */
					  ub4 *alenp, /* size after value/piece has been read */
					  ub1 *piecep, /* which piece */
					  dvoid **indpp) /* indicator value */
{
	oci8_bind *phpbind;
	pval *val;

	oci8_debug("oci8_bind_in_callback: ictxp=%08x", ictxp);
	phpbind = (oci8_bind *)ictxp;
	oci8_debug("oci8_bind_in_callback: phpbind=%08x", phpbind);
	if (phpbind) {
		oci8_debug("oci8_bind_in_callback: phpbind->val=%08x", phpbind->value);
	}
	if (!phpbind || !(val = phpbind->value)) {
		php3_error(E_WARNING, "!phpbind || !phpbind->val");
		return OCI_ERROR;
	}
	if (val->type == IS_STRING && val->value.str.val == undefined_variable_string) {
		/* NULL value */
		phpbind->indicator = -1;
		phpbind->size = 0;
		phpbind->buf = NULL;
	} else {
		if (phpbind->is_opaque) {
			/* XXX get list entry and return pointer to data */
			/* give NULL value for now */
			phpbind->indicator = -1;
			phpbind->size = 0;
			phpbind->buf = NULL;
		} else {
			convert_to_string(val); /* XXX FIXME should make a copy instead */
			phpbind->indicator = 0;
			phpbind->size = val->value.str.len + 1;
			phpbind->buf = val->value.str.val;
		}
	}

	(void)iter; /* ignoring iter */
	(void)index; /* ignoring index */
	*bufpp = phpbind->buf;
	*alenp = phpbind->size;
	*indpp = (dvoid *)&phpbind->indicator;

	*piecep = OCI_ONE_PIECE; /* pass all data in one go */

	return OCI_CONTINUE;
}

/* }}} */
/* {{{ oci8_bind_out_callback() */

static sb4
oci8_bind_out_callback(dvoid *ctxp, /* context pointer */
					   OCIBind *bindp, /* bind handle */
					   ub4 iter, /* 0-based execute iteration value */
					   ub4 index, /* index of current array for PL/SQL or
									 row index for SQL */
					   dvoid **bufpp, /* pointer to data */
					   ub4 **alenpp, /* size after value/piece has been read */
					   ub1 *piecep, /* which piece */
					   dvoid **indpp, /* indicator value */
					   ub2 **rcodepp) /* return code */
{
	oci8_bind *phpbind;
	pval *val;

	oci8_debug("oci8_bind_out_callback: iter=%d index=%d alen=%d piece=%d",
			   iter, index, **alenpp, *piecep);

	phpbind = (oci8_bind *)ctxp;
	if (!phpbind) {
		oci8_debug("oci8_bind_out_callback: phpbind = NULL");
		return OCI_ERROR;
	}
	val = phpbind->value;
	if (val == NULL) {
		oci8_debug("oci8_bind_out_callback: phpbind->value = NULL");
		return OCI_ERROR;
	}
	if (phpbind->is_opaque) {
		oci8_debug("OUT binding of opaque types not yet implemented");
		return OCI_ERROR;
	} else {
		/* XXX risky, if the variable has been freed, nasty things
		 * could happen here.
		 */
		if (val->type == IS_STRING && val->value.str.val) {
			efree(val->value.str.val);
		}
		val->type = IS_STRING;
		
		phpbind->buf = val->value.str.val;
		phpbind->size = val->value.str.len + 1;
#if 0
		phpbind->buf = (dvoid *)val->value.str.val;
		phpbind->size = (ub4)val->value.str.len;
		*alenpp = (dvoid *)&phpbind->buf;
		*bufpp = (dvoid *)&phpbind->size;
		*piecep = OCI_ONE_PIECE;
		*indpp = (dvoid *)&phpbind->indicator;
#endif
	}
	
	return OCI_CONTINUE;
}

/* }}} */

/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ int    OCIBindByName(int stmt, string name, mixed &var) */

void php3_oci8_bindbyname(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *name, *var;
	oci8_statement *statement;
	oci8_bind *bind, *tmp_bind;
	sword error;

	if (getParameters(ht, 3, &stmt, &name, &var) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	convert_to_string(name);
	convert_to_string(var);
	statement = oci8_get_stmt(stmt->value.lval, "OCIBindByName", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	bind = ecalloc(1,sizeof(oci8_bind));
	if (!bind) {
		/* out of memory */
		RETURN_FALSE;
	}
	if (statement->binds == NULL) {
		statement->binds = emalloc(sizeof(HashTable));
		if (statement->binds == NULL ||
			_php3_hash_init(statement->binds, 13, NULL, NULL, 0) == FAILURE) {
			/* out of memory */
			RETURN_FALSE;
		}
	}
	if (_php3_hash_next_index_insert(statement->binds, bind,
									 sizeof(oci8_bind),
									 (void **)&tmp_bind) == SUCCESS) {
		efree(bind);
		bind = tmp_bind;
	}
	bind->value = var;
	error = OCIBindByName(statement->pStmt, /* statement handle */
						  (OCIBind **)&bind->pBind, /* bind hdl (will alloc) */
						  statement->pError, /* error handle */
						  name->value.str.val, /* placeholder name */
						  name->value.str.len, /* placeholder length */
						  (dvoid *)0, /* in/out data */
						  OCI8_GLOBAL(php3_oci8_module).max_bind_data_size,
						  (ub2)SQLT_STR, /* in/out data type */
						  (dvoid *)0, /* indicator (ignored) */
						  (ub2 *)0, /* size array (ignored) */
						  (ub2 *)0, /* return code (ignored) */
						  (ub4)0, /* maxarr_len (PL/SQL only?) */
						  (ub4 *)0, /* actual array size (PL/SQL only?) */
						  OCI_DATA_AT_EXEC /* mode */);
	if (error != OCI_SUCCESS) {
		oci8_error(statement->pError, "OCIBindByName", error);
		RETURN_FALSE;
	}
	error = OCIBindDynamic(bind->pBind,
						   statement->pError,
						   (dvoid *)bind,
						   oci8_bind_in_callback,
						   (dvoid *)bind,
						   oci8_bind_out_callback
		);
	RETVAL_TRUE;
}

/* }}} */
/* {{{ string OCIColumnName(int stmt, int col) */

void php3_oci8_columnname(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *col;
	oci8_statement *statement;
	oci8_out_column *outcol;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	convert_to_long(col);
	statement = oci8_get_stmt(stmt->value.lval, "OCIColumnName", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	outcol = oci8_get_col(statement, col->value.lval, "OCIColumnName");
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	RETVAL_STRINGL(outcol->name, outcol->name_len, 1);
}

/* }}} */
/* {{{ int    OCIColumnSize(int stmt, int col) */

void php3_oci8_columnsize(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *col;
	oci8_statement *statement;
	oci8_out_column *outcol;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	convert_to_long(col);
	statement = oci8_get_stmt(stmt->value.lval, "OCIColumnSize", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	outcol = oci8_get_col(statement, col->value.lval, "OCIColumnSize");
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	RETVAL_LONG(outcol->size);
}

/* }}} */
/* {{{ mixed  OCIColumnType(int stmt, int col) */

void php3_oci8_columntype(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *col;
	oci8_statement *statement;
	oci8_out_column *outcol;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	convert_to_long(col);
	statement = oci8_get_stmt(stmt->value.lval, "OCIColumnType", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	outcol = oci8_get_col(statement, col->value.lval, "OCIColumnType");
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	switch (outcol->type) {
		case SQLT_DAT:
			RETVAL_STRING("DATE",1);
			break;
		case SQLT_NUM:
			RETVAL_STRING("NUMBER",1);
			break;
		case SQLT_LNG:
			RETVAL_STRING("LONG",1);
			break;
		case SQLT_BIN:
			RETVAL_STRING("RAW",1);
			break;
		case SQLT_LBI:
			RETVAL_STRING("LONG RAW",1);
			break;
		case SQLT_CHR:
			RETVAL_STRING("VARCHAR",1);
			break;
		case SQLT_AFC:
			RETVAL_STRING("CHAR",1);
			break;
		case SQLT_BLOB:
			RETVAL_STRING("BLOB",1);
			break;
		case SQLT_CLOB:
			RETVAL_STRING("CLOB",1);
			break;
		case SQLT_BFILE:
			RETVAL_STRING("BFILE",1);
			break;
		default:
			RETVAL_LONG(outcol->type);
	}
}

/* }}} */
/* {{{ int    OCIColumnIsNULL(int stmt, int col) */

void php3_oci8_columnisnull(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *col;
	oci8_statement *statement;
	oci8_out_column *outcol;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	convert_to_long(col);
	statement = oci8_get_stmt(stmt->value.lval, "OCIColumnIsNULL", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	outcol = oci8_get_col(statement, col->value.lval, "OCIColumnIsNULL");
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	if (outcol->indicator == -1) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}

/* }}} */
/* {{{ void   OCIDebug(int onoff) */

/* Disables or enables the internal debug output.
 * By default it is disabled.
 */
void php3_oci8_internaldebug(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);
	OCI8_GLOBAL(php3_oci8_module).debug_mode = arg->value.lval;
}


/* }}} */
/* {{{ int    OCIExecute(int stmt) */

void php3_oci8_execute(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt;
	oci8_statement *statement;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIExecute", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	
	if (oci8_execute(statement, "OCIExecute")) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}

/* }}} */
/* {{{ int    OCIFetch(int stmt) */

void php3_oci8_fetch(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt;
	oci8_statement *statement;
	ub4 nrows = 1; /* only one row at a time is supported for now */
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);

	statement = oci8_get_stmt(stmt->value.lval, "OCIFetch", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	if (oci8_fetch(statement, nrows, "OCIFetch")) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
}

/* }}} */
/* {{{ int    OCIFetchInto(int stmt, array &output) */

void php3_oci8_fetchinto(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *array, *element;
	oci8_statement *statement;
	oci8_out_column *column;
	ub4 nrows = 1;
	int i;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIFetchInto", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	if (array->type != IS_ARRAY) {
		if (array_init(array) == FAILURE) {
			php3_error(E_WARNING,
					   "OCIFetchInto: unable to convert arg 2 to array");
			RETURN_FALSE;
		}
	}
	if (!oci8_fetch(statement, nrows, "OCIFetchInto")) {
		RETURN_FALSE;
	}

	for (i = 0; i < statement->ncolumns; i++) {
		column = oci8_get_col(statement, i + 1, "OCIFetchInto");
		if (column == NULL || column->indicator == -1) {
			continue;
		}
		element = oci8_make_pval(column, "OCIFetchInto");
		_php3_hash_index_update(array->value.ht, i, (void *)element,
								sizeof(pval), NULL);
	}
	RETVAL_LONG(statement->ncolumns);
}

/* }}} */
/* {{{ int    OCIFreeStatement(int stmt) */

void php3_oci8_freestatement(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt;
	oci8_statement *statement;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIFreeStatement", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	oci8_free_stmt(statement);
	RETVAL_TRUE;
}

/* }}} */
/* {{{ int    OCILogoff(int conn) */

/* Logs off and disconnects.
 */
void php3_oci8_logoff(INTERNAL_FUNCTION_PARAMETERS)
{
	oci8_connection *connection;
	pval *arg;
	int index, index_t;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);
	index = arg->value.lval;
	connection = (oci8_connection *)php3_list_find(index, &index_t);
	if (!connection || !OCI8_CONN_TYPE(index_t)) {
		RETURN_FALSE;
	}
	if (php3_list_delete(index) == SUCCESS) {
		RETVAL_TRUE;
	} else {
		oci8_debug("OCILogoff: php3_list_delete failed.");
		RETVAL_FALSE;
	}
}

/* }}} */
/* {{{ int    OCILogon(string user, string pass[, string db]) */

/* Connects to an Oracle 8 database and logs on.  If the
 * optional third parameter is not specified, PHP uses the environment
 * variable ORACLE_SID to determine which database to connect to.
 */
void php3_oci8_logon(INTERNAL_FUNCTION_PARAMETERS)
{
    text *username, *password, *dbname;
    pval *userParam, *passParam, *dbParam;
    ub4 username_len, password_len, dbname_len;
    oci8_connection *connection;
    sword error;
    OCI8_TLS_VARS;

    if (getParameters(ht, 3, &userParam, &passParam, &dbParam) == SUCCESS) {
		convert_to_string(userParam);
		convert_to_string(passParam);
		convert_to_string(dbParam);
		username = userParam->value.str.val;
		password = passParam->value.str.val;
		dbname = dbParam->value.str.val;
		dbname_len = dbParam->value.str.len;
    } else if (getParameters(ht, 2, &userParam, &passParam) == SUCCESS) {
		convert_to_string(userParam);
		convert_to_string(passParam);
		username = userParam->value.str.val;
		password = passParam->value.str.val;
		dbname = (text *)NULL;
		dbname_len = 0;
    } else {
		WRONG_PARAM_COUNT;
    }
    username_len = userParam->value.str.len;
    password_len = passParam->value.str.len;

    if (OCI8_GLOBAL(php3_oci8_module).max_links != -1 &&
		OCI8_GLOBAL(php3_oci8_module).num_links >=
		OCI8_GLOBAL(php3_oci8_module).max_links) {
		php3_error(E_WARNING, "OCILogon: Too many open links (%d)",
				   OCI8_GLOBAL(php3_oci8_module).num_links);
		RETURN_FALSE;
    }

	connection = (oci8_connection *)ecalloc(1,sizeof(oci8_connection));

	if (connection == NULL) {
		RETURN_OUT_OF_MEMORY;
	}
		
	/* Set up the Oracle environment. */
#if 0
    OCIInitialize(OCI_DEFAULT, (dvoid *)connection, mymalloc, myrealloc, myfree);
#else
    OCIInitialize(OCI_DEFAULT, NULL, NULL, NULL, NULL);
#endif
	OCIEnvInit(&connection->pEnv, OCI_DEFAULT, 0, NULL);
	OCIHandleAlloc(connection->pEnv, (dvoid **)&connection->pError,
				   OCI_HTYPE_ERROR, 0, NULL);
	OCIHandleAlloc(connection->pEnv,
				   (dvoid **)&connection->pServiceContext,
				   OCI_HTYPE_SVCCTX, 0, NULL);

	error = OCILogon((OCIEnv *)connection->pEnv,
					 (OCIError *)connection->pError,
					 (OCISvcCtx **)&connection->pServiceContext,
					 (CONST text *)username, (ub4)username_len,
					 (CONST text *)password, (ub4)password_len,
					 (CONST text *)dbname,   (ub4)dbname_len);
	if (error) {
		oci8_error(connection->pError, "OCILogon", error);
		RETURN_FALSE;
	}
		
	connection->open = 1;
		
	/* non persistent, simply add to list */
	RETVAL_LONG(php3_list_insert(connection, OCI8_GLOBAL(php3_oci8_module).le_conn));
	OCI8_GLOBAL(php3_oci8_module).num_links++;
}

/* }}} */
/* {{{ int    OCINumCols(int stmt) */

void php3_oci8_numcols(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt;
	oci8_statement *statement;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCINumCols", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	RETVAL_LONG(statement->ncolumns);
}

/* }}} */
/* {{{ int    OCIParse(int conn, string query) */

void php3_oci8_parse(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *conn, *query;
	oci8_connection *connection;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &conn, &query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(conn);
	convert_to_string(query);

	connection = oci8_get_conn(conn->value.lval, "OCIParse", list, plist);
	if (connection == NULL) {
		RETURN_FALSE;
	}

	RETVAL_LONG(oci8_parse(connection,
						   query->value.str.val,
						   query->value.str.len,
						   list));
}

/* }}} */
/* {{{ string OCIResult(int stmt, mixed column) */

void php3_oci8_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt, *col, *ret;
	oci8_statement *statement;
	oci8_out_column *outcol;
	OCI8_TLS_VARS;

	if (getParameters(ht, 2, &stmt, &col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	convert_to_long(col);
	statement = oci8_get_stmt(stmt->value.lval, "OCIResult", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}
	outcol = oci8_get_col(statement, col->value.lval, "OCIResult");
	if (outcol == NULL) {
		RETURN_FALSE;
	}
	ret = oci8_make_pval(outcol, "OCIResult");
	if (ret == NULL) {
		RETVAL_FALSE;
	} else {
		memcpy(return_value, ret, sizeof(pval));
	}
}

/* }}} */
/* {{{ string OCIServerVersion(int conn) */

void php3_oci8_serverversion(INTERNAL_FUNCTION_PARAMETERS)
{
	oci8_connection *connection;
	pval *arg;
	int index, index_t;
	sword error;
	char version[256];
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);
	index = arg->value.lval;
	connection = (oci8_connection *)php3_list_find(index, &index_t);
	if (!connection || !OCI8_CONN_TYPE(index_t)) {
		RETURN_FALSE;
	}
	error = OCIServerVersion(connection->pServiceContext,
							 connection->pError, version, sizeof(version),
							 OCI_HTYPE_SVCCTX);
	if (error != OCI_SUCCESS) {
		oci8_error(connection->pError, "OCIServerVersion", error);
		RETURN_FALSE;
	}
	RETVAL_STRING(version,1);
}

/* }}} */
/* {{{ int    OCIStatementType(int stmt) */

/* XXX it would be better with a general interface to OCIAttrGet() */

void php3_oci8_statementtype(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *stmt;
	oci8_statement *statement;
	ub2 stmttype;
	sword error;
	OCI8_TLS_VARS;

	if (getParameters(ht, 1, &stmt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(stmt);
	statement = oci8_get_stmt(stmt->value.lval, "OCIStatementType", list);
	if (statement == NULL) {
		RETURN_FALSE;
	}

	error = OCIAttrGet((dvoid *)statement->pStmt, OCI_HTYPE_STMT,
					   (ub2 *)&stmttype, (ub4 *)0, OCI_ATTR_STMT_TYPE,
					   statement->pError);
	if (error != OCI_SUCCESS) {
		oci8_error(statement->pError, "OCIStatementType", error);
		RETURN_FALSE;
	}

	switch (stmttype) {
		case OCI_STMT_SELECT:
			RETVAL_STRING("SELECT",1);
			break;
		case OCI_STMT_UPDATE:
			RETVAL_STRING("UPDATE",1);
			break;
		case OCI_STMT_DELETE:
			RETVAL_STRING("DELETE",1);
			break;
		case OCI_STMT_INSERT:
			RETVAL_STRING("INSERT",1);
			break;
		case OCI_STMT_CREATE:
			RETVAL_STRING("CREATE",1);
			break;
		case OCI_STMT_DROP:
			RETVAL_STRING("DROP",1);
			break;
		case OCI_STMT_ALTER:
			RETVAL_STRING("ALTER",1);
			break;
		case OCI_STMT_BEGIN:
			RETVAL_STRING("BEGIN",1);
			break;
		case OCI_STMT_DECLARE:
			RETVAL_STRING("DECLARE",1);
			break;
		default:
			RETVAL_STRING("UNKNOWN",1);
	}
}

/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
