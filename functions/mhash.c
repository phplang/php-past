/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 4.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 2000 PHP Development Team (See Credits file)           |
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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#include "php.h"

#if HAVE_LIBMHASH

#include "php_mhash.h"

#include <mhash.h>

function_entry mhash_functions[] = {
	PHP_FE(mhash_get_block_size, NULL)
	PHP_FE(mhash_get_hash_name, NULL)
	PHP_FE(mhash_count, NULL)
	PHP_FE(mhash, NULL)
	{0},
};

static int php_minit_mhash(INIT_FUNC_ARGS);

zend_module_entry mhash_module_entry = {
	"mhash", 
	mhash_functions,
	php_minit_mhash, NULL,
	NULL, NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES,
};

#define MHASH_FAILED_MSG "mhash initialization failed"

static int php_minit_mhash(INIT_FUNC_ARGS)
{
	int i;
	char *name;
	char buf[128];

	for(i = 0; i <= mhash_count(); i++) {
		name = mhash_get_hash_name(i);
		if(name) {
			snprintf(buf, 127, "MHASH_%s", name);
			php3_register_long_constant(buf, strlen(buf) + 1, i, CONST_PERSISTENT, module_number);
			free(name);
		}
	}
	
	return SUCCESS;
}

/* {{{ proto int mhash_count(void)
   Get the number of available hashes */
PHP_FUNCTION(mhash_count)
{
	RETURN_LONG(mhash_count());
}
/* }}} */

/* {{{ proto int mhash_get_block_size(int hash)
   Get the block size of hash */
PHP_FUNCTION(mhash_get_block_size)
{
	pval *hash;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &hash) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(hash);

	RETURN_LONG(mhash_get_block_size(hash->value.lval));
}
/* }}} */


/* {{{ proto string mhash_get_hash_name(int hash)
   Get the name of hash */
PHP_FUNCTION(mhash_get_hash_name)
{
	pval *hash;
	char *name;

	if(ARG_COUNT(ht) != 1 || getParameters(ht, 1, &hash) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(hash);

	name = mhash_get_hash_name(hash->value.lval);
	if(name) {
		RETVAL_STRING(name, 1);
		free(name);
	} else {
		RETVAL_FALSE;
	}
}
/* }}} */

/* {{{ proto string mhash(int hash, string data)
   Hash data with hash */
PHP_FUNCTION(mhash)
{
	pval *hash, *data;
	MHASH td;
	int bsize;
	unsigned char *hash_data;

	if(ARG_COUNT(ht) != 2 || getParameters(ht, 2, &hash, &data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(hash);
	convert_to_string(data);

	bsize = mhash_get_block_size(hash->value.lval);
	td = mhash_init(hash->value.lval);
	if(td == MHASH_FAILED) {
		php3_error(E_WARNING, MHASH_FAILED_MSG);
		RETURN_FALSE;
	}

	mhash(td, data->value.str.val, data->value.str.len);

	hash_data = (unsigned char *) mhash_end(td);
	
	if (hash_data) {
		RETVAL_STRINGL(hash_data, bsize, 1);
	
		free(hash_data);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

#endif
