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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */

/* $Id: parser.h,v 1.232 1998/02/01 23:59:34 shane Exp $ */


#ifndef _PARSER_H
#define _PARSER_H

#define CGI_BINARY (!APACHE && !USE_SAPI)

#include "version.h"

#if MSVC5
#include "config.w32.h"
#include "win95nt.h"
# if defined(COMPILE_DL)
# define PHPAPI __declspec(dllimport) 
# else
# define PHPAPI __declspec(dllexport) 
# endif
#else
# include "config.h"
# define PHPAPI
# define THREAD_LS
#endif
#include "request_info.h"


/*Thread Safety*/
#if THREAD_SAFE
#define GLOBAL(a) php3_globals->a
#define STATIC GLOBAL
#define TLS_VARS \
	php3_globals_struct *php3_globals; \
	php3_globals = TlsGetValue(TlsIndex);
#define CREATE_MUTEX(a,b) a = CreateMutex (NULL, FALSE, b);
#define SET_MUTEX(a) WaitForSingleObject( a, INFINITE );
#define FREE_MUTEX(a) ReleaseMutex(a);

/*redirect variables to the flex structure*/
#if !defined(YY_BUFFER_NEW) && !defined(COMPILE_DL)
#include "FlexSafe.h"
#endif

#define INLINE_TLS ,struct php3_global_struct *php3_globals
#define INLINE_TLS_VOID struct php3_global_struct *php3_globals
#define _INLINE_TLS ,php3_globals
#define _INLINE_TLS_VOID php3_globals

#else
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

#endif

/*
 * Then the ODBC support can use both iodbc and Solid,
 * uncomment this.
 * #define HAVE_ODBC (HAVE_IODBC|HAVE_SOLID)
 */

#include <stdlib.h>
#include <ctype.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STDARG_H
#include <stdarg.h>
#else   
# if HAVE_SYS_VARARGS_H
# include <sys/varargs.h>
# endif 
#endif 


#include "hash.h"
#include "alloc.h"
#if REGEX
#include "regex/regex.h"
#define _REGEX_H 1				/* this should stop Apache from loading the system version of regex.h */
#define _RX_H 1				  		/* Try defining these for Linux to	*/
#define __REGEXP_LIBRARY_H__ 1 		/* avoid Apache including regex.h	*/
#else
#include <regex.h>
#endif
#if STDC_HEADERS
#include <string.h>
#else
# ifndef HAVE_MEMCPY
# define memcpy(d, s, n)         bcopy((s), (d), (n))
# endif
#endif
#include "safe_mode.h"

#ifndef HAVE_STRERROR
extern char *strerror(int);
#endif

#include "fopen-wrappers.h"

#include "mod_php3.h"  /* the php3_ini structure comes from here */

#if APACHE /* apache httpd */
#include "httpd.h"
#include "http_main.h"
#include "http_core.h"
#include "http_request.h"
#include "http_protocol.h"
#include "http_config.h"
#include "http_log.h"
#define BLOCK_INTERRUPTIONS block_alarms()
#define UNBLOCK_INTERRUPTIONS unblock_alarms()
#ifndef THREAD_SAFE
extern request_rec *php3_rqst;
#endif
#endif

#if CGI_BINARY /* CGI version */
#define BLOCK_INTERRUPTIONS
#define UNBLOCK_INTERRUPTIONS
#endif


#if (!HAVE_SNPRINTF)
#define snprintf ap_snprintf
#define vsnprintf ap_vsnprintf
extern int ap_snprintf(char *, size_t, const char *, ...);
extern int ap_vsnprintf(char *, size_t, const char *, va_list);
#endif

extern PHPAPI char *empty_string;
extern PHPAPI char *undefined_variable_string;

#define EXEC_INPUT_BUF 4096
#define PHP_DOCUMENT_ROOT "/webshare/wwwroot"
#define PHP_USER_DIR "public_html"

#if DEBUG
#ifdef inline
#undef inline
#endif
#define inline
#endif

