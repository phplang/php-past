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
/* $Id: */
#include <stdio.h>
#include "php.h"
#include "internal_functions.h"
#include "php3_list.h"
#include "functions/mime.h"
#include "functions/type.h"
#include "functions/php3_string.h"
#include "functions/post.h"
#include "functions/url.h"

int php3_track_vars;
extern int le_uploads;
extern HashTable list;

/*
 * php3_getput()
 *
 * This copies the uploaded file to a temporary file.
 */
static void php3_getput(void) {
#if MODULE_MAGIC_NUMBER > 19961007
	char upload_buffer[BUFSIZ];
#endif
	size_t bytes=0;
	char *fn;
	FILE *fp;
	int length, cnt;

	length = GLOBAL(request_info).content_length;
	if (length > php3_ini.upload_max_filesize) {
		php3_error(E_WARNING, "Max file size of %ld bytes exceeded - temporary file not saved", php3_ini.upload_max_filesize);
		SET_VAR_STRING("PHP_PUT_FILENAME", estrdup("none"));
		return;
	}
	fn = tempnam(php3_ini.upload_tmp_dir, "php");
	fp = fopen(fn, "w");
	if (!fp) {
		php3_error(E_WARNING, "File Upload Error - Unable to open temporary file [%s]", fn);
		return;
	}
	cnt = length;
#if FHTTPD
	bytes = fwrite(req->databuffer, 1, length, fp);
#else
#if MODULE_MAGIC_NUMBER > 19961007
	if (should_client_block(GLOBAL(php3_rqst))) {
		void (*handler) (int);
		int dbsize, len_read, dbpos = 0;

		hard_timeout("copy script args", GLOBAL(php3_rqst));    /* start timeout timer */
		handler = signal(SIGPIPE, SIG_IGN);             /* Ignore sigpipes for now */
		while ((len_read = get_client_block(GLOBAL(php3_rqst), upload_buffer, BUFSIZ)) > 0) {
			if ((dbpos + len_read) > length) dbsize = length - dbpos;
			else dbsize = len_read;
			reset_timeout(GLOBAL(php3_rqst));       /* Make sure we don't timeout */
			if((bytes=fwrite(upload_buffer, 1, dbsize, fp))<dbsize) {
				bytes+=dbpos;
				break;
			}
			dbpos += dbsize;
			bytes=dbpos;
		}
		signal(SIGPIPE, handler);       /* restore normal sigpipe handling */
		kill_timeout(GLOBAL(php3_rqst));        /* stop timeout timer */
	}
#else
	cnt = 0;
	do {
		char upload_buffer[BUFSIZ];

#if APACHE
		bytes = read_client_block(php3_rqst, upload_buffer + cnt, min(length - cnt,BUFSIZ));
#endif
#if CGI_BINARY
		bytes = fread(upload_buffer + cnt, 1, ((length-cnt)<BUFSIZ)?length-cnt:BUFSIZ, stdin);
#endif
#if USE_SAPI
		bytes = GLOBAL(sapi_rqst)->readclient(GLOBAL(sapi_rqst)->scid,upload_buffer + cnt, 1, min(length - cnt,BUFSIZ));
#endif
		cnt += bytes;
	} while (bytes && cnt < length);
#endif
#endif
	fclose(fp);
	if (bytes < (size_t)length) {
		php3_error(E_WARNING, "Only %d bytes were written, expected to write %ld", bytes, length);
	}
	SET_VAR_STRING("PHP_PUT_FILENAME", estrdup(fn));
	php3_list_do_insert(&GLOBAL(list),fn,GLOBAL(le_uploads));  /* Tell PHP about the file so the destructor can unlink it later */
}


/*
 * php3_getpost()
 *
 * This reads the post form data into a string.
 * Remember to free this pointer when done with it.
 */
