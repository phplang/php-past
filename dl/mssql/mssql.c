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
   | Authors: Zeev Suraski <zeev@zend.com>            Original sybase.c   |
   |          Frank M. Kromann <fmk@businesnet.dk>                        |
   +----------------------------------------------------------------------+
   | php3_mssql_get_column_content_with_type() based on code by:          |
   |                     Muhammad A Muquit <MA_Muquit@fccc.edu>           |
   |                     Rasmus Lerdorf <rasmus@lerdorf.on.ca>            |
   +----------------------------------------------------------------------+
 */

/* $Id: mssql.c,v 1.19 2000/05/23 18:13:28 fmk Exp $ */
#define IS_EXT_MODULE

#if !(WIN32|WINNT)
#include "config.h"
#endif
#include "php.h"
#include "internal_functions.h"


#include "php3_mssql.h"

#if HAVE_MSSQL
#define SAFE_STRING(s) ((s)?(s):"")

#include "functions/php3_string.h"
#include "php3_list.h"

#if BROKEN_MSSQL_PCONNECTS
#include "http_log.h"
#endif

function_entry mssql_functions[] = {
	{"mssql_connect",				php3_mssql_connect,					NULL},
	{"mssql_pconnect",				php3_mssql_pconnect,				NULL},
	{"mssql_close",					php3_mssql_close,					NULL},
	{"mssql_select_db",				php3_mssql_select_db,				NULL},
	{"mssql_query",					php3_mssql_query,					NULL},
	{"mssql_free_result",			php3_mssql_free_result,				NULL},
	{"mssql_get_last_message",		php3_mssql_get_last_message,		NULL},
	{"mssql_num_rows",				php3_mssql_num_rows,				NULL},
	{"mssql_num_fields",			php3_mssql_num_fields,				NULL},
	{"mssql_fetch_field",			php3_mssql_fetch_field,				NULL},
	{"mssql_fetch_row",				php3_mssql_fetch_row,				NULL},
	{"mssql_fetch_array",			php3_mssql_fetch_array,				NULL},
	{"mssql_fetch_object",			php3_mssql_fetch_object,			NULL},
	{"mssql_field_length",			php3_mssql_field_length,			NULL},
	{"mssql_field_name",			php3_mssql_field_name,				NULL},
	{"mssql_field_type",			php3_mssql_field_type,				NULL},
	{"mssql_data_seek",				php3_mssql_data_seek,				NULL},
	{"mssql_field_seek",			php3_mssql_field_seek,				NULL},
	{"mssql_result",				php3_mssql_result,					NULL},
	{"mssql_min_error_severity",	php3_mssql_min_error_severity,		NULL},
	{"mssql_min_message_severity",	php3_mssql_min_message_severity,	NULL},
	{NULL, NULL, NULL}
};

php3_module_entry mssql_module_entry = {
	"Microsoft SQL", mssql_functions, php3_minit_mssql, php3_mshutdown_mssql, php3_rinit_mssql, php3_rshutdown_mssql, php3_info_mssql, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &mssql_module_entry; }
#endif


#define msSQL_GLOBAL(a) php3_mssql_module.a
#define msSQL_TLS_VARS
mssql_module php3_mssql_module;



#define CHECK_LINK(link) { if (link==-1) { php3_error(E_WARNING,"MS SQL:  A link to the server could not be established"); RETURN_FALSE; } }


static void php3_mssql_get_column_content_with_type(mssql_link *mssql_ptr,int offset,pval *result, int column_type);
static void php3_mssql_get_column_content_without_type(mssql_link *mssql_ptr,int offset,pval *result, int column_type);

/* error handler */
static int php3_mssql_error_handler(DBPROCESS *dbproc,int severity,int dberr,
										int oserr,char *dberrstr,char *oserrstr)
{
	msSQL_TLS_VARS;

	if (severity >= msSQL_GLOBAL(min_error_severity)) {
		php3_error(E_WARNING,"MS SQL error:  %s (severity %d)",dberrstr,severity);
	}
	return INT_CANCEL;  
}

/* message handler */
static int php3_mssql_message_handler(DBPROCESS *dbproc,DBINT msgno,int msgstate,
										int severity,char *msgtext,char *srvname,
										char *procname,DBUSMALLINT line)
{
	msSQL_TLS_VARS;

	if (severity >= msSQL_GLOBAL(min_message_severity)) {
		php3_error(E_WARNING,"MS SQL message:  %s (severity %d)",msgtext,severity);
	}
	STR_FREE(msSQL_GLOBAL(server_message));
	msSQL_GLOBAL(server_message) = estrdup(msgtext);
	return 0;
}


static int _clean_invalid_results(list_entry *le)
{
	msSQL_TLS_VARS;

	if (le->type == msSQL_GLOBAL(le_result)) {
		mssql_link *mssql_ptr = ((mssql_result *) le->ptr)->mssql_ptr;
		
		if (!mssql_ptr->valid) {
			return 1;
		}
	}
	return 0;
}


