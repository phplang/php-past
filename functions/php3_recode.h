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
   | Authors: Kristian Koehntopp (kris@koehntopp.de)                      |
   +----------------------------------------------------------------------+
 */


/* $Id: php3_recode.h,v 1.4 2000/01/01 04:44:09 sas Exp $ */

#ifndef _PHP3_RECODE_H
#define _PHP3_RECODE_H

#if COMPILE_DL
#define HAVE_RECODE 1
#endif

#if HAVE_RECODE

#ifndef DLEXPORT
#define DLEXPORT
#endif

#define HAVE_BROKEN_RECODE

#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <recode.h>
#include <unistd.h>

extern php3_module_entry recode_module_entry;
#define recode_module_ptr &recode_module_entry

extern int php3_minit_recode(INIT_FUNC_ARGS);
extern int php3_mshutdown_recode(void);
extern void php3_info_recode(void);

extern void php3_recode_string(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_recode_file(INTERNAL_FUNCTION_PARAMETERS);

typedef struct {
	RECODE_OUTER   outer;
} recode_module;

extern recode_module php3_recode_module;

#else

#define recode_module_ptr NULL

#endif

#endif /* _PHP3_RECODE_H */
