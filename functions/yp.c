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
   | Authors: Stephanie Wehner <_@r4k.net>                                |
   +----------------------------------------------------------------------+
 */
/* $Id: yp.c,v 1.4 2000/02/21 19:13:59 eschmid Exp $ */

#include "php.h"
#include "internal_functions.h"
#include "php3_yp.h"

#if HAVE_YP

#include <rpcsvc/ypclnt.h>

function_entry yp_functions[] = {
	{"yp_get_default_domain",   php3_yp_get_default_domain, NULL},
	{"yp_order",                php3_yp_order,              NULL},
	{"yp_master",               php3_yp_master,             NULL},
	{"yp_match",                php3_yp_match,              NULL},
	{"yp_first",                php3_yp_first,              NULL},
	{"yp_next",                 php3_yp_next,               NULL},
	{NULL, NULL, NULL}
};

php3_module_entry yp_module_entry = {
	"YP",
	yp_functions,
	php3_minit_yp,
	NULL,
	NULL,
	NULL,
	php3_info_yp,
	STANDARD_MODULE_PROPERTIES
};


int php3_minit_yp(INIT_FUNC_ARGS) {
	return SUCCESS;
}

/* {{{ proto string yp_get_default_domain(void)
   Returns the domain or false */
void php3_yp_get_default_domain(INTERNAL_FUNCTION_PARAMETERS) {
	char *outdomain;

	if(yp_get_default_domain(&outdomain)) {
		RETURN_FALSE;
	}
	RETVAL_STRING(outdomain,1);
}
/* }}} */

/* {{{ proto int yp_order(string domain, string map)            
   Returns the order number or false */
void php3_yp_order(INTERNAL_FUNCTION_PARAMETERS) {
	pval *domain, *map;

#if SOLARIS_YP
	unsigned long outval;
#else
      int outval;
#endif

	if((ARG_COUNT(ht) != 2) || (getParameters(ht,2,&domain,&map) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(domain);
	convert_to_string(map);

	if(yp_order(domain->value.str.val,map->value.str.val,&outval)) {
		RETURN_FALSE;
	}

	RETVAL_LONG(outval);
}
/* }}} */

/* {{{ proto string yp_master(string domain, string map)
   Returns the machine name of the master */
void php3_yp_master(INTERNAL_FUNCTION_PARAMETERS) {
	pval *domain, *map;
	char *outname;

	if((ARG_COUNT(ht) != 2) || (getParameters(ht,2,&domain,&map) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(domain);
	convert_to_string(map);

	if(yp_master(domain->value.str.val,map->value.str.val,&outname)) {
		RETURN_FALSE;
	}

	RETVAL_STRING(outname,1);
}
/* }}} */

/* {{{ proto string yp_match(string domain, string map, string key)
   Returns the matched line or false */
void php3_yp_match(INTERNAL_FUNCTION_PARAMETERS) {
	pval *domain, *map, *key;
	char *outval;
	int outvallen;

	if((ARG_COUNT(ht) != 3) || (getParameters(ht,3,&domain,&map,&key) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(domain);
	convert_to_string(map);
	convert_to_string(key);

	if(yp_match(domain->value.str.val,map->value.str.val,key->value.str.val,key->value.str.len,&outval,&outvallen)) {
		RETURN_FALSE;
	}

	RETVAL_STRING(outval,1);
}
/* }}} */

/* {{{ proto array yp_first(string domain, string map)
   Returns the first key as $var["key"] and the first line as $var["value"] */
void php3_yp_first(INTERNAL_FUNCTION_PARAMETERS) {
	pval *domain, *map;
	char *outval, *outkey;
	int outvallen, outkeylen;

	if((ARG_COUNT(ht) != 2) || (getParameters(ht,2,&domain,&map) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(domain);
	convert_to_string(map);

	if(yp_first(domain->value.str.val,map->value.str.val,&outkey,&outkeylen,&outval,&outvallen)) {
		RETURN_FALSE;
	}
	array_init(return_value);
	add_assoc_string(return_value,"key",outkey,1);
	add_assoc_string(return_value,"value",outval,1);
}
/* }}} */

/* {{{ proto array yp_next(string domain, string map, string key)
   Returns an array with $var[$key] and the the line as the value */
void php3_yp_next(INTERNAL_FUNCTION_PARAMETERS) {
	pval *domain, *map, *key;
	char *outval, *outkey;
	int outvallen, outkeylen;

	if((ARG_COUNT(ht) != 3) || (getParameters(ht,3,&domain,&map, &key) == FAILURE)) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(domain);
	convert_to_string(map);
	convert_to_string(key);

	if(yp_next(domain->value.str.val,map->value.str.val,key->value.str.val,key->value.str.len,&outkey,&outkeylen,&outval,&outvallen)) {
		RETURN_FALSE;
	}
	array_init(return_value);
      add_assoc_string(return_value,outkey,outval,1);
}
/* }}} */

void php3_info_yp() {
	PUTS("Compiled with YP Support.");
}
#endif /* HAVE_YP */
