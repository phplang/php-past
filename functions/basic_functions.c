/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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


#ifdef THREAD_SAFE
#include "tls.h"
#endif
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
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include "safe_mode.h"
#include "functions/basic_functions.h"
#include "functions/phpmath.h"
#include "functions/php3_string.h"
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
#include "functions/mail.h"
#if WIN32|WINNT
#include "win32/unistd.h"
#endif

static unsigned char second_and_third_args_force_ref[] = { 3, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };
static unsigned char third_and_fourth_args_force_ref[] = { 4, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };
#if PHP_DEBUGGER
extern int _php3_send_error(char *message, char *address);
#endif



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
	{"asort",		php3_asort,					first_arg_force_ref},
	{"arsort",		php3_arsort,				first_arg_force_ref},
	{"sort",		php3_sort,					first_arg_force_ref},
	{"rsort",		php3_rsort,					first_arg_force_ref},
	{"sizeof",		php3_count,					first_arg_allow_ref},
	{"count",		php3_count,					first_arg_allow_ref},
	{"time",		php3_time,					NULL},
	{"mktime",		php3_mktime,				NULL},
	{"gmmktime",	php3_gmmktime,				NULL},
#if HAVE_STRFTIME
	{"strftime",		php3_strftime,				NULL},
#endif
	{"date",		php3_date,					NULL},
	{"gmdate",		php3_gmdate,				NULL},
	{"getdate",		php3_getdate,				NULL},
	{"checkdate",	php3_checkdate,				NULL},
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
	{"gettype",		php3_gettype,				NULL},
	{"settype",		php3_settype,				first_arg_force_ref},
	{"min",			php3_min,					NULL},
	{"max",			php3_max,					NULL},

	{"addslashes",	php3_addslashes,			NULL},
	{"chop",		php3_chop,					NULL},
	{"trim",		php3_trim,					NULL},
	{"ltrim",		php3_ltrim,					NULL},
	{"rtrim",		php3_chop,					NULL},
	{"pos",			array_current,				first_arg_force_ref},

	{"fsockopen",			php3_fsockopen,		third_and_fourth_args_force_ref},
	{"getimagesize",		php3_getimagesize,	NULL},
	{"htmlspecialchars",	php3_htmlspecialchars,	NULL},
	{"htmlentities",		php3_htmlentities,	NULL},
	{"md5",					php3_md5,			NULL},

	{"parse_url",	php3_parse_url,				NULL},

	{"parse_str",	php3_parsestr,				NULL},
	{"phpinfo",		php3_info,					NULL},
	{"phpversion",	php3_version,				NULL},
	{"strlen",		php3_strlen,				NULL},
	{"strcmp",		php3_strcmp,				NULL},
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
	{"strrchr",		php3_strrchr,				NULL},
	{"substr",		php3_substr,				NULL},
	{"quotemeta",	php3_quotemeta,				NULL},
	{"urlencode",	php3_urlencode,				NULL},
	{"urldecode",	php3_urldecode,				NULL},
	{"rawurlencode",	php3_rawurlencode,		NULL},
	{"rawurldecode",	php3_rawurldecode,		NULL},
	{"ucfirst",		php3_ucfirst,				NULL},
	{"strtr",		php3_strtr,					NULL},
	{"sprintf",		php3_user_sprintf,			NULL},
	{"printf",		php3_user_printf,			NULL},
	{"setlocale",	php3_setlocale,				NULL},

	{"exec",			php3_exec,				second_and_third_args_force_ref},
	{"system",			php3_system,			second_arg_force_ref},
	{"escapeshellcmd",	php3_escapeshellcmd,	NULL},
	{"passthru",		php3_passthru,			second_arg_force_ref},

	{"soundex",		soundex,					NULL},

	{"rand",		php3_rand,					NULL},
	{"srand",		php3_srand,					NULL},
	{"getrandmax",	php3_getrandmax,			NULL},
	{"gethostbyaddr",	php3_gethostbyaddr,		NULL},
	{"gethostbyname",	php3_gethostbyname,		NULL},
	{"gethostbynamel",	php3_gethostbynamel,	NULL},
#if !(WIN32|WINNT)||HAVE_BINDLIB
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
	{"pi",			php3_pi,					NULL},
	{"pow",			php3_pow,					NULL},
	{"exp",			php3_exp,					NULL},
	{"log",			php3_log,					NULL},
	{"log10",		php3_log10,					NULL},
	{"sqrt",		php3_sqrt,					NULL},
	{"bindec",		php3_bindec,				NULL},
	{"hexdec",		php3_hexdec,				NULL},
	{"octdec",		php3_octdec,				NULL},
	{"decbin",		php3_decbin,				NULL},
	{"decoct",		php3_decoct,				NULL},
	{"dechex",		php3_dechex,				NULL},
	{"number_format",	php3_number_format,		NULL},

#if HAVE_PUTENV
	{"putenv",		php3_putenv,				NULL},
#endif
	{"microtime",	php3_microtime,				NULL},
	{"uniqid",		php3_uniqid,				NULL},
	{"linkinfo",	php3_linkinfo,				NULL},
	{"readlink",	php3_readlink,				NULL},
	{"symlink",		php3_symlink,				NULL},
	{"link",		php3_link,					NULL},
	{"get_current_user",	php3_get_current_user,	NULL},
	{"set_time_limit",	php3_set_time_limit,	NULL},
	
	{"get_cfg_var",	php3_get_cfg_var,			NULL},
	{"magic_quotes_runtime",	php3_set_magic_quotes_runtime,	NULL},
	
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
	{NULL, NULL, NULL}
};

