#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_main.h"
#include "http_protocol.h"
#include "util_script.h"
#include "mod_php.h"

module php_module;

extern int apache_php_module_main(request_rec *r, php_module_conf *, int);

int send_parsed_php(request_rec *r) {
	int fd;
	php_module_conf *conf;

	/* Make sure file exists */
	if (r->finfo.st_mode == 0) return NOT_FOUND;

	/* Open the file */	
	if(!(fd=popenf(r->pool, r->filename, O_RDONLY, 0))) {
		log_reason("file permissions deny server access", r->filename, r);
		return FORBIDDEN;
	}

	/* grab configuration settings */
	conf = (php_module_conf *)get_module_config(r->per_dir_config,&php_module);
	
	/* Init timeout */
	hard_timeout ("send", r);

	if(r->header_only) {
		r->content_type = "text/html";		
		send_http_header(r);
		kill_timeout (r);
		pclosef(r->pool, fd);
		return OK;
    }

	chdir_file(r->filename);
	add_common_vars(r);
	add_cgi_vars(r);
	apache_php_module_main(r,conf,fd);

	/* Done, turn off timeout, close file and return */
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
	new->MsqlLogDB=NULL;
	new->AccessDir=NULL;
	new->MaxDataSpace=8192;
	return new;
}

char *phpflaghandler(cmd_parms *cmd, php_module_conf *conf, int val) {
	int c = (int)cmd->info;

	switch(c) {
	case 0:
		conf->ShowInfo = val;
		break;
	case 1:
		conf->Logging = val;
		break;
	}			
	return NULL;
}

char *phptake1handler(cmd_parms *cmd, php_module_conf *conf, char *arg) {
	int c = (int)cmd->info;

	switch(c) {
	case 0:
		conf->UploadTmpDir = pstrdup(cmd->pool,arg);
		break;
	case 1:
		conf->dbmLogDir = pstrdup(cmd->pool,arg);
		break;
	case 2:
		conf->MsqlLogDB = pstrdup(cmd->pool,arg);
		break;
	case 3:
		conf->AccessDir = pstrdup(cmd->pool,arg);
		break;
	case 4:
		conf->MaxDataSpace = atoi(arg);
		break;
	}
	return NULL;
}

command_rec php_commands[] = {
	{ "phpShowInfo",phpflaghandler,(void *)0,OR_FILEINFO,FLAG,"on|off" },
	{ "phpLogging",phpflaghandler,(void *)1,OR_FILEINFO,FLAG,"on|off" },
	{ "phpUploadTmpDir",phptake1handler,(void *)0,OR_FILEINFO,TAKE1,"directory" },
	{ "phpDbmLogDir",phptake1handler,(void *)1,OR_FILEINFO,TAKE1,"directory" },
	{ "phpMsqlLogDB",phptake1handler,(void *)2,OR_FILEINFO,TAKE1,"database" },
	{ "phpAccessDir",phptake1handler,(void *)3,OR_FILEINFO,TAKE1,"directory" },
	{ "phpMaxDataSpace",phptake1handler,(void *)4,OR_FILEINFO,TAKE1,"number of kilobytes" },
	{ NULL }
};

handler_rec php_handlers[] = {
	{ "application/x-httpd-php", send_parsed_php },
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
