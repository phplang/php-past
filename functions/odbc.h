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
   | Authors: Brian Schaffner <brian@tool.net>                            |
   +----------------------------------------------------------------------+
 */


/* $Id: odbc.h,v 1.6 1997/12/31 15:56:38 rasmus Exp $ */

#ifndef _ODBC_H
#define _ODBC_H

#if HAVE_ODBC|HAVE_IODBC && !HAVE_UODBC
extern php3_module_entry odbc_module_entry;
#define odbc_module_ptr &odbc_module_entry

extern void ODBCconnect(INTERNAL_FUNCTION_PARAMETERS);
extern void ODBCdisconnect(INTERNAL_FUNCTION_PARAMETERS);
extern void ODBCfetch(INTERNAL_FUNCTION_PARAMETERS);
extern void ODBCrowcount(INTERNAL_FUNCTION_PARAMETERS);
extern void ODBCexecdirect(INTERNAL_FUNCTION_PARAMETERS);
extern void ODBCfree(INTERNAL_FUNCTION_PARAMETERS);
extern void ODBCgetdata(INTERNAL_FUNCTION_PARAMETERS);
extern int php3_minit_odbc(INITFUNCARG);
extern int php3_mshutdown_odbc(void);
extern int php3_rinit_odbc(INITFUNCARG);
#else
#define odbc_module_ptr NULL
#endif
#endif /* _ODBC_H */
