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
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id: basic_functions.c,v 1.285 2000/02/23 22:57:21 zeev Exp $ */
#include "php.h"
#include "modules.h"
#include "internal_functions.h"
#include "internal_functions_registry.h"
#include "php3_list.h"
#include "basic_functions.h"
#include "operators.h"
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#if MSVC5
#include <winsock.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include "safe_mode.h"
#include "functions/basic_functions.h"
#include "functions/phpmath.h"
#include "functions/php3_string.h"
#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif
#include "functions/dns.h"
#include "functions/md5.h"
#include "functions/html.h"
#include "functions/post.h"
#include "functions/exec.h"
#include "functions/info.h"
#include "functions/url.h"
#include "functions/datetime.h"
#include "functions/fsock.h"
#include "functions/image.h"
#include "functions/php3_link.h"
#include "functions/php3_filestat.h"
#include "functions/microtime.h"
#include "functions/pageinfo.h"
#include "functions/uniqid.h"
#include "functions/base64.h"
#include "functions/php3_mail.h"
#include "functions/php3_var.h"
#include "functions/php3_iptc.h"
#include "functions/quot_print.h"
#include "functions/cyr_convert.h"
#if WIN32|WINNT
#include "win32/unistd.h"
#endif

static unsigned char second_and_third_args_force_ref[] = { 3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };
#if PHP_DEBUGGER
extern int _php3_send_error(char *message, char *address);
#endif
static pval *user_compare_func_name;
static HashTable *user_shutdown_function_names;

#if HAVE_PUTENV
static HashTable sm_protected_env_vars;
static char *sm_allowed_env_vars;
#endif


/* some prototypes for local functions */
void user_shutdown_function_dtor(pval *user_shutdown_function_name);
int user_shutdown_function_executor(pval *user_shutdown_function_name);
void php3_call_shutdown_functions(void);

function_entry basic_functions[] = {
	{"intval",		int_value,					NULL},
	{"doubleval",	double_value,				NULL},
	{"strval",		string_value,				NULL},
	{"define",		php3_define,				NULL},
	{"defined",		php3_defined,				NULL},
	{"short_tags",	php3_toggle_short_open_tag, NULL},
	{"sleep",		php3_sleep,					NULL},
	{"usleep",		php3_usleep,				NULL},
	{"ksort",		php3_key_sort,				first_arg_force_ref},
	{"krsort",		php3_key_rsort,				first_arg_force_ref},
	{"asort",		php3_asort,					first_arg_force_ref},
	{"arsort",		php3_arsort,				first_arg_force_ref},
	{"sort",		php3_sort,					first_arg_force_ref},
	{"rsort",		php3_rsort,					first_arg_force_ref},
	{"usort",		php3_user_sort,				first_arg_force_ref},
	{"uasort",		php3_auser_sort,			first_arg_force_ref},
	{"uksort",		php3_user_key_sort,			first_arg_force_ref},
	{"shuffle",		php3_shuffle,				first_arg_force_ref},
	{"array_walk",  php3_array_walk,			first_arg_force_ref},
	{"sizeof",		php3_count,					first_arg_allow_ref},
	{"count",		php3_count,					first_arg_allow_ref},
	{"time",		php3_time,					NULL},
	{"mktime",		php3_mktime,				NULL},
	{"gmmktime",	php3_gmmktime,				NULL},
#if HAVE_STRFTIME
	{"strftime",		php3_strftime,				NULL},
	{"gmstrftime",      php3_gmstrftime,            NULL},
#endif
	{"date",		php3_date,					NULL},
	{"gmdate",		php3_gmdate,				NULL},
	{"getdate",		php3_getdate,				NULL},
	{"checkdate",	php3_checkdate,				NULL},
	{"strtotime",	php3_strtotime,				NULL},
	{"chr",			php3_chr,					NULL},
	{"ord",			php3_ord,					NULL},
	{"flush",		php3_flush,					NULL},
	{"end",			array_end,					first_arg_force_ref},
	{"prev",		array_prev,					first_arg_force_ref},
	{"next",		array_next,					first_arg_force_ref},
	{"reset",		array_reset,				first_arg_force_ref},
	{"current",		array_current,				first_arg_force_ref},
	{"key",			array_current_key,			first_arg_force_ref},
	{"each",		array_each,					first_arg_force_ref},
	{"range",		php3_range,					NULL},
	{"gettype",		php3_gettype,				NULL},
	{"settype",		php3_settype,				first_arg_force_ref},
	{"min",			php3_min,					NULL},
	{"max",			php3_max,					NULL},

	{"addslashes",	php3_addslashes,			NULL},
	{"chop",		php3_chop,					NULL},
	{"str_replace",	php3_str_replace,			NULL},
	{"chunk_split",	php3_chunk_split,			NULL},
	{"strip_tags",	php3_strip_tags,			NULL},
	{"trim",		php3_trim,					NULL},
	{"ltrim",		php3_ltrim,					NULL},
	{"rtrim",		php3_chop,					NULL},
	{"pos",			array_current,				first_arg_force_ref},

	{"getimagesize",		php3_getimagesize,	NULL},
	{"htmlspecialchars",	php3_htmlspecialchars,	NULL},
	{"htmlentities",		php3_htmlentities,	NULL},
	{"gamma_correct_tag",	php3_gamma_correct_tag,		NULL},
	{"md5",					php3_md5,			NULL},

	{"iptcparse",	php3_iptcparse,				NULL},
	{"iptcembed",	php3_iptcembed,				NULL},
	{"parse_url",	php3_parse_url,				NULL},

	{"parse_str",	php3_parsestr,				NULL},
	{"phpinfo",		php3_info,					NULL},
	{"phpversion",	php3_version,				NULL},
	PHP_FE(extension_loaded,				NULL)
	{"strlen",		php3_strlen,				NULL},
	{"strcmp",		php3_strcmp,				NULL},
	{"strspn",		php3_strspn,				NULL},
	{"strcspn",		php3_strcspn,				NULL},
	{"strcasecmp",	php3_strcasecmp,			NULL},
	{"strtok",		php3_strtok,				NULL},
	{"strtoupper",	php3_strtoupper,			NULL},
	{"strtolower",	php3_strtolower,			NULL},
	{"strchr",		php3_strstr,				NULL},
	{"strpos",		php3_strpos,				NULL},
	{"strrpos",		php3_strrpos,				NULL},
	{"strrev",		php3_strrev,				NULL},
	{"hebrev",		php3_hebrev,				NULL},
	{"hebrevc",		php3_hebrev_with_conversion,NULL},
	{"nl2br",		php3_newline_to_br,			NULL},
	{"basename",	php3_basename,				NULL},
	{"dirname", 	php3_dirname,				NULL},
	{"stripslashes",	php3_stripslashes,		NULL},
	{"strstr",		php3_strstr,				NULL},
	{"stristr",		php3_stristr,				NULL},
	{"strrchr",		php3_strrchr,				NULL},
	{"substr",		php3_substr,				NULL},
	{"quotemeta",	php3_quotemeta,				NULL},
	{"urlencode",	php3_urlencode,				NULL},
	{"urldecode",	php3_urldecode,				NULL},
	{"rawurlencode",	php3_rawurlencode,		NULL},
	{"rawurldecode",	php3_rawurldecode,		NULL},
	{"ucfirst",		php3_ucfirst,				NULL},
	{"ucwords",		php3_ucwords,				NULL},
	{"strtr",		php3_strtr,					NULL},
	{"sprintf",		php3_user_sprintf,			NULL},
	{"printf",		php3_user_printf,			NULL},
	{"similar_text",	php3_similar_text,		NULL},
	{"setlocale",	php3_setlocale,				NULL},

	{"exec",			php3_exec,				second_and_third_args_force_ref},
	{"system",			php3_system,			second_arg_force_ref},
	{"escapeshellcmd",	php3_escapeshellcmd,	NULL},
	{"passthru",		php3_passthru,			second_arg_force_ref},

	{"soundex",		soundex,					NULL},

	{"rand",		php3_rand,					NULL},
	{"srand",		php3_srand,					NULL},
	{"getrandmax",	php3_getrandmax,			NULL},
	{"mt_rand",		php3_mt_rand,				NULL},
	{"mt_srand",		php3_mt_srand,			NULL},
	{"mt_getrandmax",	php3_mt_getrandmax,		NULL},
	{"gethostbyaddr",	php3_gethostbyaddr,		NULL},
	{"gethostbyname",	php3_gethostbyname,		NULL},
	{"gethostbynamel",	php3_gethostbynamel,	NULL},
#if !(WIN32|WINNT|OS2)||HAVE_BINDLIB
	{"checkdnsrr",          php3_checkdnsrr,        NULL},
	{"getmxrr",             php3_getmxrr,           second_and_third_args_force_ref},
#endif
	{"explode",		php3_explode,				NULL},
	{"implode",		php3_implode,				NULL},
	{"getenv",		php3_getenv,				NULL},
	{"error_reporting",	php3_error_reporting,	NULL},
	{"clearstatcache",	php3_clearstatcache,	NULL},

	{"unlink",		php3_unlink,				NULL},

	{"getmyuid",	php3_getmyuid,				NULL},
	{"getmypid",	php3_getmypid,				NULL},
	/*getmyiid is here for forward compatibility with 3.1
	  See pageinfo.c in 3.1 for more information*/
	{"getmyiid",	php3_getmypid,				NULL},
	{"getmyinode",	php3_getmyinode,			NULL},
	{"getlastmod",	php3_getlastmod,			NULL},

	{"base64_decode",	php3_base64_decode,		NULL},
	{"base64_encode",	php3_base64_encode,		NULL},

	{"abs",			php3_abs,					NULL},
	{"ceil",		php3_ceil,					NULL},
	{"floor",		php3_floor,					NULL},
	{"round",		php3_round,					NULL},
	{"sin",			php3_sin,					NULL},
	{"cos",			php3_cos,					NULL},
	{"tan",			php3_tan,					NULL},
	{"asin",		php3_asin,					NULL},
	{"acos",		php3_acos,					NULL},
	{"atan",		php3_atan,					NULL},
	{"atan2",		php3_atan2,					NULL},
	{"pi",			php3_pi,					NULL},
	{"pow",			php3_pow,					NULL},
	{"exp",			php3_exp,					NULL},
	{"log",			php3_log,					NULL},
	{"log10",		php3_log10,					NULL},
	{"sqrt",		php3_sqrt,					NULL},
	{"deg2rad",		php3_deg2rad,				NULL},
	{"rad2deg",		php3_rad2deg,				NULL},
	{"bindec",		php3_bindec,				NULL},
	{"hexdec",		php3_hexdec,				NULL},
	{"octdec",		php3_octdec,				NULL},
	{"decbin",		php3_decbin,				NULL},
	{"decoct",		php3_decoct,				NULL},
	{"dechex",		php3_dechex,				NULL},
	PHP_FE(bin2hex, NULL)
	{"base_convert",php3_base_convert,			NULL},
	{"number_format",	php3_number_format,		NULL},

#if HAVE_PUTENV
	{"putenv",		php3_putenv,				NULL},
#endif
#if HAVE_GETRUSAGE
	{"getrusage",	php3_getrusage,				NULL},
#endif
#if HAVE_GETTIMEOFDAY
	{"gettimeofday",	php3_gettimeofday,		NULL},
	{"microtime",	php3_microtime,				NULL},
#endif
	{"uniqid",		php3_uniqid,				NULL},
	{"linkinfo",	php3_linkinfo,				NULL},
	{"readlink",	php3_readlink,				NULL},
	{"symlink",		php3_symlink,				NULL},
	{"link",		php3_link,					NULL},
	{"quoted_printable_decode",	php3_quoted_printable_decode, NULL},	
	{"convert_cyr_string",	php3_convert_cyr_string, NULL},	
	{"get_current_user",	php3_get_current_user,	NULL},
	{"set_time_limit",	php3_set_time_limit,	NULL},
	
	{"get_cfg_var",	php3_get_cfg_var,			NULL},
	{"magic_quotes_runtime",	php3_set_magic_quotes_runtime,	NULL},
	{"set_magic_quotes_runtime",	php3_set_magic_quotes_runtime,	NULL},
	{"get_magic_quotes_gpc",		php3_get_magic_quotes_gpc,	NULL},
	{"get_magic_quotes_runtime",	php3_get_magic_quotes_runtime,	NULL},
	
	{"is_long",		php3_is_long,				first_arg_allow_ref},
	{"is_int",		php3_is_long,				first_arg_allow_ref},
	{"is_integer",	php3_is_long,				first_arg_allow_ref},
	{"is_float",	php3_is_double,				first_arg_allow_ref},
	{"is_double",	php3_is_double,				first_arg_allow_ref},
	{"is_real",		php3_is_double,				first_arg_allow_ref},
	{"is_string",	php3_is_string,				first_arg_allow_ref},
	{"is_array",	php3_is_array,				first_arg_allow_ref},
	{"is_object",	php3_is_object,				first_arg_allow_ref},

	{"leak",		php3_leak,					NULL},	
	{"error_log",	php3_error_log,				NULL},	
	{"call_user_func",	php3_call_user_func,	NULL},
	{"call_user_method", php3_call_user_method,	NULL},

	PHP_FE(var_dump,					NULL)
	PHP_FE(serialize,					first_arg_allow_ref)
	PHP_FE(unserialize,					first_arg_allow_ref)
	
	PHP_FE(register_shutdown_function,	NULL)
	
	PHP_FE(connection_aborted,	NULL)
	PHP_FE(connection_timeout,	NULL)
	PHP_FE(connection_status,	NULL)
	PHP_FE(ignore_user_abort,	NULL)

	PHP_FE(extract, NULL)
	
	PHP_FE(function_exists,		NULL)

	{NULL, NULL, NULL}
};

