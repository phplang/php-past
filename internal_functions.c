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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: internal_functions.c,v 1.363 2000/01/01 04:31:12 sas Exp $ */

#include "php.h"
#ifndef MSVC5
#endif
#include "internal_functions.h"
#include "internal_functions_registry.h"
#include "php3_list.h"
#include "modules.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "functions/php3_ifx.h"
#include "functions/php3_ldap.h"
#include "functions/php3_mysql.h"
#include "functions/php3_bcmath.h"
#include "functions/php3_msql.h"
#include "functions/basic_functions.h"
#include "functions/phpmath.h"
#include "functions/php3_string.h"
#include "functions/php3_oci8.h"
#include "functions/php3_magick.h"
#include "functions/oracle.h"
#include "functions/base64.h"
#include "functions/php3_dir.h"
#include "functions/dns.h"
#include "functions/php3_pgsql.h"
#include "functions/php3_velocis.h"
#include "functions/php3_sybase.h"
#include "functions/php3_sybase-ct.h"
#include "functions/reg.h"
#include "functions/php3_mail.h"
#include "functions/imap.h"
#include "functions/imsp.h"
#include "functions/php3_mcal.h"
#include "functions/php_ftp.h"
#include "functions/php3_aspell.h"
#include "functions/md5.h"
#include "functions/php3_gd.h"
#include "functions/html.h"
#include "functions/dl.h"
#include "functions/head.h"
#include "functions/post.h"
#include "functions/exec.h"
#include "functions/php3_solid.h"
#include "functions/adabasd.h"
#include "functions/file.h"
#include "functions/dbase.h"
#include "functions/hw.h"
#include "functions/filepro.h"
#include "functions/db.h"
#include "functions/php3_syslog.h"
#include "functions/php3_filestat.h"
#include "functions/php3_browscap.h"
#include "functions/pack.h"
#include "php3_debugger.h"
#include "functions/php3_unified_odbc.h"
#include "dl/snmp/php3_snmp.h"
#include "functions/php3_zlib.h"
#include "functions/php3_COM.h"
#include "functions/php3_interbase.h"
#include "functions/php3_xml.h"
#include "functions/php3_pdf.h"
#include "functions/php3_cpdf.h"
#include "functions/php3_fdf.h"
#include "functions/php3_sysvsem.h"
#include "functions/php3_sysvshm.h"
#include "functions/php3_dav.h"
#include "functions/php3_gettext.h"
#include "functions/php3_wddx.h"
#include "functions/php3_wddx_a.h"
#include "functions/php3_yp.h"
#include "functions/php3_dba.h"
#include "functions/php_mcrypt.h"
#include "functions/php_mhash.h"
#include "functions/php3_pcre.h"
#include "functions/php3_posix.h"
#include "functions/php3_recode.h"
#include "functions/php_lcg.h"

#ifndef WIN32
#include <netinet/in.h>
#endif
#include "functions/fsock.h"
#include "functions/php3_mckcrypt.h"

extern php3_ini_structure php3_ini;
extern php3_ini_structure php3_ini_master;

HashTable list_destructors,module_registry;
static int module_count;
int current_module_being_cleaned;

unsigned char first_arg_force_ref[] = { 1, BYREF_FORCE };
unsigned char first_arg_allow_ref[] = { 1, BYREF_ALLOW };
unsigned char second_arg_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
unsigned char second_arg_allow_ref[] = { 2, BYREF_NONE, BYREF_ALLOW };

