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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id: php.h,v 1.69 2000/04/10 19:29:36 andi Exp $ */

#ifndef _PHP_H
#define _PHP_H

#define PHP_API_VERSION 19980604

#define YYDEBUG 0

#define CGI_BINARY (!APACHE && !USE_SAPI && !FHTTPD)

#include "php_version.h"

/* somebody stealing BOOL from windows.  pick something else!
#ifndef BOOL
#define BOOL MYBOOL
#endif
*/

#if DEBUG || !(defined(__GNUC__)||defined(WIN32))
#ifdef inline
#undef inline
#endif
#define inline
#endif

#if WIN32
#include "config.w32.h"
#include "win95nt.h"
# if defined(COMPILE_DL)
# define PHPAPI __declspec(dllimport) 
# else
# define PHPAPI __declspec(dllexport) 
# endif
/* This is used for functions we always need to export. */
# define PHPAPI_EXPORT __declspec(dllexport)
#else
# include "config.h"
# define PHPAPI
# define PHPAPI_EXPORT
# define THREAD_LS
#endif

#ifdef __EMX__
#define OS2 1
#define TCPIPV4
#define MAXSOCKETS 2048
#if !APACHE
#define strcasecmp(s1, s2) stricmp(s1, s2)
#define strncasecmp(s1, s2, n) strnicmp(s1, s2, n)
#endif
#endif

#if HAVE_UNIX_H
#include <unix.h>
#endif

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "request_info.h"

#if HAVE_LIBDL
# if MSVC5
#  include <windows.h>
#  define dlclose FreeLibrary
#  define dlopen(a,b) LoadLibrary(a)
#  define dlsym GetProcAddress
# else
#if HAVE_DLFCN_H && !(defined(_AIX) && APACHE)
#  include <dlfcn.h>
#endif
# endif
#endif

#define GLOBAL(a) a
#define STATIC GLOBAL
#define TLS_VARS
#define CREATE_MUTEX(a,b)
#define SET_MUTEX(a)
#define FREE_MUTEX(a)

/* needed in control structures */
#define INLINE_TLS
#define INLINE_TLS_VOID void
#define _INLINE_TLS
#define _INLINE_TLS_VOID

/*
 * Then the ODBC support can use both iodbc and Solid,
 * uncomment this.
 * #define HAVE_ODBC (HAVE_IODBC|HAVE_SOLID)
 */

#ifdef HAVE_FEATURES_H
#undef _GNU_SOURCE
#define _GNU_SOURCE 1
#include <features.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_STDARG_H
#include <stdarg.h>
#else   
# if HAVE_SYS_VARARGS_H
# include <sys/varargs.h>
# endif 
#endif 

#if 0
#include "compat.h"
#endif
	
#include "php3_hash.h"
#include "php_alloc.h"

#if REGEX
#include "regex/regex.h"
#else
#include <regex.h>
#endif

#ifndef _REGEX_H
#define _REGEX_H 1				/* this should stop Apache from loading the system version of regex.h */
#endif
#ifndef _REGEX_H_
#define _REGEX_H_ 1
#endif
#ifndef _RX_H
#define _RX_H 1				  	/* Try defining these for Linux to	*/
#endif
#ifndef __REGEXP_LIBRARY_H__
#define __REGEXP_LIBRARY_H__ 1 	/* avoid Apache including regex.h	*/
#endif
#ifndef _H_REGEX
#define _H_REGEX 1              /* This one is for AIX */
#endif

#if STDC_HEADERS
# include <string.h>
#else
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n)	bcopy((s), (d), (n))
#  define memmove(d, s, n)	bcopy ((s), (d), (n))
# endif
#endif

#include "safe_mode.h"

#ifndef HAVE_STRERROR
extern char *strerror(int);
#endif

#include "fopen-wrappers.h"

#include "mod_php3.h"  /* the php3_ini structure comes from here */

#if APACHE /* apache httpd */
# if HAVE_AP_CONFIG_H
#include "ap_config_auto.h"
#ifdef RHAPSODY
#undef HAVE_SNPRINTF
#endif
#include "ap_config.h"
#ifdef RHAPSODY
#undef HAVE_SNPRINTF
#define HAVE_SNPRINTF 1
#endif
# endif
# if HAVE_OLD_COMPAT_H
#include "compat.h"
# endif
# if HAVE_AP_COMPAT_H
#include "ap_compat.h"
# endif
#include "httpd.h"
#include "http_main.h"
#include "http_core.h"
#include "http_request.h"
#include "http_protocol.h"
#include "http_config.h"
#include "http_log.h"
#define BLOCK_INTERRUPTIONS block_alarms()
#define UNBLOCK_INTERRUPTIONS unblock_alarms()
extern request_rec *php3_rqst;
#endif

