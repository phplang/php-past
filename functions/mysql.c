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
   | Authors: Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */
 
/* $Id: mysql.c,v 1.198 2000/09/30 17:32:44 sas Exp $ */


/* TODO:
 *
 * ? Safe mode implementation
 */

#define IS_EXT_MODULE

#if COMPILE_DL
# include "dl/phpdl.h"
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
# if HAVE_MYSQL_MYSQL_H
#  include <mysql/mysql.h>
# else
#  include <mysql.h>
# endif
#ifdef HAVE_MYSQL_REAL_CONNECT
#ifdef HAVE_ERRMSG_H
#include <errmsg.h>
#endif
#endif
#include "php3_list.h"

#define SAFE_STRING(s) ((s)?(s):"")

#if MYSQL_VERSION_ID > 32199
#define mysql_row_length_type unsigned long
#else
#define mysql_row_length_type unsigned int
#endif

#if defined(mysql_errno) || MYSQL_VERSION_ID > 32299
#define HAVE_MYSQL_ERRNO 1
#endif

#define MYSQL_ASSOC		1<<0
#define MYSQL_NUM		1<<1
#define MYSQL_BOTH		(MYSQL_ASSOC|MYSQL_NUM)

#if MYSQL_VERSION_ID < 32224
#define PHP_MYSQL_VALID_RESULT(mysql)       \
	(mysql_num_fields(mysql)>0)
#else
#define PHP_MYSQL_VALID_RESULT(mysql)       \
	(mysql_field_count(mysql)>0)
#endif

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
#ifdef HAVE_MYSQL_ERRNO
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
	{"mysql_tablename",		php3_mysql_result,			NULL},
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
	{"mysql_db_name",		php3_mysql_result,			NULL},
	{"mysql_dbname",		php3_mysql_result,			NULL},
#if MYSQL_VERSION_ID > 32302
	{"mysql_change_user",	php3_mysql_change_user,		NULL},
#endif

	{NULL, NULL, NULL}
};

php3_module_entry mysql_module_entry = {
	"MySQL", mysql_functions, php3_minit_mysql, php3_mshutdown_mysql, php3_rinit_mysql, NULL, php3_info_mysql, STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &mysql_module_entry; }
#endif

#if APACHE
extern void timeout(int sig);
#endif

#define MySQL_GLOBAL(a) a
#define MySQL_TLS_VARS
mysql_module php3_mysql_module;

#define CHECK_LINK(link) { if (link==-1) { php3_error(E_WARNING,"MySQL:  A link to the server could not be established"); RETURN_FALSE; } }

/* NOTE  Don't ask me why, but soon as I made this the list
 * destructor, I stoped getting access violations in windows
 * with mysql 3.22.7a
 */
static void _free_mysql_result(MYSQL_RES *mysql_result){
	mysql_free_result(mysql_result);
}

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
	MySQL_GLOBAL(php3_mysql_module).le_result = register_list_destructors(_free_mysql_result,NULL);
	MySQL_GLOBAL(php3_mysql_module).le_link = register_list_destructors(_close_mysql_link,NULL);
	MySQL_GLOBAL(php3_mysql_module).le_plink = register_list_destructors(NULL,_close_mysql_plink);

#if 0
	printf("Registered:  %d,%d,%d\n",MySQL_GLOBAL(php3_mysql_module).le_result,MySQL_GLOBAL(php3_mysql_module).le_link,MySQL_GLOBAL(php3_mysql_module).le_plink);
#endif
	mysql_module_entry.type = type;

	REGISTER_LONG_CONSTANT("MYSQL_ASSOC", MYSQL_ASSOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQL_NUM", MYSQL_NUM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MYSQL_BOTH", MYSQL_BOTH, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}


