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
   | Authors: Zeev Suraski <bourbon@netvision.net.il> Original sybase.c   |
   |          Frank M. Kromann <fmk@businesnet.dk>                        |
   +----------------------------------------------------------------------+
   | php3_mssql_get_column_content_with_type() based on code by:          |
   |                     Muhammad A Muquit <MA_Muquit@fccc.edu>           |
   |                     Rasmus Lerdorf <rasmus@lerdorf.on.ca>            |
   +----------------------------------------------------------------------+
 */
 
#ifdef THREAD_SAFE
#include "tls.h"
#endif

#ifndef MSVC5
#include "config.h"
#endif
#include "php.h"
#include "internal_functions.h"


#include "php3_mssql.h"

#if HAVE_MSSQL

#include "php3_string.h"
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
//	"Microsoft SQL", mssql_functions, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, NULL
};

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &mssql_module_entry; }
#endif

/*
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{

    switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
		break;    
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
    }
    return 1;
}
*/

//THREAD_LS 
mssql_module php3_mssql_module;
static HashTable *resource_list, *resource_plist;


#define CHECK_LINK(link) { if (link==-1) { php3_error(E_WARNING,"MS SQL:  A link to the server could not be established"); RETURN_FALSE; } }


static void php3_mssql_get_column_content_with_type(mssql_link *mssql_ptr,int offset,pval *result, int column_type);
static void php3_mssql_get_column_content_without_type(mssql_link *mssql_ptr,int offset,pval *result, int column_type);

/* error handler */
static int php3_mssql_error_handler(DBPROCESS *dbproc,int severity,int dberr,
										int oserr,char *dberrstr,char *oserrstr)
{
	if (severity >= php3_mssql_module.min_error_severity) {
		php3_error(E_WARNING,"MS SQL error:  %s (severity %d)",dberrstr,severity);
	}
	return INT_CANCEL;  
}

/* message handler */
static int php3_mssql_message_handler(DBPROCESS *dbproc,DBINT msgno,int msgstate,
										int severity,char *msgtext,char *srvname,
										char *procname,DBUSMALLINT line)
{
	if (severity >= php3_mssql_module.min_message_severity) {
		php3_error(E_WARNING,"MS SQL message:  %s (severity %d)",msgtext,severity);
	}
	STR_FREE(php3_mssql_module.server_message);
	php3_mssql_module.server_message = estrdup(msgtext);
	return 0;
}


