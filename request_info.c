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
   | Author: Jim Winstead (jimw@php.net)                                  |
   +----------------------------------------------------------------------+
 */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"

#ifndef THREAD_SAFE
php3_request_info request_info;
#endif

#if CGI_BINARY
int php3_init_request_info(void *conf) {
	char *buf;
	TLS_VARS;

	/* We always need to emalloc() filename, since it gets placed into
	   the include file hash table, and gets freed with that table.
	   Notice that this means that we don't need to efree() it in
	   php3_destroy_request_info()! */
	GLOBAL(request_info).filename = NULL;
	GLOBAL(request_info).path_info = getenv("PATH_INFO");
	GLOBAL(request_info).path_translated = getenv("PATH_TRANSLATED");
	GLOBAL(request_info).query_string = getenv("QUERY_STRING");
	GLOBAL(request_info).current_user = NULL;
	GLOBAL(request_info).current_user_length=0;
	GLOBAL(request_info).request_method = getenv("REQUEST_METHOD");
	GLOBAL(request_info).script_name = getenv("SCRIPT_NAME");
	buf = getenv("CONTENT_LENGTH");
	GLOBAL(request_info).content_length = (buf ? atoi(buf) : 0);
	GLOBAL(request_info).content_type = getenv("CONTENT_TYPE");
	GLOBAL(request_info).cookies = getenv("HTTP_COOKIE");

	return SUCCESS;
}

int php3_destroy_request_info(void *conf) {
	TLS_VARS;
	STR_FREE(GLOBAL(request_info).current_user);
	return SUCCESS;
}
#endif

#if APACHE
int php3_init_request_info(void *conf) {
	char *buf;
	TLS_VARS;

	/* see above for why this has to be estrdup()'d */
	GLOBAL(request_info).filename = estrdup(GLOBAL(php3_rqst)->filename);
	GLOBAL(request_info).request_method = GLOBAL(php3_rqst)->method;
	GLOBAL(request_info).query_string = GLOBAL(php3_rqst)->args;
	GLOBAL(request_info).content_type = table_get(GLOBAL(php3_rqst)->subprocess_env, "CONTENT_TYPE");

	buf = table_get(GLOBAL(php3_rqst)->subprocess_env, "CONTENT_LENGTH");
	GLOBAL(request_info).content_length = (buf ? atoi(buf) : 0);

	GLOBAL(request_info).cookies = table_get(GLOBAL(php3_rqst)->subprocess_env, "HTTP_COOKIE");

	return SUCCESS;
}

#endif

#if USE_SAPI
/* temporary until I figure a beter way to do it*/
int php3_init_request_info(void *conf) {
	TLS_VARS;
	if(GLOBAL(sapi_rqst)->filename)
		GLOBAL(request_info).filename = estrdup(GLOBAL(sapi_rqst)->filename);
	else
		GLOBAL(request_info).filename = NULL;
	GLOBAL(request_info).path_info = GLOBAL(sapi_rqst)->path_info;
	GLOBAL(request_info).path_translated = GLOBAL(sapi_rqst)->path_translated;
	GLOBAL(request_info).query_string = GLOBAL(sapi_rqst)->query_string;
	GLOBAL(request_info).current_user = GLOBAL(sapi_rqst)->current_user;
	GLOBAL(request_info).current_user_length=GLOBAL(sapi_rqst)->current_user_length;
	GLOBAL(request_info).request_method = GLOBAL(sapi_rqst)->request_method;
	GLOBAL(request_info).script_name = GLOBAL(sapi_rqst)->script_name;
	GLOBAL(request_info).content_length = GLOBAL(sapi_rqst)->content_length;
	GLOBAL(request_info).content_type = GLOBAL(sapi_rqst)->content_type;
	GLOBAL(request_info).cookies = GLOBAL(sapi_rqst)->cookies;

	return SUCCESS;
}
#endif

#if !CGI_BINARY
int php3_destroy_request_info(void *conf) {
	/* see above for why we don't want to efree() request_info.filename */
	return SUCCESS;
}
#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
