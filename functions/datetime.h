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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: datetime.h,v 1.5 1998/01/17 21:24:00 andi Exp $ */

#ifndef _DATETIME_H
#define _DATETIME_H

extern void php3_time(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_mktime(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_date(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_gmdate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getdate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_checkdate(INTERNAL_FUNCTION_PARAMETERS);

extern char *php3_std_date(time_t t);

#endif /* _DATETIME_H */