static int _clean_invalid_results(list_entry *le)
{
	if (le->type == php3_mssql_module.le_result) {
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
				yystype_destructor(&result->data[i][j] _INLINE_TLS);
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
	mssql_ptr->valid = 0;
	_php3_hash_apply(resource_list,(int (*)(void *))_clean_invalid_results);
	dbclose(mssql_ptr->link);
	dbfreelogin(mssql_ptr->login);
	efree(mssql_ptr);
	php3_mssql_module.num_links--;
}


static void _close_mssql_plink(mssql_link *mssql_ptr)
{
	dbclose(mssql_ptr->link);
	dbfreelogin(mssql_ptr->login);
	free(mssql_ptr);
	php3_mssql_module.num_persistent--;
	php3_mssql_module.num_links--;
}


int php3_minit_mssql(INIT_FUNC_ARGS)
{
	char *interface_file;
	long compatability_mode;

	if (dbinit()==FAIL) {
		return FAILURE;
	}
	dberrhandle((DBERRHANDLE_PROC) php3_mssql_error_handler);
	dbmsghandle((DBMSGHANDLE_PROC) php3_mssql_message_handler);

	if (cfg_get_string("mssql.interface_file",&interface_file)==SUCCESS) {
//		dbsetifile(interface_file);
	}
	if (cfg_get_long("mssql.allow_persistent",&php3_mssql_module.allow_persistent)==FAILURE) {
		php3_mssql_module.allow_persistent=1;
	}
	if (cfg_get_long("mssql.max_persistent",&php3_mssql_module.max_persistent)==FAILURE) {
		php3_mssql_module.max_persistent=-1;
	}
	if (cfg_get_long("mssql.max_links",&php3_mssql_module.max_links)==FAILURE) {
		php3_mssql_module.max_links=-1;
	}
	if (cfg_get_long("mssql.min_error_severity",&php3_mssql_module.cfg_min_error_severity)==FAILURE) {
		php3_mssql_module.cfg_min_error_severity=10;
	}
	if (cfg_get_long("mssql.min_message_severity",&php3_mssql_module.cfg_min_message_severity)==FAILURE) {
		php3_mssql_module.cfg_min_message_severity=10;
	}
	if (cfg_get_long("mssql.compatability_mode",&compatability_mode)==FAILURE) {
		compatability_mode = 0;
	}
	if (compatability_mode) {
		php3_mssql_module.get_column_content = php3_mssql_get_column_content_with_type;
	} else {
		php3_mssql_module.get_column_content = php3_mssql_get_column_content_without_type;	
	}
	
	php3_mssql_module.num_persistent=0;
	php3_mssql_module.le_link = register_list_destructors(_close_mssql_link,NULL);
	php3_mssql_module.le_plink = register_list_destructors(NULL,_close_mssql_plink);
	php3_mssql_module.le_result = register_list_destructors(_free_mssql_result,NULL);
	
	return SUCCESS;
}


int php3_rinit_mssql(INIT_FUNC_ARGS)
{
	php3_mssql_module.default_link=-1;
	php3_mssql_module.num_links = php3_mssql_module.num_persistent;
	php3_mssql_module.appname = "PHP";
	php3_mssql_module.server_message = empty_string;
	php3_mssql_module.min_error_severity = php3_mssql_module.cfg_min_error_severity;
	php3_mssql_module.min_message_severity = php3_mssql_module.cfg_min_message_severity;
	return SUCCESS;
}

int php3_mshutdown_mssql(void)
{
	dbexit();
	return SUCCESS;
}

int php3_rshutdown_mssql(void)
{
	STR_FREE(php3_mssql_module.server_message);
	return SUCCESS;
}

static void php3_mssql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	char *user,*passwd,*host;
	char *hashed_details;
	int hashed_details_length;
	mssql_link mssql,*mssql_ptr;

	resource_list = list;
	resource_plist = plist;
		
	switch(ARG_COUNT(ht)) {
		case 0: /* defaults */
			host=user=passwd=NULL;
			hashed_details_length=6+3;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			strcpy(hashed_details,"mssql___");
			break;
		case 1: {
				pval *yyhost;
				
				if (getParameters(ht, 1, &yyhost)==FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				host = yyhost->value.str.val;
				user=passwd=NULL;
				hashed_details_length = yyhost->value.str.len+6+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"mssql_%s__",yyhost->value.str.val);
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
				hashed_details_length = yyhost->value.str.len+yyuser->value.str.len+6+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"mssql_%s_%s_",yyhost->value.str.val,yyuser->value.str.val);
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
				hashed_details_length = yyhost->value.str.len+yyuser->value.str.len+yypasswd->value.str.len+6+3;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"mssql_%s_%s_%s",yyhost->value.str.val,yyuser->value.str.val,yypasswd->value.str.val); /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}


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
	DBSETLAPP(mssql.login,php3_mssql_module.appname);
	mssql.valid = 1;

	DBSETLVERSION(mssql.login, DBVER60);
