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
   |          Stefan Röhrich <sr@linux.de>                                |
   +----------------------------------------------------------------------+
 */
/* $Id: zlib.c,v 1.7 1998/08/14 23:47:16 steffann Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if MSVC5
#include <windows.h>
#include <winsock.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#else
#include <sys/param.h>
/* #include <sys/uio.h> */
#endif
#include "head.h"
#include "internal_functions.h"
#include "safe_mode.h"
#include "php3_list.h"
#include "php3_string.h"
#include "php3_zlib.h"
#include "fopen-wrappers.h"
#if HAVE_PWD_H
#if WIN32|WINNT
#ifdef HAVE_UNISTD_H
#undef HAVE_UNISTD_H
#endif
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#include "snprintf.h"
#if HAVE_ZLIB
#include <zlib.h>

#if COMPILE_DL
#include "dl/phpdl.h"
#ifndef PUTS
#define PUTS(a) php3_printf("%s",a)
#endif
#ifndef PUTC
#define PUTC(a) PUTS(a)
#endif
#ifndef PHPWRITE
#define PHPWRITE(a,n) php3_write((a),(n))
#endif
#endif

#if defined(THREAD_SAFE)
DWORD ZLIBtls;
static int numthreads=0;
void *zlib_mutex;

typedef struct zlib_global_struct{
	int gzgetss_state;
	int le_zp;
}zlib_global_struct;

#define ZLIB_GLOBAL(a) zlib_globals->a

#define ZLIB_TLS_VARS \
	zlib_global_struct *zlib_globals; \
	zlib_globals=TlsGetValue(ZLIBtls); 

#else
static int gzgetss_state = 0;
int le_zp;
#define ZLIB_GLOBAL(a) a
#define ZLIB_TLS_VARS
#endif

