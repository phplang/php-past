/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */


/* $Id: phpdir.h,v 1.7 2000/01/01 04:44:10 sas Exp $ */

#ifndef _PHPDIR_H
#define _PHPDIR_H

/* directory functions */
extern void php3_opendir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_closedir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_chdir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rewinddir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_readdir(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getdir(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _PHPDIR_H */