//	DBSETLTIME(mssql.login, TIMEOUT_INFINITE);


	if (!php3_mssql_module.allow_persistent) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;

		/* try to find if we already have this link in our persistent list */
		if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;

			if (php3_mssql_module.max_links!=-1 && php3_mssql_module.num_links>=php3_mssql_module.max_links) {
				php3_error(E_WARNING,"MS SQL:  Too many open links (%d)",php3_mssql_module.num_links);
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			if (php3_mssql_module.max_persistent!=-1 && php3_mssql_module.num_persistent>=php3_mssql_module.max_persistent) {
				php3_error(E_WARNING,"MS SQL:  Too many open persistent links (%d)",php3_mssql_module.num_persistent);
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

//			if (dbsetopt(mssql.link,DBBUFFER,"2",-1)==FAIL) {
			if (dbsetopt(mssql.link,DBBUFFER,"2")==FAIL) {
				efree(hashed_details);
				dbfreelogin(mssql.login);
				dbclose(mssql.link);
				RETURN_FALSE;
			}

			/* hash it up */
			mssql_ptr = (mssql_link *) malloc(sizeof(mssql_link));
			memcpy(mssql_ptr,&mssql,sizeof(mssql_link));
			new_le.type = php3_mssql_module.le_plink;
			new_le.ptr = mssql_ptr;
			if (_php3_hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry),NULL)==FAILURE) {
				free(mssql_ptr);
				efree(hashed_details);
				dbfreelogin(mssql.login);
				RETURN_FALSE;
			}
			php3_mssql_module.num_persistent++;
			php3_mssql_module.num_links++;
		} else {  /* we do */
			if (le->type != php3_mssql_module.le_plink) {
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
					/*php3_error(E_WARNING,"MS SQL:  Link to server lost, unable to reconnect");*/
					_php3_hash_del(plist, hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
#if BROKEN_MSSQL_PCONNECTS
				log_error("PHP/MS SQL:  Reconnect successful!",php3_rqst->server);
#endif
//				if (dbsetopt(mssql_ptr->link,DBBUFFER,"2",-1)==FAIL) {
				if (dbsetopt(mssql_ptr->link,DBBUFFER,"2")==FAIL) {
#if BROKEN_MSSQL_PCONNECTS
					log_error("PHP/MS SQL:  Unable to set required options",php3_rqst->server);
#endif
					_php3_hash_del(plist, hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
		}
		return_value->value.lval = php3_list_insert(mssql_ptr,php3_mssql_module.le_plink);
		return_value->type = IS_LONG;
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
			if (ptr && (type==php3_mssql_module.le_link || type==php3_mssql_module.le_plink)) {
				return_value->value.lval = php3_mssql_module.default_link = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				_php3_hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (php3_mssql_module.max_links!=-1 && php3_mssql_module.num_links>=php3_mssql_module.max_links) {
			php3_error(E_WARNING,"MS SQL:  Too many open links (%d)",php3_mssql_module.num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		
		if ((mssql.link=dbopen(mssql.login,host))==NULL) {
			/*php3_error(E_WARNING,"MS SQL:  Unable to connect to server:  %s",mssql_error(mssql));*/
			efree(hashed_details);
			RETURN_FALSE;
		}

//		if (dbsetopt(mssql.link,DBBUFFER,"2",-1)==FAIL) {
		if (dbsetopt(mssql.link,DBBUFFER,"2")==FAIL) {
			efree(hashed_details);
			dbfreelogin(mssql.login);
			dbclose(mssql.link);
			RETURN_FALSE;
		}
		
		/* add it to the list */
		mssql_ptr = (mssql_link *) emalloc(sizeof(mssql_link));
		memcpy(mssql_ptr,&mssql,sizeof(mssql_link));
		return_value->value.lval = php3_list_insert(mssql_ptr,php3_mssql_module.le_link);
		return_value->type = IS_LONG;
		
		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (_php3_hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry),NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		php3_mssql_module.num_links++;
	}
	efree(hashed_details);
	php3_mssql_module.default_link=return_value->value.lval;
}


static int php3_mssql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (php3_mssql_module.default_link==-1) { /* no link opened yet, implicitly open one */
		HashTable dummy;

		_php3_hash_init(&dummy,0,NULL,NULL,0);
		php3_mssql_do_connect(&dummy,return_value,list,plist,0);
		_php3_hash_destroy(&dummy);
	}
	return php3_mssql_module.default_link;
}


DLEXPORT void php3_mssql_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mssql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

DLEXPORT void php3_mssql_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mssql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}


DLEXPORT void php3_mssql_close(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_link_index;
	int id,type;
	
	switch (ARG_COUNT(ht)) {
		case 0:
			id = php3_mssql_module.default_link;
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
	if (type!=php3_mssql_module.le_link && type!=php3_mssql_module.le_plink) {
		php3_error(E_WARNING,"%d is not a MS SQL link index",id);
		RETURN_FALSE;
	}
	
	php3_list_delete(id);
	RETURN_TRUE;
}
	

DLEXPORT void php3_mssql_select_db(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*mssql_link_index;
	int id,type;
	mssql_link  *mssql_ptr;
	
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
	if (type!=php3_mssql_module.le_link && type!=php3_mssql_module.le_plink) {
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


static void php3_mssql_get_column_content_with_type(mssql_link *mssql_ptr,int offset,pval *result, int column_type)
{
	if (dbdatlen(mssql_ptr->link,offset) == 0) {
		var_reset(result);
		return;
	}

	switch (column_type)
	{
		case SQLINT2:
		case SQLINT4: 
		case SQLNUMERIC: {	
			result->value.lval = (long) anyintcol(offset);
			result->type = IS_LONG;
			break;
		}
		case SQLCHAR:
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
		default: {
			if (dbwillconvert(coltype(offset),SQLCHAR)) {
				char *res_buf;
				int res_length = dbdatlen(mssql_ptr->link,offset);
				register char *p;
			
				res_buf = (char *) emalloc(res_length+1);
				memset(res_buf, ' ', res_length+1);
				dbconvert(NULL,coltype(offset),dbdata(mssql_ptr->link,offset), res_length,SQLCHAR,res_buf,-1);
		
				/* get rid of trailing spaces */
				p = res_buf + res_length;
				while (*p == ' ') {
					p--;
					res_length--;
				}
				*(++p) = 0; /* put a trailing NULL */
		
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
		char *res_buf;
		int res_length = dbdatlen(mssql_ptr->link,offset);
		register char *p;
			
		res_buf = (char *) emalloc(res_length+1);
		memset(res_buf, ' ', res_length+1);
		dbconvert(NULL,coltype(offset),dbdata(mssql_ptr->link,offset), res_length,SQLCHAR,res_buf,-1);
		
		/* get rid of trailing spaces */
		p = res_buf + res_length;
		while (*p == ' ') {
			p--;
			res_length--;
		}
		*(++p) = 0; /* put a trailing NULL */
		
		result->value.str.len = res_length;
		result->value.str.val = res_buf;
		result->type = IS_STRING;
	} else {
		php3_error(E_WARNING,"MS SQL:  column %d has unknown data type (%d)", offset, coltype(offset));
		var_reset(result);
	}
}


DLEXPORT void php3_mssql_query(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *query,*mssql_link_index;
	int id,type,retvalue;
	mssql_link *mssql_ptr;
	mssql_result *result;
	int num_fields;
	int blocks_initialized=1;
	int i,j;
	int *column_types;

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_mssql_module.default_link;
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
	if (type!=php3_mssql_module.le_link && type!=php3_mssql_module.le_plink) {
		php3_error(E_WARNING,"%d is not a MS SQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	if (dbcmd(mssql_ptr->link,query->value.str.val)==FAIL) {
		/*php3_error(E_WARNING,"MS SQL:  Unable to set query");*/
		RETURN_FALSE;
	}
	if (dbsqlexec(mssql_ptr->link)==FAIL || dbresults(mssql_ptr->link)==FAIL) {
		/*php3_error(E_WARNING,"MS SQL:  Query failed");*/
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
			php3_mssql_module.get_column_content(mssql_ptr, j, &result->data[i][j-1], column_types[j-1]);
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
			case SQLINT2:
			case SQLINT4:
			case SQLFLT8:
			case SQLNUMERIC:
			case SQLDECIMAL:
				result->fields[i].numeric = 1;
				break;
			case SQLCHAR:
			case SQLTEXT:
			default:
				result->fields[i].numeric = 0;
				break;
		}
	}
	efree(column_types);
	return_value->value.lval = php3_list_insert(result,php3_mssql_module.le_result);
	return_value->type = IS_LONG;
}

                        
DLEXPORT void php3_mssql_free_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index;
	mssql_result *result;
	int type;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	if (mssql_result_index->value.lval==0) {
		RETURN_FALSE;
	}
	result = (mssql_result *) php3_list_find(mssql_result_index->value.lval,&type);
	
	if (type!=php3_mssql_module.le_result) {
//		php3_error(E_WARNING,"%d is not a MS SQL result index",mssql_result_index->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(mssql_result_index->value.lval);
	RETURN_TRUE;
}


DLEXPORT void php3_mssql_get_last_message(INTERNAL_FUNCTION_PARAMETERS)
{
	RETURN_STRING(php3_mssql_module.server_message,1);
}


DLEXPORT void php3_mssql_num_rows(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result_index;
	int type,id;
	mssql_result *result;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result_index);
	id = result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=php3_mssql_module.le_result) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}	
	
	return_value->value.lval = result->num_rows;
	return_value->type = IS_LONG;
}


DLEXPORT void php3_mssql_num_fields(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result_index;
	int type,id;
	mssql_result *result;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result_index);
	id = result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=php3_mssql_module.le_result) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}	
	
	return_value->value.lval = result->num_fields;
	return_value->type = IS_LONG;
}


DLEXPORT void php3_mssql_fetch_row(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index;
	int type,i,id;
	mssql_result *result;
	pval field_content;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=php3_mssql_module.le_result) {
		php3_error(E_WARNING,"%d is not a MS SQL result index",id);
		RETURN_FALSE;
	}
	
	if (result->cur_row >= result->num_rows) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	for (i=0; i<result->num_fields; i++) {
		field_content = result->data[result->cur_row][i];
		yystype_copy_constructor(&field_content);
		_php3_hash_index_update(return_value->value.ht, i, (void *) &field_content, sizeof(pval), NULL);
	}
	result->cur_row++;
}


static void php3_mssql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index;
	mssql_result *result;
	int type;
	int i;
	pval *yystype_ptr,tmp;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &mssql_result_index)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	result = (mssql_result *) php3_list_find(mssql_result_index->value.lval,&type);
	
	if (type!=php3_mssql_module.le_result) {
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
		yystype_copy_constructor(&tmp);
		if (php3_ini.magic_quotes_runtime && tmp.type == IS_STRING) {
			tmp.value.str.val = _php3_addslashes(tmp.value.str.val,tmp.value.str.len,&tmp.value.str.len,1);
		}
		_php3_hash_index_update(return_value->value.ht, i, (void *) &tmp, sizeof(pval), (void **) &yystype_ptr);
		_php3_hash_pointer_update(return_value->value.ht, result->fields[i].name, strlen(result->fields[i].name)+1, yystype_ptr);
	}
	result->cur_row++;
}


