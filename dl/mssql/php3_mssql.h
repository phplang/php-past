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
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id: php3_mssql.h,v 1.7 2000/08/14 19:59:42 fmk Exp $ */

#ifndef _PHP3_MSSQL_H
#define _PHP3_MSSQL_H


#if COMPILE_DL
#undef HAVE_MSSQL
#define HAVE_MSSQL 1
#include "dl/phpdl.h"
#endif

#if HAVE_MSSQL
#ifndef DLEXPORT
#define DLEXPORT
#endif

#if HAVE_FREETDS
#define dbfreelogin(a) dbloginfree(a)
#define FREETDS_OPTION , -1
#define SQLINT1 SYBINT1
#define SQLINT2 SYBINT2
#define SQLINT4 SYBINT4
#define SQLINTN SYBINTN
#define SQLCHAR SYBCHAR
#define SQLVARCHAR SYBVARCHAR
#define SQLTEXT SYBTEXT
#define SQLFLT SYBFLT
#define SQLFLT8 SYBFLT8
#define SQLFLTN SYBFLTN
#define SQLNUMERIC SYBNUMERIC
#define SQLDECIMAL SYBDECIMAL
#define SQLMONEY SYBMONEY
#define SQLMONEY4 SYBMONEY4
#define SQLMONEYN SYBMONEYN
#define SQLBINARY SYBBINARY
#define SQLVARBINARY SYBVARBINARY
#define SQLDATETIME SYBDATETIME
#define SQLDATETIM4 SYBDATETIME4
#define SQLDATETIMN SYBDATETIMN
#define SQLBIT SYBBIT
#define SQLIMAGE SYBIMAGE
#else
#define FREETDS_OPTION
#endif

extern php3_module_entry mssql_module_entry;
#define mssql_module_ptr &mssql_module_entry


int php3_minit_mssql(INIT_FUNC_ARGS);
int php3_rinit_mssql(INIT_FUNC_ARGS);
int php3_mshutdown_mssql(void);
int php3_rshutdown_mssql(void);
void php3_info_mssql(void);
void php3_mssql_connect(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_pconnect(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_close(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_select_db(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_query(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_free_result(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_get_last_message(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_num_rows(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_num_fields(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_fetch_field(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_fetch_row(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_fetch_array(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_fetch_object(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_field_length(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_field_name(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_field_type(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_data_seek(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_field_seek(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_result(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_min_error_severity(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_min_message_severity(INTERNAL_FUNCTION_PARAMETERS);

/*
void php3_mssql_db_query(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_list_fields(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_fetch_lengths(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_field_table(INTERNAL_FUNCTION_PARAMETERS);
void php3_mssql_field_flags(INTERNAL_FUNCTION_PARAMETERS);
*/


#include "sqlfront.h"
#include "sqldb.h"
//#include <sqlerror.h>

#define coltype(j) dbcoltype(mssql_ptr->link,j)
#define intcol(i) ((int) *(DBINT *) dbdata(mssql_ptr->link,i))
#define smallintcol(i) ((int) *(DBSMALLINT *) dbdata(mssql_ptr->link,i))
#define tinyintcol(i) ((int) *(DBTINYINT *) dbdata(mssql_ptr->link,i))
#define anyintcol(j) (coltype(j)==SQLINT4?intcol(j):(coltype(j)==SQLINT2?smallintcol(j):tinyintcol(j)))
#define charcol(i) ((DBCHAR *) dbdata(mssql_ptr->link,i))
#define floatcol(i) ((float) *(DBFLT8 *) dbdata(mssql_ptr->link,i))


typedef struct mssql_link_struct {
	LOGINREC *login;
	DBPROCESS *link;
	int valid;
}mssql_link;

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	char *appname;
	char *server_message;
	int le_link,le_plink,le_result;
	long magic_quotes_runtime;
	long min_error_severity,min_message_severity;
	long cfg_min_error_severity,cfg_min_message_severity;
	void (*get_column_content)(mssql_link *mssql_ptr,int offset,pval *result,int column_type);
	long textsize, textlimit; 
	HashTable *resource_list, *resource_plist;
} mssql_module;

#define MSSQL_ROWS_BLOCK 128

typedef struct {
	char *name,*column_source;
	long max_length; 
	int numeric;
	int type;
} mssql_field;

typedef struct {
	pval **data;
	mssql_field *fields;
	mssql_link *mssql_ptr;
	int cur_row,cur_field;
	int num_rows,num_fields;
} mssql_result;

extern mssql_module php3_mssql_module;

#else

#define mssql_module_ptr NULL

#endif

#endif /* _PHP3_MSSQL_H */
