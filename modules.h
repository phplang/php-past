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
 */

/* $Id: modules.h,v 1.32 2000/01/01 04:44:07 sas Exp $ */

#ifndef _MODULES_H
#define _MODULES_H

#define INIT_FUNC_ARGS int type, int module_number
#define SHUTDOWN_FUNC_ARGS void
#define STANDARD_MODULE_PROPERTIES 0, 0, 0, NULL, 0

#define MODULE_PERSISTENT 1
#define MODULE_TEMPORARY 2

typedef struct {
	char *name;
	function_entry *functions;
	int (*module_startup_func)(INIT_FUNC_ARGS);
	int (*module_shutdown_func)(void);
	int (*request_startup_func)(INIT_FUNC_ARGS);
	int (*request_shutdown_func)(void);
	void (*info_func)(void);
	int request_started,module_started;
	unsigned char type;
	void *handle;
	int module_number;
} php3_module_entry;


typedef struct {
	char *name;
	php3_module_entry *module;
} php3_builtin_module;

extern HashTable module_registry;

extern void module_destructor(php3_module_entry *module);
extern int module_registry_cleanup(php3_module_entry *module);
extern int module_registry_request_startup(php3_module_entry *module);


/* configuration module */
extern int php3_init_config(void);
extern int php3_shutdown_config(void);

/* environment module */
extern int php3_init_environ(void);
extern int php3_shutdown_environ(void);

/* debugger module */
extern int php3_init_debugger(INIT_FUNC_ARGS);
extern int php3_shutdown_debugger(void);
extern void php3_debugger_error(char *message, int type, char *filename, int lineno);


#ifdef NEW_CODE
/* initial work on taking php3_ini initialization out of main.c, generalizing it,
 * and making it possible for users to override it on a per-script basis.
 *
 * Will *not* make it for 3.0, aim for 3.1 or 3.01
 */
typedef struct {
	char *name;
	int type;
	void *cached_location;
	long lval;
	double dval;
	char *strval;
	int strlen;
} php3_ini_entry;

php3_ini_entry configuration_entries[] =
{
	{ "max_execution_time",		IS_LONG,	&php3_ini.max_execution_time,	30L,	0.0,		NULL,			0 },
	{ "highlight.string",		IS_STRING,	&php3_ini.highlight_string,		0L,		0.0,		"#FFFFFF",		sizeof("#FFFFFF")-1 },
	{ NULL,						0,			NULL,							0L,		0.0,		NULL,			0 }
};
#endif /* new code */

#endif
