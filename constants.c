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

/* $Id: constants.c,v 1.29 2000/02/20 18:40:32 eschmid Exp $ */
#include "php.h"
#include "internal_functions.h"
#include "constants.h"
/* FIXME: v3.1 put into global struct.  I'm putting here to be usable
   elsewhere in php */



static HashTable php3_constants;
static void register_constant(php3_constant *c);

void free_php3_constant(php3_constant *c)
{
	TLS_VARS;
	if (!(c->flags & CONST_PERSISTENT)) {
		pval_destructor(&c->value _INLINE_TLS);
	}
	free(c->name);
}

static int clean_non_persistent_constant(php3_constant *c)
{
	if (c->flags & CONST_PERSISTENT) {
		return 0;
	} else {
		return 1;
	}
}


static int clean_module_constant(php3_constant *c, int *module_number)
{
	if (c->module_number == *module_number) {
		return 1;
	} else {
		return 0;
	}
}

void clean_module_constants(int module_number)
{
	TLS_VARS;
	_php3_hash_apply_with_argument(&GLOBAL(php3_constants), (int (*)(void *,void *)) clean_module_constant, (void *) &module_number);
}

int php3_startup_constants(void)
{
	char *php3_os;
#if WIN32|WINNT
	DWORD dwBuild=0;
	DWORD dwVersion = GetVersion();
	DWORD dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	DWORD dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
#endif
	TLS_VARS;

#if WIN32|WINNT
	/* Get build numbers for Windows NT or Win95 */
	if (dwVersion < 0x80000000){
		php3_os="WINNT";
	} else {
		php3_os="WIN32";
	}
#else
	php3_os=PHP_OS;
#endif


	if (_php3_hash_init(&GLOBAL(php3_constants), 20, NULL, (void(*)(void *)) free_php3_constant, 1)==FAILURE) {
		return FAILURE;
	}
	
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_VERSION", PHP_VERSION, sizeof(PHP_VERSION)-1, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("TRUE", 1L, CONST_PERSISTENT);
	REGISTER_MAIN_STRINGL_CONSTANT("FALSE", "", 0, CONST_PERSISTENT);
	REGISTER_MAIN_STRINGL_CONSTANT("PHP_OS", php3_os, strlen(php3_os), CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_ERROR", E_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_WARNING", E_WARNING, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_NOTICE", E_NOTICE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_PARSE", E_PARSE, CONST_PERSISTENT | CONST_CS);
	REGISTER_MAIN_LONG_CONSTANT("E_ALL", E_ALL, CONST_PERSISTENT | CONST_CS);
		
	return SUCCESS;
}


int php3_shutdown_constants(void)
{
	TLS_VARS;
	_php3_hash_destroy(&GLOBAL(php3_constants));
	return SUCCESS;
}


void clean_non_persistent_constants(void)
{
	TLS_VARS;
	_php3_hash_apply(&GLOBAL(php3_constants), (int (*)(void *)) clean_non_persistent_constant);
}


void php3_register_long_constant(char *name, uint name_len, long lval, int flags, int module_number)
{
	php3_constant c;
	
	c.value.type = IS_LONG;
	c.value.value.lval = lval;
	c.flags = flags;
	c.name = php3_strndup(name,name_len);
	c.name_len = name_len;
	c.module_number = module_number;
	register_constant(&c);
}


void php3_register_double_constant(char *name, uint name_len, double dval, int flags, int module_number)
{
	php3_constant c;
	
	c.value.type = IS_DOUBLE;
	c.value.value.dval = dval;
	c.flags = flags;
	c.name = php3_strndup(name,name_len);
	c.name_len = name_len;
	c.module_number = module_number;
	register_constant(&c);
}


void php3_register_stringl_constant(char *name, uint name_len, char *strval, uint strlen, int flags, int module_number)
{
	php3_constant c;
	
	c.value.type = IS_STRING;
	c.value.value.str.val = strval;
	c.value.value.str.len = strlen;
	c.flags = flags;
	c.name = php3_strndup(name,name_len);
	c.name_len = name_len;
	c.module_number = module_number;
	register_constant(&c);
}


void php3_register_string_constant(char *name, uint name_len, char *strval, int flags, int module_number)
{
	php3_register_stringl_constant(name, name_len, strval, strlen(strval), flags, module_number);
}


int php3_get_constant(char *name, uint name_len, pval *result)
{
	php3_constant *c;
	char *lookup_name = estrndup(name,name_len);
	int retval;
	TLS_VARS;

	php3_str_tolower(lookup_name, name_len);

	if (_php3_hash_find(&GLOBAL(php3_constants), lookup_name, name_len+1, (void **) &c)==SUCCESS) {
		if ((c->flags & CONST_CS) && memcmp(c->name, name, name_len)!=0) {
			retval=0;
		} else {
			retval=1;
			*result = c->value;
			pval_copy_constructor(result);
		}
	} else {
		retval=0;
	}
	
	efree(lookup_name);
	return retval;
}


void register_constant(php3_constant *c)
{
	char *lowercase_name = php3_strndup(c->name, c->name_len);
	TLS_VARS;

#if 0
	printf("Registering constant for module %d\n",c->module_number);
#endif

	php3_str_tolower(lowercase_name, c->name_len);
	if (_php3_hash_add(&GLOBAL(php3_constants), lowercase_name, c->name_len, (void *) c, sizeof(php3_constant), NULL)==FAILURE) {
		pval_destructor(&c->value);
	}
	free(lowercase_name);
}

/* {{{ proto bool define(string var_name, mixed value [, int case_sensitive])
   Defines a constant value */
void php3_define(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *var, *val, *non_cs;
	int case_sensitive;
	php3_constant c;
	
	switch(ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &var, &val)==FAILURE) {
				RETURN_FALSE;
			}
			case_sensitive = CONST_CS;
			break;
		case 3:
			if (getParameters(ht, 3, &var, &val, &non_cs)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(non_cs);
			if (non_cs->value.lval) {
				case_sensitive = 0;
			} else {
				case_sensitive = CONST_CS;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	switch(val->type) {
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
			break;
		default:
			php3_error(E_WARNING,"Constants may only evaluate to scalar values");
			RETURN_FALSE;
			break;
	}
	convert_to_string(var);
	
	c.value = *val;
	pval_copy_constructor(&c.value);
	c.flags = case_sensitive | ~CONST_PERSISTENT; /* non persistent */
	c.name = php3_strndup(var->value.str.val, var->value.str.len);
	c.name_len = var->value.str.len+1;
	register_constant(&c);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int defined(string constant_name)
   Tests if a constant is defined */
void php3_defined(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *var;
	char *lowercase_str;
	php3_constant *c;
	long defined;
	TLS_VARS;
		
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &var)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(var);
	lowercase_str = estrndup(var->value.str.val, var->value.str.len);
	php3_str_tolower(lowercase_str,var->value.str.len);

	if (_php3_hash_find(&GLOBAL(php3_constants), lowercase_str, var->value.str.len+1, (void **) &c)==SUCCESS) {
		if ((c->flags & CONST_CS) && memcmp(c->name, var->value.str.val, var->value.str.len)!=0) {
			defined=0;
		} else {
			defined=1;
		}
	} else {
		defined=0;
	}
	efree(lowercase_str);
	
	RETURN_LONG(defined);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