#if HAVE_PWD_H
# if WIN32
#include "win32/pwd.h"
#include "win32/param.h"
# else
#include <pwd.h>
#include <sys/param.h>
# endif
#endif
#if CGI_BINARY /* CGI version */
#define BLOCK_INTERRUPTIONS
#define UNBLOCK_INTERRUPTIONS
#endif

#if HAVE_LIMITS_H
#include <limits.h>
#endif

#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif

#ifndef LONG_MIN
#define LONG_MIN (- LONG_MAX - 1)
#endif

#if FHTTPD /* fhttpd */
#define BLOCK_INTERRUPTIONS
#define UNBLOCK_INTERRUPTIONS
#endif

#ifndef HAVE_SNPRINTF
#define snprintf ap_snprintf
#define vsnprintf ap_vsnprintf
extern int ap_snprintf(char *, size_t, const char *, ...);
extern int ap_vsnprintf(char *, size_t, const char *, va_list);
#endif

extern PHPAPI char *empty_string;
extern PHPAPI char *undefined_variable_string;

#define EXEC_INPUT_BUF 4096

#if APACHE
extern PHPAPI void php3_puts(const char *s);
extern PHPAPI void php3_putc(char c);
extern PHPAPI int php3_write(const void *a, int n);
# define PUTS(s) php3_puts(s)
# define PUTC(c) php3_putc(c)
# define PHPWRITE(a,n) php3_write(a,n)
/* thies@digicol.de 990119 # define PHPWRITE(a,n) rwrite((a),(n),GLOBAL(php3_rqst)) */
#endif

#if FHTTPD

#include <servproc.h>

#ifndef IDLE_TIMEOUT
#define IDLE_TIMEOUT 120
#endif
#ifndef SIGACTARGS
#define SIGACTARGS int n
#endif

extern struct http_server *server;
extern struct request *req;
extern struct httpresponse *response;
extern int global_alarmflag;
extern int idle_timeout;
extern int exit_status;
extern int headermade;
extern char **currentheader;
extern char *headerfirstline;
extern int headerlines;

void alarmhandler(SIGACTARGS);
void setalarm(int t);
int checkinput(int h);

extern PHPAPI void php3_fhttpd_free_header(void);
extern PHPAPI void php3_fhttpd_puts_header(char *s);
extern PHPAPI void php3_fhttpd_puts(char *s);
extern PHPAPI void php3_fhttpd_putc(char c);
extern PHPAPI int php3_fhttpd_write(char *a,int n);
# define PUTS(s) php3_fhttpd_puts(s)
# define PUTC(c) php3_fhttpd_putc(c)
# define PHPWRITE(a,n) php3_fhttpd_write((a),(n))
#endif

#if CGI_BINARY
extern PHPAPI void php3_puts(const char *s);
extern PHPAPI void php3_putc(char c);
extern PHPAPI int php3_write(const void *a, int n);
/* to test user abort in a cgi, make CGI_CHECK_ABOUT = 1 */
#define CGI_CHECK_ABORT 0
# if CGI_CHECK_ABORT
#  define PUTS(a) php3_puts(a)
#  define PUTC(a) php3_putc(a)
#  define PHPWRITE(a,n) php3_write((a),(n))
# else
#  define PUTS(a) fputs(a,stdout)
#  define PUTC(a) fputc(a,stdout)
#  define PHPWRITE(a,n) fwrite((a),1,(n),stdout)
# endif
#endif

#define E_ERROR 0x1
#define E_WARNING 0x2
#define E_PARSE 0x4
#define E_NOTICE 0x8
#define E_CORE_ERROR 0x10
#define E_CORE_WARNING 0x20
#define E_ALL (E_ERROR | E_WARNING | E_PARSE | E_NOTICE | E_CORE_ERROR | E_CORE_WARNING)
#define E_CORE (E_CORE_ERROR | E_CORE_WARNING)

/* data types */
#define IS_LONG 0x1
#define IS_DOUBLE 0x2
#define IS_STRING 0x4
#define IS_ARRAY 0x8
#define IS_USER_FUNCTION 0x10
#define IS_INTERNAL_FUNCTION 0x20
#define IS_CLASS 0x40
#define IS_OBJECT 0x80
/* the following two are for the parser's internal use ONLY */
#define IS_NULL 0x100
#define IS_BC 0x200

#define VALID_FUNCTION (IS_USER_FUNCTION|IS_INTERNAL_FUNCTION)
#define IS_HASH (IS_ARRAY | IS_OBJECT)