static char *php3_getpost(pval *http_post_vars)
{
	char *buf = NULL;
	const char *ctype;
#if MODULE_MAGIC_NUMBER > 19961007
	char argsbuffer[HUGE_STRING_LEN];
#else
	int bytes;
#endif
	int length, cnt;
	int file_upload = 0;
	int unknown_content_type = 1;
	char *mb;
	char boundary[100];
	TLS_VARS;
	
	ctype = GLOBAL(request_info).content_type;
	if (!ctype) {
		php3_error(E_NOTICE, "POST Error: content-type missing");
	}
	else if (strncasecmp(ctype, "application/x-www-form-urlencoded", 33) && strncasecmp(ctype, "multipart/form-data", 19)
#if HAVE_FDFLIB
 && strncasecmp(ctype, "application/vnd.fdf", 19)
#endif
      ) {
		php3_error(E_NOTICE, "Unknown POST content-type: %s", ctype);
	}
	else {
		unknown_content_type = 0;
	}

	/* if this is a POST file upload, figure out the boundary */
	if (ctype && !strncasecmp(ctype, "multipart/form-data", 19)) {
		file_upload = 1;
		mb = strchr(ctype, '=');
		if (mb) {
			size_t len;

			mb++;
			
			len = strlen(mb);
			if (len >= sizeof(boundary)) {
				php3_error(E_WARNING, "File Upload Error: Boundary length exceeds limit.");
				return NULL;
			}

			if (mb[0] == '"' && mb[len-1] == '"') {
				memcpy(boundary, mb + 1, len - 2);
				boundary[len - 2] = '\0';
			} else {
				/* Copies the NUL byte */
				memcpy(boundary, mb, len + 1);
			}
		} else {
			php3_error(E_WARNING, "File Upload Error: No MIME boundary found");
			php3_error(E_WARNING, "There should have been a \"boundary=something\" in the Content-Type string");
			php3_error(E_WARNING, "The Content-Type string was: \"%s\"", ctype);
			return NULL;
		}
	}

	/* read in all of the data */
	length = GLOBAL(request_info).content_length;
	cnt = length;
	buf = (char *) emalloc((length + 1) * sizeof(char));
	if (!buf) {
		php3_error(E_WARNING, "Unable to allocate memory in php3_getpost()");
		return NULL;
	}
#if FHTTPD
	memcpy(buf,req->databuffer,length);
	buf[length]=0;
#else
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
#endif

	/* if it was a POST file upload, pass it off to someone else */
	if (file_upload) {
		php3_mime_split(buf, cnt, boundary, http_post_vars);
		efree(buf);
		return NULL;
	}
	buf[cnt] = '\0';

#if HAVE_FDFLIB
	if (ctype && !strncasecmp(ctype, "application/vnd.fdf", 19)) {
		pval postdata, *postdata_ptr;
		postdata_ptr = &postdata;
		postdata_ptr->type = IS_STRING;
		postdata_ptr->value.str.val = (char *) estrdup(buf);
		postdata_ptr->value.str.len = cnt;
		_php3_hash_add(&GLOBAL(symbol_table), "HTTP_FDF_DATA", sizeof("HTTP_FDF_DATA"), postdata_ptr, sizeof(pval),NULL);
	}
#endif

	if (unknown_content_type) {
		pval rawdata;
		rawdata.type = IS_STRING;
		/* we don't need to copy buf, it was allocated with emalloc.
		   and we return NULL so nobody else will be trying to free it.
		*/
		rawdata.value.str.val = buf;
		rawdata.value.str.len = cnt;
		_php3_hash_add(&GLOBAL(symbol_table), "HTTP_RAW_POST_DATA", sizeof("HTTP_RAW_POST_DATA"), &rawdata, sizeof(pval), NULL);
		return NULL;
	}

	return (buf);
}


/*
 * parse Get/Post/Cookie string and create appropriate variable
 *
 * This is a tad ugly because it was yanked out of the middle of
 * the old TreatData function.  This is a temporary measure filling 
 * the gap until a more flexible parser can be built to do this.
 */