php3_module_entry basic_functions_module = {
	"Basic Functions",			/* extension name */
	basic_functions,			/* function list */
	php3_minit_basic,			/* process startup */
	php3_mshutdown_basic,		/* process shutdown */
	php3_rinit_basic,			/* request startup */
	php3_rshutdown_basic,		/* request shutdown */
	NULL,						/* extension info */
	STANDARD_MODULE_PROPERTIES
};

#if HAVE_PUTENV
static HashTable putenv_ht;

static void _php3_putenv_destructor(putenv_entry *pe)
{
	if (pe->previous_value) {
		putenv(pe->previous_value);
	} else {
# if HAVE_UNSETENV
		unsetenv(pe->key);
# else
		char **env;
		
		for (env = environ; env != NULL && *env != NULL; env++) {
			if (!strncmp(*env,pe->key,pe->key_len) && (*env)[pe->key_len]=='=') {	/* found it */
				*env = "";
				break;
			}
		}
# endif
	}
	efree(pe->putenv_string);
	efree(pe->key);
}
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define EXTR_OVERWRITE		0
#define EXTR_SKIP			1
#define EXTR_PREFIX_SAME	2
#define	EXTR_PREFIX_ALL		3

int php3_minit_basic(INIT_FUNC_ARGS)
{
	char *protected_vars, *protected_var;
	TLS_VARS;

	REGISTER_DOUBLE_CONSTANT("M_PI", M_PI, CONST_CS | CONST_PERSISTENT);
	
	REGISTER_LONG_CONSTANT("EXTR_OVERWRITE", EXTR_OVERWRITE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_SKIP", EXTR_SKIP, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_SAME", EXTR_PREFIX_SAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EXTR_PREFIX_ALL", EXTR_PREFIX_ALL, CONST_CS | CONST_PERSISTENT);

	_php3_hash_init(&sm_protected_env_vars, 5, NULL, NULL, 1);

	if (cfg_get_string("safe_mode_protected_env_vars", &protected_vars) == FAILURE) {
		protected_vars = NULL;
	}

	if (protected_vars) {
		int dummy=1;

		protected_vars = estrdup(protected_vars);
		protected_var=strtok(protected_vars, ", ");
		while (protected_var) {
			_php3_hash_update(&sm_protected_env_vars, protected_var, strlen(protected_var), &dummy, sizeof(int), NULL);
			protected_var=strtok(NULL, ", ");
		}
		efree(protected_vars);
	}

	if (cfg_get_string("safe_mode_allowed_env_vars", &sm_allowed_env_vars) == FAILURE) {
		sm_allowed_env_vars = NULL;
	}

	return SUCCESS;
}


int php3_mshutdown_basic(void)
{
	_php3_hash_destroy(&sm_protected_env_vars);
}


int php3_rinit_basic(INIT_FUNC_ARGS)
{
	TLS_VARS;
	GLOBAL(strtok_string) = NULL;
	GLOBAL(locale_string) = NULL;
#if HAVE_PUTENV
	if (_php3_hash_init(&putenv_ht, 1, NULL, (void (*)(void *)) _php3_putenv_destructor, 0) == FAILURE) {
		return FAILURE;
	}
#endif
	user_compare_func_name=NULL;
	user_shutdown_function_names=NULL;
	return SUCCESS;
}


int php3_rshutdown_basic(void)
{
	TLS_VARS;
	STR_FREE(GLOBAL(strtok_string));
#if HAVE_PUTENV
	_php3_hash_destroy(&putenv_ht);
#endif
	/* Check if locale was changed and change it back
	   to the value in startup environment */
	if (GLOBAL(locale_string) != NULL) {
		setlocale(LC_ALL, "");
	}
	STR_FREE(GLOBAL(locale_string));
	return SUCCESS;
}

/********************
 * System Functions *
 ********************/
/* {{{ proto string getenv(string varname)
   Get the value of an environment variable */
void php3_getenv(INTERNAL_FUNCTION_PARAMETERS)
{
#if FHTTPD
	int i;
#endif
	pval *str;
	char *ptr;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

#if FHTTPD
	ptr=NULL;
	if (str->type == IS_STRING && req){
		for(i=0;i<req->nlines;i++){
			if (req->lines[i].paramc>1){
				if (req->lines[i].params[0]){
					if (!strcmp(req->lines[i].params[0],
							   str->value.str.val)){
						ptr=req->lines[i].params[1];
						i=req->nlines;
					}
				}
			}
		}
	}
	if (!ptr) ptr = getenv(str->value.str.val);
	if (ptr
#else

	if (str->type == IS_STRING &&
#if APACHE
		((ptr = (char *)table_get(GLOBAL(php3_rqst)->subprocess_env, str->value.str.val)) || (ptr = getenv(str->value.str.val)))
#endif
#if CGI_BINARY
		(ptr = getenv(str->value.str.val))
#endif

#if USE_SAPI
		(ptr = GLOBAL(sapi_rqst)->getenv(GLOBAL(sapi_rqst)->scid,str->value.str.val))
#endif
#endif
		) {
		RETURN_STRING(ptr,1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto void putenv(string setting)
   Set the value of an environment variable */
#if HAVE_PUTENV
void php3_putenv(INTERNAL_FUNCTION_PARAMETERS)
{

	pval *str;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);

	if (str->value.str.val && *(str->value.str.val)) {
		int ret;
		char *p,**env;
		putenv_entry pe;
		
		pe.putenv_string = estrndup(str->value.str.val,str->value.str.len);
		pe.key = str->value.str.val;
		if ((p=strchr(pe.key,'='))) { /* nullify the '=' if there is one */
			*p='\0';
		}
		pe.key_len = strlen(pe.key);
		pe.key = estrndup(pe.key,pe.key_len);
		
		if (php3_ini.safe_mode) {
			/* Check the protected list */
			if (_php3_hash_exists(&sm_protected_env_vars, pe.key, pe.key_len)) {
				php3_error(E_WARNING, "Safe Mode:  Cannot override protected environment variable '%s'", pe.key);
				efree(pe.putenv_string);
				efree(pe.key);
				RETURN_FALSE;
			}

			/* Check the allowed list */
			if (sm_allowed_env_vars && *sm_allowed_env_vars) {
				char *allowed_env_vars = estrdup(sm_allowed_env_vars);
				char *allowed_prefix = strtok(allowed_env_vars, ", ");
				unsigned char allowed=0;

				while (allowed_prefix) {
					if (!strncmp(allowed_prefix, pe.key, strlen(allowed_prefix))) {
						allowed=1;
						break;
					}
					allowed_prefix = strtok(NULL, ", ");
				}
				efree(allowed_env_vars);
				if (!allowed) {
					php3_error(E_WARNING, "Safe Mode:  Cannot set environment variable '%s' - it's not in the allowed list", pe.key);
					efree(pe.putenv_string);
					efree(pe.key);
					RETURN_FALSE;
				}
			}
		}

		_php3_hash_del(&putenv_ht,pe.key,pe.key_len+1);
		
		/* find previous value */
		pe.previous_value = NULL;
		for (env = environ; env != NULL && *env != NULL; env++) {
			if (!strncmp(*env,pe.key,pe.key_len) && (*env)[pe.key_len]=='=') {	/* found it */
				pe.previous_value = *env;
				break;
			}
		}

		if ((ret=putenv(pe.putenv_string))==0) { /* success */
			_php3_hash_add(&putenv_ht,pe.key,pe.key_len+1,(void **) &pe,sizeof(putenv_entry),NULL);
			RETURN_TRUE;
		} else {
			efree(pe.putenv_string);
			efree(pe.key);
			RETURN_FALSE;
		}
	}
}
#endif
/* }}} */

/* {{{ proto int connection_aborted(void)
   Returns true if client disconnected */
void php3_connection_aborted(INTERNAL_FUNCTION_PARAMETERS)
{
	TLS_VARS;

	RETVAL_LONG(GLOBAL(php_connection_status)&PHP_CONNECTION_ABORTED);
}
/* }}} */

/* {{{ proto int connection_timeout(void)
   Returns true if script timed out */
void php3_connection_timeout(INTERNAL_FUNCTION_PARAMETERS)
{
	TLS_VARS;

	RETVAL_LONG(GLOBAL(php_connection_status)&PHP_CONNECTION_TIMEOUT);
}
/* }}} */

/* {{{ proto int connection_status(void)
   Returns the connection status bitfield */
void php3_connection_status(INTERNAL_FUNCTION_PARAMETERS)
{
	TLS_VARS;

	RETVAL_LONG(GLOBAL(php_connection_status));
}
/* }}} */

/* {{{ proto int ignore_user_abort(boolean value)
   Set whether we want to ignore a user abort event or not */
void php3_ignore_user_abort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	int old_setting;
	TLS_VARS;

	old_setting = GLOBAL(ignore_user_abort);
	switch (ARG_COUNT(ht)) {
		case 0:
			break;
		case 1:
			if (getParameters(ht,1,&arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg);
			GLOBAL(ignore_user_abort)=arg->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	RETVAL_LONG(old_setting);
}
/* }}} */

/* {{{ proto int error_reporting([int level])
   Set/get the current error reporting level */
void php3_error_reporting(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	int old_error_reporting;
	TLS_VARS;

	old_error_reporting = GLOBAL(error_reporting);
	switch (ARG_COUNT(ht)) {
		case 0:
			break;
		case 1:
			if (getParameters(ht,1,&arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long(arg);
			GLOBAL(error_reporting)=arg->value.lval;
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	RETVAL_LONG(old_error_reporting);
}
/* }}} */

/* {{{ proto int short_tags(int state)
   Turn the short tags option on or off - returns previous state */
void php3_toggle_short_open_tag(INTERNAL_FUNCTION_PARAMETERS) {
	pval *value;
	int ret;
	TLS_VARS;
	
	ret = php3_ini.short_open_tag;

	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(value);
	php3_ini.short_open_tag = value->value.lval;
	RETURN_LONG(ret);
}
/* }}} */

/*******************
 * Basic Functions *
 *******************/
/* {{{ proto int intval(mixed var [, int base])
   Get the integer value of a variable using the optional base for the conversion */
void int_value(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *num, *arg_base;
	int base;
	
	switch(ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht, 1, &num) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		base = 10;
		break;
	case 2:
		if (getParameters(ht, 2, &num, &arg_base) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(arg_base);
		base = arg_base->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_long_base(num, base);
	*return_value = *num;
}
/* }}} */

/* {{{ proto double doubleval(mixed var)
   Get the double-precision value of a variable */
void double_value(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	*return_value = *num;
}
/* }}} */

/* {{{ proto string strval(mixed var) 
   Get the string value of a variable */
void string_value(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(num);
	*return_value = *num;
	pval_copy_constructor(return_value);
}
/* }}} */

static int array_key_compare(const void *a, const void *b)
{
	Bucket *first;
	Bucket *second;
	int min, r;

	first = *((Bucket **) a);
	second = *((Bucket **) b);

	if (first->arKey == NULL && second->arKey == NULL) {
		return (first->h - second->h);
	} else if (first->arKey == NULL) {
		return -1;
	} else if (second->arKey == NULL) {
		return 1;
	}
	min = MIN(first->nKeyLength, second->nKeyLength);
	if ((r = memcmp(first->arKey, second->arKey, min)) == 0) {
		return (first->nKeyLength - second->nKeyLength);
	} else {
		return r;
	}
}

static int array_reverse_key_compare(const void *a, const void *b)
{ 
	return array_key_compare(a,b)*-1;
}

/* {{{ proto int krsort(array array_arg)
   Sort an array reverse by key */
void php3_key_rsort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in krsort() call");
		return;
	}
	if (!ParameterPassedByReference(ht,1)) {
		php3_error(E_WARNING, "Array not passed by reference in call to krsort()");
		return;
	}
	if (_php3_hash_sort(array->value.ht, array_reverse_key_compare,0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */
/* {{{ proto int ksort(array array_arg)
   Sort an array by key */
void php3_key_sort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in ksort() call");
		return;
	}
	if (!ParameterPassedByReference(ht,1)) {
		php3_error(E_WARNING, "Array not passed by reference in call to ksort()");
		return;
	}
	if (_php3_hash_sort(array->value.ht, array_key_compare,0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */

static int array_data_shuffle(const void *a, const void*b) {
	return (
	/* This is just a little messy. */
#ifdef HAVE_LRAND48
        lrand48()
#else
#ifdef HAVE_RANDOM
        random()
#else
        rand()
#endif
#endif
	% 2) ? 1 : -1;
}

/* {{{ proto int shuffle(array array_arg)
   Randomly shuffle the contents of an array */
PHP_FUNCTION(shuffle)
{
	pval *array;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in shuffle() call");
		return;
	}
	if (!ParameterPassedByReference(ht,1)) {
		php3_error(E_WARNING, "Array not passed by reference in call to shuffle()");
		return;
	}
	if (_php3_hash_sort(array->value.ht, array_data_shuffle,1) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */


/* the current implementation of count() is a definite example of what
 * user functions should NOT look like.  It's a hack, until we get
 * unset() to work right in 3.1
 */
/* {{{ proto int sizeof(mixed var)
   An alias for count */
/* }}} */

/* {{{ proto int count(mixed var)
   Count the number of elements in a variable (usually an array) */
void php3_count(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	Bucket *p;
	pval *tmp;
	int num_elements;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		if (array->type == IS_STRING && array->value.str.val==undefined_variable_string) {
			RETURN_LONG(0);
		} else {
			RETURN_LONG(1);
		}
	}
	
	for (p=array->value.ht->pListHead,num_elements=0; p; p=p->pListNext) {
		tmp = (pval *) p->pData;
		if (tmp->type == IS_STRING && tmp->value.str.val == undefined_variable_string) {
			continue;
		}
		num_elements++;
	}
	
	RETURN_LONG(num_elements);
}
/* }}} */

/* Numbers are always smaller than strings int this function as it
 * anyway doesn't make much sense to compare two different data types.
 * This keeps it consistant and simple.
 */
static int array_data_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval *first;
	pval *second;
	double dfirst, dsecond;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	first = (pval *) f->pData;
	second = (pval *) s->pData;

	if ((first->type == IS_LONG || first->type == IS_DOUBLE) &&
		(second->type == IS_LONG || second->type == IS_DOUBLE)) {
		if (first->type == IS_LONG) {
			dfirst = (double) first->value.lval;
		} else {
			dfirst = first->value.dval;
		}
		if (second->type == IS_LONG) {
			dsecond = (double) second->value.lval;
		} else {
			dsecond = second->value.dval;
		}
		if (dfirst < dsecond) {
			return -1;
		} else if (dfirst == dsecond) {
			return 0;
		} else {
			return 1;
		}
	}
	if ((first->type == IS_LONG || first->type == IS_DOUBLE) &&
		second->type == IS_STRING) {
		return -1;
	} else if ((first->type == IS_STRING) &&
			   (second->type == IS_LONG || second->type == IS_DOUBLE)) {
		return 1;
	}
	if (first->type == IS_STRING && second->type == IS_STRING) {
		return strcmp(first->value.str.val, second->value.str.val);
	}
	return 0;					/* Anything else is equal as it can't be compared */
}

static int array_reverse_data_compare(const void *a, const void *b)
{
	return array_data_compare(a,b)*-1;
}

/* {{{ proto void asort(array array_arg)
   Sort an array and maintain index association */
void php3_asort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in asort() call");
		return;
	}
    if (!ParameterPassedByReference(ht,1)) {
        php3_error(E_WARNING, "Array not passed by reference in call to asort()");
		return;
    }
	if (_php3_hash_sort(array->value.ht, array_data_compare,0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void arsort(array array_arg)
   Sort an array in reverse order and maintain index association */
void php3_arsort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in arsort() call");
		return;
	}
    if (!ParameterPassedByReference(ht,1)) {
        php3_error(E_WARNING, "Array not passed by reference in call to arsort()");
		return;
    }
	if (_php3_hash_sort(array->value.ht, array_reverse_data_compare,0) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void sort(array array_arg)
   Sort an array */
void php3_sort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in sort() call");
		return;
	}
    if (!ParameterPassedByReference(ht,1)) {
        php3_error(E_WARNING, "Array not passed by reference in call to sort()");
		return;
    }
	if (_php3_hash_sort(array->value.ht, array_data_compare,1) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto void rsort(array array_arg)
   Sort an array in reverse order */
void php3_rsort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in rsort() call");
		return;
	}
    if (!ParameterPassedByReference(ht,1)) {
        php3_error(E_WARNING, "Array not passed by reference in call to rsort()");
		return;
    }
	if (_php3_hash_sort(array->value.ht, array_reverse_data_compare,1) == FAILURE) {
		return;
	}
	RETURN_TRUE;
}
/* }}} */


static int array_user_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval *args[2];
	pval retval;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	args[0] = (pval *) f->pData;
	args[1] = (pval *) s->pData;

	if (call_user_function(&GLOBAL(function_table), NULL, user_compare_func_name, &retval, 2, args)==SUCCESS) {
		convert_to_long(&retval);
		return retval.value.lval;
	} else {
		return 0;
	}
}

/* {{{ proto void usort(array array_arg, string cmp_function)
   Sort an array by values using a user-defined comparison function */
void php3_user_sort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	pval *old_compare_func;
	TLS_VARS;

	old_compare_func = user_compare_func_name;
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &array, &user_compare_func_name) == FAILURE) {
		user_compare_func_name = old_compare_func;
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in usort() call");
		user_compare_func_name = old_compare_func;
		return;
	}
	convert_to_string(user_compare_func_name);
	if (_php3_hash_sort(array->value.ht, array_user_compare, 1) == FAILURE) {
		user_compare_func_name = old_compare_func;
		return;
	}
	user_compare_func_name = old_compare_func;
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto void uasort(array array_arg, string cmp_function)
   Sort an array with a user-defined comparison function and maintain index association */
void php3_auser_sort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	pval *old_compare_func;
	TLS_VARS;

	old_compare_func = user_compare_func_name;
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &array, &user_compare_func_name) == FAILURE) {
		user_compare_func_name = old_compare_func;
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in uasort() call");
		user_compare_func_name = old_compare_func;
		return;
	}
	convert_to_string(user_compare_func_name);
	if (_php3_hash_sort(array->value.ht, array_user_compare, 0) == FAILURE) {
		user_compare_func_name = old_compare_func;
		return;
	}
	user_compare_func_name = old_compare_func;
	RETURN_TRUE;
}
/* }}} */

static int array_user_key_compare(const void *a, const void *b)
{
	Bucket *f;
	Bucket *s;
	pval key1, key2;
	pval *args[2];
	pval retval;
	int status;

	args[0] = &key1;
	args[1] = &key2;
	
	f = *((Bucket **) a);
	s = *((Bucket **) b);

	if (f->arKey) {
		key1.value.str.val = estrndup(f->arKey, f->nKeyLength);
		key1.value.str.len = f->nKeyLength;
		key1.type = IS_STRING;
	} else {
		key1.value.lval = f->h;
		key1.type = IS_LONG;
	}
	if (s->arKey) {
		key2.value.str.val = estrndup(s->arKey, s->nKeyLength);
		key2.value.str.len = s->nKeyLength;
		key2.type = IS_STRING;
	} else {
		key2.value.lval = s->h;
		key2.type = IS_LONG;
	}

	status = call_user_function(&GLOBAL(function_table), NULL, user_compare_func_name, &retval, 2, args);
	
	pval_destructor(&key1);
	pval_destructor(&key2);
	
	if (status==SUCCESS) {
		convert_to_long(&retval);
		return retval.value.lval;
	} else {
		return 0;
	}
}

/* {{{ proto void uksort(array array_arg, string cmp_function)
   Sort an array by keys using a user-defined comparison function */
void php3_user_key_sort(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	pval *old_compare_func;
	TLS_VARS;

	old_compare_func = user_compare_func_name;
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &array, &user_compare_func_name) == FAILURE) {
		user_compare_func_name = old_compare_func;
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in uksort() call");
		user_compare_func_name = old_compare_func;
		return;
	}
	convert_to_string(user_compare_func_name);
	if (_php3_hash_sort(array->value.ht, array_user_key_compare, 0) == FAILURE) {
		user_compare_func_name = old_compare_func;
		return;
	}
	user_compare_func_name = old_compare_func;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto mixed end(array array_arg)
   Advances array argument's internal pointer to the last element and return it */
void array_end(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array, *entry;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Variable passed to end() is not an array or object");
		return;
	}
    if (!ParameterPassedByReference(ht,1)) {
        php3_error(E_WARNING, "Array not passed by reference in call to end()");
    }
	_php3_hash_internal_pointer_end(array->value.ht);
	while (1) {
		if (_php3_hash_get_current_data(array->value.ht, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}
		if (entry->type == IS_STRING && entry->value.str.val==undefined_variable_string) {
			_php3_hash_move_backwards(array->value.ht);
		} else {
			break;
		}
	}
	*return_value = *entry;
	pval_copy_constructor(return_value);
}
/* }}} */

/* {{{ proto mixed prev(array array_arg)
   Move array argument's internal pointer to the previous element and return it */
void array_prev(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array, *entry;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Variable passed to prev() is not an array or object");
		RETURN_FALSE;
	}
	do {
		_php3_hash_move_backwards(array->value.ht);
		if (_php3_hash_get_current_data(array->value.ht, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}
	} while (entry->type==IS_STRING && entry->value.str.val==undefined_variable_string);
	
	*return_value = *entry;
	pval_copy_constructor(return_value);
}
/* }}} */

/* {{{ proto mixed next(array array_arg)
   Move array argument's internal pointer to the next element and return it */
void array_next(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array, *entry;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Variable passed to next() is not an array or object");
		RETURN_FALSE;
	}
	do {
		_php3_hash_move_forward(array->value.ht);
		if (_php3_hash_get_current_data(array->value.ht, (void **) &entry) == FAILURE) {
			RETURN_FALSE;
		}
	} while (entry->type==IS_STRING && entry->value.str.val==undefined_variable_string);
	
	*return_value = *entry;
	pval_copy_constructor(return_value);
}
/* }}} */

/* {{{ proto array each(array array_arg)
   Return next key/value pair from an array */
void array_each(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array,*entry,real_entry;
	char *string_key;
	ulong num_key;
	int retval;
	pval *inserted_pointer;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING,"Variable passed to each() is not an array or object");
		return;
	}
	while((retval=_php3_hash_get_current_data(array->value.ht, (void **) &entry)) == SUCCESS) {
		if (entry->type == IS_STRING && entry->value.str.val==undefined_variable_string) {
			_php3_hash_move_forward(array->value.ht);
			continue;
		} else {
			break;
		}
	}
	if (retval==FAILURE) {
		RETURN_FALSE;
	}
	array_init(return_value);
	real_entry = *entry;
	pval_copy_constructor(&real_entry);
	_php3_hash_index_update(return_value->value.ht,1,(void *) &real_entry,sizeof(pval),(void **) &inserted_pointer);
	_php3_hash_pointer_update(return_value->value.ht, "value", sizeof("value"), (void *) inserted_pointer);
	switch (_php3_hash_get_current_key(array->value.ht, &string_key, &num_key)) {
		case HASH_KEY_IS_STRING:
			add_get_index_string(return_value,0,string_key,(void **) &inserted_pointer,0);
			break;
		case HASH_KEY_IS_LONG:
			add_get_index_long(return_value,0,num_key, (void **) &inserted_pointer);
			break;
	}
	_php3_hash_pointer_update(return_value->value.ht, "key", sizeof("key"), (void *) inserted_pointer);
	_php3_hash_move_forward(array->value.ht);
}
/* }}} */

