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
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   |          Andreas Karajannis <Andreas.Karajannis@gmd.de>              |
   |          Frank M. Kromann <fmk@businessnet.dk> Support for DB/2 CLI  |
   +----------------------------------------------------------------------+
 */

/* $Id: php3_unified_odbc.h,v 1.57 2000/09/28 16:26:10 fmk Exp $ */

#ifndef _UNIFIED_ODBC_H
#define _UNIFIED_ODBC_H

/* 
 * Support for IBM DB/2 UDB on Windows NT, OS/2 and AIX
 * define HAVE_IBMDB2 in config.h or config.w32.h
 * add sqllib/include to the include path (IBM files)
 * link with db2cli.lib
 */

#if HAVE_UODBC

/* ODBC_UNIQUE_NAMES should be defined in config.h if you are
 * compiling more than one of any of the uodbc databases
 * this will give each module unique function names
 * to avoid clashes between modules.
 */

/*these defines are used to seperate the different
database modules that use the uodbc interface so that
two or more of them can be compiled into the same
php binary*/
#define UODBC_NAMED_FE(php_name, name, arg_types) { #php_name, name, arg_types }
#define UODBC_NAMED_FUNCTION(name) void name(INTERNAL_FUNCTION_PARAMETERS)

/* checking in the same order as in configure.in */
# if HAVE_SOLID
#  include <cli0core.h>
#  include <cli0ext1.h>
#  define HAVE_SQL_EXTENDED_FETCH 0
extern void php3_solid_fetch_prev(INTERNAL_FUNCTION_PARAMETERS);
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE solid
#   define UODBC_NAME "solid"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(solid_##name, php3_solid_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(solid_##php_name, php3_solid_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_solid_##name)
#   define UODBC_FNAME(name) php3i_solid_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(solid.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(solid_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_solid_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define solid_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_EMPRESS
#  include <sql.h>
#  include <sqlext.h>
#  define HAVE_SQL_EXTENDED_FETCH 0
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE empress
#   define UODBC_NAME "empress"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(empress_##name, php3_empress_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(empress_##php_name, php3_empress_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_empress_##name)
#   define UODBC_FNAME(name) php3i_empress_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(empress.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(empress_##a)
#       define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_empress_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define empress_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_ADABAS
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>
#  define HAVE_SQL_EXTENDED_FETCH 1
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE adabas
#   define UODBC_NAME "adabas"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(ada_##name, php3_ada_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(ada_##php_name, php3_ada_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_ada_##name)
#   define UODBC_FNAME(name) php3i_ada_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(ada.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(ada_##a)
#       define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_ada_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define ada_module_ptr &UODBC_MODULE_ENTRY
#  endif

# elif HAVE_IODBC  && !(WIN32|WINNT)
#  include <isql.h>
#  include <isqlext.h>
#  define HAVE_SQL_EXTENDED_FETCH 1
#  ifndef SQL_ACTIVE_STATEMENTS
#    include <odbc_types.h>
#    include <odbc_funcs.h>
#    define SQL_FD_FETCH_ABSOLUTE   0x00000010L
#    define SQL_CURSOR_DYNAMIC      2UL
#    define SQL_NO_TOTAL            (-4)
#    define SQL_SO_DYNAMIC          0x00000004L
#    define SQL_LEN_DATA_AT_EXEC_OFFSET  (-100)
#    define SQL_LEN_DATA_AT_EXEC(length) (-(length)+SQL_LEN_DATA_AT_EXEC_OFFSET)
#  endif
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE iodbc
#   define UODBC_NAME "iodbc"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(iodbc_##name, php3_iodbc_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(iodbc_##php_name, php3_iodbc_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_iodbc_##name)
#   define UODBC_FNAME(name) php3i_iodbc_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(iodbc.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(iodbc_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_iodbc_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define iodbc_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_OPENLINK

#  include <iodbc.h>
#  include <isql.h>
#  include <isqlext.h>
#  include <udbcext.h>
#  define HAVE_SQL_EXTENDED_FETCH 0 
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE opnlnk
#   define UODBC_NAME "opnlnk"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(opnlnk_##name, php3_opnlnk_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(opnlnk_##php_name, php3_opnlnk_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_opnlnk_##name)
#   define UODBC_FNAME(name) php3i_opnlnk_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(opnlnk.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(opnlnk_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_opnlnk_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define opnlnk_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_VELOCIS
/* Nothing ??? */
#  define UNIX
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <sql.h>
#  include <sqlext.h>
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE velocis
#   define UODBC_NAME "velocis"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(velocis_##name, php3_velocis_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(velocis_##php_name, php3_velocis_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_velocis_##name)
#   define UODBC_FNAME(name) php3i_velocis_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(velocis.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(velocis_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_velocis_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define velocis_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_CODBC
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <odbc.h>
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE codbc
#   define UODBC_NAME "codbc"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(codbc_##name, php3_codbc_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(codbc_##php_name, php3_codbc_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_codbc_##name)
#   define UODBC_FNAME(name) php3i_codbc_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(codbc.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(codbc_##a)
#	define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_codbc_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define codbc_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_IBMDB2
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <sqlcli1.h>
#  ifdef DB268K
/* Need to include ASLM for 68K applications */
#   include <LibraryManager.h>
#  endif
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE db2
#   define UODBC_NAME "db2"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(db2_##name, php3_db2_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(db2_##php_name, php3_db2_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_db2_##name)
#   define UODBC_FNAME(name) php3i_db2_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(db2.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(db2_##a)
#   define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_db2_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define db2_module_ptr &UODBC_MODULE_ENTRY
#  endif

