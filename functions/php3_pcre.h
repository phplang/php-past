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
   | Authors: Andrey Zmievski <andrey@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id: php3_pcre.h,v 1.8 2000/01/09 10:35:13 fmk Exp $ */

#ifndef _PHP_PCRE_H
#define _PHP_PCRE_H

#if HAVE_PCRE
#if COMPILE_DL
#include "dl/phpdl.h"
#endif

#include "modules.h"
#include "internal_functions.h"

#include "pcrelib/pcre.h"
#if HAVE_LOCALE_H
#include <locale.h>
#endif

extern void php_info_pcre(void);
extern int php_minit_pcre(INIT_FUNC_ARGS);
extern int php_mshutdown_pcre(SHUTDOWN_FUNC_ARGS);
extern int php_rinit_pcre(INIT_FUNC_ARGS);

PHP_FUNCTION(preg_match);
PHP_FUNCTION(preg_match_all);
PHP_FUNCTION(preg_replace);
PHP_FUNCTION(preg_split);
PHP_FUNCTION(preg_quote);

extern php3_module_entry pcre_module_entry;
#define pcre_module_ptr &pcre_module_entry

typedef struct {
        pcre *re;
        pcre_extra *extra;
#if HAVE_SETLOCALE
	char *locale;
	unsigned const char *tables;
#endif
} pcre_cache_entry;

extern HashTable pcre_cache;

#else

#define pcre_module_ptr NULL

#endif /* HAVE_PCRE */

#endif /* _PHP_PCRE_H */
