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
   | Authors: Stephanie Wehner <_@r4k.net>                                |
   +----------------------------------------------------------------------+
 */
/* $Id: php3_yp.h,v 1.2 2000/01/01 04:44:10 sas Exp $ */ 

#ifndef _PHP3_YP_H
#define _PHP3_YP_H

#if COMPILE_DL
#undef HAVE_YP
#define HAVE_YP 1
#endif

#if HAVE_YP

extern php3_module_entry yp_module_entry;
#define yp_module_ptr &yp_module_entry

/* yp.c functions */
extern int php3_minit_yp(INIT_FUNC_ARGS);
extern void php3_yp_get_default_domain(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_yp_order(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_yp_master(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_yp_match(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_yp_first(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_yp_next(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_info_yp(void);

#else

#define yp_module_ptr NULL

#endif /* HAVE_YP */
#endif /* _PHP3_YP_H */