DLEXPORT void php3_mssql_fetch_object(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
	}
}


DLEXPORT void php3_mssql_fetch_array(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mssql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

DLEXPORT void php3_mssql_data_seek(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id;
	mssql_result *result;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &mssql_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=php3_mssql_module.le_result) {
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


static char *php3_mssql_get_field_name(int type)
{
	switch (type) {
		case SQLBINARY:
		case SQLVARBINARY:
			return "blob";
			break;
		case SQLCHAR:
		case SQLVARCHAR:
		case SQLTEXT:
			return "string";
		case SQLDATETIME:
		case SQLDATETIM4:
		case SQLDATETIMN:
			return "datetime";
			break;
		case SQLDECIMAL:
		case SQLFLT8:
		case SQLFLTN:
		case SQLNUMERIC:
			return "real";
			break;
		case SQLINT1:
		case SQLINT2:
		case SQLINT4:
		case SQLINTN:
			return "int";
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


void php3_mssql_fetch_field(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;

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
	if (type!=php3_mssql_module.le_result) {
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

void php3_mssql_field_length(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;

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
	if (type!=php3_mssql_module.le_result) {
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

void php3_mssql_field_name(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;

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
	if (type!=php3_mssql_module.le_result) {
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

void php3_mssql_field_type(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;

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
	if (type!=php3_mssql_module.le_result) {
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

DLEXPORT void php3_mssql_field_seek(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mssql_result_index,*offset;
	int type,id,field_offset;
	mssql_result *result;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &mssql_result_index, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=php3_mssql_module.le_result) {
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


DLEXPORT void php3_mssql_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *row, *field, *mssql_result_index;
	int id,type,field_offset=0;
	mssql_result *result;
	

	if (ARG_COUNT(ht)!=3 || getParameters(ht, 3, &mssql_result_index, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(mssql_result_index);
	id = mssql_result_index->value.lval;
	
	result = (mssql_result *) php3_list_find(id,&type);
	if (type!=php3_mssql_module.le_result) {
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
	yystype_copy_constructor(return_value);
}


DLEXPORT void php3_info_mssql(void)
{
	char maxp[16],maxl[16];
	
	if (php3_mssql_module.max_persistent==-1) {
		strcpy(maxp,"Unlimited");
	} else {
		snprintf(maxp,15,"%ld",php3_mssql_module.max_persistent);
		maxp[15]=0;
	}
	if (php3_mssql_module.max_links==-1) {
		strcpy(maxl,"Unlimited");
	} else {
		snprintf(maxl,15,"%ld",php3_mssql_module.max_links);
		maxl[15]=0;
	}
	php3_printf("<table cellpadding=5>"
				"<tr><td>Allow persistent links:</td><td>%s</td></tr>\n"
				"<tr><td>Persistent links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Total links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Application name:</td><td>%s</td></tr>\n"
				"<tr><td valign=\"top\" width=\"20%%\">Client API information:</td><td><pre>%s</pre></td></tr>\n"
				"</table>\n",
				(php3_mssql_module.allow_persistent?"Yes":"No"),
				php3_mssql_module.num_persistent,maxp,
				php3_mssql_module.num_links,maxl,
				php3_mssql_module.appname,
				"MSSQL 6.5");
}


DLEXPORT void php3_mssql_min_error_severity(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *severity;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	php3_mssql_module.min_error_severity = severity->value.lval;
}


DLEXPORT void php3_mssql_min_message_severity(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *severity;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &severity)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(severity);
	php3_mssql_module.min_message_severity = severity->value.lval;
}

#endif
