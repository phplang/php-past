/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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
   | Authors: Jouni Ahto <jah@cultnet.fi>                                 |
   |          Andrew Avdeev <andy@simgts.mv.ru>                           |
   +----------------------------------------------------------------------+
 */

/* $Id: php3_interbase.h,v 1.5 1999/01/31 10:33:10 andrew Exp $ */

#ifndef _PHP3_IBASE_H
#define _PHP3_IBASE_H

#if COMPILE_DL
#undef HAVE_IBASE
#define HAVE_IBASE 1
#endif

#if HAVE_IBASE
#include <ibase.h>

extern php3_module_entry ibase_module_entry;
#define php3_ibase_module_ptr &ibase_module_entry

extern int php3_minit_ibase(INIT_FUNC_ARGS);
extern int php3_rinit_ibase(INIT_FUNC_ARGS);
extern int php3_mfinish_ibase(void);
extern void php3_info_ibase(void);
extern void php3_ibase_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_pconnect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_query(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_fetch_row(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_fetch_object(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_free_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_prepare(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_bind(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_execute(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_free_query(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_timefmt(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ibase_num_fields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_field_info(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ibase_trans(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_commit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_rollback(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ibase_blob_create(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_blob_add(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_blob_cancel(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_blob_open(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_blob_get(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_blob_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_blob_echo(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_blob_info(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ibase_blob_import(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ibase_errmsg(INTERNAL_FUNCTION_PARAMETERS);

#define IBASE_MSGSIZE 256
#define MAX_ERRMSG (IBASE_MSGSIZE*2)

typedef struct {
	long default_link;
	long num_links, num_persistent;
	long max_links, max_persistent;
	long allow_persistent;
	int le_blob, le_trans, le_link, le_plink, le_result, le_query;
	char *default_user, *default_password;
	char *timeformat;
    char *cfg_timeformat;
    char *errmsg;
} ibase_module;

typedef struct {
    isc_tr_handle trans;
} ibase_trans;

typedef struct {
    isc_tr_handle default_trans; /* default transaction*/
	isc_db_handle link;
} ibase_db_link;

typedef struct {
    isc_tr_handle trans_handle; 
    isc_db_handle link;
    ISC_QUAD bl_qd;
    isc_blob_handle bl_handle;
} ibase_blob_handle;

typedef struct _php3_ibase_result {
	isc_stmt_handle result;
    isc_tr_handle trans_handle;
    int trans_single; /* commit transaction when free result */
	isc_db_handle link; /* db link for this result */
	XSQLDA *sqlda;
} ibase_result_handle;

typedef struct _php3_ibase_query {
	isc_stmt_handle query;
    isc_tr_handle trans_handle;
    int trans_single; /* commit transaction when free query */
	XSQLDA *sqlda;
	int alloced;
} ibase_query_handle;

typedef struct _php3_ibase_varchar {
    short var_len;
    char var_str[1];
} IBASE_VCHAR;

extern ibase_module php3_ibase_module;

#else

#define php3_ibase_module_ptr NULL

#endif /* HAVE_IBASE */

#endif /* _PHP3_IBASE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
