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
 
/* $Id: mysql.c,v 1.160 1998/05/23 23:42:08 zeev Exp $ */


/* TODO:
 *
 * ? Safe mode implementation
 */

#if defined(COMPILE_DL)
#include "dl/phpdl.h"
#endif
#if defined(THREAD_SAFE)
#include "tls.h"
DWORD MySQLTls;
static int numthreads=0;
void *mysql_mutex;
#endif

#if WIN32|WINNT
#include <winsock.h>
#else
#include "config.h"
#include "build-defs.h"

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <netdb.h>
#include <netinet/in.h>
#endif

#include "php.h"
#include "internal_functions.h"
#include "php3_string.h"
#include "php3_mysql.h"

#if HAVE_MYSQL
#include <mysql.h>
#ifdef HAVE_MYSQL_REAL_CONNECT
#ifdef HAVE_ERRMSG_H
#include <errmsg.h>
#endif
#endif
#include "php3_list.h"

#define SAFE_STRING(s) ((s)?(s):"")

function_entry mysql_functions[] = {
	{"mysql_connect",		php3_mysql_connect,			NULL},
	{"mysql_pconnect",		php3_mysql_pconnect,		NULL},
	{"mysql_close",			php3_mysql_close,			NULL},
	{"mysql_select_db",		php3_mysql_select_db,		NULL},
	{"mysql_create_db",		php3_mysql_create_db,		NULL},
	{"mysql_drop_db",		php3_mysql_drop_db,			NULL},
	{"mysql_query",			php3_mysql_query,			NULL},
	{"mysql_db_query",		php3_mysql_db_query,		NULL},
	{"mysql_list_dbs",		php3_mysql_list_dbs,		NULL},
	{"mysql_list_tables",	php3_mysql_list_tables,		NULL},
	{"mysql_list_fields",	php3_mysql_list_fields,		NULL},
	{"mysql_error",			php3_mysql_error,			NULL},
#ifdef mysql_errno
	{"mysql_errno",			php3_mysql_errno,			NULL},
#endif
	{"mysql_affected_rows",	php3_mysql_affected_rows,	NULL},
	{"mysql_insert_id",		php3_mysql_insert_id,		NULL},
	{"mysql_result",		php3_mysql_result,			NULL},
	{"mysql_num_rows",		php3_mysql_num_rows,		NULL},
	{"mysql_num_fields",	php3_mysql_num_fields,		NULL},
	{"mysql_fetch_row",		php3_mysql_fetch_row,		NULL},
	{"mysql_fetch_array",	php3_mysql_fetch_array,		NULL},
	{"mysql_fetch_object",	php3_mysql_fetch_object,	NULL},
	{"mysql_data_seek",		php3_mysql_data_seek,		NULL},
	{"mysql_fetch_lengths",	php3_mysql_fetch_lengths,	NULL},
	{"mysql_fetch_field",	php3_mysql_fetch_field,		NULL},
	{"mysql_field_seek",	php3_mysql_field_seek,		NULL},
	{"mysql_free_result",	php3_mysql_free_result,		NULL},
	{"mysql_field_name",	php3_mysql_field_name,		NULL},
	{"mysql_field_table",	php3_mysql_field_table,		NULL},
	{"mysql_field_len",		php3_mysql_field_len,		NULL},
	{"mysql_field_type",	php3_mysql_field_type,		NULL},
	{"mysql_field_flags",	php3_mysql_field_flags,		NULL}, 
	/* for downwards compatability */
	{"mysql",				php3_mysql_db_query,		NULL},
	{"mysql_fieldname",		php3_mysql_field_name,		NULL},
	{"mysql_fieldtable",	php3_mysql_field_table,		NULL},
	{"mysql_fieldlen",		php3_mysql_field_len,		NULL},
	{"mysql_fieldtype",		php3_mysql_field_type,		NULL},
	{"mysql_fieldflags",	php3_mysql_field_flags,		NULL},
	{"mysql_selectdb",		php3_mysql_select_db,		NULL},
	{"mysql_createdb",		php3_mysql_create_db,		NULL},
	{"mysql_dropdb",		php3_mysql_drop_db,			NULL},
	{"mysql_freeresult",	php3_mysql_free_result,		NULL},
	{"mysql_numfields",		php3_mysql_num_fields,		NULL},
	{"mysql_numrows",		php3_mysql_num_rows,		NULL},
	{"mysql_listdbs",		php3_mysql_list_dbs,		NULL},
	{"mysql_listtables",	php3_mysql_list_tables,		NULL},
	{"mysql_listfields",	php3_mysql_list_fields,		NULL},
	{"mysql_dbname",		php3_mysql_result,			NULL},
	{"mysql_tablename",		php3_mysql_result,			NULL},
	{NULL, NULL, NULL}
};

php3_module_entry mysql_module_entry = {
	"MySQL", mysql_functions, php3_minit_mysql, php3_mshutdown_mysql, php3_rinit_mysql, NULL, php3_info_mysql, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &mysql_module_entry; }
