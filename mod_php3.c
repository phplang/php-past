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
   | (with helpful hints from Dean Gaudet <dgaudet@arctic.org>            |
   +----------------------------------------------------------------------+
 */
/* $Id: mod_php3.c,v 1.55 1998/02/01 03:01:11 jim Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#include "parser.h"
#else
#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_main.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_log.h"
#endif

#include "util_script.h"

#include "mod_php3.h"


module MODULE_VAR_EXPORT php3_module;

#ifndef THREAD_SAFE
int saved_umask;
#else
#define GLOBAL(a) php3_globals->a
#define STATIC GLOBAL
#define TLS_VARS \
	php3_globals_struct *php3_globals; \
	php3_globals = TlsGetValue(TlsIndex);
#endif

#ifndef TLS_VARS
#define TLS_VARS
#endif

#ifndef GLOBAL
#define GLOBAL(x) x
#endif

extern php3_ini_structure php3_ini;  /* active config */
extern php3_ini_structure php3_ini_master;  /* master copy of config */

extern int apache_php3_module_main(request_rec * r, int fd, int display_source_mode, int preprocessed);
extern int php3_module_startup();
extern void php3_module_shutdown();

extern int tls_create(void);
extern int tls_destroy(void);
extern int tls_startup(void);
extern int tls_shutdown(void);

#if WIN32|WINNT

/* 
	we will want to change this to the apache api
	process and thread entry and exit functions
*/
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
		/* 
		   I should be loading ini vars here 
		   and doing whatever true global inits
		   need to be done
		*/
		if(!tls_startup())
			return 0;
		if(!tls_create())
			return 0;

		break;
    case DLL_THREAD_ATTACH:
		if(!tls_create())
			return 0;
		/*		if (php3_module_startup()==FAILURE) {
			return FAILURE;
		}
*/		break;
    case DLL_THREAD_DETACH:
		if(!tls_destroy())
			return 0;
/*		if (initialized) {
			php3_module_shutdown();
			return SUCCESS;
		} else {
			return FAILURE;
		}
*/		break;
    case DLL_PROCESS_DETACH:
		/*
		    close down anything down in process_attach 
		*/
		if(!tls_destroy())
			return 0;
		if(!tls_shutdown())
			return 0;
		break;
    }
    return 1;
}
#endif

void php3_save_umask()
{
	TLS_VARS;
	GLOBAL(saved_umask) = umask(077);
	umask(GLOBAL(saved_umask));
}

void php3_restore_umask()
{
	TLS_VARS;
	umask(GLOBAL(saved_umask));
}

int send_php3(request_rec *r, int display_source_mode, int preprocessed)
{
	int fd, retval;
	php3_ini_structure *conf;

	/* We don't aacept OPTIONS requests, but take everything else */
	if (r->method_number == M_OPTIONS) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}

	/* Make sure file exists */
	if (r->finfo.st_mode == 0)
		return NOT_FOUND;

	/* grab configuration settings */
	conf = (php3_ini_structure *) get_module_config(r->per_dir_config, &php3_module);
	memcpy(&php3_ini,conf,sizeof(php3_ini_structure)); /* copy to active configuration */

	/* 
	 * If PHP parser engine has been turned off with the phpEngine off directive,
	 * then decline to handle this request
	 */
	if (!conf->engine) {
		r->content_type = "text/html";
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	/* Open the file */
	/* popenf isnt working on windows, so lets just open it*/
#if WIN32|WINNT
	fd = open(r->filename, O_RDONLY,0);
#else
	if ((fd = popenf(r->pool, r->filename, O_RDONLY, 0)) == -1) {
		log_reason("file permissions deny server access", r->filename, r);
		return FORBIDDEN;
	}
#endif
	/* Apache 1.2 has a more complex mechanism for reading POST data */
#if MODULE_MAGIC_NUMBER > 19961007
	if ((retval = setup_client_block(r, REQUEST_CHUNKED_ERROR)))
		return retval;
#endif

	if (conf->last_modified) {
#if MODULE_MAGIC_NUMBER < 19970912
		if ((retval = set_last_modified(r, r->finfo.st_mtime))) {
			return retval;
		}
#else
		update_mtime (r, r->finfo.st_mtime);
		set_last_modified(r);
		set_etag(r);
#endif
	}
	/* Assume output will be HTML.  Individual scripts may change this 
	   further down the line */
	r->content_type = "text/html";

	/* Init timeout */
	hard_timeout("send", r);

	php3_save_umask();
	chdir_file(r->filename);
	add_common_vars(r);
	add_cgi_vars(r);
	apache_php3_module_main(r, fd, display_source_mode, preprocessed);

	/* Done, restore umask, turn off timeout, close file and return */
	php3_restore_umask();
	kill_timeout(r);
#if WIN32|WINNT
	close(fd);
#else
	pclosef(r->pool, fd);
#endif
	return OK;
}