#if APACHE
extern PHPAPI void php3_apache_puts(char *s);
extern PHPAPI void php3_apache_putc(char c);
# if !defined(COMPILE_DL)
# define PUTS(s) php3_apache_puts(s)
# define PUTC(c) php3_apache_putc(c)
# define PHPWRITE(a,n) rwrite((a),(n),GLOBAL(php3_rqst))
# endif
#endif

#if CGI_BINARY
# if !defined(COMPILE_DL)
# define PUTS(a) fputs((a),stdout)
# define PUTC(a) fputc((a),stdout)
# define PHPWRITE(a,n) fwrite((a),(n),1,stdout)
# endif
#endif

#if defined(THREAD_SAFE) && defined(COMPILE_DL)
#define PUTS(a) php3_printf("%s",a)
#define PUTC(a) PUTS(a)
#define PHPWRITE(a,n) php3_write((a),(n))
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
#define IS_EMPTY 0x10
#define IS_USER_FUNCTION 0x20
#define IS_INTERNAL_FUNCTION 0x40
#define IS_TEMPORARY_INTERNAL_FUNCTION 0x80
#define IS_UNSUPPORTED_FUNCTION 0x100
#define IS_CLASS 0x200
#define IS_OBJECT 0x400
#define IS_NULL 0x800

#define VALID_FUNCTION (IS_USER_FUNCTION|IS_INTERNAL_FUNCTION|IS_TEMPORARY_INTERNAL_FUNCTION)
#define IS_HASH (IS_ARRAY | IS_OBJECT)

/* general definitions */
#undef SUCCESS
#undef FAILURE
#define SUCCESS 0
#define FAILURE -1				/* this MUST stay a negative number, or it may effect functions! */

#define PHP3_MIME_TYPE "application/x-httpd-php3"

/* macros */
#define DO_OR_DIE(retvalue) if (retvalue==FAILURE) { return FAILURE; }
#define MAX(a,b)  (((a)>(b))?(a):(b))
#define MIN(a,b)  (((a)<(b))?(a):(b))
#define STR_FREE(ptr) if (ptr && ptr!=empty_string && ptr!=undefined_variable_string) { efree(ptr); }
#define COPY_STRING(yy)   (yy).value.strval = (char *) estrndup((yy).value.strval,(yy).strlen)

#define DO_NOTHING 0
#define DO_BREAK 1
#define DO_CONTINUE 2


typedef union {
	unsigned char switched;
	unsigned char included;
	unsigned short function_call_type;
	unsigned char array_write;
	unsigned char error_reporting;
} control_structure_data;		/* control-structure data */


typedef struct _yystype_struct _yystype;
#define YYSTYPE _yystype

#define INTERNAL_FUNCTION_PARAMETERS HashTable *ht, YYSTYPE *return_value, HashTable *list, HashTable *plist
#define INTERNAL_FUNCTION_PARAM_PASSTHRU ht, return_value, list, plist

typedef union {
	long lval;					/* long value */
	double dval;				/* double value */
	char *strval;				/* string value */
	char chval;					/* char value */
	HashTable *ht;				/* hash table value */
	void (*internal_function)(INTERNAL_FUNCTION_PARAMETERS);
	void *yystype_ptr;  /* used for implementation of multi-dimensional arrays */
} yystype_value;

struct _yystype_struct {
	/* Variable information */
	unsigned short type;		/* active type */

	/* Control structures */
	control_structure_data cs_data;
	unsigned int offset;

	unsigned char *func_arg_types;  /* optionally used to force passing by reference */

	yystype_value value;		/* value */
	int strlen;		/* string length */
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
	unsigned char *func_arg_types;
	int lineno;
	char *function_name;
	unsigned short function_type;
	void (*handler)(INTERNAL_FUNCTION_PARAMETERS);
} FunctionState;



/* global variables */
#ifndef THREAD_SAFE
extern HashTable symbol_table, function_table;
extern HashTable include_names;
extern HashTable *active_symbol_table;
extern int phplineno, current_lineno;
extern int error_reporting,tmp_error_reporting;
extern YYSTYPE *data,globals;
extern FunctionState function_state;
extern int include_count;
#endif
extern PHPAPI int le_index_ptr;  /* list entry type for index pointers */

