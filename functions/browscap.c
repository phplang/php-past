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
   +----------------------------------------------------------------------+
 */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "php3_browscap.h"

#ifndef THREAD_SAFE
HashTable browser_hash;
static char *lookup_browser_name;
static YYSTYPE *found_browser_entry;
#endif

function_entry browscap_functions[] = {
	{"get_browser",		php3_get_browser,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry browscap_module_entry = {
	"browscap", browscap_functions, php3_minit_browscap, php3_mshutdown_browscap, NULL, NULL, NULL, 0, 0, 0, NULL
};


static int browser_reg_compare(YYSTYPE *browser)
{
	YYSTYPE *browser_name;
	regex_t r;
	TLS_VARS;

	if (GLOBAL(found_browser_entry)) { /* already found */
		return 0;
	}
	hash_find(browser->value.ht,"browser_name_pattern",sizeof("browser_name_pattern"),(void **) &browser_name);
	if (!strchr(browser_name->value.strval,'*')) {
		return 0;
	}
	if (regcomp(&r,browser_name->value.strval,REG_NOSUB)!=0) {
		return 0;
	}
	if (regexec(&r,GLOBAL(lookup_browser_name),0,NULL,0)==0) {
		GLOBAL(found_browser_entry) = browser;
	}
	regfree(&r);
	return 0;
}

void php3_get_browser(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *agent_name,*agent,tmp;
	TLS_VARS;

	if (!php3_ini.browscap) {
		RETURN_FALSE;
	}
	
	switch(ARG_COUNT(ht)) {
		case 0:
			if (hash_find(&GLOBAL(symbol_table), "HTTP_USER_AGENT", sizeof("HTTP_USER_AGENT"), (void **) &agent_name)==FAILURE) {
				agent_name = &tmp;
				var_reset(agent_name);
			}
			break;
		case 1:
			if (getParameters(ht, 1, &agent_name)==FAILURE) {
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	convert_to_string(agent_name);

	if (hash_find(&GLOBAL(browser_hash), agent_name->value.strval, agent_name->strlen+1, (void **) &agent)==FAILURE) {
		GLOBAL(lookup_browser_name) = agent_name->value.strval;
		GLOBAL(found_browser_entry) = NULL;
		hash_apply(&GLOBAL(browser_hash),(int (*)(void *)) browser_reg_compare);
		
		if (GLOBAL(found_browser_entry)) {
			agent = GLOBAL(found_browser_entry);
		} else if (hash_find(&GLOBAL(browser_hash), "Default Browser", sizeof("Default Browser"), (void **) &agent)==FAILURE) {
			RETURN_FALSE;
		}
	}
	
	*return_value = *agent;
	return_value->type = IS_OBJECT;
	yystype_copy_constructor(return_value);
	return_value->value.ht->pDestructor = YYSTYPE_DESTRUCTOR;

	while (hash_find(agent->value.ht, "parent", sizeof("parent"), (void **) &agent_name)==SUCCESS) {
		if (hash_find(&GLOBAL(browser_hash), agent_name->value.strval, agent_name->strlen+1, (void **) &agent)==FAILURE) {
			break;
		}
		hash_merge(return_value->value.ht,agent->value.ht,(void (*)(void *pData)) yystype_copy_constructor, (void *) &tmp, sizeof(YYSTYPE));
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
