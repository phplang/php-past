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
   |          Mitch Golden <mgolden@interport.net>                        |
   +----------------------------------------------------------------------+
 */

/*
 *  $Id: oracle.c,v 1.46 1998/02/02 05:02:54 ssb Exp $
 *
 * TODO:
 *
 * PHP 3.0 stuff:
 *
 * - rewrite the column stuff to something else than a linked list
 *
 * General stuff:
 *
 * - MAGIC_QUOTES  (hard, because you can't escape ' as \', must be ''
 *                  and the routine _php3_addslashes is used for multiple
 *                  things.)
 * - array/table support
 * - persistent connections (migrate this from 2.0)
 *
 * Function wishlist:
 *
 * - Ora_ColumnName
 * - Ora_ColumnType
 * - Ora_Bind (bind PHP variables to SQL placeholders)
 *            done in 2.0, but should be reimplemented for 3.0
 * - Ora_FetchLong (requires proper binary string support in PHP)
 * - Ora_Options (easy, it's just that nobody has needed it so far :)
 *
 */

/* PHP stuff */
#include "parser.h"
#include "internal_functions.h"
#include "oracle.h"

#if HAVE_ORACLE

#include "list.h"
#include "build-defs.h"

#ifdef THREAD_SAFE

void *uodbc_mutex;
DWORD ORACLETls;
static int numthreads=0;

typedef struct oracle_global_struct {
	oracle_module php3_oracle_module;
} oracle_global_struct;

#define ORACLE_GLOBAL(a) oracle_globals->a

#define ORACLE_TLS_VARS \
	oracle_global_struct *oracle_globals = TlsGetValue(ORACLETls); 

#else
oracle_module php3_oracle_module;
#define ORACLE_GLOBAL(a) a
#define ORACLE_TLS_VARS
#endif

#undef ORACLE_DEBUG

static oraConnection *ora_add_conn(HashTable *);
static oraConnection *ora_get_conn(HashTable *, int);
static void ora_del_conn(HashTable *, int);
static oraCursor *ora_add_cursor(HashTable *);
static oraCursor *ora_get_cursor(HashTable *, int);
static void ora_del_cursor(HashTable *, int);
static char *ora_error(Cda_Def *);
static int ora_describe_define(oraCursor *);