function_entry php3_zlib_functions[] = {
	{"readgzfile",	php3_readgzfile,	NULL},
	{"gzrewind",	php3_gzrewind,   	NULL},
	{"gzclose",		php3_gzclose,		NULL},
	{"gzeof",		php3_gzeof,			NULL},
	{"gzgetc",		php3_gzgetc,		NULL},
	{"gzgets",		php3_gzgets,		NULL},
	{"gzgetss",		php3_gzgetss,		NULL},
	{"gzread",		php3_gzread,		NULL},
	{"gzopen",		php3_gzopen,		NULL},
	{"gzpassthru",	php3_gzpassthru,   	NULL},
	{"gzseek",		php3_gzseek,	   	NULL},
	{"gztell",		php3_gztell,	   	NULL},
	{"gzwrite",		php3_gzwrite,		NULL},
	{"gzputs",		php3_gzwrite,		NULL},
	{"gzfile",		php3_gzfile,   		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry php3_zlib_module_entry = {
	"zlib", php3_zlib_functions, php3_minit_zlib, php3_mshutdown_zlib, NULL, NULL, php3_info_zlib, STANDARD_MODULE_PROPERTIES
};

#if defined(COMPILE_DL)
DLEXPORT php3_module_entry *get_module(void) { return &php3_zlib_module_entry; }

#if (WIN32|WINNT) && defined(THREAD_SAFE)

/*NOTE: You should have an odbc.def file where you
export DllMain*/
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    return 1;
}
#endif
#endif

int php3_minit_zlib(INIT_FUNC_ARGS)
{
#if defined(THREAD_SAFE)
	zlib_global_struct *zlib_globals;
#if !defined(COMPILE_DL)
	CREATE_MUTEX(zlib_mutex,"ZLib_TLS");
	SET_MUTEX(zlib_mutex);
	numthreads++;
	if (numthreads==1){
	if ((ZLIBtls=TlsAlloc())==0xFFFFFFFF){
		FREE_MUTEX(zlib_mutex);
		return 0;
	}}
	FREE_MUTEX(zlib_mutex);
#endif
	zlib_globals = (zlib_global_struct *) LocalAlloc(LPTR, sizeof(zlib_global_struct)); 
	TlsSetValue(ZLIBtls, (void *) zlib_globals);
#endif
	
	ZLIB_GLOBAL(le_zp) = register_list_destructors(gzclose,NULL);
	return SUCCESS;
}

int php3_mshutdown_zlib(void){
#if defined(THREAD_SAFE)
	zlib_global_struct *zlib_globals;
	zlib_globals = TlsGetValue(ZLIBtls); 
	if (zlib_globals != 0) 
		LocalFree((HLOCAL) zlib_globals); 
#if !defined(COMPILE_DL)
	SET_MUTEX(zlib_mutex);
	numthreads--;
	if (!numthreads){
	if (!TlsFree(ZLIBtls)){
		FREE_MUTEX(zlib_mutex);
		return 0;
	}}
	FREE_MUTEX(zlib_mutex);
#endif
#endif
	return SUCCESS;
}

void php3_info_zlib(void)
{
		PUTS("Zlib support active (compiled with ");
		PUTS(ZLIB_VERSION);
		PUTS(", linked with ");
		PUTS(zlibVersion());
		PUTS(").");
}

static gzFile *php3_gzopen_with_path(char *filename, char *mode, char *path, char **opened_path);

static gzFile php3_gzopen_wrapper(char *path, char *mode, int options)
{
	if (options & USE_PATH && php3_ini.include_path != NULL) {
		return php3_gzopen_with_path(path, mode, php3_ini.include_path, NULL);
	}
	else {
		if (options & ENFORCE_SAFE_MODE && php3_ini.safe_mode && (!_php3_checkuid(path,1))) {
			php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner of file to be read.");
			return NULL;
		}
		if (_php3_check_open_basedir(path)) return NULL;
		return gzopen(path, mode);
	}
}

/*
 * Tries to open a .gz-file with a PATH-style list of directories.
 * If the filename starts with "." or "/", the path is ignored.
 */
static gzFile *php3_gzopen_with_path(char *filename, char *mode, char *path, char **opened_path)
{
	char *pathbuf, *ptr, *end;
	char trypath[MAXPATHLEN + 1];
	struct stat sb;
	gzFile *zp;
	
	if (opened_path) {
		*opened_path = NULL;
	}
	
	/* Relative path open */
	if (*filename == '.') {
		if (php3_ini.safe_mode &&(!_php3_checkuid(filename,2))) {
			php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner.");
			return(NULL);
		}
		if (_php3_check_open_basedir(filename)) return NULL;
		zp = gzopen(filename, mode);
		if (zp && opened_path) {
			*opened_path = expand_filepath(filename);
		}
		return zp;
	}

	/* Absolute path open - prepend document_root in safe mode */
#if WIN32|WINNT
	if ((*filename == '\\')||(*filename == '/')||(filename[1] == ':')) {
#else
	if (*filename == '/') {
#endif
		if (php3_ini.safe_mode) {
			snprintf(trypath,MAXPATHLEN,"%s%s",php3_ini.doc_root,filename);
			if (!_php3_checkuid(trypath,2)) {
				php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner.");
				return(NULL);
			}
			if (_php3_check_open_basedir(trypath)) return NULL;
			zp = gzopen(trypath, mode);
			if (zp && opened_path) {
				*opened_path = expand_filepath(trypath);
			}
			return zp;
		} else {
			if (_php3_check_open_basedir(filename)) return NULL;
			return gzopen(filename, mode);
		}
	}

	if (!path || (path && !*path)) {
		if (php3_ini.safe_mode &&(!_php3_checkuid(filename,2))) {
			php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner.");
			return(NULL);
		}
		if (_php3_check_open_basedir(filename)) return NULL;
		zp = gzopen(filename, mode);
		if (zp && opened_path) {
			*opened_path = strdup(filename);
		}
		return zp;
	}

	pathbuf = estrdup(path);

	ptr = pathbuf;

	while (ptr && *ptr) {
#if WIN32|WINNT
		end = strchr(ptr, ';');
#else
		end = strchr(ptr, ':');
#endif
		if (end != NULL) {
			*end = '\0';
			end++;
		}
		snprintf(trypath, MAXPATHLEN, "%s/%s", ptr, filename);
		if (php3_ini.safe_mode) {
			if (stat(trypath,&sb) == 0 &&(!_php3_checkuid(trypath,2))) {
				php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner.");
				efree(pathbuf);
				return(NULL);
			}
		}
		if ((zp = gzopen(trypath, mode)) != NULL) {
			if (_php3_check_open_basedir(trypath)) {
				gzclose(zp);
			   	efree(pathbuf);
			   	return NULL;
			}
			if (opened_path) {
				*opened_path = expand_filepath(trypath);
			}
			efree(pathbuf);
			return zp;
		}
		ptr = end;
	}
	efree(pathbuf);
	return NULL;
}

void php3_gzfile(INTERNAL_FUNCTION_PARAMETERS) {
	pval *filename, *arg2;
	gzFile zp;
	char *slashed, buf[8192];
	register int i=0;
	int use_include_path = 0;

	/* check args */
	switch (ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht,1,&filename) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (getParameters(ht,2,&filename,&arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(arg2);
		use_include_path = arg2->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

	zp = php3_gzopen_wrapper(filename->value.str.val,"r", use_include_path|ENFORCE_SAFE_MODE);
	if (!zp) {
		php3_error(E_WARNING,"gzFile(\"%s\") - %s",filename->value.str.val,strerror(errno));
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	/* Now loop through the file and do the magic quotes thing if needed */
	memset(buf,0,8191);
	while((int)gzgets(zp, buf, 8191)) {
		if (php3_ini.magic_quotes_runtime) {
			int len;
			
			slashed = _php3_addslashes(buf,0,&len,0); /* 0 = don't free source string */
            add_index_stringl(return_value, i++, slashed, len, 0);
		} else {
			add_index_string(return_value, i++, buf, 1);
		}
	}
	gzclose(zp);

}



void php3_gzopen(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	gzFile *zp;
	char *p;
	int id;
	int use_include_path = 0;
	ZLIB_TLS_VARS;
	
	switch(ARG_COUNT(ht)) {
	case 2:
		if (getParameters(ht,2,&arg1,&arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (getParameters(ht,3,&arg1,&arg2,&arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(arg3);
		use_include_path = arg3->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);
	convert_to_string(arg2);
	p = estrndup(arg2->value.str.val,arg2->value.str.len);

	/*
	 * We need a better way of returning error messages from
	 * php3_gzopen__wrapper().
	 */
	zp = php3_gzopen_wrapper(arg1->value.str.val, p, use_include_path|ENFORCE_SAFE_MODE);
	if (!zp) {
		php3_error(E_WARNING,"gzopen(\"%s\",\"%s\") - %s",
					arg1->value.str.val, p, strerror(errno));
		efree(p);
		RETURN_FALSE;
	}
	ZLIB_GLOBAL(gzgetss_state)=0;
	id = php3_list_insert(zp,ZLIB_GLOBAL(le_zp));
	efree(p);
	RETURN_LONG(id);
}	

void php3_gzclose(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	gzFile *zp;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id=arg1->value.lval;
	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp))) {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		RETURN_FALSE;
	}
	php3_list_delete(id);
	RETURN_TRUE;
}


void php3_gzeof(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	gzFile *zp;
	int id, type;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id = arg1->value.lval;
	zp = php3_list_find(id,&type);
	if ((!zp || (type!=ZLIB_GLOBAL(le_zp)))) {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		/* we're at the eof if the file doesn't exist */
		RETURN_TRUE;
	}
	if ((gzeof(zp))) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

void php3_gzgets(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	gzFile *zp;
	int id, len, type;
	char *buf;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	id = arg1->value.lval;
	len = arg2->value.lval;

	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp)))  {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		RETURN_FALSE;
	}
	buf = emalloc(sizeof(char) * (len + 1));
	/* needed because recv doesnt put a null at the end*/
	memset(buf,0,len+1);
	if (!((int)gzgets(zp, buf, len))) {
		efree(buf);
		RETVAL_FALSE;
	} else {
		if (php3_ini.magic_quotes_runtime) {
			return_value->value.str.val = _php3_addslashes(buf,0,&return_value->value.str.len,1);
		} else {
			return_value->value.str.val = buf;
			return_value->value.str.len = strlen(return_value->value.str.val);
		}
		return_value->type = IS_STRING;
	}
	return;
}

void php3_gzgetc(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	gzFile *zp;
	int id, type;
	char *buf;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id = arg1->value.lval;

	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp))) {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		RETURN_FALSE;
	}
	buf = emalloc(sizeof(char) * 2);
	if ((*buf=gzgetc(zp)) == (-1)) {
		efree(buf);
		RETVAL_FALSE;
	} else {
		buf[1]='\0';
		return_value->value.str.val = buf; 
		return_value->value.str.len = 1; 
		return_value->type = IS_STRING;
	}
	return;
}

/* Strip any HTML tags while reading */
void php3_gzgetss(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *fd, *bytes;
	gzFile *zp;
	int id, len, br, type;
	char *buf, *p, *rbuf, *rp, c, lc;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &fd, &bytes) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fd);
	convert_to_long(bytes);

	id = fd->value.lval;
	len = bytes->value.lval;

	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp))) {
		php3_error(E_WARNING, "Unable to find gz-file identifier %d", id);
		RETURN_FALSE;
	}

	buf = emalloc(sizeof(char) * (len + 1));
	/*needed because recv doesnt set null char at end*/
	memset(buf,0,len+1);
	if (!((int)gzgets(zp, buf, len))) {
		efree(buf);
		RETURN_FALSE;
	}

	rbuf = estrdup(buf);
	c = *buf;
	lc = '\0';
	p = buf;
	rp = rbuf;
	br = 0;

	while (c) {
		switch (c) {
			case '<':
				if (ZLIB_GLOBAL(gzgetss_state) == 0) {
					lc = '<';
					ZLIB_GLOBAL(gzgetss_state) = 1;
				}
				break;

			case '(':
				if (ZLIB_GLOBAL(gzgetss_state) == 2) {
					if (lc != '\"') {
						lc = '(';
						br++;
					}
				} else if (ZLIB_GLOBAL(gzgetss_state) == 0) {
					*(rp++) = c;
				}
				break;	

			case ')':
				if (ZLIB_GLOBAL(gzgetss_state) == 2) {
					if (lc != '\"') {
						lc = ')';
						br--;
					}
				} else if (ZLIB_GLOBAL(gzgetss_state) == 0) {
					*(rp++) = c;
				}
				break;	

			case '>':
				if (ZLIB_GLOBAL(gzgetss_state) == 1) {
					lc = '>';
					ZLIB_GLOBAL(gzgetss_state) = 0;
				} else if (ZLIB_GLOBAL(gzgetss_state) == 2) {
					if (!br && lc != '\"') {
						ZLIB_GLOBAL(gzgetss_state) = 0;
					}
				}
				break;

			case '\"':
				if (ZLIB_GLOBAL(gzgetss_state) == 2) {
					if (lc == '\"') {
						lc = '\0';
					} else if (lc != '\\') {
						lc = '\"';
					}
				} else if (ZLIB_GLOBAL(gzgetss_state) == 0) {
					*(rp++) = c;
				}
				break;

			case '?':
				if (ZLIB_GLOBAL(gzgetss_state)==1) {
					br=0;
					ZLIB_GLOBAL(gzgetss_state)=2;
					break;
				}
				/* fall-through */

			default:
				if (ZLIB_GLOBAL(gzgetss_state) == 0) {
					*(rp++) = c;
				}	
		}
		c = *(++p);
	}	
	*rp = '\0';
	efree(buf);
	RETVAL_STRING(rbuf,1);
	efree(rbuf);
}