/* {{{ proto mixed reset(array array_arg)
   Set array argument's internal pointer to the first element and return it */
void array_reset(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array, *entry;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Variable passed to reset() is not an array or object");
		return;
	}
	_php3_hash_internal_pointer_reset(array->value.ht);
	while (1) {
		if (_php3_hash_get_current_data(array->value.ht, (void **) &entry) == FAILURE) {
			return;
		}
		if (entry->type==IS_STRING && entry->value.str.val==undefined_variable_string) {
			_php3_hash_move_forward(array->value.ht);
		} else {
			break;
		}
	}
		
	*return_value = *entry;
	pval_copy_constructor(return_value);
}
/* }}} */

/* {{{ proto mixed pos(array array_arg)
   An alias for current */
/* }}} */

/* {{{ proto mixed current(array array_arg)
   Return the element currently pointed to by the internal array pointer */
void array_current(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array, *entry;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Variable passed to current() is not an array or object");
		return;
	}
	if (_php3_hash_get_current_data(array->value.ht, (void **) &entry) == FAILURE) {
		return;
	}
	*return_value = *entry;
	pval_copy_constructor(return_value);
}
/* }}} */

/* {{{ proto mixed key(array array_arg)
   Return the key of the element currently pointed to by the internal array pointer */
void array_current_key(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	char *string_key;
	ulong num_key;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &array) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Variable passed to key() is not an array or object");
		return;
	}
    if (!ParameterPassedByReference(ht,1)) {
        php3_error(E_WARNING, "Array not passed by reference in call to key()");
    }
	switch (_php3_hash_get_current_key(array->value.ht, &string_key, &num_key)) {
		case HASH_KEY_IS_STRING:
			return_value->value.str.val = string_key;
			return_value->value.str.len = strlen(string_key);
			return_value->type = IS_STRING;
			break;
		case HASH_KEY_IS_LONG:
			return_value->type = IS_LONG;
			return_value->value.lval = num_key;
			break;
		case HASH_KEY_NON_EXISTANT:
			return;
	}
}
/* }}} */

