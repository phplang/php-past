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

/* $Id: php3_unified_odbc.h,v 1.16 1998/02/09 22:49:38 shane Exp $ */

#ifndef _UNIFIED_ODBC_H
#define _UNIFIED_ODBC_H

#if HAVE_UODBC

/* checking in the same order as in configure.in */
# if HAVE_SOLID
#  include <cli0core.h>
#  include <cli0ext1.h>
#  define HAVE_SQL_EXTENDED_FETCH 0
# elif HAVE_IODBC  && !MSVC5
#  include <isql.h>
#  include <isqlext.h>
#  include <odbc_types.h>
#  include <odbc_funcs.h>
#  define HAVE_SQL_EXTENDED_FETCH 1
#  define SQL_FD_FETCH_ABSOLUTE   0x00000010L
#  define SQL_CURSOR_DYNAMIC      2UL
#  define SQL_NO_TOTAL            (-4)
#  define SQL_SO_DYNAMIC          0x00000004L
#  define SQL_LEN_DATA_AT_EXEC_OFFSET  (-100)
#  define SQL_LEN_DATA_AT_EXEC(length) (-(length)+SQL_LEN_DATA_AT_EXEC_OFFSET)

# else
#  define HAVE_SQL_EXTENDED_FETCH 1
#  include <WINDOWS.H>
#  include <sql.h>
#  include <sqlext.h>
# endif

# ifndef MSVC5
#  define FAR
# endif

extern php3_module_entry uodbc_module_entry;
# define uodbc_module_ptr &uodbc_module_entry

/* uODBC functions */
extern int php3_minit_uodbc(INITFUNCARG);
extern int php3_mshutdown_uodbc(void);
extern int php3_rinit_uodbc(INITFUNCARG);
extern void php3_info_uodbc(void);
extern void php3_uodbc_setoption(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_autocommit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_close_all(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_commit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_pconnect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_do_connect(INTERNAL_FUNCTION_PARAMETERS, int);
extern void php3_uodbc_cursor(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_do(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_execute(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_fetch_into(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_fetch_row(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_field_len(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_field_name(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_field_type(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_free_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_num_fields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_num_rows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_prepare(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_result_all(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_rollback(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_uodbc_transact(INTERNAL_FUNCTION_PARAMETERS, int);

typedef struct uodbc_connection {
	HDBC hdbc;
	int open;
} uodbc_connection;

typedef struct uodbc_result_value {
	char name[32];
	char *value;
	long int vallen;
	int passthru;
} uodbc_result_value;

typedef struct uodbc_result {
	HSTMT stmt;
	uodbc_result_value *values;
	SWORD numcols;
	SWORD numparams;
# if HAVE_SQL_EXTENDED_FETCH
	int fetch_abs;
# endif
	int fetched;
	uodbc_connection *conn_ptr;
} uodbc_result;

typedef struct {
	HENV henv;
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
	HashTable *resource_list;
	HashTable *resource_plist;
} uodbc_module;

# ifndef THREAD_SAFE
extern uodbc_module php3_odbc_module;
# endif

int uodbc_add_result(HashTable *list, uodbc_result *result);
uodbc_result *uodbc_get_result(HashTable *list, int count);
void uodbc_del_result(HashTable *list, int count);
int uodbc_add_conn(HashTable *list, HDBC conn);
uodbc_connection *uodbc_get_conn(HashTable *list, int count);
void uodbc_del_conn(HashTable *list, int ind);
void uodbc_sql_error(HDBC conn, HSTMT stmt, char *func);
int uodbc_bindcols(uodbc_result *result);
#else

# define uodbc_module_ptr NULL

#endif /* HAVE_UODBC */

#endif /* _UNIFIED_ODBC_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
