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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
/* $Id: head.c,v 1.126 2000/04/28 17:20:01 hholzgra Exp $ */
#include <stdio.h>
#include "php.h"
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

static int php3_HeaderPrinted = 0;
static int php3_PrintHeader = 1;
static CookieList *top = NULL;
static char *cont_type = NULL;
static int header_called = 0;

void php3_PushCookieList(char *, char *, time_t, char *, char *, int);
CookieList *php3_PopCookieList(void);

int php3_init_head(INIT_FUNC_ARGS)
{
	TLS_VARS;
	GLOBAL(php3_HeaderPrinted) = 0;
	if (GLOBAL(header_called) == 0)
		GLOBAL(php3_PrintHeader) = 1;
	GLOBAL(top) = NULL;
	GLOBAL(cont_type) = NULL;

	return SUCCESS;
}

/* {{{ proto int headers_sent(void)
   Return true if headers have already been sent, false otherwise */
void php3_Headers_Sent(INTERNAL_FUNCTION_PARAMETERS) {
	if (GLOBAL(php3_HeaderPrinted)) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}	
/* }}} */

void php3_noheader(void)
{
	TLS_VARS;
	GLOBAL(php3_PrintHeader) = 0;
	GLOBAL(header_called) = 1;
}


/* Implementation of the language Header() function */
/* {{{ proto void header(string header)
   Send a raw HTTP header */
void php3_Header(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
	_php3_Header(arg1->value.str.val);
}


PHPAPI void _php3_Header(char *strHeader)
{
	char *r;
#if APACHE
	char *rr = NULL;
	char *temp = NULL;
	long myuid = 0L;
	char temp2[32];
#endif
TLS_VARS;

        for(r=strHeader;*r;r++) {}
        for(--r;r>=strHeader;--r)
		if(isspace(*r))
			*r='\0';
		else
			break;

	if (GLOBAL(php3_HeaderPrinted) == 1) {
		php3_error(E_WARNING, "Cannot add more header information - the header was already sent "
							  "(header information may be added only before any output is generated from the script - "
							  "check for text or whitespace outside PHP tags, or calls to functions that output text)");
		return;					/* too late, already sent */
	}
#if APACHE
	/*
	 * Not entirely sure this is the right way to support the header
	 * command in the Apache module.  Comments?
	 */
	r = strchr(strHeader, ':');
	if (r) {
		*r = '\0';
		if (!strcasecmp(strHeader, "Content-type")) {
			if (*(r + 1) == ' ')
				GLOBAL(php3_rqst)->content_type = pstrdup(GLOBAL(php3_rqst)->pool,r + 2);
			else
				GLOBAL(php3_rqst)->content_type = pstrdup(GLOBAL(php3_rqst)->pool,r + 1);
			GLOBAL(cont_type) = (char *)GLOBAL(php3_rqst)->content_type;
		} else {
			if (*(r + 1) == ' ')
				rr = r + 2;
			else
				rr = r + 1;
			if (php3_ini.safe_mode && (!strcasecmp(strHeader, "WWW-authenticate"))) {
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
				table_set(GLOBAL(php3_rqst)->headers_out, strHeader, temp);
			} else
				table_set(GLOBAL(php3_rqst)->headers_out, strHeader, rr);
		}
		if (!strcasecmp(strHeader, "location")) {
			GLOBAL(php3_rqst)->status = REDIRECT;
		}
		*r = ':';
		GLOBAL(php3_HeaderPrinted) = 2;
	}
	if (!strncasecmp(strHeader, "http/", 5)) {
		if (strlen(strHeader) > 9) {
			GLOBAL(php3_rqst)->status = atoi(&((strHeader)[9]));
		}
		/* Use a pstrdup here to get the memory straight from Apache's per-request pool to
		 * avoid having our own memory manager complain about this memory not being freed
		 * because it really shouldn't be freed until the end of the request and it isn't
		 * easy for us to figure out when we allocated it vs. when something else might have.
		 */
		GLOBAL(php3_rqst)->status_line = pstrdup(GLOBAL(php3_rqst)->pool,&((strHeader)[9]));
	}
#else
	r = strchr(strHeader, ':');
	if (r) {
		*r = '\0';
		if (!strcasecmp(strHeader, "Content-type")) {
			if (GLOBAL(cont_type)) efree(GLOBAL(cont_type));
			GLOBAL(cont_type) = estrdup(r + 1);
#if 0 /*WIN32|WINNT / *M$ does us again*/
			if (!strcmp(GLOBAL(cont_type)," text/html")){
				*r=':';
				PUTS(arg1->value.str.val);
				PUTS("\015\012");
			}
#endif
		} else {
			*r = ':';
#if USE_SAPI
			{
				char *tempstr=emalloc(strlen(strHeader)+2);
				
				sprintf(tempstr,"%s\015\012",tempstr);
				GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,tempstr);
				efree(tempstr);
			}
#else /* CGI BINARY or FHTTPD */
#if FHTTPD
            php3_fhttpd_puts_header(strHeader);
            php3_fhttpd_puts_header("\r\n");
#else
			PUTS(strHeader);
			PUTS("\015\012");
#endif
#endif/* end if SAPI */
		}
	} else {
#if USE_SAPI
		{
		char *tempstr=emalloc(strlen(strHeader)+2);
		sprintf(tempstr,"%s\015\012",tempstr);
		GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,tempstr);
		efree(tempstr);
		}