#elif HAVE_INFORMIX
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <infxcli.h>
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE informix
#   define UODBC_NAME "informix"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(informix_##name, php3_informix_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(informix_##php_name, php3_informix_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_informix_##name)
#   define UODBC_FNAME(name) php3i_informix_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(informix.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(informix_##a)
#   define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_informix_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define informix_module_ptr &UODBC_MODULE_ENTRY
#  endif

# else /* MS ODBC */
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <WINDOWS.H>
#  include <sql.h>
#  include <sqlext.h>
#  if defined(UODBC_UNIQUE_NAMES)
#   define UODBC_TYPE wodbc
#   define UODBC_NAME "wodbc"
#   define UODBC_FE(name, arg_types) UODBC_NAMED_FE(wodbc_##name, php3_wodbc_##name, arg_types)
#   define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(wodbc_##php_name, php3_wodbc_##name, arg_types)
#   define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_wodbc_##name)
#   define UODBC_FNAME(name) php3i_wodbc_##name
#   define ODBC_INI_VAR_NAME(name) #name
#   define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(wuodbc.##a)
#   define UODBC_VAR_NAME(name) name
#   define UODBC_VAR(a) UODBC_VAR_NAME(wodbc_##a)
#   define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_wuodbc_##a)
#   define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#   define wodbc_module_ptr &UODBC_MODULE_ENTRY
#  endif
# endif


#if !defined(UODBC_UNIQUE_NAMES)
#define UODBC_TYPE odbc
#define UODBC_NAME "odbc"
#define UODBC_FE(name, arg_types) UODBC_NAMED_FE(odbc_##name, php3_odbc_##name, arg_types)
#define UODBC_FE_ALIAS(php_name, name, arg_types) UODBC_NAMED_FE(odbc_##php_name, php3_odbc_##name, arg_types)
#define UODBC_FUNCTION(name) UODBC_NAMED_FUNCTION(php3_odbc_##name)
#define UODBC_FNAME(name) php3i_odbc_##name
#define ODBC_INI_VAR_NAME(name) #name
#define ODBC_INI_VAR(a) ODBC_INI_VAR_NAME(uodbc.##a)
#define UODBC_VAR_NAME(name) name
#define UODBC_VAR(a) UODBC_VAR_NAME(uodbc_##a)
#define PHP3_UODBC_VAR(a) UODBC_VAR_NAME(php3_uodbc_##a)
#define UODBC_MODULE_ENTRY UODBC_VAR(module_entry)
#define uodbc_module_ptr &UODBC_MODULE_ENTRY
#else
#define UODBC_ALIAS(name, arg_types) UODBC_NAMED_FE(odbc_##name, php3_odbc_##name, arg_types)
#define UODBC_ALIAS_FE(php_name, name, arg_types) UODBC_NAMED_FE(odbc_##php_name, php3_odbc_##name, arg_types)
#endif



/*
*
*  Defines for ODBC
*
*/

/* win32 vars for thread local storage */
#define UODBC_MUTEX UODBC_VAR(mutex)
#define UODBC_TLS UODBC_VAR(Tls)
#define UODBC_GLOBAL_STRUCT UODBC_VAR(global_struct)
#define UODBC_GLOBALS UODBC_VAR(globals)
/* defines for variables, structs, etc */
#define UODBC_MODULE_NAME "ODBC/" UODBC_NAME
#define UODBC_FUNCTIONS UODBC_VAR(functions)
#define UODBC_CONNECTION UODBC_VAR(connection)
#define UODBC_RESULT_VALUE UODBC_VAR(result_value)
#define UODBC_RESULT UODBC_VAR(result)
#define UODBC_MODULE UODBC_VAR(module)
#define UODBC_MODULE_PTR UODBC_VAR(module_ptr)
#define PHP3_UODBC_MODULE PHP3_UODBC_VAR(module)

/* function defines */
#define PHP3_MINIT_UODBC UODBC_FNAME(minit)
#define PHP3_MSHUTDOWN_UODBC UODBC_FNAME(mshutdown)
#define PHP3_RINIT_UODBC UODBC_FNAME(rinit)
#define PHP3_INFO_UODBC UODBC_FNAME(info)

