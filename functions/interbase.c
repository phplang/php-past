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

/* $Id: interbase.c,v 1.11 1999/01/31 17:57:27 andrew Exp $ */

/* TODO: A lot... */

#include "config.h"
#include "php.h"
#include "internal_functions.h"
#include "php3_interbase.h"

#if HAVE_IBASE
#include <ibase.h>
#include <time.h>
#include "php3_list.h"
#include "php3_string.h"

/* {{{ extension definition structures */
function_entry ibase_functions[] = {
	{"ibase_connect",		php3_ibase_connect,		NULL},
	{"ibase_pconnect",		php3_ibase_pconnect,	NULL},
	{"ibase_close",			php3_ibase_close,		NULL},
	{"ibase_query",			php3_ibase_query,		NULL},
	{"ibase_fetch_row",		php3_ibase_fetch_row,	NULL},
	{"ibase_fetch_object",	php3_ibase_fetch_object,NULL},
	{"ibase_free_result",	php3_ibase_free_result,	NULL},
	{"ibase_prepare",		php3_ibase_prepare,		NULL},
	{"ibase_bind",			php3_ibase_bind,		NULL},
	{"ibase_execute",		php3_ibase_execute,		NULL},
	{"ibase_free_query",	php3_ibase_free_query,	NULL},
    {"ibase_timefmt",		php3_ibase_timefmt,		NULL},
    
	{"ibase_num_fields",	php3_ibase_num_fields,	NULL},
    {"ibase_field_info",	php3_ibase_field_info,	NULL},
    
	{"ibase_trans",			php3_ibase_trans,		NULL},
	{"ibase_commit",		php3_ibase_commit,		NULL},
    {"ibase_rollback",		php3_ibase_rollback,	NULL},
    
    {"ibase_blob_info",	    php3_ibase_blob_info,   NULL},
    {"ibase_blob_create",	php3_ibase_blob_create,	NULL},
	{"ibase_blob_add",	    php3_ibase_blob_add,	NULL},
	{"ibase_blob_cancel",	php3_ibase_blob_cancel,	NULL},
	{"ibase_blob_close",	php3_ibase_blob_close,	NULL},
	{"ibase_blob_open",		php3_ibase_blob_open,	NULL},
    {"ibase_blob_get",	    php3_ibase_blob_get,	NULL},
    {"ibase_blob_echo",	    php3_ibase_blob_echo,   NULL},
    {"ibase_blob_import",	php3_ibase_blob_import, NULL},
    
    {"ibase_errmsg",	    php3_ibase_errmsg,      NULL},
    {NULL, NULL, NULL}
};