int send_parsed_preprocessed_php3(request_rec * r)
{
	return send_php3(r, 0, 1);
}

int send_parsed_php3(request_rec * r)
{
	return send_php3(r, 0, 0);
}


int send_parsed_php3_source(request_rec * r)
{
	return send_php3(r, 1, 0);
}

/*
 * Create the per-directory config structure with defaults from php3_ini_master
 */
static void *php3_create_dir(pool * p, char *dummy)
{
	php3_ini_structure *new;

	php3_module_startup(); /* php3_ini_master is set up here */

	new = (php3_ini_structure *) palloc(p, sizeof(php3_ini_structure));
	memcpy(new,&php3_ini_master,sizeof(php3_ini_structure));

	return new;
}

/*
 * Merge in per-directory .conf directives
 */
static void *php3_merge_dir(pool *p, void *basev, void *addv) 
{
	php3_ini_structure *new = (php3_ini_structure *) palloc(p, sizeof(php3_ini_structure));
	php3_ini_structure *base = (php3_ini_structure *) basev;
	php3_ini_structure *add = (php3_ini_structure *) addv;

	/*
	 * Because of the way things are initialized, addv already contains
	 * the new conf structure at this point, so we can just memcpy it
	 */
	memcpy(new,add,sizeof(php3_ini_structure));

	return new;
}