int php3_mshutdown_mysql(void){
	MySQL_TLS_VARS;
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
	char *user,*passwd,*host,*socket = NULL,*tmp;
	char *hashed_details;
	int hashed_details_length,port = 0;
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
		/* 
		   If we are asking for a persistent connection and have a version of
		   MySQL that supports the mysql_change_user() call, don't put any of the
		   connection-specific data into hashed_details.  Otherwise, use the old
		   mechanism.
		 */
		if(persistent) {
#if MYSQL_VERSION_ID > 32302
			hashed_details_length = sizeof("mysql___")-1;
			hashed_details = (char *) emalloc(hashed_details_length+1);
			strcpy(hashed_details,"mysql___");
#else
			hashed_details_length = sizeof("mysql___")-1 + strlen(SAFE_STRING(host))+strlen(SAFE_STRING(user))+strlen(SAFE_STRING(passwd));
			hashed_details = (char *) emalloc(hashed_details_length+1);
			sprintf(hashed_details,"mysql_%s_%s_%s",SAFE_STRING(host), SAFE_STRING(user), SAFE_STRING(passwd));
#endif
		} else {
			hashed_details_length = sizeof("mysql___")-1 + strlen(SAFE_STRING(host))+strlen(SAFE_STRING(user))+strlen(SAFE_STRING(passwd));
			hashed_details = (char *) emalloc(hashed_details_length+1);
			sprintf(hashed_details,"mysql_%s_%s_%s",SAFE_STRING(host), SAFE_STRING(user), SAFE_STRING(passwd));
		}
	}

	/* We cannot use mysql_port anymore in windows, need to use
	 * mysql_real_connect() to set the port.
	 */
	if (host && (tmp=strchr(host,':'))) {
		*tmp=0;
		tmp++;
		if (tmp[0] != '/') {
			port = atoi(tmp);
			if((tmp=strchr(tmp,':'))) {
				*tmp=0;
				tmp++;
				socket=tmp;
			}
		} else {
			socket = tmp;
		}
	} else {
		port = MySQL_GLOBAL(php3_mysql_module).default_port;
	}

#if MYSQL_VERSION_ID < 32200
	mysql_port = port;