php3_builtin_module php3_builtin_modules[] = 
{
	{"Basic functions",				basic_functions_module_ptr},
	{"Dynamic extension loading",	dl_module_ptr},
	{"Directory",					php3_dir_module_ptr},
	{"File statting",				php3_filestat_module_ptr},
	{"File handling",				php3_file_module_ptr},
	{"GNU gettext",					php3_gettext_module_ptr},
	{"HTTP Header",					php3_header_module_ptr},
	{"Sendmail",					mail_module_ptr},
	{"Debugger",					debugger_module_ptr},
	{"Syslog",						syslog_module_ptr},
	{"MySQL",						mysql_module_ptr},
	{"mcrypt",						mcrypt_module_ptr},
	{"mhash",						mhash_module_ptr},
	{"mSQL",						msql_module_ptr},
	{"PostgresSQL",					pgsql_module_ptr},
	{"Informix",					ifx_module_ptr},
	{"LDAP",						ldap_module_ptr},
	{"Velocis",						velocis_module_ptr},
	{"FilePro",						filepro_module_ptr},
	{"DBA",							dba_module_ptr},
	{"Sybase SQL",					sybase_module_ptr},
	{"Sybase SQL - CT",				sybct_module_ptr},
	{"Unified ODBC",				uodbc_module_ptr},
	{"DBase",						dbase_module_ptr},
	{"WDDX",						wddx_module_ptr},
	{"WDDX_A",						wddx_module_ptr_a},
	{"Socket functions",			fsock_module_ptr},
	{"Hyperwave",					hw_module_ptr},
	{"Regular Expressions",			regexp_module_ptr},
	{"Solid",						solid_module_ptr},
	{"Adabas",						adabas_module_ptr},
	{"Image",						gd_module_ptr},
	{"OCI8",						oci8_module_ptr},
	{"Oracle",						oracle_module_ptr},
	{"Apache",						apache_module_ptr},
	{"Crypt",						crypt_module_ptr},
	{"DBM",							dbm_module_ptr},
	{"bcmath",						bcmath_module_ptr},
	{"browscap",					browscap_module_ptr},
	{"SNMP",						snmp_module_ptr},
	{"Pack/Unpack",					pack_module_ptr},
	{"Zlib",						php3_zlib_module_ptr},
	{"Win32 COM",					COM_module_ptr},
	{"IMAP",						php3_imap_module_ptr},
	{"IMSP",						php3_imsp_module_ptr},
	{"MCAL",						php3_mcal_module_ptr},
	{"FTP",						php3_ftp_module_ptr},
	{"ASPELL",						php3_aspell_module_ptr},
	{"InterBase",					php3_ibase_module_ptr},
	{"XML",							xml_module_ptr},
	{"PDF",							pdf_module_ptr},
	{"CPDF",							cpdf_module_ptr},
	{"FDF",							fdf_module_ptr},
	{"System V semaphores",			sysvsem_module_ptr},
	{"System V shared memory",		sysvshm_module_ptr},
	{"DAV",							phpdav_module_ptr},
	{"MCK Crypt",					mckcrypt_module_ptr},
    {"YP",                          yp_module_ptr},
	{"ImageMagick",					magick_module_ptr},
	{"PCRE",					pcre_module_ptr},
    {"Posix",						posix_module_ptr},
    {"Recode",						recode_module_ptr},
	{"LCG",							phpext_lcg_ptr},
	{NULL,							NULL}
};

	
/* this function doesn't check for too many parameters */
PHPAPI int getParameters(HashTable *ht, int param_count,...)
{
	va_list ptr;
	pval **param, *tmp = NULL;
	int i;

	va_start(ptr, param_count);

	for (i = 0; i < param_count; i++) {
		param = va_arg(ptr, pval **);
		if (_php3_hash_index_find(ht, i, (void **) &tmp) == FAILURE) {
			va_end(ptr);
			return FAILURE;
		}
		*param = tmp;
	}
	va_end(ptr);
	return SUCCESS;
}


PHPAPI int getParametersArray(HashTable *ht, int param_count, pval **argument_array)
{
	int i;
	pval *data;

	for (i = 0; i < param_count; i++) {
		if (_php3_hash_index_find(ht, i, (void **) &data) == FAILURE) {
			return FAILURE;
		}
		argument_array[i] = data;
	}
	return SUCCESS;
}

