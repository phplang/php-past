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
 */


#ifndef _MODULES_H
#define _MODULES_H

#define INITFUNCARG int type

#define MODULE_PERSISTENT 1
#define MODULE_TEMPORARY 2

typedef struct {
	char *name;
	function_entry *functions;
	int (*module_startup_func)(INITFUNCARG);
	int (*module_shutdown_func)(void);
	int (*request_startup_func)(INITFUNCARG);
	int (*request_shutdown_func)(void);
	void (*info_func)(void);
	int request_started,module_started;
	unsigned char type;
	void *handle;
} php3_module_entry;


typedef struct {
	char *name;
	php3_module_entry *module;
} php3_builtin_module;


/*FIXME does this need to be thread safe?*/
#ifndef THREAD_SAFE
extern HashTable module_registry;
#endif

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
extern int php3_init_debugger(INITFUNCARG);
extern int php3_shutdown_debugger(void);
extern void php3_debugger_error(char *message, int type, char *filename, int lineno);




#endif