static void _free_mssql_result(mssql_result *result)
{
	int i,j;

	if (result->data) {
		for (i=0; i<result->num_rows; i++) {
			for (j=0; j<result->num_fields; j++) {
				php3tls_pval_destructor(&result->data[i][j]);
			}
			efree(result->data[i]);
		}
		efree(result->data);
	}
	
	if (result->fields) {
		for (i=0; i<result->num_fields; i++) {
			STR_FREE(result->fields[i].name);
			STR_FREE(result->fields[i].column_source);
		}
		efree(result->fields);
	}
	efree(result);
}


static void _close_mssql_link(mssql_link *mssql_ptr)
{
	msSQL_TLS_VARS;

	mssql_ptr->valid = 0;
	_php3_hash_apply(msSQL_GLOBAL(resource_list),(int (*)(void *))_clean_invalid_results);
	dbclose(mssql_ptr->link);
	dbfreelogin(mssql_ptr->login);
	efree(mssql_ptr);
	msSQL_GLOBAL(num_links--);
}


static void _close_mssql_plink(mssql_link *mssql_ptr)
{
	msSQL_TLS_VARS;

	dbclose(mssql_ptr->link);
	dbfreelogin(mssql_ptr->login);
	free(mssql_ptr);
	msSQL_GLOBAL(num_persistent--);
	msSQL_GLOBAL(num_links--);
}


int php3_minit_mssql(INIT_FUNC_ARGS)
{
/*	char *interface_file; */
	long compatability_mode,connecttimeout;

	if (dbinit()==FAIL) {
		return FAILURE;
	}
#if WINNT|WIN32
	dberrhandle((DBERRHANDLE_PROC) php3_mssql_error_handler);
	dbmsghandle((DBMSGHANDLE_PROC) php3_mssql_message_handler);
#endif
	
	//if (cfg_get_string("mssql.interface_file",&interface_file)==SUCCESS) {
		//dbsetifile(interface_file);
	//}
	if (cfg_get_long("mssql.allow_persistent",&msSQL_GLOBAL(allow_persistent))==FAILURE) {
		msSQL_GLOBAL(allow_persistent)=1;
	}
	if (cfg_get_long("mssql.max_persistent",&msSQL_GLOBAL(max_persistent))==FAILURE) {
		msSQL_GLOBAL(max_persistent)=-1;
	}
	if (cfg_get_long("mssql.max_links",&msSQL_GLOBAL(max_links))==FAILURE) {
		msSQL_GLOBAL(max_links)=-1;
	}
	if (cfg_get_long("mssql.min_error_severity",&msSQL_GLOBAL(cfg_min_error_severity))==FAILURE) {
		msSQL_GLOBAL(cfg_min_error_severity)=10;
	}
	if (cfg_get_long("mssql.min_message_severity",&msSQL_GLOBAL(cfg_min_message_severity))==FAILURE) {
		msSQL_GLOBAL(cfg_min_message_severity)=10;
	}
	if (cfg_get_long("mssql.compatability_mode",&compatability_mode)==FAILURE) {
		compatability_mode = 0;
	}
	if (cfg_get_long("mssql.connect_timeout",&connecttimeout)==FAILURE) {
		connecttimeout = 5;
	}
	if (cfg_get_long("magic_quotes_runtime",&msSQL_GLOBAL(magic_quotes_runtime))==FAILURE) {
		msSQL_GLOBAL(magic_quotes_runtime) = 0;
	}
	if (compatability_mode) {
		msSQL_GLOBAL(get_column_content) = php3_mssql_get_column_content_with_type;
	} else {
		msSQL_GLOBAL(get_column_content) = php3_mssql_get_column_content_without_type;	
	}
	if (cfg_get_long("mssql.textsize",&msSQL_GLOBAL(textsize))==FAILURE) {
		msSQL_GLOBAL(textsize) = -1;
	}
	if (cfg_get_long("mssql.textlimit",&msSQL_GLOBAL(textlimit))==FAILURE) {
		msSQL_GLOBAL(textlimit) = -1;
	}
	/* set a minimum timeout, and exclude infinite timeouts */
	if(connecttimeout<1)connecttimeout=1;
	dbsetlogintime(connecttimeout);

	msSQL_GLOBAL(num_persistent)=0;
	msSQL_GLOBAL(le_link) = register_list_destructors(_close_mssql_link,NULL);
	msSQL_GLOBAL(le_plink) = register_list_destructors(NULL,_close_mssql_plink);
	msSQL_GLOBAL(le_result) = register_list_destructors(_free_mssql_result,NULL);
	
	return SUCCESS;
}


int php3_rinit_mssql(INIT_FUNC_ARGS)
{
	msSQL_TLS_VARS;

	msSQL_GLOBAL(default_link)=-1;
	msSQL_GLOBAL(num_links) = msSQL_GLOBAL(num_persistent);
	msSQL_GLOBAL(appname) = "PHP";
	msSQL_GLOBAL(server_message) = empty_string;
	msSQL_GLOBAL(min_error_severity) = msSQL_GLOBAL(cfg_min_error_severity);
	msSQL_GLOBAL(min_message_severity) = msSQL_GLOBAL(cfg_min_message_severity);
	return SUCCESS;
}