function_entry oracle_functions[] = {
	{"ora_close",		php3_Ora_Close,			NULL},
	{"ora_commit",		php3_Ora_Commit,		NULL},
	{"ora_commitoff",	php3_Ora_CommitOff,		NULL},
	{"ora_commiton",	php3_Ora_CommitOn,		NULL},
	{"ora_error",		php3_Ora_Error,			NULL},
	{"ora_errorcode",	php3_Ora_ErrorCode,		NULL},
	{"ora_exec",		php3_Ora_Exec,			NULL},
	{"ora_fetch",		php3_Ora_Fetch,			NULL},
	{"ora_getcolumn",	php3_Ora_GetColumn,		NULL},
	{"ora_logoff",		php3_Ora_Logoff,		NULL},
	{"ora_logon",		php3_Ora_Logon,			NULL},
	{"ora_open",		php3_Ora_Open,			NULL},
	{"ora_parse",		php3_Ora_Parse,			NULL},
	{"ora_rollback",	php3_Ora_Rollback,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry oracle_module_entry = {
	"Oracle", oracle_functions, php3_minit_oracle, php3_mshutdown_oracle,
	NULL, NULL, php3_info_oracle, 0, 0, 0, NULL
};

static const text *ora_func_tab[] =
{(text *) "unused",
/*  1, 2  */ (text *) "unused", (text *) "OSQL",
/*  3, 4  */ (text *) "unused", (text *) "OEXEC/OEXN",
/*  5, 6  */ (text *) "unused", (text *) "OBIND",
/*  7, 8  */ (text *) "unused", (text *) "ODEFIN",
/*  9, 10 */ (text *) "unused", (text *) "ODSRBN",
/* 11, 12 */ (text *) "unused", (text *) "OFETCH/OFEN",
/* 13, 14 */ (text *) "unused", (text *) "OOPEN",
/* 15, 16 */ (text *) "unused", (text *) "OCLOSE",
/* 17, 18 */ (text *) "unused", (text *) "unused",
/* 19, 20 */ (text *) "unused", (text *) "unused",
/* 21, 22 */ (text *) "unused", (text *) "ODSC",
/* 23, 24 */ (text *) "unused", (text *) "ONAME",
/* 25, 26 */ (text *) "unused", (text *) "OSQL3",
/* 27, 28 */ (text *) "unused", (text *) "OBNDRV",
/* 29, 30 */ (text *) "unused", (text *) "OBNDRN",
/* 31, 32 */ (text *) "unused", (text *) "unused",
/* 33, 34 */ (text *) "unused", (text *) "OOPT",
/* 35, 36 */ (text *) "unused", (text *) "unused",
/* 37, 38 */ (text *) "unused", (text *) "unused",
/* 39, 40 */ (text *) "unused", (text *) "unused",
/* 41, 42 */ (text *) "unused", (text *) "unused",
/* 43, 44 */ (text *) "unused", (text *) "unused",
/* 45, 46 */ (text *) "unused", (text *) "unused",
/* 47, 48 */ (text *) "unused", (text *) "unused",
/* 49, 50 */ (text *) "unused", (text *) "unused",
/* 51, 52 */ (text *) "unused", (text *) "OCAN",
/* 53, 54 */ (text *) "unused", (text *) "OPARSE",
/* 55, 56 */ (text *) "unused", (text *) "OEXFET",
/* 57, 58 */ (text *) "unused", (text *) "OFLNG",
/* 59, 60 */ (text *) "unused", (text *) "ODESCR",
/* 61, 62 */ (text *) "unused", (text *) "OBNDRA"
};

#if COMPILE_DL
php3_module_entry *get_module() { return &oracle_module_entry; };
#if (WIN32|WINNT) && defined(THREAD_SAFE)

/* NOTE: You should have an odbc.def file where you export DllMain */
BOOL WINAPI DllMain(HANDLE hModule, 
					DWORD  ul_reason_for_call, 
					LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH:
			if ((ORACLETls = TlsAlloc()) == 0xFFFFFFFF){
				return 0;
			}
			break;    
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			if(!TlsFree(ORACLETls)){
				return 0;
			}
			break;
    }
    return 1;
}
#endif
#endif


static void _close_oraconn(oraConnection *conn)
{
	/* XXX TODO FIXME call ologof() here if the connection is open */
	efree(conn);
}


static void _close_oracur(oraCursor *cur)
{
	/* XXX TODO FIXME call oclose() here if the cursor is open */
	if (cur->columns) {
		hash_destroy(cur->columns);
		efree(cur->columns);
	}
	if (cur->current_query) {
		efree(cur->current_query);
	}
	efree(cur);
}


int php3_minit_oracle(INITFUNCARG)
{
#ifdef THREAD_SAFE
	oracle_global_struct *oracle_globals;
#if !COMPILE_DL
#if WIN32|WINNT
	CREATE_MUTEX(oracle_mutex,"ORACLE_TLS");
#endif
#endif
#if !COMPILE_DL
	SET_MUTEX(oracle_mutex);
	numthreads++;
	if(numthreads==1){
	if((ORACLETls=TlsAlloc())==0xFFFFFFFF){
		FREE_MUTEX(oracle_mutex);
		return 0;
	}}
	FREE_MUTEX(oracle_mutex);
#endif
	oracle_globals =
		(oracle_global_struct *) LocalAlloc(LPTR, sizeof(oracle_global_struct)); 
	TlsSetValue(ORACLETls, (void *) oracle_globals);
#endif

	ORACLE_GLOBAL(php3_oracle_module).le_conn =
		register_list_destructors(_close_oraconn,NULL);
	ORACLE_GLOBAL(php3_oracle_module).le_cursor =
		register_list_destructors(_close_oracur,NULL);

	return SUCCESS;
}

int php3_mshutdown_oracle(void)
{
#ifdef THREAD_SAFE
	oracle_global_struct *oracle_globals;
	oracle_globals = TlsGetValue(ORACLETls); 
	if (oracle_globals != 0) {
		LocalFree((HLOCAL) oracle_globals);
	}
#if !COMPILE_DL
	SET_MUTEX(oracle_mutex);
	numthreads--;
	if (!numthreads) {
		if (!TlsFree(ORACLETls)) {
			FREE_MUTEX(oracle_mutex);
			return 0;
		}
	}
	FREE_MUTEX(oracle_mutex);
#endif
#endif
	return SUCCESS;

}

/*
   ** PHP functions
 */

void php3_Ora_Logon(INTERNAL_FUNCTION_PARAMETERS)
{								/* userid, password */
	YYSTYPE *user, *pw;
	oraConnection *conn;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &user, &pw) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(user);
	convert_to_string(pw);

	conn = ora_add_conn(list);

	if (conn == NULL || orlon(&conn->lda, conn->hda, user->value.strval, -1,
							  pw->value.strval, -1, 0)) {
		php3_error(E_WARNING, "Unable to connect to ORACLE (%s)",
					ora_error(&conn->lda));
		RETVAL_LONG(-1);
		if (conn != NULL) {
			ora_del_conn(list, conn->ind);
		}
	} else {
		RETVAL_LONG(conn->ind);
	}
}


