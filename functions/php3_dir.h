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


/* $Id: php3_dir.h,v 1.3 1997/12/31 15:56:40 rasmus Exp $ */

#ifndef _PHP3_DIR_H
#define _PHP3_DIR_H
extern php3_module_entry php3_dir_module_entry;
#define php3_dir_module_ptr &php3_dir_module_entry

/* directory functions */
extern int php3_minit_dir(INITFUNCARG);
extern void php3_opendir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_closedir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_chdir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rewinddir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_readdir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getdir(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _PHP3_DIR_H */