int php3_mshutdown_mssql(void)
{
	msSQL_TLS_VARS;
	dbexit();
	return SUCCESS;
}

int php3_rshutdown_mssql(void)
{
	msSQL_TLS_VARS;

	STR_FREE(msSQL_GLOBAL(server_message));
	return SUCCESS;
}

static void php3_mssql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	char *user,*passwd,*host;
	char *hashed_details;
	int hashed_details_length;
	mssql_link mssql,*mssql_ptr;
	list_entry *le;
	char buffer[32];
	msSQL_TLS_VARS;

	msSQL_GLOBAL(resource_list) = list;
	msSQL_GLOBAL(resource_plist) = plist;
		
	switch(ARG_COUNT(ht)) {
		case 0: /* defaults */
			host=user=passwd=NULL;
			break;
		case 1: {
				pval *yyhost;
				
				if (getParameters(ht, 1, &yyhost)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				host = yyhost->value.str.val;
				user=passwd=NULL;
			}
			break;
		case 2: {
				pval *yyhost,*yyuser;
				
				if (getParameters(ht, 2, &yyhost, &yyuser)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyuser);
				host = yyhost->value.str.val;
				user = yyuser->value.str.val;
				passwd=NULL;
			}
			break;
		case 3: {
				pval *yyhost,*yyuser,*yypasswd;
			
				if (getParameters(ht, 3, &yyhost, &yyuser, &yypasswd) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyuser);
				convert_to_string(yypasswd);
				host = yyhost->value.str.val;
				user = yyuser->value.str.val;
				passwd = yypasswd->value.str.val;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	hashed_details_length = sizeof("mssql___")-1 + strlen(SAFE_STRING(host))+strlen(SAFE_STRING(user))+strlen(SAFE_STRING(passwd));
	hashed_details = (char *) emalloc(hashed_details_length+1);
	sprintf(hashed_details,"mssql_%s_%s_%s",SAFE_STRING(host), SAFE_STRING(user), SAFE_STRING(passwd));

	return_value->value.lval = 0;
	return_value->type = IS_LONG;

	/* set a DBLOGIN record */	
	if ((mssql.login=dblogin())==NULL) {
		php3_error(E_WARNING,"MS SQL:  Unable to allocate login record");
		RETURN_FALSE;
	}
	
	if (user) {
		DBSETLUSER(mssql.login,user);
	}
	if (passwd) {
		DBSETLPWD(mssql.login,passwd);
	}
	DBSETLAPP(mssql.login,msSQL_GLOBAL(appname));
	mssql.valid = 1;

#if WINNT|WIN32
	DBSETLVERSION(mssql.login, DBVER60);
#endif
//	DBSETLTIME(mssql.login, TIMEOUT_INFINITE);

	if (!msSQL_GLOBAL(allow_persistent)) {
		persistent=0;
	}
	if (persistent) {

		/* try to find if we already have this link in our persistent list */
		if (_php3_hash_find(plist, hashed_details, hashed_details_length+1,
			(void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;

			if (msSQL_GLOBAL(max_links)!=-1 && msSQL_GLOBAL(num_links)>=msSQL_GLOBAL(max_links)) {
				php3_error(E_WARNING,"MS SQL:  Too many open links (%d)",msSQL_GLOBAL(num_links));
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			if (msSQL_GLOBAL(max_persistent)!=-1 && msSQL_GLOBAL(num_persistent)>=msSQL_GLOBAL(max_persistent)) {
				php3_error(E_WARNING,"MS SQL:  Too many open persistent links (%d)",msSQL_GLOBAL(num_persistent));
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			/* create the link */
			if ((mssql.link=dbopen(mssql.login,host))==FAIL) {
				/*php3_error(E_WARNING,"MS SQL:  Unable to connect to server:  %s",mssql_error(mssql));*/
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}

			if (dbsetopt(mssql.link,DBBUFFER,"2" FREETDS_OPTION)==FAIL) {
				efree(hashed_details);
				dbfreelogin(mssql.login);
				dbclose(mssql.link);
				RETURN_FALSE;
			}

			if (msSQL_GLOBAL(textsize) != -1) {
				sprintf(buffer, "%li", msSQL_GLOBAL(textsize));
				if (dbsetopt(mssql.link, DBTEXTSIZE, buffer FREETDS_OPTION)==FAIL) {
					efree(hashed_details);
					dbfreelogin(mssql.login);
					RETURN_FALSE;
				}	
			}
			if (msSQL_GLOBAL(textlimit) != -1) {
				sprintf(buffer, "%li", msSQL_GLOBAL(textlimit));
				if (dbsetopt(mssql.link, DBTEXTLIMIT, buffer FREETDS_OPTION)==FAIL) {
					efree(hashed_details);
					dbfreelogin(mssql.login);
					RETURN_FALSE;
				}
			}

			/* hash it up */
			mssql_ptr = (mssql_link *) malloc(sizeof(mssql_link));
			memcpy(mssql_ptr,&mssql,sizeof(mssql_link));
			new_le.type = msSQL_GLOBAL(le_plink);
			new_le.ptr = mssql_ptr;
			if (_php3_hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry),NULL)==FAILURE) {
				free(mssql_ptr);
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			msSQL_GLOBAL(num_persistent++);
			msSQL_GLOBAL(num_links++);
		} else {  /* we do */
			if (le->type != msSQL_GLOBAL(le_plink)) {
#if BROKEN_MSSQL_PCONNECTS
				log_error("PHP/MS SQL:  Hashed persistent link is not a MS SQL link!",php3_rqst->server);
#endif
				php3_error(E_WARNING,"MS SQL:  Hashed persistent link is not a MS SQL link!");
				RETURN_FALSE;
			}
			
			mssql_ptr = (mssql_link *) le->ptr;
			/* test that the link hasn't died */
			if (DBDEAD(mssql_ptr->link)==TRUE) {
#if BROKEN_MSSQL_PCONNECTS
				log_error("PHP/MS SQL:  Persistent link died, trying to reconnect...",php3_rqst->server);
#endif
				if ((mssql_ptr->link=dbopen(mssql_ptr->login,host))==FAIL) {
#if BROKEN_MSSQL_PCONNECTS
					log_error("PHP/MS SQL:  Unable to reconnect!",php3_rqst->server);
#endif
					php3_error(E_WARNING,"MS SQL:  Link to server lost, unable to reconnect");
					_php3_hash_del(plist, hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
#if BROKEN_MSSQL_PCONNECTS
				log_error("PHP/MS SQL:  Reconnect successful!",php3_rqst->server);
#endif
				if (dbsetopt(mssql_ptr->link,DBBUFFER,"2" FREETDS_OPTION)==FAIL) {
#if BROKEN_MSSQL_PCONNECTS
					log_error("PHP/MS SQL:  Unable to set required options",php3_rqst->server);
#endif
					_php3_hash_del(plist, hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
		}
		return_value->value.lval = php3_list_insert(mssql_ptr,msSQL_GLOBAL(le_plink));
	} else { /* non persistent */
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual mssql link sits.
		 * if it doesn't, open a new mssql link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (_php3_hash_find(list,hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;

			if (index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = php3_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==msSQL_GLOBAL(le_link) || type==msSQL_GLOBAL(le_plink))) {
				return_value->value.lval = msSQL_GLOBAL(default_link) = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				_php3_hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (msSQL_GLOBAL(max_links)!=-1 && msSQL_GLOBAL(num_links)>=msSQL_GLOBAL(max_links)) {
			php3_error(E_WARNING,"MS SQL:  Too many open links (%d)",msSQL_GLOBAL(num_links));
			efree(hashed_details);
			RETURN_FALSE;
		}
		
		if ((mssql.link=dbopen(mssql.login,host))==NULL) {
			/*php3_error(E_WARNING,"MS SQL:  Unable to connect to server:  %s",mssql_error(mssql));*/
			efree(hashed_details);
			RETURN_FALSE;
		}

		if (dbsetopt(mssql.link,DBBUFFER,"2" FREETDS_OPTION)==FAIL) {
			efree(hashed_details);
			dbfreelogin(mssql.login);
			dbclose(mssql.link);
			RETURN_FALSE;
		}

		if (msSQL_GLOBAL(textlimit) != -1) {
			sprintf(buffer, "%li", msSQL_GLOBAL(textlimit));
			if (dbsetopt(mssql.link, DBTEXTLIMIT, buffer FREETDS_OPTION)==FAIL) {
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
		}
		if (msSQL_GLOBAL(textsize) != -1) {
			sprintf(buffer, "SET TEXTSIZE %li", msSQL_GLOBAL(textsize));

			dbcmd(mssql.link, buffer);
			dbsqlexec(mssql.link);
			dbresults(mssql.link);
//			if (dbsetopt(mssql.link, DBTEXTSIZE, buffer)==FAIL) {
//				efree(hashed_details);
//				dbfreelogin(mssql.login);
//				RETURN_FALSE;
//			}	
		}

		/* add it to the list */
		mssql_ptr = (mssql_link *) emalloc(sizeof(mssql_link));
		memcpy(mssql_ptr,&mssql,sizeof(mssql_link));
		return_value->value.lval = php3_list_insert(mssql_ptr,msSQL_GLOBAL(le_link));
		
		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (_php3_hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry),NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		msSQL_GLOBAL(num_links++);
	}
	efree(hashed_details);
	msSQL_GLOBAL(default_link)=return_value->value.lval;
}


static int php3_mssql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	msSQL_TLS_VARS;

	if (msSQL_GLOBAL(default_link)==-1) { /* no link opened yet, implicitly open one */
		HashTable dummy;

		_php3_hash_init(&dummy,0,NULL,NULL,0);
		php3_mssql_do_connect(&dummy,return_value,list,plist,0);
		_php3_hash_destroy(&dummy);
	}
	return msSQL_GLOBAL(default_link);
}


/* {{{ proto int mssql_connect([string host [, string user [, string password]]])
   Open MS-SQL server connection */
void php3_mssql_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mssql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto int mssql_pconnect([string host [, string user [, string password]]])
   Open permanent MS-SQL server connection */
void php3_mssql_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mssql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto bool mssql_close([int link_id])
   Close MS-SQL connection */
void php3_mssql_close(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_link_index;
	int id,type;
	msSQL_TLS_VARS;

	
	switch (ARG_COUNT(ht)) {
		case 0:
			id = msSQL_GLOBAL(default_link);
			break;
		case 1:
			if (getParameters(ht, 1, &mssql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mssql_link_index);
			id = mssql_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_link) && type!=msSQL_GLOBAL(le_plink)) {
		php3_error(E_WARNING,"%d is not a MS SQL link index",id);
		RETURN_FALSE;
	}
	
	php3_list_delete(id);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool mssql_select_db(string database [, int link_id])
   Select MS-SQL database */
void php3_mssql_select_db(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*mssql_link_index;
	int id,type;
	mssql_link  *mssql_ptr;
	msSQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_mssql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &mssql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mssql_link_index);
			id = mssql_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	mssql_ptr = (mssql_link *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_link) && type!=msSQL_GLOBAL(le_plink)) {
		php3_error(E_WARNING,"%d is not a MS SQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	
	if (dbuse(mssql_ptr->link,db->value.str.val)==FAIL) {
		/*php3_error(E_WARNING,"MS SQL:  Unable to select database:  %s",mssql_error(mssql));*/
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */

static void php3_mssql_get_column_content_with_type(mssql_link *mssql_ptr,int offset,pval *result, int column_type)
{
	if (dbdatlen(mssql_ptr->link,offset) == 0) {
		var_reset(result);
		return;
	}

	switch (column_type)
	{
		case SQLINT1:
		case SQLINT2:
		case SQLINT4:
		case SQLINTN: {	
			result->value.lval = (long) anyintcol(offset);
			result->type = IS_LONG;
			break;
		}
		case SQLCHAR:
		case SQLVARCHAR:
		case SQLTEXT: {
			int length;
			char *data = charcol(offset);

			length=dbdatlen(mssql_ptr->link,offset);
			while (length>0 && charcol(offset)[length-1] == ' ') { /* nuke trailing whitespace */
				length--;
			}
			result->value.str.val = estrndup(data,length);
			result->value.str.len = length;
			result->type = IS_STRING;
			break;
		}
		case SQLFLT8: {
			result->value.dval = (double) floatcol(offset);
			result->type = IS_DOUBLE;
			break;
		}
		case SQLNUMERIC:
		default: {
			if (dbwillconvert(coltype(offset),SQLCHAR)) {
				char *res_buf;
				int res_length = dbdatlen(mssql_ptr->link,offset);
			
				res_buf = (char *) emalloc(res_length+1);
				memset(res_buf, 0, res_length+1);
				dbconvert(NULL,coltype(offset),dbdata(mssql_ptr->link,offset), res_length,SQLCHAR,res_buf,-1);
		
				result->value.str.len = res_length;
				result->value.str.val = res_buf;
				result->type = IS_STRING;
			} else {
				php3_error(E_WARNING,"MS SQL:  column %d has unknown data type (%d)", offset, coltype(offset));
				var_reset(result);
			}
		}
	}
}

static void php3_mssql_get_column_content_without_type(mssql_link *mssql_ptr,int offset,pval *result, int column_type)
{
	if (dbdatlen(mssql_ptr->link,offset) == 0) {
		var_reset(result);
		return;
	}
	if (dbwillconvert(coltype(offset),SQLCHAR)) {
		unsigned char *res_buf;
		int res_length = dbdatlen(mssql_ptr->link,offset);
		
		res_buf = (unsigned char *) emalloc(res_length+1 + 19);
		memset(res_buf, 0, res_length+1 + 10);
		dbconvert(NULL,coltype(offset),dbdata(mssql_ptr->link,offset), res_length, SQLCHAR,res_buf,-1);
		
		result->value.str.len = strlen(res_buf);
		result->value.str.val = res_buf;
		result->type = IS_STRING;
	} else {
		php3_error(E_WARNING,"MS SQL:  column %d has unknown data type (%d)", offset, coltype(offset));
		var_reset(result);
	}
}


/* {{{ proto int mssql_query(string query [, int link_id])
   Send MS-SQL query */
void php3_mssql_query(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *query,*mssql_link_index;
	int id,type,retvalue;
	mssql_link *mssql_ptr;
	mssql_result *result;
	int num_fields;
	int blocks_initialized=1;
	int i,j;
	int *column_types;
	msSQL_TLS_VARS;


	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = msSQL_GLOBAL(default_link);
			break;
		case 2:
			if (getParameters(ht, 2, &query, &mssql_link_index)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mssql_link_index);
			id = mssql_link_index->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	mssql_ptr = (mssql_link *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_link) && type!=msSQL_GLOBAL(le_plink)) {
		php3_error(E_WARNING,"%d is not a MS SQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	
	if (dbcmd(mssql_ptr->link,query->value.str.val)==FAIL) {
		php3_error(E_WARNING,"MS SQL:  Unable to set query");
		RETURN_FALSE;
	}
	if (dbsqlexec(mssql_ptr->link)==FAIL || dbresults(mssql_ptr->link)==FAIL) {
		php3_error(E_WARNING,"MS SQL:  Query failed");
		RETURN_FALSE;
	}
	
	/* The following is more or less the equivalent of mysql_store_result().
	 * fetch all rows from the server into the row buffer, thus:
	 * 1)  Being able to fire up another query without explicitly reading all rows
	 * 2)  Having numrows accessible
	 */
	retvalue=dbnextrow(mssql_ptr->link);
	
	if (retvalue==FAIL) {
		RETURN_FALSE;
	}

	num_fields = dbnumcols(mssql_ptr->link);
	if (num_fields<=0) {
		RETURN_TRUE;
	}
	
	column_types = (int *) emalloc(sizeof(int) * num_fields);
	for (i=0; i<num_fields; i++) {
		column_types[i] = coltype(i+1);
	}
	
	result = (mssql_result *) emalloc(sizeof(mssql_result));
	result->data = (pval **) emalloc(sizeof(pval *)*MSSQL_ROWS_BLOCK);
	result->mssql_ptr = mssql_ptr;
	result->cur_field=result->cur_row=result->num_rows=0;
	result->num_fields = num_fields;

	i=0;
	while (retvalue!=FAIL && retvalue!=NO_MORE_ROWS) {
		result->num_rows++;
		if (result->num_rows > blocks_initialized*MSSQL_ROWS_BLOCK) {
			result->data = (pval **) erealloc(result->data,sizeof(pval *)*MSSQL_ROWS_BLOCK*(++blocks_initialized));
		}
		result->data[i] = (pval *) emalloc(sizeof(pval)*num_fields);
		for (j=1; j<=num_fields; j++) {
			msSQL_GLOBAL(get_column_content(mssql_ptr, j, &result->data[i][j-1], column_types[j-1]));
		}
		retvalue=dbnextrow(mssql_ptr->link);
		dbclrbuf(mssql_ptr->link,DBLASTROW(mssql_ptr->link)-1); 
		i++;
	}
	result->num_rows = DBCOUNT(mssql_ptr->link);
	
	result->fields = (mssql_field *) emalloc(sizeof(mssql_field)*num_fields);
	j=0;
	for (i=0; i<num_fields; i++) {
		char *fname = (char *)dbcolname(mssql_ptr->link,i+1);
		char computed_buf[16];
		
		if (*fname) {
			result->fields[i].name = estrdup(fname);
		} else {
			if (j>0) {
				snprintf(computed_buf,16,"computed%d",j);
			} else {
				strcpy(computed_buf,"computed");
			}
			result->fields[i].name = estrdup(computed_buf);
			j++;
		}
		result->fields[i].max_length = dbcollen(mssql_ptr->link,i+1);
		result->fields[i].column_source = estrdup(dbcolsource(mssql_ptr->link,i+1));
		if (!result->fields[i].column_source) {
			result->fields[i].column_source = empty_string;
		}
		result->fields[i].type = column_types[i];
		/* set numeric flag */
		switch (column_types[i]) {
			case SQLINT1:
			case SQLINT2:
			case SQLINT4:
			case SQLFLT8:
			case SQLNUMERIC:
			case SQLDECIMAL:
				result->fields[i].numeric = 1;
				break;
			case SQLCHAR:
			case SQLVARCHAR:
			case SQLTEXT:
			default:
				result->fields[i].numeric = 0;
				break;
		}
	}
	efree(column_types);
	return_value->value.lval = php3_list_insert(result,msSQL_GLOBAL(le_result));
	return_value->type = IS_LONG;
}
/* }}} */
                        
/* {{{ proto bool mssql_free_result(int result)
   Free result memory */
void php3_mssql_free_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index;
	mssql_result *result;
	int type;
	msSQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	if (mssql_result_index->value.lval==0) {
		RETURN_FALSE;
	}
	result = (mssql_result *) php3_list_find(mssql_result_index->value.lval,&type);
	
	if (type!=msSQL_GLOBAL(le_result)) {
//		php3_error(E_WARNING,"%d is not a MS SQL result index",mssql_result_index->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(mssql_result_index->value.lval);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mssql_get_last_message(void)
   Returns the last message from server (over min_message_severity?) */
void php3_mssql_get_last_message(INTERNAL_FUNCTION_PARAMETERS)
{
	msSQL_TLS_VARS;

	RETURN_STRING(msSQL_GLOBAL(server_message),1);
}
/* }}} */

/* {{{ proto int mssql_num_rows(int result)
   Get number of rows in result */
void php3_mssql_num_rows(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result_index;
	int type,id;
	mssql_result *result;
	msSQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result_index);
	id = result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}	
	
	return_value->value.lval = result->num_rows;
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int mssql_num_fields(int result)
   Get number of fields in result */
void php3_mssql_num_fields(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result_index;
	int type,id;
	mssql_result *result;
	msSQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result_index);
	id = result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}	
	
	return_value->value.lval = result->num_fields;
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto array mssql_fetch_row(int result)
   Get row as enumerated array */
void php3_mssql_fetch_row(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index;
	int type,i,id;
	mssql_result *result;
	pval field_content;
	msSQL_TLS_VARS;


	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	for (i=0; i<result->num_fields; i++) {
		field_content = result->data[result->cur_row][i];
		pval_copy_constructor(&field_content);
		_php3_hash_index_update(return_value->value.ht, i, (void *) &field_content, sizeof(pval), NULL);
	}
	result->cur_row++;
}
/* }}} */

static void php3_mssql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index;
	mssql_result *result;
	int type;
	int i;
	pval *pvalue_ptr,tmp;
	msSQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	result = (mssql_result *) php3_list_find(mssql_result_index->value.lval,&type);
	
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",mssql_result_index->value.lval);
		RETURN_FALSE;
	}
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	
	for (i=0; i<result->num_fields; i++) {
		tmp = result->data[result->cur_row][i];
		pval_copy_constructor(&tmp);
		if (msSQL_GLOBAL(magic_quotes_runtime) && tmp.type == IS_STRING) {
			tmp.value.str.val = _php3_addslashes(tmp.value.str.val,tmp.value.str.len,&tmp.value.str.len,1);
		}
		_php3_hash_index_update(return_value->value.ht, i, (void *) &tmp, sizeof(pval), (void **) &pvalue_ptr);
		_php3_hash_pointer_update(return_value->value.ht, result->fields[i].name, strlen(result->fields[i].name)+1, pvalue_ptr);
	}
	result->cur_row++;
}

/* {{{ proto object mssql_fetch_object(int result)
   Fetch row as object */
void php3_mssql_fetch_object(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
	}
}
/* }}} */

/* {{{ proto array mssql_fetch_array(int result)
   Fetch row as array */
void php3_mssql_fetch_array(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto bool mssql_data_seek(int result, int offset)
   Move internal row pointer */
void php3_mssql_data_seek(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id;
	mssql_result *result;
	msSQL_TLS_VARS;


	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &mssql_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}

	convert_to_long(offset);
	if (offset->value.lval<0 || offset->value.lval>=result->num_rows) {
		php3_error(E_WARNING,"MS SQL:  Bad row offset");
		RETURN_FALSE;
	}
	
	result->cur_row = offset->value.lval;
	RETURN_TRUE;
}
/* }}} */

static char *php3_mssql_get_field_name(int type)
{
	switch (type) {
		case SQLBINARY:
		case SQLVARBINARY:
			return "blob";
			break;
		case SQLCHAR:
		case SQLVARCHAR:
			return "char";
			break;
		case SQLTEXT:
			return "text";
			break;
		case SQLDATETIME:
		case SQLDATETIM4:
		case SQLDATETIMN:
			return "datetime";
			break;
		case SQLDECIMAL:
		case SQLFLT8:
		case SQLFLTN:
			return "real";
			break;
		case SQLINT1:
		case SQLINT2:
		case SQLINT4:
		case SQLINTN:
			return "int";
			break;
		case SQLNUMERIC:
			return "numeric";
			break;
		case SQLMONEY:
		case SQLMONEY4:
		case SQLMONEYN:
			return "money";
			break;
		case SQLBIT:
			return "bit";
			break;
		case SQLIMAGE:
			return "image";
			break;
		default:
			return "unknown";
			break;
	}
}


/* {{{ proto object mssql_fetch_field(int result [, int offset])
   Get field information */
void php3_mssql_fetch_field(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;
	msSQL_TLS_VARS;


	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &mssql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (getParameters(ht, 2, &mssql_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(offset);
			field_offset = offset->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ARG_COUNT(ht)==2) { /* field specified explicitly */
			php3_error(E_WARNING,"MS SQL:  Bad column offset");
		}
		RETURN_FALSE;
	}

	if (object_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	add_property_string(return_value, "name",result->fields[field_offset].name, 1);
	add_property_long(return_value, "max_length",result->fields[field_offset].max_length);
	add_property_string(return_value, "column_source",result->fields[field_offset].column_source, 1);
	add_property_long(return_value, "numeric", result->fields[field_offset].numeric);
	add_property_string(return_value, "type", php3_mssql_get_field_name(result->fields[field_offset].type), 1);
}
/* }}} */

/* {{{ proto int mssql_field_length(int result [, int offset])
   Get the length of a field */
void php3_mssql_field_length(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;
	msSQL_TLS_VARS;


	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &mssql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (getParameters(ht, 2, &mssql_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(offset);
			field_offset = offset->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ARG_COUNT(ht)==2) { /* field specified explicitly */
			php3_error(E_WARNING,"MS SQL:  Bad column offset");
		}
		RETURN_FALSE;
	}

	return_value->value.lval = result->fields[field_offset].max_length;
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto string mssql_field_name(int result [, int offset])
   Get the name of a field */
void php3_mssql_field_name(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;
	msSQL_TLS_VARS;


	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &mssql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (getParameters(ht, 2, &mssql_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(offset);
			field_offset = offset->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ARG_COUNT(ht)==2) { /* field specified explicitly */
			php3_error(E_WARNING,"MS SQL:  Bad column offset");
		}
		RETURN_FALSE;
	}

	return_value->value.str.val = estrdup(result->fields[field_offset].name);
	return_value->value.str.len = strlen(result->fields[field_offset].name);
	return_value->type = IS_STRING;
}
/* }}} */

/* {{{ proto string mssql_field_type(int result [, int offset])
   Get the type of a field */
void php3_mssql_field_type(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;
	msSQL_TLS_VARS;


	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &mssql_result_index)==FAILURE) {
				RETURN_FALSE;
			}
			field_offset=-1;
			break;
		case 2:
			if (getParameters(ht, 2, &mssql_result_index, &offset)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(offset);
			field_offset = offset->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (field_offset==-1) {
		field_offset = result->cur_field;
		result->cur_field++;
	}
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		if (ARG_COUNT(ht)==2) { /* field specified explicitly */
			php3_error(E_WARNING,"MS SQL:  Bad column offset");
		}
		RETURN_FALSE;
	}

	return_value->value.str.val = estrdup(php3_mssql_get_field_name(result->fields[field_offset].type));
	return_value->value.str.len = strlen(php3_mssql_get_field_name(result->fields[field_offset].type));
	return_value->type = IS_STRING;
}
/* }}} */

/* {{{ proto bool mssql_field_seek(int result, int offset)
   Set field offset */
void php3_mssql_field_seek(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;
	msSQL_TLS_VARS;


	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &mssql_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	convert_to_long(offset);
	field_offset = offset->value.lval;
	
	if (field_offset<0 || field_offset >= result->num_fields) {
		php3_error(E_WARNING,"MS SQL:  Bad column offset");
		RETURN_FALSE;
	}

	result->cur_field = field_offset;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string mssql_result(int result, int row, mixed field)
   Get result data */
void php3_mssql_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *row, *field, *mssql_result_index;
	int id,type,field_offset=0;
	mssql_result *result;
	msSQL_TLS_VARS;


	if (ARG_COUNT(ht)!=3 || getParameters(ht, 3, &mssql_result_index, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=msSQL_GLOBAL(le_result)) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=result->num_rows) {
		php3_error(E_WARNING,"MS SQL:  Bad row offset (%d)",row->value.lval);
		RETURN_FALSE;
	}

	switch(field->type) {
		case IS_STRING: {
			int i;

			for (i=0; i<result->num_fields; i++) {
				if (!strcasecmp(result->fields[i].name,field->value.str.val)) {
					field_offset = i;
					break;
				}
			}
			if (i>=result->num_fields) { /* no match found */
				php3_error(E_WARNING,"MS SQL:  %s field not found in result",field->value.str.val);
				RETURN_FALSE;
			}
			break;
		}
		default:
			convert_to_long(field);
			field_offset = field->value.lval;
			if (field_offset<0 || field_offset>=result->num_fields) {
				php3_error(E_WARNING,"MS SQL:  Bad column offset specified");
				RETURN_FALSE;
			}
			break;
	}

	*return_value = result->data[row->value.lval][field_offset];
	pval_copy_constructor(return_value);
}
/* }}} */

void php3_info_mssql(void)
{
	char maxp[16],maxl[16];
	msSQL_TLS_VARS;

	
	if (msSQL_GLOBAL(max_persistent==-1)) {
		strcpy(maxp,"Unlimited");
	} else {
		snprintf(maxp,15,"%ld",msSQL_GLOBAL(max_persistent));
		maxp[15]=0;
	}
	if (msSQL_GLOBAL(max_links==-1)) {
		strcpy(maxl,"Unlimited");
	} else {
		snprintf(maxl,15,"%ld",msSQL_GLOBAL(max_links));
		maxl[15]=0;
	}
	php3_printf("<table cellpadding=5>"
				"<tr><td>Allow persistent links:</td><td>%s</td></tr>\n"
				"<tr><td>Persistent links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Total links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Application name:</td><td>%s</td></tr>\n"
				"<tr><td valign=\"top\" width=\"20%%\">Client API information:</td><td><pre>%s</pre></td></tr>\n"
				"</table>\n",
				(msSQL_GLOBAL(allow_persistent)?"Yes":"No"),
				msSQL_GLOBAL(num_persistent),maxp,
				msSQL_GLOBAL(num_links),maxl,
				msSQL_GLOBAL(appname),
				"MSSQL 6.5");
}


/* {{{ proto void mssql_min_error_severity(int severity)
   Sets the lower error severity */
void php3_mssql_min_error_severity(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *severity;
	msSQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	msSQL_GLOBAL(min_error_severity) = severity->value.lval;
}
/* }}} */

/* {{{ proto void mssql_min_message_severity(int severity)
   Sets the lower message severity */
void php3_mssql_min_message_severity(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *severity;
	msSQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	msSQL_GLOBAL(min_message_severity) = severity->value.lval;
}
/* }}} */

#endif