void _php3_parse_gpc_data(char *val, char *var, pval *track_vars_array)
{
	int var_type;
	char *ind, *tmp = NULL, *ret = NULL;
	int var_len;
	TLS_VARS;
	
	var_type = php3_check_ident_type(var);
	if (var_type == GPC_INDEXED_ARRAY) {
		ind = php3_get_ident_index(var);
		if (php3_ini.magic_quotes_gpc) {
			ret = _php3_addslashes(ind, 0, NULL, 1);
		} else {
			ret = ind;
		}
	}
	if (var_type & GPC_ARRAY) {		/* array (indexed or not */
		tmp = strchr(var, '[');
		if (tmp) {
			*tmp = '\0';
		}
	}
	/* ignore leading spaces in the variable name */
	while (*var && *var==' ') {
		var++;
	}
	var_len = strlen(var);
	if (var_len==0) { /* empty variable name, or variable name with a space in it */
		return;
	}

	/* ensure that we don't have spaces or dots in the variable name (not binary safe) */
	for (tmp=var; *tmp; tmp++) {
		switch(*tmp) {
			case ' ':
			case '.':
				*tmp='_';
				break;
		}
	}

	tmp = estrdup(val);
	if (var_type & GPC_ARRAY) {
		pval arr1, arr2, *arr_ptr, entry;

		/* If the array doesn't exist, create it */
		if (_php3_hash_find(GLOBAL(active_symbol_table), var, var_len+1, (void **) &arr_ptr) == FAILURE) {
			if (array_init(&arr1)==FAILURE) {
				return;
			}
			_php3_hash_update(GLOBAL(active_symbol_table), var, var_len+1, &arr1, sizeof(pval), NULL);
			if (track_vars_array) {
				if (array_init(&arr2)==FAILURE) {
					return;
				}
				_php3_hash_update(track_vars_array->value.ht, var, var_len+1, (void *) &arr2, sizeof(pval),NULL);
			}
		} else {
			if (arr_ptr->type!=IS_ARRAY) {
				pval_destructor(arr_ptr _INLINE_TLS);
				if (array_init(arr_ptr)==FAILURE) {
					return;
				}
				if (track_vars_array) {
					if (array_init(&arr2)==FAILURE) {
						return;
					}
					_php3_hash_update(track_vars_array->value.ht, var, var_len+1, (void *) &arr2, sizeof(pval),NULL);
				}
			}
			arr1 = *arr_ptr;
			if (track_vars_array && _php3_hash_find(track_vars_array->value.ht, var, var_len+1, (void **) &arr_ptr) == FAILURE) {
				return;
			}
			arr2 = *arr_ptr;
		}
		/* Now create the element */
		if (php3_ini.magic_quotes_gpc) {
			entry.value.str.val = _php3_addslashes(tmp, 0, &entry.value.str.len, 0);
		} else {
			entry.value.str.len = strlen(tmp);
			entry.value.str.val = estrndup(tmp,entry.value.str.len);
		}
		entry.type = IS_STRING;

		/* And then insert it */
		if (ret) {		/* indexed array */
			if (php3_check_type(ret) == IS_LONG) {
				_php3_hash_index_update(arr1.value.ht, atol(ret), &entry, sizeof(pval),NULL);	/* s[ret]=tmp */
				if (track_vars_array) {
					pval_copy_constructor(&entry);
					_php3_hash_index_update(arr2.value.ht, atol(ret), &entry, sizeof(pval),NULL);
				}
			} else {
				_php3_hash_update(arr1.value.ht, ret, strlen(ret)+1, &entry, sizeof(pval),NULL);	/* s["ret"]=tmp */
				if (track_vars_array) {
					pval_copy_constructor(&entry);
					_php3_hash_update(arr2.value.ht, ret, strlen(ret)+1, &entry, sizeof(pval),NULL);
				}
			}
			efree(ret);
			ret = NULL;
		} else {		/* non-indexed array */
			_php3_hash_next_index_insert(arr1.value.ht, &entry, sizeof(pval),NULL);
			if (track_vars_array) {
				pval_copy_constructor(&entry);
				_php3_hash_next_index_insert(arr2.value.ht, &entry, sizeof(pval),NULL);
			}
		}
	} else {			/* we have a normal variable */
		pval entry;
		
		if (php3_ini.magic_quotes_gpc) {
			entry.value.str.val = _php3_addslashes(tmp, 0, &entry.value.str.len, 0);
		} else {
			entry.value.str.len = strlen(tmp);
			entry.value.str.val = estrndup(tmp,entry.value.str.len);
		}
		entry.type = IS_STRING;
		_php3_hash_update(GLOBAL(active_symbol_table), var, var_len+1, (void *) &entry, sizeof(pval),NULL);
		if (track_vars_array) {
			pval_copy_constructor(&entry);
			_php3_hash_update(track_vars_array->value.ht, var, var_len+1, (void *) &entry, sizeof(pval),NULL);
		}
	}

	if (tmp) efree(tmp);
}


