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
   |          Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */
/* $Id: fopen-wrappers.c,v 1.7 1998/02/02 00:08:55 shane Exp $ */

#ifdef THREAD_SAFE
# include "tls.h"
#endif

#include "parser.h"
#include "modules.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if MSVC5
# include <windows.h>
# include <winsock.h>
# define O_RDONLY _O_RDONLY
# include "win32/param.h"
#else
# include <sys/param.h>
#endif

#include "safe_mode.h"
#include "list.h"
#include "functions/head.h"
#include "functions/url.h"
#include "functions/base64.h"
#include "functions/fsock.h"
#include "functions/php3_string.h"

#if HAVE_PWD_H
# if MSVC5
#  include "win32/pwd.h"
# else
#  include <pwd.h>
# endif
#endif

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif

#if MSVC5
# include <winsock.h>
#else
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
#endif

#if MSVC5
# undef AF_UNIX
#endif

#if defined(AF_UNIX)
# include <sys/un.h>
#endif

static FILE *php3_fopen_url_wrapper(char *path, char *mode, int options SOCK_ARG_IN);

#if WIN32|WINNT
int _php3_getftpresult(int socketd);
#else
static int _php3_getftpresult(FILE *fpc);
#endif

PHPAPI FILE *php3_fopen_wrapper(char *path, char *mode, int options SOCK_ARG_IN) {
#if PHP3_URL_FOPEN
	if (!(options & IGNORE_URL)) {
		return php3_fopen_url_wrapper(path, mode, options SOCK_ARG);
	}
#endif

	if (options & USE_PATH && php3_ini.include_path != NULL) {
		return php3_fopen_with_path(path, mode, php3_ini.include_path, NULL);
	}
	else {
		if (options & ENFORCE_SAFE_MODE && php3_ini.safe_mode && (!_php3_checkuid(path,1))) {
			php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner of file to be read.");
			return NULL;
		}
		return fopen(path, mode);
	}
}

/* FIXME: What does the comment below mean? jimw */
/* Lots more stuff needs to go in here.  Trivial case for now */
FILE *php3_fopen_for_parser(char *filename) {
	FILE *fp = NULL;
	char *fn;

#if (!APACHE)
	char *temp = NULL;
#if CGI_BINARY || USE_SAPI
	int l;
#endif
#if HAVE_PWD_H
	char *s, user[32];
	struct passwd *pw = NULL;
#endif
#endif
	TLS_VARS;
	
	
	fn = filename;

#if CGI_BINARY || USE_SAPI
/* CGI is the only case where we're never not given a filename and
   we have to build it from the path_info and the DOCUMENT_ROOT */
	if (!fn) {
		fn = GLOBAL(request_info).path_info;
		if (fn) {
			GLOBAL(request_info).filename = estrdup(fn);
#if HAVE_PWD_H
			if (*(fn + 1) == '~') {
				s = strchr(fn + 1, '/');
				if (s) {
					*s = '\0';
				}
				strcpy(user, fn + 2);	/* This strcpy is safe, path size is known */
				if (s) {
					*s = '/';
				}
				l = strlen(php3_ini.user_dir);
				if (user) {
					pw = getpwnam(user);
					if (pw) {
						temp = emalloc(l + strlen(fn) + strlen(PHP_USER_DIR) + strlen(pw->pw_dir) + 4);
						strcpy(temp, pw->pw_dir);
						strcat(temp, "/");
						strcat(temp, php3_ini.user_dir);
						strcat(temp, "/");
						if (s) {
							strcat(temp, s);
						}
						STR_FREE(GLOBAL(request_info).filename);
						GLOBAL(request_info).filename = fn = temp;
					}
				}
			} else {
#endif
				l = strlen(php3_ini.doc_root);
				temp = emalloc(l + strlen(fn) + 2);
				if (temp) {
					strcpy(temp, php3_ini.doc_root);
					if ((php3_ini.doc_root[l - 1] != '/') && (fn[0] != '/')) {
						strcat(temp, "/");
					}
					strcat(temp, fn);
					STR_FREE(GLOBAL(request_info).filename);
					GLOBAL(request_info).filename = fn = temp;
				}
#if HAVE_PWD_H
			}
#endif
		}
	}
#endif

	if (!fn) {
		php3_error(E_WARNING, "fn is empty");
		/* we have to free request_info.filename here because
		   php3_destroy_request_info assumes that it will get
		   freed when the include_names hash is emptied, but
		   we're not adding it in this case */
		STR_FREE(GLOBAL(request_info).filename);
		return NULL;
	}
	fp = fopen(fn, "r");
	if (!fp) {
		php3_error(E_ERROR, "Unable to open %s", fn);
		STR_FREE(GLOBAL(request_info).filename); /* for same reason as above */
	} else {
		hash_index_update(&GLOBAL(include_names), 0, (void *) &fn, sizeof(char *),NULL);
	}

#if CGI_BINARY || USE_SAPI
	temp=strdup(fn);
	_php3_dirname(temp);
	if(strlen(temp)) chdir(temp);
	free(temp);
#endif

	return fp;
}


