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
   | Authors: Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */
 
/* $Id: msql.c,v 1.95 1998/06/22 20:28:33 zeev Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif

#ifndef MSVC5
#include "config.h"
#endif
#include "php.h"
#include "internal_functions.h"
#if COMPILE_DL
#include "dl/phpdl.h"
#include "functions/dl.h"
#endif
#include "php3_msql.h"
#include "functions/reg.h"
#include "php3_string.h"



#if HAVE_MSQL

#if defined(WIN32) && defined(MSQL1)
#include <msql1.h>
#else
#include <msql.h>
#endif
#include "php3_list.h"

#ifdef THREAD_SAFE
DWORD MSQLTls;
static int numthreads=0;

typedef struct msql_global_struct{
	msql_module php3_msql_module;
}msql_global_struct;

#define MSQL_GLOBAL(a) msql_globals->a

#define MSQL_TLS_VARS \
	msql_global_struct *msql_globals; \
	msql_globals=TlsGetValue(MSQLTls); 

#else
#define MSQL_GLOBAL(a) a
#define MSQL_TLS_VARS
msql_module php3_msql_module;
#endif

function_entry msql_functions[] = {
	{"msql_connect",		php3_msql_connect,			NULL},
	{"msql_pconnect",		php3_msql_pconnect,			NULL},
	{"msql_close",			php3_msql_close,			NULL},
	{"msql_select_db",		php3_msql_select_db,		NULL},
	{"msql_create_db",		php3_msql_create_db,		NULL},
	{"msql_drop_db",		php3_msql_drop_db,			NULL},
	{"msql_query",			php3_msql_query,			NULL},
	{"msql_db_query",		php3_msql_db_query,			NULL},
	{"msql_list_dbs",		php3_msql_list_dbs,			NULL},
	{"msql_list_tables",	php3_msql_list_tables,		NULL},
	{"msql_list_fields",	php3_msql_list_fields,		NULL},
	{"msql_error",			php3_msql_error,			NULL},
	{"msql_result",			php3_msql_result,			NULL},
	{"msql_num_rows",		php3_msql_num_rows,			NULL},
	{"msql_num_fields",		php3_msql_num_fields,		NULL},
	{"msql_fetch_row",		php3_msql_fetch_row,		NULL},
	{"msql_fetch_array",	php3_msql_fetch_array,		NULL},
	{"msql_fetch_object",	php3_msql_fetch_object,		NULL},
	{"msql_data_seek",		php3_msql_data_seek,		NULL},
	{"msql_fetch_field",	php3_msql_fetch_field,		NULL},
	{"msql_field_seek",		php3_msql_field_seek,		NULL},
	{"msql_free_result",	php3_msql_free_result,		NULL},
	{"msql_fieldname",		php3_msql_field_name,		NULL},
	{"msql_fieldtable",		php3_msql_field_table,		NULL},
	{"msql_fieldlen",		php3_msql_field_len,		NULL},
	{"msql_fieldtype",		php3_msql_field_type,		NULL},
	{"msql_fieldflags",		php3_msql_field_flags,		NULL},
	{"msql_regcase",		php3_sql_regcase,			NULL},
	/* for downwards compatability */
	{"msql",				php3_msql_db_query,			NULL},
	{"msql_selectdb",		php3_msql_select_db,		NULL},
	{"msql_createdb",		php3_msql_create_db,		NULL},
	{"msql_dropdb",			php3_msql_drop_db,			NULL},
	{"msql_freeresult",		php3_msql_free_result,		NULL},
	{"msql_numfields",		php3_msql_num_fields,		NULL},
	{"msql_numrows",		php3_msql_num_rows,			NULL},
	{"msql_listdbs",		php3_msql_list_dbs,			NULL},
	{"msql_listtables",		php3_msql_list_tables,		NULL},
	{"msql_listfields",		php3_msql_list_fields,		NULL},
	{"msql_dbname",			php3_msql_result,			NULL},
	{"msql_tablename",		php3_msql_result,			NULL},
	{NULL, NULL, NULL}
};


