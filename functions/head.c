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
   | Authors: Rasmus Lerdorf                                              |
   +----------------------------------------------------------------------+
 */
/* $Id: head.c,v 1.82 1998/01/25 07:13:43 shane Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include <stdio.h>
#include "parser.h"
#include "internal_functions.h"
#include "reg.h"
#include "php3_string.h"
#include "pageinfo.h"
#include "reg.h"
#include "main.h"
#include "datetime.h"
#include "head.h"
#include "post.h"
#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif

#include "safe_mode.h"
#include "url.h"


/* need to figure out some nice way to get rid of these */
#ifndef THREAD_SAFE
static int php3_HeaderPrinted = 0;
static int php3_PrintHeader = 1;
static CookieList *top = NULL;
static char *cont_type = NULL;
static int header_called = 0;
#endif

void php3_PushCookieList(char *, char *, time_t, char *, char *, int);
CookieList *php3_PopCookieList(void);

int php3_init_head(INITFUNCARG)
{
	TLS_VARS;
	GLOBAL(php3_HeaderPrinted) = 0;
	GLOBAL(php3_PrintHeader) = 1;
	GLOBAL(top) = NULL;
	GLOBAL(cont_type) = NULL;

	return SUCCESS;
}

void php3_noheader(void)
{
	TLS_VARS;
	GLOBAL(php3_PrintHeader) = 0;
	GLOBAL(header_called) = 1;
}

void php3_Header(INTERNAL_FUNCTION_PARAMETERS)
{
	char *r;
#if APACHE
	char *rr = NULL;
	char *temp = NULL;
	long myuid = 0L;
	char temp2[32];
#endif
	YYSTYPE *arg1;
TLS_VARS;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
#if APACHE
	if (GLOBAL(php3_HeaderPrinted) == 1) {
#if DEBUG
		php3_error(E_WARNING, "Woops, call to Header - already gone\n");
#endif
		return;					/* too late, already sent */
	}
	/*
	 * Not entirely sure this is the right way to support the header
	 * command in the Apache module.  Comments?
	 */
	r = strchr(arg1->value.strval, ':');
	if (r) {
		*r = '\0';
		if (!strcasecmp(arg1->value.strval, "Content-type")) {
			if (*(r + 1) == ' ')
				GLOBAL(php3_rqst)->content_type = pstrdup(GLOBAL(php3_rqst)->pool,r + 2);
			else
				GLOBAL(php3_rqst)->content_type = pstrdup(GLOBAL(php3_rqst)->pool,r + 1);
		} else {
			if (*(r + 1) == ' ')
				rr = r + 2;
			else
				rr = r + 1;
			if (php3_ini.safe_mode && (!strcasecmp(arg1->value.strval, "WWW-authenticate"))) {
				myuid = _php3_getuid();
				sprintf(temp2, "realm=\"%ld ", myuid);  /* SAFE */
				temp = _php3_regreplace("realm=\"", temp2, rr, 1, 0);
				if (!strcmp(temp, rr)) {
					sprintf(temp2, "realm=%ld", myuid);  /* SAFE */
					temp = _php3_regreplace("realm=", temp2, rr, 1, 0);
					if (!strcmp(temp, rr)) {
						sprintf(temp2, " realm=%ld", myuid);  /* SAFE */
						temp = _php3_regreplace("$", temp2, rr, 0, 0);
					}
				}
				table_set(GLOBAL(php3_rqst)->headers_out, arg1->value.strval, temp);
			} else
				table_set(GLOBAL(php3_rqst)->headers_out, arg1->value.strval, rr);
		}
		if (!strcasecmp(arg1->value.strval, "location")) {
			GLOBAL(php3_rqst)->status = REDIRECT;
		}
		*r = ':';
		GLOBAL(php3_HeaderPrinted) = 2;
	}
	if (!strncasecmp(arg1->value.strval, "http/", 5)) {
		if (strlen(arg1->value.strval) > 9) {
			GLOBAL(php3_rqst)->status = atoi(&((arg1->value.strval)[9]));
		}
		/* Use a pstrdup here to get the memory straight from Apache's per-request pool to
		 * avoid having our own memory manager complain about this memory not being freed
		 * because it really shouldn't be freed until the end of the request and it isn't
		 * easy for us to figure out when we allocated it vs. when something else might have.
		 */
		GLOBAL(php3_rqst)->status_line = pstrdup(GLOBAL(php3_rqst)->pool,&((arg1->value.strval)[9]));
	}
#else
	r = strchr(arg1->value.strval, ':');
	if (r) {
		*r = '\0';
		if (!strcasecmp(arg1->value.strval, "Content-type")) {
			GLOBAL(cont_type) = estrdup(r + 1);;
		} else {
			*r = ':';
#if USE_SAPI
			{
				char *tempstr=emalloc(strlen(arg1->value.strval)+2);
				
				sprintf(tempstr,"%s\015\012",tempstr);
				GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,tempstr);
				efree(tempstr);
			}
#else /* CGI BINARY */
			PUTS(arg1->value.strval);
			PUTS("\015\012");
#endif/* end if SAPI */
		}
	} else {
#if USE_SAPI
		{
		char *tempstr=emalloc(strlen(arg1->value.strval)+2);
		sprintf(tempstr,"%s\015\012",tempstr);
		GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,tempstr);
		efree(tempstr);
		}