/* {{{ proto array range(int low, int high)
   Create an array containing the range of integers from low to high (inclusive) */
PHP_FUNCTION(range)
{
	pval *plow, *phigh;
	int low, high;
	if (ARG_COUNT(ht) != 2 || getParameters(ht,2,&plow,&phigh) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(plow);
	convert_to_long(phigh);
	low = plow->value.lval;
	high = phigh->value.lval;

        /* allocate an array for return */
        if (array_init(return_value) == FAILURE) {
                RETURN_FALSE;
        }

	for (; low <= high; low++) {
		add_next_index_long(return_value, low);
	}
}
/* }}} */

/* {{{ proto void flush(void)
   Flush the output buffer */
#ifdef __cplusplus
void php3_flush(HashTable *)
#else
void php3_flush(INTERNAL_FUNCTION_PARAMETERS)
#endif
{
#if APACHE
	TLS_VARS;
#  if MODULE_MAGIC_NUMBER > 19970110
	if (rflush(GLOBAL(php3_rqst)) == -1)
		GLOBAL(php_connection_status) |= PHP_CONNECTION_ABORTED;
#  else
	if (bflush(GLOBAL(php3_rqst)->connection->client) == -1)
		GLOBAL(php_connection_status) |= PHP_CONNECTION_ABORTED;
#  endif
#endif
#if FHTTPD
       /*FIXME -- what does it flush really? the whole response?
		It flushes whatever has been sent to Apache so far -RL
        */
#endif
#if CGI_BINARY
	fflush(stdout);
#endif
#if USE_SAPI
	TLS_VARS;
	GLOBAL(sapi_rqst)->flush(GLOBAL(sapi_rqst)->scid);
#endif
}
/* }}} */

/* {{{ proto void sleep(int seconds)
   Delay for a given number of seconds */
void php3_sleep(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(num);
	sleep(num->value.lval);
}
/* }}} */

/* {{{ proto void usleep(int micro_seconds)
   Delay for a given number of micro seconds */
void php3_usleep(INTERNAL_FUNCTION_PARAMETERS)
{
#if HAVE_USLEEP
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(num);
	usleep(num->value.lval);
#endif
}
/* }}} */

/* {{{ proto string gettype(mixed var)
   Returns the type of the variable */
void php3_gettype(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	switch (arg->type) {
		case IS_LONG:
			RETVAL_STRING("integer",1);
			break;
		case IS_DOUBLE:
			RETVAL_STRING("double",1);
			break;
		case IS_STRING:
			RETVAL_STRING("string",1);
			break;
		case IS_ARRAY:
			RETVAL_STRING("array",1);
			break;
		case IS_USER_FUNCTION:
			RETVAL_STRING("user function",1);
			break;
		case IS_INTERNAL_FUNCTION:
			RETVAL_STRING("internal function",1);
			break;
		case IS_OBJECT:
			RETVAL_STRING("object",1);
			break;
		default:
			RETVAL_STRING("unknown type",1);
	}
}
/* }}} */

/* {{{ proto int settype(string var, string type)
   Set the type of the variable */
void php3_settype(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *var, *type;
	char *new_type;
	TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &var, &type) ==
		FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(type);
	new_type = type->value.str.val;

	if (!strcasecmp(new_type, "integer")) {
		convert_to_long(var);
	} else if (!strcasecmp(new_type, "double")) {
		convert_to_double(var);
	} else if (!strcasecmp(new_type, "string")) {
		convert_to_string(var);
	} else if (!strcasecmp(new_type, "array")) {
		convert_to_array(var);
	} else if (!strcasecmp(new_type, "object")) {
		convert_to_object(var);
	} else {
		php3_error(E_WARNING, "settype: invalid type");
		RETURN_FALSE;
	}
	RETVAL_TRUE;
}
/* }}} */