void php3_gzwrite(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3=NULL;
	gzFile *zp;
	int ret,id,type;
	int num_bytes;
	ZLIB_TLS_VARS;

	switch (ARG_COUNT(ht)) {
		case 2:
			if (getParameters(ht, 2, &arg1, &arg2)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(arg2);
			num_bytes = arg2->value.str.len;
			break;
		case 3:
			if (getParameters(ht, 3, &arg1, &arg2, &arg3)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_string(arg2);
			convert_to_long(arg3);
			num_bytes = MIN(arg3->value.lval, arg2->value.str.len);
			break;
		default:
			WRONG_PARAM_COUNT;
			break;
	}				
	convert_to_long(arg1);
	id = arg1->value.lval;	

	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp))) {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		RETURN_FALSE;
	}

	/* strip slashes only if the length wasn't specified explicitly */
	if (!arg3 && php3_ini.magic_quotes_runtime) {
		_php3_stripslashes(arg2->value.str.val,&num_bytes);
	}

	ret = gzwrite(zp, arg2->value.str.val,num_bytes);
	RETURN_LONG(ret);
}	

void php3_gzrewind(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id,type;
	gzFile *zp;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id = arg1->value.lval;	
	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp))) {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		RETURN_FALSE;
	}
	gzrewind(zp);
	RETURN_TRUE;
}