php3_module_entry ibase_module_entry =
{
	"InterBase",
	ibase_functions,
	php3_minit_ibase,
	NULL,
	php3_rinit_ibase,
	NULL,
	/*php3_info_ibase*/ NULL,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ prototype for blob destructor */
static void _php3_ibase_free_blob(ibase_blob_handle *ib_blob);
/* }}} */


/* {{{ thread safety stuff */
#if defined(THREAD_SAFE)
typedef ibase_global_struct{
	ibase_module php3_ibase_module;
} ibase_global_struct;

#define IBASE_GLOBAL(a) ibase_globals->a

#define IBASE_TLS_VARS \
		ibase_global_struct *ibase_globals; \
		ibase_globals=TlsGetValue(IBASETls); 

#else
#define IBASE_GLOBAL(a) a
#define IBASE_TLS_VARS
ibase_module php3_ibase_module;
#endif
/* }}} */



#define RESET_ERRMSG { IBASE_GLOBAL(php3_ibase_module).errmsg[0] = '\0';}
#define TEST_ERRMSG ( IBASE_GLOBAL(php3_ibase_module).errmsg[0] != '\0')

/* {{{ proto string ibase_errmsg()
   Return error message */
void php3_ibase_errmsg(INTERNAL_FUNCTION_PARAMETERS)
{
    char *errmsg = IBASE_GLOBAL(php3_ibase_module).errmsg;
    IBASE_TLS_VARS;
    if(errmsg[0]){
        RETURN_STRING(errmsg,1);
    }
    RETURN_FALSE;
}
/* }}} */

/* {{{ _php3_ibase_error(ISC_STATUS *status)
   print interbase error and save it for ibase_errmsg() */
static void _php3_ibase_error(ISC_STATUS *status)
{
    char *s, *errmsg = IBASE_GLOBAL(php3_ibase_module).errmsg;

	s = errmsg;
	while((s - errmsg) < MAX_ERRMSG - (IBASE_MSGSIZE + 2) && isc_interprete(s, &status)){
        strcat(errmsg, " ");
		s = errmsg + strlen(errmsg);
	}
	php3_error(E_WARNING, "InterBase: %s",errmsg);
}
/* }}} */

/* {{{ _php3_module_error(char *msg, ...)
   print php interbase module error  and save it for ibase_errmsg() */
static void _php3_module_error(char *msg, ...)
{
    char *errmsg = IBASE_GLOBAL(php3_ibase_module).errmsg;
    va_list ap;
    int len;

	va_start(ap, msg);
	len = vsnprintf(errmsg, MAX_ERRMSG - 1, msg, ap);
    va_end(ap);
    errmsg[len] = '\0';
    
	php3_error(E_WARNING, "InterBase module: %s",errmsg);
}
/* }}} */

/* {{{ _php3_ibase_rollback_trans() */
static void _php3_ibase_rollback_trans(ibase_trans *ib_trans)
{
	ISC_STATUS status[20];
	IBASE_TLS_VARS;
	
	if(ib_trans->trans != NULL){
		if(isc_rollback_transaction(status, &ib_trans->trans)){
			_php3_ibase_error(status);
		}
	}
	efree(ib_trans);
}
/* }}} */

/* {{{ _php3_ibase_close_link() */
static void _php3_ibase_close_link(ibase_db_link *link)
{
	ISC_STATUS status[20];
	IBASE_TLS_VARS;

	if(link->default_trans != NULL){
		if(isc_rollback_transaction(status, &link->default_trans)){
			_php3_ibase_error(status);
		}
	}
	isc_detach_database(status, &link->link);
	IBASE_GLOBAL(php3_ibase_module).num_links--;
	efree(link);
}
/* }}} */

/* {{{ _php3_ibase_close_plink() */
static void _php3_ibase_close_plink(ibase_db_link *link)
{
	ISC_STATUS status[20];
	IBASE_TLS_VARS;

	if(link->default_trans != NULL){
		if(isc_rollback_transaction(status, &link->default_trans)){
			_php3_ibase_error(status);
		}
	}
	isc_detach_database(status, &link->link);
	php3_ibase_module.num_persistent--;
	php3_ibase_module.num_links--;
	free(link);
}
/* }}} */

/* {{{ _php3_ibase_free_result() */
static void _php3_ibase_free_result(ibase_result_handle *result)
{
	int i;
	ISC_STATUS status[20];
	IBASE_TLS_VARS;

 	if (result->trans_single) { /*FIXME : commit or rollback?*/
 		isc_commit_transaction(status, &result->trans_handle);
	}
	if (result->sqlda != NULL) {
		for (i = 0; i < result->sqlda->sqld; i++) {
			efree(result->sqlda->sqlvar[i].sqldata);
			efree(result->sqlda->sqlvar[i].sqlind);
		}
		efree(result->sqlda);
	}
	isc_dsql_free_statement(status, &result->result, DSQL_drop);
	efree(result);
}
/* }}} */

/* {{{ _php3_ibase_free_query() */
static void _php3_ibase_free_query(ibase_query_handle *query)
{
	int i;
	ISC_STATUS status[20];

 	if (query->trans_single) { /*FIXME : commit or rollback?*/
 		isc_commit_transaction(status, &query->trans_handle);
	}
	if (query->sqlda != NULL) {
		if (query->alloced) {
			for (i = 0; i < query->sqlda->sqld; i++) {
				efree(query->sqlda->sqlvar[i].sqldata);
				efree(query->sqlda->sqlvar[i].sqlind);
			}
		}
		efree(query->sqlda);
	}
	isc_dsql_free_statement(status, &query->query, DSQL_drop);
	efree(query);
}
/* }}} */

/* {{{ startup, shutdown and info functions */
int php3_minit_ibase(INIT_FUNC_ARGS)
{
	IBASE_TLS_VARS;

	if (cfg_get_long("ibase.allow_persistent", &IBASE_GLOBAL(php3_ibase_module).allow_persistent) == FAILURE) {
		IBASE_GLOBAL(php3_ibase_module).allow_persistent = 1;
	}
	if (cfg_get_long("ibase.max_persistent", &IBASE_GLOBAL(php3_ibase_module).max_persistent) == FAILURE) {
		IBASE_GLOBAL(php3_ibase_module).max_persistent = -1;
	}
	if (cfg_get_long("ibase.max_links", &IBASE_GLOBAL(php3_ibase_module).max_links) == FAILURE) {
		IBASE_GLOBAL(php3_ibase_module).max_links = -1;
	}
	if (cfg_get_string("ibase.default_user", &IBASE_GLOBAL(php3_ibase_module).default_user) == FAILURE
		|| IBASE_GLOBAL(php3_ibase_module).default_user[0] == 0) {
		IBASE_GLOBAL(php3_ibase_module).default_user = "";
	}
	if (cfg_get_string("ibase.default_password", &IBASE_GLOBAL(php3_ibase_module).default_password) == FAILURE
		|| IBASE_GLOBAL(php3_ibase_module).default_password[0] == 0) {	
		IBASE_GLOBAL(php3_ibase_module).default_password = "";
	}
	if (cfg_get_string("ibase.timeformat", &IBASE_GLOBAL(php3_ibase_module).timeformat) == FAILURE) {
		IBASE_GLOBAL(php3_ibase_module).cfg_timeformat = "%Y-%m-%d %H:%M:%S";
	}
	IBASE_GLOBAL(php3_ibase_module).num_persistent=0;
	IBASE_GLOBAL(php3_ibase_module).le_result = register_list_destructors(_php3_ibase_free_result, NULL);
	IBASE_GLOBAL(php3_ibase_module).le_query = register_list_destructors(_php3_ibase_free_query, NULL);
	IBASE_GLOBAL(php3_ibase_module).le_trans = register_list_destructors(_php3_ibase_rollback_trans, NULL);
	IBASE_GLOBAL(php3_ibase_module).le_blob = register_list_destructors(_php3_ibase_free_blob, NULL);
	IBASE_GLOBAL(php3_ibase_module).le_link = register_list_destructors(_php3_ibase_close_link, NULL);
	IBASE_GLOBAL(php3_ibase_module).le_plink = register_list_destructors(NULL, _php3_ibase_close_plink);
	return SUCCESS;
}

int php3_rinit_ibase(INIT_FUNC_ARGS)
{
	IBASE_TLS_VARS;

	IBASE_GLOBAL(php3_ibase_module).default_link= -1;
	IBASE_GLOBAL(php3_ibase_module).num_links = php3_ibase_module.num_persistent;
	IBASE_GLOBAL(php3_ibase_module).timeformat = estrndup(IBASE_GLOBAL(php3_ibase_module).cfg_timeformat, strlen(IBASE_GLOBAL(php3_ibase_module).cfg_timeformat));
	IBASE_GLOBAL(php3_ibase_module).errmsg = emalloc(sizeof(char)*MAX_ERRMSG);
	return SUCCESS;
}

/* TODO IF NEEDED

int php3_mfinish_ibase(void)
{
}
*/

int php3_rfinish_ibase(void)
{
	IBASE_TLS_VARS;

	efree(IBASE_GLOBAL(php3_ibase_module).timeformat);
	efree(IBASE_GLOBAL(php3_ibase_module).errmsg);
	return SUCCESS;
}

void php3_info_ibase(void)
{
	/* TODO */
}
/* }}} */

/* {{{ _php_ibase_attach_db() */
static int _php_ibase_attach_db(char *server, char *uname, char *passwd, char *charset, int buffers, char *role, isc_db_handle *db)
{
	char dpb_buffer[256], *dpb, *p;
	int dpb_length, len;
	ISC_STATUS status[20];

	dpb = dpb_buffer;

	*dpb++ = isc_dpb_version1;

	if (uname != NULL && (len = strlen(uname))) {
		*dpb++ = isc_dpb_user_name;
		*dpb++ = len;
		for (p = uname; *p;) {
			*dpb++ = *p++;
		}
	}

	if (passwd != NULL && (len = strlen(passwd))) {
		*dpb++ = isc_dpb_password;
		*dpb++ = strlen(passwd);
		for (p = passwd; *p;) {
			*dpb++ = *p++;
		}
	}

	if (charset != NULL && (len = strlen(charset))) {
		*dpb++ = isc_dpb_lc_ctype;
		*dpb++ = strlen(charset);
		for (p = charset; *p;) {
			*dpb++ = *p++;
		}
	}

	if (buffers) {
		*dpb++ = isc_dpb_num_buffers;
		*dpb++ = 1;
		*dpb++ = buffers;
	}

#ifdef isc_dpb_sql_role_name
	if (role != NULL && (len = strlen(role))) {
		*dpb++ = isc_dpb_sql_role_name;
		*dpb++ = strlen(role);
		for (p = role; *p;) {
			*dpb++ = *p++;
		}
	}
#endif

	dpb_length = dpb - dpb_buffer;

    if(isc_attach_database(status, strlen(server), server, db, dpb_length, dpb_buffer)){
			_php3_ibase_error(status);
			return 1;
	}
	return 0;
}
/* }}} */

/* {{{ _php3_ibase_connect() */
static void _php3_ibase_connect(INTERNAL_FUNCTION_PARAMETERS, int persistent)
{
	pval *server, *uname, *passwd;

	char *ib_server, *ib_uname, *ib_passwd;
	int ib_server_len, ib_uname_len, ib_passwd_len;
	isc_db_handle db_handle = NULL;
	char *hashed_details;
	int hashed_details_length;
	ibase_db_link *ib_link;
    IBASE_TLS_VARS;
    
    RESET_ERRMSG;
        
	ib_uname = IBASE_GLOBAL(php3_ibase_module).default_user;
	ib_passwd = IBASE_GLOBAL(php3_ibase_module).default_password;
	ib_uname_len = ib_uname ? strlen(ib_uname) : 0;
	ib_passwd_len = ib_passwd ? strlen(ib_passwd) : 0;

	switch(ARG_COUNT(ht)) {
	case 1:
		{
			if (getParameters(ht, 1, &server) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(server);
			ib_server = server->value.str.val;
			ib_server_len = server->value.str.len;
			hashed_details_length = server->value.str.len+ib_uname_len+ib_passwd_len+5+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			sprintf(hashed_details, "ibase_%s_%s_%s", ib_server, ib_uname, ib_passwd);
		}
		break;
	case 2:
		{
			if (getParameters(ht, 2, &server, &uname) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(server);
			convert_to_string(uname);
			ib_server = server->value.str.val;
			ib_uname = uname->value.str.val;
			ib_server_len = server->value.str.len;
			ib_uname_len = uname->value.str.len;
			hashed_details_length = server->value.str.len+uname->value.str.len+ib_passwd_len+5+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			sprintf(hashed_details, "ibase_%s_%s_%s", ib_server, ib_uname, ib_passwd);
		}
		break;
	case 3:
		{
			if (getParameters(ht, 3, &server, &uname, &passwd) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(server);
			convert_to_string(uname);
			convert_to_string(passwd);
			ib_server = server->value.str.val;
			ib_uname = uname->value.str.val;
			ib_passwd = passwd->value.str.val;
			ib_server_len = server->value.str.len;
			ib_uname_len = uname->value.str.len;
			ib_passwd_len = passwd->value.str.len;
			hashed_details_length = server->value.str.len+uname->value.str.len+passwd->value.str.len+5+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			sprintf(hashed_details, "ibase_%s_%s_%s", ib_server, ib_uname, ib_passwd);
		}
		break;
	default:
		WRONG_PARAM_COUNT;
		break;
	}

	if (persistent) {
		list_entry *le;
		
		if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {
			list_entry new_le;
			
			if (IBASE_GLOBAL(php3_ibase_module).max_links!=-1 && IBASE_GLOBAL(php3_ibase_module).num_links>=IBASE_GLOBAL(php3_ibase_module).max_links) {
				_php3_module_error("Too many open links (%d)", IBASE_GLOBAL(php3_ibase_module).num_links);
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (IBASE_GLOBAL(php3_ibase_module).max_persistent!=-1 && IBASE_GLOBAL(php3_ibase_module).num_persistent>=IBASE_GLOBAL(php3_ibase_module).max_persistent) {
				_php3_module_error("Too many open persistent links (%d)", IBASE_GLOBAL(php3_ibase_module).num_persistent);
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* create the ib_link */

			if (_php_ibase_attach_db(ib_server, ib_uname, ib_passwd, NULL, 0, NULL, &db_handle)) {
				efree(hashed_details);
				RETURN_FALSE;
			}

			ib_link = (ibase_db_link *) malloc(sizeof(ibase_db_link));
			ib_link->link = db_handle;
			ib_link->default_trans = NULL;
			/* hash it up */
			new_le.type = php3_ibase_module.le_plink;
			new_le.ptr = ib_link;
			if (_php3_hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				efree(hashed_details);
				efree(ib_link);
				RETURN_FALSE;
			}
			IBASE_GLOBAL(php3_ibase_module).num_links++;
			IBASE_GLOBAL(php3_ibase_module).num_persistent++;
		} else {
			if (le->type != IBASE_GLOBAL(php3_ibase_module).le_plink) {
				RETURN_FALSE;
			}
			/* TODO: ensure that the ib_link did not die */
			ib_link = (ibase_db_link *) le->ptr;
			ib_link->default_trans = NULL;
		}
		return_value->value.lval = php3_list_insert(ib_link, IBASE_GLOBAL(php3_ibase_module).le_plink);
		return_value->type = IS_LONG;
	} else {
		list_entry *index_ptr, new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual ib_link sits.
		 * if it doesn't, open a new ib_link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (_php3_hash_find(list,hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type,xlink;
			void *ptr;

			if (index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}
			xlink = (int) index_ptr->ptr;
			ptr = php3_list_find(xlink,&type);   /* check if the xlink is still there */
			if (ptr && (type==IBASE_GLOBAL(php3_ibase_module).le_link || type==IBASE_GLOBAL(php3_ibase_module).le_plink)) {
				return_value->value.lval = IBASE_GLOBAL(php3_ibase_module).default_link = xlink;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				_php3_hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (IBASE_GLOBAL(php3_ibase_module).max_links!=-1 && IBASE_GLOBAL(php3_ibase_module).num_links>=IBASE_GLOBAL(php3_ibase_module).max_links) {
			_php3_module_error("Too many open links (%d)", IBASE_GLOBAL(php3_ibase_module).num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		/* create the ib_link */

		if (_php_ibase_attach_db(ib_server, ib_uname, ib_passwd, NULL, 0, NULL, &db_handle)) {
			efree(hashed_details);
			RETURN_FALSE;
		}

		ib_link = (ibase_db_link *) emalloc(sizeof(ibase_db_link));
		ib_link->link = db_handle;
		ib_link->default_trans = NULL;

		/* add it to the list */
		return_value->value.lval = php3_list_insert(ib_link, IBASE_GLOBAL(php3_ibase_module).le_link);
		return_value->type = IS_LONG;

		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (_php3_hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		IBASE_GLOBAL(php3_ibase_module).num_links++;
	}
	efree(hashed_details);
	IBASE_GLOBAL(php3_ibase_module).default_link=return_value->value.lval;
}
/* }}} */

/* {{{ proto int ibase_connect(string database [, string username] [, string password])
   Open a connection to an InterBase database */
void php3_ibase_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_ibase_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int ibase_pconnect(string database [, string username] [, string password])
   Open a persistent connection to an InterBase database */
void php3_ibase_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_ibase_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int ibase_close([int link_identifier])
   Close an InterBase connection */
void php3_ibase_close(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *link_arg;
	ibase_db_link *ib_link;
	int link_id, type;
	IBASE_TLS_VARS;
	
    RESET_ERRMSG;
    
	switch (ARG_COUNT(ht)) {
		case 0:
			link_id = IBASE_GLOBAL(php3_ibase_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &link_arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(link_arg);
			link_id = link_arg->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ib_link = (ibase_db_link *) php3_list_find(link_id, &type);
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_link && type!=IBASE_GLOBAL(php3_ibase_module).le_plink) {
		_php3_module_error("%d is not link index",link_id);
		RETURN_FALSE;
	}
	
	php3_list_delete(link_id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ _php3_ibase_prepare() */
static XSQLDA *_php3_ibase_prepare(isc_db_handle db, isc_tr_handle tr, isc_stmt_handle *query_handle, char *query)
{
	ISC_STATUS status[20];
	XSQLDA *isqlda;

	isqlda = (XSQLDA *) emalloc(XSQLDA_LENGTH(0));
	isqlda->sqln = 0;
	isqlda->version = SQLDA_VERSION1;

	if (isc_dsql_allocate_statement(status, &db, query_handle)) {
		efree(isqlda);
		_php3_ibase_error(status);
		return NULL;
	}

	if (isc_dsql_prepare(status, &tr, query_handle, 0, query, 1, isqlda)) {
		efree(isqlda);
		_php3_ibase_error(status);
		return NULL;
	}
	
	/*
	 * Check if query has placeholders and needs binding. If it has, allocate
	 * input sqlda big enough and return it.
	 */


	if (isc_dsql_describe_bind(status, query_handle, 1, isqlda)) {
		efree(isqlda);
		_php3_ibase_error(status);
		return NULL;
	}
	
	if (isqlda->sqld > 1) {
		isqlda = (XSQLDA *) erealloc(isqlda, XSQLDA_LENGTH(isqlda->sqld));
		isqlda->sqln = isqlda->sqld;
		isqlda->version = SQLDA_VERSION1;
		if (isc_dsql_describe(status, query_handle, 1, isqlda)) {
			efree(isqlda);
			_php3_ibase_error(status);
			return NULL;
		}
		return isqlda;
	} else if (isqlda->sqld == 1) {
		return isqlda;
	} else {
		efree(isqlda);
		return NULL;
	}
}
/* }}} */

/* {{{ _php3_ibase_execute() */
static XSQLDA *_php3_ibase_execute(isc_tr_handle tr_handle, isc_stmt_handle query_handle, XSQLDA *isqlda, ISC_STATUS *status)
{
	int i, coltype;
	static char query_info[] = { isc_info_sql_stmt_type };
	char info_buffer[18];
	short l;
	long query_type;
	XSQLDA *osqlda;

	/*
	 * Find out what kind of query is to be executed.
	 */
	
	if (!isc_dsql_sql_info(status, &query_handle, sizeof(query_info), query_info, sizeof(info_buffer), info_buffer)) {
		l = (short) isc_vax_integer((char ISC_FAR *) info_buffer + 1, 2);
		query_type = isc_vax_integer((char ISC_FAR *) info_buffer + 3, l);
	}

	if (query_type == isc_info_sql_stmt_select || query_type == isc_info_sql_stmt_select_for_upd) {
		/*
		 * Select, need to allocate output sqlda and and prepare it for use.
		 */

		osqlda = (XSQLDA *) emalloc(XSQLDA_LENGTH(0));
		osqlda->sqln = 0;
		osqlda->version = SQLDA_VERSION1;

		if (isc_dsql_describe(status, &query_handle, 1, osqlda)) {
			efree(osqlda);
			_php3_ibase_error(status);
			return NULL;
		}

		if (osqlda->sqld) {
			osqlda = (XSQLDA *) erealloc(osqlda, XSQLDA_LENGTH(osqlda->sqld));
			osqlda->sqln = osqlda->sqld;
			osqlda->version = SQLDA_VERSION1;
			if (isc_dsql_describe(status, &query_handle, 1, osqlda)) {
				efree(osqlda);
				_php3_ibase_error(status);
				return NULL;
			}
		}
		for (i = 0; i < osqlda->sqld; i++) {
			osqlda->sqlvar[i].sqlind = (short *) emalloc(sizeof(short));
			coltype = osqlda->sqlvar[i].sqltype & ~1;
			switch(coltype)
				{
				case SQL_TEXT:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(char)*(osqlda->sqlvar[i].sqllen));
					break;
				case SQL_VARYING:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(char)*(osqlda->sqlvar[i].sqllen+2));
					break;
				case SQL_SHORT:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(short));
					break;
				case SQL_LONG:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(long));
					break;
				case SQL_FLOAT:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(float));
					break;
				case SQL_DOUBLE:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(double));
					break;
				case SQL_DATE:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(ISC_QUAD));
					break;
				case SQL_BLOB:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(ISC_QUAD));
					break;
				case SQL_ARRAY:
					osqlda->sqlvar[i].sqldata = (char *) emalloc(sizeof(ISC_QUAD));
					break;
				}
		}
		if (isqlda == NULL) {
			if (isc_dsql_execute(status, &tr_handle, &query_handle, 1, NULL)) {
				efree(osqlda);
				_php3_ibase_error(status);
				return NULL;
			} else {
				return osqlda;
			}
	
		} else {
			if (isc_dsql_execute2(status, &tr_handle, &query_handle, 1, isqlda, osqlda)) {
				efree(osqlda);
				_php3_ibase_error(status);
				return NULL;
			} else {
				return osqlda;
			}
	
		}
	} else {
		/* Not select */
		if (isc_dsql_execute(status, &tr_handle, &query_handle, 1, isqlda)) {
			_php3_ibase_error(status);
			return NULL;
		}
	}

	return NULL;
}
/* }}} */


#define GET_TRANS_ARG(trans_arg,trans_ptr) { \
    int type; \
    convert_to_long(trans_arg); \
    trans_ptr = (ibase_trans *) php3_list_find(trans_arg->value.lval, &type);\
    if (type!=IBASE_GLOBAL(php3_ibase_module).le_trans) {\
        _php3_module_error("%d is not transaction index",trans_arg->value.lval);\
        RETURN_FALSE;\
    }                 \
}

#define TRANS_HANDLE(ib_trans,ib_link) \
    (ib_trans ? ib_trans->trans : (ib_link->default_trans ? ib_link->default_trans : 0))


/* {{{ proto int ibase_trans([int link_identifier, ][string trans_args])
   Start transaction */
void php3_ibase_trans(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *link_arg, *trans_arg;
	char tpb[20], *tpbp = NULL, *trans_args = NULL;
	int tpb_len = 0, link_id, trans_id, type, trans_def = 0;
	ibase_db_link *ib_link;
	ibase_trans *ib_trans;
	isc_tr_handle tr_handle = NULL;
	ISC_STATUS status[20];
	IBASE_TLS_VARS;
	

    RESET_ERRMSG;
    
	switch (ARG_COUNT(ht)) {
		case 0:
			link_id = IBASE_GLOBAL(php3_ibase_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &trans_arg) == FAILURE) {
				RETURN_FALSE;
			}
			link_id = IBASE_GLOBAL(php3_ibase_module).default_link;
			convert_to_string(trans_arg);
			trans_args = trans_arg->value.str.val;
			break;
		case 2:
			if (getParameters(ht, 2, &link_arg, &trans_arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(link_arg);
			link_id = link_arg->value.lval;
			convert_to_string(trans_arg);
			trans_args = trans_arg->value.str.val;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ib_link = (ibase_db_link *) php3_list_find(link_id, &type);
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_link && type!=IBASE_GLOBAL(php3_ibase_module).le_plink) {
		_php3_module_error("%d is not link index",link_id);
		RETURN_FALSE;
	}
	if(trans_args){
		tpb[tpb_len++] = isc_tpb_version3;
        tpbp = tpb;
		if(strstr(trans_args,"DEFAULT")||strstr(trans_args,"default")){
			trans_def = 1;
			if(ib_link->default_trans != NULL){
				_php3_module_error("default transaction already started");
				RETURN_FALSE;
			}
		}
		/* access mode */
		if(strstr(trans_args,"READ")||strstr(trans_args,"read")) /* READ ONLY TRANSACTION */
			tpb[tpb_len++] = isc_tpb_read;
		else
			tpb[tpb_len++] = isc_tpb_write;
		/* isolation level */
		if(strstr(trans_args,"COMMITED")||strstr(trans_args,"commited"))
			tpb[tpb_len++] = isc_tpb_read_committed;
		else if (strstr(trans_args,"CONSISTENCY")||strstr(trans_args,"consistency"))
			tpb[tpb_len++] = isc_tpb_consistency;
		else 
			tpb[tpb_len++] = isc_tpb_concurrency;
		/* lock resolution */
		if(strstr(trans_args,"NOWAIT")||strstr(trans_args,"nowait"))
			tpb[tpb_len++] = isc_tpb_nowait;
		else 
			tpb[tpb_len++] = isc_tpb_wait;

	}

	if (isc_start_transaction(status, &tr_handle, 1, &ib_link->link, tpb_len, tpbp)) {
		_php3_ibase_error(status);
		RETURN_FALSE;
	}

	if(trans_def){
		ib_link->default_trans = tr_handle;
		RETURN_TRUE;
	}else{
		ib_trans = (ibase_trans *) emalloc(sizeof(ibase_trans));
		ib_trans->trans = tr_handle;
		trans_id = php3_list_insert(ib_trans, IBASE_GLOBAL(php3_ibase_module).le_trans);
		RETURN_LONG(trans_id);
	}
}
/* }}} */

/* {{{ _php3_ibase_trans_end() */
#define COMMIT 1
#define ROLLBACK 0
static void _php3_ibase_trans_end(INTERNAL_FUNCTION_PARAMETERS, int commit)
{
	pval *trans_arg;
	int trans_id, type;
	ibase_db_link *ib_link;
	ibase_trans *ib_trans = NULL;
	isc_tr_handle tr_handle;
	ISC_STATUS status[20];
    IBASE_TLS_VARS;

    
    RESET_ERRMSG;

	switch (ARG_COUNT(ht)) {
		case 0:
			trans_id = IBASE_GLOBAL(php3_ibase_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &trans_arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(trans_arg);
			trans_id = trans_arg->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	ib_trans = (ibase_trans *) php3_list_find(trans_id, &type);
	ib_link = (ibase_db_link *)ib_trans; 
	if (type==IBASE_GLOBAL(php3_ibase_module).le_link || type==IBASE_GLOBAL(php3_ibase_module).le_plink) {
		ib_link = (ibase_db_link *)ib_trans; /* default transaction */
		ib_trans = NULL;
		tr_handle = ib_link->default_trans;
	}else if (type==IBASE_GLOBAL(php3_ibase_module).le_trans){
		ib_link = NULL; /* manual transaction */
		tr_handle = ib_trans->trans;
	}else{
		_php3_module_error("%d is not link or transaction index",trans_id);
		RETURN_FALSE;
	}

	if (commit) {
		if(isc_commit_transaction(status, &tr_handle)){
            _php3_ibase_error(status);
            RETURN_FALSE;
		}
	}else{
		if(isc_rollback_transaction(status, &tr_handle)){
            _php3_ibase_error(status);
            RETURN_FALSE;
		}
	}

    if(ib_link){ /* default transaction */
        ib_link->default_trans = NULL;
    }else{ /* manual transaction */
		ib_trans->trans = NULL; /* already commited or rolled...*/
        php3_list_delete(trans_id);
    }
	
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ibase_commit([int link_identifier, ] int trans_number)
   Commit transaction */
void php3_ibase_commit(INTERNAL_FUNCTION_PARAMETERS)
{
    _php3_ibase_trans_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, COMMIT);
}
/* }}} */

/* {{{ proto int ibase_rollback([int link_identifier, ] int trans_number)
   Roolback transaction */
void php3_ibase_rollback(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_ibase_trans_end(INTERNAL_FUNCTION_PARAM_PASSTHRU, ROLLBACK);
}
/* }}} */

/* {{{ proto int ibase_query([int link_identifier, ]string query[, trans])
 Execute a query (without parameter placeholders). */
void php3_ibase_query(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *link_arg, *query_arg, *trans_arg;
	int link_id, type, trans_single = 0;
	ibase_db_link *ib_link;
	ibase_trans *ib_trans = NULL;
	isc_tr_handle trans_handle = NULL;
	isc_stmt_handle query_handle = NULL;
	ISC_STATUS status[20];
	XSQLDA *isqlda, *osqlda;
	ibase_result_handle *ibase_result;
    IBASE_TLS_VARS;

    RESET_ERRMSG;

	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query_arg) == FAILURE) {
				RETURN_FALSE;
			}
			link_id = IBASE_GLOBAL(php3_ibase_module).default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &link_arg, &query_arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(link_arg);
			link_id = link_arg->value.lval;
			break;
		case 3:
			if (getParameters(ht, 3, &link_arg, &query_arg, &trans_arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(link_arg);
            link_id = link_arg->value.lval;
            GET_TRANS_ARG(trans_arg,ib_trans);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ib_link = (ibase_db_link *) php3_list_find(link_id, &type);
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_link && type!=IBASE_GLOBAL(php3_ibase_module).le_plink) {
		_php3_module_error("%d is not link index",link_id);
		RETURN_FALSE;
    }

    trans_handle = TRANS_HANDLE(ib_trans,ib_link);
    
    trans_single = (trans_handle == NULL);
    if(trans_single){
        if (isc_start_transaction(status, &trans_handle, 1, &ib_link->link, 0, NULL)) {
			_php3_ibase_error(status);
            RETURN_FALSE;
        }
    }
    
	convert_to_string(query_arg);

    isqlda = _php3_ibase_prepare(ib_link->link, trans_handle, &query_handle, query_arg->value.str.val);
    if(TEST_ERRMSG){
        RETURN_FALSE;
    }
	if (isqlda != NULL) {
		if(trans_single)
			isc_rollback_transaction(status, &trans_handle);
		isc_dsql_free_statement(status, &query_handle, DSQL_drop);
		_php3_module_error("ibase_query doesn't support parameter placeholders in query");
		RETURN_FALSE;
	}

	osqlda = _php3_ibase_execute(trans_handle, query_handle, isqlda, status);
	if (osqlda != NULL) {
		ibase_result = (ibase_result_handle *) emalloc(sizeof(ibase_result_handle));
		ibase_result->result = query_handle;
		ibase_result->sqlda = osqlda;
        ibase_result->trans_handle = trans_handle;
        ibase_result->trans_single = trans_single;
        ibase_result->link = ib_link->link;
        RETURN_LONG(php3_list_insert(ibase_result, php3_ibase_module.le_result));
	} else {
		if (trans_single) {
			if (status[0] && status[1]) {
				isc_rollback_transaction(status, &trans_handle);
				isc_dsql_free_statement(status, &query_handle, DSQL_drop);
				RETURN_FALSE;
			} else {
				isc_commit_transaction(status, &trans_handle);
				isc_dsql_free_statement(status, &query_handle, DSQL_drop);
				RETURN_TRUE;
			}
		}
	}
}
/* }}} */


/* {{{ _php3_ibase_fetch_hash() */
static void _php3_ibase_fetch_hash(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result_arg, *blflag_arg;
	int type, blobchar = 0, i, collen;
	char string_data[255], *char_data;
	ibase_result_handle *ibase_result;
	ISC_STATUS status[20];
	XSQLVAR *var;
	IBASE_VCHAR *vchar;
	IBASE_TLS_VARS;

    RESET_ERRMSG;

    switch(ARG_COUNT(ht)){
        case 1:
            if (ARG_COUNT(ht)==1 && getParameters(ht, 1, &result_arg)==FAILURE) {
                RETURN_FALSE;
            }
            break;
        case 2:
            if(ARG_COUNT(ht)==2 && getParameters(ht, 2, &result_arg, &blflag_arg)==FAILURE) {
                RETURN_FALSE;
            }
            convert_to_string(blflag_arg);
            if(strcasecmp(blflag_arg->value.str.val,"TEXT")){ /* FIXME: need? */
                _php3_module_error("invalid blob flag '%s'", blflag_arg->value.str.val);
                RETURN_FALSE;
            }else{
                blobchar = 1;
            }
            break;
        default:
            WRONG_PARAM_COUNT;
            break;

    }
    
	convert_to_long(result_arg);
	ibase_result = (ibase_result_handle *) php3_list_find(result_arg->value.lval, &type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_result) {
		_php3_module_error("%d is not result index", result_arg->value.lval);
		RETURN_FALSE;
	}

	if (ibase_result->sqlda == NULL) {
		_php3_module_error("trying to fetch results from a non-select query");
		RETURN_FALSE;
	}

	if (isc_dsql_fetch(status, &ibase_result->result, 1, ibase_result->sqlda) != 100L) {
		if (array_init(return_value)==FAILURE) {
			RETURN_FALSE;
		}

		var = ibase_result->sqlda->sqlvar;
		for (i = 0; i < ibase_result->sqlda->sqld; i++, var++) {
			if (*var->sqlind == -1)   /* in SQL null, in PHP variable not set */
				continue;
			switch(var->sqltype & ~1) {
				case SQL_TEXT:
					{
						char_data = (char *)emalloc(sizeof(char)*(var->sqllen+1));
						collen = sprintf(char_data, "%*.*s", var->sqllen, var->sqllen, var->sqldata);
						if (php3_ini.magic_quotes_runtime) {
							int newlen;
							char *tmp = _php3_addslashes(char_data, collen, &newlen, 0);
                            efree(char_data);
							add_get_index_stringl(return_value, i, tmp, newlen, (void **) &result_arg, 0);
						} else {
							add_get_index_stringl(return_value, i, char_data, collen, (void **) &result_arg, 0);
						}
						_php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
					}
					break;
				case SQL_VARYING:
					{
						vchar = (IBASE_VCHAR *) var->sqldata;
						char_data = (char *)emalloc(sizeof(char)*(vchar->var_len+1));
						collen = sprintf(char_data, "%*.*s", vchar->var_len, vchar->var_len, vchar->var_str);
						if (php3_ini.magic_quotes_runtime) {
							int newlen;
							char *tmp = _php3_addslashes(char_data, collen, &newlen, 0);
                            efree(char_data);
							add_get_index_stringl(return_value, i, tmp, newlen, (void **) &result_arg, 0);
						} else {
							add_get_index_stringl(return_value, i, char_data, collen, (void **) &result_arg, 0);
						}
						_php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
					}
					break;
				case SQL_SHORT:
					collen = sprintf(string_data, "%d", *(short *)(var->sqldata));
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &result_arg, 1);
					_php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
					break;
				case SQL_LONG:
					if (var->sqlscale) {
						int j, f = 1;
						float n;
						n = *(long *)(var->sqldata);
						for (j = 0; j < -var->sqlscale; j++)
							f *= 10;
						n /= f;
						collen = sprintf(string_data, "%.*f", -var->sqlscale, n);

					} else {
						collen = sprintf(string_data, "%ld", *(long *)(var->sqldata));
					}
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &result_arg, 1);
					_php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
					break;
				case SQL_FLOAT:
					collen = sprintf(string_data, "%f", *(float *)(var->sqldata));
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &result_arg, 1);
					_php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
					break;
				case SQL_DOUBLE:
					if (var->sqlscale) {
						collen = sprintf(string_data, "%.*f", -var->sqlscale, *(double *)(var->sqldata));
					} else {
						collen = sprintf(string_data, "%f", *(double *)(var->sqldata));
					}
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &result_arg, 1);
					_php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
					break;
				case SQL_DATE: {
					struct tm t;

					isc_decode_date((ISC_QUAD *) var->sqldata, &t);
					
#if HAVE_STRFTIME
					collen = (int) strftime(string_data, 255, IBASE_GLOBAL(php3_ibase_module).timeformat, &t);
#else
					if(!t.tm_hour && !t.tm_min && !t.tm_sec) 
						collen = sprintf(string_data, "%4d-%02d-%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);
					else
						collen = sprintf(string_data, "%4d-%02d-%02d %02d:%02d:%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
#endif
					add_get_index_stringl(return_value, i, string_data, collen, (void **) &result_arg, 1);
					_php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
					}
					break;
                case SQL_BLOB:
                    if(blobchar){
                        int stat;
                        isc_blob_handle bl_handle = NULL;
                        ISC_LONG bl_len = 0, cur_len = 0, max_len;
                        char bl_items[1], *bl_data, bl_info[20], *p;
                            
                        if(isc_open_blob(status, &ibase_result->link, &ibase_result->trans_handle, &bl_handle, (ISC_QUAD ISC_FAR *) var->sqldata)){
                            _php3_ibase_error(status);
                            RETURN_FALSE;
                        }

                        bl_items[0] = isc_info_blob_total_length;
                        if(isc_blob_info(status,&bl_handle,sizeof(bl_items),bl_items,sizeof(bl_info),bl_info)){
                            _php3_ibase_error(status);
                            RETURN_FALSE;
                        }
                        /* find total length of blob's data */
                        for (p = bl_info; *p != isc_info_end && p < bl_info+sizeof(bl_info);){
                            int item_len, item = *p++;
                            item_len = (short)isc_vax_integer(p, 2);
                            p += 2;
                            if (item == isc_info_blob_total_length)
                                bl_len = isc_vax_integer(p, item_len);
                            p += item_len;
                        }
                        
                        bl_data = emalloc(bl_len+1);

                        for(cur_len = stat = 0; stat == 0 && cur_len < max_len; ){
                            unsigned short seg_len, max_seg = max_len-cur_len > USHRT_MAX ? USHRT_MAX : max_len-cur_len;
                            stat = isc_get_segment(status, &bl_handle, &seg_len, max_seg, &bl_data[cur_len]);
                            cur_len += seg_len;
                            if(cur_len > max_len){ /* never!*/
                                efree(bl_data);
                                _php3_module_error("php module internal error in %s %d",__FILE__,__LINE__);
                                RETURN_FALSE;
                            }
                        }
                        
                        if (status[0] && (status[1] != isc_segstr_eof)){
                            efree(bl_data);
                            _php3_ibase_error(status);
                            RETURN_FALSE;
                        }
                        bl_data[cur_len] = '\0'; 
                        if(isc_close_blob(status, &bl_handle)){
                            efree(bl_data);
                            _php3_ibase_error(status);
                            RETURN_FALSE;
                        }
                        add_get_index_stringl(return_value, i, bl_data, cur_len, (void **) &result_arg, 0);
                        _php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
                    }else{ /* blob id only */
                        /* return string
                        "db_handle:trans_handle:blob_quad_high:blob_quad_low"
                        */
                        ISC_QUAD *bl_qd = (ISC_QUAD ISC_FAR *) var->sqldata;
                        
                        collen = sprintf(string_data, "%p:%p:%lx:%lx", ibase_result->link,ibase_result->trans_handle,
                                         bl_qd->gds_quad_high, bl_qd->gds_quad_low);
                        add_get_index_stringl(return_value, i, string_data, collen, (void **) &result_arg, 1);
                        _php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
                    }
                    break;
				case SQL_ARRAY:
					{
						/* TODO, currently just show the id */
						ISC_QUAD bid;
						bid = *(ISC_QUAD ISC_FAR *) var->sqldata;
						sprintf(string_data, "%lx:%lx", bid.isc_quad_high, bid.isc_quad_low);
						add_get_index_stringl(return_value, i, string_data, collen, (void **) &result_arg, 1);
						_php3_hash_pointer_update(return_value->value.ht, var->aliasname, var->aliasname_length+1, result_arg);
					}
					break;
				default:
					break;
			}
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array ibase_fetch_row(int result [,blob_flag])
   Fetch a row with blob's body from the results of a query. */
void php3_ibase_fetch_row(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_ibase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto object ibase_fetch_object(int result [,blob_flag])
   Fetch a object with blob's body from the results of a query. */
void php3_ibase_fetch_object(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_ibase_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (return_value->type==IS_ARRAY) {
		return_value->type = IS_OBJECT;
	}
}
/* }}} */


/* {{{ proto int ibase_free_result(int result)
   Free the memory used by a result. */
void php3_ibase_free_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result_var;
	ibase_result_handle *ibase_result;
	int type;
	IBASE_TLS_VARS;

    RESET_ERRMSG;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result_var)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result_var);
	if (result_var->value.lval==0) {
		RETURN_FALSE;
	}
	
	ibase_result = (ibase_result_handle *) php3_list_find(result_var->value.lval,&type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_result) {
		_php3_module_error("%d is not result index",result_var->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(result_var->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ibase_prepare([int link_identifier, ]string query)
   Prepare a query for later binding of parameter placeholders and execution. */
void php3_ibase_prepare(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *query, *ibase_link;
	int id, type;
	isc_db_handle db_handle;
	isc_tr_handle tr_handle = NULL;
	isc_stmt_handle query_handle = NULL;
	ISC_STATUS status[20];
	XSQLDA *isqlda;
	ibase_query_handle *ibase_query;
	IBASE_TLS_VARS;

    RESET_ERRMSG;

	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query) == FAILURE) {
				RETURN_FALSE;
			}
			id = IBASE_GLOBAL(php3_ibase_module).default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &ibase_link, &query) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(ibase_link);
			id = ibase_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	db_handle = (isc_db_handle) php3_list_find(id, &type);
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_link && type!=IBASE_GLOBAL(php3_ibase_module).le_plink) {
		_php3_module_error("%d is not link index", id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);

	if (1/*!IBASE_GLOBAL(php3_ibase_module).manualtransactions*/) {
		if (isc_start_transaction(status, &tr_handle, 1, &db_handle, 0, NULL)) {
			_php3_ibase_error(status);
			RETURN_FALSE;
		}
	}

	isqlda = _php3_ibase_prepare(db_handle, tr_handle, &query_handle, query->value.str.val);
	ibase_query = (ibase_query_handle *) emalloc(sizeof(ibase_query_handle));
	ibase_query->query = query_handle;
	ibase_query->sqlda = isqlda;
 	ibase_query->trans_handle = tr_handle;
	ibase_query->alloced = 0;
	return_value->value.lval = php3_list_insert(ibase_query, php3_ibase_module.le_query);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int ibase_bind (int query)
   Bind parameter placeholders in a previously prepared query. Still nonfunctional. */
void php3_ibase_bind(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *query;
	ibase_query_handle *ibase_query;
	int type;
	IBASE_TLS_VARS;

    RESET_ERRMSG;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &query)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(query);
	if (query->value.lval==0) {
		RETURN_FALSE;
	}
	
	ibase_query = (ibase_query_handle *) php3_list_find(query->value.lval,&type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_query) {
		_php3_module_error("%d is not query index",query->value.lval);
		RETURN_FALSE;
	}

	if (ibase_query->sqlda == NULL) {
		_php3_module_error("trying to bind a query having no parameter placeholders");
		RETURN_FALSE;
	}

	/* TODO 
	for (i = 0; i < ibase_query->sqlda->sqld; i++) {
		ibase_query->sqlda->sqlvar[i].sqlind = (short *) emalloc(sizeof(short));
		coltype = ibase_query->sqlda->sqlvar[i].sqltype & ~1;
		switch(coltype)
			{
			case SQL_TEXT:
				break;
			case SQL_VARYING:
				break;
			case SQL_SHORT:
				break;
			case SQL_LONG:
				break;
			case SQL_FLOAT:
				break;
			case SQL_DOUBLE:
				break;
			case SQL_DATE:
				break;
			case SQL_BLOB:
				break;
			case SQL_ARRAY:
				break;
			}
	}
	*/
}
/* }}} */

/* {{{ proto int ibase_execute(int query)
   Execute a previously prepared (and possibly binded) query. */
void php3_ibase_execute(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *query;
	ibase_query_handle *ibase_query;
	ibase_result_handle *ibase_result;
	int type;
	ISC_STATUS status[20];
	XSQLDA *osqlda;
	IBASE_TLS_VARS;

    RESET_ERRMSG;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &query)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(query);
	if (query->value.lval==0) {
		RETURN_FALSE;
	}
	
	ibase_query = (ibase_query_handle *) php3_list_find(query->value.lval,&type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_query) {
		_php3_module_error("%d is not query index", query->value.lval);
		RETURN_FALSE;
	}

	/* FIXME: manual and other trans todo...*/
 	osqlda = _php3_ibase_execute(ibase_query->trans_handle, ibase_query->query, ibase_query->sqlda, status);
	ibase_result = (ibase_result_handle *) emalloc(sizeof(ibase_result_handle));
	ibase_result->result = ibase_query->query;
	ibase_result->sqlda = osqlda;
	return_value->value.lval = php3_list_insert(ibase_result, IBASE_GLOBAL(php3_ibase_module).le_result);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int ibase_free_query(int query)
   Free memory used by a query */
void php3_ibase_free_query(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *query_arg;
	ibase_query_handle *ibase_query;
	int type;
	IBASE_TLS_VARS;

    RESET_ERRMSG;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &query_arg)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(query_arg);
	if (query_arg->value.lval==0) {
		RETURN_FALSE;
	}
	
	ibase_query = (ibase_query_handle *) php3_list_find(query_arg->value.lval, &type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_query) {
		_php3_module_error("%d is not query index", query_arg->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(query_arg->value.lval);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int ibase_timefmt(string format)
   Sets the format of datetime columns returned from queries. Still nonfunctional. */
void php3_ibase_timefmt(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *fmt;
	IBASE_TLS_VARS;

    RESET_ERRMSG;

#if HAVE_STRFTIME
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &fmt)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(fmt);
	efree(IBASE_GLOBAL(php3_ibase_module).timeformat);
	IBASE_GLOBAL(php3_ibase_module).timeformat = estrndup(fmt->value.str.val, fmt->value.str.len);
	RETURN_TRUE;
#else
	_php3_module_error("ibase_timefmt not supported on this platform");
	RETURN_FALSE;
#endif
}
/* }}} */


/* {{{ proto int ibase_num_fields(int result)
   Get the number of fields in result */
void php3_ibase_num_fields(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	int type;
	ibase_result_handle *ibase_result;

    RESET_ERRMSG;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	ibase_result = (ibase_result_handle *) php3_list_find(result->value.lval, &type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_result) {
		_php3_module_error("%d is not result index", result->value.lval);
		RETURN_FALSE;
	}

	if (ibase_result->sqlda == NULL) {
		_php3_module_error("trying to get num fields from a non-select query");
		RETURN_FALSE;
	}

	RETURN_LONG(ibase_result->sqlda->sqld);
}
/* }}} */


/* {{{ proto array ibase_field_info(int result, int field_number)
   Get information about a field */
void php3_ibase_field_info(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result, *field;
	int type, len;
	ibase_result_handle *ibase_result;
	char buf[30], *s;
	XSQLVAR *var;
    IBASE_TLS_VARS;

    RESET_ERRMSG;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	ibase_result = (ibase_result_handle *) php3_list_find(result->value.lval, &type);
	
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_result) {
		_php3_module_error("%d is not result index", result->value.lval);
		RETURN_FALSE;
	}

	if (ibase_result->sqlda == NULL) {
		_php3_module_error("trying to get field info from a non-select query");
		RETURN_FALSE;
	}

	convert_to_long(field);

	if (field->value.lval<0 || field->value.lval>=ibase_result->sqlda->sqld)
		RETURN_FALSE;

	if (array_init(return_value)==FAILURE)
		RETURN_FALSE;
	
	var = ibase_result->sqlda->sqlvar + field->value.lval;
	
	add_get_index_stringl(return_value, 0, var->sqlname, var->sqlname_length, (void **) &result, 1);
	_php3_hash_pointer_update(return_value->value.ht, "name", sizeof(char)*5, result);

    add_get_index_stringl(return_value, 1, var->aliasname, var->aliasname_length, (void **) &result, 1);
	_php3_hash_pointer_update(return_value->value.ht, "alias", sizeof(char)*6, result);

	add_get_index_stringl(return_value, 2, var->relname, var->relname_length, (void **) &result, 1);
	_php3_hash_pointer_update(return_value->value.ht, "relation", sizeof(char)*9, result);

	len = sprintf(buf, "%d", var->sqllen);
	add_get_index_stringl(return_value, 3, buf, len, (void **) &result, 1);
	_php3_hash_pointer_update(return_value->value.ht, "length", sizeof(char)*7, result);

	switch (var->sqltype & ~1){
        case SQL_TEXT:     s = "TEXT"; break;
        case SQL_VARYING:  s = "VARYING"; break;
        case SQL_SHORT:	   s = "SHORT"; break;
        case SQL_LONG:	   s = "LONG"; break;
        case SQL_FLOAT:	   s = "FLOAT"; break;
        case SQL_DOUBLE:   s = "DOUBLE"; break;
        case SQL_D_FLOAT:  s = "D_FLOAT"; break;
        case SQL_DATE:	   s = "DATE"; break;
        case SQL_BLOB:	   s = "BLOB"; break;
        case SQL_ARRAY:	   s = "ARRAY"; break;
        case SQL_QUAD:	   s = "QUAD"; break;
	default:
		sprintf(buf,"unknown (%d)", var->sqltype & ~1);
		s = buf;
		break;
	}
	add_get_index_stringl(return_value, 4, s, strlen(s), (void **) &result, 1);
	_php3_hash_pointer_update(return_value->value.ht, "type", sizeof(char)*5, result);

}
/* }}} */


/*  begin blobs *********************************************/

#define IBASE_BLOB_SEG 4096

/* get string blob identifier from argument
 format:  database_link:transaction_handle:gds_quad_high:gds_quad_low
 */
#define GET_BLOB_STR_ARG(blob_arg, link, trans_handle, bl_qd) \
    if(blob_arg->type != IS_STRING || \
       sscanf(blob_arg->value.str.val,"%p%*[:]%p%*[:]%lx%*[:]%lx", \
              &link,&trans_handle,&bl_qd.gds_quad_high,&bl_qd.gds_quad_low) != 4){ \
        _php3_module_error("invalid blob id string"); \
        RETURN_FALSE; \
    }


/* get blob handle from argument
 note: blob already open when handle active
 */
#define GET_BLOB_HANDLE_ARG(blob_arg, blob_ptr) \
{ \
    int type; \
    convert_to_long(blob_arg); \
    blob_ptr = (ibase_blob_handle *) php3_list_find(blob_arg->value.lval, &type); \
    if (type!=IBASE_GLOBAL(php3_ibase_module).le_blob) { \
        _php3_module_error("%d is not blob handle",blob_arg->value.lval); \
        RETURN_FALSE; \
    } \
}


/* internal struct */
typedef struct {
    ISC_LONG  max_segment;	    /* Length of longest segment */
    ISC_LONG  num_segments; 	/* Total number of segments */
    ISC_LONG  total_length;  	/* Total length of blob */
    int       bl_stream;        /* blob is stream ? */
}IBASE_BLOBINFO;


/* {{{ int _php3_ibase_blob_info(isc_blob_handle bl_handle,IBASE_BLOBINFO *bl_info)*/
static int _php3_ibase_blob_info(isc_blob_handle bl_handle,IBASE_BLOBINFO *bl_info)
{
    ISC_STATUS status[20];
    
    static char bl_items[] = {
        isc_info_blob_num_segments,
        isc_info_blob_max_segment,
        isc_info_blob_total_length,
        isc_info_blob_type
    };
    
    char bl_inf[sizeof(long)*8], *p;

    bl_info->max_segment = 0;
    bl_info->num_segments = 0;
    bl_info->total_length = 0;
    bl_info->bl_stream = 0;

    if(isc_blob_info(status,&bl_handle,sizeof(bl_items),bl_items,sizeof(bl_inf),bl_inf)){
        _php3_ibase_error(status);
        return -1;
    }

    for (p = bl_inf; *p != isc_info_end && p < bl_inf+sizeof(bl_inf);){
        int item_len, item = *p++;
        item_len = (short)isc_vax_integer(p, 2);
        p += 2;
        switch(item){
            case isc_info_blob_num_segments:
                bl_info->num_segments = isc_vax_integer(p, item_len);
                break;
            case isc_info_blob_max_segment:
                bl_info->max_segment = isc_vax_integer(p, item_len);
                break;
            case isc_info_blob_total_length:
                bl_info->total_length = isc_vax_integer(p, item_len);
                break;
            case isc_info_blob_type:
                bl_info->bl_stream = isc_vax_integer(p, item_len);
                break;
            case isc_info_end:
                break;
            case isc_info_truncated:
            case isc_info_error:  /* hmm. don't think so...*/
                _php3_ibase_error(status);
                return -1;
        }/*switch*/
        p += item_len;
    }/*for*/
    return 0;
}
/* }}} */

/* {{{ _php3_ibase_free_blob()  ibase_blob_handle destructor */
static void _php3_ibase_free_blob(ibase_blob_handle *ib_blob)
{
	ISC_STATUS status[20];
	IBASE_TLS_VARS;
	
	if(ib_blob->bl_handle != NULL){ /* blob open*/
        if(isc_cancel_blob(status, &ib_blob->bl_handle)){
            _php3_ibase_error(status);
        }
	}
	efree(ib_blob);
}
/* }}} */


/* {{{ proto int ibase_blob_create([[int link_identifier ], int trans_arg])
   Create blob for adding data */
void php3_ibase_blob_create(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *link_arg,*trans_arg;
	int type, link_id = 0;
	ISC_STATUS status[20];
	ibase_db_link *ib_link;
	ibase_trans *ib_trans = NULL;
    ibase_blob_handle *ib_blob;
    IBASE_TLS_VARS;

    RESET_ERRMSG;

    switch (ARG_COUNT(ht)) {
        case 0:
			link_id = IBASE_GLOBAL(php3_ibase_module).default_link;
            break;
		case 1:
			if (getParameters(ht, 1, &trans_arg) == FAILURE) {
				RETURN_FALSE;
            }
            GET_TRANS_ARG(trans_arg, ib_trans);
			link_id = IBASE_GLOBAL(php3_ibase_module).default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &link_arg, &trans_arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(link_arg);
            link_id = link_arg->value.lval;
            GET_TRANS_ARG(trans_arg, ib_trans);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	ib_link = (ibase_db_link *) php3_list_find(link_id, &type);
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_link && type!=IBASE_GLOBAL(php3_ibase_module).le_plink) {
		_php3_module_error("%d is not link index",link_id);
		RETURN_FALSE;
    }

    ib_blob = (ibase_blob_handle *) emalloc(sizeof(ibase_blob_handle));
    ib_blob->trans_handle = ib_trans ? ib_trans->trans : ib_link->default_trans;
    ib_blob->link = ib_link->link;
    ib_blob->bl_handle = NULL;
    
    if (isc_create_blob(status, &ib_blob->link, &ib_blob->trans_handle, &ib_blob->bl_handle,&ib_blob->bl_qd)){
        efree(ib_blob);
        _php3_ibase_error(status);
        RETURN_FALSE;
    }
    
    RETURN_LONG(php3_list_insert(ib_blob, IBASE_GLOBAL(php3_ibase_module).le_blob));
}
/* }}} */


/* {{{ proto int ibase_blob_open(string blob_id)
   Open blob for retriving data parts  */
void php3_ibase_blob_open(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *blob_arg;
	ISC_STATUS status[20];
    ibase_blob_handle *ib_blob;
    IBASE_TLS_VARS;

    RESET_ERRMSG;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &blob_arg)==FAILURE) {
		WRONG_PARAM_COUNT;
    }

    ib_blob = (ibase_blob_handle *) emalloc(sizeof(ibase_blob_handle));

    GET_BLOB_STR_ARG(blob_arg,ib_blob->link,ib_blob->trans_handle,ib_blob->bl_qd)

    ib_blob->bl_handle = NULL;
    if (isc_open_blob(status, &ib_blob->link, &ib_blob->trans_handle, &ib_blob->bl_handle, &ib_blob->bl_qd)){
        efree(ib_blob);
        _php3_ibase_error(status);
        RETURN_FALSE;
    }

    RETURN_LONG(php3_list_insert(ib_blob, IBASE_GLOBAL(php3_ibase_module).le_blob));
}
/* }}} */


/* {{{ proto int ibase_blob_put(int blob_id, string data)
   Add data into created blob */
void php3_ibase_blob_add(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *blob_arg,*string_arg;
	ISC_STATUS status[20];
    ibase_blob_handle *ib_blob;
    IBASE_TLS_VARS;

    RESET_ERRMSG;

    if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &blob_arg, &string_arg)==FAILURE) {
		WRONG_PARAM_COUNT;
    }

    GET_BLOB_HANDLE_ARG(blob_arg, ib_blob);
    
	convert_to_string(string_arg);

    if (isc_put_segment(status, &ib_blob->bl_handle, string_arg->value.str.len, string_arg->value.str.val)){
        _php3_ibase_error(status);
		RETURN_FALSE;
    }
    RETURN_TRUE;
}
/* }}} */


/* {{{ proto string ibase_blob_get(int blob_id, int len)
   Get len bytes data from open blob */
void php3_ibase_blob_get(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *blob_arg, *len_arg;
    int stat;
    char *bl_data;
    unsigned short max_len = 0, cur_len, seg_len;
	ISC_STATUS status[20];
    ibase_blob_handle *ib_blob;
    IBASE_TLS_VARS;

    RESET_ERRMSG;

    if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &blob_arg, &len_arg) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

    convert_to_long(len_arg);
    max_len = len_arg->value.lval;

    GET_BLOB_HANDLE_ARG(blob_arg, ib_blob);

    bl_data = emalloc(max_len+1);

    for(cur_len = stat = 0; stat == 0; ){
        stat = isc_get_segment(status, &ib_blob->bl_handle, &seg_len, max_len-cur_len, &bl_data[cur_len]);
        cur_len += seg_len;
        if(cur_len > max_len){ /* never!*/
            efree(bl_data);
            _php3_module_error("php module internal error in %s %d",__FILE__,__LINE__);
            RETURN_FALSE;
        }
    }

    bl_data[cur_len] = '\0';
    if (status[0] && (status[1] != isc_segstr_eof && status[1] != isc_segment)){
        efree(bl_data);
        _php3_ibase_error(status);
        RETURN_FALSE;
    }
    RETURN_STRINGL(bl_data,cur_len,0);
}
/* }}} */


#define BLOB_CLOSE 1
#define BLOB_CANCEL 2

/* {{{ int ibase_blob_close(int blob_id)
   Close or Cancel created or Close open blob */
static void _php3_ibase_blob_end(INTERNAL_FUNCTION_PARAMETERS, int bl_end)
{
	pval *blob_arg;
	ISC_STATUS status[20];
    ibase_blob_handle *ib_blob;
    char string_data[80];
    IBASE_TLS_VARS;

    RESET_ERRMSG;
    
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &blob_arg)==FAILURE) {
		WRONG_PARAM_COUNT;
    }

    GET_BLOB_HANDLE_ARG(blob_arg, ib_blob);

    if(bl_end == BLOB_CLOSE){ /* return string id here */
        if (isc_close_blob(status, &ib_blob->bl_handle)){
            _php3_ibase_error(status);
            RETURN_FALSE;
        }
        ib_blob->bl_handle = NULL;
        sprintf(string_data, "%p:%p:%lx:%lx", ib_blob->link,ib_blob->trans_handle,
                ib_blob->bl_qd.gds_quad_high, ib_blob->bl_qd.gds_quad_low);
        php3_list_delete(blob_arg->value.lval);
        RETURN_STRING(string_data,1);
    }else{ /* discard created blob */
        if (isc_cancel_blob(status, &ib_blob->bl_handle)){
            _php3_ibase_error(status);
            RETURN_FALSE;
        }
        ib_blob->bl_handle = NULL;
        php3_list_delete(blob_arg->value.lval);
        RETURN_TRUE;
    }
}
/* }}} */