/* {{{ proto mixed min(mixed arg1 [, mixed arg2 [, ...]])
   Return the lowest value in an array or a series of arguments */
void php3_min(INTERNAL_FUNCTION_PARAMETERS)
{
	int argc=ARG_COUNT(ht);
	pval *result;
	TLS_VARS;

	if (argc<=0) {
		php3_error(E_WARNING, "min: must be passed at least 1 value");
		var_uninit(return_value);
		return;
	}
	if (argc == 1) {
		pval *arr;

		if (getParameters(ht, 1, &arr) == FAILURE ||
			arr->type != IS_ARRAY) {
			WRONG_PARAM_COUNT;
		}
		if (_php3_hash_minmax(arr->value.ht, array_data_compare, 0, (void **) &result)==SUCCESS) {
			*return_value = *result;
			pval_copy_constructor(return_value);
		} else {
			php3_error(E_WARNING, "min: array must contain at least 1 element");
			var_uninit(return_value);
		}
	} else {
		if (_php3_hash_minmax(ht, array_data_compare, 0, (void **) &result)==SUCCESS) {
			*return_value = *result;
			pval_copy_constructor(return_value);
		}
	}
}
/* }}} */

/* {{{ proto mixed max(mixed arg1 [, mixed arg2 [, ...]])
   Return the highest value in an array or a series of arguments */