void php3_gztell(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	long pos;
	gzFile *zp;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id = arg1->value.lval;	
	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp))) {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		RETURN_FALSE;
	}
	pos = gztell(zp);
	RETURN_LONG(pos);
}

void php3_gzseek(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int ret,id,type;
	long pos;
	gzFile *zp;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	pos = arg2->value.lval;
	id = arg1->value.lval;
	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp))) {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		RETURN_FALSE;
	}
 	ret = gzseek(zp,pos,SEEK_SET);
	RETURN_LONG(ret);
}

/*
 * Read a file and write the ouput to stdout
 */
void php3_readgzfile(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	char buf[8192];
	gzFile *zp;
	int b, size;
	int use_include_path = 0;

	
	/* check args */
	switch (ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht,1,&arg1) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (getParameters(ht,2,&arg1,&arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(arg2);
		use_include_path = arg2->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);	

	/*
	 * We need a better way of returning error messages from
	 * php3_gzopen_wrapper().
	 */
	zp = php3_gzopen_wrapper(arg1->value.str.val,"r", use_include_path|ENFORCE_SAFE_MODE);
	if (!zp){
		php3_error(E_WARNING,"ReadGzFile(\"%s\") - %s",arg1->value.str.val,strerror(errno));
		RETURN_FALSE;
	}
	size= 0;
	if (php3_header()) {  /* force header if not already sent */
		while((b = gzread(zp, buf, sizeof(buf))) > 0) {
			PHPWRITE(buf,b);
			size += b ;
		}
	}
   	gzclose(zp);
	RETURN_LONG(size);
}

/*
 * Read to EOF on a file descriptor and write the output to stdout.
 */
void php3_gzpassthru(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	gzFile *zp;
	char buf[8192];
	int id, size, b, type;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	id = arg1->value.lval;
	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp))) {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		RETURN_FALSE;
	}
	size = 0;
	if (php3_header()) { /* force headers if not already sent */
		while((b = gzread(zp, buf, sizeof(buf))) > 0) {
			PHPWRITE(buf,b);
			size += b ;
		}
	}
/*  gzclose(zp); */
	php3_list_delete(id);
	RETURN_LONG(size);
}

void php3_gzread(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1, *arg2;
	gzFile *zp;
	int id, len, type;
	ZLIB_TLS_VARS;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg1);
	convert_to_long(arg2);
	id = arg1->value.lval;
	len = arg2->value.lval;

	zp = php3_list_find(id,&type);
	if (!zp || (type!=ZLIB_GLOBAL(le_zp))) {
		php3_error(E_WARNING,"Unable to find gz-file identifier %d",id);
		RETURN_FALSE;
	}
	return_value->value.str.val = emalloc(sizeof(char) * (len + 1));
	/* needed because recv doesnt put a null at the end*/
	
	return_value->value.str.len = gzread(zp, return_value->value.str.val, len);
	return_value->value.str.val[return_value->value.str.len] = 0;

	if (php3_ini.magic_quotes_runtime) {
		return_value->value.str.val = _php3_addslashes(return_value->value.str.val,return_value->value.str.len,&return_value->value.str.len,1);
	}
	return_value->type = IS_STRING;
}

#endif /* HAVE_ZLIB */
/*
 * Local variables:
 * tab-width: 4
 * End:
 */