#else /* CGI BINARY */
		PUTS(arg1->value.strval);
		PUTS("\015\012");
#endif /* endif SAPI */
	}
#endif
}

/*
 * php3_header() flushes the header info built up using calls to
 * the Header() function.  If type is 1, a redirect to str is done.
 * Otherwise type should be 0 and str NULL.
 *
 * The function returns non-zero if output is allowed after the
 * call, and zero otherwise.  Any call to php3_header() must check
 * the return status and if false, no output must be sent.  This
 * is in order to correctly handle HEAD requests.
 */
PHPAPI int php3_header(int type, char *str)
{
	CookieList *cookie;
	int len = 0;
	char *r, *tempstr, *dt;
	time_t t;
TLS_VARS;

#if APACHE
	if (!GLOBAL(php3_rqst)) {  /* we're not in a request, allow output */
		return 1;
	}
	if ((GLOBAL(php3_PrintHeader) && !GLOBAL(php3_HeaderPrinted)) || (GLOBAL(php3_PrintHeader) && GLOBAL(php3_HeaderPrinted) == 2)) {
		if (type == 1) {
			table_set(GLOBAL(php3_rqst)->headers_out, "Location", str);
			GLOBAL(php3_rqst)->status = REDIRECT;
		}
		if (!(GLOBAL(initialized) & INIT_ENVIRONMENT) && GLOBAL(request_info).request_method && !strcasecmp(GLOBAL(request_info).request_method, "post")) {
			php3_treat_data(PARSE_POST, NULL);	/* POST Data */
		}
		cookie = php3_PopCookieList();
		while (cookie) {
			if (cookie->name)
				len += strlen(cookie->name);
			if (cookie->value)
				len += strlen(cookie->value);
			if (cookie->path)
				len += strlen(cookie->path);
			if (cookie->domain)
				len += strlen(cookie->domain);
			tempstr = emalloc(len + 100);
			if (!cookie->value || (cookie->value && !*cookie->value)) {
				/* 
				 * MSIE doesn't delete a cookie when you set it to a null value
				 * so in order to force cookies to be deleted, even on MSIE, we
				 * pick an expiry date 1 year and 1 second in the past
				 */
				sprintf(tempstr, "%s=deleted", cookie->name);
				t = time(NULL) - 31536001;
				strcat(tempstr, "; expires=");
				dt = php3_std_date(t);
				strcat(tempstr, dt);
				efree(dt);
			} else {
				r = _php3_urlencode(cookie->value);
				sprintf(tempstr, "%s=%s", cookie->name, cookie->value ? r : "");
				if(r) efree(r);
				if(cookie->name) efree(cookie->name);
				if(cookie->value) efree(cookie->value);
				cookie->name=NULL;
				cookie->value=NULL;
				if (cookie->expires > 0) {
					strcat(tempstr, "; expires=");
					dt = php3_std_date(cookie->expires);
					strcat(tempstr, dt);
					efree(dt);
				}
			}
			if (cookie->path && strlen(cookie->path)) {
				strcat(tempstr, "; path=");
				strcat(tempstr, cookie->path);
				efree(cookie->path);
				cookie->path=NULL;
			}
			if (cookie->domain && strlen(cookie->domain)) {
				strcat(tempstr, "; domain=");
				strcat(tempstr, cookie->domain);
				efree(cookie->domain);
				cookie->domain=NULL;
			}
			if (cookie->secure) {
				strcat(tempstr, "; secure");
			}
			table_add(GLOBAL(php3_rqst)->headers_out, "Set-Cookie", tempstr);
			if(cookie->domain) efree(cookie->domain);
			if(cookie->path) efree(cookie->path);
			if(cookie->name) efree(cookie->name);
			if(cookie->value) efree(cookie->value);
			efree(cookie);
			cookie = php3_PopCookieList();
			efree(tempstr);
		}
		GLOBAL(php3_HeaderPrinted) = 1;
		GLOBAL(header_called) = 1;
		send_http_header(GLOBAL(php3_rqst));
		if (GLOBAL(php3_rqst)->header_only) {
			void *conf = NULL;
			php3_request_shutdown(conf _INLINE_TLS);	/* On a HEAD command, exit right after sending header */
			return(0);
		}
	}
#else
	if (GLOBAL(php3_PrintHeader) && !GLOBAL(php3_HeaderPrinted)) {
		if (!(GLOBAL(initialized) & INIT_ENVIRONMENT) && GLOBAL(request_info).request_method && !strcasecmp(GLOBAL(request_info).request_method, "post")) {
			php3_treat_data(PARSE_POST, NULL);	/* POST Data */
		}
		cookie = php3_PopCookieList();
		while (cookie) {
			if (cookie->name)
				len += strlen(cookie->name);
			if (cookie->value)
				len += strlen(cookie->value);
			if (cookie->path)
				len += strlen(cookie->path);
			if (cookie->domain)
				len += strlen(cookie->domain);
			tempstr = emalloc(len + 100);
			if (!cookie->value || (cookie->value && !*cookie->value)) {
				/* 
				 * MSIE doesn't delete a cookie when you set it to a null value
				 * so in order to force cookies to be deleted, even on MSIE, we
				 * pick an expiry date 1 year and 1 second in the past
				 */
				sprintf(tempstr, "%s=deleted", cookie->name);
				strcat(tempstr, "; expires=");
				t = time(NULL) - 31536001;
				dt = php3_std_date(t);
				strcat(tempstr, dt);
				efree(dt);
			} else {
				r = _php3_urlencode(cookie->value);
				sprintf(tempstr, "%s=%s", cookie->name, cookie->value ? r : "");
				if(r) efree(r);
				if(cookie->value) efree(cookie->value);
				cookie->value=NULL;
				if(cookie->name) efree(cookie->name);
				cookie->name=NULL;
				if (cookie->expires > 0) {
					strcat(tempstr, "; expires=");
					dt = php3_std_date(cookie->expires);
					strcat(tempstr, dt);
					efree(dt);
				}
			}
			if (cookie->path && strlen(cookie->path)) {
				strcat(tempstr, "; path=");
				strcat(tempstr, cookie->path);
				efree(cookie->path);
				cookie->path=NULL;
			}
			if (cookie->domain && strlen(cookie->domain)) {
				strcat(tempstr, "; domain=");
				strcat(tempstr, cookie->domain);
				efree(cookie->domain);
				cookie->domain=NULL;
			}
			if (cookie->secure) {
				strcat(tempstr, "; secure");
			}
#if USE_SAPI
			{
			char *tempstr2=emalloc(strlen(str)+14);
			sprintf(tempstr2,"Set-Cookie: %s\015\012",tempstr);
			GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,tempstr2);
			efree(tempstr2);
			}
#else /* CGI BINARY */
			PUTS("Set-Cookie: ");
			PUTS(tempstr);
			PUTS("\015\012");
#endif /* endif SAPI */
			if(cookie->domain) efree(cookie->domain);
			if(cookie->path) efree(cookie->path);
			if(cookie->name) efree(cookie->name);
			if(cookie->value) efree(cookie->value);
			efree(cookie);
			cookie = php3_PopCookieList();
			efree(tempstr);
		}
		if (type == 1) {
#if USE_SAPI
			tempstr=emalloc(strlen(str)+14);
			sprintf(tempstr,"Location: %s\015\012\015\012",str);
			GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,tempstr);
			efree(tempstr);
#else /* CGI BINARY */
			PUTS("Location: ");
			PUTS(str);
			PUTS("\015\012\015\012");
#endif /* endif SAPI */
		} else {
			if (!GLOBAL(cont_type)) {
#if USE_SAPI
			GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,"Content-type: text/html\015\012\015\012");
#else /* CGI BINARY */
				PUTS("Content-type: text/html\015\012\015\012");
#endif /* endif SAPI */
			} else {
#if USE_SAPI
				tempstr=emalloc(strlen(GLOBAL(cont_type))+18);
				sprintf(tempstr,"Content-type: %s\015\012\015\012",GLOBAL(cont_type));
				GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,tempstr);
				free(tempstr);