/* {{{ proto int ibase_blob_close(int blob_id)
   Close blob */
void php3_ibase_blob_close(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_ibase_blob_end(INTERNAL_FUNCTION_PARAM_PASSTHRU,BLOB_CLOSE);
}
/* }}} */


/* {{{ proto int ibase_blob_cancel(int blob_id)
   Cancel creating blob */
void php3_ibase_blob_cancel(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_ibase_blob_end(INTERNAL_FUNCTION_PARAM_PASSTHRU,BLOB_CANCEL);
}
/* }}} */


/* {{{ proto object ibase_blob_info(string blob_id_str)
   Return blob length and other useful info */
void php3_ibase_blob_info(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *blob_arg, *result_var;
	ISC_STATUS status[20];
    isc_tr_handle trans_handle; 
    isc_db_handle link;
    isc_blob_handle bl_handle = NULL;
    ISC_QUAD bl_qd;
    IBASE_BLOBINFO bl_info;
    IBASE_TLS_VARS;

    RESET_ERRMSG;

    if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &blob_arg)==FAILURE) {
		WRONG_PARAM_COUNT;
    }
    
    GET_BLOB_STR_ARG(blob_arg,link,trans_handle,bl_qd);

    if (isc_open_blob(status, &link, &trans_handle, &bl_handle, &bl_qd)){
        _php3_ibase_error(status);
        RETURN_FALSE;
    }

    if (array_init(return_value)==FAILURE){
        RETURN_FALSE;
    }

    if(_php3_ibase_blob_info(bl_handle,&bl_info)){
        RETURN_FALSE;
    }

    add_get_index_long(return_value, 0, bl_info.total_length, (void **)&result_var);
    _php3_hash_pointer_update(return_value->value.ht, "length", sizeof("length"), result_var);

    add_get_index_long(return_value, 1, bl_info.num_segments, (void **)&result_var);
    _php3_hash_pointer_update(return_value->value.ht, "numseg", sizeof("numseg"), result_var);
    
    add_get_index_long(return_value, 2, bl_info.max_segment, (void **)&result_var);
	_php3_hash_pointer_update(return_value->value.ht, "maxseg", sizeof("maxseg"), result_var);

	add_get_index_long(return_value, 3, bl_info.bl_stream, (void **)&result_var);
    _php3_hash_pointer_update(return_value->value.ht, "stream", sizeof("stream"), result_var);
}
/* }}} */