void php3_Ora_Logoff(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	int index;
	oraConnection *conn;
	YYSTYPE *arg;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	index = arg->value.lval;
	conn = ora_get_conn(list, index);

	if (conn == NULL) {
		php3_error(E_WARNING, "No such connection id (%d)", index);
		RETURN_FALSE;
	}
	if (ologof(&conn->lda)) {
		RETVAL_LONG(-1);
		php3_error(E_WARNING, "Error logging off of connection %d",
					index);
	} else {
		RETVAL_LONG(0);
	}

	ora_del_conn(list, index);
}


void php3_Ora_Open(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	YYSTYPE *arg;
	oraConnection *conn;
	oraCursor *cursor;
	int conn_ind;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	conn_ind = arg->value.lval;
	conn = ora_get_conn(list, conn_ind);
	if (conn == NULL) {
		php3_error(E_WARNING, "Invalid connection index %d", conn_ind);
		RETURN_FALSE;
	}
	cursor = ora_add_cursor(list);

	if (cursor == NULL ||
	oopen(&cursor->cda, &conn->lda, (text *) 0, -1, -1, (text *) 0, -1)) {
		php3_error(E_WARNING, "Unable to open new cursor (%s)",
					ora_error(&cursor->cda));
		if (cursor != NULL) {
			ora_del_cursor(list, cursor->ind);
		}
		RETVAL_LONG(-1);
	}
	RETVAL_LONG(cursor->ind);
}


void php3_Ora_Close(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	YYSTYPE *arg;
	oraCursor *cursor;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	cursor = ora_get_cursor(list, arg->value.lval);

	if (cursor == NULL) {
		php3_error(E_WARNING, "Invalid cursor index %d", arg->value.lval);
		RETURN_FALSE;
	}
	if (oclose(&cursor->cda)) {
		php3_error(E_WARNING, "Unable to close cursor (%s)",
					ora_error(&cursor->cda));
		RETURN_FALSE;
	}
	ora_del_cursor(list, cursor->ind);
	RETVAL_LONG(0);
}


