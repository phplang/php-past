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
   | Authors: Zeev Suraski <bourbon@netvision.net.il>                     |
   |          Jouni Ahto <jah@cultnet.fi> (large object interface)        |
   +----------------------------------------------------------------------+
 */
 
/* $Id: pgsql.c,v 1.46 1998/01/22 23:00:30 zeev Exp $ */


#ifndef MSVC5
#include "config.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "php3_pgsql.h"

#if HAVE_PGSQL

#include "list.h"


function_entry pgsql_functions[] = {
	{"pg_connect",		php3_pgsql_connect,			NULL},
	{"pg_pconnect",		php3_pgsql_pconnect,		NULL},
	{"pg_close",		php3_pgsql_close,			NULL},
	{"pg_dbname",		php3_pgsql_dbname,			NULL},
	{"pg_errormessage",	php3_pgsql_error_message,	NULL},
	{"pg_options",		php3_pgsql_options,			NULL},
	{"pg_port",			php3_pgsql_port,			NULL},
	{"pg_tty",			php3_pgsql_tty,				NULL},
	{"pg_host",			php3_pgsql_host,			NULL},
	{"pg_exec",			php3_pgsql_exec,			NULL},
	{"pg_numrows",		php3_pgsql_num_rows,		NULL},
	{"pg_numfields",	php3_pgsql_num_fields,		NULL},
	{"pg_fieldname",	php3_pgsql_field_name,		NULL},
	{"pg_fieldsize",	php3_pgsql_field_size,		NULL},
	{"pg_fieldtype",	php3_pgsql_field_type,		NULL},
	{"pg_fieldnum",		php3_pgsql_field_number,	NULL},
	{"pg_result",		php3_pgsql_result,			NULL},
	{"pg_fieldprtlen",	php3_pgsql_data_length,		NULL},
	{"pg_fieldisnull",	php3_pgsql_data_isnull,		NULL},
	{"pg_freeresult",	php3_pgsql_free_result,		NULL},
	{"pg_getlastoid",	php3_pgsql_last_oid,		NULL},
	{"pg_locreate",		php3_pgsql_lo_create,		NULL},
	{"pg_lounlink", 	php3_pgsql_lo_unlink,		NULL},
	{"pg_loopen",		php3_pgsql_lo_open,			NULL},
	{"pg_loclose",		php3_pgsql_lo_close,		NULL},
	{"pg_loread",		php3_pgsql_lo_read,			NULL},
	{"pg_lowrite",		php3_pgsql_lo_write,		NULL},
	{"pg_loreadall",	php3_pgsql_lo_readall,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry pgsql_module_entry = {
	"PostgresSQL", pgsql_functions, php3_minit_pgsql, NULL, php3_rinit_pgsql, NULL, NULL, 0, 0, 0, NULL
};

#if COMPILE_DL
php3_module_entry *get_module() { return &pgsql_module_entry; }
#endif

extern int php3_header(int, char *);

THREAD_LS pgsql_module php3_pgsql_module;

static void _close_pgsql_link(PGconn *link)
{
	PQfinish(link);
	php3_pgsql_module.num_links--;
}


static void _close_pgsql_plink(PGconn *link)
{
	PQfinish(link);
	php3_pgsql_module.num_persistent--;
	php3_pgsql_module.num_links--;
}


static void _free_ptr(pgLofp *lofp)
{
	efree(lofp);
}


int php3_minit_pgsql(INITFUNCARG)
{
	if (cfg_get_long("pgsql.allow_persistent",&php3_pgsql_module.allow_persistent)==FAILURE) {
		php3_pgsql_module.allow_persistent=1;
	}
	if (cfg_get_long("pgsql.max_persistent",&php3_pgsql_module.max_persistent)==FAILURE) {
		php3_pgsql_module.max_persistent=-1;
	}
	if (cfg_get_long("pgsql.max_links",&php3_pgsql_module.max_links)==FAILURE) {
		php3_pgsql_module.max_links=-1;
	}
	php3_pgsql_module.num_persistent=0;
	php3_pgsql_module.le_link = register_list_destructors(_close_pgsql_link,NULL);
	php3_pgsql_module.le_plink = register_list_destructors(NULL,_close_pgsql_plink);
	php3_pgsql_module.le_result = register_list_destructors(PQclear,NULL);
	php3_pgsql_module.le_lofp = register_list_destructors(_free_ptr,NULL);
	php3_pgsql_module.le_string = register_list_destructors(_free_ptr,NULL);
	return SUCCESS;
}


int php3_rinit_pgsql(INITFUNCARG)
{
	php3_pgsql_module.default_link=-1;
	php3_pgsql_module.num_links = php3_pgsql_module.num_persistent;
	return SUCCESS;
}


void php3_pgsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	char *host=NULL,*port=NULL,*options=NULL,*tty=NULL,*dbname=NULL,*connstring=NULL;
	char *hashed_details;
	int hashed_details_length;
	PGconn *pgsql;
	
	switch(ARG_COUNT(ht)) {
		case 1: { /* new style, using connection string */
				YYSTYPE *yyconnstring;
				if (getParameters(ht, 1, &yyconnstring) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyconnstring);
				connstring = yyconnstring->value.strval;
				hashed_details_length = yyconnstring->strlen+5+5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"pgsql_%s",connstring); /* SAFE */
			}
			break;
		case 3: { /* host, port, dbname */
				YYSTYPE *yyhost, *yyport, *yydbname;
				
				if (getParameters(ht, 3, &yyhost, &yyport, &yydbname) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyport);
				convert_to_string(yydbname);
				host = yyhost->value.strval;
				port = yyport->value.strval;
				dbname = yydbname->value.strval;
				options=tty=NULL;
				hashed_details_length = yyhost->strlen+yyport->strlen+yydbname->strlen+5+5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"pgsql_%s_%s___%s",host,port,dbname);  /* SAFE */
			}
			break;
		case 4: { /* host, port, options, dbname */
				YYSTYPE *yyhost, *yyport, *yyoptions, *yydbname;
				
				if (getParameters(ht, 4, &yyhost, &yyport, &yyoptions, &yydbname) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyport);
				convert_to_string(yyoptions);
				convert_to_string(yydbname);
				host = yyhost->value.strval;
				port = yyport->value.strval;
				options = yyoptions->value.strval;
				dbname = yydbname->value.strval;
				tty=NULL;
				hashed_details_length = yyhost->strlen+yyport->strlen+yyoptions->strlen+yydbname->strlen+5+5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"pgsql_%s_%s_%s__%s",host,port,options,dbname);  /* SAFE */
			}
			break;
		case 5: { /* host, port, options, tty, dbname */
				YYSTYPE *yyhost, *yyport, *yyoptions, *yytty, *yydbname;
				
				if (getParameters(ht, 5, &yyhost, &yyport, &yyoptions, &yytty, &yydbname) == FAILURE) {
					RETURN_FALSE;
				}
				convert_to_string(yyhost);
				convert_to_string(yyport);
				convert_to_string(yyoptions);
				convert_to_string(yytty);
				convert_to_string(yydbname);
				host = yyhost->value.strval;
				port = yyport->value.strval;
				options = yyoptions->value.strval;
				tty = yytty->value.strval;
				dbname = yydbname->value.strval;
				hashed_details_length = yyhost->strlen+yyport->strlen+yyoptions->strlen+yytty->strlen+yydbname->strlen+5+5;
				hashed_details = (char *) emalloc(hashed_details_length+1);
				sprintf(hashed_details,"pgsql_%s_%s_%s_%s_%s",host,port,options,tty,dbname);  /* SAFE */
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (persistent) {
		list_entry *le;
		
		/* try to find if we already have this link in our persistent list */
		if (hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le)==FAILURE) {  /* we don't */
			list_entry new_le;
			
			if (php3_pgsql_module.max_links!=-1 && php3_pgsql_module.num_links>=php3_pgsql_module.max_links) {
				php3_error(E_WARNING,"PostgresSQL:  Too many open links (%d)",php3_pgsql_module.num_links);
				efree(hashed_details);
				RETURN_FALSE;
			}
			if (php3_pgsql_module.max_persistent!=-1 && php3_pgsql_module.num_persistent>=php3_pgsql_module.max_persistent) {
				php3_error(E_WARNING,"PostgresSQL:  Too many open persistent links (%d)",php3_pgsql_module.num_persistent);
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* create the link */
			if (connstring) {
				pgsql=PQconnectdb(connstring);
			} else {
				pgsql=PQsetdb(host,port,options,tty,dbname);
			}
			if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
				php3_error(E_WARNING,"Unable to connect to PostgresSQL server:  %s",PQerrorMessage(pgsql));
				efree(hashed_details);
				RETURN_FALSE;
			}

			/* hash it up */
			new_le.type = php3_pgsql_module.le_plink;
			new_le.ptr = pgsql;
			if (hash_update(plist, hashed_details, hashed_details_length+1, (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
				efree(hashed_details);
				RETURN_FALSE;
			}
			php3_pgsql_module.num_links++;
			php3_pgsql_module.num_persistent++;
		} else {  /* we do */
			if (le->type != php3_pgsql_module.le_plink) {
				RETURN_FALSE;
			}
			/* ensure that the link did not die */
			if (PQstatus(le->ptr)==CONNECTION_BAD) { /* the link died */
				if (connstring) {
					le->ptr=PQconnectdb(connstring);
				} else {
					le->ptr=PQsetdb(host,port,options,tty,dbname);
				}
				if (le->ptr==NULL || PQstatus(le->ptr)==CONNECTION_BAD) {
					php3_error(E_WARNING,"PostgresSQL link lost, unable to reconnect");
					hash_del(plist,hashed_details,hashed_details_length+1);
					efree(hashed_details);
					RETURN_FALSE;
				}
			}
			pgsql = (PGconn *) le->ptr;
		}
		return_value->value.lval = php3_list_insert(pgsql,php3_pgsql_module.le_plink);
		return_value->type = IS_LONG;
	} else {
		list_entry *index_ptr,new_index_ptr;
		
		/* first we check the hash for the hashed_details key.  if it exists,
		 * it should point us to the right offset where the actual pgsql link sits.
		 * if it doesn't, open a new pgsql link, add it to the resource list,
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
			if (ptr && (type==php3_pgsql_module.le_link || type==php3_pgsql_module.le_plink)) {
				return_value->value.lval = php3_pgsql_module.default_link = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				hash_del(list,hashed_details,hashed_details_length+1);
			}
		}
		if (php3_pgsql_module.max_links!=-1 && php3_pgsql_module.num_links>=php3_pgsql_module.max_links) {
			php3_error(E_WARNING,"PostgresSQL:  Too many open links (%d)",php3_pgsql_module.num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		if (connstring) {
			pgsql=PQconnectdb(connstring);
		} else {
			pgsql=PQsetdb(host,port,options,tty,dbname);
		}
		if (pgsql==NULL || PQstatus(pgsql)==CONNECTION_BAD) {
			php3_error(E_WARNING,"Unable to connect to PostgresSQL server:  %s",PQerrorMessage(pgsql));
			efree(hashed_details);
			RETURN_FALSE;
		}

		/* add it to the list */
		return_value->value.lval = php3_list_insert(pgsql,php3_pgsql_module.le_link);
		return_value->type = IS_LONG;

		/* add it to the hash */
		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL)==FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		php3_pgsql_module.num_links++;
	}
	efree(hashed_details);
	php3_pgsql_module.default_link=return_value->value.lval;
}


int php3_pgsql_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if (php3_pgsql_module.default_link==-1) { /* no link opened yet, implicitly open one */
		HashTable tmp;
		
		hash_init(&tmp,0,NULL,NULL,0);
		php3_pgsql_do_connect(&tmp,return_value,list,plist,0);
		hash_destroy(&tmp);
	}
	return php3_pgsql_module.default_link;
}


void php3_pgsql_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

void php3_pgsql_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}


void php3_pgsql_close(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *pgsql_link;
	int id,type;
	PGconn *pgsql;
	
	switch (ARG_COUNT(ht)) {
		case 0:
			id = php3_pgsql_module.default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_link && type!=php3_pgsql_module.le_plink) {
		php3_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	php3_list_delete(pgsql_link->value.lval);
	RETURN_TRUE;
}
	

#define PHP3_PG_DBNAME 1
#define PHP3_PG_ERROR_MESSAGE 2
#define PHP3_PG_OPTIONS 3
#define PHP3_PG_PORT 4
#define PHP3_PG_TTY 5
#define PHP3_PG_HOST 6

void php3_pgsql_get_link_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	YYSTYPE *pgsql_link;
	int id,type;
	PGconn *pgsql;
	
	switch(ARG_COUNT(ht)) {
		case 0:
			id = php3_pgsql_module.default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_link && type!=php3_pgsql_module.le_plink) {
		php3_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	switch(entry_type) {
		case PHP3_PG_DBNAME:
			return_value->value.strval = PQdb(pgsql);
			break;
		case PHP3_PG_ERROR_MESSAGE:
			return_value->value.strval = PQerrorMessage(pgsql);
			break;
		case PHP3_PG_OPTIONS:
			return_value->value.strval = PQoptions(pgsql);
			break;
		case PHP3_PG_PORT:
			return_value->value.strval = PQport(pgsql);
			break;
		case PHP3_PG_TTY:
			return_value->value.strval = PQtty(pgsql);
			break;
		case PHP3_PG_HOST:
			return_value->value.strval = PQhost(pgsql);
			break;
		default:
			RETURN_FALSE;
	}
	return_value->strlen = strlen(return_value->value.strval);
	return_value->value.strval = (char *) estrdup(return_value->value.strval);
	return_value->type = IS_STRING;
}


void php3_pgsql_dbname(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_DBNAME);
}

void php3_pgsql_error_message(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_ERROR_MESSAGE);
}

void php3_pgsql_options(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_OPTIONS);
}

void php3_pgsql_port(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_PORT);
}

void php3_pgsql_tty(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_TTY);
}

void php3_pgsql_host(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_link_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_HOST);
}

void php3_pgsql_exec(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *query,*pgsql_link;
	int id,type;
	PGconn *pgsql;
	PGresult *pgsql_result;
	ExecStatusType  status;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &query)==FAILURE) {
				RETURN_FALSE;
			}
			id = php3_pgsql_module.default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &pgsql_link, &query)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_link && type!=php3_pgsql_module.le_plink) {
		php3_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	convert_to_string(query);
	pgsql_result=PQexec(pgsql,query->value.strval);
	
	if (pgsql_result) {
		status = PQresultStatus(pgsql_result);
	} else {
		status = (ExecStatusType) PQstatus(pgsql);
	}
	
	
	switch (status) {
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:
			php3_error(E_WARNING,"PostgresSQL query failed:  %s",PQerrorMessage(pgsql));
			RETURN_FALSE;
			break;
		case PGRES_COMMAND_OK: /* successful command that did not return rows */
		default:
			if (pgsql_result) {
				return_value->value.lval = php3_list_insert(pgsql_result,php3_pgsql_module.le_result);
				return_value->type = IS_LONG;
			} else {
				RETURN_FALSE;
			}
			break;
	}
}


#define PHP3_PG_NUM_ROWS 1
#define PHP3_PG_NUM_FIELDS 2

void php3_pgsql_get_result_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	YYSTYPE *result;
	PGresult *pgsql_result;
	int type;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pgsql_result = (PGresult *) php3_list_find(result->value.lval,&type);
	
	if (type!=php3_pgsql_module.le_result) {
		php3_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP3_PG_NUM_ROWS:
			return_value->value.lval = PQntuples(pgsql_result);
			break;
		case PHP3_PG_NUM_FIELDS:
			return_value->value.lval = PQnfields(pgsql_result);
			break;
		default:
			RETURN_FALSE;
	}
	return_value->type = IS_LONG;
}


void php3_pgsql_num_rows(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_NUM_ROWS);
}

void php3_pgsql_num_fields(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_result_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_NUM_FIELDS);
}


char *get_field_name(PGconn *pgsql, Oid oid, HashTable *list)
{
	PGresult *result;
	char hashed_oid_key[32];
	list_entry *field_type;
	char *ret=NULL;
	
	/* try to lookup the type in the resource list */
	snprintf(hashed_oid_key,31,"pgsql_oid_%d",(int) oid);
	hashed_oid_key[31]=0;

	if (hash_find(list,hashed_oid_key,strlen(hashed_oid_key)+1,(void **) &field_type)==SUCCESS) {
		ret = estrdup((char *)field_type->ptr);
	} else { /* hash all oid's */
		int i,num_rows;
		int oid_offset,name_offset;
		char *tmp_oid, *tmp_name;
		list_entry new_oid_entry;

		if ((result=PQexec(pgsql,"select oid,typname from pg_type"))==NULL) {
			return empty_string;
		}
		num_rows=PQntuples(result);
		oid_offset = PQfnumber(result,"oid");
		name_offset = PQfnumber(result,"typname");
		
		for (i=0; i<num_rows; i++) {
			if ((tmp_oid=PQgetvalue(result,i,oid_offset))==NULL) {
				continue;
			}
			snprintf(hashed_oid_key,31,"pgsql_oid_%s",tmp_oid);
			if ((tmp_name=PQgetvalue(result,i,name_offset))==NULL) {
				continue;
			}
			new_oid_entry.type = php3_pgsql_module.le_string;
			new_oid_entry.ptr = estrdup(tmp_name);
			hash_update(list,hashed_oid_key,strlen(hashed_oid_key)+1,(void *) &new_oid_entry, sizeof(list_entry), NULL);
			if (!ret && atoi(tmp_oid)==oid) {
				ret = estrdup(tmp_name);
			}
		}
	}
	return ret;
}
			

#define PHP3_PG_FIELD_NAME 1
#define PHP3_PG_FIELD_SIZE 2
#define PHP3_PG_FIELD_TYPE 3

void php3_pgsql_get_field_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	YYSTYPE *result,*field;
	PGresult *pgsql_result;
	int type;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pgsql_result = (PGresult *) php3_list_find(result->value.lval,&type);
	
	if (type!=php3_pgsql_module.le_result) {
		php3_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	convert_to_long(field);
	
	if (field->value.lval<0 || field->value.lval>=PQnfields(pgsql_result)) {
		php3_error(E_WARNING,"Bad field offset specified");
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP3_PG_FIELD_NAME:
			return_value->value.strval = PQfname(pgsql_result,field->value.lval);
			return_value->strlen = strlen(return_value->value.strval);
			return_value->value.strval = estrndup(return_value->value.strval,return_value->strlen);
			return_value->type = IS_STRING;
			break;
		case PHP3_PG_FIELD_SIZE:
			return_value->value.lval = PQfsize(pgsql_result,field->value.lval);
			return_value->type = IS_LONG;
			break;
		case PHP3_PG_FIELD_TYPE:
			return_value->value.strval = get_field_name(pgsql_result->conn,PQftype(pgsql_result,field->value.lval),list);
			return_value->strlen = strlen(return_value->value.strval);
			return_value->type = IS_STRING;
			break;
		default:
			RETURN_FALSE;
	}
}


void php3_pgsql_field_name(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_FIELD_NAME);
}

void php3_pgsql_field_size(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_FIELD_SIZE);
}

void php3_pgsql_field_type(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_get_field_info(INTERNAL_FUNCTION_PARAM_PASSTHRU,PHP3_PG_FIELD_TYPE);
}


void php3_pgsql_field_number(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *result,*field;
	PGresult *pgsql_result;
	int type;
	
	if (ARG_COUNT(ht)!=2 || getParameters(ht, 2, &result, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pgsql_result = (PGresult *) php3_list_find(result->value.lval,&type);
	
	if (type!=php3_pgsql_module.le_result) {
		php3_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	convert_to_string(field);
	return_value->value.lval = PQfnumber(pgsql_result,field->value.strval);
	return_value->type = IS_LONG;
}


void php3_pgsql_result(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *result, *row, *field=NULL;
	PGresult *pgsql_result;
	int type,field_offset;
	
	
	if (ARG_COUNT(ht)!=3 || getParameters(ht, 3, &result, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pgsql_result = (PGresult *) php3_list_find(result->value.lval,&type);
	
	if (type!=php3_pgsql_module.le_result) {
		php3_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=PQntuples(pgsql_result)) {
		php3_error(E_WARNING,"Unable to jump to row %d on PostgresSQL result index %d",row->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	switch(field->type) {
		case IS_STRING:
			field_offset = PQfnumber(pgsql_result,field->value.strval);
			break;
		default:
			convert_to_long(field);
			field_offset = field->value.lval;
			break;
	}
	if (field_offset<0 || field_offset>=PQnfields(pgsql_result)) {
		php3_error(E_WARNING,"Bad column offset specified");
		RETURN_FALSE;
	}
	
	return_value->value.strval = PQgetvalue(pgsql_result,row->value.lval,field_offset);
	return_value->strlen = (return_value->value.strval ? strlen(return_value->value.strval) : 0);
	return_value->value.strval = safe_estrndup(return_value->value.strval,return_value->strlen);
	return_value->type = IS_STRING;
}

#define PHP3_PG_DATA_LENGTH 1
#define PHP3_PG_DATA_ISNULL 2

void php3_pgsql_data_info(INTERNAL_FUNCTION_PARAMETERS, int entry_type)
{
	YYSTYPE *result,*row,*field;
	PGresult *pgsql_result;
	int type,field_offset;
	
	if (ARG_COUNT(ht)!=3 || getParameters(ht, 3, &result, &row, &field)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pgsql_result = (PGresult *) php3_list_find(result->value.lval,&type);
	
	if (type!=php3_pgsql_module.le_result) {
		php3_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	
	convert_to_long(row);
	if (row->value.lval<0 || row->value.lval>=PQntuples(pgsql_result)) {
		php3_error(E_WARNING,"Unable to jump to row %d on PostgresSQL result index %d",row->value.lval,result->value.lval);
		RETURN_FALSE;
	}
	switch(field->type) {
		case IS_STRING:
			field_offset = PQfnumber(pgsql_result,field->value.strval);
			break;
		default:
			convert_to_long(field);
			field_offset = field->value.lval;
			break;
	}
	if (field_offset<0 || field_offset>=PQnfields(pgsql_result)) {
		php3_error(E_WARNING,"Bad column offset specified");
		RETURN_FALSE;
	}
	
	switch (entry_type) {
		case PHP3_PG_DATA_LENGTH:
			return_value->value.lval = PQgetlength(pgsql_result,row->value.lval,field_offset);
			break;
		case PHP3_PG_DATA_ISNULL:
			return_value->value.lval = PQgetisnull(pgsql_result,row->value.lval,field_offset);
			break;
	}
	return_value->type = IS_LONG;
}

void php3_pgsql_data_length(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP3_PG_DATA_LENGTH);
}

void php3_pgsql_data_isnull(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_pgsql_data_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP3_PG_DATA_ISNULL);
}

void php3_pgsql_free_result(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *result;
	PGresult *pgsql_result;
	int type;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pgsql_result = (PGresult *) php3_list_find(result->value.lval,&type);
	
	if (type!=php3_pgsql_module.le_result) {
		php3_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	php3_list_delete(result->value.lval);
}


void php3_pgsql_last_oid(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *result;
	PGresult *pgsql_result;
	int type;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(result);
	pgsql_result = (PGresult *) php3_list_find(result->value.lval,&type);
	
	if (type!=php3_pgsql_module.le_result) {
		php3_error(E_WARNING,"%d is not a PostgresSQL result index",result->value.lval);
		RETURN_FALSE;
	}
	return_value->value.strval = (char *) PQoidStatus(pgsql_result);
	if (return_value->value.strval) {
		return_value->strlen = strlen(return_value->value.strval);
		return_value->value.strval = estrndup(return_value->value.strval, return_value->strlen);
		return_value->type = IS_STRING;
	} else {
		return_value->value.strval = empty_string;
	} 
}

void php3_pgsql_lo_create(INTERNAL_FUNCTION_PARAMETERS)
{
  	YYSTYPE *pgsql_link;
	PGconn *pgsql;
	Oid pgsql_oid;
	int id, type;
	
	switch(ARG_COUNT(ht)) {
		case 0:
			id = php3_pgsql_module.default_link;
			break;
		case 1:
			if (getParameters(ht, 1, &pgsql_link)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_link && type!=php3_pgsql_module.le_plink) {
		php3_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	/* XXX: Archive modes not supported until I get some more data. Don't think anybody's
	   using it anyway. I believe it's also somehow related to the 'time travel' feature of
	   PostgreSQL, that's on the list of features to be removed... Create modes not supported.
	   What's the use of an object that can be only written to, but not read from, and vice
	   versa? Beats me... And the access type (r/w) must be specified again when opening
	   the object, probably (?) overrides this. (Jouni) 
	 */

	if ((pgsql_oid=lo_creat(pgsql, INV_READ|INV_WRITE))==0) {
		php3_error(E_WARNING,"Unable to create PostgresSQL large object");
		RETURN_FALSE;
	}

	return_value->value.lval = pgsql_oid;
	return_value->type = IS_LONG;
}

void php3_pgsql_lo_unlink(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *pgsql_link, *oid;
	PGconn *pgsql;
	Oid pgsql_oid;
	int id, type;

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &oid)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(oid);
			pgsql_oid = oid->value.lval;
			id = php3_pgsql_module.default_link;
			break;
		case 2:
			if (getParameters(ht, 2, &pgsql_link, &oid)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			convert_to_long(oid);
			pgsql_oid = oid->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	pgsql = (PGconn *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_link && type!=php3_pgsql_module.le_plink) {
		php3_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	if (lo_unlink(pgsql, pgsql_oid)==-1) {
		php3_error(E_WARNING,"Unable to delete PostgresSQL large object %d", (int) pgsql_oid);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

void php3_pgsql_lo_open(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *pgsql_link, *oid, *mode;
	PGconn *pgsql;
	Oid pgsql_oid;
	int id, type, pgsql_mode=0, pgsql_lofd;
	int create=0;
	char *mode_string=NULL;
	pgLofp *pgsql_lofp;

	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &oid, &mode)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(oid);
			pgsql_oid = oid->value.lval;
			convert_to_string(mode);
			mode_string = mode->value.strval;
			id = php3_pgsql_module.default_link;
			break;
		case 3:
			if (getParameters(ht, 3, &pgsql_link, &oid, &mode)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_link);
			id = pgsql_link->value.lval;
			convert_to_long(oid);
			pgsql_oid = oid->value.lval;
			convert_to_string(mode);
			mode_string = mode->value.strval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (PGconn *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_link && type!=php3_pgsql_module.le_plink) {
		php3_error(E_WARNING,"%d is not a PostgresSQL link index",id);
		RETURN_FALSE;
	}
	
	/* r/w/+ is little bit more PHP-like than INV_READ/INV_WRITE and a lot of
	   faster to type. Unfortunately, doesn't behave the same way as fopen()...
	   (Jouni)
	 */

	if (strchr(mode_string, 'r')==mode_string) {
		pgsql_mode |= INV_READ;
		if (strchr(mode_string, '+')==mode_string+1) {
			pgsql_mode |= INV_WRITE;
		}
	}
	if (strchr(mode_string, 'w')==mode_string) {
		pgsql_mode |= INV_WRITE;
		create = 1;
		if (strchr(mode_string, '+')==mode_string+1) {
			pgsql_mode |= INV_READ;
		}
	}


	pgsql_lofp = (pgLofp *) emalloc(sizeof(pgLofp));

	if ((pgsql_lofd=lo_open(pgsql, pgsql_oid, pgsql_mode))==-1) {
		if (create) {
			if ((pgsql_oid=lo_creat(pgsql, INV_READ|INV_WRITE))==0) {
				efree(pgsql_lofp);
				php3_error(E_WARNING,"Unable to create PostgresSQL large object");
				RETURN_FALSE;
			} else {
				if ((pgsql_lofd=lo_open(pgsql, pgsql_oid, pgsql_mode))==-1) {
					if (lo_unlink(pgsql, pgsql_oid)==-1) {
						efree(pgsql_lofp);
						php3_error(E_WARNING,"Something's really messed up!!! Your database is badly corrupted in a way NOT related to PHP.");
						RETURN_FALSE;
					}
					efree(pgsql_lofp);
					php3_error(E_WARNING,"Unable to open PostgresSQL large object");
					RETURN_FALSE;
				} else {
					pgsql_lofp->conn = pgsql;
					pgsql_lofp->lofd = pgsql_lofd;
					return_value->value.lval = php3_list_insert(pgsql_lofp, php3_pgsql_module.le_lofp);
					return_value->type = IS_LONG;
				}
			}
		} else {
			php3_error(E_WARNING,"Unable to open PostgresSQL large object");
			RETURN_FALSE;
		}
	} else {
		pgsql_lofp->conn = pgsql;
		pgsql_lofp->lofd = pgsql_lofd;
		return_value->value.lval = php3_list_insert(pgsql_lofp, php3_pgsql_module.le_lofp);
		return_value->type = IS_LONG;
	}
}

void php3_pgsql_lo_close(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *pgsql_lofp;
	int id, type;
	pgLofp *pgsql;

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &pgsql_lofp)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_lofp);
			id = pgsql_lofp->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (pgLofp *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_lofp) {
		php3_error(E_WARNING,"%d is not a PostgresSQL large object index",id);
		RETURN_FALSE;
	}
	
	if (lo_close((PGconn *)pgsql->conn, pgsql->lofd)<0) {
		php3_error(E_WARNING,"Unable to close PostgresSQL large object descriptor %d", pgsql->lofd);
		RETVAL_FALSE;
	} else {
		RETVAL_TRUE;
	}
	php3_list_delete(id);
	return;
}

void php3_pgsql_lo_read(INTERNAL_FUNCTION_PARAMETERS)
{
  	YYSTYPE *pgsql_id, *len;
	int id, buf_len, type;
	char *buf;
	pgLofp *pgsql;

	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &pgsql_id, &len)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_id);
			id = pgsql_id->value.lval;
			convert_to_long(len);
			buf_len = len->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (pgLofp *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_lofp) {
		php3_error(E_WARNING,"%d is not a PostgresSQL large object index",id);
		RETURN_FALSE;
	}
	
	buf = (char *) emalloc(sizeof(char)*(buf_len+1));
	if (!lo_read((PGconn *)pgsql->conn, pgsql->lofd, buf, buf_len)) {
		efree(buf);
		RETURN_FALSE;
	}
	return_value->value.strval = buf;
	return_value->strlen = strlen(return_value->value.strval);
	return_value->type = IS_STRING;
}

void php3_pgsql_lo_write(INTERNAL_FUNCTION_PARAMETERS)
{
  	YYSTYPE *pgsql_id, *str;
	int id, buf_len, nbytes, type;
	char *buf;
	pgLofp *pgsql;

	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &pgsql_id, &str)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_id);
			id = pgsql_id->value.lval;
			convert_to_string(str);
			buf = str->value.strval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (pgLofp *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_lofp) {
		php3_error(E_WARNING,"%d is not a PostgresSQL large object index",id);
		RETURN_FALSE;
	}
	
	buf_len = strlen(buf);
	if ((nbytes = lo_write((PGconn *)pgsql->conn, pgsql->lofd, buf, buf_len))==-1) {
		RETURN_FALSE;
	}
	return_value->value.lval = nbytes;
	return_value->type = IS_LONG;
}

void php3_pgsql_lo_readall(INTERNAL_FUNCTION_PARAMETERS)
{
  	YYSTYPE *pgsql_id;
	int i, id, nbytes, tbytes, type;
	char buf[8192];
	pgLofp *pgsql;
	int output=1;

	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &pgsql_id)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(pgsql_id);
			id = pgsql_id->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}

	pgsql = (pgLofp *) php3_list_find(id,&type);
	if (type!=php3_pgsql_module.le_lofp) {
		php3_error(E_WARNING,"%d is not a PostgresSQL large object index",id);
		RETURN_FALSE;
	}

	output=php3_header(0, NULL);

	tbytes = 0;
	while ((nbytes = lo_read((PGconn *)pgsql->conn, pgsql->lofd, buf, 8192))>0) {
		for(i=0; i<nbytes; i++) {
			if(output) PUTC(buf[i]);
		}
		tbytes += i;
	}
	return_value->value.lval = tbytes;
	return_value->type = IS_LONG;
}
	
#endif
