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
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "head.h"
#include "info.h"
#ifndef MSVC5
#include "build-defs.h"
#endif

#define PHP3_CONF_STR(directive,value1,value2) \
	PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">"); \
	PUTS(directive); \
	PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">&nbsp;"); \
	if(value1) PUTS(value1); \
	else PUTS("<i>none</i>"); \
	PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">&nbsp;"); \
	if(value2) PUTS(value2); \
	else PUTS("<i>none</i>"); \
	PUTS("</td></tr>\n");

#define PHP3_CONF_LONG(directive,value1,value2) \
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">%s</td><td bgcolor=\"" CONTENTS_COLOR "\">%ld</td><td bgcolor=\"" CONTENTS_COLOR "\">%ld</td></tr>\n",directive,value1,value2);

/* THREAD FIXME */
extern php3_ini_structure php3_ini;
extern php3_ini_structure php3_ini_master;
extern char **environ;

#define SECTION(name)  PUTS("<hr><h2>" name "</h2>\n")

#if APACHE
extern module *top_module;
#endif

#define ENTRY_NAME_COLOR "#999999"
#define CONTENTS_COLOR "#DDDDDD"
#define HEADER_COLOR "#00DDDD"

static int _display_module_info(php3_module_entry *module)
{
	TLS_VARS;
	
	PUTS("<tr><th bgcolor=\"" ENTRY_NAME_COLOR "\">");
	PUTS(module->name);
	PUTS("</th><td bgcolor=\"" CONTENTS_COLOR "\">");
	if (module->info_func) {
		module->info_func();
	} else {
		PUTS("&nbsp;");
	}
	PUTS("</td></tr>\n");
	return 0;
}


