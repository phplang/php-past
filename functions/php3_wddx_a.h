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

/* $Id: php3_wddx_a.h,v 1.3 2000/01/01 04:44:10 sas Exp $ */

#ifndef _WDDX_A_H
#define _WDDX_A_H

#if HAVE_LIBEXPAT

#if RAW_XML_INCLUDEPATH
#include <xmlparse.h>
#else
#include <xml/xmlparse.h>
#endif

extern int php3_minit_wddx(INIT_FUNC_ARGS);

extern php3_module_entry wddx_module_entry_a;
#define wddx_module_ptr_a &wddx_module_entry_a

PHP_FUNCTION(wddx_serialize_value);
PHP_FUNCTION(wddx_serialize_vars);
PHP_FUNCTION(wddx_packet_start);
PHP_FUNCTION(wddx_packet_end);
PHP_FUNCTION(wddx_add_vars);
PHP_FUNCTION(wddx_deserialize);

#else

#define wddx_module_ptr_a NULL

#endif /* HAVE_LIBEXPAT */

#endif /* !_WDDX_A_H */