void php3_max(INTERNAL_FUNCTION_PARAMETERS)
{
	int argc=ARG_COUNT(ht);
	pval *result;
	TLS_VARS;

	if (argc<=0) {
		php3_error(E_WARNING, "max: must be passed at least 1 value");
		var_uninit(return_value);
		return;
	}
	if (argc == 1) {
		pval *arr;

		if (getParameters(ht, 1, &arr) == FAILURE ||
			arr->type != IS_ARRAY) {
			WRONG_PARAM_COUNT;
		}
		if (_php3_hash_minmax(arr->value.ht, array_data_compare, 1, (void **) &result)==SUCCESS) {
			*return_value = *result;
			pval_copy_constructor(return_value);
		} else {
			php3_error(E_WARNING, "max: array must contain at least 1 element");
			var_uninit(return_value);
		}
	} else {
		if (_php3_hash_minmax(ht, array_data_compare, 1, (void **) &result)==SUCCESS) {
			*return_value = *result;
			pval_copy_constructor(return_value);
		}
	}
}
/* }}} */

static pval *php3_array_walk_func_name;

static int _php3_array_walk(const void *a)
{
	pval *args[1];
	pval retval;

	args[0] = (pval *)a;
	
	call_user_function(&GLOBAL(function_table), NULL, php3_array_walk_func_name, &retval, 1, args);
	return 0;
}

/* {{{ proto int array_walk(array array_arg, string function)
   Apply the given function on every element of the argument array */
void php3_array_walk(INTERNAL_FUNCTION_PARAMETERS) {
	pval *array, *old_walk_func_name;
	TLS_VARS;

	old_walk_func_name = php3_array_walk_func_name;
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &array, &php3_array_walk_func_name) == FAILURE) {
		php3_array_walk_func_name = old_walk_func_name;
		WRONG_PARAM_COUNT;
	}
	if (!(array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in array_walk() call");
		php3_array_walk_func_name = old_walk_func_name;
		return;
	}
	convert_to_string(php3_array_walk_func_name);
	_php3_hash_apply(array->value.ht, (int (*)(void *))_php3_array_walk);
	php3_array_walk_func_name = old_walk_func_name;
	RETURN_TRUE;
}
/* }}} */