void php3_treat_data(int arg, char *str)
{
	char *res = NULL, *var, *val;
	pval array,*array_ptr;
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
						_php3_hash_add(&GLOBAL(symbol_table), "HTTP_POST_VARS", sizeof("HTTP_POST_VARS"), array_ptr, sizeof(pval),NULL);
						break;
					case PARSE_GET:
						_php3_hash_add(&GLOBAL(symbol_table), "HTTP_GET_VARS", sizeof("HTTP_GET_VARS"), array_ptr, sizeof(pval),NULL);
						break;
					case PARSE_COOKIE:
						_php3_hash_add(&GLOBAL(symbol_table), "HTTP_COOKIE_VARS", sizeof("HTTP_COOKIE_VARS"), array_ptr, sizeof(pval),NULL);
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

	if (arg == PARSE_POST) {
		res = php3_getpost(array_ptr);
	} else if (arg == PARSE_GET) {		/* Get data */
		var = GLOBAL(request_info).query_string;
		if (var && *var) {
			res = (char *) estrdup(var);
		}
	} else if (arg == PARSE_COOKIE) {		/* Cookie data */
		var = (char *)GLOBAL(request_info).cookies;
		if (var && *var) {
			res = (char *) estrdup(var);
		}
	} else if (arg == PARSE_STRING) {		/* String data */
		res = str;
      } else if (arg == PARSE_PUT) {          /* Put data */
              php3_getput();
              return;
	}
	if (!res) {
		return;
	}
	
	if (arg == PARSE_COOKIE) {
		var = strtok(res, ";");
	} else if (arg == PARSE_POST) {
		var = strtok(res, "&");
	} else {
		var = strtok(res, php3_ini.arg_separator);
	}

	while (var) {
		val = strchr(var, '=');
		if (val) { /* have a value */
			*val++ = '\0';
			/* FIXME: XXX: not binary safe, discards returned length */
			_php3_urldecode(var, strlen(var));
			_php3_urldecode(val, strlen(val));
			_php3_parse_gpc_data(val,var,array_ptr);
		}
		if (arg == PARSE_COOKIE) {
			var = strtok(NULL, ";");
		} else if (arg == PARSE_POST) {
			var = strtok(NULL, "&");
		} else {
			var = strtok(NULL, php3_ini.arg_separator);
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
	int len;
	char *escaped_str;
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
	if (strcmp(t=getword(GLOBAL(php3_rqst)->pool, &s, ' '), "Basic")) {
		/* Client tried to authenticate using wrong auth scheme */
		php3_error(E_WARNING, "client used wrong authentication scheme (%s)", t);
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
		if (php3_ini.magic_quotes_gpc) {
			escaped_str = _php3_addslashes(user, 0, &len, 0);
			SET_VAR_STRINGL("PHP_AUTH_USER", escaped_str, len);
		} else {
			SET_VAR_STRING("PHP_AUTH_USER", estrdup(user));
		}
	}
	if (t) {
		if (php3_ini.magic_quotes_gpc) {
			escaped_str = _php3_addslashes(t, 0, &len, 0);
			SET_VAR_STRINGL("PHP_AUTH_PW", escaped_str, len);
		} else {
			SET_VAR_STRING("PHP_AUTH_PW", estrdup(t));
		}
	}
	if (type) {
		if (php3_ini.magic_quotes_gpc) {
			escaped_str = _php3_addslashes(type, 0, &len, 0);
			SET_VAR_STRINGL("PHP_AUTH_TYPE", escaped_str, len);
		} else {
			SET_VAR_STRING("PHP_AUTH_TYPE", estrdup(type));
		}
	}
#endif
#if FHTTPD
	int i,len;
	struct rline *l;
	char *type;
	char *escaped_str;

	if(req && req->remote_user){
		for(i=0; i < req->nlines; i++){
			l=req->lines+i;
			if((l->paramc > 1)&&!strcasecmp(l->params[0], "REMOTE_PW")){
				type = "Basic";
				if (php3_ini.magic_quotes_gpc) {
					escaped_str = _php3_addslashes(type, 0, &len, 0);
					SET_VAR_STRINGL("PHP_AUTH_TYPE", escaped_str, len);
					escaped_str = _php3_addslashes(l->params[1], 0, &len, 0);
					SET_VAR_STRINGL("PHP_AUTH_PW", escaped_str, len);
					escaped_str = _php3_addslashes(req->remote_user, 0, &len, 0);
					SET_VAR_STRINGL("PHP_AUTH_USER", escaped_str, len);

				} else {
					SET_VAR_STRING("PHP_AUTH_TYPE", estrdup(type));
					SET_VAR_STRING("PHP_AUTH_PW", estrdup(l->params[1]));
					SET_VAR_STRING("PHP_AUTH_USER", estrdup(req->remote_user));
				}
				i=req->nlines;
			}
		}
	}
#endif
}

/* {{{ proto void parse_str(string str)
   Parses str as if it were the query string passed via an URL and sets variables in the current scope */
void php3_parsestr(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg;
	char *res = NULL;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);
	if (arg->value.str.val && *arg->value.str.val) {
		res = estrndup(arg->value.str.val,arg->value.str.len);
	}
	php3_treat_data(PARSE_STRING, res);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