/*
 * Tries to open a file with a PATH-style list of directories.
 * If the filename starts with "." or "/", the path is ignored.
 */
PHPAPI FILE *php3_fopen_with_path(char *filename, char *mode, char *path, char **opened_path)
{
	char *pathbuf, *ptr, *end;
	char trypath[MAXPATHLEN + 1];
	struct stat sb;
	FILE *fp;
	TLS_VARS;
	
	if (opened_path) {
		*opened_path = NULL;
	}
	
	/* Relative path open */
	if (*filename == '.') {
		if(php3_ini.safe_mode &&(!_php3_checkuid(filename,2))) {
			php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner.");
			return(NULL);
		}
		if (opened_path) {
			*opened_path = strdup(filename);
		}
		return fopen(filename, mode);
	}

	/* Absolute path open - prepend document_root in safe mode */
#if WIN32|WINNT
	if ((*filename == '\\')||(*filename == '/')||(filename[1] == ':')) {
#else
	if (*filename == '/') {
#endif
		if(php3_ini.safe_mode) {
			strncpy(trypath,php3_ini.doc_root,MAXPATHLEN);
			strncat(trypath,filename,MAXPATHLEN);
			if(!_php3_checkuid(trypath,2)) {
				php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner.");
				return(NULL);
			}
			if (opened_path) {
				*opened_path = strdup(trypath);
			}
			return fopen(trypath, mode);
		} else {
			return fopen(filename, mode);
		}
	}

	if(!path || (path && !*path)) {
		if(php3_ini.safe_mode &&(!_php3_checkuid(filename,2))) {
			php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner.");
			return(NULL);
		}
		if (opened_path) {
			*opened_path = strdup(filename);
		}
		return fopen(filename, mode);
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
		if(php3_ini.safe_mode) {
			if(stat(trypath,&sb) == 0 &&(!_php3_checkuid(trypath,2))) {
				php3_error(E_WARNING,"SAFE MODE Restriction in effect.  Invalid owner.");
				return(NULL);
			}
		}
		if ((fp = fopen(trypath, mode)) != NULL) {
			if (opened_path) {
				*opened_path = strdup(trypath);
			}
			efree(pathbuf);
			return fp;
		}
		ptr = end;
	}
	efree(pathbuf);
	return NULL;
}

/*
 * If the specified path starts with "http://" (insensitive to case),
 * a socket is opened to the specified web server and a file pointer is
 * position at the start of the body of the response (past any headers).
 * This makes a HTTP/1.0 request, and knows how to pass on the username
 * and password for basic authentication.
 *
 * If the specified path starts with "ftp://" (insensitive to case),
 * a pair of sockets are used to request the specified file and a file
 * pointer to the requested file is returned. Passive mode ftp is used,
 * so if the server doesn't suppor this, it will fail!
 *
 * Otherwise, fopen is called as usual and the file pointer is returned.
 */

static FILE *php3_fopen_url_wrapper(char *path, char *mode, int options SOCK_ARG_IN) {
	url *resource;
	int result;
	char *scratch, *tmp;

	char tmp_line[256];
	char location[256];
	int chptr=0;
	char *tpath, *ttpath;
	int body = 0;
	int reqok = 0;
	int lineone = 1;
	int i, ch = 0;

	char oldch1 = 0;
	char oldch2 = 0;
	char oldch3 = 0;
	char oldch4 = 0;
	char oldch5 = 0;
	
	FILE *fp;
#if !(WIN32|WINNT)
	FILE *fpc;
	int socketd;
#endif
	struct sockaddr_in server;
	unsigned short portno;
#if WIN32|WINNT
	char winfeof;
#endif

	if (!strncasecmp(path,"http://",7)) {

		resource = url_parse(path);
		if (resource == NULL) {
			php3_error(E_WARNING, "invalid url specified, %s", path);
			return(NULL);
		}

		/* use port 80 if one wasn't specified */
		if (resource->port == 0) resource->port = 80;

		SOCKETD = socket(AF_INET, SOCK_STREAM, 0);
		if (SOCKETD SOCK_ERR) {
#if WIN32|WINNT
			SOCK_FCLOSE(SOCKETD);
			SOCKETD=0;
#endif
			free_url(resource);
			return(NULL);
		}

		server.sin_addr.s_addr = lookup_hostname(resource->host);
		server.sin_family = AF_INET;
		
		if(server.sin_addr.s_addr == -1) {
#if WIN32|WINNT
			SOCK_FCLOSE(SOCKETD);
			SOCKETD=0;
#endif
			free_url(resource);
			return(NULL);
		}

		server.sin_port = htons(resource->port);
 
		if (connect(SOCKETD, (struct sockaddr *)&server, sizeof(server)) SOCK_CONN_ERR) {
#if WIN32|WINNT
			SOCK_FCLOSE(SOCKETD);
			SOCKETD=0;
#endif
			free_url(resource);
			return(NULL);
		}

#if !(WIN32|WINNT)
		if ((fp = fdopen (SOCKETD, "r+")) == NULL) {
			free_url(resource);
			return(NULL);
		}

#ifdef HAVE_SETVBUF  
		if ((setvbuf(fp, NULL, _IONBF, 0)) != 0) {
			free_url(resource);
			return(NULL);
		}
#endif
#endif /*win32*/

		/* tell remote http which file to get */
		SOCK_WRITE("GET ", SOCK_FP);
		if (resource->path != NULL) {
			SOCK_WRITE(resource->path, SOCK_FP);
		}
		else {
			SOCK_WRITE("/", SOCK_FP);
		}

		/* append the query string, if any */
		if (resource->query != NULL) {
			SOCK_WRITE("?", SOCK_FP);
			SOCK_WRITE(resource->query, SOCK_FP);
		}
		SOCK_WRITE(" HTTP/1.0\n", SOCK_FP);

		/* send authorization header if we have user/pass */
		if (resource->user != NULL && resource->pass != NULL) {
			scratch = (char *)emalloc(strlen(resource->user) + strlen(resource->pass) + 2);
			if (!scratch) {
				free_url(resource);
				return (NULL);
			}

			strcpy(scratch, resource->user);
			strcat(scratch, ":");
			strcat(scratch, resource->pass);
			tmp = base64_encode(scratch);

			SOCK_WRITE("Authorization: Basic ", SOCK_FP);
			/* output "user:pass" as base64-encoded string */
			SOCK_WRITE(tmp, SOCK_FP);
			SOCK_WRITE("\n", SOCK_FP);
			efree(scratch);
			efree(tmp);
		}

		/* if the user has configured who they are, send a From: line */
		if (cfg_get_string("from", &scratch) == SUCCESS) {
			SOCK_WRITE("From: ", SOCK_FP);
			SOCK_WRITE(scratch, SOCK_FP);
			SOCK_WRITE("\n", SOCK_FP);
		}

		/* send a Host: header so name-based virtual hosts work */
		SOCK_WRITE("Host: ", SOCK_FP);
		SOCK_WRITE(resource->host, SOCK_FP);
		SOCK_WRITE("\n", SOCK_FP);

		/* identify ourselves */
		SOCK_WRITE("User-Agent: PHP/", SOCK_FP);
		SOCK_WRITE(PHP_VERSION, SOCK_FP);
		SOCK_WRITE("\n", SOCK_FP);

		/* end the headers */
		SOCK_WRITE("\n", SOCK_FP);

		/* Read past http header */
		body = 0;
		location[0] = '\0';
#if WIN32|WINNT
		while (!body && recv(SOCK_FP,(char *)&winfeof,1,MSG_PEEK)){
#else
		while (!body && !feof(fp)) {
#endif
			SOCK_FGETC(ch,SOCK_FP);
			if (ch == EOF) {
				SOCK_FCLOSE(SOCK_FP);
#if WIN32|WINNT
				SOCKETD=0;
#endif
				free_url(resource);
				return(NULL);
			}
			oldch5 = oldch4;
			oldch4 = oldch3;
			oldch3 = oldch2;
			oldch2 = oldch1;
			oldch1 = ch;

			tmp_line[chptr++]=ch;
			if (ch == 10 || ch == 13) {
				tmp_line[chptr]='\0';
				chptr=0;
				if (!strncasecmp(tmp_line,"Location: ",10)) {
					tpath=tmp_line+10;
					strcpy(location,tpath);
				}
			}

			if (lineone && (ch == 10 || ch == 13)) {
				lineone=0;
			}
			if (lineone && oldch5 == ' ' && oldch4 == '2' && oldch3 == '0' &&
				oldch2 == '0' && oldch1 == ' ' ) {
				reqok=1;
			}
			if (oldch4 == 13 && oldch3 == 10 && oldch2 == 13 && oldch1 == 10) {
				body=1;
			}
			if (oldch2 == 10 && oldch1 == 10) {
				body=1;
			}
			if (oldch2 == 13 && oldch1 == 13) {
				body=1;
			}
		}
		if (!reqok) {
			SOCK_FCLOSE(SOCK_FP);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			free_url(resource);
			if (location[0]!='\0') {
				return php3_fopen_url_wrapper(location, mode, options SOCK_ARG);
			} else {
				return(NULL);
			}
		}		
		free_url(resource);
#if WIN32|WINNT
		*issock=1;
#endif
		return(fp);
	}
	else if (!strncasecmp(path,"ftp://",6)) {
		resource = url_parse(path);
		if (resource == NULL) {
			php3_error(E_WARNING, "invalid url specified, %s", path);
			return(NULL);
		}

		/* use port 21 if one wasn't specified */
		if (resource->port == 0) resource->port = 21;

		SOCKETD = socket(AF_INET, SOCK_STREAM, 0);
		if(SOCKETD SOCK_ERR) {
#if WIN32|WINNT
			SOCK_FCLOSE(SOCKETD);
			SOCKETD=0;
#endif
			free_url(resource);
			return(NULL);
		}

		server.sin_addr.s_addr = lookup_hostname(resource->host);
		server.sin_family = AF_INET;
		
		if(server.sin_addr.s_addr == -1) {
#if WIN32|WINNT
			SOCK_FCLOSE(SOCKETD);
			SOCKETD=0;
#endif
			free_url(resource);
			return(NULL);
		}

		server.sin_port = htons(resource->port);
 
		if (connect(SOCKETD, (struct sockaddr *)&server, sizeof(server)) SOCK_CONN_ERR) {
#if WIN32|WINNT
			SOCK_FCLOSE(SOCKETD);
			SOCKETD=0;
#endif
			free_url(resource);
			return(NULL);
		}

#if !(WIN32|WINNT)
		if ((fpc = fdopen (SOCKETD, "r+")) == NULL) {
			free_url(resource);
			return(NULL);
		}

#ifdef HAVE_SETVBUF  
		if ((setvbuf(fpc, NULL, _IONBF, 0)) != 0) {
			free_url(resource);
			fclose(fpc);
			return(NULL);
		}
#endif
#endif

		/* Start talking to ftp server */
		result = _php3_getftpresult(SOCK_FPC);
		if (result > 299 || result < 200) {
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}

		/* send the user name */
		SOCK_WRITE("USER ", SOCK_FPC);
		if (resource->user != NULL) {
			_php3_rawurldecode(resource->user);
			SOCK_WRITE(resource->user, SOCK_FPC);
		} else {
			SOCK_WRITE("anonymous", SOCK_FPC);
		}
		SOCK_WRITE("\n", SOCK_FPC);

		/* get the response */
		result = _php3_getftpresult(SOCK_FPC);

		/* if a password is required, send it */
		if (result >= 300 && result <= 399) {
			SOCK_WRITE("PASS ", SOCK_FPC);
			if (resource->pass != NULL) {
				_php3_rawurldecode(resource->pass);
				SOCK_WRITE(resource->pass, SOCK_FPC);
			} else {
				/* if the user has configured who they are,
				   send that as the password */
				if (cfg_get_string("from", &scratch) == SUCCESS) {
					SOCK_WRITE(scratch, SOCK_FPC);
				} else {
					SOCK_WRITE("anonymous", SOCK_FPC);
				}
			}
			SOCK_WRITE("\n", SOCK_FPC);

			/* read the response */
			result = _php3_getftpresult(SOCK_FPC);
			if (result > 299 || result < 200) {
				free_url(resource);
				SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
				SOCKETD=0;
#endif
				return(NULL);
			}
		}
		else if (result > 299 || result < 200) {
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}


		/* find out the size of the file (verifying it exists) */
		SOCK_WRITE("SIZE ", SOCK_FPC);
		SOCK_WRITE(resource->path, SOCK_FPC);
		SOCK_WRITE("\n", SOCK_FPC);

		/* read the response */
		result = _php3_getftpresult(SOCK_FPC);
		if (result > 299 || result < 200) {
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}

		/* set the connection to be binary */
		SOCK_WRITE("TYPE I\n", SOCK_FPC);
		result = _php3_getftpresult(SOCK_FPC);
		if (result > 299 || result < 200) {
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}

		/* set up the passive connection */
		SOCK_WRITE("PASV\n", SOCK_FPC);
		while (SOCK_FGETS(tmp_line, 256, SOCK_FPC) != 0 && 
			   !(isdigit((int)tmp_line[0]) && isdigit((int)tmp_line[1]) &&
				 isdigit((int)tmp_line[2]) && tmp_line[3] == ' '));

		/* make sure we got a 227 response */
		if (strncmp(tmp_line, "227", 3)) {
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}

		/* parse pasv command (129,80,95,25,13,221) */
		tpath = tmp_line;

		/* skip over the "227 Some message " part */
		for (tpath += 4; *tpath && !isdigit((int)*tpath); tpath++);
		if (!*tpath) {
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}

		/* skip over the host ip, we just assume it's the same */
		for (i = 0; i < 4; i++) {
			for (; isdigit((int)*tpath); tpath++);
			if (*tpath == ',') {
				tpath++;
			} else {
#if WIN32|WINNT
				SOCK_FCLOSE(SOCKETD);
				SOCKETD=0;
#endif
				return(NULL);
			}
		}

		/* pull out the MSB of the port */
		portno = (unsigned short)strtol(tpath, &ttpath, 10) * 256;
		if (ttpath == NULL) {
			/* didn't get correct response from PASV */
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}
		tpath = ttpath;
		if (*tpath == ',') {
			tpath++;
		} else {
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}

		/* pull out the LSB of the port */
		portno += (unsigned short)strtol(tpath, &ttpath, 10);

		if (ttpath == NULL) {
			/* didn't get correct response from PASV */
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}

		/* finally, send a message to start retrieving the file, and
		   close the command connection */
		SOCK_WRITE("RETR ", SOCK_FPC);
		SOCK_WRITE(resource->path, SOCK_FPC);
		SOCK_WRITE("\nQUIT\n", SOCK_FPC);
		SOCK_FCLOSE(SOCK_FPC);

		/* open the data channel */
		SOCKETD = socket(AF_INET, SOCK_STREAM, 0);
		if (SOCKETD SOCK_ERR) {
#if WIN32|WINNT
			SOCK_FCLOSE(SOCKETD);
			SOCKETD=0;
#endif
			free_url(resource);
			return(NULL);
		}

		server.sin_addr.s_addr = lookup_hostname(resource->host);
		server.sin_family = AF_INET;
		
		if(server.sin_addr.s_addr == -1) {
			free_url(resource);
			SOCK_FCLOSE(SOCK_FPC);
#if WIN32|WINNT
			SOCKETD=0;
#endif
			return(NULL);
		}

		server.sin_port = htons(portno);
 
		if(connect(SOCKETD, (struct sockaddr *)&server, sizeof(server)) SOCK_CONN_ERR) {
			free_url(resource);
#if WIN32|WINNT
			SOCK_FCLOSE(SOCKETD);
			SOCKETD=0;
#endif
			return(NULL);
		}

#if !(WIN32|WINNT)
		if ((fp = fdopen (SOCKETD, "r+")) == NULL) {
			free_url(resource);
			return(NULL);
		}

#ifdef HAVE_SETVBUF  
		if ((setvbuf(fp, NULL, _IONBF, 0)) != 0) {
			free_url(resource);
			fclose(fp);
			return(NULL);
		}
#endif
#endif
		free_url(resource);
#if WIN32|WINNT
		*issock=1;
#endif
		return(fp);

	} else {
		if (options & USE_PATH) {
			fp = php3_fopen_with_path(path, mode, php3_ini.include_path, NULL);
		}
		else {
			fp = fopen(path, mode);
		}

#if WIN32|WINNT
		*issock=0;
#endif

		return(fp); 
	}

	/* Should never get here. */
#if WIN32|WINNT
	SOCK_FCLOSE(SOCKETD);
	SOCKETD=0;
#endif
	return(NULL);
}

#if WIN32|WINNT
int _php3_getftpresult(int socketd) {
#else
static int _php3_getftpresult(FILE *fpc) {
#endif
	char tmp_line[256];

	while (SOCK_FGETS(tmp_line, 256, SOCK_FTP) && 
		   !(isdigit((int)tmp_line[0]) && isdigit((int)tmp_line[1]) &&
			 isdigit((int)tmp_line[2]) && tmp_line[3] == ' '));

	return strtol(tmp_line, NULL, 10);
}

PHPAPI int php3_isurl(char *path) 
{
	return(!strncasecmp(path,"http://",7) || !strncasecmp(path,"ftp://",6));
}

PHPAPI char *php3_strip_url_passwd(char *path) 
{
	char *tmppath=NULL;
	if(!strncasecmp(path,"ftp://",6)) {
		tmppath=path; 
		tmppath+=6;
		for(;*tmppath!=':' && *tmppath!='\0' ; tmppath++ );
		tmppath++ ;
		for(;*tmppath!='@' && *tmppath!='\0' ; tmppath++ ) 
			*tmppath='*';
	}
	return(path);
}

/*
 * Local variables:
 * tab-width: 4
 * End:
 */