PHPAPI int getThis(pval **this)
{
	pval *data;
	TLS_VARS;

	if (_php3_hash_find(GLOBAL(function_state.symbol_table), "this", sizeof("this"), (void **)&data) == FAILURE) {
		return FAILURE;
	}
	*this = data;
	return SUCCESS;
}

PHPAPI int ParameterPassedByReference(HashTable *ht, uint n)
{
	return _php3_hash_index_is_pointer(ht, n-1);
}


PHPAPI void wrong_param_count()
{
	TLS_VARS;

	php3_error(E_WARNING,"Wrong parameter count for %s()",GLOBAL(function_state.function_name));
}

	
inline PHPAPI int array_init(pval *arg)
{
	arg->value.ht = (HashTable *) emalloc(sizeof(HashTable));
	if (!arg->value.ht || _php3_hash_init(arg->value.ht, 0, NULL, PVAL_DESTRUCTOR, 0)) {
		php3_error(E_CORE_ERROR, "Cannot allocate memory for array");
		return FAILURE;
	}
	arg->type = IS_ARRAY;
	return SUCCESS;
}

inline PHPAPI int object_init(pval *arg)
{
	arg->value.ht = (HashTable *) emalloc(sizeof(HashTable));
	if (!arg->value.ht || _php3_hash_init(arg->value.ht, 0, NULL, PVAL_DESTRUCTOR, 0)) {
		php3_error(E_CORE_ERROR, "Cannot allocate memory for array");
		return FAILURE;
	}
	arg->type = IS_OBJECT;
	return SUCCESS;
}


inline PHPAPI int add_assoc_long(pval *arg, char *key, long n)
{
	pval tmp;

	tmp.type = IS_LONG;
	tmp.value.lval = n;
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), NULL);
}


inline PHPAPI int add_assoc_double(pval *arg, char *key, double d)
{
	pval tmp;

	tmp.type = IS_DOUBLE;
	tmp.value.dval = d;
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), NULL);
}


inline PHPAPI int add_assoc_string(pval *arg, char *key, char *str, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = strlen(str);
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), NULL);
}


inline PHPAPI int add_assoc_stringl(pval *arg, char *key, char *str, uint length, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = length;
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), NULL);
}


inline PHPAPI int add_assoc_function(pval *arg, char *key,void (*function_ptr)(INTERNAL_FUNCTION_PARAMETERS))
{
	pval tmp;
	
	tmp.type = IS_INTERNAL_FUNCTION;
	tmp.value.func.addr.internal = function_ptr;
	tmp.value.func.arg_types = NULL;
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), NULL);
}


inline PHPAPI int add_index_long(pval *arg, uint index, long n)
{
	pval tmp;

	tmp.type = IS_LONG;
	tmp.value.lval = n;
	return _php3_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(pval),NULL);
}


inline PHPAPI int add_index_double(pval *arg, uint index, double d)
{
	pval tmp;

	tmp.type = IS_DOUBLE;
	tmp.value.dval = d;
	return _php3_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(pval),NULL);
}


inline PHPAPI int add_index_string(pval *arg, uint index, char *str, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = strlen(str);
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(pval),NULL);
}


inline PHPAPI int add_index_stringl(pval *arg, uint index, char *str, uint length, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = length;
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(pval),NULL);
}

inline PHPAPI int add_next_index_long(pval *arg, long n)
{
	pval tmp;

	tmp.type = IS_LONG;
	tmp.value.lval = n;
	return _php3_hash_next_index_insert(arg->value.ht, &tmp, sizeof(pval),NULL);
}


inline PHPAPI int add_next_index_double(pval *arg, double d)
{
	pval tmp;

	tmp.type = IS_DOUBLE;
	tmp.value.dval = d;
	return _php3_hash_next_index_insert(arg->value.ht, &tmp, sizeof(pval),NULL);
}


