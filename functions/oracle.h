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

/*  $Id: oracle.h,v 1.20 1998/02/01 22:10:52 zeev Exp $ */

#ifndef _PHP3_ORACLE_H
#define _PHP3_ORACLE_H

#if HAVE_ORACLE

#include "oratypes.h"
#include "ocidfn.h"
#ifdef __STDC__
#include "ociapr.h"

extern php3_module_entry oracle_module_entry;
#define oracle_module_ptr &oracle_module_entry

/* The maximum number of Oracle connections in a single document */
#define MAX_CONNECTIONS 15

/* The maximum number of open Oracle cursors */
#define MAX_CURSORS 50

/* oparse flags */
#define  DEFER_PARSE        1
#define  NATIVE             1
#define  VERSION_7          2

#define ORANUMWIDTH			38

#if (defined(__osf__) && defined(__alpha)) || defined(CRAY) || defined(KSR)
#define HDA_SIZE 512
#else
#define HDA_SIZE 256
#endif

#define ORAUIDLEN 32
#define ORAPWLEN 32
#define ORANAMELEN 32
#define ORABUFLEN 2000

/* Some Oracle error codes */
#define VAR_NOT_IN_LIST			1007
#define NO_DATA_FOUND			1403
#define NULL_VALUE_RETURNED		1405

/* Some Oracle types */
#define VARCHAR2_TYPE		1
#define NUMBER_TYPE			2
#define INT_TYPE			3
#define FLOAT_TYPE			4
#define STRING_TYPE			5
#define ROWID_TYPE			11
#define DATE_TYPE			12

/* Some SQL and OCI function codes */
#define FT_INSERT			3
#define FT_SELECT			4
#define FT_UPDATE			5
#define FT_DELETE			9

#define FC_OOPEN			14

typedef struct {
	int open;
	int ind;
	Lda_Def lda;
	ub1 hda[HDA_SIZE];
	char userid[ORAUIDLEN];
	char password[ORAPWLEN];
} oraConnection;

typedef struct oraColumn {
	sb4 dbsize;
	sb2 dbtype;
	text cbuf[ORANAMELEN+1];
	sb4 cbufl;
	sb4 dsize;
	sb2 prec;
	sb2 scale;
	sb2 nullok;
	float flt_buf;
	sword int_buf;
	ub1 *buf;
	sb2 indp;
	ub2 col_retlen, col_retcode;
} oraColumn;

typedef struct oraCursor {
	int open;
	int ind;
	Cda_Def cda;
	text *current_query;
	HashTable *columns;
#endif
	int ncols;
} oraCursor;

typedef struct {
	char *defDB;
	char *defUser;
	char *defPW;
	long allow_persistent;
	long max_persistent;
	long max_links;
	long num_persistent;
	long num_links;
	int le_conn, le_pconn, le_cursor, le_column;
} oracle_module;

extern void php3_Ora_Close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_Commit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_CommitOff(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_CommitOn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_Error(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_ErrorCode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_Exec(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_Fetch(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_GetColumn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_Logoff(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_Logon(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_Open(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_Parse(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_Ora_Rollback(INTERNAL_FUNCTION_PARAMETERS);
extern int php3_minit_oracle(INITFUNCARG);
extern int php3_mshutdown_oracle(void);
extern void php3_info_oracle(void);

#else

#define oracle_module_ptr NULL

#endif /* HAVE_ORACLE */

#endif /* _PHP3_ORACLE_H */

/*
 * Local Variables:
 * tab-width: 4
 * End:
 */