#if MODULE_MAGIC_NUMBER > 19961007
const char *php3flaghandler(cmd_parms * cmd, php3_ini_structure * conf, int val)
{
#else
char *php3flaghandler(cmd_parms * cmd, php3_ini_structure * conf, int val)
{
#endif
	int c = (int) cmd->info;

	switch (c) {
		case 0:
			conf->track_errors = val;
			break;
		case 1:
			conf->magic_quotes_gpc = val;
			break;
		case 2:
			conf->magic_quotes_runtime = val;
			break;
		case 3:
			conf->short_open_tag = val;
			break;
		case 4:
			conf->safe_mode = val;
			break;
		case 5:
			conf->track_vars = val;
			break;
		case 6:
			conf->sql_safe_mode = val;
			break;
		case 7:
			conf->engine = val;
			break;
		case 8:
			conf->xbithack = val;
			break;
		case 9:
			conf->last_modified = val;
			break;
		case 10:
			conf->log_errors = val;
			break;
	}
	return NULL;
}

#if MODULE_MAGIC_NUMBER > 19961007
const char *php3take1handler(cmd_parms * cmd, php3_ini_structure * conf, char *arg)
{
#else
char *php3take1handler(cmd_parms * cmd, php3_ini_structure * conf, char *arg)
{
#endif
	int c = (int) cmd->info;

	switch (c) {
		case 0:
			conf->errors = atoi(arg);
			break;
		case 1:
			conf->doc_root = pstrdup(cmd->pool, arg);
			break;
		case 2:
			conf->user_dir = pstrdup(cmd->pool, arg);
			break;
		case 3:
			conf->safe_mode_exec_dir = pstrdup(cmd->pool, arg);
			break;
		case 4:
			conf->include_path = pstrdup(cmd->pool, arg);
			break;
		case 5:
			conf->auto_prepend_file = pstrdup(cmd->pool, arg);
			break;
		case 6:
			conf->auto_append_file = pstrdup(cmd->pool, arg);
			break;
		case 7:
			conf->upload_tmp_dir = pstrdup(cmd->pool, arg);
			break;
		case 8:
			conf->extension_dir = pstrdup(cmd->pool, arg);
			break;
		case 9:
			conf->error_log = pstrdup(cmd->pool, arg);
			break;
	}
	return NULL;
}

int php3_xbithack_handler(request_rec * r)
{
	php3_ini_structure *conf;

	conf = (php3_ini_structure *) get_module_config(r->per_dir_config, &php3_module);
	if (!(r->finfo.st_mode & S_IXUSR)) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	if (conf->xbithack == 0) {
		r->allowed |= (1 << METHODS) - 1;
		return DECLINED;
	}
	return send_parsed_php3(r);
}

void php3_exit_handler(server_rec *s, pool *p)
{
	php3_module_shutdown();
}

void php3_init_handler(server_rec *s, pool *p)
{
	/* For Apache 1.2 try registering a cleanup function for the main server pool */
#if MODULE_MAGIC_NUMBER < 19970728
	register_cleanup(p, NULL, php3_module_shutdown, php3_module_shutdown);
#endif
}

handler_rec php3_handlers[] =
{
	{"application/x-httpd-php3", send_parsed_php3},
	{"application/x-httpd-php3-preprocessed", send_parsed_preprocessed_php3},
	{"application/x-httpd-php3-source", send_parsed_php3_source},
	{"text/html", php3_xbithack_handler},
	{NULL}
};


command_rec php3_commands[] =
{
	{"php3_error_reporting", php3take1handler, (void *)0, OR_OPTIONS, TAKE1, "error reporting level"},
	{"php3_doc_root", php3take1handler, (void *)1, RSRC_CONF, TAKE1, "directory"}, /* not used yet */
	{"php3_user_dir", php3take1handler, (void *)2, RSRC_CONF, TAKE1, "user directory"}, /* not used yet */
	{"php3_safe_mode_exec_dir", php3take1handler, (void *)3, RSRC_CONF, TAKE1, "safe mode executable dir"},
	{"php3_include_path", php3take1handler, (void *)4, OR_OPTIONS, TAKE1, "colon-separated path"},
	{"php3_auto_prepend_file", php3take1handler, (void *)5, OR_OPTIONS, TAKE1, "file name"},
	{"php3_auto_append_file", php3take1handler, (void *)6, OR_OPTIONS, TAKE1, "file name"},
	{"php3_upload_tmp_dir", php3take1handler, (void *)7,  RSRC_CONF, TAKE1, "directory"},
	{"php3_extension_dir", php3take1handler, (void *)8,  RSRC_CONF, TAKE1, "directory"},
	{"php3_error_log", php3take1handler, (void *)9, OR_OPTIONS, TAKE1, "error log file"},

	{"php3_track_errors", php3flaghandler, (void *)0, OR_OPTIONS, FLAG, "on|off"},
	{"php3_magic_quotes_gpc", php3flaghandler, (void *)1, OR_OPTIONS, FLAG, "on|off"},
	{"php3_magic_quotes_runtime", php3flaghandler, (void *)2, OR_OPTIONS, FLAG, "on|off"},
	{"php3_short_open_tag", php3flaghandler, (void *)3, OR_OPTIONS, FLAG, "on|off"},
	{"php3_safe_mode", php3flaghandler, (void *)4, RSRC_CONF, FLAG, "on|off"},
	{"php3_track_vars", php3flaghandler, (void *)5, RSRC_CONF, FLAG, "on|off"},
	{"php3_sql_safe_mode", php3flaghandler, (void *)6,  RSRC_CONF, FLAG, "on|off"},
	{"php3_engine", php3flaghandler, (void *)7, RSRC_CONF, FLAG, "on|off"},
	{"php3_xbithack", php3flaghandler, (void *)8, OR_OPTIONS, FLAG, "on|off"},
	{"php3_last_modified", php3flaghandler, (void *)9, OR_OPTIONS, FLAG, "on|off"},
	{"php3_log_errors", php3flaghandler, (void *)10, OR_OPTIONS, FLAG, "on|off"},
	{NULL}
};



module MODULE_VAR_EXPORT php3_module =
{
	STANDARD_MODULE_STUFF,
	php3_init_handler,			/* initializer */
	php3_create_dir,			/* per-directory config creator */
	php3_merge_dir,				/* dir merger */
	NULL,						/* per-server config creator */
	NULL, 						/* merge server config */
	php3_commands,				/* command table */
	php3_handlers,				/* handlers */
	NULL,						/* filename translation */
	NULL,						/* check_user_id */
	NULL,						/* check auth */
	NULL,						/* check access */
	NULL,						/* type_checker */
	NULL,						/* fixups */
	NULL						/* logger */
#if MODULE_MAGIC_NUMBER >= 19970103
	,NULL						/* header parser */
#endif
#if MODULE_MAGIC_NUMBER >= 19970719
	,NULL             			/* child_init */
#endif
#if MODULE_MAGIC_NUMBER >= 19970728
	,php3_exit_handler			/* child_exit */
#endif
#if MODULE_MAGIC_NUMBER >= 19970902
	,NULL						/* post read-request */
#endif
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