#if 0
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    return 1;
}
#endif
#endif

#if APACHE
extern void timeout(int sig);
#endif

#if defined(THREAD_SAFE)
typedef struct mysql_global_struct{
	mysql_module php3_mysql_module;
}mysql_global_struct;

#define MySQL_GLOBAL(a) mysql_globals->a

#define MySQL_TLS_VARS \
	mysql_global_struct *mysql_globals; \
	mysql_globals=TlsGetValue(MySQLTls); 

#else
#define MySQL_GLOBAL(a) a
#define MySQL_TLS_VARS
mysql_module php3_mysql_module;
#endif

#define CHECK_LINK(link) { if (link==-1) { php3_error(E_WARNING,"MySQL:  A link to the server could not be established"); RETURN_FALSE; } }

static void _close_mysql_link(MYSQL *link)
{
#if APACHE
	void (*handler) (int);
#endif
	MySQL_TLS_VARS;

#if APACHE
	handler = signal(SIGPIPE, SIG_IGN);
#endif

	mysql_close(link);

#if APACHE
	signal(SIGPIPE,handler);
#endif

	efree(link);
	MySQL_GLOBAL(php3_mysql_module).num_links--;
}

static void _close_mysql_plink(MYSQL *link)
{
#if APACHE
	void (*handler) (int);
#endif
	MySQL_TLS_VARS;

#if APACHE
	handler = signal(SIGPIPE, SIG_IGN);
#endif

	mysql_close(link);

#if APACHE
	signal(SIGPIPE,handler);
#endif

	free(link);
	MySQL_GLOBAL(php3_mysql_module).num_persistent--;
	MySQL_GLOBAL(php3_mysql_module).num_links--;
}

int php3_minit_mysql(INIT_FUNC_ARGS)
{
#if defined(THREAD_SAFE)
	mysql_global_struct *mysql_globals;
	CREATE_MUTEX(mysql_mutex,"MySQL_TLS");
	SET_MUTEX(mysql_mutex);
	numthreads++;
	if (numthreads==1){
	if ((MySQLTls=TlsAlloc())==0xFFFFFFFF){
		FREE_MUTEX(mysql_mutex);
		return 0;
	}}
	FREE_MUTEX(mysql_mutex);
	mysql_globals = (mysql_global_struct *) LocalAlloc(LPTR, sizeof(mysql_global_struct)); 
	TlsSetValue(MySQLTls, (void *) mysql_globals);
#endif

	if (cfg_get_long("mysql.allow_persistent",&MySQL_GLOBAL(php3_mysql_module).allow_persistent)==FAILURE) {
		MySQL_GLOBAL(php3_mysql_module).allow_persistent=1;
	}
	if (cfg_get_long("mysql.max_persistent",&MySQL_GLOBAL(php3_mysql_module).max_persistent)==FAILURE) {
		MySQL_GLOBAL(php3_mysql_module).max_persistent=-1;
	}
	if (cfg_get_long("mysql.max_links",&MySQL_GLOBAL(php3_mysql_module).max_links)==FAILURE) {
		MySQL_GLOBAL(php3_mysql_module).max_links=-1;
	}
	if (cfg_get_string("mysql.default_host",&MySQL_GLOBAL(php3_mysql_module).default_host)==FAILURE
		|| MySQL_GLOBAL(php3_mysql_module).default_host[0]==0) {
		MySQL_GLOBAL(php3_mysql_module).default_host=NULL;
	}
	if (cfg_get_string("mysql.default_user",&MySQL_GLOBAL(php3_mysql_module).default_user)==FAILURE
		|| MySQL_GLOBAL(php3_mysql_module).default_user[0]==0) {
		MySQL_GLOBAL(php3_mysql_module).default_user=NULL;
	}
	if (cfg_get_string("mysql.default_password",&MySQL_GLOBAL(php3_mysql_module).default_password)==FAILURE
		|| MySQL_GLOBAL(php3_mysql_module).default_password[0]==0) {	
		MySQL_GLOBAL(php3_mysql_module).default_password=NULL;
	}
	if (cfg_get_long("mysql.default_port",&MySQL_GLOBAL(php3_mysql_module).default_port)==FAILURE
		|| MySQL_GLOBAL(php3_mysql_module).default_port==0) {
#if !(WIN32|WINNT)
		struct servent *serv_ptr;
		char *env;
		
		MySQL_GLOBAL(php3_mysql_module).default_port = MYSQL_PORT;
		if ((serv_ptr = getservbyname("mysql", "tcp"))) {
			MySQL_GLOBAL(php3_mysql_module).default_port = (uint) ntohs((ushort) serv_ptr->s_port);
		}
		if ((env = getenv("MYSQL_TCP_PORT"))) {
			MySQL_GLOBAL(php3_mysql_module).default_port = (uint) atoi(env);
		}
#else
		MySQL_GLOBAL(php3_mysql_module).default_port = MYSQL_PORT;
#endif
	}
	MySQL_GLOBAL(php3_mysql_module).num_persistent=0;
	MySQL_GLOBAL(php3_mysql_module).le_result = register_list_destructors(mysql_free_result,NULL);
	MySQL_GLOBAL(php3_mysql_module).le_link = register_list_destructors(_close_mysql_link,NULL);
	MySQL_GLOBAL(php3_mysql_module).le_plink = register_list_destructors(NULL,_close_mysql_plink);

#if 0
	printf("Registered:  %d,%d,%d\n",MySQL_GLOBAL(php3_mysql_module).le_result,MySQL_GLOBAL(php3_mysql_module).le_link,MySQL_GLOBAL(php3_mysql_module).le_plink);
#endif
	mysql_module_entry.type = type;

	return SUCCESS;
}


