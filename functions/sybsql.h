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
   | Authors: MA_Muquit@fccc.edu                                          |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
*/

/* $Id: sybsql.h,v 1.4 1997/12/31 15:56:51 rasmus Exp $ */

#ifndef _SYBSQL_H
#define _SYBSQL_H

#if COMPILE_DL
#undef HAVE_SYABSE
#define HAVE_SYBASE 1
#endif

#if HAVE_SYBASE

extern php3_module_entry sybase_old_module_entry;
#define sybase_old_module_ptr &sybase_old_module_entry

void php3_sybsql_connect(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_pconnect(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_dbuse(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_query(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_checkconnect(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_isrow(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_print_a_row(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_nextrow(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_numrows(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_print_all_rows(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_result(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_seek(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_numfields(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_fieldname(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_result_all(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_getfield(INTERNAL_FUNCTION_PARAMETERS);
void php3_sybsql_exit(INTERNAL_FUNCTION_PARAMETERS);
int php3_rinit_sybsql(INITFUNCARG);
extern void php3_info_sybsql(void);

#else

#define sybase_old_module_ptr NULL

#endif

#endif /* _SYBSQL_H */
