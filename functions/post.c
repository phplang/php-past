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
/* $Id: */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include <stdio.h>
#include "parser.h"
#include "internal_functions.h"
#include "functions/mime.h"
#include "functions/type.h"
#include "functions/php3_string.h"
#include "functions/post.h"
#include "functions/url.h"

#ifndef THREAD_SAFE
int php3_track_vars;
#endif

/*
 * php3_getpost()
 *
 * This reads the post form data into a string.
 * Remember to free this pointer when done with it.
 */
static char *php3_getpost(void)
{
	char *buf = NULL;
#if MODULE_MAGIC_NUMBER > 19961007
	char argsbuffer[HUGE_STRING_LEN];
#else
	int bytes;
#endif
	int length, cnt;
	int file_upload = 0;
	char *mb;
	char boundary[100];
	TLS_VARS;
	
	buf = GLOBAL(request_info).content_type;
	if (!buf) {
		php3_error(E_WARNING, "POST Error: content-type missing");
		return (NULL);
	}
	if (strncasecmp(buf, "application/x-www-form-urlencoded", 33) && strncasecmp(buf, "multipart/form-data", 19)) {
		php3_error(E_WARNING, "Unsupported content-type: %s", buf);
		return (NULL);
	}
	if (!strncasecmp(buf, "multipart/form-data", 19)) {
		file_upload = 1;
		mb = strchr(buf, '=');
		if (mb)
			strncpy(boundary, mb + 1, sizeof(boundary));
		else {
			php3_error(E_WARNING, "File Upload Error: No MIME boundary found");
			php3_error(E_WARNING, "There should have been a \"boundary=something\" in the Content-Type string");
			php3_error(E_WARNING, "The Content-Type string was: \"%s\"", buf);
			return (NULL);
		}
	}
	length = GLOBAL(request_info).content_length;
	cnt = length;
	buf = (char *) emalloc((length + 1) * sizeof(char));
	if (!buf) {
		php3_error(E_WARNING, "Unable to allocate memory in php3_getpost()");
		return (NULL);
	}
#if MODULE_MAGIC_NUMBER > 19961007
	if (should_client_block(GLOBAL(php3_rqst))) {
		void (*handler) (int);
		int dbsize, len_read, dbpos = 0;

		hard_timeout("copy script args", GLOBAL(php3_rqst));	/* start timeout timer */
		handler = signal(SIGPIPE, SIG_IGN);		/* Ignore sigpipes for now */
		while ((len_read = get_client_block(GLOBAL(php3_rqst), argsbuffer, HUGE_STRING_LEN)) > 0) {
			if ((dbpos + len_read) > length)
				dbsize = length - dbpos;
			else
				dbsize = len_read;
			reset_timeout(GLOBAL(php3_rqst));	/* Make sure we don't timeout */
			memcpy(buf + dbpos, argsbuffer, dbsize);
			dbpos += dbsize;
		}
		signal(SIGPIPE, handler);	/* restore normal sigpipe handling */
		kill_timeout(GLOBAL(php3_rqst));	/* stop timeout timer */
	}
#else
	cnt = 0;
	do {
#if APACHE
		bytes = read_client_block(php3_rqst, buf + cnt, length - cnt);
#endif
#if CGI_BINARY
		bytes = fread(buf + cnt, 1, length - cnt, stdin);
#endif
#if USE_SAPI
		bytes = GLOBAL(sapi_rqst)->readclient(GLOBAL(sapi_rqst)->scid,buf + cnt, 1, length - cnt);
#endif
		cnt += bytes;
	} while (bytes && cnt < length);
#endif
	if (file_upload) {
		php3_mime_split(buf, cnt, boundary);
		efree(buf);
		return (NULL);
	}
	buf[cnt] = '\0';
	return (buf);
}


static void php3_dot_to_underscore(char *str)
{
	char *s = str;
	while (*s) {
		if (*s == '.')
			*s = '_';
		s++;
	}
}

/*
 * parse Get/Post/Cookie string and create appropriate variable
 *
 * This is a tad ugly because it was yanked out of the middle of
 * the old TreatData function.  This is a temporary measure filling 
 * the gap until a more flexible parser can be built to do this.
 */
