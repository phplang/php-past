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
/* $Id: php3_snmp.h,v 1.3 1998/01/18 20:45:04 shane Exp $ */
#ifndef _PHP3_SNMP_H
#define _PHP3_SNMP_H

#if COMPILE_DL
#undef HAVE_SNMP
#define HAVE_SNMP 1
#endif
#ifndef DLEXPORT
#define DLEXPORT
#endif

#if HAVE_SNMP

extern php3_module_entry snmp_module_entry;
#define snmp_module_ptr &snmp_module_entry

extern DLEXPORT void php3_snmpget(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_snmpwalk(INTERNAL_FUNCTION_PARAMETERS);

#else

#define snmp_module_ptr NULL

#endif /* HAVE_SNMP */

#endif  /* _PHP3_SNMP_H */
