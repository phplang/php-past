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
   | Authors: Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
 */

/* $Id: dbase.h,v 1.6 1998/01/15 22:01:08 jim Exp $ */

#ifndef _DBASE_H
#define _DBASE_H
#if DBASE
extern php3_module_entry dbase_module_entry;
#define dbase_module_ptr &dbase_module_entry

extern int php3_minit_dbase(INITFUNCARG);
extern void php3_dbase_open(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_create(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_numrecords(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_numfields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_add_record(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_get_record(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_delete_record(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dbase_pack(INTERNAL_FUNCTION_PARAMETERS);
#else
#define dbase_module_ptr NULL
#endif
#endif /* _DBASE_H */