php3_module_entry msql_module_entry = {
	"mSQL", msql_functions, php3_minit_msql, php3_mshutdown_msql, php3_rinit_msql, NULL, php3_info_msql, STANDARD_MODULE_PROPERTIES
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &msql_module_entry; }
#if (WIN32|WINNT) && defined(THREAD_SAFE)

/*NOTE: You should have an odbc.def file where you
export DllMain*/
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
		if ((MSQLTls=TlsAlloc())==0xFFFFFFFF){
			return 0;
		}
		break;    
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (!TlsFree(MSQLTls)){
			return 0;
		}
		break;
    }
    return 1;
}
#endif
#endif


static void _close_msql_link(int link)
{
	MSQL_TLS_VARS;
	msqlClose(link);
	MSQL_GLOBAL(php3_msql_module).num_links--;
}


static void _close_msql_plink(int link)
{
	MSQL_TLS_VARS;
	msqlClose(link);
	MSQL_GLOBAL(php3_msql_module).num_persistent--;
	MSQL_GLOBAL(php3_msql_module).num_links--;
}


DLEXPORT int php3_minit_msql(INIT_FUNC_ARGS)
{
#ifdef THREAD_SAFE
	msql_global_struct *msql_globals;
#if !COMPILE_DL
	CREATE_MUTEX(msql_mutex,"MSQL_TLS");
	SET_MUTEX(msql_mutex);
	numthreads++;
	if (numthreads==1){
	if ((MSQLTls=TlsAlloc())==0xFFFFFFFF){
		FREE_MUTEX(msql_mutex);
		return 0;
	}}
	FREE_MUTEX(msql_mutex);
#endif
	msql_globals = (msql_global_struct *) LocalAlloc(LPTR, sizeof(msql_global_struct)); 
	TlsSetValue(MSQLTls, (void *) msql_globals);
#endif

	if (cfg_get_long("msql.allow_persistent",&MSQL_GLOBAL(php3_msql_module).allow_persistent)==FAILURE) {
		MSQL_GLOBAL(php3_msql_module).allow_persistent=1;
	}
	if (cfg_get_long("msql.max_persistent",&MSQL_GLOBAL(php3_msql_module).max_persistent)==FAILURE) {
		MSQL_GLOBAL(php3_msql_module).max_persistent=-1;
	}
	if (cfg_get_long("msql.max_links",&MSQL_GLOBAL(php3_msql_module).max_links)==FAILURE) {
		MSQL_GLOBAL(php3_msql_module).max_links=-1;
	}
	MSQL_GLOBAL(php3_msql_module).num_persistent=0;
	MSQL_GLOBAL(php3_msql_module).le_result = register_list_destructors(msqlFreeResult,NULL);
	MSQL_GLOBAL(php3_msql_module).le_link = register_list_destructors(_close_msql_link,NULL);
	MSQL_GLOBAL(php3_msql_module).le_plink = register_list_destructors(NULL,_close_msql_plink);
	
	msql_module_entry.type = type;

	return SUCCESS;
}

DLEXPORT int php3_mshutdown_msql(void){
#ifdef THREAD_SAFE
	msql_global_struct *msql_globals;
	msql_globals = TlsGetValue(MSQLTls); 
	if (msql_globals != 0) 
		LocalFree((HLOCAL) msql_globals); 
#if !COMPILE_DL
	SET_MUTEX(msql_mutex);
	numthreads--;
	if (!numthreads){
	if (!TlsFree(MSQLTls)){
		FREE_MUTEX(msql_mutex);
		return 0;
	}}
	FREE_MUTEX(msql_mutex);
#endif
#endif
	return SUCCESS;
}

DLEXPORT int php3_rinit_msql(INIT_FUNC_ARGS)
{
	MSQL_TLS_VARS;
	MSQL_GLOBAL(php3_msql_module).default_link=-1;
	MSQL_GLOBAL(php3_msql_module).num_links = MSQL_GLOBAL(php3_msql_module).num_persistent;
	return SUCCESS;
}