/* internal function defines */
#define UODBC_ADD_RESULT UODBC_FNAME(add_result)
#define UODBC_GET_RESULT UODBC_FNAME(get_result)
#define UODBC_DEL_RESULT UODBC_FNAME(del_result)
#define UODBC_ADD_CONN UODBC_FNAME(add_conn)
#define UODBC_GET_CONN UODBC_FNAME(get_conn)
#define UODBC_DEL_CONN UODBC_FNAME(del_conn)
#define UODBC_SQL_ERROR UODBC_FNAME(sql_error)
#define UODBC_BINDCOLS UODBC_FNAME(bindcols)
#define PHP3_UODBC_DO_CONNECT UODBC_FNAME(_do_connect)
#define PHP3_UODBC_TRANSACT UODBC_FNAME(_do_transact)

# ifndef MSVC5
#  define FAR
# endif

extern php3_module_entry UODBC_MODULE_ENTRY;

/* uODBC functions */
extern int PHP3_MINIT_UODBC(INIT_FUNC_ARGS);
extern int PHP3_MSHUTDOWN_UODBC(void);
extern int PHP3_RINIT_UODBC(INIT_FUNC_ARGS);
extern void PHP3_INFO_UODBC(void);
extern UODBC_FUNCTION(setoption);
extern UODBC_FUNCTION(autocommit);
extern UODBC_FUNCTION(close);
extern UODBC_FUNCTION(close_all);
extern UODBC_FUNCTION(commit);
extern UODBC_FUNCTION(connect);
extern UODBC_FUNCTION(pconnect);
extern void PHP3_UODBC_DO_CONNECT(INTERNAL_FUNCTION_PARAMETERS, int);
extern UODBC_FUNCTION(cursor);
extern UODBC_FUNCTION(exec);
/*extern UODBC_FUNCTION(do);*/
extern UODBC_FUNCTION(execute);
extern UODBC_FUNCTION(fetch_into);
extern UODBC_FUNCTION(fetch_row);
extern UODBC_FUNCTION(field_len);
extern UODBC_FUNCTION(field_name);
extern UODBC_FUNCTION(field_type);
extern UODBC_FUNCTION(field_num);
extern UODBC_FUNCTION(free_result);
extern UODBC_FUNCTION(num_fields);
extern UODBC_FUNCTION(num_rows);
extern UODBC_FUNCTION(prepare);
extern UODBC_FUNCTION(result);
extern UODBC_FUNCTION(result_all);
extern UODBC_FUNCTION(rollback);
extern void PHP3_UODBC_TRANSACT(INTERNAL_FUNCTION_PARAMETERS, int);
extern UODBC_FUNCTION(binmode);
extern UODBC_FUNCTION(longreadlen);
extern UODBC_FUNCTION(tables);

typedef struct UODBC_CONNECTION {
#if HAVE_IBMDB2
	SQLHANDLE hdbc;
#else
	HDBC hdbc;
#endif
	int open;
# if HAVE_SQL_EXTENDED_FETCH
	int fetch_abs;
# endif
} UODBC_CONNECTION;

typedef struct UODBC_RESULT_VALUE {
	char name[32];
	char *value;
	long int vallen;
	SDWORD coltype;
} UODBC_RESULT_VALUE;

typedef struct UODBC_RESULT {
#if HAVE_IBMDB2
	SQLHANDLE stmt;
#else
	HSTMT stmt;
#endif
	UODBC_RESULT_VALUE *values;
	SWORD numcols;
	SWORD numparams;
    long longreadlen;
    int binmode;
	int fetched;
	UODBC_CONNECTION *conn_ptr;
} UODBC_RESULT;

typedef struct {
#if HAVE_IBMDB2
	SQLHANDLE henv;
#else
	HENV henv;
#endif
	char *defDB;
	char *defUser;
	char *defPW;
	long allow_persistent;
	long max_persistent;
	long max_links;
	long num_persistent;
	long num_links;
	int defConn;
	int le_result, le_conn, le_pconn;
    long defaultlrl;
    long defaultbinmode;
	HashTable *resource_list;
	HashTable *resource_plist;
	HashTable *conns;
} UODBC_MODULE;

extern UODBC_MODULE PHP3_UODBC_MODULE;

int UODBC_ADD_RESULT(HashTable *list, UODBC_RESULT *result);
UODBC_RESULT *UODBC_GET_RESULT(HashTable *list, int count);
void UODBC_DEL_RESULT(HashTable *list, int count);
int UODBC_ADD_CONN(HashTable *list, HDBC conn);
UODBC_CONNECTION *UODBC_GET_CONN(HashTable *list, int count);
void UODBC_DEL_CONN(HashTable *list, int ind);
#if HAVE_IBMDB2
void UODBC_SQL_ERROR(SQLHANDLE conn, SQLHANDLE stmt, char *func);
#else
void UODBC_SQL_ERROR(HDBC conn, HSTMT stmt, char *func);
#endif
int UODBC_BINDCOLS(UODBC_RESULT *result);

#define IS_SQL_LONG(x) (x == SQL_LONGVARBINARY || x == SQL_LONGVARCHAR)
#define IS_SQL_BINARY(x) (x == SQL_BINARY || x == SQL_VARBINARY || x == SQL_LONGVARBINARY)

#else
# define uodbc_module_ptr NULL

#endif /* HAVE_UODBC || HAVE_IBMDB2 */

#endif /* _UNIFIED_ODBC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
