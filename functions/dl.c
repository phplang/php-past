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

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "dl.h"

#if HAVE_LIBDL
#include <stdlib.h>
#include <stdio.h>
#if MSVC5
#include <windows.h>
#define dlclose FreeLibrary
#define dlopen(a,b) LoadLibrary(a)
#define dlsym GetProcAddress
#else
#include <dlfcn.h>
#endif
#ifndef RTLD_LAZY
# define RTLD_LAZY 1    /* Solaris 1, FreeBSD's (2.1.7.1 and older) */
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#if MSVC5
#include "win32/param.h"
#include "win32/winutil.h"
#else
#include <sys/param.h>
#endif

function_entry dl_functions[] = {
	{"dl",		dl,		NULL},
	{NULL, NULL, NULL}
};


php3_module_entry dl_module_entry = {
	"PHP_DL", dl_functions, NULL, NULL, NULL, NULL, php3_info_dl, 0, 0, 0, NULL
};

#endif


void dl(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *file;

	/* obtain arguments */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(file);

	if (php3_ini.safe_mode) {
		php3_error(E_ERROR, "Dynamically loaded extensions aren't allowed when running in SAFE MODE.");
	} else {
		php3_dl(file,MODULE_TEMPORARY,return_value);
	}
}


#if HAVE_LIBDL

void php3_dl(YYSTYPE *file,int type,YYSTYPE *return_value)
{
	void *handle;
	char libpath[MAXPATHLEN + 1];
	php3_module_entry *module_entry,*tmp;
	php3_module_entry *(*get_module)(void);
	TLS_VARS;
	
	if (cfg_get_string("extension_dir",&php3_ini.extension_dir)==SUCCESS && php3_ini.extension_dir){
		int extension_dir_len = strlen(php3_ini.extension_dir);

		if (php3_ini.extension_dir[extension_dir_len-1]=='/' || php3_ini.extension_dir[extension_dir_len-1]=='\\') {
			sprintf(libpath,"%s%s",php3_ini.extension_dir,file->value.strval);  /* SAFE */
		} else {
			sprintf(libpath,"%s/%s",php3_ini.extension_dir,file->value.strval);  /* SAFE */
		}
	} else {
		sprintf(libpath,"%s",file->value.strval);  /* SAFE */
	}

	/* load dynamic symbol */
	handle = dlopen(libpath, RTLD_LAZY);
	if (!handle) {
#if MSVC5
		php3_error(E_ERROR,"Unable to load dynamic library '%s'<br>\n%s",libpath,php3_win_err());
#else
		php3_error(E_ERROR,"Unable to load dynamic library '%s' - %s",libpath,dlerror());
#endif
		RETURN_FALSE;
	}
	get_module = (php3_module_entry *(*)(void)) dlsym(handle, "get_module");
	
	if (!get_module) {
		dlclose(handle);
		php3_error(E_CORE_WARNING,"Invalid library (maybe not a PHP3 library) '%s' ",file->value.strval);
		RETURN_FALSE;
	}
	module_entry = get_module();
	if (module_entry->module_startup_func) {
		if (module_entry->module_startup_func(type)==FAILURE) {
			php3_error(E_CORE_WARNING,"%s:  Unable to initialize module",module_entry->name);
			dlclose(handle);
			RETURN_FALSE;
		}
	}
	module_entry->type = type;
	register_module(module_entry);


	if (module_entry->request_startup_func) {
		if (module_entry->request_startup_func(type)) {
			php3_error(E_CORE_WARNING,"%s:  Unable to initialize module",module_entry->name);
			dlclose(handle);
			RETURN_FALSE;
		}
	}
	
	/* update the .request_started property... */
	if (hash_find(&GLOBAL(module_registry),module_entry->name,strlen(module_entry->name)+1,(void **) &tmp)==FAILURE) {
		php3_error(E_ERROR,"%s:  Loaded module got lost",module_entry->name);
		RETURN_FALSE;
	}
	tmp->request_started=1;
	tmp->handle = handle;
	
	RETURN_TRUE;
}

void php3_info_dl(void){
	TLS_VARS;
	PUTS("Dynamic Library support enabled.\n");
}

#else

void php3_dl(YYSTYPE *file,int type,YYSTYPE *return_value)
{
	php3_error(E_WARNING,"Cannot dynamically load %s - dynamic modules are not supported",file->value.strval);
	RETURN_FALSE;
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
