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
   |                                                                      |
   +----------------------------------------------------------------------+
 */


/* $Id: adabasd.h,v 1.20 2000/01/01 04:44:09 sas Exp $ */

#ifndef _ADABASD_H
#define _ADABASD_H

#if HAVE_ADABAS && !HAVE_UODBC

extern php3_module_entry adabas_module_entry;
#define adabas_module_ptr &adabas_module_entry

/* Adabas D functions */
extern int php3_minit_adabas(INIT_FUNC_ARGS);
extern int php3_mshutdown_adabas(void);
extern int php3_rinit_adabas(INIT_FUNC_ARGS);
extern void php3_info_adabas(void);
extern void php3_Ada_autocommit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_closeAll(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_commit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_exec(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_aFetch(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_fetchRow(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_fieldLen(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_fieldName(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_fieldType(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_freeResult(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_numFields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_numRows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_resultAll(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ada_rollback(INTERNAL_FUNCTION_PARAMETERS);

#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>

typedef struct ResultValue {
        char name[32];
        char *value;
        long int vallen;
        int passthru;
} ResultValue;

typedef struct AdaResult {
        HSTMT stmt;
        ResultValue *values;
        int numcols;
		int fetch_abs;
        int fetched;
		HDBC hdbc;
} AdaResult;

typedef struct {
	HENV henv;
	char *defDB;
	char *defUser;
	char *defPW;
	int defConn;
	int le_result,le_conn;
} adabas_module;

extern adabas_module php3_adabas_module;

int ada_add_result(HashTable *list, AdaResult *result);
AdaResult *ada_get_result(HashTable *list, int count);
void ada_del_result(HashTable *list, int count);
int ada_add_conn(HashTable *list, HDBC conn);
void make_def_conn(HashTable *list);
HDBC ada_get_conn(HashTable *list, int count);
void ada_del_conn(HashTable *list, int ind);
void ada_sql_error(HDBC conn, HSTMT stmt, char *func);

#else

#define adabas_module_ptr NULL

#endif

#endif /* _ADABASD_H */