void php3_Ora_CommitOff(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	YYSTYPE *arg;
	oraConnection *conn;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	conn = ora_get_conn(list, arg->value.lval);
	if (conn == NULL) {
		php3_error(E_WARNING, "Invalid connection index %d",
					arg->value.lval);
		RETURN_FALSE;
	}
	if (ocof(&conn->lda)) {
		php3_error(E_WARNING, "Unable to turn off auto-commit (%s)",
					ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETVAL_LONG(0);
}


void php3_Ora_CommitOn(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	YYSTYPE *arg;
	oraConnection *conn;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	conn = ora_get_conn(list, arg->value.lval);
	if (conn == NULL) {
		php3_error(E_WARNING, "Invalid connection index %d",
					arg->value.lval);
		RETURN_FALSE;
	}
	if (ocon(&conn->lda)) {
		php3_error(E_WARNING, "Unable to turn on auto-commit (%s)",
					ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETVAL_LONG(0);
}


void php3_Ora_Commit(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	YYSTYPE *arg;
	oraConnection *conn;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	conn = ora_get_conn(list, arg->value.lval);
	if (conn == NULL) {
		php3_error(E_WARNING, "Invalid connection index %d",
					arg->value.lval);
		RETURN_FALSE;
	}
	if (ocom(&conn->lda)) {
		php3_error(E_WARNING, "Unable to commit transaction (%s)",
					ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETVAL_LONG(0);
}


void php3_Ora_Rollback(INTERNAL_FUNCTION_PARAMETERS)
{								/* conn_index */
	YYSTYPE *arg;
	oraConnection *conn;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	conn = ora_get_conn(list, arg->value.lval);
	if (conn == NULL) {
		php3_error(E_WARNING, "Invalid connection index %d",
					arg->value.lval);
		RETURN_FALSE;
	}
	if (orol(&conn->lda)) {
		php3_error(E_WARNING, "Unable to roll back transaction (%s)",
					ora_error(&conn->lda));
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}


void php3_Ora_Parse(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_ind, sql_statement [, defer] */
	int argc;
	YYSTYPE *argv[3];
	oraCursor *cursor;
	sword defer = 0;
	text *query;

	argc = ARG_COUNT(ht);
	if ((argc != 2 && argc != 3) || getParametersArray(ht, argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(argv[0]);
	convert_to_string(argv[1]);

	if (argc == 3) {
		convert_to_long(argv[2]);
		if (argv[2]->value.lval != 0) {
			defer = DEFER_PARSE;
		}
	}
	/* XXX use pool 2 instead? */
	query = (text *) estrndup(argv[1]->value.strval,argv[1]->strlen);

	if (query == NULL) {
		php3_error(E_WARNING, "Invalid query");
		RETURN_FALSE;
	}
	cursor = ora_get_cursor(list, argv[0]->value.lval);
	if (cursor == NULL) {
		php3_error(E_WARNING, "Invalid cursor index %d",
					argv[0]->value.lval);
		RETURN_FALSE;
	}
	if (cursor->current_query) {
		efree(cursor->current_query);
	}
	cursor->current_query = query;

	if (oparse(&cursor->cda, query, (sb4) - 1, defer, VERSION_7)) {
		php3_error(E_WARNING, "Ora_Parse failed (%s)",
					ora_error(&cursor->cda));
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/* Returns the number of SELECT-list items for a select, number of
   ** affected rows for UPDATE/INSERT/DELETE, 0 for another successful
   ** statement or -1 on error.
 */
void php3_Ora_Exec(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index */
	YYSTYPE *arg;
	oraCursor *cursor;
	int ncol = 0;
	ub2 sqlfunc;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	cursor = ora_get_cursor(list, arg->value.lval);
	if (cursor == NULL) {
		php3_error(E_WARNING, "Invalid cursor index %d",
					arg->value.lval);
		RETURN_FALSE;
	}
	sqlfunc = cursor->cda.ft;
	if (sqlfunc == FT_SELECT) {
		ncol = ora_describe_define(cursor);
		if (ncol < 0) {
			/* error message is given by ora_describe_define() */
			RETURN_FALSE;
		}
	}
	if (oexec(&cursor->cda)) {
		php3_error(E_WARNING, "Ora_Exec failed (%s)",
					ora_error(&cursor->cda));
		RETURN_FALSE;
	}
	sqlfunc = cursor->cda.ft;
	if (sqlfunc == FT_SELECT) {
		RETVAL_LONG(ncol);
	} else {
		RETVAL_LONG(0);
	}
}


void php3_Ora_Fetch(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index */
	YYSTYPE *arg;
	oraCursor *cursor;
	sword err;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);

	/* Find the cursor */
	cursor = ora_get_cursor(list, arg->value.lval);
	if (cursor == NULL) {
		php3_error(E_WARNING, "Invalid cursor index %d", arg->value.lval);
		RETURN_FALSE;
	}
	/* Get data from Oracle */
	err = ofetch(&cursor->cda);
	if (err) {
		if (cursor->cda.rc == NO_DATA_FOUND) {
			RETURN_LONG(0);
		}
		php3_error(E_WARNING, "Ora_Fetch failed (%s)",
					ora_error(&cursor->cda));
		RETURN_FALSE;
	}

	RETVAL_LONG(1);
}


void php3_Ora_GetColumn(INTERNAL_FUNCTION_PARAMETERS)
{								/* cursor_index, column_index */
	YYSTYPE *argv[2];
	int cursor_ind, found;
	oraCursor *cursor = NULL;
	oraColumn *column = NULL;
	sb2 type;

	if (ARG_COUNT(ht) != 2 || getParametersArray(ht, 2, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(argv[0]);
	/* don't convert the column index yet, it might be the column name */

	cursor_ind = argv[0]->value.lval;
	/* Find the cursor */
	cursor = ora_get_cursor(list, cursor_ind);
	if (cursor == NULL) {
		php3_error(E_WARNING, "Invalid cursor index %d", cursor_ind);
		RETURN_FALSE;
	}

	convert_to_long(argv[1]);
	found = hash_index_find(cursor->columns, argv[1]->value.lval,
							(void **)&column);

	if (found == FAILURE || column == NULL) {
		php3_error(E_WARNING, "Invalid column");
		RETURN_FALSE;
	}

	type = column->dbtype;

	if (column->col_retcode != 0 && column->col_retcode != 1406) {
		/* So error fetching column.  The most common is 1405, a NULL */
		/* was retreived.  1406 is ASCII or string buffer data was */
		/* truncated. The converted data from the database did not fit */
		/* into the buffer.  Since we allocated the buffer to be large */
		/* enough, this should not occur.  Anyway, we probably want to */
		/* return what we did get, in that case */
		RETURN_FALSE;
	} else if (type == FLOAT_TYPE) {
		RETURN_DOUBLE(column->flt_buf);
	} else if (type == INT_TYPE) {
		RETURN_LONG(column->int_buf);
	} else if (type == STRING_TYPE || type == VARCHAR2_TYPE) {
		RETURN_STRINGL(column->buf, column->dsize);
	} else {
		php3_error(E_WARNING,
			   "Ora_GetColumn found invalid type (%d)", type);
		RETURN_FALSE;
	}
}

void php3_Ora_Error(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *cursor_index;
	oraCursor *cursor;

	if (ARG_COUNT(ht) != 1 || getParametersArray(ht, 1, &cursor_index) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(cursor_index);
	cursor = ora_get_cursor(list, cursor_index->value.lval);
	if (!cursor) {
		php3_error(E_WARNING, "Ora_Error: could not find cursor");
		RETURN_FALSE;
	}

	return_value->type = IS_STRING;
	return_value->value.strval = ora_error(&cursor->cda);
	return_value->strlen = strlen(return_value->value.strval);
}


void php3_Ora_ErrorCode(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *cursor_index;
	oraCursor *cursor;

	if (ARG_COUNT(ht) != 1 || getParametersArray(ht, 1, &cursor_index) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(cursor_index);
	cursor = ora_get_cursor(list, cursor_index->value.lval);
	if (!cursor) {
		php3_error(E_WARNING, "Ora_Error: could not find cursor");
		RETURN_FALSE;
	}

	RETURN_LONG(cursor->cda.rc);
}


void php3_info_oracle()
{
	php3_printf("Oracle version: %s<br>\n"
			    "Compile-time ORACLE_HOME: %s<br>\n"
			    "Libraries used: %s",
			    PHP_ORACLE_VERSION, PHP_ORACLE_HOME, PHP_ORACLE_LIBS);
}


/*
** Functions internal to this module.
*/


static oraConnection *
ora_add_conn(HashTable *list)
{
	int i;
	oraConnection *conn;
	ORACLE_TLS_VARS;

	conn = emalloc(sizeof(oraConnection));

	i = php3_list_insert(conn, ORACLE_GLOBAL(php3_oracle_module).le_conn);
	conn->ind = i;

	return conn;
}

static oraConnection *
ora_get_conn(HashTable *list, int ind)
{
	oraConnection *conn;
	int type;
	ORACLE_TLS_VARS;

	conn = php3_list_find(ind, &type);
	if (!conn || type != ORACLE_GLOBAL(php3_oracle_module).le_conn) {
		return NULL;
	}
	return conn;
}

static void
ora_del_conn(HashTable *list, int ind)
{
	/* XXX we don't check that this is really an oracle connection */
	php3_list_delete(ind);
}


static oraCursor *
ora_add_cursor(HashTable *list)
{
	int i;
	oraCursor *cursor;
	ORACLE_TLS_VARS;

	cursor = emalloc(sizeof(oraCursor));

	i = php3_list_insert(cursor, ORACLE_GLOBAL(php3_oracle_module).le_cursor);
	cursor->ind = i;
	cursor->current_query = NULL;

	return cursor;
}


static oraCursor *
ora_get_cursor(HashTable *list, int ind)
{
	oraCursor *cursor;
	int type;
	ORACLE_TLS_VARS;

	cursor = php3_list_find(ind, &type);
	if (!cursor || type != ORACLE_GLOBAL(php3_oracle_module).le_cursor) {
		return NULL;
	}
	return cursor;
}

static void ora_del_cursor(HashTable *list, int ind)
{
	/* XXX we don't check that this is really an oracle cursor */
	php3_list_delete(ind);
}

static char *
ora_error(Cda_Def * cda)
{
	sword n, l;
	text *errmsg;

	errmsg = (text *) emalloc(512);
	n = oerhms(cda, cda->rc, errmsg, 400);

	/* remove the last newline */
	l = strlen(errmsg);
	if (l < 400 && errmsg[l - 1] == '\n') {
		errmsg[l - 1] = '\0';
		l--;
	}
	if (cda->fc > 0) {
		strncat(errmsg, " -- while processing OCI function ", 100);
		strcat(errmsg, ora_func_tab[cda->fc]);
	}
	return (char *) errmsg;
}


static void yystype_ora_column_destructor(oraColumn *col)
{
	if (col && col->buf) {
		efree(col->buf);
	}
}


static sword
ora_describe_define(oraCursor * cursor)
{
	long col = 0;
	sword len;
	int columns_left = 1;
	ub1 *ptr;
	Cda_Def *cda = &cursor->cda;
	oraColumn *column;

/*	cursor->column_top = NULL;*/
	if (cursor->columns) {
		hash_destroy(cursor->columns);
	} else {
		cursor->columns = (HashTable *)emalloc(sizeof(HashTable));
	}
	if (!cursor->columns ||
		hash_init(cursor->columns, 19, NULL,
				  (void (*)(void *))yystype_ora_column_destructor, 0) ==
		FAILURE)
	{
		php3_error(E_ERROR, "Unable to initialize column list");
	}

	while (columns_left) {

		column = emalloc(sizeof(oraColumn));

		column->buf = NULL;
		column->cbuf[ORANAMELEN] = '\0';
		column->cbufl = ORANAMELEN;
		column->dbsize = 0;

		/* Fetch a select-list item description */
		if (odescr(cda, (sword)col + 1, &column->dbsize, &column->dbtype,
				   &column->cbuf[0], &column->cbufl, &column->dsize,
				   &column->prec, &column->scale, &column->nullok)) {
			if (cda->rc == VAR_NOT_IN_LIST) {
				columns_left = 0;
				efree(column);
				break;
			} else {
				php3_error(E_WARNING, "%s", ora_error(cda));
				return -1;
			}
		}

		if (hash_index_update(cursor->columns, col, column, sizeof(oraColumn), NULL) == FAILURE) {
			efree(column);
			php3_error(E_ERROR, "Could not make column placeholder");
			return -1;
		}
		/* The odefin() call below will make OCI copy results into the
		 * buffer given by `ptr'.  Since hash_index_update() copied
		 * `column', we have to go fetch the copy, since that's where
		 * the data should be copied when ofetch() is called.
		 */
		efree(column);
		if (hash_index_find(cursor->columns, col, (void **)&column) == FAILURE) {
			php3_error(E_ERROR, "Column placeholder gone?");
			return -1;
		}

		column->cbuf[column->cbufl] = '\0';

		/* Determine the data type and length */
		if (column->dbtype == NUMBER_TYPE) {
			column->dbsize = ORANUMWIDTH;
			if (column->scale != 0) {
				/* Handle NUMBER with scale as float. */
				ptr = (ub1 *) &column->flt_buf;
				len = (sword) sizeof(float);
				column->dbtype = FLOAT_TYPE;
			} else {
				ptr = (ub1 *) &column->int_buf;
				len = (sword) sizeof(sword);
				column->dbtype = INT_TYPE;
			}
		} else {
			if (column->dbtype == DATE_TYPE) {
				column->dbsize = 10;
			} else if (column->dbtype == ROWID_TYPE) {
				column->dbsize = 18;
			}
			if (column->dbsize > ORABUFLEN) {
				column->dbsize = ORABUFLEN;
			}
			len = column->dbsize + 1;
			ptr = column->buf = (ub1 *) emalloc(len);
			if (ptr == NULL) {
				php3_error(E_ERROR, "Out of memory");
				return -1;
			}
			column->dbtype = STRING_TYPE;
		}

		/* Define an output variable for the column */
		if (odefin(cda, (sword)col + 1, ptr, len, column->dbtype,
				   -1, &column->indp, (text *) 0, -1, -1,
				   &column->col_retlen, &column->col_retcode)) {
			php3_error(E_WARNING, "%s", ora_error(cda));
			return -1;
		}

		col++;
	}

	cursor->ncols = (int)col;
	return cursor->ncols;
}

#endif							/* HAVE_ORACLE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