inline PHPAPI int add_next_index_string(pval *arg, char *str, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = strlen(str);
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_next_index_insert(arg->value.ht, &tmp, sizeof(pval),NULL);
}


inline PHPAPI int add_next_index_stringl(pval *arg, char *str, uint length, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = length;
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_next_index_insert(arg->value.ht, &tmp, sizeof(pval),NULL);
}


inline PHPAPI int add_get_assoc_string(pval *arg, char *key, char *str, void **dest, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = strlen(str);
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), dest);
}


inline PHPAPI int add_get_assoc_stringl(pval *arg, char *key, char *str, uint length, void **dest, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = length;
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), dest);
}


inline PHPAPI int add_get_index_long(pval *arg, uint index, long l, void **dest)
{
	pval tmp;

	tmp.type = IS_LONG;
	tmp.value.lval= l;
	return _php3_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(pval),dest);
}

inline PHPAPI int add_get_index_double(pval *arg, uint index, double d, void **dest)
{
	pval tmp;

	tmp.type = IS_DOUBLE;
	tmp.value.dval= d;
	return _php3_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(pval),dest);
}


inline PHPAPI int add_get_index_string(pval *arg, uint index, char *str, void **dest, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = strlen(str);
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(pval),dest);
}


inline PHPAPI int add_get_index_stringl(pval *arg, uint index, char *str, uint length, void **dest, int duplicate)
{
	pval tmp;

	tmp.type = IS_STRING;
	tmp.value.str.len = length;
	if (duplicate) {
		tmp.value.str.val = estrndup(str,tmp.value.str.len);
	} else {
		tmp.value.str.val = str;
	}
	return _php3_hash_index_update(arg->value.ht, index, (void *) &tmp, sizeof(pval),dest);
}

int module_startup_modules(void)
{
	php3_builtin_module *ptr = php3_builtin_modules;
	TLS_VARS;
	GLOBAL(module_count) = 0;

	while (ptr->name) {
		if (ptr->module) {
			ptr->module->module_number = _php3_next_free_module();
			if (ptr->module->module_startup_func) {
				if (ptr->module->module_startup_func(MODULE_PERSISTENT, ptr->module->module_number)==FAILURE) {
					php3_error(E_CORE_ERROR,"Unable to start %s module",ptr->name);
					return FAILURE;
				}
			}
			ptr->module->type = MODULE_PERSISTENT;
			register_module(ptr->module);
		}
		ptr++;
	}
	return SUCCESS;
}


int _register_list_destructors(void (*list_destructor)(void *), void (*plist_destructor)(void *), int module_number)
{
	list_destructors_entry ld;
	TLS_VARS;
	
#if 0
	printf("Registering destructors %d for module %d\n", GLOBAL(list_destructors).nNextFreeElement, module_number);
#endif
	
	ld.list_destructor=(void (*)(void *)) list_destructor;
	ld.plist_destructor=(void (*)(void *)) plist_destructor;
	ld.module_number = module_number;
	ld.resource_id = GLOBAL(list_destructors).nNextFreeElement;
	
	if (_php3_hash_next_index_insert(&GLOBAL(list_destructors),(void *) &ld,sizeof(list_destructors_entry),NULL)==FAILURE) {
		return FAILURE;
	}
	return GLOBAL(list_destructors).nNextFreeElement-1;
}


