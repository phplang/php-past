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

/* $Id: file.h,v 1.15 1998/02/01 01:37:39 jim Exp $ */

#ifndef _FILE_H
#define _FILE_H

#ifndef INITFUNCARG
#include "modules.h"
#endif

extern php3_module_entry php3_file_module_entry;
#define php3_file_module_ptr &php3_file_module_entry

extern int php3_minit_file(INITFUNCARG);
extern void php3_tempnam(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fopen(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fclose(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_popen(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pclose(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_feof(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fgetc(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fgets(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fgetss(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fputs(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rewind(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ftell(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fseek(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_mkdir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rmdir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fpassthru(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_readfile(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_fileumask(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rename(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_file_copy(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_file(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _FILE_H */