#else /* CGI BINARY or FHTTPD */
#if FHTTPD
        php3_fhttpd_puts_header(strHeader);
        php3_fhttpd_puts_header("\r\n");
#else
		PUTS(strHeader);
		PUTS("\015\012");
#endif
#endif /* endif SAPI */
	}
#endif
}
/* }}} */

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
PHPAPI int php3_header(void)
{
#if APACHE
	CookieList *cookie;
	int len = 0;
	time_t t;
      char *dt, *cookievalue = NULL;
#endif
#if APACHE || defined(USE_SAPI) || FHTTPD
	char *tempstr;
#endif
TLS_VARS;

	if (GLOBAL(header_is_being_sent)) {
		return 0;
	} else {
		GLOBAL(header_is_being_sent) = 1;
	}

#if APACHE
	if (!GLOBAL(php3_rqst)) {  /* we're not in a request, allow output */
		GLOBAL(header_is_being_sent) = 0;
		return 1;
	}
	if ((GLOBAL(php3_PrintHeader) && !GLOBAL(php3_HeaderPrinted)) || (GLOBAL(php3_PrintHeader) && GLOBAL(php3_HeaderPrinted) == 2)) {
              if (!(GLOBAL(initialized) & INIT_ENVIRONMENT) && GLOBAL(request_info).request_method) {
                      if(!strcasecmp(GLOBAL(request_info).request_method, "post"))
                              php3_treat_data(PARSE_POST, NULL);      /* POST Data */
                      else {
                              if(!strcasecmp(GLOBAL(request_info).request_method, "put"))
                                      php3_treat_data(PARSE_PUT, NULL);       /* PUT Data */
                      }
		}
		cookie = php3_PopCookieList();
		while (cookie) {
			if (cookie->name)
				len += strlen(cookie->name);
                      if (cookie->value) {
                              cookievalue = _php3_urlencode(cookie->value, strlen (cookie->value));
                              len += strlen(cookievalue);
                      }
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
				/* FIXME: XXX: this is not binary data safe */
                              sprintf(tempstr, "%s=%s", cookie->name, cookie->value ? cookievalue : "");
				if (cookie->name) efree(cookie->name);
				if (cookie->value) efree(cookie->value);
                              if (cookievalue) efree(cookievalue);
				cookie->name=NULL;
				cookie->value=NULL;
                              cookievalue=NULL;
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
			if (cookie->domain) efree(cookie->domain);
			if (cookie->path) efree(cookie->path);
			if (cookie->name) efree(cookie->name);
			if (cookie->value) efree(cookie->value);
                      if (cookievalue) efree(cookievalue);
			efree(cookie);
			cookie = php3_PopCookieList();
			efree(tempstr);
		}
		GLOBAL(php3_HeaderPrinted) = 1;
		GLOBAL(header_called) = 1;
		send_http_header(GLOBAL(php3_rqst));
		if (GLOBAL(php3_rqst)->header_only) {
			GLOBAL(shutdown_requested) = NORMAL_SHUTDOWN;
			GLOBAL(header_is_being_sent) = 0;
			return(0);
		}
	}
#else
	if (GLOBAL(php3_PrintHeader) && !GLOBAL(php3_HeaderPrinted)) {
              if (!(GLOBAL(initialized) & INIT_ENVIRONMENT) && GLOBAL(request_info).request_method) {
                      if(!strcasecmp(GLOBAL(request_info).request_method, "post"))
                              php3_treat_data(PARSE_POST, NULL);      /* POST Data */
                      else {
                              if(!strcasecmp(GLOBAL(request_info).request_method, "put"))
                                      php3_treat_data(PARSE_PUT, NULL);       /* PUT Data */
                      }
		}
		if (php3_ini.expose_php) {
			PUTS("X-Powered-By: PHP/" PHP_VERSION "\r\n");
		}
		if (!GLOBAL(cont_type)) {
#if USE_SAPI
			GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,"Content-type: text/html\015\012\015\012");
#else /* CGI BINARY or FHTTPD */
#if FHTTPD
			php3_fhttpd_puts_header("Content-type: text/html\r\n");
#else
			PUTS("Content-type: text/html\015\012\015\012");
#endif
#endif /* endif SAPI */
		} else {
#if 0 /*WIN32|WINNT / *M$ does us again*/
			if (!strcmp(GLOBAL(cont_type),"text/html")){
#endif
#if USE_SAPI
			tempstr=emalloc(strlen(GLOBAL(cont_type))+18);
			sprintf(tempstr,"Content-type: %s\015\012\015\012",GLOBAL(cont_type));
			GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,tempstr);
			efree(tempstr);
#else /* CGI_BINARY or FHTTPD */
#if FHTTPD
			tempstr = emalloc(strlen(GLOBAL(cont_type))
							  + sizeof("Content-type:") + 2);
			if(tempstr) {
				strcpy(tempstr, "Content-type:");
				strcpy(tempstr + sizeof("Content-type:") - 1,
					   GLOBAL(cont_type));
				strcat(tempstr, "\r\n");
				php3_fhttpd_puts_header(tempstr);
				efree(tempstr);
			}
#else
			PUTS("Content-type:");
			PUTS(GLOBAL(cont_type));
			PUTS("\015\012\015\012");
#endif
#endif /* endif SAPI */
			efree(GLOBAL(cont_type));
#if 0 /*WIN32|WINNT / *M$ does us again*/
			} else {
				PUTS("\015\012");
			}/*end excluding output of text/html*/
#endif
		}
