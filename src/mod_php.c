/***[mod_php.c]***************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996,1997 Rasmus Lerdorf                                    *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License as published by      *
*  the Free Software Foundation; either version 2 of the License, or         *
*  (at your option) any later version.                                       *
*                                                                            *
*  This program is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU General Public License for more details.                              *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with this program; if not, write to the Free Software               *
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
*                                                                            *
\****************************************************************************/
#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_main.h"
#include "http_protocol.h"
#include "util_script.h"
#include "mod_php.h"

#if WINNT|WIN32
module MODULE_VAR_EXPORT php_module;
#else
module php_module;
#endif
int saved_umask;
   
#ifdef PHP_XBITHACK
#define DEFAULT_PHP_XBITHACK 1
#else
#define DEFAULT_PHP_XBITHACK 0
#endif

extern int apache_php_module_main(request_rec *r, php_module_conf *, int);

void save_umask() {
    saved_umask = umask(077);
	umask(saved_umask);
}

void restore_umask() {
	umask(saved_umask);
}

int send_parsed_php(request_rec *r) {
	int fd, retval;
	php_module_conf *conf;
    
	/* Make sure file exists */
	if (r->finfo.st_mode == 0) return NOT_FOUND;

	/* grab configuration settings */
	conf = (php_module_conf *)get_module_config(r->per_dir_config,&php_module);

	/* 
	 * If PHP parser engine has been turned off with the phpEngine off directive,
	 * then decline to handle this request
	 */
	if(!conf->engine) {
		r->content_type = "text/html";		
		return DECLINED;
	}

	/* Open the file */	
	if((fd=popenf(r->pool, r->filename, O_RDONLY, 0))==-1) {
		log_reason("file permissions deny server access", r->filename, r);
		return FORBIDDEN;
	}

	/* Apache 1.2 has a more complex mechanism for reading POST data */
#if MODULE_MAGIC_NUMBER > 19961007
	if ((retval = setup_client_block(r,REQUEST_CHUNKED_ERROR))) return retval;
#endif

	if (conf->LastModified) {
#if MODULE_MAGIC_NUMBER < 19970912
		if(retval = set_last_modified (r, r->finfo.st_mtime)) {
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
	hard_timeout ("send", r);

	save_umask();
	chdir_file(r->filename);
	add_common_vars(r);
	add_cgi_vars(r);
	apache_php_module_main(r,conf,fd);

	/* Done, restore umask, turn off timeout, close file and return */
	restore_umask();
	kill_timeout (r);
	pclosef(r->pool, fd);
	return OK;
}

/* Initialize a per-server module configuation structure */
void *php_create_conf(pool *p, char *dummy) {
	php_module_conf *new;

	new = (php_module_conf *)palloc(p,sizeof(php_module_conf));	
	new->ShowInfo = 1;
	new->Logging = 1;
	new->UploadTmpDir=NULL;
	new->dbmLogDir=NULL;
	new->SQLLogDB=NULL;
	new->SQLLogHost=NULL;
	new->AccessDir=NULL;
	new->MaxDataSpace=8192;
	new->XBitHack=DEFAULT_PHP_XBITHACK;
	new->IncludePath=NULL;
	new->AutoPrependFile=NULL;
	new->AutoAppendFile=NULL;
	new->Debug = 0;
	new->engine = 1;
	new->LastModified = 0;
	new->AdaUser = NULL;
	new->AdaPW = NULL;
	new->AdaDB = NULL;
	return new;
}

#if MODULE_MAGIC_NUMBER > 19961007
const char *phpflaghandler(cmd_parms *cmd, php_module_conf *conf, int val) {
#else
char *phpflaghandler(cmd_parms *cmd, php_module_conf *conf, int val) {
#endif
	int c = (int)cmd->info;

	switch(c) {
	case 0:
		conf->ShowInfo = val;
		break;
	case 1:
		conf->Logging = val;
		break;
	case 2:
		conf->XBitHack = val;
		break;
	case 3:
		conf->Debug = val;
		break;
	case 4:
		conf->engine = val;
		break;
	case 5:
		conf->LastModified = val;
		break;
	}			
	return NULL;
}

#if MODULE_MAGIC_NUMBER > 19961007
const char *phptake1handler(cmd_parms *cmd, php_module_conf *conf, char *arg) {
#else
char *phptake1handler(cmd_parms *cmd, php_module_conf *conf, char *arg) {
#endif
	int c = (int)cmd->info;

	switch(c) {
	case 0:
		conf->UploadTmpDir = pstrdup(cmd->pool,arg);
		break;
	case 1:
		conf->dbmLogDir = pstrdup(cmd->pool,arg);
		break;
	case 2:
		conf->SQLLogDB = pstrdup(cmd->pool,arg);
		break;
	case 3:
		conf->AccessDir = pstrdup(cmd->pool,arg);
		break;
	case 4:
		conf->MaxDataSpace = atoi(arg);
		break;
	case 5:
		conf->SQLLogHost = pstrdup(cmd->pool,arg);
		break;
	case 6:
		conf->IncludePath = pstrdup(cmd->pool,arg);
		break;
	case 7:
		conf->AutoPrependFile = pstrdup(cmd->pool,arg);
		break;
	case 8:
		conf->AutoAppendFile = pstrdup(cmd->pool,arg);
		break;
	case 9:
		conf->AdaDB = pstrdup(cmd->pool,arg);
		break;
	case 10:
		conf->AdaUser = pstrdup(cmd->pool,arg);
		break;
	case 11:
		conf->AdaPW = pstrdup(cmd->pool,arg);
		break;
	}
	return NULL;
}

int php_xbithack_handler (request_rec *r) {   
	php_module_conf *conf;

	conf = (php_module_conf *)get_module_config(r->per_dir_config,&php_module);
	if (!(r->finfo.st_mode & S_IXUSR)) return DECLINED;
	if (conf->XBitHack == 0) return DECLINED;
	return send_parsed_php (r);
}  

handler_rec php_handlers[] = {
	{ "application/x-httpd-php", send_parsed_php },
	{ "text/html", php_xbithack_handler },
	{ NULL }
}; 


command_rec php_commands[] = {
	{ "phpShowInfo",phpflaghandler,(void *)0,OR_OPTIONS,FLAG,"on|off" },
	{ "phpLogging",phpflaghandler,(void *)1,OR_OPTIONS,FLAG,"on|off" },
	{ "phpDebug",phpflaghandler,(void *)3,OR_OPTIONS,FLAG,"on|off" },
	{ "phpUploadTmpDir",phptake1handler,(void *)0,OR_OPTIONS,TAKE1,"directory" },
	{ "phpDbmLogDir",phptake1handler,(void *)1,OR_OPTIONS,TAKE1,"directory" },
	{ "phpMsqlLogDB",phptake1handler,(void *)2,OR_OPTIONS,TAKE1,"database" },
	{ "phpSQLLogDB",phptake1handler,(void *)2,OR_OPTIONS,TAKE1,"database" },
	{ "phpAccessDir",phptake1handler,(void *)3,OR_OPTIONS,TAKE1,"directory" },
	{ "phpMaxDataSpace",phptake1handler,(void *)4,OR_OPTIONS,TAKE1,"number of kilobytes" },
	{ "phpMsqlLogHost",phptake1handler,(void *)5,OR_OPTIONS,TAKE1,"hostname" },
	{ "phpSQLLogHost",phptake1handler,(void *)5,OR_OPTIONS,TAKE1,"hostname" },
	{ "phpXBitHack", phpflaghandler, (void *)2, OR_OPTIONS, FLAG, "on|off" },
	{ "phpIncludePath",phptake1handler,(void *)6,OR_OPTIONS,TAKE1,"colon-separated path" },
	{ "phpEngine",phpflaghandler,(void *)4,RSRC_CONF,FLAG,"on|off" },
	{ "phpLastModified", phpflaghandler, (void *)5, OR_OPTIONS, FLAG, "on|off" },
	{ "phpAutoPrependFile", phptake1handler,(void *)7,OR_OPTIONS,TAKE1,"file name" },
	{ "phpAutoAppendFile", phptake1handler,(void *)8,OR_OPTIONS,TAKE1,"file name" },
	{ "phpAdaDefDB",phptake1handler,(void *)9,OR_OPTIONS,TAKE1,"database" },
	{ "phpAdaDefUser",phptake1handler,(void *)10,OR_OPTIONS,TAKE1,"user" },
	{ "phpAdaDefPW",phptake1handler,(void *)11,OR_OPTIONS,TAKE1,"password" },
	{ NULL }
};



module php_module = {
	STANDARD_MODULE_STUFF,
	NULL,				/* initializer */
	php_create_conf,	/* dir config creater */
	NULL,				/* dir merger --- default is to override */
	NULL,				/* server config */
	NULL,				/* merge server config */
	php_commands,		/* command table */
	php_handlers,		/* handlers */
	NULL,				/* filename translation */
	NULL,				/* check_user_id */
	NULL,				/* check auth */
	NULL,				/* check access */
	NULL,				/* type_checker */
	NULL,				/* fixups */
	NULL				/* logger */
};