#else /* CGI_BINARY */
				PUTS("Content-type:");
				PUTS(GLOBAL(cont_type));
				PUTS("\015\012\015\012");
#endif /* endif SAPI */
				efree(GLOBAL(cont_type));
			}
#if USE_SAPI
			GLOBAL(sapi_rqst)->flush(GLOBAL(sapi_rqst)->scid);
#else
			fflush(stdout);
#endif
		}
		GLOBAL(php3_HeaderPrinted) = 1;
		GLOBAL(header_called) = 1;
	}
#endif
	return(1);
}

void php3_PushCookieList(char *name, char *value, time_t expires, char *path, char *domain, int secure)
{
	CookieList *new;
	TLS_VARS;

	new = emalloc(sizeof(CookieList));
	new->next = GLOBAL(top);
	new->name = name;
	new->value = value;
	new->expires = expires;
	new->path = path;
	new->domain = domain;
	new->secure = secure;
	GLOBAL(top) = new;
}

CookieList *php3_PopCookieList(void)
{
	CookieList *ret;
	TLS_VARS;

	ret = GLOBAL(top);
	if (GLOBAL(top))
		GLOBAL(top) = GLOBAL(top)->next;
	return (ret);
}

/* php3_SetCookie(name,value,expires,path,domain,secure) */
void php3_SetCookie(INTERNAL_FUNCTION_PARAMETERS)
{
	char *name = NULL, *value = NULL, *path = NULL, *domain = NULL;
	time_t expires = 0;
	int secure = 0;
	YYSTYPE *arg[6];
	int arg_count;
	TLS_VARS;

	arg_count = ARG_COUNT(ht);
	if (arg_count < 1 || arg_count > 6 || getParametersArray(ht, arg_count, arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if (GLOBAL(php3_HeaderPrinted) == 1) {
		php3_error(E_WARNING, "Oops, php3_SetCookie called after header has been sent\n");
		return;
	}
	switch (arg_count) {
		case 6:
			convert_to_boolean_long(arg[5]);
			secure = arg[5]->value.lval;
		case 5:
			convert_to_string(arg[4]);
			domain = estrndup(arg[4]->value.strval,arg[4]->strlen);
		case 4:
			convert_to_string(arg[3]);
			path = estrndup(arg[3]->value.strval,arg[3]->strlen);
		case 3:
			convert_to_long(arg[2]);
			expires = arg[2]->value.lval;
		case 2:
			convert_to_string(arg[1]);
			value = estrndup(arg[1]->value.strval,arg[1]->strlen);
		case 1:
			convert_to_string(arg[0]);
			name = estrndup(arg[0]->value.strval,arg[0]->strlen);
	}
	php3_PushCookieList(name, value, expires, path, domain, secure);
}


int php3_headers_unsent(void)
{
	TLS_VARS;
	if (GLOBAL(php3_HeaderPrinted)!=1 || !GLOBAL(php3_PrintHeader)) {
		return 1;
	} else {
		return 0;
	}
}


function_entry php3_header_functions[] = {
	{"setcookie",		php3_SetCookie,		NULL},
	{"header",			php3_Header,		NULL},
	{NULL, NULL, NULL}
};


php3_module_entry php3_header_module_entry = {
	"PHP_head", php3_header_functions, NULL, NULL, php3_init_head, NULL, NULL, 0, 0, 0, NULL
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
