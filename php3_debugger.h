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
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   +----------------------------------------------------------------------+
 */

/* $Id: php3_debugger.h,v 1.13 2000/01/01 04:44:07 sas Exp $ */

#ifndef _PHP3_DEBUGGER_H
#define _PHP3_DEBUGGER_H

#if PHP_DEBUGGER

extern php3_module_entry debugger_module_entry;
#define debugger_module_ptr &debugger_module_entry

#if WIN32|WINNT
typedef long pid_t;
#endif

typedef struct {
    int debug_socket;
    char *myhostname;
    char *debugger_host;
    long debugger_port;
    long debugger_default;
    char *currenttime;
    pid_t mypid;
} debugger_module;

void php3_debugger_on(INTERNAL_FUNCTION_PARAMETERS);
void php3_debugger_off(INTERNAL_FUNCTION_PARAMETERS);
int php3_minit_debugger(INIT_FUNC_ARGS);
int php3_mshutdown_debugger(void);
int php3_rinit_debugger(INIT_FUNC_ARGS);
int php3_rshutdown_debugger(void);
int php3_start_debugger(char *);
int php3_stop_debugger(void);
void send_debug_info(void);
void php3_debugger_frame_location(FunctionState *, int);


#else /* no PHP_DEBUGGER */

#define debugger_module_ptr NULL

#endif /* PHP_DEBUGGER */

#endif /* _PHP3_DEBUGGER_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