int php3_mshutdown_mysql(void){
#if defined(THREAD_SAFE)
	mysql_global_struct *mysql_globals;
	mysql_globals = TlsGetValue(MySQLTls); 
	if (mysql_globals != 0) 
		LocalFree((HLOCAL) mysql_globals); 
	SET_MUTEX(mysql_mutex);
	numthreads--;
	if (!numthreads){
	if (!TlsFree(MySQLTls)){
		FREE_MUTEX(mysql_mutex);
		return 0;
	}}
	FREE_MUTEX(mysql_mutex);
#endif
	return SUCCESS;
}

int php3_rinit_mysql(INIT_FUNC_ARGS)
{
	MySQL_TLS_VARS;

	MySQL_GLOBAL(php3_mysql_module).default_link=-1;
	MySQL_GLOBAL(php3_mysql_module).num_links = MySQL_GLOBAL(php3_mysql_module).num_persistent;
	return SUCCESS;
}


void php3_info_mysql(void)
{
	char maxp[16],maxl[16];
	MySQL_TLS_VARS;

	if (MySQL_GLOBAL(php3_mysql_module).max_persistent==-1) {
		strcpy(maxp,"Unlimited");
	} else {
		snprintf(maxp,15,"%ld",MySQL_GLOBAL(php3_mysql_module).max_persistent);
		maxp[15]=0;
	}
	if (MySQL_GLOBAL(php3_mysql_module).max_links==-1) {
		strcpy(maxl,"Unlimited");
	} else {
		snprintf(maxl,15,"%ld",MySQL_GLOBAL(php3_mysql_module).max_links);
		maxl[15]=0;
	}
	php3_printf("<table cellpadding=5>"
				"<tr><td>Allow persistent links:</td><td>%s</td></tr>\n"
				"<tr><td>Persistent links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Total links:</td><td>%d/%s</td></tr>\n"
				"<tr><td>Client API version:</td><td>%s</td></tr>\n"
#if !(WIN32|WINNT)
				"<tr><td valign=\"top\">Compilation definitions:</td><td>"
				"<tt>MYSQL_INCLUDE=%s<br>\n"
		        "MYSQL_LFLAGS=%s<br>\n"
		        "MYSQL_LIBS=%s<br></tt></td></tr>"
#endif
				"</table>\n",
				(MySQL_GLOBAL(php3_mysql_module).allow_persistent?"Yes":"No"),
				MySQL_GLOBAL(php3_mysql_module).num_persistent,maxp,
				MySQL_GLOBAL(php3_mysql_module).num_links,maxl,
				mysql_get_client_info()
#if !(WIN32|WINNT)
				,PHP_MYSQL_INCLUDE,PHP_MYSQL_LFLAGS,PHP_MYSQL_LIBS
#endif
				);
}


static void php3_mysql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
#if APACHE
	void (*handler) (int);