#if 0
void php3_max(INTERNAL_FUNCTION_PARAMETERS)
{
	pval **argv;
	int argc, i;
	unsigned short max_type = IS_LONG;
	TLS_VARS;

	argc = ARG_COUNT(ht);
	/* if there is one parameter and this parameter is an array of
	 * 2 or more elements, use that array
	 */
	if (argc == 1) {
		argv = (pval **)emalloc(sizeof(pval *) * argc);
		if (getParametersArray(ht, argc, argv) == FAILURE ||
			argv[0]->type != IS_ARRAY) {
			WRONG_PARAM_COUNT;
		}
		if (argv[0]->value.ht->nNumOfElements < 2) {
			php3_error(E_WARNING,
					   "min: array must contain at least 2 elements");
			RETURN_FALSE;
		}
		/* replace the function parameters with the array */
		ht = argv[0]->value.ht;
		argc = _php3_hash_num_elements(ht);
		efree(argv);
	} else if (argc < 2) {
		WRONG_PARAM_COUNT;
	}
	argv = (pval **)emalloc(sizeof(pval *) * argc);
	if (getParametersArray(ht, argc, argv) == FAILURE) {
		efree(argv);
		WRONG_PARAM_COUNT;
	}
	/* figure out what types to compare
	 * if the arguments contain a double, convert all of them to a double
	 * else convert all of them to long
	 */
	for (i = 0; i < argc; i++) {
		if (argv[i]->type == IS_DOUBLE) {
			max_type = IS_DOUBLE;
			break;
		}
	}
	if (max_type == IS_LONG) {
		convert_to_long(argv[0]);
		return_value->value.lval = argv[0]->value.lval;
		for (i = 1; i < argc; i++) {
			convert_to_long(argv[i]);
			if (argv[i]->value.lval > return_value->value.lval) {
				return_value->value.lval = argv[i]->value.lval;
			}
		}
	} else {
		convert_to_double(argv[0]);
		return_value->value.dval = argv[0]->value.dval;
		for (i = 1; i < argc; i++) {
			convert_to_double(argv[i]);
			if (argv[i]->value.dval > return_value->value.dval) {
				return_value->value.dval = argv[i]->value.dval;
			}
		}
	}
	efree(argv);
	return_value->type = max_type;
}
#endif

/* {{{ proto string get_current_user(void)
   Get the name of the owner of the current PHP script */
void php3_get_current_user(INTERNAL_FUNCTION_PARAMETERS)
{
	TLS_VARS;

	RETURN_STRING(_php3_get_current_user(),1);
}
/* }}} */

/* {{{ proto string get_cfg_var(string option_name)
   Get the value of a PHP configuration option */
void php3_get_cfg_var(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *varname;
	char *value;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &varname)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(varname);
	
	if (cfg_get_string(varname->value.str.val,&value)==FAILURE) {
		RETURN_FALSE;
	}
	RETURN_STRING(value,1);
}
/* }}} */

/* {{{ proto int set_magic_quotes_runtime(int new_setting)
   Set the current active configuration setting of magic_quotes_runtime and return previous */
void php3_set_magic_quotes_runtime(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *new_setting;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &new_setting)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_long(new_setting);
	
	php3_ini.magic_quotes_runtime=new_setting->value.lval;
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int get_magic_quotes_runtime(void)
   Get the current active configuration setting of magic_quotes_runtime */
void php3_get_magic_quotes_runtime(INTERNAL_FUNCTION_PARAMETERS)
{
	RETURN_LONG(php3_ini.magic_quotes_runtime);
}
/* }}} */

/* {{{ proto int get_magic_quotes_gpc(void)
   Get the current active configuration setting of magic_quotes_gpc */
void php3_get_magic_quotes_gpc(INTERNAL_FUNCTION_PARAMETERS)
{
	RETURN_LONG(php3_ini.magic_quotes_gpc);
}
/* }}} */

void php3_is_type(INTERNAL_FUNCTION_PARAMETERS,int type)
{
	pval *arg;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &arg)==FAILURE) {
		RETURN_FALSE;
	}
	if (arg->type == type) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

/* {{{ proto bool is_int(mixed var)
   An alias for is_long */
/* }}} */

/* {{{ proto bool is_integer(mixed var)
   An alias for is_long */
/* }}} */

/* {{{ proto bool is_long(mixed var)
   Returns true if variable is a long (integer) */
void php3_is_long(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_LONG); }
/* }}} */

/* {{{ proto bool is_float(mixed var)
   An alias for is_double */
/* }}} */

/* {{{ proto bool is_real(mixed var)
   An alias for is_double */
/* }}} */

/* {{{ proto bool is_double(mixed var)
   Returns true if variable is a double */
void php3_is_double(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_DOUBLE); }
/* }}} */

/* {{{ proto bool is_string(mixed var)
   Returns true if variable is a string */
void php3_is_string(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_STRING); }
/* }}} */

/* {{{ proto bool is_array(mixed var)
   Returns true if variable is an array */
void php3_is_array(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_ARRAY); }
/* }}} */

/* {{{ proto bool is_object(mixed var)
   Returns true if variable is an object */
void php3_is_object(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_OBJECT); }
/* }}} */

/* {{{ proto void leak(int bytes)
   Leak memory - only useful for debugging internal PHP memory manager issues */
void php3_leak(INTERNAL_FUNCTION_PARAMETERS)
{
	int leakbytes=3;
	pval *leak;

	if (ARG_COUNT(ht)>=1) {
		if (getParameters(ht, 1, &leak)==SUCCESS) {
			convert_to_long(leak);
			leakbytes = leak->value.lval;
		}
	}
	
	emalloc(leakbytes);
}
/* }}} */

/* 
	1st arg = error message
	2nd arg = error option
	3rd arg = optional parameters (email address or tcp address)
	4th arg = used for additional headers if email

  error options
    0 = send to php3_error_log (uses syslog or file depending on ini setting)
	1 = send via email to 3rd parameter 4th option = additional headers
	2 = send via tcp/ip to 3rd parameter (name or ip:port)
	3 = save to file in 3rd parameter
*/
/* {{{ proto int error_log(string message, int message_type [, string destination] [, string extra_headers])
   Send an error message somewhere */
void php3_error_log(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *string, *erropt = NULL, *option = NULL, *emailhead = NULL;
	int opt_err = 0;
	char *message, *opt=NULL, *headers=NULL;
	TLS_VARS;

	switch(ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht,1,&string) == FAILURE) {
			php3_error(E_WARNING,"Invalid argument 1 in error_log");
			RETURN_FALSE;
		}
		break;
	case 2:
		if (getParameters(ht,2,&string,&erropt) == FAILURE) {
			php3_error(E_WARNING,"Invalid arguments in error_log");
			RETURN_FALSE;
		}
		convert_to_long(erropt);
		opt_err=erropt->value.lval;
		break;
	case 3:
		if (getParameters(ht,3,&string,&erropt,&option) == FAILURE){
			php3_error(E_WARNING,"Invalid arguments in error_log");
			RETURN_FALSE;
		}
		convert_to_long(erropt);
		opt_err=erropt->value.lval;
		convert_to_string(option);
		opt=option->value.str.val;
		break;
	case 4:
		if (getParameters(ht,4,&string,&erropt,&option,&emailhead) == FAILURE){
			php3_error(E_WARNING,"Invalid arguments in error_log");
			RETURN_FALSE;
		}
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	convert_to_string(string);
	message=string->value.str.val;
	if (erropt != NULL) {
		convert_to_long(erropt);
		opt_err=erropt->value.lval;
	}
	if (option != NULL) {
		convert_to_string(option);
		opt=option->value.str.val;
	}
	if (emailhead != NULL) {
		convert_to_string(emailhead);
		headers=emailhead->value.str.val;
	}

	if (_php3_error_log(opt_err,message,opt,headers)==FAILURE) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