/* general definitions */
#undef SUCCESS
#undef FAILURE
#define SUCCESS 0
#define FAILURE -1				/* this MUST stay a negative number, or it may effect functions! */

#define DONT_FREE 0
#define DO_FREE 1

#define PHP3_MIME_TYPE "application/x-httpd-php3"

/* macros */
#undef MIN
#undef MAX
#undef COPY_STRING
#define DO_OR_DIE(retvalue) if (retvalue==FAILURE) { return FAILURE; }
#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)<(b))?(a):(b))
#define STR_FREE(ptr) if (ptr && ptr!=empty_string && ptr!=undefined_variable_string) { efree(ptr); }
#define COPY_STRING(yy)   (yy).value.str.val = (char *) estrndup((yy).value.str.val,(yy).value.str.len)

#define DO_NOTHING 0
#define DO_BREAK 1
#define DO_CONTINUE 2

#ifndef MAXPATHLEN
#define MAXPATHLEN 256    /* Should be safe for any weird systems that do not define it */
#endif

typedef union {
	unsigned char switched;
	unsigned char included;
	unsigned short function_call_type;
	unsigned char array_write;
	unsigned char error_reporting;
} control_structure_data;		/* control-structure data */


typedef struct _pval_struct pval;
#define YYSTYPE pval

#define INTERNAL_FUNCTION_PARAMETERS HashTable *ht, pval *return_value, HashTable *list, HashTable *plist
#define INTERNAL_FUNCTION_PARAM_PASSTHRU ht, return_value, list, plist

#define PHP_NAMED_FUNCTION(name) void name(INTERNAL_FUNCTION_PARAMETERS)
#define PHP_FUNCTION(name) PHP_NAMED_FUNCTION(php3_##name)

#define PHP_NAMED_FE(php_name, name, arg_types) { #php_name, name, arg_types },
#define PHP_FE(name, arg_types) PHP_NAMED_FE(name, php3_##name, arg_types)

typedef union {
	long lval;					/* long value */
	double dval;				/* double value */
	struct {
		char *val;
		int len;
	} str;
	char chval;					/* char value */
	HashTable *ht;				/* hash table value */
	struct {
		union {
			void (*internal)(INTERNAL_FUNCTION_PARAMETERS);
			HashTable *statics;  /* static symbol table for user functions */
		} addr;
		unsigned char *arg_types;
	} func;
		
	struct {
		pval *pvalue;  /* used for implementation of multi-dimensional arrays */
		int string_offset;
	} varptr;
} pvalue_value;


struct _pval_struct {
	/* Variable information */
	unsigned short type;		/* active type */

	/* Control structures */
	control_structure_data cs_data;
	unsigned int offset;

	pvalue_value value;		/* value */
};

typedef struct {
	char *fname;
	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
	unsigned char *func_arg_types;
} function_entry;


typedef struct {
	int loop_nest_level;
	int loop_change_type;
	int loop_change_level;
	int returned;
	HashTable *symbol_table;
	HashTable *function_symbol_table;
	HashTable *hosting_function_table;
	unsigned char *func_arg_types;
	int lineno;
	char *function_name;
	unsigned short function_type;
	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
	pval *object_pointer;
} FunctionState;

/* Connection states */
#define PHP_CONNECTION_NORMAL  0
#define PHP_CONNECTION_ABORTED 1
#define PHP_CONNECTION_TIMEOUT 2


/* global variables */
extern HashTable symbol_table, function_table;
extern HashTable include_names;
extern HashTable *active_symbol_table;
extern int phplineno, current_lineno;
extern int php_connection_status;
extern int ignore_user_abort;
extern int error_reporting,tmp_error_reporting;
extern pval *data,globals;
extern FunctionState function_state;
extern int include_count;
#if (!PHP_ISAPI)
extern char **environ;
#endif
extern PHPAPI int le_index_ptr;  /* list entry type for index pointers */

extern int phplex(pval *phplval);

extern int lex_scan(pval *phplval);
extern void phperror(char *error);
extern PHPAPI void php3_error(int type, const char *format,...);
extern PHPAPI int php3_printf(const char *format,...);
extern void php3_log_err(char *log_message);
extern int Debug(char *format,...);
extern int phpparse(void);
extern int cfgparse(void);
extern void reset_scanner(void);
extern void html_putc(char c);

/* functions */
extern int include_file(pval *file,int display_source);
extern int conditional_include_file(pval *file, pval *return_offset);
extern void initialize_input_file_buffer(FILE *f);
extern void eval_string(pval *str, pval *return_offset, int display_source);
extern int end_current_file_execution(int *retval);
extern void clean_input_source_stack(void);
extern int _php3_hash_environment(void);
extern int module_startup_modules(void);

