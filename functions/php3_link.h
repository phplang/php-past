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
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id: php3_link.h,v 1.2 1997/12/31 15:56:42 rasmus Exp $ */
#ifndef _PHP3_LINK_H
#define _PHP3_LINK_H

extern void php3_link(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_unlink(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_readlink(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_linkinfo(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_symlink(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _PHP3_LINK_H */
