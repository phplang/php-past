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
   |          Shane Caraveo <shane@caraveo.com>                           |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: dl.h,v 1.15 1997/12/31 15:56:23 rasmus Exp $ */

#ifndef _DL_H
#define _DL_H

extern void php3_dl(YYSTYPE *file,int type,YYSTYPE *return_value);


#if HAVE_LIBDL

extern php3_module_entry dl_module_entry;
#define dl_module_ptr &dl_module_entry

/* dynamic loading functions */
extern void dl(INTERNAL_FUNCTION_PARAMETERS);
extern int php3_minit_dl(INITFUNCARG);
extern int php3_mshutdown_dl(void);
extern int php3_rshutdown_dl(void);
extern void php3_info_dl(void);

#else

#define dl_module_ptr NULL

#endif


#endif /* _DL_H */