/* {{{ proto int ibase_blob_echo(string blob_id_str)
   Output blob contents to browser  */
void php3_ibase_blob_echo(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *blob_arg;
    char bl_data[IBASE_BLOB_SEG];
    unsigned short seg_len;
	ISC_STATUS status[20];
    isc_tr_handle trans_handle; 
    isc_db_handle link;
    isc_blob_handle bl_handle = NULL;
    ISC_QUAD bl_qd;
    IBASE_TLS_VARS;

    RESET_ERRMSG;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &blob_arg)==FAILURE) {
		WRONG_PARAM_COUNT;
    }

    GET_BLOB_STR_ARG(blob_arg, link,trans_handle,bl_qd);
    
    if (!php3_header()) {
        RETURN_FALSE;
    }

    if (isc_open_blob(status, &link, &trans_handle, &bl_handle,&bl_qd)){
        _php3_ibase_error(status);
        RETURN_FALSE;
    }

    while(!isc_get_segment(status, &bl_handle, &seg_len, sizeof(bl_data), bl_data)
          || status[1] == isc_segment){
        PHPWRITE(bl_data,seg_len);
    }
    
    if (status[0] && (status[1] != isc_segstr_eof)){
        _php3_ibase_error(status);
        RETURN_FALSE;
    }

    if (isc_close_blob(status, &bl_handle)){
        _php3_ibase_error(status);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}
