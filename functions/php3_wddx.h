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
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

/* $Id: php3_wddx.h,v 1.4 2000/07/10 10:15:36 sas Exp $ */

#ifndef _WDDX_H
#define _WDDX_H

#if HAVE_LIBEXPAT

#if RAW_XML_INCLUDEPATH
#include <xmlparse.h>
#else
#include <xml/xmlparse.h>
#endif

extern php3_module_entry wddx_module_entry;
#define wddx_module_ptr &wddx_module_entry

PHP_FUNCTION(wddx_serialize);
PHP_FUNCTION(wddx_unserialize);

#else

#define wddx_module_ptr NULL

#endif /* HAVE_LIBEXPAT */

#endif /* !_WDDX_H */
