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
   | Authors: Timothy Whitfield <timothy@ametro.net>                      |
   | on behalf of American MetroComm Internet Services
   +----------------------------------------------------------------------+
 */
#ifndef _php3_mckcrypt_h
#define _php3_mckcrypt_h

/* MCKCRYPT.H - header file for MCKCRYPT.C
 */
#if HAVE_MCK
extern php3_module_entry mckcrypt_module_entry;
#define mckcrypt_module_ptr &mckcrypt_module_entry

extern void php3_mckencr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_mckdecr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_mckbase64_encode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_mckbase64_decode(INTERNAL_FUNCTION_PARAMETERS);
#else

#define mckcrypt_module_ptr NULL

#endif

#endif
