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

/* $Id: pack.h,v 1.2 1998/02/05 00:02:49 willer Exp $ */

#ifndef _PACK_H
#define _PACK_H

#ifndef INITFUNCARG
#include "modules.h"
#endif

extern php3_module_entry pack_module_entry;
#define pack_module_ptr &pack_module_entry

extern int php3_minit_pack(INITFUNCARG);
extern void php3_pack(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_unpack(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _PACK_H */