void _php3_info(void)
{
	char **env,*tmp1,*tmp2;
	TLS_VARS;
	
	if(!php3_header(0,NULL)) {  /* Don't send anything on a HEAD request */
		return;
	}
	php3_printf("<center><h1>PHP Version %s</h1></center>\n", PHP_VERSION);
	PUTS("by <a href=\"mailto:rasmus@lerdorf.on.ca\">Rasmus Lerdorf</a>,\n");
	PUTS("<a href=\"mailto:andi@php.net\">Andi Gutmans</a>,\n");
	PUTS("<a href=\"mailto:bourbon@netvision.net.il\">Zeev Suraski</a>,\n");
	PUTS("<a href=\"mailto:ssb@guardian.no\">Stig Bakken</a>,\n");
	PUTS("<a href=\"mailto:shane@caraveo.com\">Shane Caraveo</a>,\n");
	PUTS("<a href=\"mailto:jimw@php.net\">Jim Winstead</a>, and countless others.<P>\n");

	PUTS("<hr><p><b>");
#ifdef MSVC5
	PUTS("Windows95/NT Version</b> compiled with MS VC++ V5\n");
#else
	{
		FILE *fp;
		char buf[256];

		buf[255]=0;
		PUTS("Unix version:</b> ");
		fp = popen("uname -a","r");
		if(fp) {
			while(fgets(buf,255,fp)) {
				PUTS(buf);
			}
			pclose(fp);
		}
	}
#endif
	PUTS("\n");


	PUTS("<center>\n");
	
	SECTION("Extensions");
	PUTS("<table border=5 width=\"600\">\n");
	PUTS("<tr><th bgcolor=\"" HEADER_COLOR "\">Extensions</th><th bgcolor=\"" HEADER_COLOR "\">Additional Information</th></tr>\n");
	
#ifndef MSVC5
	PUTS("<tr><th bgcolor=\"" ENTRY_NAME_COLOR "\">PHP core</th>\n");
	PUTS("<td bgcolor=\"" CONTENTS_COLOR "\"><tt>CFLAGS=" PHP_CFLAGS "<br>\n");
	PUTS("HSREGEX=" PHP_HSREGEX "</td></tr>\n");
#endif

#if APACHE
	{
		module *modp = NULL;
#if !defined(WIN32) && !defined(WINNT)
		char name[64];
		char *p;
#endif
		server_rec *serv = GLOBAL(php3_rqst)->server;
		extern char server_root[MAX_STRING_LEN];
		extern uid_t user_id;
		extern char *user_name;
		extern gid_t group_id;
		extern int max_requests_per_child;

		PUTS("<tr><th bgcolor=\"" ENTRY_NAME_COLOR "\">Apache</th><td>\n");
#if WIN32|WINNT
		PUTS("Apache for Windows 95/NT<br>");
#else
		php3_printf("<tt>APACHE_INCLUDE=%s<br>\n", PHP_APACHE_INCLUDE);
		php3_printf("APACHE_TARGET=%s<br></tt>\n", PHP_APACHE_TARGET);
#endif
		php3_printf("Apache Version: <b>%s</b><br>",SERVER_VERSION);
#ifdef APACHE_RELEASE
		php3_printf("Apache Release: <b>%d</b><br>",APACHE_RELEASE);
#endif
		php3_printf("Apache API Version: <b>%d</b><br>",MODULE_MAGIC_NUMBER);
		php3_printf("Hostname/port: <b>%s:%u</b><br>\n",serv->server_hostname,serv->port);
#if !defined(WIN32) && !defined(WINNT)
		php3_printf("User/Group: <b>%s(%d)/%d</b><br>\n",user_name,(int)user_id,(int)group_id);
		php3_printf("Max Requests: <b>per child: %d &nbsp;&nbsp; keep alive: %s &nbsp;&nbsp; max per connection: %d</b><br>\n",max_requests_per_child,serv->keep_alive ? "on":"off", serv->keep_alive_max);
#endif
		php3_printf("Timeouts: <b>connection: %d &nbsp;&nbsp; keep-alive: %d</b><br>",serv->timeout,serv->keep_alive_timeout);
#if !defined(WIN32) && !defined(WINNT)
		php3_printf("Server Root: <b>%s</b><br>\n",server_root);
		
		PUTS("Loaded modules: ");
		for(modp = top_module; modp; modp = modp->next) {
			strncpy(name, modp->name, sizeof(name) - 1);
			if ((p = strrchr(name, '.'))) {
				*p='\0'; /* Cut off ugly .c extensions on module names */
			}
			PUTS(name);
			if (modp->next) {
				PUTS(", ");
			}
		}
#endif
		PUTS("<br>\n");
	}	
	PUTS("</td></tr>\n");
#endif
	hash_apply(&GLOBAL(module_registry),(int (*)(void *))_display_module_info);
	PUTS("</table>\n");


	SECTION("Configuration");
	PUTS("<table border=5 width=\"600\">\n");
	PUTS("<tr><th bgcolor=\"" HEADER_COLOR "\">Directive</th><th bgcolor=\"" HEADER_COLOR "\">Master Value</th><th bgcolor=\"" HEADER_COLOR "\">Local Value</th></tr>\n");
	PHP3_CONF_LONG("error_reporting", php3_ini_master.errors, php3_ini.errors);
	PHP3_CONF_LONG("max execution time", php3_ini_master.max_execution_time, php3_ini.max_execution_time);
	PHP3_CONF_LONG("memory limit", php3_ini_master.memory_limit, php3_ini.memory_limit);
	PHP3_CONF_STR("error_log", php3_ini_master.error_log, php3_ini.error_log);
	PHP3_CONF_LONG("magic_quotes_gpc", php3_ini_master.magic_quotes_gpc, php3_ini.magic_quotes_gpc);
	PHP3_CONF_LONG("magic_quotes_runtime", php3_ini_master.magic_quotes_runtime, php3_ini.magic_quotes_runtime);
	PHP3_CONF_LONG("track_errors", php3_ini_master.track_errors, php3_ini.track_errors);
	PHP3_CONF_STR("doc_root", php3_ini_master.doc_root, php3_ini.doc_root);
	PHP3_CONF_STR("user_dir", php3_ini_master.user_dir, php3_ini.user_dir);
	PHP3_CONF_LONG("safe_mode", php3_ini_master.safe_mode, php3_ini.safe_mode);
	PHP3_CONF_LONG("track_vars", php3_ini_master.track_vars, php3_ini.track_vars);
	PHP3_CONF_STR("safe_mode_exec_dir", php3_ini_master.safe_mode_exec_dir, php3_ini.safe_mode_exec_dir);
	PHP3_CONF_STR("cgi_ext", php3_ini_master.cgi_ext, php3_ini.cgi_ext);
	PHP3_CONF_STR("isapi_ext", php3_ini_master.isapi_ext, php3_ini.isapi_ext);
	PHP3_CONF_STR("nsapi_ext", php3_ini_master.nsapi_ext, php3_ini.nsapi_ext);
	PHP3_CONF_STR("include_path", php3_ini_master.include_path, php3_ini.include_path);
	PHP3_CONF_STR("auto_prepend_file", php3_ini_master.auto_prepend_file, php3_ini.auto_prepend_file);
	PHP3_CONF_STR("auto_append_file", php3_ini_master.auto_append_file, php3_ini.auto_append_file);
	PHP3_CONF_STR("upload_tmp_dir", php3_ini_master.upload_tmp_dir, php3_ini.upload_tmp_dir);
	PHP3_CONF_STR("extension_dir", php3_ini_master.extension_dir, php3_ini.extension_dir);
	PHP3_CONF_STR("arg_separator", php3_ini_master.arg_separator, php3_ini.arg_separator);
	PHP3_CONF_LONG("short_open_tag", php3_ini_master.short_open_tag, php3_ini.short_open_tag);
	PHP3_CONF_STR("debugger.host", php3_ini_master.debugger_host, php3_ini.debugger_host);
	PHP3_CONF_LONG("debugger.port", php3_ini_master.debugger_port, php3_ini.debugger_port);
	PHP3_CONF_LONG("sql_safe_mode", php3_ini_master.sql_safe_mode, php3_ini.sql_safe_mode);
	PHP3_CONF_LONG("xbithack", php3_ini_master.xbithack, php3_ini.xbithack);
	PHP3_CONF_LONG("engine", php3_ini_master.engine, php3_ini.engine);
	PHP3_CONF_LONG("last_modified", php3_ini_master.last_modified, php3_ini.last_modified);

	/* And now for the highlight colours */
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_comment</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",php3_ini_master.highlight_comment,php3_ini_master.highlight_comment,php3_ini.highlight_comment,php3_ini.highlight_comment);
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_default</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",php3_ini_master.highlight_default,php3_ini_master.highlight_default,php3_ini.highlight_default,php3_ini.highlight_default);
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_html</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",php3_ini_master.highlight_html,php3_ini_master.highlight_html,php3_ini.highlight_html,php3_ini.highlight_html);
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_string</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",php3_ini_master.highlight_string,php3_ini_master.highlight_string,php3_ini.highlight_string,php3_ini.highlight_string);
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_bg</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",php3_ini_master.highlight_bg,php3_ini_master.highlight_bg,php3_ini.highlight_bg,php3_ini.highlight_bg);
	php3_printf("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">highlight_keyword</td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td><td bgcolor=\"" CONTENTS_COLOR "\"><font color=%s>&nbsp;%s</font></td></tr>\n",php3_ini_master.highlight_keyword,php3_ini_master.highlight_keyword,php3_ini.highlight_keyword,php3_ini.highlight_keyword);
	PUTS("</table>");

	SECTION("Environment");
	PUTS("<table border=5 width=\"600\">\n");
	PUTS("<tr><th bgcolor=\"" HEADER_COLOR "\">Variable</th><th bgcolor=\"" HEADER_COLOR "\">Value</th></tr>\n");
	for (env=environ; env!=NULL && *env !=NULL; env++) {
		tmp1 = estrdup(*env);
		if ((tmp2 = strchr(tmp1,'='))) {
			*tmp2 = 0;
			tmp2++;
		}
		PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
		PUTS(tmp1);
		PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
		if (tmp2 && *tmp2) {
			PUTS(tmp2);
		} else {
			PUTS("&nbsp;");
		}
		PUTS("</td></tr>\n");
		efree(tmp1);
	}
	PUTS("</table>\n");

#if APACHE
	{
		register int i;
		array_header *arr = table_elts(GLOBAL(php3_rqst)->subprocess_env);
		table_entry *elts = (table_entry *)arr->elts;

		SECTION("Apache Environment");	
		PUTS("<table border=5 width=\"600\">\n");
		PUTS("<tr><th bgcolor=\"" HEADER_COLOR "\">Variable</th><th bgcolor=\"" HEADER_COLOR "\">Value</th></tr>\n");
		for (i=0; i < arr->nelts; i++) {
			PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
			PUTS(elts[i].key);
			PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
			PUTS(elts[i].val);
			PUTS("&nbsp;</td></tr>\n");
		}
		PUTS("</table>\n");
	}
#endif		

#if APACHE
	{
		array_header *env_arr;
		table_entry *env;
		int i;

		SECTION("HTTP Headers Information");
		PUTS("<table border=5 width=\"600\">\n");
		PUTS(" <tr><th colspan=2 bgcolor=\"" HEADER_COLOR "\">HTTP Request Headers</th></tr>\n");
		PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">HTTP Request</td><td bgcolor=\"" CONTENTS_COLOR "\">");
		PUTS(GLOBAL(php3_rqst)->the_request);
		PUTS("&nbsp;</td></tr>\n");
		env_arr = table_elts(GLOBAL(php3_rqst)->headers_in);
		env = (table_entry *)env_arr->elts;
		for (i = 0; i < env_arr->nelts; ++i) {
			if (env[i].key) {
				PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
				PUTS(env[i].key);
				PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
				PUTS(env[i].val);
				PUTS("&nbsp;</td></tr>\n");
			}
		}
		PUTS(" <tr><th colspan=2  bgcolor=\"" HEADER_COLOR "\">HTTP Response Headers</th></tr>\n");
		env_arr = table_elts(GLOBAL(php3_rqst)->headers_out);
		env = (table_entry *)env_arr->elts;
		for(i = 0; i < env_arr->nelts; ++i) {
			if (env[i].key) {
				PUTS("<tr><td bgcolor=\"" ENTRY_NAME_COLOR "\">");
				PUTS(env[i].key);
				PUTS("</td><td bgcolor=\"" CONTENTS_COLOR "\">");
				PUTS(env[i].val);
				PUTS("&nbsp;</td></tr>\n");
			}
		}
		PUTS("</table>\n\n");
	}
#endif

	PUTS("</center>");
	SECTION("GNU Public License");
    PUTS("<TT>This program is free software; you can redistribute it and/or modify\n");
    PUTS("it under the terms of the GNU General Public License as published by\n");
    PUTS("the Free Software Foundation; either version 2 of the License, or\n");
    PUTS("(at your option) any later version.<p>\n");
    PUTS("This program is distributed in the hope that it will be useful,\n");
    PUTS("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
    PUTS("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
    PUTS("GNU General Public License for more details.<p>\n");
    PUTS("You should have received a copy of the GNU General Public License\n");
    PUTS("along with this program; if not, write to the <b>Free Software\n");
    PUTS("Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.</b></TT><p>\n");
	
}

void php3_info(INTERNAL_FUNCTION_PARAMETERS)
{
	TLS_VARS;
	
	_php3_info();
	RETURN_TRUE;
}


void php3_version(INTERNAL_FUNCTION_PARAMETERS)
{
	TLS_VARS;
	
    RETURN_STRING(PHP_VERSION);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
