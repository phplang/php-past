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
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   +----------------------------------------------------------------------+
 */

/* $Id: dav.c,v 1.10 2000/01/01 04:31:14 sas Exp $ */

#define IS_EXT_MODULE
#if COMPILE_DL
#  include "dl/phpdl.h"
#endif
#include "php.h"
#include "internal_functions.h"
#include "php3_dav.h"

#if HAVE_MOD_DAV

# include "php3_dav.h"
# include "variables.h"

#define DAV_GLOBAL(a) a
#define DAV_TLS_VARS

phpdav_module php3_dav_module;

# define DAV_HANDLER(a) DAV_GLOBAL(php3_dav_module).a##_handler
# define DAV_SET_HANDLER(a,b) \
	dav_set_handler(&DAV_GLOBAL(php3_dav_module).a##_handler,(b))

/* {{{ dynamically loadable module stuff */

# if COMPILE_DL
DLEXPORT php3_module_entry *get_module() { return &phpdav_module_entry; };
# endif /* COMPILE_DL */

/* }}} */
/* {{{ function prototypes */

int php3_minit_phpdav(INIT_FUNC_ARGS);
int php3_rinit_phpdav(INIT_FUNC_ARGS);
int php3_mshutdown_phpdav(void);
int php3_rshutdown_phpdav(void);
void php3_info_phpdav(void);

/* }}} */
/* {{{ extension definition structures */

function_entry phpdav_functions[] = {
    PHP_FE(dav_set_mkcol_handlers, NULL)
    {NULL, NULL, NULL}
};

php3_module_entry phpdav_module_entry = {
    "DAV",                   /* extension name */
    phpdav_functions,        /* extension function list */
    php3_minit_phpdav,       /* extension-wide startup function */
    php3_mshutdown_phpdav,   /* extension-wide shutdown function */
    php3_rinit_phpdav,       /* per-request startup function */
    php3_rshutdown_phpdav,   /* per-request shutdown function */
    php3_info_phpdav,        /* information function */
    STANDARD_MODULE_PROPERTIES
};

/* }}} */
/* {{{ startup, shutdown and info functions */

    /* {{{ php3_minit_phpdav */

int php3_minit_phpdav(INIT_FUNC_ARGS)
{
    return SUCCESS;
}

/* }}} */
    /* {{{ php3_rinit_phpdav */

int php3_rinit_phpdav(INIT_FUNC_ARGS)
{
    return SUCCESS;
}

/* }}} */
    /* {{{ php3_mshutdown_phpdav() */

int php3_mshutdown_phpdav()
{
    DAV_TLS_VARS;
    return SUCCESS;
}

/* }}} */
    /* {{{ php3_rshutdown_phpdav() */

int php3_rshutdown_phpdav()
{
    if (DAV_HANDLER(mkcol_test)) {
		efree(DAV_HANDLER(mkcol_test));
    }
    if (DAV_HANDLER(mkcol_create)) {
		efree(DAV_HANDLER(mkcol_create));
    }
    return SUCCESS;
}

/* }}} */
    /* {{{ php3_info_phpdav() */

void php3_info_phpdav()
{
}

/* }}} */

/* }}} */
/* {{{ extension-internal functions */

    /* {{{ dav_set_handler() */

static void
dav_set_handler(char **nameBufp, pval *data)
{
    if (data->value.str.len > 0) {
		if (*nameBufp != NULL) {
			efree(*nameBufp);
		}
		*nameBufp = php3i_pval_strdup(data);
    } else {
		if (*nameBufp != NULL) {
			efree(*nameBufp);
		}
		*nameBufp = NULL;
    }
}

/* }}} */
    /* {{{ dav_call_handler() */

static int
dav_call_handler(char *funcName, int argc, pval **argv)
{
    if (funcName) {
		pval *retval, *func;
		int i, ret;
		HashTable *function_table;

		func = php3i_string_pval(funcName);
		retval = emalloc(sizeof(pval));
		function_table = php3i_get_function_table();
		if (call_user_function(function_table, NULL, func, retval, argc, argv) == FAILURE) {
			php3tls_pval_destructor(retval);
			efree(retval);
			return HTTP_INTERNAL_SERVER_ERROR;
		}
		php3tls_pval_destructor(func);
		efree(func);
		for (i = 0; i < argc; i++) {
			php3tls_pval_destructor(argv[i]);
			efree(argv[i]);
		}
		convert_to_long(retval);
		ret = retval->value.lval;
		efree(retval);
		return ret;
    }
    return DECLINED;
}

/* }}} */

int phpdav_mkcol_test_handler(request_rec *r)
{
    pval *arg;

    if (DAV_HANDLER(mkcol_test) == NULL) {
		return DECLINED;
    }
    arg = php3i_string_pval(r->filename);
	return dav_call_handler(DAV_HANDLER(mkcol_test), 1, &arg);
}

int phpdav_mkcol_create_handler(request_rec *r)
{
    pval *arg;

    if (DAV_HANDLER(mkcol_create) == NULL) {
		return DECLINED;
    }
    arg = php3i_string_pval(r->filename);
	return dav_call_handler(DAV_HANDLER(mkcol_create), 1, &arg);
}

/* }}} */

/************************* EXTENSION FUNCTIONS *************************/

/* {{{ proto void dav_set_mkcol_handlers(string test, string create)
   Sets the function to test whether a DAV collection exists for MKCOL */
PHP_FUNCTION(dav_set_mkcol_handlers)
{
    pval *test, *create;
    DAV_TLS_VARS;

    if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &test, &create) == FAILURE) {
		WRONG_PARAM_COUNT;
    }
    DAV_SET_HANDLER(mkcol_test, test);
    DAV_SET_HANDLER(mkcol_create, create);
    RETVAL_TRUE;
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