void _php3_parse_gpc_data(char *t, char *s, YYSTYPE *track_vars_array)
{
	int itype;
	char *ind, *tmp = NULL, *ret = NULL, *u = NULL;
	TLS_VARS;
	
	php3_dot_to_underscore(s);
	itype = php3_CheckIdentType(s);
	if (itype == 2) {	/* indexed array */
		ind = php3_GetIdentIndex(s);
		if(php3_ini.magic_quotes_gpc) {
			ret = _php3_addslashes(ind, 1);
		} else {
			ret = ind;
		}
	}
	if (itype) {		/* non-indexed array */
		u = strchr(s, '[');
		if (u)
			*u = '\0';
	}
	if (strlen(s)==0) { /* empty variable name, FIXME? */
		return;
	}
	/*
	   Here PHP2 had a check to prevent a GET or COOKIE variable
	   from overwriting a variable set via the POST method.
	   Not sure how I can do this in PHP3, and I am not sure I
	   even need to do it.
	 */

	tmp = estrdup(t);
	if (itype) {
		YYSTYPE arr1, arr2, *arr_ptr, entry;

		/* If the array doesn't exist, create it */
		if (hash_find(GLOBAL(active_symbol_table), s, strlen(s)+1, (void **) &arr_ptr) == FAILURE) {
			if (array_init(&arr1)==FAILURE) {
				return;
			}
			hash_update(GLOBAL(active_symbol_table), s, strlen(s)+1, &arr1, sizeof(YYSTYPE), NULL);
			if (track_vars_array) {
				if (array_init(&arr2)==FAILURE) {
					return;
				}
				hash_update(track_vars_array->value.ht, s, strlen(s)+1, (void *) &arr2, sizeof(YYSTYPE),NULL);
			}
		} else {
			if (arr_ptr->type!=IS_ARRAY) {
				yystype_destructor(arr_ptr _INLINE_TLS);
				if (array_init(arr_ptr)==FAILURE) {
					return;
				}
			}
			arr1 = *arr_ptr;
			if (track_vars_array && hash_find(track_vars_array->value.ht, s, strlen(s)+1, (void **) &arr_ptr) == FAILURE) {
				return;
			}
			arr2 = *arr_ptr;
		}
		/* Now create the element */
		if (php3_ini.magic_quotes_gpc) {
			entry.value.strval = _php3_addslashes(tmp, 0);
			entry.strlen = strlen(entry.value.strval);
		} else {
			entry.strlen = strlen(tmp);
			entry.value.strval = estrndup(tmp,entry.strlen);
		}
		entry.type = IS_STRING;

		/* And then insert it */
		if (ret) {		/* indexed array */
			if (php3_CheckType(ret) == IS_LONG) {
				hash_index_update(arr1.value.ht, atol(ret), &entry, sizeof(YYSTYPE),NULL);	/* s[ret]=tmp */
				if (track_vars_array) {
					yystype_copy_constructor(&entry);
					hash_index_update(arr2.value.ht, atol(ret), &entry, sizeof(YYSTYPE),NULL);
				}
			} else {
				hash_update(arr1.value.ht, ret, strlen(ret)+1, &entry, sizeof(YYSTYPE),NULL);	/* s["ret"]=tmp */
				if (track_vars_array) {
					yystype_copy_constructor(&entry);
					hash_update(arr2.value.ht, ret, strlen(ret)+1, &entry, sizeof(YYSTYPE),NULL);
				}
			}
			efree(ret);
			ret = NULL;
		} else {		/* non-indexed array */
			hash_next_index_insert(arr1.value.ht, &entry, sizeof(YYSTYPE),NULL);
			if (track_vars_array) {
				yystype_copy_constructor(&entry);
				hash_next_index_insert(arr2.value.ht, &entry, sizeof(YYSTYPE),NULL);
			}
		}
	} else {			/* we have a normal variable */
		YYSTYPE entry;
		
		if (php3_ini.magic_quotes_gpc) {
			entry.value.strval = _php3_addslashes(tmp, 0);
			entry.strlen = strlen(entry.value.strval);
		} else {
			entry.strlen = strlen(tmp);
			entry.value.strval = estrndup(tmp,entry.strlen);
		}
		entry.type = IS_STRING;
		hash_update(&GLOBAL(symbol_table), s, strlen(s)+1, (void *) &entry, sizeof(YYSTYPE),NULL);
		if (track_vars_array) {
			yystype_copy_constructor(&entry);
			hash_update(track_vars_array->value.ht, s, strlen(s)+1, (void *) &entry, sizeof(YYSTYPE),NULL);
		}
	}

	if (tmp) efree(tmp);
}


