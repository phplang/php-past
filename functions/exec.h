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


/* $Id: exec.h,v 1.4 1998/01/30 06:41:57 rasmus Exp $ */

#ifndef _EXEC_H
#define _EXEC_H

extern void php3_system(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_exec(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_escapeshellcmd(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_passthru(INTERNAL_FUNCTION_PARAMETERS);

char *_php3_escapeshellcmd(char *);
#endif /* _EXEC_H */