/* registers all functions in *library_functions in the function hash */
PHPAPI int register_functions(function_entry *functions)
{
	function_entry *ptr = functions;
	pval phps;
	int count=0,unload=0;
	TLS_VARS;

	while (ptr->fname) {
		phps.value.func.addr.internal = ptr->handler;
		phps.type = IS_INTERNAL_FUNCTION;
		phps.value.func.arg_types = ptr->func_arg_types;
		if (!phps.value.func.addr.internal) {
			php3_error(E_CORE_WARNING,"Null function defined as active function");
			unregister_functions(functions,count);
			return FAILURE;
		}
		if (_php3_hash_add(&GLOBAL(function_table), ptr->fname, strlen(ptr->fname)+1, &phps, sizeof(pval), NULL) == FAILURE) {
			unload=1;
			break;
		}
		ptr++;
		count++;
	}
	if (unload) { /* before unloading, display all remaining bad function in the module */
		while (ptr->fname) {
			if (_php3_hash_exists(&GLOBAL(function_table), ptr->fname, strlen(ptr->fname)+1)) {
				php3_error(E_CORE_WARNING,"Module load failed - duplicate function name - %s",ptr->fname);
			}
			ptr++;
		}
		unregister_functions(functions,count);
		return FAILURE;
	}
	return SUCCESS;
}

/* count=-1 means erase all functions, otherwise, 
 * erase the first count functions
 */
PHPAPI void unregister_functions(function_entry *functions,int count)
{
	function_entry *ptr = functions;
	int i=0;
	TLS_VARS;

	while (ptr->fname) {
		if (count!=-1 && i>=count) {
			break;
		}
#if 0
		php3_printf("Unregistering %s()\n",ptr->fname);
#endif
		_php3_hash_del(&GLOBAL(function_table),ptr->fname,strlen(ptr->fname)+1);
		ptr++;
		i++;
	}
}


int register_module(php3_module_entry *module)
{
	TLS_VARS;

#if 0
	php3_printf("%s:  Registering module %d\n",module->name, module->module_number);
#endif
	if (module->functions && register_functions(module->functions)==FAILURE) {
		php3_error(E_CORE_WARNING,"%s:  Unable to register functions, unable to load",module->name);
		return FAILURE;
	}
	module->module_started=1;
	return _php3_hash_add(&GLOBAL(module_registry),module->name,strlen(module->name)+1,(void *)module,sizeof(php3_module_entry),NULL);
}


void module_destructor(php3_module_entry *module)
{
	TLS_VARS;
	if (module->type == MODULE_TEMPORARY) {
		_php3_hash_apply_with_argument(&GLOBAL(list_destructors), (int (*)(void *,void *)) clean_module_resource_destructors, (void *) &(module->module_number));
		clean_module_constants(module->module_number);
	}

	if (module->request_started && module->request_shutdown_func) {
#if 0
		php3_printf("%s:  Request shutdown\n",module->name);
#endif
		module->request_shutdown_func();
	}
	module->request_started=0;
	if (module->module_started && module->module_shutdown_func) {
#if 0
		php3_printf("%s:  Module shutdown\n",module->name);
#endif
		module->module_shutdown_func();
	}
	module->module_started=0;
	if(module->functions) unregister_functions(module->functions,-1);

#if HAVE_LIBDL
	if (module->handle) {
		dlclose(module->handle);
	}
#endif
}


/* call request startup for all modules */
int module_registry_request_startup(php3_module_entry *module)
{
	if (!module->request_started && module->request_startup_func) {
#if 0
		php3_printf("%s:  Request startup\n",module->name);
#endif
		module->request_startup_func(module->type, module->module_number);
	}
	module->request_started=1;
	return 0;
}


/* for persistent modules - call request shutdown and flag NOT to erase
 * for temporary modules - do nothing, and flag to erase
 */
int module_registry_cleanup(php3_module_entry *module)
{
	switch(module->type) {
		case MODULE_PERSISTENT:
			if (module->request_started && module->request_shutdown_func) {
#if 0
				php3_printf("%s:  Request shutdown\n",module->name);
#endif
				module->request_shutdown_func();
			}
			module->request_started=0;
			return 0;
			break;
		case MODULE_TEMPORARY:
			return 1;
			break;
	}
	return 0;
}


/* return the next free module number */
int _php3_next_free_module(void)
{
	TLS_VARS;
	return ++GLOBAL(module_count);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