void php3_treat_data(int arg, char *str)
{
	char *res = NULL, *s, *t, *tt;
	char o = '\0';
	int inc = 0;
	YYSTYPE array,*array_ptr;
	TLS_VARS;
	
	switch (arg) {
		case PARSE_POST:
		case PARSE_GET:
		case PARSE_COOKIE:
			if (GLOBAL(php3_track_vars)) {
				array_init(&array);
				array_ptr = &array;
				switch (arg) {
					case PARSE_POST:
						hash_add(&GLOBAL(symbol_table), "HTTP_POST_VARS", sizeof("HTTP_POST_VARS"), array_ptr, sizeof(YYSTYPE),NULL);
						break;
					case PARSE_GET:
						hash_add(&GLOBAL(symbol_table), "HTTP_GET_VARS", sizeof("HTTP_GET_VARS"), array_ptr, sizeof(YYSTYPE),NULL);
						break;
					case PARSE_COOKIE:
						hash_add(&GLOBAL(symbol_table), "HTTP_COOKIE_VARS", sizeof("HTTP_COOKIE_VARS"), array_ptr, sizeof(YYSTYPE),NULL);
						break;
				}
			} else {
				array_ptr=NULL;
			}
			break;
		default:
			array_ptr=NULL;
			break;
	}

	if (arg == PARSE_POST)
		res = php3_getpost();

	else if (arg == PARSE_GET) {		/* Get data */
		s = GLOBAL(request_info).query_string;
		res = s;
		if (s && *s) {
			res = (char *) estrdup(s);
		}
		inc = -1;
	} else if (arg == PARSE_COOKIE) {		/* Cookie data */
		s = GLOBAL(request_info).cookies;
		res = s;
		if (s && *s) {
			res = (char *) estrdup(s);
		}
		inc = -1;
	} else if (arg == PARSE_STRING) {		/* String data */
		res = str;
		inc = -1;
	}
	if (!res)
		return;
	if (!*res) {
		return;
	}
	if (arg == PARSE_COOKIE) {
		s = strtok(res, ";");
	} else if(arg == PARSE_POST) {
		s = strtok(res, "&");
	} else {
		s = strtok(res, php3_ini.arg_separator);
	}
	

	while (s) {
		t = strchr(s, '=');
		if (t) {
			*t = '\0';
			tt = strchr(s, '+');
			while (tt) {
				s = tt + 1;
				tt = strchr(s, '+');
			}
			if (arg == PARSE_COOKIE) {
				tt = strchr(s, ' ');
				while (tt) {
					s = tt + 1;
					tt = strchr(s, ' ');
				}
			}
			_php3_urldecode(s);
			_php3_urldecode(t+1);
			_php3_parse_gpc_data(t+1,s,array_ptr);
			if (tt)
				*tt = o;
		}
		if (arg == PARSE_COOKIE) {
			s = strtok(NULL, ";");
		} else if(arg == PARSE_POST) {
			s = strtok(NULL, "&");
		} else {
			s = strtok(NULL, php3_ini.arg_separator);
		}
	}
	efree(res);
}


void php3_TreatHeaders(void)
{
#if APACHE
#if MODULE_MAGIC_NUMBER > 19961007
	const char *s = NULL;
#else
	char *s = NULL;
#endif
	char *t;
	char *user, *type;
	TLS_VARS;

	if (GLOBAL(php3_rqst)->headers_in)
		s = table_get(GLOBAL(php3_rqst)->headers_in, "Authorization");
	if (!s)
		return;

	/* Check to make sure that this URL isn't authenticated
	   using a traditional auth module mechanism */
	if (auth_type(GLOBAL(php3_rqst))) {
		/*php3_error(E_WARNING, "Authentication done by server module\n");*/
		return;
	}
	if (strcmp(getword(GLOBAL(php3_rqst)->pool, &s, ' '), "Basic")) {
		/* Client tried to authenticate using wrong auth scheme */
		php3_error(E_WARNING, "client used wrong authentication scheme", GLOBAL(php3_rqst)->uri, GLOBAL(php3_rqst));
		return;
	}
	t = uudecode(GLOBAL(php3_rqst)->pool, s);
#if MODULE_MAGIC_NUMBER > 19961007
	user = getword_nulls_nc(GLOBAL(php3_rqst)->pool, &t, ':');
#else
	user = getword(GLOBAL(php3_rqst)->pool, &t, ':');
#endif
	type = "Basic";

	if (user) {
		if(php3_ini.magic_quotes_gpc) {
			SET_VAR_STRING("PHP_AUTH_USER", _php3_addslashes(user, 0));
		} else {
			SET_VAR_STRING("PHP_AUTH_USER", estrdup(user));
		}
	}
	if (t) {
		if(php3_ini.magic_quotes_gpc) {
			SET_VAR_STRING("PHP_AUTH_PW", _php3_addslashes(t, 0));
		} else {
			SET_VAR_STRING("PHP_AUTH_PW", estrdup(t));
		}
	}
	if (type) {
		if(php3_ini.magic_quotes_gpc) {
			SET_VAR_STRING("PHP_AUTH_TYPE", _php3_addslashes(type, 0));
		} else {
			SET_VAR_STRING("PHP_AUTH_TYPE", estrdup(type));
		}
	}
#endif
}

void php3_parsestr(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	char *res = NULL;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);
	if (arg->value.strval && *arg->value.strval) {
		res = estrndup(arg->value.strval,arg->strlen);
	}
	php3_treat_data(3, res);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