PHPAPI int _php3_error_log(int opt_err,char *message,char *opt,char *headers){
	FILE *logfile;
	int issock=0, socketd=0;;

	switch(opt_err){
	case 1: /*send an email*/
		{
#if HAVE_SENDMAIL
		if (!_php3_mail(opt,"PHP3 error_log message",message,headers)){
			return FAILURE;
		}
#else
		php3_error(E_WARNING,"Mail option not available!");
		return FAILURE;
#endif
		}
		break;
	case 2: /*send to an address */
#if PHP_DEBUGGER
		if (!_php3_send_error(message,opt)){
			return FAILURE;
		}
#else
		php3_error(E_WARNING,"TCP/IP option not available!");
		return FAILURE;
#endif
		break;
	case 3: /*save to a file*/
		logfile=php3_fopen_wrapper(opt,"a", (IGNORE_URL|ENFORCE_SAFE_MODE), &issock, &socketd);
		if(!logfile) {
			php3_error(E_WARNING,"error_log: Unable to write to %s",opt);
			return FAILURE;
		}
		fwrite(message,strlen(message),1,logfile);
		fclose(logfile);
		break;
	default:
		php3_log_err(message);
		break;
	}
	return SUCCESS;
}

/* {{{ proto mixed call_user_func(???)
   ??? */
void php3_call_user_func(INTERNAL_FUNCTION_PARAMETERS)
{
	pval **params;
	pval retval;
	int arg_count=ARG_COUNT(ht);
	
	if (arg_count<1) {
		WRONG_PARAM_COUNT;
	}
	params = (pval **) emalloc(sizeof(pval)*arg_count);
	
	if (getParametersArray(ht, arg_count, params)==FAILURE) {
		efree(params);
		RETURN_FALSE;
	}
	convert_to_string(params[0]);
	if (call_user_function(&GLOBAL(function_table), NULL, params[0], &retval, arg_count-1, params+1)==SUCCESS) {
		*return_value = retval;
	} else {
		php3_error(E_WARNING,"Unable to call %s() - function does not exist", params[0]->value.str.val);
	}
	efree(params);
}
/* }}} */

/* {{{ proto mixed call_user_method(???)
   ??? */
void php3_call_user_method(INTERNAL_FUNCTION_PARAMETERS)
{
	pval **params;
	pval retval;
	int arg_count=ARG_COUNT(ht);
	
	if (arg_count<2) {
		WRONG_PARAM_COUNT;
	}
	params = (pval **) emalloc(sizeof(pval)*arg_count);
	
	if (getParametersArray(ht, arg_count, params)==FAILURE) {
		efree(params);
		RETURN_FALSE;
	}
	if (params[1]->type != IS_OBJECT) {
		php3_error(E_WARNING,"2nd argument is not an object\n");
		efree(params);
		RETURN_FALSE;
	}
	convert_to_string(params[0]);
	if (call_user_function(&GLOBAL(function_table), params[1], params[0], &retval, arg_count-2, params+2)==SUCCESS) {
		*return_value = retval;
	} else {
		php3_error(E_WARNING,"Unable to call %s() - function does not exist", params[0]->value.str.val);
	}
	efree(params);
}
/* }}} */

void user_shutdown_function_dtor(pval *user_shutdown_function_name)
{
	pval retval;

	if (call_user_function(&GLOBAL(function_table), NULL, user_shutdown_function_name, &retval, 0, NULL)==SUCCESS) {
		pval_destructor(&retval);
	}
	pval_destructor(user_shutdown_function_name);
}


void php3_call_shutdown_functions(void)
{
	if (user_shutdown_function_names) {
		_php3_hash_destroy(user_shutdown_function_names);
		efree(user_shutdown_function_names);
	}
}

/* {{{ proto void register_shutdown_function(string function_name)
   Register a user-level function to be called on request termination */
PHP_FUNCTION(register_shutdown_function)
{
	pval *arg, shutdown_function_name;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &arg)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(arg);
	if (!user_shutdown_function_names) {
		user_shutdown_function_names = (HashTable *) emalloc(sizeof(HashTable));
		_php3_hash_init(user_shutdown_function_names, 0, NULL, (void (*)(void *))user_shutdown_function_dtor, 0);
	}
	
	shutdown_function_name = *arg;
	pval_copy_constructor(&shutdown_function_name);
	
	_php3_hash_next_index_insert(user_shutdown_function_names, &shutdown_function_name, sizeof(pval), NULL);
}
/* }}} */


/* {{{ proto int function_exists(string function_name) 
   Checks if a given function has been defined */
PHP_FUNCTION(function_exists)
{
	pval *fname;
	pval *tmp;
	char *lcname;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &fname)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(fname);
	lcname = estrdup(fname->value.str.val);
	php3_str_tolower(lcname, fname->value.str.len);
	if (_php3_hash_find(&GLOBAL(function_table), lcname,
						fname->value.str.len+1, (void**)&tmp) == FAILURE) {
		efree(lcname);
		RETURN_FALSE;
	} else {
		efree(lcname);
		RETURN_TRUE;
	}
}
/* }}} */


/* {{{ int _php3_valid_var_name(char *varname) */
static int _php3_valid_var_name(char *varname)
{
	int len, i;
	
	if (!varname)
		return 0;
	
	len = strlen(varname);
	
	if (!isalpha((int)varname[0]) && varname[0] != '_')
		return 0;
	
	if (len > 1) {
		for(i=1; i<len; i++) {
			if (!isalnum((int)varname[i]) && varname[i] != '_') {
				return 0;
			}
		}
	}
	
	return 1;
}
/* }}} */


/* {{{ proto void extract(array var_array, int extract_type [, string prefix])
   Imports variables into symbol table from an array */
PHP_FUNCTION(extract)
{
	pval *var_array, *etype, *prefix;
	pval *var, *exist;
	pval data;
	char *varname, *finalname;
	ulong lkey;
	int res, extype;
	
	switch(ARG_COUNT(ht)) {
		case 1:
			if (getParameters(ht, 1, &var_array) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			extype = EXTR_OVERWRITE;
			break;

		case 2:
			if (getParameters(ht, 2, &var_array, &etype) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long(etype);
			extype = etype->value.lval;
			if (extype > EXTR_SKIP && extype <= EXTR_PREFIX_ALL) {
				WRONG_PARAM_COUNT;
			}
			break;
			
		case 3:
			if (getParameters(ht, 3, &var_array, &etype, &prefix) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			extype = etype->value.lval;
			break;

		default:
			WRONG_PARAM_COUNT;
			break;
	}
	
	if (extype < EXTR_OVERWRITE || extype > EXTR_PREFIX_ALL) {
		php3_error(E_WARNING, "Wrong argument in call to extract()");
		return;
	}
	
	if (!(var_array->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in call to extract()");
		return;
	}
		
	_php3_hash_internal_pointer_reset(var_array->value.ht);
	while(_php3_hash_get_current_data(var_array->value.ht, (void **)&var) == SUCCESS) {
		if (!(var->type == IS_STRING &&
			var->value.str.val == undefined_variable_string)) {

			if (_php3_hash_get_current_key(var_array->value.ht, &varname, &lkey) ==
					HASH_KEY_IS_STRING) {

				data = *var;
				pval_copy_constructor(&data);
								
				if (_php3_valid_var_name(varname)) {
					finalname = NULL;
					
					res = _php3_hash_find(GLOBAL(active_symbol_table),
										  varname, strlen(varname)+1, (void**)&exist);
					switch (extype) {
						case EXTR_OVERWRITE:
							finalname = estrdup(varname);
							break;

						case EXTR_PREFIX_SAME:
							if (res != SUCCESS)
								finalname = estrdup(varname);
							/* break omitted intentionally */

						case EXTR_PREFIX_ALL:
							if (!finalname) {
								finalname = emalloc(strlen(varname) + prefix->value.str.len + 2);
								strcpy(finalname, prefix->value.str.val);
								strcat(finalname, "_");
								strcat(finalname, varname);
							}
							break;
							
						default:
							if (res != SUCCESS)
								finalname = estrdup(varname);
							break;
					}
					
					if (finalname) {
						_php3_hash_update(GLOBAL(active_symbol_table), finalname,
										  strlen(finalname)+1, &data, sizeof(pval), NULL);
						efree(finalname);
					}
					else
						pval_destructor(&data);
				}

				efree(varname);
			}
		}
		_php3_hash_move_forward(var_array->value.ht);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
