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
   | Authors: Kristian Koehntopp (kris@koehntopp.de)                      |
   +----------------------------------------------------------------------+
 */


/* $Id: php3_posix.h,v 1.6 2000/01/01 04:44:09 sas Exp $ */

#ifndef _PHP3_POSIX_H
#define _PHP3_POSIX_H

#if COMPILE_DL
#define HAVE_POSIX 1
#endif

#if HAVE_POSIX
#ifndef DLEXPORT
#define DLEXPORT
#endif

extern php3_module_entry posix_module_entry;
#define posix_module_ptr &posix_module_entry

extern int php3_minit_posix(INIT_FUNC_ARGS);
extern int php3_mshutdown_posix(void);
extern void php3_info_posix(void);

extern void php3_posix_kill(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_posix_getpid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getppid(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_posix_getuid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getgid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_geteuid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getegid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_setuid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_setgid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getgroups(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getlogin(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_posix_getpgrp(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_setsid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_setpgid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getpgid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getsid(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_posix_uname(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_times(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_posix_ctermid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_ttyname(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_isatty(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_posix_getcwd(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_posix_mkfifo(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getgrnam(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getgrgid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getpwnam(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_posix_getpwuid(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_posix_getrlimit(INTERNAL_FUNCTION_PARAMETERS);

typedef struct {
	int dummy;
} posix_module;

extern posix_module php3_posix_module;

#else

#define posix_module_ptr NULL

#endif

#endif /* _PHP3_POSIX_H */