DLEXPORT void php3_info_msql(void)
{
	char maxp[16],maxl[16];
	MSQL_TLS_VARS;

	if (MSQL_GLOBAL(php3_msql_module).max_persistent==-1) {
		strcpy(maxp,"Unlimited");
	} else {
		snprintf(maxp,15,"%ld",MSQL_GLOBAL(php3_msql_module).max_persistent);
		maxp[15]=0;
	}
	if (MSQL_GLOBAL(php3_msql_module).max_links==-1) {
		strcpy(maxl,"Unlimited");
	} else {
		snprintf(maxl,15,"%ld",MSQL_GLOBAL(php3_msql_module).max_links);
		maxl[15]=0;
	}
	php3_printf("<table>"
				"<tr><td>Allow persistent links:</td><td>%s</td></tr>\n"
				"<tr><td>Persistent links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Total links:</td><td>%d/%s</td></tr>\n"
				"</table>\n",
				(MSQL_GLOBAL(php3_msql_module).allow_persistent?"Yes":"No"),
				MSQL_GLOBAL(php3_msql_module).num_persistent,maxp,
				MSQL_GLOBAL(php3_msql_module).num_links,maxl);
}


static void php3_msql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	char *host;
	char *hashed_details;
	int hashed_details_length;
	int msql;
	MSQL_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
		case 0: /* defaults */
			host=NULL;
			hashed_details=estrndup("msql_",5);
			hashed_details_length=4+1;
			break;
		case 1: {
				pval *yyhost;
			
				if (getParameters(ht, 1, &yyhost) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				host = yyhost->value.str.val;
				hashed_details_length = yyhost->value.str.len+4+1;
				hashed_details = emalloc(hashed_details_length+1);
				sprintf(hashed_details,"msql_%s",yyhost->value.str.val); /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (!MSQL_GLOBAL(php3_msql_module).allow_persistent) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;
		
		if (MSQL_GLOBAL(php3_msql_module).max_links!=-1 && MSQL_GLOBAL(php3_msql_module).num_links>=MSQL_GLOBAL(php3_msql_module).max_links) {
			php3_error(E_WARNING,"mSQL:  Too many open links (%d)",MSQL_GLOBAL(php3_msql_module).num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		if (MSQL_GLOBAL(php3_msql_module).max_persistent!=-1 && MSQL_GLOBAL(php3_msql_module).num_persistent>=MSQL_GLOBAL(php3_msql_module).max_persistent) {
			php3_error(E_WARNING,"mSQL:  Too many open persistent links (%d)",MSQL_GLOBAL(php3_msql_module).num_persistent);
			efree(hashed_details);
			RETURN_FALSE;
		}
		
		/* try to find if we already have this link in our persistent list */
		if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;
			
			/* create the link */
			if ((msql=msqlConnect(host))==-1) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			/* hash it up */
			new_le.type = MSQL_GLOBAL(php3_msql_module).le_plink;
			new_le.ptr = (void *) msql;
			if (_php3_hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			MSQL_GLOBAL(php3_msql_module).num_persistent++;
			MSQL_GLOBAL(php3_msql_module).num_links++;
		} else {  /* we do */
			if (le->type != MSQL_GLOBAL(php3_msql_module).le_plink) {
				efree(hashed_details);
				RETURN_FALSE;
			}
#if 0
			/* ensure that the link did not die */
			/* still have to find a way to do this nicely with mSQL */
			if (msql_stat(le->ptr)==NULL) { /* the link died */
				if (msql_connect(le->ptr,host,user,passwd)==NULL) {
					php3_error(E_WARNING,"mSQL link lost, unable to reconnect");
					_php3_hash_del(plist,hashed_details,hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
#endif
			msql = (int) le->ptr;
		}
		return_value->value.lval = php3_list_insert((void *)msql,MSQL_GLOBAL(php3_msql_module).le_plink);
		return_value->type = IS_LONG;
	} else {
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual msql link sits.
		 * if it doesn't, open a new msql link, add it to the resource list,
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
			if (ptr && (type==MSQL_GLOBAL(php3_msql_module).le_link || type==MSQL_GLOBAL(php3_msql_module).le_plink)) {
				return_value->value.lval = MSQL_GLOBAL(php3_msql_module).default_link = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				_php3_hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (MSQL_GLOBAL(php3_msql_module).max_links!=-1 && MSQL_GLOBAL(php3_msql_module).num_links>=MSQL_GLOBAL(php3_msql_module).max_links) {
			php3_error(E_WARNING,"mSQL:  Too many open links (%d)",MSQL_GLOBAL(php3_msql_module).num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		if ((msql=msqlConnect(host))==-1) {
			efree(hashed_details);
			RETURN_FALSE;
		}

		/* add it to the list */
		return_value->value.lval = php3_list_insert((void *)msql,MSQL_GLOBAL(php3_msql_module).le_link);
		return_value->type = IS_LONG;
		
		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (_php3_hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		MSQL_GLOBAL(php3_msql_module).num_links++;
	}
	efree(hashed_details);
	MSQL_GLOBAL(php3_msql_module).default_link=return_value->value.lval;
}


static int php3_msql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	MSQL_TLS_VARS;
	if (MSQL_GLOBAL(php3_msql_module).default_link==-1) { /* no link opened yet, implicitly open one */
		HashTable tmp;
		
		_php3_hash_init(&tmp,0,NULL,NULL,0);
		php3_msql_do_connect(&tmp,return_value,list,plist,0);
		_php3_hash_destroy(&tmp);
	}
	return MSQL_GLOBAL(php3_msql_module).default_link;
}


DLEXPORT void php3_msql_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_msql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

DLEXPORT void php3_msql_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_msql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}

DLEXPORT void php3_msql_close(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *msql_link;
	int id,type;
	int msql;
	MSQL_TLS_VARS;
	
	switch (ARG_COUNT(ht)) {
		case 0:
			id = MSQL_GLOBAL(php3_msql_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(msql_link);
			id = msql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	msql = (int) php3_list_find(id,&type);
	if (type!=MSQL_GLOBAL(php3_msql_module).le_link && type!=MSQL_GLOBAL(php3_msql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a mSQL link index",id);
		RETURN_FALSE;
	}
	
	php3_list_delete(id);
	RETURN_TRUE;
}
	
			
DLEXPORT void php3_msql_select_db(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*msql_link;
	int id,type;
	int msql;
	MSQL_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(msql_link);
			id = msql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	msql = (int) php3_list_find(id,&type);
	if (type!=MSQL_GLOBAL(php3_msql_module).le_link && type!=MSQL_GLOBAL(php3_msql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a mSQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	
	if (msqlSelectDB(msql,db->value.str.val)==-1) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

DLEXPORT void php3_msql_create_db(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*msql_link;
	int id,type;
	int msql;
	MSQL_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			id = php3_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &msql_link)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long(msql_link);
			id = msql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	msql = (int) php3_list_find(id,&type);
	if (type!=MSQL_GLOBAL(php3_msql_module).le_link && type!=MSQL_GLOBAL(php3_msql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a mSQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (msqlCreateDB(msql,db->value.str.val)<0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}


DLEXPORT void php3_msql_drop_db(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*msql_link;
	int id,type;
	int msql;
	MSQL_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			id = php3_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &msql_link)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long(msql_link);
			id = msql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	msql = (int) php3_list_find(id,&type);
	if (type!=MSQL_GLOBAL(php3_msql_module).le_link && type!=MSQL_GLOBAL(php3_msql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a mSQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (msqlDropDB(msql,db->value.str.val)<0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}


DLEXPORT void php3_msql_query(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *query,*msql_link;
	int id,type;
	int msql;
	m_result *msql_result;
	MSQL_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			id = MSQL_GLOBAL(php3_msql_module).default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &query, &msql_link)==FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long(msql_link);
			id = msql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	msql = (int) php3_list_find(id,&type);
	if (type!=MSQL_GLOBAL(php3_msql_module).le_link && type!=MSQL_GLOBAL(php3_msql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a mSQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	if (msqlQuery(msql,query->value.str.val)==-1) {
		RETURN_FALSE;
	}
	if ((msql_result=msqlStoreResult())==NULL) {
		/*
		php3_error(E_WARNING,"Unable to save mSQL query result");
		RETURN_FALSE;
		*/
		RETURN_TRUE;
	}
	return_value->value.lval = php3_list_insert(msql_result,MSQL_GLOBAL(php3_msql_module).le_result);
	return_value->type = IS_LONG;
}


DLEXPORT void php3_msql_db_query(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*query,*msql_link;
	int id,type;
	int msql;
	m_result *msql_result;
	MSQL_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &db, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 3:
			if (getParameters(ht, 3, &db, &query, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(msql_link);
			id = msql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	msql = (int) php3_list_find(id,&type);
	if (type!=MSQL_GLOBAL(php3_msql_module).le_link && type!=MSQL_GLOBAL(php3_msql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a mSQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (msqlSelectDB(msql,db->value.str.val)==-1) {
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	if (msqlQuery(msql,query->value.str.val)==-1) {
		RETURN_FALSE;
	}
	if ((msql_result=msqlStoreResult())==NULL) {
		/*
		php3_error(E_WARNING,"Unable to save mSQL query result");
		RETURN_FALSE;
		*/
		RETURN_TRUE;
	}
	return_value->value.lval = php3_list_insert(msql_result,MSQL_GLOBAL(php3_msql_module).le_result);
	return_value->type = IS_LONG;
}


DLEXPORT void php3_msql_list_dbs(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *msql_link;
	int id,type;
	int msql;
	m_result *msql_result;
	MSQL_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
		case 0:
			id = php3_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 1:
			if (getParameters(ht, 1, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(msql_link);
			id = msql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	msql = (int) php3_list_find(id,&type);
	if (type!=MSQL_GLOBAL(php3_msql_module).le_link && type!=MSQL_GLOBAL(php3_msql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a mSQL link index",id);
		RETURN_FALSE;
	}
	if ((msql_result=msqlListDBs(msql))==NULL) {
		php3_error(E_WARNING,"Unable to save mSQL query result");
		RETURN_FALSE;
	}
	return_value->value.lval = php3_list_insert(msql_result,MSQL_GLOBAL(php3_msql_module).le_result);
	return_value->type = IS_LONG;
}


DLEXPORT void php3_msql_list_tables(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*msql_link;
	int id,type;
	int msql;
	m_result *msql_result;
	MSQL_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(msql_link);
			id = msql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	msql = (int) php3_list_find(id,&type);
	if (type!=MSQL_GLOBAL(php3_msql_module).le_link && type!=MSQL_GLOBAL(php3_msql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a mSQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (msqlSelectDB(msql,db->value.str.val)==-1) {
		RETURN_FALSE;
	}
	if ((msql_result=msqlListTables(msql))==NULL) {
		php3_error(E_WARNING,"Unable to save mSQL query result");
		RETURN_FALSE;
	}
	return_value->value.lval = php3_list_insert(msql_result,MSQL_GLOBAL(php3_msql_module).le_result);
	return_value->type = IS_LONG;
}


DLEXPORT void php3_msql_list_fields(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*table,*msql_link;
	int id,type;
	int msql;
	m_result *msql_result;
	MSQL_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &db, &table)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_msql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 3:
			if (getParameters(ht, 3, &db, &table, &msql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(msql_link);
			id = msql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	msql = (int) php3_list_find(id,&type);
	if (type!=MSQL_GLOBAL(php3_msql_module).le_link && type!=MSQL_GLOBAL(php3_msql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a mSQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (msqlSelectDB(msql,db->value.str.val)==-1) {
		RETURN_FALSE;
	}
	convert_to_string(table);
	if ((msql_result=msqlListFields(msql,table->value.str.val))==NULL) {
		php3_error(E_WARNING,"Unable to save mSQL query result");
		RETURN_FALSE;
	}
	return_value->value.lval = php3_list_insert(msql_result,MSQL_GLOBAL(php3_msql_module).le_result);
	return_value->type = IS_LONG;
}


void php3_msql_error(INTERNAL_FUNCTION_PARAMETERS)
{
	if (ARG_COUNT(ht)) {
		WRONG_PARAM_COUNT;
	}
	RETURN_STRING(msqlErrMsg,1);
}


DLEXPORT void php3_msql_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result, *row, *field=NULL;
	m_result *msql_result;
	m_row sql_row;
	int type,field_offset=0;
	MSQL_TLS_VARS;
	
	
	switch (ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &result, &row)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 3:
			if (getParameters(ht, 3, &result, &row, &field)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_long(result);
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=msqlNumRows(msql_result)) {
		php3_error(E_WARNING,"Unable to jump to row %d on mSQL result index %d",row->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	msqlDataSeek(msql_result,row->value.lval);
	if ((sql_row=msqlFetchRow(msql_result))==NULL) { /* shouldn't happen? */
		RETURN_FALSE;
	}

	if (field) {
		switch(field->type) {
			case IS_STRING: {
					int i=0;
					m_field *tmp_field;
					char *table_name,*field_name,*tmp;
					
					if ((tmp=strchr(field->value.str.val,'.'))) {
						*tmp = 0;
						table_name = estrdup(field->value.str.val);
						field_name = estrdup(tmp+1);
					} else {
						table_name = NULL;
						field_name = estrndup(field->value.str.val,field->value.str.len);
					}
					msqlFieldSeek(msql_result,0);
					while ((tmp_field=msqlFetchField(msql_result))) {
						if ((!table_name || !strcasecmp(tmp_field->table,table_name)) && !strcasecmp(tmp_field->name,field_name)) {
							field_offset = i;
							break;
						}
						i++;
					}
					if (!tmp_field) { /* no match found */
						php3_error(E_WARNING,"%s%s%s not found in mSQL result index %d",
									(table_name?table_name:""), (table_name?".":""), field_name, result->value.lval);
						efree(field_name);
						if (table_name) {
							efree(table_name);
						}
						RETURN_FALSE;
					}
					efree(field_name);
					if (table_name) {
						efree(table_name);
					}
				}
				break;
			default:
				convert_to_long(field);
				field_offset = field->value.lval;
				if (field_offset<0 || field_offset>=msqlNumFields(msql_result)) {
					php3_error(E_WARNING,"Bad column offset specified");
					RETURN_FALSE;
				}
				break;
		}
	}
	
	if (sql_row[field_offset]) {
		if (php3_ini.magic_quotes_runtime) {
			return_value->value.str.val = _php3_addslashes(sql_row[field_offset],0,&return_value->value.str.len,0);
		} else {	
			return_value->value.str.len = (sql_row[field_offset]?strlen(sql_row[field_offset]):0);
			return_value->value.str.val = (char *) safe_estrndup(sql_row[field_offset],return_value->value.str.len);
		}
	} else {
		var_reset(return_value);
	}
	
	return_value->type = IS_STRING;
}


DLEXPORT void php3_msql_num_rows(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	m_result *msql_result;
	int type;
	MSQL_TLS_VARS;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	return_value->value.lval = msqlNumRows(msql_result);
	return_value->type = IS_LONG;
}


DLEXPORT void php3_msql_num_fields(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	m_result *msql_result;
	int type;
	MSQL_TLS_VARS;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	return_value->value.lval = msqlNumFields(msql_result);
	return_value->type = IS_LONG;
}


DLEXPORT void php3_msql_fetch_row(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	m_result *msql_result;
	m_row msql_row;
	int type;
	int num_fields;
	int i;
	MSQL_TLS_VARS;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	if ((msql_row=msqlFetchRow(msql_result))==NULL) {
		RETURN_FALSE;
	}
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	num_fields = msqlNumFields(msql_result);
	
	for (i=0; i<num_fields; i++) {
		if (msql_row[i]) {
			add_index_string(return_value, i, msql_row[i], 1);
		} else {
			add_index_stringl(return_value, i, empty_string, 0, 1);
		}
	}
}


static void php3_msql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	m_result *msql_result;
	m_row msql_row;
	m_field *msql_field;
	int type;
	int num_fields;
	int i;
	pval *yystype_ptr;
	MSQL_TLS_VARS;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	if ((msql_row=msqlFetchRow(msql_result))==NULL) {
		RETURN_FALSE;
	}

	num_fields = msqlNumFields(msql_result);
	
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	
	msqlFieldSeek(msql_result,0);
	for (msql_field=msqlFetchField(msql_result),i=0; msql_field; msql_field=msqlFetchField(msql_result),i++) {
		if (msql_row[i]) {
			if (php3_ini.magic_quotes_runtime) {
				add_get_index_string(return_value, i, _php3_addslashes(msql_row[i],0,NULL,0), (void **) &yystype_ptr, 0);
			} else {
				add_get_index_string(return_value, i, msql_row[i], (void **) &yystype_ptr, 1);
			}
		} else {
			add_get_index_stringl(return_value, i, empty_string, 0, (void **) &yystype_ptr, 1);
		}
		_php3_hash_pointer_update(return_value->value.ht, msql_field->name, strlen(msql_field->name)+1, yystype_ptr);
	}
}


DLEXPORT void php3_msql_fetch_object(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_msql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
	}
}


DLEXPORT void php3_msql_fetch_array(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_msql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}


DLEXPORT void php3_msql_data_seek(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result,*offset;
	m_result *msql_result;
	int type;
	MSQL_TLS_VARS;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	convert_to_long(offset);
	if (offset->value.lval<0 || offset->value.lval>=msqlNumRows(msql_result)) {
		php3_error(E_WARNING,"Offset %d is invalid for mSQL result index %d",offset->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	msqlDataSeek(msql_result,offset->value.lval);
	RETURN_TRUE;
}


static char *php3_msql_get_field_name(int field_type)
{
	switch (field_type) {
#if MSQL1
		case INT_TYPE:
			return "int";
			break;
		case CHAR_TYPE:
			return "char";
			break;
		case REAL_TYPE:
			return "real";
			break;
		case IDENT_TYPE:
			return "ident";
			break;
		case NULL_TYPE:
			return "null";
			break;
#else
		case INT_TYPE:
		case UINT_TYPE:
		case CHAR_TYPE:
		case TEXT_TYPE:
		case REAL_TYPE:
		case NULL_TYPE:
		case DATE_TYPE:
		case TIME_TYPE:
		case MONEY_TYPE:
			return msqlTypeNames[field_type];
			break;
#endif
		default:
			return "unknown";
			break;
	}
}


DLEXPORT void php3_msql_fetch_field(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result, *field=NULL;
	m_result *msql_result;
	m_field *msql_field;
	int type;
	MSQL_TLS_VARS;
	
	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &result)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		case 2:
			if (getParameters(ht, 2, &result, &field)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(field);
		default:
			WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	if (field) {
		if (field->value.lval<0 || field->value.lval>=msqlNumRows(msql_result)) {
			php3_error(E_NOTICE,"mSQL:  Bad field offset specified");
			RETURN_FALSE;
		}
		msqlFieldSeek(msql_result,field->value.lval);
	}
	if ((msql_field=msqlFetchField(msql_result))==NULL) {
		RETURN_FALSE;
	}
	if (object_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}

	add_property_string(return_value, "name",(msql_field->name?msql_field->name:empty_string), 1);
	add_property_string(return_value, "table",(msql_field->table?msql_field->table:empty_string), 1);
	add_property_long(return_value, "not_null",IS_NOT_NULL(msql_field->flags));
#if MSQL1
	add_property_long(return_value, "primary_key",(msql_field->flags&PRI_KEY_FLAG?1:0));
#else
	add_property_long(return_value, "unique",(msql_field->flags&UNIQUE_FLAG?1:0));
#endif

	add_property_string(return_value, "type",php3_msql_get_field_name(msql_field->type), 1);
}

DLEXPORT void php3_msql_field_seek(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result, *offset;
	m_result *msql_result;
	int type;
	MSQL_TLS_VARS;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	convert_to_long(offset);
	if (offset->value.lval<0 || offset->value.lval>=msqlNumFields(msql_result)) {
		php3_error(E_WARNING,"Field %d is invalid for mSQL result index %d",offset->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	msqlFieldSeek(msql_result,offset->value.lval);
	RETURN_TRUE;
}


#define PHP3_MSQL_FIELD_NAME 1
#define PHP3_MSQL_FIELD_TABLE 2
#define PHP3_MSQL_FIELD_LEN 3
#define PHP3_MSQL_FIELD_TYPE 4
#define PHP3_MSQL_FIELD_FLAGS 5
 
static void php3_msql_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	pval *result, *field;
	m_result *msql_result;
	m_field *msql_field;
	int type;
	MSQL_TLS_VARS;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	convert_to_long(field);
	if (field->value.lval<0 || field->value.lval>=msqlNumFields(msql_result)) {
		php3_error(E_WARNING,"Field %d is invalid for mSQL result index %d",field->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	msqlFieldSeek(msql_result,field->value.lval);
	if ((msql_field=msqlFetchField(msql_result))==NULL) {
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP3_MSQL_FIELD_NAME:
			return_value->value.str.len = strlen(msql_field->name);
			return_value->value.str.val = estrndup(msql_field->name,return_value->value.str.len);
			return_value->type = IS_STRING;
			break;
		case PHP3_MSQL_FIELD_TABLE:
			return_value->value.str.len = strlen(msql_field->table);
			return_value->value.str.val = estrndup(msql_field->table,return_value->value.str.len);
			return_value->type = IS_STRING;
			break;
		case PHP3_MSQL_FIELD_LEN:
			return_value->value.lval = msql_field->length;
			return_value->type = IS_LONG;
			break;
		case PHP3_MSQL_FIELD_TYPE:
			return_value->value.str.val = estrdup(php3_msql_get_field_name(msql_field->type));
			return_value->value.str.len = strlen(return_value->value.str.val);
			return_value->type = IS_STRING;
			break;
		case PHP3_MSQL_FIELD_FLAGS:
#if MSQL1
			if ((msql_field->flags&NOT_NULL_FLAG) && (msql_field->flags&PRI_KEY_FLAG)) {
				return_value->value.str.val = estrndup("primary key not null",20);
				return_value->value.str.len = 20;
				return_value->type = IS_STRING;
			} else if (msql_field->flags&NOT_NULL_FLAG) {
				return_value->value.str.val = estrndup("not null",8);
				return_value->value.str.len = 8;
				return_value->type = IS_STRING;
			} else if (msql_field->flags&PRI_KEY_FLAG) {
				return_value->value.str.val = estrndup("primary key",11);
				return_value->value.str.len = 11;
				return_value->type = IS_STRING;
			} else {
				var_reset(return_value);
			}
#else
			if ((msql_field->flags&NOT_NULL_FLAG) && (msql_field->flags&UNIQUE_FLAG)) {
				return_value->value.str.val = estrndup("unique not null",15);
				return_value->value.str.len = 15;
				return_value->type = IS_STRING;
			} else if (msql_field->flags&NOT_NULL_FLAG) {
				return_value->value.str.val = estrndup("not null",8);
				return_value->value.str.len = 8;
				return_value->type = IS_STRING;
			} else if (msql_field->flags&UNIQUE_FLAG) {
				return_value->value.str.val = estrndup("unique",6);
				return_value->value.str.len = 6;
				return_value->type = IS_STRING;
			} else {
				var_reset(return_value);
			}
#endif
			break;
		default:
			RETURN_FALSE;
	}
}


DLEXPORT void php3_msql_field_name(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MSQL_FIELD_NAME);
}

DLEXPORT void php3_msql_field_table(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MSQL_FIELD_TABLE);
}

DLEXPORT void php3_msql_field_len(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MSQL_FIELD_LEN);
}

DLEXPORT void php3_msql_field_type(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MSQL_FIELD_TYPE);
}

DLEXPORT void php3_msql_field_flags(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_msql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MSQL_FIELD_FLAGS);
}


DLEXPORT void php3_msql_free_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	m_result *msql_result;
	int type;
	MSQL_TLS_VARS;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	if (result->value.lval==0) {
		RETURN_FALSE;
	}
	msql_result = (m_result *) php3_list_find(result->value.lval,&type);
	
	if (type!=MSQL_GLOBAL(php3_msql_module).le_result) {
		php3_error(E_WARNING,"%d is not a mSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(result->value.lval);
	RETURN_TRUE;
}

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */

