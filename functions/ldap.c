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
   | Authors: Amitay Isaacs <amitay@cse.iitb.ernet.in>                    |
   |          Eric Warnke   <ericw@albany.edu>                            |
   +----------------------------------------------------------------------+
 */
 

/* $Id: ldap.c,v 1.27 1998/01/30 19:53:09 shane Exp $ */

#ifndef MSVC5
#include "config.h"
#endif
#include "parser.h"
#include "internal_functions.h"

#if HAVE_LDAP

#if COMPILE_DL
#include "dl/phpdl.h"
#include "functions/dl.h"
#endif
#include "list.h"
#include "php3_ldap.h"

#ifdef MSVC5
#include <string.h>
#define strdup _strdup
#undef WINDOWS
#undef strcasecmp
#undef strncasecmp
#define WINSOCK 1
#define __STDC__ 1
#endif
#include <lber.h>
#include <ldap.h>


ldap_module php3_ldap_module;



/*
	This is just a small subset of the functionality provided by the LDAP library. All the 
	operations are synchronous. Referrals are not handled automatically.
*/

function_entry ldap_functions[] = {
	{"ldap_connect", 				php3_ldap_connect,				NULL},
	/* {"ldap_pconnect", 			php3_ldap_pconnect,				NULL}, */
	{"ldap_close", 					php3_ldap_unbind,				NULL},
	{"ldap_bind",					php3_ldap_bind,					NULL},
	{"ldap_unbind",					php3_ldap_unbind,				NULL},
	{"ldap_read",					php3_ldap_read,					NULL},
	{"ldap_list",					php3_ldap_list,					NULL},
	{"ldap_search",					php3_ldap_search,				NULL},
	{"ldap_free_result", 			php3_ldap_free_result,			NULL},
	{"ldap_count_entries", 			php3_ldap_count_entries, 		NULL},
	{"ldap_first_entry",			php3_ldap_first_entry,			NULL},
	{"ldap_next_entry",				php3_ldap_next_entry,			NULL},
	{"ldap_get_entries",			php3_ldap_get_entries,			NULL},
	{"ldap_free_entry", 			php3_ldap_free_entry,			NULL},
	{"ldap_first_attribute",		php3_ldap_first_attribute,		NULL},
	{"ldap_next_attribute",			php3_ldap_next_attribute,		NULL},
	{"ldap_get_attributes",			php3_ldap_get_attributes,		NULL},
	{"ldap_get_attribute_values",	php3_ldap_get_attribute_values,	NULL},
	{"ldap_get_values",				php3_ldap_get_values,			NULL},
	{"ldap_get_dn",					php3_ldap_get_dn,				NULL},
	{"ldap_explode_rdn",			php3_ldap_explode_rdn,			NULL},
	{"ldap_explode_dn",				php3_ldap_explode_dn,			NULL},
	{"ldap_dn2ufn",					php3_ldap_dn2ufn,				NULL},

	{"ldap_add", 					php3_ldap_add,					NULL},
	{"ldap_delete",					php3_ldap_delete,				NULL},
	{"ldap_modify",					php3_ldap_modify,				NULL},
	{NULL, NULL, NULL}
};


php3_module_entry ldap_module_entry = {
	"LDAP", ldap_functions, php3_minit_ldap, NULL, NULL, NULL, php3_info_ldap, 0, 0, 0, NULL
};



#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void ) { return &ldap_module_entry; }
#endif



static void _close_ldap_link(LDAP *ld)
{
  	ldap_unbind_s(ld);
	/* php3_printf("Freeing ldap connection");*/
	php3_ldap_module.num_links--;
}

#if 0
static void _close_ldap_plink(LDAP *ld)
{
        ldap_unbind_s(ld);
	php3_ldap_module.num_persistent--;
	php3_ldap_module.num_links--;
}
#endif

static void _free_ldap_result(LDAPMessage *result)
{
        ldap_msgfree(result);
}

#if 0
static void _free_ber_entry(BerElement *ber)
{
  /*if(ber!=NULL) ber_free(ber,0);*/
}
#endif

int php3_minit_ldap(INITFUNCARG)
{
#if 0
	if (cfg_get_long("ldap.allow_persistent", &php3_ldap_module.allow_persistent) == FAILURE) {
		php3_ldap_module.allow_persistent = 1;
	}
	if (cfg_get_long("ldap.max_persistent", &php3_ldap_module.max_persistent) == FAILURE) {
		php3_ldap_module.max_persistent = -1;
	}
#endif
	if (cfg_get_long("ldap.max_links", &php3_ldap_module.max_links) == FAILURE) {
		php3_ldap_module.max_links = -1;
	}

	if(cfg_get_string("ldap.base_dn", &php3_ldap_module.base_dn) == FAILURE) {
		php3_ldap_module.base_dn = NULL;
	}

	php3_ldap_module.num_persistent = 0;
	php3_ldap_module.le_result = register_list_destructors(_free_ldap_result, NULL);
	php3_ldap_module.le_result_entry = register_list_destructors(NULL, NULL);
	php3_ldap_module.le_ber_entry = register_list_destructors(NULL, NULL);
	php3_ldap_module.le_link = register_list_destructors(_close_ldap_link, NULL);
	/* php3_ldap_module.le_plink = register_list_destructors(NULL, _close_ldap_plink); */

	ldap_module_entry.type = type;

	return SUCCESS;
}

#if 0
int php3_rinit_ldap(INITFUNCARG)
{
	php3_ldap_module.default_link = -1;
	php3_ldap_module.num_links = php3_ldap_module.num_persistent;

	return SUCCESS;
}
#endif

void php3_info_ldap(void)
{
	char maxl[16];
	/*
	if(php3_ldap_module.max_persistent == -1) {
		strcpy(maxp, "Unlimited");
	} else {
		snprintf(maxp, 15, "%ld", php3_ldap_module.max_persistent);
		maxp[15] = 0;
	}
	*/
	if(php3_ldap_module.max_links == -1) {
		strcpy(maxl, "Unlimited");
	} else {
		snprintf(maxl, 15, "%ld", php3_ldap_module.max_links);
		maxl[15] = 0;
	}

	php3_printf("<table>"
		    /* "<tr><td>Allow persistent links:</td><td>%s</td></tr>\n"
				"<tr><td>Persistent links:</td><td>%d/%s</td></tr>\n"*/
				"<tr><td>Total links:</td><td>%d/%s</td></tr>\n"
		                "<tr><td>RCS Version:</td><td>$Id: ldap.c,v 1.27 1998/01/30 19:53:09 shane Exp $</td></tr>\n"
				"</table>\n",
		    /*	(php3_ldap_module.allow_persistent?"Yes":"No"),
				php3_ldap_module.num_persistent,maxp,*/
				php3_ldap_module.num_links,maxl);
}


void php3_ldap_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	char *host;
	int port;
	/*	char *hashed_details;
	int hashed_details_length;*/
	LDAP *ldap;

	switch(ARG_COUNT(ht)) {
		case 0: 
			host = NULL;
			port = 0;
			/* hashed_details = estrndup("ldap_", 5);
			hashed_details_length = 4+1; */
			break;

		case 1: {
				YYSTYPE *yyhost;

				if(getParameters(ht, 1, &yyhost) == FAILURE) {
					RETURN_FALSE;
				}

				convert_to_string(yyhost);
				host = yyhost->value.strval;
				port = 389; /* Default port */

				/* hashed_details_length = yyhost->strlen+4+1;
				hashed_details = emalloc(hashed_details_length+1); 
				sprintf(hashed_details, "ldap_%s", yyhost->value.strval);*/ 
			}
			break;

		case 2: {
				YYSTYPE *yyhost, *yyport;

				if(getParameters(ht, 2, &yyhost, &yyport) == FAILURE) {
					RETURN_FALSE;
				}

				convert_to_string(yyhost);
				host = yyhost->value.strval;
				convert_to_long(yyport);
				port = yyport->value.lval;

			/* Do we need to take care of hosts running multiple LDAP servers ? */
				/*	hashed_details_length = yyhost->strlen+4+1;
				hashed_details = emalloc(hashed_details_length+1);
				sprintf(hashed_details, "ldap_%s", yyhost->value.strval);*/ 
			}
			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}
#if 0
	if(!php3_ldap_module.allow_persistent) {
		persistent = 0;
	}

	if (persistent) {
		list_entry *le;

		if (php3_ldap_module.max_links!=-1 && php3_ldap_module.num_links>=php3_ldap_module.max_links) {
			php3_error(E_WARNING, "LDAP: Too many open links (%d)", php3_ldap_module.num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		if (php3_ldap_module.max_persistent!=-1 && php3_ldap_module.num_persistent>=php3_ldap_module.max_persistent) {
			php3_error(E_WARNING, "LDAP: Too many open persistent links (%d)", php3_ldap_module.num_persistent);
			efree(hashed_details);
			RETURN_FALSE;
		}	
	
		if(hash_find(plist, hashed_details, hashed_details_length+1, (void **) &le) == FAILURE) {
			list_entry new_le;

			if ((ldap = ldap_open(host, port)) == NULL) {
				php3_error(E_WARNING,"LDAP:  Unable to connect to server: %s",ldap->ld_error);
				efree(hashed_details);
				RETURN_FALSE;
			}

			new_le.type = php3_ldap_module.le_plink;
			new_le.ptr = ldap;
			if (hash_update(plist, hashed_details, hashed_details_length+1, (void *)&new_le, sizeof(list_entry), NULL) == FAILURE) {
				efree(hashed_details);
				RETURN_FALSE;
			}

			php3_ldap_module.num_persistent++;
			php3_ldap_module.num_links++;
		} else {	
			if (le->type != php3_ldap_module.le_plink) {
				efree(hashed_details);
				RETURN_FALSE;
			}

			ldap = (LDAP *) le->ptr;
		}
		return_value->value.lval = php3_list_insert(ldap, php3_ldap_module.le_plink);
		return_value->type = IS_LONG;
	} else {
		list_entry *index_ptr, new_index_ptr;

		if(hash_find(list, hashed_details, hashed_details_length+1, (void **) &index_ptr) == SUCCESS) {
			int type, link;
			void *ptr;

			if(index_ptr->type != le_index_ptr) {
				RETURN_FALSE;
			}	

			link = (int) index_ptr->ptr;
			ptr = php3_list_find(link, &type);  /* check if the link is still there */
			if (ptr || !(type == php3_ldap_module.le_link || type == php3_ldap_module.le_plink)) {
				return_value->value.lval = php3_ldap_module.default_link = link;
				return_value->type = IS_LONG;
				efree(hashed_details);
				return;
			} else {
				hash_del(list, hashed_details, hashed_details_length+1);
			}
		}

		if (php3_ldap_module.max_links!=-1 && php3_ldap_module.num_links>=php3_ldap_module.max_links) {
			php3_error(E_WARNING, "LDAP: Too many open links (%d)", php3_ldap_module.num_links);
			efree(hashed_details);
			RETURN_FALSE;
		}
		if ((ldap = ldap_open(host, port)) == NULL) {
			efree(hashed_details);
			RETURN_FALSE;
		}

		return_value->value.lval = php3_list_insert((void *)ldap, php3_ldap_module.le_link);
		return_value->type = IS_LONG;

		new_index_ptr.ptr = (void *) return_value->value.lval;
		new_index_ptr.type = le_index_ptr;
		if (hash_update(list,hashed_details,hashed_details_length+1,(void *) &new_index_ptr, sizeof(list_entry), NULL) == FAILURE) {
			efree(hashed_details);
			RETURN_FALSE;
		}
		php3_ldap_module.num_links++;

	}
	efree(hashed_details);
	php3_ldap_module.default_link = return_value->value.lval;
#endif
	if (php3_ldap_module.max_links!=-1 && php3_ldap_module.num_links>=php3_ldap_module.max_links) {
	  php3_error(E_WARNING, "LDAP: Too many open links (%d)", php3_ldap_module.num_links);
	  RETURN_FALSE;
	}
	ldap = ldap_open(host,port);
	if ( ldap == NULL ) {
	  RETURN_FALSE;
	} else {
	  RETURN_LONG(php3_list_insert((void*)ldap,php3_ldap_module.le_link));
	}

}

#if 0
static int php3_ldap_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
	if(php3_ldap_module.default_link == -1) {
		HashTable dummy;

		hash_init(&dummy, 0, NULL, NULL, 0);
		php3_ldap_do_connect(&dummy, return_value, list, plist, 0);
		hash_destroy(&dummy);
	}

	return php3_ldap_module.default_link;
}
#endif

#if 0
void php3_ldap_connect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_ldap_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}


void php3_ldap_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_ldap_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
#endif

static LDAP * _get_ldap_link(YYSTYPE *link, HashTable *list)
{
LDAP *ldap;
int type;

	convert_to_long(link);
	ldap = (LDAP *) php3_list_find(link->value.lval, &type);
	
	if (!ldap || !(type == php3_ldap_module.le_link || type == php3_ldap_module.le_plink)) {
	  php3_error(E_WARNING, "%d is not a LDAP link index", link->value.lval);
	  return NULL;
	}
	return ldap;
}


static LDAPMessage * _get_ldap_result(YYSTYPE *result, HashTable *list)
{
LDAPMessage *ldap_result;
int type;

	convert_to_long(result);
	ldap_result = (LDAPMessage *) php3_list_find(result->value.lval, &type);

	if(!ldap_result || type != php3_ldap_module.le_result) {
		php3_error(E_WARNING, "%d is not a LDAP result index", result->value.lval);
		return NULL;
	}

	return ldap_result;
}


static LDAPMessage * _get_ldap_result_entry(YYSTYPE *result, HashTable *list)
{
LDAPMessage *ldap_result_entry;
int type;

	convert_to_long(result);
	ldap_result_entry = (LDAPMessage *) php3_list_find(result->value.lval, &type);

	if(!ldap_result_entry || type != php3_ldap_module.le_result_entry) {
		php3_error(E_WARNING, "%d is not a LDAP result entry index", result->value.lval);
		return NULL;
	}

	return ldap_result_entry;
}


static BerElement * _get_ber_entry(YYSTYPE *berp, HashTable *list)
{
BerElement *ber;
int type;

	convert_to_long(berp);
	ber = (BerElement *) php3_list_find(berp->value.lval, &type);

	if( type != php3_ldap_module.le_ber_entry) {
		php3_error(E_WARNING, "%d is not a BerElement index", berp->value.lval);
		return NULL;
	}

	return ber;
}


void php3_ber_free(INTERNAL_FUNCTION_PARAMETERS)
{
        YYSTYPE *berp;
		
	if ( getParameters(ht,1,&berp) == FAILURE ) RETURN_FALSE;
	/*_free_ber_entry(_get_ber_entry(berp,list));*/
	
	php3_list_delete(berp->value.lval);
	RETURN_TRUE;
}


void php3_ldap_bind(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *link, *bind_rdn, *bind_pw;
char *ldap_bind_rdn, *ldap_bind_pw;
LDAP *ldap;

	switch(ARG_COUNT(ht)) {
		case 1: /* Anonymous Bind */
			if(getParameters(ht, 1, &link) == FAILURE) {
				RETURN_FALSE;
			}

			ldap_bind_rdn = NULL;
			ldap_bind_pw = NULL;

			break;

		case 3 :
			if(getParameters(ht, 3, &link, &bind_rdn, &bind_pw) == FAILURE) {
				RETURN_FALSE;
			}

			convert_to_string(bind_rdn);
			convert_to_string(bind_pw);

			ldap_bind_rdn = bind_rdn->value.strval;
			ldap_bind_pw = bind_pw->value.strval;

			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}	

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	if(ldap_bind_s(ldap, ldap_bind_rdn, ldap_bind_pw, LDAP_AUTH_SIMPLE) != LDAP_SUCCESS) {
		php3_error(E_WARNING,"LDAP:  Unable to bind to server: %s",ldap_err2string(ldap->ld_errno));
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}


void php3_ldap_unbind(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *link;
LDAP *ldap;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &link) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(link);

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	/* dont do _close_ldap_link(ldap);*/
	php3_list_delete(link->value.lval);
	RETURN_TRUE;
}


static void php3_ldap_do_search(INTERNAL_FUNCTION_PARAMETERS, int scope)
{
YYSTYPE *link, *base_dn, *filter, *attrs, *attr;
char *ldap_base_dn, *ldap_filter;
LDAP *ldap;
char **ldap_attrs = NULL; 
int attrsonly;
LDAPMessage *ldap_result;
int num_attribs=0, i;

	switch(ARG_COUNT(ht)) {
		case 3 :
			if(getParameters(ht, 3, &link, &base_dn, &filter) == FAILURE) {
				RETURN_FALSE;
			}

			convert_to_string(base_dn);
			convert_to_string(filter);

			ldap_base_dn = base_dn->value.strval;
			ldap_filter = filter->value.strval;

			break;

		case 4 : 
			if(getParameters(ht, 4, &link, &base_dn, &filter, &attrs) == FAILURE) {
				RETURN_FALSE;
			}

			if(attrs->type != IS_ARRAY) {
				php3_error(E_WARNING, "LDAP: Expected Array as last element");
				RETURN_FALSE;
			}

			convert_to_string(base_dn);
			convert_to_string(filter);

			ldap_base_dn = base_dn->value.strval;
			ldap_filter = filter->value.strval;

			num_attribs = hash_num_elements(attrs->value.ht);
			if((ldap_attrs = emalloc((num_attribs+1) * sizeof(char *))) == NULL) {
				php3_error(E_WARNING, "LDAP: Could not allocate memory");
				RETURN_FALSE;
				return;
			}

			for(i=0; i<num_attribs; i++) {
				if (hash_index_find(attrs->value.ht, i, (void **) &attr) == FAILURE) {
					php3_error(E_WARNING, "LDAP: Array initialization wrong");
					RETURN_FALSE;
					return;
				}
				convert_to_string(attr);
				ldap_attrs[i] = attr->value.strval;
			}
			ldap_attrs[num_attribs] = NULL;

			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}

	/* fix to make null base_dn's work */
	if ( strlen(ldap_base_dn) < 1 ) {
	  ldap_base_dn = NULL;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	/* Is it useful to only get the attributes ? */
	attrsonly = 0;	

	/* We can possibly add the timeout value also */

	if(ldap_search_s(ldap, ldap_base_dn, scope, ldap_filter, ldap_attrs, attrsonly, &ldap_result) != LDAP_SUCCESS) {
		php3_error(E_WARNING, "LDAP: Unable to perform the search: %s", ldap_err2string(ldap->ld_errno));
		RETURN_FALSE;
	}

	if(ldap_attrs != NULL) {
	  /*	for(i=0; i<num_attribs; i++) efree(ldap_attrs[i]); */
		efree(ldap_attrs);
	}

	RETURN_LONG(php3_list_insert(ldap_result, php3_ldap_module.le_result));
}


void php3_ldap_read(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_BASE);
}


void php3_ldap_list(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_ONELEVEL);
}


void php3_ldap_search(INTERNAL_FUNCTION_PARAMETERS)
{
	php3_ldap_do_search(INTERNAL_FUNCTION_PARAM_PASSTHRU, LDAP_SCOPE_SUBTREE);
}


void php3_ldap_free_result(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *result;
LDAPMessage *ldap_result;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap_result = _get_ldap_result(result, list);
	if(ldap_result == NULL) {
		RETURN_FALSE;
	} else {
		_free_ldap_result(ldap_result);
		php3_list_delete(result->value.lval);
	}

}


void php3_ldap_count_entries(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *result, *link;
LDAP *ldap;
LDAPMessage *ldap_result;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &link, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	ldap_result = _get_ldap_result(result, list);
	if(ldap_result == NULL) RETURN_FALSE;

	RETURN_LONG(ldap_count_entries(ldap, ldap_result));
}


void php3_ldap_first_entry(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *result, *link;
LDAP *ldap;
LDAPMessage *ldap_result;
LDAPMessage *ldap_result_entry;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &link, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	ldap_result = _get_ldap_result(result, list);
	if(ldap_result == NULL) RETURN_FALSE;

	if((ldap_result_entry = ldap_first_entry(ldap, ldap_result)) == NULL) {
	        /* php3_error(E_WARNING, "LDAP: Unable to read the entries from result : %s", ldap_err2string(ldap->ld_errno));*/
		RETURN_FALSE;
	} else {
		RETURN_LONG(php3_list_insert(ldap_result_entry, php3_ldap_module.le_result_entry));
	}
}


void php3_ldap_next_entry(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *result_entry, *link;
LDAP *ldap;
LDAPMessage *ldap_result_entry, *ldap_result_entry_next;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &link, &result_entry) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	ldap_result_entry = _get_ldap_result_entry(result_entry, list);
	if(ldap_result_entry == NULL) RETURN_FALSE;

	if((ldap_result_entry_next = ldap_next_entry(ldap, ldap_result_entry)) == NULL) {
	        /* php3_error(E_WARNING, "LDAP: Unable to read the entries from result : %s", ldap_err2string(ldap->ld_errno));*/
		RETURN_FALSE;
	} else {
	/* php3_list_delete(result->value.lval); */
		RETURN_LONG(php3_list_insert(ldap_result_entry_next, php3_ldap_module.le_result_entry));
	}
}


void php3_ldap_get_entries(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *link, *result;
LDAPMessage *ldap_result, *ldap_result_entry;
YYSTYPE tmp1, tmp2;
LDAP *ldap;
int num_entries, num_attrib, num_values, i;
int attr_count, entry_count;
BerElement *ber;
char *attribute;
char **ldap_value;
char *dn;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &link, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	ldap_result = _get_ldap_result(result, list);
	if(ldap_result == NULL) RETURN_FALSE;

	num_entries = ldap_count_entries(ldap, ldap_result);

	array_init(return_value);
	add_assoc_long(return_value, "count", num_entries);

	if(num_entries == 0) return;
	
	ldap_result_entry = ldap_first_entry(ldap, ldap_result);
	if(ldap_result_entry == NULL) RETURN_FALSE;
	
	entry_count = 0;

	while(ldap_result_entry != NULL) {

		num_attrib = 0;
		attribute = ldap_first_attribute(ldap, ldap_result_entry, &ber);
		if(attribute == NULL) RETURN_FALSE;
		while (attribute != NULL) {
			num_attrib++;
			attribute = ldap_next_attribute(ldap, ldap_result_entry, ber);
		}

		array_init(&tmp1);

		attr_count = 0;
		attribute = ldap_first_attribute(ldap, ldap_result_entry, &ber);
		while (attribute != NULL) {
			ldap_value = ldap_get_values(ldap, ldap_result_entry, attribute);
			num_values = ldap_count_values(ldap_value);

			array_init(&tmp2);
			add_assoc_long(&tmp2, "count", num_values);
			for(i=0; i<num_values; i++) {
				add_index_string(&tmp2, i, ldap_value[i], 1);
			}	
			free(ldap_value);

			hash_update(tmp1.value.ht, attribute, strlen(attribute)+1, (void *) &tmp2, sizeof(YYSTYPE), NULL);
			add_index_string(&tmp1, attr_count, attribute, 1);

			attr_count++;
			attribute = ldap_next_attribute(ldap, ldap_result_entry, ber);
		}

		add_assoc_long(&tmp1, "count", num_attrib);
		dn = ldap_get_dn(ldap, ldap_result_entry);
		add_assoc_string(&tmp1, "dn", dn, 1);

		hash_index_update(return_value->value.ht, entry_count, (void *) &tmp1, sizeof(YYSTYPE), NULL);
		
		entry_count++;
		ldap_result_entry = ldap_next_entry(ldap, ldap_result_entry);
	}

	add_assoc_long(return_value, "count", num_entries);
}


void php3_ldap_free_entry(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *result_entry;
LDAPMessage *ldap_result_entry;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &result_entry) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap_result_entry = _get_ldap_result_entry(result_entry, list);
	if(ldap_result_entry == NULL) RETURN_FALSE;

	_free_ldap_result(ldap_result_entry);
	php3_list_delete(result_entry->value.lval);
}


void php3_ldap_first_attribute(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *result,*link,*berp;
LDAP *ldap;
LDAPMessage *ldap_result_entry;
BerElement *ber;
char *attribute;

	if(ARG_COUNT(ht) != 3 || getParameters(ht, 3, &link, &result,&berp) == FAILURE || ParameterPassedByReference(ht,3)==0 ) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link,list);
	if(ldap == NULL) RETURN_FALSE;

	ldap_result_entry = _get_ldap_result_entry(result,list);
	if(ldap_result_entry == NULL) RETURN_FALSE;

	if((attribute = ldap_first_attribute(ldap, ldap_result_entry, &ber)) == NULL) {
	  /* php3_error(E_WARNING, "LDAP: Unable to read the attributes from entry : %s", ldap_err2string(ldap->ld_errno)); */
		RETURN_FALSE;
	} else {

	        /* brep is passed by ref so we do not have to account for memory */
	        berp->type=IS_LONG;
		berp->value.lval=php3_list_insert(ber, php3_ldap_module.le_ber_entry);

		RETVAL_STRING(attribute);
#ifdef WINDOWS
		ldap_memfree(attribute);
#endif
	}
}


void php3_ldap_next_attribute(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *result,*link,*berp;
LDAP *ldap;
LDAPMessage *ldap_result_entry;
BerElement *ber;
char *attribute;

	if(ARG_COUNT(ht) != 3 || getParameters(ht, 3, &link, &result,&berp) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link,list);
	if(ldap == NULL) RETURN_FALSE;

	ldap_result_entry = _get_ldap_result_entry(result,list);
	if(ldap_result_entry == NULL) RETURN_FALSE;

	ber = _get_ber_entry(berp,list);

	if((attribute = ldap_next_attribute(ldap, ldap_result_entry, ber)) == NULL) {
	  /* php3_error(E_WARNING, "LDAP: Unable to read the attributes from entry : %s", ldap_err2string(ldap->ld_errno)); */
		RETURN_FALSE;
	}else {

	        RETVAL_STRING(attribute);
#ifdef WINDOWS
	        ldap_memfree(attribute);
#endif
	}
}


void php3_ldap_get_attributes(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *result, *link;
LDAP *ldap;
LDAPMessage *ldap_result_entry;
BerElement *ber;
char *attribute;
int count;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &link, &result) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	ldap_result_entry = _get_ldap_result_entry(result, list);
	if(ldap_result_entry == NULL) RETURN_FALSE;

	if(array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	
	count = 0;
	if((attribute = ldap_first_attribute(ldap, ldap_result_entry, &ber)) == NULL) {
		php3_error(E_WARNING, "LDAP: Unable to read the attributes from entry : %s", ldap_err2string(ldap->ld_errno));
		RETURN_FALSE;
	} else {
		add_next_index_string(return_value, attribute, 1);
		count++;
		while((attribute = ldap_next_attribute(ldap, ldap_result_entry, ber)) != NULL) {
			add_next_index_string(return_value, attribute, 1);
			count++;
		}
	}

	add_assoc_long(return_value, "count", count);
}


void php3_ldap_get_attribute_values(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *link, *result_entry, *attr;
LDAP *ldap;
LDAPMessage *ldap_result_entry;
char *attribute;
char **ldap_value;
int i, num_values;

	if(ARG_COUNT(ht) != 3 || getParameters(ht, 3, &link, &result_entry, &attr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	ldap_result_entry = _get_ldap_result_entry(result_entry, list);
	if(ldap_result_entry == NULL) RETURN_FALSE;

	convert_to_string(attr);
	attribute = attr->value.strval;

	if((ldap_value = ldap_get_values(ldap, ldap_result_entry, attribute)) == NULL) {
		php3_error(E_WARNING, "LDAP: Cannot get the value(s) of attribute %s", ldap_err2string(ldap->ld_errno));
		RETURN_FALSE;
	}

	num_values = ldap_count_values(ldap_value);

	if(array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	for(i=0; i<num_values; i++) {
		add_next_index_string(return_value, ldap_value[i], 1);
	}
	
	add_assoc_long(return_value, "count", num_values);

	ldap_value_free(ldap_value);
}


void php3_ldap_get_values(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *link, *result_entry;
YYSTYPE tmp;
LDAP *ldap;
LDAPMessage *ldap_result_entry;
char *attribute;
char **ldap_value;
int i, count, num_values, num_attrib;
BerElement *ber;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &link, &result_entry) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	ldap_result_entry = _get_ldap_result_entry(result_entry, list);
	if(ldap_result_entry == NULL) RETURN_FALSE;

	num_attrib = 0;
	attribute = ldap_first_attribute(ldap, ldap_result_entry, &ber);
	if(attribute == NULL) RETURN_FALSE;
	while (attribute != NULL) {
		num_attrib++;
		attribute = ldap_next_attribute(ldap, ldap_result_entry, ber);
	}
	
	array_init(return_value);

	count=0;
	attribute = ldap_first_attribute(ldap, ldap_result_entry, &ber);
	while (attribute != NULL) {
		ldap_value = ldap_get_values(ldap, ldap_result_entry, attribute);
		num_values = ldap_count_values(ldap_value);

		array_init(&tmp);
		add_assoc_long(&tmp, "count", num_values);
		for(i=0; i<num_values; i++) {
			add_index_string(&tmp, i, ldap_value[i], 1);
		}
		free(ldap_value);

		hash_update(return_value->value.ht, attribute, strlen(attribute)+1, (void *) &tmp, sizeof(YYSTYPE), NULL);
		add_index_string(return_value, count, attribute, 1);

		count++;
		attribute = ldap_next_attribute(ldap, ldap_result_entry, ber);
	}
	
	add_assoc_long(return_value, "count", num_attrib);
}


void php3_ldap_get_dn(INTERNAL_FUNCTION_PARAMETERS) 
{
	YYSTYPE *link,*entryp;
	LDAP *ld;
	LDAPMessage *entry;
	char *text;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &link, &entryp) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ld = _get_ldap_link(link, list);
	if(ld == NULL) RETURN_FALSE;

	entry = _get_ldap_result_entry(entryp, list);
	if(entry == NULL) RETURN_FALSE;

	text = ldap_get_dn(ld, entry);
	if ( text != NULL ) {
		RETVAL_STRING(text);
#ifdef WINDOWS
		ldap_memfree(text);
#endif
	} else {
		RETURN_FALSE;
	}
}


void php3_ldap_explode_rdn(INTERNAL_FUNCTION_PARAMETERS)
{
	RETURN_TRUE;
}


void php3_ldap_explode_dn(INTERNAL_FUNCTION_PARAMETERS)
{
	RETURN_TRUE;
}


void php3_ldap_dn2ufn(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *dn;
	char *ufn;

	if(ARG_COUNT(ht) !=1 || getParameters(ht,1,&dn)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(dn);
	
	ufn = ldap_dn2ufn(dn->value.strval);
	
	if(ufn !=NULL) {
		RETVAL_STRING(ufn);
#ifdef WINDOWS
		ldap_memfree(ufn);
#endif
	} else {
		RETURN_FALSE;
	}
}
	

void php3_ldap_modify(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *link, *dn, *attr, *values, *value;
LDAP *ldap;
char *ldap_dn, *ldap_attr;
LDAPMod **ldap_mods;
int i, num_values;

	if(ARG_COUNT(ht) != 4 || getParameters(ht, 4, &link, &dn, &attr, &values) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(values->type != IS_ARRAY) {
		php3_error(E_WARNING, "LDAP: Expected Array as the last element");
		RETURN_FALSE;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	convert_to_string(dn);
	ldap_dn = dn->value.strval;

	convert_to_string(attr);
	ldap_attr = attr->value.strval;

	num_values = hash_num_elements(values->value.ht);
	
	ldap_mods = emalloc(2 * sizeof(LDAPMod *));

	ldap_mods[0] = emalloc(sizeof(LDAPMod));

	ldap_mods[0]->mod_op = LDAP_MOD_REPLACE;
	ldap_mods[0]->mod_type = estrdup(ldap_attr);
	ldap_mods[0]->mod_values = emalloc((num_values+1) * sizeof(char *));
	
	for(i=0; i<num_values; i++) {
		hash_index_find(values->value.ht, i, (void **) &value);
		convert_to_string(value);
		ldap_mods[0]->mod_values[i] = value->value.strval;
	}
	ldap_mods[0]->mod_values[num_values] = NULL;

	ldap_mods[1] = NULL;

	if(ldap_modify_s(ldap, ldap_dn, ldap_mods) != LDAP_SUCCESS) {
		ldap_perror(ldap, "LDAP");
/*
		php3_error(E_WARNING, "LDAP: modify operation could not be completed.");
*/
	}

	efree(ldap_mods[0]->mod_type);
	efree(ldap_mods[0]->mod_values);
	efree(ldap_mods[0]);
	efree(ldap_mods);

	RETURN_TRUE;
}


void php3_ldap_add(INTERNAL_FUNCTION_PARAMETERS)
{
	RETURN_TRUE;
}


void php3_ldap_delete(INTERNAL_FUNCTION_PARAMETERS)
{
YYSTYPE *link, *dn;
LDAP *ldap;
char *ldap_dn;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &link, &dn) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ldap = _get_ldap_link(link, list);
	if(ldap == NULL) RETURN_FALSE;

	convert_to_string(dn);
	ldap_dn = dn->value.strval;

	if(ldap_delete_s(ldap, ldap_dn) != LDAP_SUCCESS) {
		ldap_perror(ldap, "LDAP");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}


#endif
