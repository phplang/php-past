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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */


/* $Id: reg.h,v 1.9 1997/12/31 15:56:48 rasmus Exp $ */

#ifndef _REG_H
#define _REG_H

extern php3_module_entry regexp_module_entry;
#define regexp_module_ptr &regexp_module_entry

extern char *_php3_regreplace(const char *pattern, const char *replace, const char *string, int icase, int extended);

extern void php3_ereg(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_eregi(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_eregireplace(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_eregreplace(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_split(INTERNAL_FUNCTION_PARAMETERS);
extern PHPAPI void php3_sql_regcase(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _REG_H */
