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
   | Authors: Stig Sæther Bakken <ssb@guardian.no>                        |
   |                                                                      |
   | Initial work sponsored by Thies Arntzen <thies@digicol.de> of        |
   | Digital Collections, http://www.digicol.de/                          |
   +----------------------------------------------------------------------+
 */

/* $Id */

#if HAVE_OCI8
# ifndef _PHP_OCI8_H
#  define _PHP_OCI8_H
# endif

# if (defined(__osf__) && defined(__alpha))
#  ifndef A_OSF
#   define A_OSF
#  endif
#  ifndef OSF1
#   define OSF1
#  endif
#  ifndef _INTRINSICS
#   define _INTRINSICS
#  endif
# endif /* osf alpha */

# include <oci.h>

# if 0
typedef struct {
    OCIEnv *pEnv;
} oci8_request;
# endif

typedef struct {
	int id;
    OCIEnv *pEnv;
    OCIServer *pServer;
    OCISvcCtx *pServiceContext;
    OCIError *pError;
	OCISession *pSession;
	HashTable *descriptors;
	int descriptors_count;
    int open;
} oci8_connection;

typedef struct {
	dvoid *ocidescr;
	ub4 type;
} oci8_descriptor;

typedef struct {
    pval *pval;
    text *name;
    ub4 name_len;
	ub4 type;
	char *data;			/* for pval cache */
} oci8_define;


typedef struct {
	int id;
	oci8_connection *conn;
    OCIError *pError;
    OCIStmt *pStmt;
	text *last_query;
	HashTable *columns;
	int ncolumns;
	HashTable *binds;
	HashTable *defines;
	int executed;
} oci8_statement;

typedef struct {
	OCIBind *pBind;
	pval *value;
	dvoid *descr;		/* used for binding of LOBS etc */
	ub4 maxsize;
	sb2 indicator;
	ub2 retcode;
} oci8_bind;

typedef struct {
	oci8_statement *statement;
	OCIDefine *pDefine;
    text *name;
    ub4 name_len;
    ub2 type;
    ub4 size4;
    ub4 storage_size4;
	ub2 size2;
	sb2 indicator;
	ub2 retcode;
	ub4 rlen;
	ub2 is_descr;
    int descr;
    oci8_descriptor *pdescr;
	void *data;
	oci8_define *define;
} oci8_out_column;

typedef struct {
    char *default_username;
    char *default_password;
    char *default_dbname;
    long debug_mode;
    long allow_persistent;
    long max_persistent;
    long max_links;
    long num_persistent;
    long num_links;
    int le_conn;
    int le_stmt; 
} oci8_module;

extern php3_module_entry oci8_module_entry;
# define oci8_module_ptr &oci8_module_entry

# define OCI8_MAX_NAME_LEN 64
# define OCI8_MAX_DATA_SIZE 2097152 /* two megs */

/* this one has to be changed to include persistent connections as well */
# define OCI8_CONN_TYPE(x) ((x)==OCI8_GLOBAL(php3_oci8_module).le_conn)
# define OCI8_STMT_TYPE(x) ((x)==OCI8_GLOBAL(php3_oci8_module).le_stmt)
# define OCI8_DESCR_TYPE(x) ((x)==OCI8_GLOBAL(php3_oci8_module).le_descr)

# define RETURN_OUT_OF_MEMORY \
	php3_error(E_WARNING, "Out of memory");\
	RETURN_FALSE
# define OCI8_FAIL(c,f,r) \
	php3i_oci8_error((c)->pError,(f),(r));\
	RETURN_FALSE

void php3_oci_logon(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci_plogon(INTERNAL_FUNCTION_PARAMETERS);
void php3_oci_do_logon(INTERNAL_FUNCTION_PARAMETERS, int persistent);

#else /* !HAVE_OCI8 */

# define oci8_module_ptr NULL

#endif /* HAVE_OCI8 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