/* needed for modules only */
extern PHPAPI int php3i_get_le_fp(void);

/*from basic functions*/
extern PHPAPI int _php3_error_log(int opt_err,char *message,char *opt,char *headers);


/* configuration abstraction layer */
extern PHPAPI int cfg_get_long(char *varname, long *result);
extern PHPAPI int cfg_get_double(char *varname, double *result);
extern PHPAPI int cfg_get_string(char *varname, char **result);

extern PHPAPI php3_ini_structure php3_ini;

#include "stack.h"
#include "operators.h"
#include "token_cache.h"
#include "variables.h"
#include "constants.h"

#define RETVAL_RESOURCE(l) { return_value->type = IS_LONG; \
                         return_value->value.lval = l; }
#define RETVAL_LONG(l) { return_value->type = IS_LONG; \
                         return_value->value.lval = l; }
#define RETVAL_DOUBLE(d) { return_value->type = IS_DOUBLE; \
                           return_value->value.dval = d; }
#define RETVAL_STRING(s,duplicate) { char *__s=(s); \
			return_value->value.str.len = strlen(__s); \
			return_value->value.str.val = (duplicate?estrndup(__s,return_value->value.str.len):__s); \
			return_value->type = IS_STRING; }
#define RETVAL_STRINGL(s,l,duplicate) { char *__s=(s); int __l=l; \
			return_value->value.str.len = __l; \
			return_value->value.str.val = (duplicate?estrndup(__s,__l):__s); \
      			return_value->type = IS_STRING; }

#define RETVAL_FALSE  {var_reset(return_value);}
#define RETVAL_TRUE   RETVAL_LONG(1L)

#define RETURN_RESOURCE(l) { return_value->type = IS_LONG; \
                         return_value->value.lval = l; \
                         return; }
#define RETURN_LONG(l) { return_value->type = IS_LONG; \
                         return_value->value.lval = l; \
                         return; }
#define RETURN_DOUBLE(d) { return_value->type = IS_DOUBLE; \
                           return_value->value.dval = d; \
                           return; }
#define RETURN_STRING(s,duplicate) { char *__s=(s); \
			return_value->value.str.len = strlen(__s); \
			return_value->value.str.val = (duplicate?estrndup(__s,return_value->value.str.len):__s); \
			return_value->type = IS_STRING; \
			return; }
#define RETURN_STRINGL(s,l,duplicate) { char *__s=(s); int __l=l; \
			return_value->value.str.len = __l; \
			return_value->value.str.val = (duplicate?estrndup(__s,__l):__s); \
      			return_value->type = IS_STRING; \
			return; }

/*#define RETURN_NEG    RETURN_LONG(-1L) */
#define RETURN_ZERO   RETURN_LONG(0L)
#define RETURN_FALSE  {var_reset(return_value); return;}
#define RETURN_TRUE   RETURN_LONG(1L)

#define SET_VAR_STRING(n,v) { \
                           { \
                               pval var; \
							   char *str=v; /* prevent 'v' from being evaluated more than once */ \
                               var.value.str.val = (str); \
                               var.value.str.len = strlen((str)); \
                               var.type = IS_STRING; \
                               _php3_hash_update(&GLOBAL(symbol_table), (n), strlen((n))+1, &var, sizeof(pval),NULL); \
                           } \
                       }
#define SET_VAR_STRINGL(n,v,l) { \
                           { \
                               pval var; \
                               char *name=(n); \
                               var.value.str.val = (v); \
                               var.value.str.len = (l); \
                               var.type = IS_STRING; \
                               _php3_hash_update(&GLOBAL(symbol_table), name, strlen(name)+1, &var, sizeof(pval),NULL); \
                           } \
                       }
#define SET_VAR_LONG(n,v) { \
                           { \
                               pval var; \
                               var.value.lval = (v); \
                               var.type = IS_LONG; \
                               _php3_hash_update(&GLOBAL(symbol_table), (n), strlen((n))+1, &var, sizeof(pval),NULL); \
                           } \
                       }
#define SET_VAR_DOUBLE(n,v) { \
                           { \
                               pval var; \
                               var.value.dval = (v); \
                               var.type = IS_DOUBLE; \
                               _php3_hash_update(&GLOBAL(symbol_table)), (n), strlen((n))+1, &var, sizeof(pval),NULL); \
                           } \
                       }

extern int yylineno;
extern void phprestart(FILE *input_file);

#if HAVE_DMALLOC
#include <dmalloc.h>
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