#endif

	if (!MySQL_GLOBAL(php3_mysql_module).allow_persistent) {
		persistent=0;
	}
	if (persistent) {
		list_entry *le;
		
		/* try to find if we already have this link in our persistent list */
		if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
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
#if MYSQL_VERSION_ID > 32199 /* this lets us set the port number */
		mysql_init(mysql);
		if (mysql_real_connect(mysql,host,user,passwd,NULL,port,socket,0)==NULL) {
#else
		if (mysql_connect(mysql,host,user,passwd)==NULL) {
#endif
				php3_error(E_WARNING,"%s",mysql_error(mysql));
				free(mysql);
				efree(hashed_details);
				RETURN_FALSE;
			}
			
			/* hash it up */
			new_le.type = MySQL_GLOBAL(php3_mysql_module).le_plink;
			new_le.ptr = mysql;
			if (_php3_hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
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
#if defined(HAVE_MYSQL_ERRNO) && defined(CR_SERVER_GONE_ERROR)
			mysql_stat(le->ptr);
			if (mysql_errno((MYSQL *)le->ptr) == CR_SERVER_GONE_ERROR) {
#else
			if (!strcasecmp(mysql_stat(le->ptr),"mysql server has gone away")) { /* the link died */
#endif
#if APACHE
				signal(SIGPIPE,handler);
#endif
#if MYSQL_VERSION_ID > 32199 /* this lets us set the port number */
				if (mysql_real_connect(le->ptr,host,user,passwd,NULL,port,socket,0)==NULL) {
#else
				if (mysql_connect(le->ptr,host,user,passwd)==NULL) {
#endif
					php3_error(E_WARNING,"MySQL:  Link to server lost, unable to reconnect");
					_php3_hash_del(plist, hashed_details, hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			} 
#if MYSQL_VERSION_ID > 32302
			else {
				if(mysql_change_user((MYSQL *)le->ptr, user, passwd, NULL)!=0) {
					php3_error(E_WARNING,"%s",mysql_error((MYSQL *)le->ptr));
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
#endif
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
		if (_php3_hash_find(list,hashed_details,hashed_details_length+1,(void **) &index_ptr)==SUCCESS) {
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
				_php3_hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (MySQL_GLOBAL(php3_mysql_module).max_links!=-1 && MySQL_GLOBAL(php3_mysql_module).num_links>=MySQL_GLOBAL(php3_mysql_module).max_links) {
			php3_error(E_WARNING,"MySQL:  Too many open links (%d)",MySQL_GLOBAL(php3_mysql_module).num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}

		mysql = (MYSQL *) emalloc(sizeof(MYSQL));
#if MYSQL_VERSION_ID > 32199 /* this lets us set the port number */
		mysql_init(mysql);
		if (mysql_real_connect(mysql,host,user,passwd,NULL,port,socket,0)==NULL) {
#else
		if (mysql_connect(mysql,host,user,passwd)==NULL) {
#endif
			php3_error(E_WARNING,"MySQL Connection Failed: %s\n",mysql_error(mysql));
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
		if (_php3_hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
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
		
		_php3_hash_init(&tmp,0,NULL,NULL,0);
		php3_mysql_do_connect(&tmp,return_value,list,plist,0);
		_php3_hash_destroy(&tmp);
	}
	return MySQL_GLOBAL(php3_mysql_module).default_link;
}

/* {{{ proto int mysql_connect([string hostname[:port][:/path/to/socket]] [, string username] [, string password])
   Open a connection to a MySQL Server */
void php3_mysql_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
/* }}} */

/* {{{ proto int mysql_pconnect([string hostname[:port][:/path/to/socket]] [, string username] [, string password])
   Open a persistent connection to a MySQL Server */
void php3_mysql_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}
/* }}} */

/* {{{ proto int mysql_close([int link_identifier])
   Close a MySQL connection */
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
/* }}} */

/* {{{ proto int mysql_select_db(string database_name [, int link_identifier])
   Select a MySQL database */
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
/* }}} */

#if MYSQL_VERSION_ID > 32302
/* {{{ proto int mysql_change_user(string user, string password [, string database [, int link_identifier]])
   Changes the logged in user on the current connection */
void php3_mysql_change_user(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *user=NULL,*passwd=NULL,*db=NULL,*mysql_link=NULL;
	int id=0,type;
	MYSQL *mysql;
	MySQL_TLS_VARS;


	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &user, &passwd)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(user);
			convert_to_string(passwd);
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 3:
			if (getParameters(ht, 3, &user, &passwd, &db)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(user);
			convert_to_string(passwd);
			convert_to_string(db);
			id = php3_mysql_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
			break;
		case 4:
			if (getParameters(ht, 4, &user, &passwd, &db, &mysql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(user);
			convert_to_string(passwd);
			convert_to_string(db);
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
	if (mysql_change_user(mysql, 
		  user ? user->value.str.val:NULL, 
		  passwd ? passwd->value.str.val:NULL, 
		  db ? db->value.str.val:NULL) != 0) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} */
#endif

/* {{{ proto int mysql_create_db(string database_name [, int link_identifier])
   Create a MySQL database */
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
/* }}} */

/* {{{ proto int mysql_drop_db(string database_name [, int link_identifier])
   Drop (delete) a MySQL database */
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
/* }}} */

/* {{{ proto int mysql_query(string query [, int link_identifier])
   Send an SQL query to MySQL */
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
	/* mysql_query binary unsafe, use mysql_real_query */
#if MYSQL_VERSION_ID > 32199 
	if (mysql_real_query(mysql,query->value.str.val,query->value.str.len)!=0) {
		RETURN_FALSE;
	}
#else
	if (mysql_query(mysql,query->value.str.val)!=0) {
		RETURN_FALSE;
	}
#endif
	if ((mysql_result=mysql_store_result(mysql))==NULL) {
		if (PHP_MYSQL_VALID_RESULT(mysql)) { /* query should have returned rows */
			php3_error(E_WARNING, "MySQL:  Unable to save result set");
			RETURN_FALSE;
		} else {
			RETURN_TRUE;
		}
	}
	return_value->value.lval = php3_list_insert(mysql_result,MySQL_GLOBAL(php3_mysql_module).le_result);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int mysql_db_query(string database_name, string query [, int link_identifier])
   Send an SQL query to MySQL */
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
	/* mysql_query is binary unsafe, use mysql_real_query */
#if MYSQL_VERSION_ID > 32199 
	if (mysql_real_query(mysql,query->value.str.val,query->value.str.len)!=0) {
		RETURN_FALSE;
	}
#else
	if (mysql_query(mysql,query->value.str.val)!=0) {
		RETURN_FALSE;
	}
#endif
	if ((mysql_result=mysql_store_result(mysql))==NULL) {
		if (PHP_MYSQL_VALID_RESULT(mysql)) { /* query should have returned rows */
			php3_error(E_WARNING, "MySQL:  Unable to save result set");
			RETURN_FALSE;
		} else {
			RETURN_TRUE;
		}
	}
	return_value->value.lval = php3_list_insert(mysql_result,MySQL_GLOBAL(php3_mysql_module).le_result);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int mysql_list_dbs([int link_identifier])
   List databases available on a MySQL server */
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
/* }}} */

/* {{{ proto int mysql_list_tables(string database_name [, int link_identifier])
   List tables in a MySQL database */
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
/* }}} */

/* {{{ proto int mysql_list_fields(string database_name, string table_name [, int link_identifier])
   List MySQL result fields */
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
/* }}} */

/* {{{ proto string mysql_error([int link_identifier])
   Returns the text of the error message from previous MySQL operation */
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
/* }}} */

/* {{{ proto int mysql_errno([int link_identifier])
   Returns the number of the error message from previous MySQL operation */
#ifdef HAVE_MYSQL_ERRNO
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
/* }}} */

/* {{{ proto int mysql_affected_rows([int link_identifier])
   Get number of affected rows in previous MySQL operation */
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
	
	/* conversion from int64 to long happing here */
	return_value->value.lval = (long)mysql_affected_rows(mysql);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int mysql_insert_id([int link_identifier])
   Get the id generated from the previous INSERT operation */
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
	
	/* conversion from int64 to long happing here */
	return_value->value.lval = (long)mysql_insert_id(mysql);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int mysql_result(int result, int row [, mixed field])
   Get result data */
void php3_mysql_result(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result, *row, *field=NULL;
	MYSQL_RES *mysql_result;
	MYSQL_ROW sql_row;
	mysql_row_length_type *sql_row_lengths;
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
		} else 
		{	
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
/* }}} */

/* {{{ proto int mysql_num_rows(int result)
   Get number of rows in a result */
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
	
	/* conversion from int64 to long happing here */
	return_value->value.lval = (long)mysql_num_rows(mysql_result);
	return_value->type = IS_LONG;
}
/* }}} */

/* {{{ proto int mysql_num_fields(int result)
   Get number of fields in a result */
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
/* }}} */


static void php3_mysql_fetch_hash(INTERNAL_FUNCTION_PARAMETERS, int result_type)
{
	pval *result, *arg2;
	MYSQL_RES *mysql_result;
	MYSQL_ROW mysql_row;
	MYSQL_FIELD *mysql_field;
	mysql_row_length_type *mysql_row_lengths;
	int type;
	int num_fields;
	int i;
	MySQL_TLS_VARS;


	switch (ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &result)==FAILURE) {
				RETURN_FALSE;
			}
			if (!result_type) {
				result_type = MYSQL_BOTH;
			}
			break;
		case 2:
			if (getParameters(ht, 2, &result, &arg2)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg2);
			result_type = arg2->value.lval;
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
		char *data;
		int data_len;
		int should_copy;

		if (mysql_row[i]) {
			if (php3_ini.magic_quotes_runtime) {
				data=_php3_addslashes(mysql_row[i],mysql_row_lengths[i],&data_len,0);
				should_copy=0;
			} else {
				data = mysql_row[i];
				data_len = mysql_row_lengths[i];
				should_copy=1;
			}
			if (result_type & MYSQL_NUM) {
				add_index_stringl(return_value, i, data, data_len, should_copy);
				should_copy=1;
			}
			if (result_type & MYSQL_ASSOC) {
				add_assoc_stringl(return_value, mysql_field->name, data, data_len, should_copy);
			}
		} else {
			/* NULL field, don't set it */
			/* add_get_index_stringl(return_value, i, empty_string, 0, (void **) &pval_ptr); */
		}
	}
}


/* {{{ proto array mysql_fetch_row(int result)
   Get a result row as an enumerated array */
void php3_mysql_fetch_row(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, MYSQL_NUM);
}
/* }}} */


/* {{{ proto object mysql_fetch_object(int result [, int result_typ])
   Fetch a result row as an object */
void php3_mysql_fetch_object(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
	if (return_value->type==IS_ARRAY) {
		return_value->type=IS_OBJECT;
	}
}
/* }}} */


/* {{{ proto array mysql_fetch_array(int result [, int result_typ])
   Fetch a result row as an associative array */
void php3_mysql_fetch_array(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_fetch_hash(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int mysql_data_seek(int result, int row_number)
   Move internal result pointer */
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
/* }}} */

/* {{{ proto array mysql_fetch_lengths(int result)
   Get max data size of each column in a result */
void php3_mysql_fetch_lengths(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *result;
	MYSQL_RES *mysql_result;
	mysql_row_length_type *lengths;
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
/* }}} */

static char *php3_mysql_get_field_name(int field_type)
{
	switch(field_type) {
		case FIELD_TYPE_STRING:
		case FIELD_TYPE_VAR_STRING:
			return "string";
			break;
#ifdef FIELD_TYPE_TINY
		case FIELD_TYPE_TINY:
#endif
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

/* {{{ proto object mysql_fetch_field(int result [, int field_offset])
   Get column information from a result and return as an object */
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
	add_property_long(return_value, "not_null",IS_NOT_NULL(mysql_field->flags)?1:0);
	add_property_long(return_value, "primary_key",IS_PRI_KEY(mysql_field->flags)?1:0);
	add_property_long(return_value, "multiple_key",(mysql_field->flags&MULTIPLE_KEY_FLAG?1:0));
	add_property_long(return_value, "unique_key",(mysql_field->flags&UNIQUE_KEY_FLAG?1:0));
	add_property_long(return_value, "numeric",IS_NUM(mysql_field->type)?1:0);
	add_property_long(return_value, "blob",IS_BLOB(mysql_field->flags)?1:0);
	add_property_string(return_value, "type",php3_mysql_get_field_name(mysql_field->type), 1);
	add_property_long(return_value, "unsigned",(mysql_field->flags&UNSIGNED_FLAG?1:0));
	add_property_long(return_value, "zerofill",(mysql_field->flags&ZEROFILL_FLAG?1:0));
}
/* }}} */

/* {{{ proto int mysql_field_seek(int result, int field_offset)
   Set result pointer to a specific field offset */
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
/* }}} */

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
	char buf[512];
	int  len;
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
			strcpy(buf, "");
#ifdef IS_NOT_NULL
			if (IS_NOT_NULL(mysql_field->flags)) {
				strcat(buf, "not_null ");
			}
#endif
#ifdef IS_PRI_KEY
			if (IS_PRI_KEY(mysql_field->flags)) {
				strcat(buf, "primary_key ");
			}
#endif
#ifdef UNIQUE_KEY_FLAG
			if (mysql_field->flags&UNIQUE_KEY_FLAG) {
				strcat(buf, "unique_key ");
			}
#endif
#ifdef MULTIPLE_KEY_FLAG
			if (mysql_field->flags&MULTIPLE_KEY_FLAG) {
				strcat(buf, "multiple_key ");
			}
#endif
#ifdef IS_BLOB
			if (IS_BLOB(mysql_field->flags)) {
				strcat(buf, "blob ");
			}
#endif
#ifdef UNSIGNED_FLAG
			if (mysql_field->flags&UNSIGNED_FLAG) {
				strcat(buf, "unsigned ");
			}
#endif
#ifdef ZEROFILL_FLAG
			if (mysql_field->flags&ZEROFILL_FLAG) {
				strcat(buf, "zerofill ");
			}
#endif
#ifdef BINARY_FLAG
			if (mysql_field->flags&BINARY_FLAG) {
				strcat(buf, "binary ");
			}
#endif
#ifdef ENUM_FLAG
			if (mysql_field->flags&ENUM_FLAG) {
				strcat(buf, "enum ");
			}
#endif
#ifdef AUTO_INCREMENT_FLAG
			if (mysql_field->flags&AUTO_INCREMENT_FLAG) {
				strcat(buf, "auto_increment ");
			}
#endif
#ifdef TIMESTAMP_FLAG
			if (mysql_field->flags&TIMESTAMP_FLAG) {
				strcat(buf, "timestamp ");
			}
#endif
			len = strlen(buf);
			/* remove trailing space, if present */
			if (len && buf[len-1] == ' ') {
				buf[len-1] = 0;
				len--;
			}
			
	   		return_value->value.str.len = len;
   			return_value->value.str.val = estrndup(buf, len);
   			return_value->type = IS_STRING;
			break;

		default:
			RETURN_FALSE;
	}
}

/* {{{ proto string mysql_field_name(int result, int field_index)
   Get the name of the specified field in a result */
void php3_mysql_field_name(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_NAME);
}
/* }}} */

/* {{{ proto string mysql_field_table(int result, int field_offset)
   Get name of the table the specified field is in */
void php3_mysql_field_table(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_TABLE);
}
/* }}} */

/* {{{ proto int mysql_field_len(int result, int field_offset)
   Returns the length of the specified field */
void php3_mysql_field_len(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_LEN);
}
/* }}} */

/* {{{ proto string mysql_field_type(int result, int field_offset)
   Get the type of the specified field in a result */
void php3_mysql_field_type(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_TYPE);
}
/* }}} */

/* {{{ proto string mysql_field_flags(int result, int field_offset)
   Get the flags associated with the specified field in a result */
void php3_mysql_field_flags(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_mysql_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_MYSQL_FIELD_FLAGS);
}
/* }}} */

/* {{{ proto int mysql_free_result(int result)
   Free result memory */
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
/* }}} */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