#ifndef THREAD_SAFE
extern int phplex(YYSTYPE *phplval);
#endif

extern int lex_scan(YYSTYPE *phplval);
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
#ifndef THREAD_SAFE
extern int include_file(YYSTYPE *file,int display_source);
extern int conditional_include_file(YYSTYPE *file, YYSTYPE *return_offset);
extern void initialize_input_file_buffer(FILE *f);
extern void eval_string(YYSTYPE *str, YYSTYPE *return_offset);
#endif
extern int hash_environment(void);
extern int module_startup_modules(void);


/* configuration abstraction layer */
extern PHPAPI int cfg_get_long(char *varname, long *result);
extern PHPAPI int cfg_get_double(char *varname, double *result);
extern PHPAPI int cfg_get_string(char *varname, char **result);

extern PHPAPI php3_ini_structure php3_ini;

#include "stack.h"
#include "operators.h"
#include "token_cache.h"
#include "variables.h"

#define RETVAL_LONG(l) { return_value->type = IS_LONG; \
                         return_value->value.lval = l; }
#define RETVAL_DOUBLE(d) { return_value->type = IS_DOUBLE; \
                           return_value->value.dval = d; }
#define RETVAL_STRING(s) { return_value->strlen = strlen(s); \
                           return_value->value.strval = estrndup(s,return_value->strlen); \
                           return_value->type = IS_STRING; }
#define RETVAL_STRINGL(s,l) { return_value->strlen = l; \
                              return_value->value.strval = estrndup(s,l); \
                              return_value->type = IS_STRING; }

#define RETVAL_FALSE  {var_reset(return_value);}
#define RETVAL_TRUE   RETVAL_LONG(1L)

#define RETURN_LONG(l) { return_value->type = IS_LONG; \
                         return_value->value.lval = l; \
                         return; }
#define RETURN_DOUBLE(d) { return_value->type = IS_DOUBLE; \
                           return_value->value.dval = d; \
                           return; }
#define RETURN_STRING(s) { return_value->strlen = strlen(s); \
                           return_value->value.strval = estrdup(s); \
                           return_value->type = IS_STRING; \
                           return; }
#define RETURN_STRINGL(s,l) { return_value->strlen = l; \
                              return_value->value.strval = estrndup(s,l); \
                              return_value->type = IS_STRING; \
                              return; }

/*#define RETURN_NEG    RETURN_LONG(-1L) */
#define RETURN_ONE    RETURN_LONG(1L)
#define RETURN_ZERO   RETURN_LONG(0L)
#define RETURN_FALSE  {var_reset(return_value); return;}
#define RETURN_TRUE   RETURN_LONG(1L)

#define SET_VAR_STRING(n,v) { \
                           { \
                               YYSTYPE var; \
							   char *str=v; /* prevent 'v' from being evaluated more than once */ \
                               var.value.strval = (str); \
                               var.strlen = strlen((str)); \
                               var.type = IS_STRING; \
                               hash_update(&GLOBAL(symbol_table), (n), strlen((n))+1, &var, sizeof(YYSTYPE),NULL); \
                           } \
                       }
#define SET_VAR_LONG(n,v) { \
                           { \
                               YYSTYPE var; \
                               var.value.lval = (v); \
                               var.type = IS_LONG; \
                               hash_update(&GLOBAL(symbol_table), (n), strlen((n))+1, &var, sizeof(YYSTYPE),NULL); \
                           } \
                       }
#define SET_VAR_DOUBLE(n,v) { \
                           { \
                               YYSTYPE var; \
                               var.value.dval = (v); \
                               var.type = IS_DOUBLE; \
                               hash_update(&GLOBAL(symbol_table)), (n), strlen((n))+1, &var, sizeof(YYSTYPE),NULL); \
                           } \
                       }

#ifndef THREAD_SAFE
extern int yylineno;
#endif
extern void phprestart(FILE *input_file);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