#endif
	char *user,*passwd,*host,*tmp;
	char *hashed_details;
	int hashed_details_length;
	MYSQL *mysql;
	MySQL_TLS_VARS;

	if (php3_ini.sql_safe_mode) {
		if (ARG_COUNT(ht)>0) {
			php3_error(E_NOTICE,"SQL safe mode in effect - ignoring host/user/password information");
		}
		host=passwd=NULL;
		user=_php3_get_current_user();
		hashed_details_length = strlen(user)+5+3;
		hashed_details = (char *) emalloc(hashed_details_length+1);
		sprintf(hashed_details,"mysql__%s_",user);
	} else {
		host = MySQL_GLOBAL(php3_mysql_module).default_host;
		user = MySQL_GLOBAL(php3_mysql_module).default_user;
		passwd = MySQL_GLOBAL(php3_mysql_module).default_password;
		
		switch(ARG_COUNT(ht)) {
			case 0: /* defaults */
				break;
			case 1: {
					pval *yyhost;
					
					if (getParameters(ht, 1, &yyhost)==FAILURE) {
						RETURN_FALSE;
					}
					convert_to_string(yyhost);
					host = yyhost->value.str.val;
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
		hashed_details_length = sizeof("mysql___")-1 + strlen(SAFE_STRING(host))+strlen(SAFE_STRING(user))+strlen(SAFE_STRING(passwd));
		hashed_details = (char *) emalloc(hashed_details_length+1);
		sprintf(hashed_details,"mysql_%s_%s_%s",SAFE_STRING(host), SAFE_STRING(user), SAFE_STRING(passwd));
	}



	if (host && (tmp=strchr(host,':'))) {
		*tmp=0;
		tmp++;
		mysql_port = atoi(tmp);
	} else {
		mysql_port = MySQL_GLOBAL(php3_mysql_module).default_port;
	}
	
	if (!MySQL_GLOBAL(php3_mysql_module).allow_persistent) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;
		
		/* try to find if we already have this link in our persistent list */
		if (hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;

			if (MySQL_GLOBAL(php3_mysql_module).max_links!=-1 && MySQL_GLOBAL(php3_mysql_module).num_links>=MySQL_GLOBAL(php3_mysql_module).max_links) {
				php3_error(E_WARNING,"MySQL:  Too many open links (%d)",MySQL_GLOBAL(php3_mysql_module).num_links);
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (MySQL_GLOBAL(php3_mysql_module).max_persistent!=-1 && MySQL_GLOBAL(php3_mysql_module).num_persistent>=MySQL_GLOBAL(php3_mysql_module).max_persistent) {
				php3_error(E_WARNING,"MySQL:  Too many open persistent links (%d)",MySQL_GLOBAL(php3_mysql_module).num_persistent);
				efree(hashed_details);
				RETURN_FALSE;
			}
			/* create the link */
			mysql = (MYSQL *) malloc(sizeof(MYSQL));
			if (mysql_connect(mysql,host,user,passwd)==NULL) {
				php3_error(E_WARNING,mysql_error(mysql));
				free(mysql);
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			/* hash it up */
			new_le.type = MySQL_GLOBAL(php3_mysql_module).le_plink;
			new_le.ptr = mysql;
			if (hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				free(mysql);
				efree(hashed_details);
				RETURN_FALSE;
			}
			MySQL_GLOBAL(php3_mysql_module).num_persistent++;
			MySQL_GLOBAL(php3_mysql_module).num_links++;
		} else {  /* we do */
			if (le->type != MySQL_GLOBAL(php3_mysql_module).le_plink) {
				RETURN_FALSE;
			}
			/* ensure that the link did not die */
#if APACHE
			handler=signal(SIGPIPE,SIG_IGN);
#endif
#if defined(mysql_errno) && defined(CR_SERVER_GONE_ERROR)
			mysql_stat(le->ptr);
			if (mysql_errno((MYSQL *)le->ptr) == CR_SERVER_GONE_ERROR) {
#else
			if (!strcasecmp(mysql_stat(le->ptr),"mysql server has gone away")) { /* the link died */
#endif
#if APACHE
				signal(SIGPIPE,handler);
#endif
				if (mysql_connect(le->ptr,host,user,passwd)==NULL) {
					php3_error(E_WARNING,"MySQL:  Link to server lost, unable to reconnect");
					hash_del(plist, hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
#if APACHE
			signal(SIGPIPE,handler);
#endif
			mysql = (MYSQL *) le->ptr;
		}
		return_value->value.lval = php3_list_insert(mysql,MySQL_GLOBAL(php3_mysql_module).le_plink);
		return_value->type = IS_LONG;
	} else { /* non persistent */
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual mysql link sits.
		 * if it doesn't, open a new mysql link, add it to the resource list,
		 * and add a pointer to it with hashed_details as the key.
		 */
		if (hash_find(list,hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
			int type,link;
			void *ptr;

			if (index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}
			link = (int) index_ptr->ptr;
			ptr = php3_list_find(link,&type);   /* check if the link is still there */
			if (ptr && (type==MySQL_GLOBAL(php3_mysql_module).le_link || type==MySQL_GLOBAL(php3_mysql_module).le_plink)) {
				return_value->value.lval = MySQL_GLOBAL(php3_mysql_module).default_link = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (MySQL_GLOBAL(php3_mysql_module).max_links!=-1 && MySQL_GLOBAL(php3_mysql_module).num_links>=MySQL_GLOBAL(php3_mysql_module).max_links) {
			php3_error(E_WARNING,"MySQL:  Too many open links (%d)",MySQL_GLOBAL(php3_mysql_module).num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		mysql = (MYSQL *) emalloc(sizeof(MYSQL));
		if (mysql_connect(mysql,host,user,passwd)==NULL) {
			php3_error(E_WARNING,mysql_error(mysql));
			efree(hashed_details);
			efree(mysql);
			RETURN_FALSE;
		}

		/* add it to the list */
		return_value->value.lval = php3_list_insert(mysql,MySQL_GLOBAL(php3_mysql_module).le_link);
		return_value->type = IS_LONG;
		
		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		MySQL_GLOBAL(php3_mysql_module).num_links++;
	}
	efree(hashed_details);
	MySQL_GLOBAL(php3_mysql_module).default_link=return_value->value.lval;
}


static int php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	MySQL_TLS_VARS;

	if (MySQL_GLOBAL(php3_mysql_module).default_link==-1) { /* no link opened yet, implicitly open one */
		HashTable tmp;
		
		hash_init(&tmp,0,NULL,NULL,0);
		php3_mysql_do_connect(&tmp,return_value,list,plist,0);
		hash_destroy(&tmp);
	}
	return MySQL_GLOBAL(php3_mysql_module).default_link;
}


void php3_mysql_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

void php3_mysql_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}

void php3_mysql_close(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mysql_link;
	int id,type;
	MYSQL *mysql;
	MySQL_TLS_VARS;

	
	switch (ARG_COUNT(ht)) {
		case 0:
			id = MySQL_GLOBAL(php3_mysql_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	php3_list_delete(id);
	RETURN_TRUE;
}
	
			
void php3_mysql_select_db(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*mysql_link;
	int id,type;
	MYSQL *mysql;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	
	if (mysql_select_db(mysql,db->value.str.val)!=0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

void php3_mysql_create_db(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*mysql_link;
	int id,type;
	MYSQL *mysql;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (mysql_create_db(mysql,db->value.str.val)==0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}


void php3_mysql_drop_db(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*mysql_link;
	int id,type;
	MYSQL *mysql;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (mysql_drop_db(mysql,db->value.str.val)==0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}


void php3_mysql_query(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *query,*mysql_link;
	int id,type;
	MYSQL *mysql;
	MYSQL_RES *mysql_result;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &query, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	if (mysql_query(mysql,query->value.str.val)!=0) {
		RETURN_FALSE;
	}
	if ((mysql_result=mysql_store_result(mysql))==NULL) {
		/*php3_error(E_WARNING,"Unable to save MySQL query result");
		RETURN_FALSE;
		*/
		RETURN_TRUE;
	}
	return_value->value.lval = php3_list_insert(mysql_result,MySQL_GLOBAL(php3_mysql_module).le_result);
	return_value->type = IS_LONG;
}


void php3_mysql_db_query(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*query,*mysql_link;
	int id,type;
	MYSQL *mysql;
	MYSQL_RES *mysql_result;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &db, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 3:
			if (getParameters(ht, 3, &db, &query, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (mysql_select_db(mysql,db->value.str.val)!=0) {
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	if (mysql_query(mysql,query->value.str.val)!=0) {
		RETURN_FALSE;
	}
	if ((mysql_result=mysql_store_result(mysql))==NULL) {
		/*
		php3_error(E_WARNING,"Unable to save MySQL query result");
		RETURN_FALSE;
		*/
		RETURN_TRUE;
	}
	return_value->value.lval = php3_list_insert(mysql_result,MySQL_GLOBAL(php3_mysql_module).le_result);
	return_value->type = IS_LONG;
}


void php3_mysql_list_dbs(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mysql_link;
	int id,type;
	MYSQL *mysql;
	MYSQL_RES *mysql_result;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 0:
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 1:
			if (getParameters(ht, 1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	if ((mysql_result=mysql_list_dbs(mysql,NULL))==NULL) {
		php3_error(E_WARNING,"Unable to save MySQL query result");
		RETURN_FALSE;
	}
	return_value->value.lval = php3_list_insert(mysql_result,MySQL_GLOBAL(php3_mysql_module).le_result);
	return_value->type = IS_LONG;
}


void php3_mysql_list_tables(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*mysql_link;
	int id,type;
	MYSQL *mysql;
	MYSQL_RES *mysql_result;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &db)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 2:
			if (getParameters(ht, 2, &db, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (mysql_select_db(mysql,db->value.str.val)!=0) {
		RETURN_FALSE;
	}
	if ((mysql_result=mysql_list_tables(mysql,NULL))==NULL) {
		php3_error(E_WARNING,"Unable to save MySQL query result");
		RETURN_FALSE;
	}
	return_value->value.lval = php3_list_insert(mysql_result,MySQL_GLOBAL(php3_mysql_module).le_result);
	return_value->type = IS_LONG;
}


void php3_mysql_list_fields(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *db,*table,*mysql_link;
	int id,type;
	MYSQL *mysql;
	MYSQL_RES *mysql_result;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &db, &table)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 3:
			if (getParameters(ht, 3, &db, &table, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	CHECK_LINK(id);
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(db);
	if (mysql_select_db(mysql,db->value.str.val)!=0) {
		RETURN_FALSE;
	}
	convert_to_string(table);
	if ((mysql_result=mysql_list_fields(mysql,table->value.str.val,NULL))==NULL) {
		php3_error(E_WARNING,"Unable to save MySQL query result");
		RETURN_FALSE;
	}
	return_value->value.lval = php3_list_insert(mysql_result,MySQL_GLOBAL(php3_mysql_module).le_result);
	return_value->type = IS_LONG;
}


void php3_mysql_error(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mysql_link;
	int id,type;
	MYSQL *mysql;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 0:
			id = MySQL_GLOBAL(php3_mysql_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (id==-1) {
		RETURN_FALSE;
	}
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	RETURN_STRING(mysql_error(mysql),1);
}


#ifdef mysql_errno
void php3_mysql_errno(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mysql_link;
	int id,type;
	MYSQL *mysql;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 0:
			id = MySQL_GLOBAL(php3_mysql_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (id==-1) {
		RETURN_FALSE;
	}
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	RETURN_LONG(mysql_errno(mysql));
}
#endif


void php3_mysql_affected_rows(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mysql_link;
	int id,type;
	MYSQL *mysql;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 0:
			id = MySQL_GLOBAL(php3_mysql_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	return_value->value.lval = mysql_affected_rows(mysql);
	return_value->type = IS_LONG;
}


void php3_mysql_insert_id(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mysql_link;
	int id,type;
	MYSQL *mysql;
	MySQL_TLS_VARS;

	
	switch(ARG_COUNT(ht)) {
		case 0:
			id = MySQL_GLOBAL(php3_mysql_module).default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(mysql_link);
			id = mysql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	mysql = (MYSQL *) php3_list_find(id,&type);
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_link && type!=MySQL_GLOBAL(php3_mysql_module).le_plink) {
		php3_error(E_WARNING,"%d is not a MySQL link index",id);
		RETURN_FALSE;
	}
	
	return_value->value.lval = mysql_insert_id(mysql);
	return_value->type = IS_LONG;
}


void php3_mysql_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result, *row, *field=NULL;
	MYSQL_RES *mysql_result;
	MYSQL_ROW sql_row;
	unsigned int *sql_row_lengths;
	int type,field_offset=0;
	MySQL_TLS_VARS;

	
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
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=(int)mysql_num_rows(mysql_result)) {
		php3_error(E_WARNING,"Unable to jump to row %d on MySQL result index %d",row->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	mysql_data_seek(mysql_result,row->value.lval);
	if ((sql_row=mysql_fetch_row(mysql_result))==NULL 
		|| (sql_row_lengths=mysql_fetch_lengths(mysql_result))==NULL) { /* shouldn't happen? */
		RETURN_FALSE;
	}
	
	if (field) {
		switch(field->type) {
			case IS_STRING: {
					int i=0;
					MYSQL_FIELD *tmp_field;
					char *table_name,*field_name,*tmp;

					if ((tmp=strchr(field->value.str.val,'.'))) {
						*tmp = 0;
						table_name = estrdup(field->value.str.val);
						field_name = estrdup(tmp+1);
					} else {
						table_name = NULL;
						field_name = estrndup(field->value.str.val,field->value.str.len);
					}
					mysql_field_seek(mysql_result,0);
					while ((tmp_field=mysql_fetch_field(mysql_result))) {
						if ((!table_name || !strcasecmp(tmp_field->table,table_name)) && !strcasecmp(tmp_field->name,field_name)) {
							field_offset = i;
							break;
						}
						i++;
					}
					if (!tmp_field) { /* no match found */
						php3_error(E_WARNING,"%s%s%s not found in MySQL result index %d",
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
				if (field_offset<0 || field_offset>=(int)mysql_num_fields(mysql_result)) {
					php3_error(E_WARNING,"Bad column offset specified");
					RETURN_FALSE;
				}
				break;
		}
	}

	if (sql_row[field_offset]) {
		if (php3_ini.magic_quotes_runtime) {
			return_value->value.str.val = _php3_addslashes(sql_row[field_offset],sql_row_lengths[field_offset],&return_value->value.str.len,0);
		} else {	
			return_value->value.str.len = sql_row_lengths[field_offset];
			return_value->value.str.val = (char *) safe_estrndup(sql_row[field_offset],return_value->value.str.len);
		}
	} else {
		return_value->value.str.val = undefined_variable_string;
		return_value->value.str.len=0;
		return_value->type = IS_STRING;
	}
	
	return_value->type = IS_STRING;
}


void php3_mysql_num_rows(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	MYSQL_RES *mysql_result;
	int type;
	MySQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	return_value->value.lval = mysql_num_rows(mysql_result);
	return_value->type = IS_LONG;
}


void php3_mysql_num_fields(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	MYSQL_RES *mysql_result;
	int type;
	MySQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	return_value->value.lval = mysql_num_fields(mysql_result);
	return_value->type = IS_LONG;
}


void php3_mysql_fetch_row(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	MYSQL_RES *mysql_result;
	MYSQL_ROW mysql_row;
	unsigned int *mysql_row_lengths;
	int type;
	int num_fields;
	int i;
	MySQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	if ((mysql_row=mysql_fetch_row(mysql_result))==NULL 
		|| (mysql_row_lengths=mysql_fetch_lengths(mysql_result))==NULL) {
		RETURN_FALSE;
	}
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	num_fields = mysql_num_fields(mysql_result);
	
	for (i=0; i<num_fields; i++) {
		if (mysql_row[i]) {
			if (php3_ini.magic_quotes_runtime) {
				int len;
				char *tmp=_php3_addslashes(mysql_row[i],mysql_row_lengths[i],&len,0);
				
				add_index_stringl(return_value, i, tmp, len, 0);
			} else {
				add_index_stringl(return_value, i, mysql_row[i], mysql_row_lengths[i], 1);
			}
		} else {
			/* NULL field, don't set it */
			/*add_index_stringl(return_value, i, empty_string, 0, 1);*/
		}
	}
}


static void php3_mysql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	MYSQL_RES *mysql_result;
	MYSQL_ROW mysql_row;
	MYSQL_FIELD *mysql_field;
	unsigned int *mysql_row_lengths;
	int type;
	int num_fields;
	int i;
	pval *yystype_ptr;
	MySQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	if ((mysql_row=mysql_fetch_row(mysql_result))==NULL 
		|| (mysql_row_lengths=mysql_fetch_lengths(mysql_result))==NULL) {
		RETURN_FALSE;
	}

	num_fields = mysql_num_fields(mysql_result);
	
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	
	mysql_field_seek(mysql_result,0);
	for (mysql_field=mysql_fetch_field(mysql_result),i=0; mysql_field; mysql_field=mysql_fetch_field(mysql_result),i++) {
		if (mysql_row[i]) {
			if (php3_ini.magic_quotes_runtime) {
				int len;
				char *tmp=_php3_addslashes(mysql_row[i],mysql_row_lengths[i],&len,0);
				
				add_get_index_stringl(return_value, i, tmp, len, (void **) &yystype_ptr, 0);
			} else {
				add_get_index_stringl(return_value, i, mysql_row[i], mysql_row_lengths[i], (void **) &yystype_ptr, 1);
			}
			hash_pointer_update(return_value->value.ht, mysql_field->name, strlen(mysql_field->name)+1, yystype_ptr);
		} else {
			/* NULL field, don't set it */
			/* add_get_index_stringl(return_value, i, empty_string, 0, (void **) &yystype_ptr); */
		}
	}
}


void php3_mysql_fetch_object(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
	}
}


void php3_mysql_fetch_array(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}


void php3_mysql_data_seek(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result,*offset;
	MYSQL_RES *mysql_result;
	int type;
	MySQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	convert_to_long(offset);
	if (offset->value.lval<0 || offset->value.lval>=(int)mysql_num_rows(mysql_result)) {
		php3_error(E_WARNING,"Offset %d is invalid for MySQL result index %d",offset->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	mysql_data_seek(mysql_result,offset->value.lval);
	RETURN_TRUE;
}


void php3_mysql_fetch_lengths(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	MYSQL_RES *mysql_result;
	int *lengths;
	int type;
	int num_fields;
	int i;
	MySQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	if ((lengths=mysql_fetch_lengths(mysql_result))==NULL) {
		RETURN_FALSE;
	}
	if (array_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}
	num_fields = mysql_num_fields(mysql_result);
	
	for (i=0; i<num_fields; i++) {
		add_index_long(return_value, i, lengths[i]);
	}
}


static char *php3_mysql_get_field_name(int field_type)
{
	switch(field_type) {
		case FIELD_TYPE_CHAR:
		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
			return "string";
			break;
		case FIELD_TYPE_SHORT:
		case FIELD_TYPE_LONG:
		case FIELD_TYPE_LONGLONG:
		case FIELD_TYPE_INT24:
			return "int";
			break;
		case FIELD_TYPE_FLOAT:
		case FIELD_TYPE_DOUBLE:
		case FIELD_TYPE_DECIMAL:
			return "real";
			break;
		case FIELD_TYPE_TIMESTAMP:
			return "timestamp";
			break;
		case FIELD_TYPE_DATE:
			return "date";
			break;
		case FIELD_TYPE_TIME:
			return "time";
			break;
		case FIELD_TYPE_DATETIME:
			return "datetime";
			break;
		case FIELD_TYPE_TINY_BLOB:
		case FIELD_TYPE_MEDIUM_BLOB:
		case FIELD_TYPE_LONG_BLOB:
		case FIELD_TYPE_BLOB:
			return "blob";
			break;
		case FIELD_TYPE_NULL:
			return "null";
			break;
		default:
			return "unknown";
			break;
	}
}


void php3_mysql_fetch_field(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result,*field=NULL;
	MYSQL_RES *mysql_result;
	MYSQL_FIELD *mysql_field;
	int type;
	MySQL_TLS_VARS;

	
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
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	if (field) {
		if (field->value.lval<0 || field->value.lval>=(int)mysql_num_fields(mysql_result)) {
			php3_error(E_WARNING,"MySQL:  Bad field offset");
			RETURN_FALSE;
		}
		mysql_field_seek(mysql_result,field->value.lval);
	}
	if ((mysql_field=mysql_fetch_field(mysql_result))==NULL) {
		RETURN_FALSE;
	}
	if (object_init(return_value)==FAILURE) {
		RETURN_FALSE;
	}

	add_property_string(return_value, "name",(mysql_field->name?mysql_field->name:empty_string), 1);
	add_property_string(return_value, "table",(mysql_field->table?mysql_field->table:empty_string), 1);
	add_property_string(return_value, "def",(mysql_field->def?mysql_field->def:empty_string), 1);
	add_property_long(return_value, "max_length",mysql_field->max_length);
	add_property_long(return_value, "not_null",IS_NOT_NULL(mysql_field->flags));
	add_property_long(return_value, "primary_key",IS_PRI_KEY(mysql_field->flags));
	add_property_long(return_value, "multiple_key",(mysql_field->flags&MULTIPLE_KEY_FLAG?1:0));
	add_property_long(return_value, "unique_key",(mysql_field->flags&UNIQUE_KEY_FLAG?1:0));
	add_property_long(return_value, "numeric",IS_NUM(mysql_field->type));
	add_property_long(return_value, "blob",IS_BLOB(mysql_field->flags));
	add_property_string(return_value, "type",php3_mysql_get_field_name(mysql_field->type), 1);
	add_property_long(return_value, "unsigned",(mysql_field->flags&UNSIGNED_FLAG?1:0));
	add_property_long(return_value, "zerofill",(mysql_field->flags&ZEROFILL_FLAG?1:0));
}


void php3_mysql_field_seek(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result, *offset;
	MYSQL_RES *mysql_result;
	int type;
	MySQL_TLS_VARS;

	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &offset)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	convert_to_long(offset);
	if (offset->value.lval<0 || offset->value.lval>=(int)mysql_num_fields(mysql_result)) {
		php3_error(E_WARNING,"Field %d is invalid for MySQL result index %d",offset->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	mysql_field_seek(mysql_result,offset->value.lval);
	RETURN_TRUE;
}


#define PHP3_MYSQL_FIELD_NAME 1
#define PHP3_MYSQL_FIELD_TABLE 2
#define PHP3_MYSQL_FIELD_LEN 3
#define PHP3_MYSQL_FIELD_TYPE 4
#define PHP3_MYSQL_FIELD_FLAGS 5
 
static void php3_mysql_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	pval *result, *field;
	MYSQL_RES *mysql_result;
	MYSQL_FIELD *mysql_field;
	int type;
	MySQL_TLS_VARS;

	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	convert_to_long(field);
	if (field->value.lval<0 || field->value.lval>=(int)mysql_num_fields(mysql_result)) {
		php3_error(E_WARNING,"Field %d is invalid for MySQL result index %d",field->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	mysql_field_seek(mysql_result,field->value.lval);
	if ((mysql_field=mysql_fetch_field(mysql_result))==NULL) {
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP3_MYSQL_FIELD_NAME:
			return_value->value.str.len = strlen(mysql_field->name);
			return_value->value.str.val = estrndup(mysql_field->name,return_value->value.str.len);
			return_value->type = IS_STRING;
			break;
		case PHP3_MYSQL_FIELD_TABLE:
			return_value->value.str.len = strlen(mysql_field->table);
			return_value->value.str.val = estrndup(mysql_field->table,return_value->value.str.len);
			return_value->type = IS_STRING;
			break;
		case PHP3_MYSQL_FIELD_LEN:
			return_value->value.lval = mysql_field->length;
			return_value->type = IS_LONG;
			break;
		case PHP3_MYSQL_FIELD_TYPE:
			return_value->value.str.val = php3_mysql_get_field_name(mysql_field->type);
			return_value->value.str.len = strlen(return_value->value.str.val);
			return_value->value.str.val = estrndup(return_value->value.str.val, return_value->value.str.len);
			return_value->type = IS_STRING;
			break;
		case PHP3_MYSQL_FIELD_FLAGS:
			if (IS_NOT_NULL(mysql_field->flags) && IS_PRI_KEY(mysql_field->flags)) {
				return_value->value.str.val = estrndup("primary key not null",20);
				return_value->value.str.len = 20;
				return_value->type = IS_STRING;
			} else if (IS_NOT_NULL(mysql_field->flags)) {
				return_value->value.str.val = estrndup("not null",8);
				return_value->value.str.len = 8;
				return_value->type = IS_STRING;
			} else if (IS_PRI_KEY(mysql_field->flags)) {
				return_value->value.str.val = estrndup("primary key",11);
				return_value->value.str.len = 11;
				return_value->type = IS_STRING;
			} else {
				var_reset(return_value);
			}
			break;
		default:
			RETURN_FALSE;
	}
}


void php3_mysql_field_name(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_NAME);
}

void php3_mysql_field_table(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_TABLE);
}

void php3_mysql_field_len(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_LEN);
}

void php3_mysql_field_type(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_TYPE);
}

void php3_mysql_field_flags(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_FLAGS);
}

void php3_mysql_free_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	MYSQL_RES *mysql_result;
	int type;
	MySQL_TLS_VARS;

	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	if (result->value.lval==0) {
		RETURN_FALSE;
	}
	
	mysql_result = (MYSQL_RES *) php3_list_find(result->value.lval,&type);
	
	if (type!=MySQL_GLOBAL(php3_mysql_module).le_result) {
		php3_error(E_WARNING,"%d is not a MySQL result index",result->value.lval);
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