/* }}} */


/* from file.c. I'm not sure, this variables always initialized? */
extern int le_fp,le_pp;
extern int wsa_fp;

/* {{{ proto string ibase_blob_import([link_id, ] file_id [, trans_id])
   Create blob, copy file in it, and close it*/
void php3_ibase_blob_import(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *link_arg, *file_arg, *trans_arg;
    int type, link_id = 0, file_id, size, b;
	int issock=0, socketd=0, *sock;
    ISC_STATUS status[20];
    ISC_QUAD bl_qd;
	isc_tr_handle trans_handle = NULL;
    isc_blob_handle bl_handle = NULL;
	ibase_db_link *ib_link;
	ibase_trans *ib_trans = NULL;
    char string_data[80],
        bl_data[IBASE_BLOB_SEG]; /* FIXME? blob_seg_size parameter?  */
	FILE *fp;
    IBASE_TLS_VARS;

    RESET_ERRMSG;

    switch (ARG_COUNT(ht)) {
        case 1:
			if (getParameters(ht, 1, &file_arg) == FAILURE) {
				RETURN_FALSE;
            }
			link_id = IBASE_GLOBAL(php3_ibase_module).default_link;
            break;
		case 2:
			if (getParameters(ht, 2, &link_arg, &file_arg) == FAILURE) {
				RETURN_FALSE;
            }
			convert_to_long(link_arg);
			link_id = link_arg->value.lval;
			break;
		case 3:
			if (getParameters(ht, 3, &link_arg, &file_arg, &trans_arg) == FAILURE) {
				RETURN_FALSE;
            }
			convert_to_long(link_arg);
			link_id = link_arg->value.lval;
            GET_TRANS_ARG(trans_arg, ib_trans);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
    }

    convert_to_long(file_arg);
    file_id = file_arg->value.lval;
	fp = php3_list_find(file_id,&type);
	if (type==GLOBAL(wsa_fp)){
		issock=1;
		sock = php3_list_find(file_id,&type);
		socketd=*sock;
	}
	if ((!fp || (type!=GLOBAL(le_fp) && type!=GLOBAL(le_pp))) && (!socketd || type!=GLOBAL(wsa_fp))) {
		_php3_module_error("Unable to find file identifier %d",file_id);
		RETURN_FALSE;
	}

	ib_link = (ibase_db_link *) php3_list_find(link_id, &type);
	if (type!=IBASE_GLOBAL(php3_ibase_module).le_link && type!=IBASE_GLOBAL(php3_ibase_module).le_plink) {
		_php3_module_error("%d is not link index",link_id);
		RETURN_FALSE;
    }

    trans_handle = ib_trans ? ib_trans->trans : ib_link->default_trans;
   
    if (isc_create_blob(status, &ib_link->link, &trans_handle, &bl_handle,&bl_qd)){
        _php3_ibase_error(status);
        RETURN_FALSE;
    }

    size = 0;
	while(issock?(b=SOCK_FGETS(bl_data,sizeof(bl_data),socketd)):(b = fread(bl_data, 1, sizeof(bl_data), fp)) > 0) {
        if (isc_put_segment(status, &bl_handle, b, bl_data)){
            _php3_ibase_error(status);
            RETURN_FALSE;
        }
        size += b ;
    }
    
    if (isc_close_blob(status, &bl_handle)){
        _php3_ibase_error(status);
        RETURN_FALSE;
    }

    sprintf(string_data, "%p:%p:%lx:%lx", ib_link->link,trans_handle,
            bl_qd.gds_quad_high, bl_qd.gds_quad_low);

	php3_list_delete(file_id);
	RETURN_STRING(string_data,1);
}
/* }}} */


/* end blobs **********************************************/


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
 