php3_module_entry basic_functions_module = {
	"Basic Functions", basic_functions, NULL, NULL, php3_rinit_basic, php3_rshutdown_basic, NULL, STANDARD_MODULE_PROPERTIES
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

int php3_rinit_basic(INIT_FUNC_ARGS)
{
	TLS_VARS;
	GLOBAL(strtok_string) = NULL;
#if HAVE_PUTENV
	if (_php3_hash_init(&putenv_ht, 1, NULL, (void (*)(void *)) _php3_putenv_destructor, 0) == FAILURE) {
		return FAILURE;
	}
#endif
	return SUCCESS;
}

int php3_rshutdown_basic(void)
{
	TLS_VARS;
	STR_FREE(GLOBAL(strtok_string));
#if HAVE_PUTENV
	_php3_hash_destroy(&putenv_ht);
#endif
	return SUCCESS;
}

/********************
 * System Functions *
 ********************/

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
		((ptr = table_get(GLOBAL(php3_rqst)->subprocess_env, str->value.str.val)) || (ptr = getenv(str->value.str.val)))
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

/*******************
 * Basic Functions *
 *******************/

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


void double_value(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_double(num);
	*return_value = *num;
}


void string_value(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(num);
	*return_value = *num;
	yystype_copy_constructor(return_value);
}

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


/* the current implementation of count() is a definite example of what
 * user functions should NOT look like.  It's a hack, until we get
 * unset() to work right in 3.1
 */
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
	yystype_copy_constructor(return_value);
}


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
	yystype_copy_constructor(return_value);
}

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
	yystype_copy_constructor(return_value);
}

void array_each(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array,*entry,real_entry;
	char *string_key;
	int int_key;
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
	yystype_copy_constructor(&real_entry);
	_php3_hash_index_update(return_value->value.ht,1,(void *) &real_entry,sizeof(pval),(void **) &inserted_pointer);
	_php3_hash_pointer_update(return_value->value.ht, "value", sizeof("value"), (void *) inserted_pointer);
	switch (_php3_hash_get_current_key(array->value.ht, &string_key, &int_key)) {
		case HASH_KEY_IS_STRING:
			add_get_index_string(return_value,0,string_key,(void **) &inserted_pointer,0);
			break;
		case HASH_KEY_IS_INT:
			add_get_index_long(return_value,0,int_key, (void **) &inserted_pointer);
			break;
	}
	_php3_hash_pointer_update(return_value->value.ht, "key", sizeof("key"), (void *) inserted_pointer);
	_php3_hash_move_forward(array->value.ht);
}
	
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
	yystype_copy_constructor(return_value);
}

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
	yystype_copy_constructor(return_value);
}


void array_current_key(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *array;
	char *string_key;
	int int_key;

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
	switch (_php3_hash_get_current_key(array->value.ht, &string_key, &int_key)) {
		case HASH_KEY_IS_STRING:
			return_value->value.str.val = string_key;
			return_value->value.str.len = strlen(string_key);
			return_value->type = IS_STRING;
			break;
		case HASH_KEY_IS_INT:
			return_value->type = IS_LONG;
			return_value->value.lval = int_key;
			break;
		case HASH_KEY_NON_EXISTANT:
			return;
	}
}

#ifdef __cplusplus
void php3_flush(HashTable *)
#else
void php3_flush(INTERNAL_FUNCTION_PARAMETERS)
#endif
{
#if APACHE
	TLS_VARS;
#  if MODULE_MAGIC_NUMBER > 19970110
	rflush(GLOBAL(php3_rqst));
#  else
	bflush(GLOBAL(php3_rqst)->connection->client);
#  endif
#endif
#if FHTTPD
       /*FIXME -- what does it flush really? the whole response?*/
#endif
#if CGI_BINARY
	fflush(stdout);
#endif
#if USE_SAPI
	TLS_VARS;
	GLOBAL(sapi_rqst)->flush(GLOBAL(sapi_rqst)->scid);
#endif
}


void php3_sleep(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *num;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &num) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(num);
	sleep(num->value.lval);
}

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
		case IS_CLASS:
			RETVAL_STRING("class",1);
			break;
		case IS_OBJECT:
			RETVAL_STRING("object",1);
			break;
		default:
			RETVAL_STRING("unknown type",1);
	}
}


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
			yystype_copy_constructor(return_value);
		} else {
			php3_error(E_WARNING, "min: array must contain at least 1 element");
			var_uninit(return_value);
		}
	} else {
		if (_php3_hash_minmax(ht, array_data_compare, 0, (void **) &result)==SUCCESS) {
			*return_value = *result;
			yystype_copy_constructor(return_value);
		}
	}
}


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
			yystype_copy_constructor(return_value);
		} else {
			php3_error(E_WARNING, "max: array must contain at least 1 element");
			var_uninit(return_value);
		}
	} else {
		if (_php3_hash_minmax(ht, array_data_compare, 1, (void **) &result)==SUCCESS) {
			*return_value = *result;
			yystype_copy_constructor(return_value);
		}
	}
}

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

void php3_get_current_user(INTERNAL_FUNCTION_PARAMETERS)
{
	TLS_VARS;

	RETURN_STRING(_php3_get_current_user(),1);
}


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


void php3_is_long(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_LONG); }
void php3_is_double(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_DOUBLE); }
void php3_is_string(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_STRING); }
void php3_is_array(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_ARRAY); }
void php3_is_object(INTERNAL_FUNCTION_PARAMETERS) { php3_is_type(INTERNAL_FUNCTION_PARAM_PASSTHRU, IS_OBJECT); }


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
		fwrite(message,strlen(message),1,logfile);
		fclose(logfile);
		break;
	default:
		php3_log_err(message);
		break;
	}
	return SUCCESS;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