#if USE_SAPI
		GLOBAL(sapi_rqst)->flush(GLOBAL(sapi_rqst)->scid);
#else
		fflush(stdout);
#endif
		GLOBAL(php3_HeaderPrinted) = 1;
		GLOBAL(header_called) = 1;
	}
#endif
	GLOBAL(header_is_being_sent) = 0;
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

void _php3_SetCookie(char * name, char * value, time_t expires, char * path, char * domain, int secure)
{
#if !APACHE
	char *tempstr;
#if FHTTPD
	char *tempstr1;
#endif
	int len=0;
	time_t t;
	char *dt,*encoded_value=NULL;
#endif
#if APACHE
	if (name) name = estrdup(name);
	if (value) value = estrdup(value);
	if (path) path = estrdup(path);
	if (domain) domain = estrdup(domain);
	php3_PushCookieList(name, value, expires, path, domain, secure);
#else
	if (name) len += strlen(name);
	if (value) {
		encoded_value = _php3_urlencode(value, strlen (value));
		len += strlen(encoded_value);
	}
	if (path) len += strlen(path);
	if (domain) len += strlen(domain);
	tempstr = emalloc(len + 100);
	if (!value || (value && !*value)) {
		/* 
		 * MSIE doesn't delete a cookie when you set it to a null value
		 * so in order to force cookies to be deleted, even on MSIE, we
		 * pick an expiry date 1 year and 1 second in the past
		 */
		sprintf(tempstr, "%s=deleted", name);
		strcat(tempstr, "; expires=");
		t = time(NULL) - 31536001;
		dt = php3_std_date(t);
		strcat(tempstr, dt);
		efree(dt);
	} else {
		/* FIXME: XXX: this is not binary data safe */
		sprintf(tempstr, "%s=%s", name, value ? encoded_value : "");
		if (expires > 0) {
			strcat(tempstr, "; expires=");
			dt = php3_std_date(expires);
			strcat(tempstr, dt);
			efree(dt);
		}
	}

	if (encoded_value) efree(encoded_value);

	if (path && strlen(path)) {
		strcat(tempstr, "; path=");
		strcat(tempstr, path);
	}
	if (domain && strlen(domain)) {
		strcat(tempstr, "; domain=");
		strcat(tempstr, domain);
	}
	if (secure) {
		strcat(tempstr, "; secure");
	}
#if USE_SAPI
	{
	char *tempstr2=emalloc(strlen(tempstr)+14);
	sprintf(tempstr2,"Set-Cookie: %s\015\012",tempstr);
	GLOBAL(sapi_rqst)->header(GLOBAL(sapi_rqst)->scid,tempstr2);
	efree(tempstr2);
	}
#else /* CGI BINARY or FHTTPD */
#if FHTTPD
	tempstr1 = emalloc(strlen(tempstr)
					  + sizeof("Set-Cookie: ") + 2);
	if(tempstr1) {
		strcpy(tempstr1, "Set-Cookie: ");
		strcpy(tempstr1 + sizeof("Set-Cookie: ") - 1, tempstr);
		strcat(tempstr1, "\r\n");
		php3_fhttpd_puts_header(tempstr1);
		efree(tempstr1);
	}
#else
	PUTS("Set-Cookie: ");
	PUTS(tempstr);
	PUTS("\015\012");
#endif
#endif /* endif SAPI */
	efree(tempstr);
#endif
}

/* php3_SetCookie(name,value,expires,path,domain,secure) */
/* {{{ proto void setcookie(string name [, string value [, int expires [, string path [, string domain [, string secure]]]]])
   Send a cookie */
void php3_SetCookie(INTERNAL_FUNCTION_PARAMETERS)
{
	char *name = NULL, *value = NULL, *path = NULL, *domain = NULL;
	time_t expires = 0;
	int secure = 0;
	pval *arg[6];
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
			domain = arg[4]->value.str.val;
		case 4:
			convert_to_string(arg[3]);
			path = arg[3]->value.str.val;
		case 3:
			convert_to_long(arg[2]);
			expires = arg[2]->value.lval;
		case 2:
			convert_to_string(arg[1]);
			value = arg[1]->value.str.val;
		case 1:
			convert_to_string(arg[0]);
			name = arg[0]->value.str.val;
	}
	_php3_SetCookie(name, value, expires, path, domain, secure);
}
/* }}} */


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
	{"headers_sent",		php3_Headers_Sent,	NULL},
	{NULL, NULL, NULL}
};


php3_module_entry php3_header_module_entry = {
	"PHP_head", php3_header_functions, NULL, NULL, php3_init_head, NULL, NULL, STANDARD_MODULE_PROPERTIES
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
