/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   |          Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
*/
/* $Id: fsock.c,v 1.85 1999/02/27 17:04:46 sas Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "php.h"
#include "php3_list.h"
#include "internal_functions.h"
#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#ifdef WIN32
#undef AF_UNIX
#endif
#if defined(AF_UNIX)
#include <sys/un.h>
#endif

#include <string.h>
#include <errno.h>

#include "base64.h"
#include "file.h"
#include "post.h"
#include "url.h"
#include "fsock.h"

#ifndef THREAD_SAFE
extern int le_fp;
#endif

#define FREE_SOCK efree(sock); if (key) efree(key)

#if WIN32|WINNT
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
#include "build-defs.h"
#endif

static unsigned char third_and_fourth_args_force_ref[] = { 4, BYREF_NONE, BYREF_NONE, BYREF_FORCE, BYREF_FORCE };

function_entry fsock_functions[] = {
	PHP_FE(fsockopen, third_and_fourth_args_force_ref)
	PHP_FE(pfsockopen, third_and_fourth_args_force_ref)
	{NULL, NULL, NULL}
};

static int php3_minit_fsock(INIT_FUNC_ARGS);
static int php3_mshutdown_fsock(void);

php3_module_entry fsock_module_entry = {
	"Socket functions", fsock_functions, php3_minit_fsock, php3_mshutdown_fsock, NULL, NULL, NULL, STANDARD_MODULE_PROPERTIES
};

#ifndef THREAD_SAFE
static HashTable ht_keys;
static HashTable ht_socks;
#endif

	/* {{{ lookup_hostname */

/*
 * Converts a host name to an IP address.
 */
int lookup_hostname(const char *addr, struct in_addr *in)
{
	struct hostent *host_info;

	if(!inet_aton(addr, in)) {
		host_info = gethostbyname(addr);
		if (host_info == 0) {
			/* Error: unknown host */
			return -1;
		}
		*in = *((struct in_addr *) host_info->h_addr);
	}
	return 0;
}
/* }}} */
	/* {{{ _php3_is_persistent_sock */

int _php3_is_persistent_sock(int sock)
{
	char *key;

	if (_php3_hash_find(&ht_socks, (char *) &sock, sizeof(sock),
				(void **) &key) == SUCCESS) {
		return 1;
	}
	return 0;
}
/* }}} */
	/* {{{ _php3_fsockopen() */

/* 
   This function takes an optional third argument which should be
   passed by reference.  The error code from the connect call is written
   to this variable.
*/
static void _php3_fsockopen(INTERNAL_FUNCTION_PARAMETERS, int persistent) {
	pval *args[4];
	int *sock=emalloc(sizeof(int));
	int *sockp;
	int id, socketd, arg_count=ARG_COUNT(ht);
	unsigned short portno;
	char *key = NULL;
	TLS_VARS;
	
	if (arg_count > 4 || arg_count < 2 || getParametersArray(ht,arg_count,args)==FAILURE) {
		FREE_SOCK;
		WRONG_PARAM_COUNT;
	}
	switch(arg_count) {
		case 4:
			if(!ParameterPassedByReference(ht,4)) {
				php3_error(E_WARNING,"error string argument to fsockopen not passed by reference");
			}
			pval_copy_constructor(args[3]);
			args[3]->value.str.val = empty_string;
			args[3]->value.str.len = 0;
			args[3]->type = IS_STRING;
			/* fall-through */
		case 3:
			if(!ParameterPassedByReference(ht,3)) {
				php3_error(E_WARNING,"error argument to fsockopen not passed by reference");
			}
			args[2]->type = IS_LONG;
			args[2]->value.lval = 0;
			break;
	}
	convert_to_string(args[0]);
	convert_to_long(args[1]);
	portno = (unsigned short) args[1]->value.lval;

	key = emalloc(args[0]->value.str.len + 10);
	sprintf(key, "%s:%d", args[0]->value.str.val, portno);

	if (persistent && _php3_hash_find(&ht_keys, key, strlen(key) + 1,
				(void *) &sockp) == SUCCESS) {
		efree(key);
		*sock = *sockp;
		RETURN_LONG(php3_list_insert(sock, GLOBAL(wsa_fp)));
	}
	
	if (portno) {
		struct sockaddr_in server;

		memset(&server, 0, sizeof(server));
		socketd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketd == SOCK_ERR) {
			FREE_SOCK;
			RETURN_FALSE;
		}
	  
		server.sin_family = AF_INET;
		
		if (lookup_hostname(args[0]->value.str.val, &server.sin_addr)) {
			FREE_SOCK;
			RETURN_FALSE;
		}
  
		server.sin_port = htons(portno);
  
		if (connect(socketd, (struct sockaddr *)&server, sizeof(server)) == SOCK_CONN_ERR) {
			FREE_SOCK;
			if(arg_count>2) args[2]->value.lval = errno;
			if(arg_count>3) {
				args[3]->value.str.val = estrdup(strerror(errno));
				args[3]->value.str.len = strlen(args[3]->value.str.val);
			}
			RETURN_FALSE;
		}
#if defined(AF_UNIX)
	} else {
		/* Unix domain socket.  s->strval is socket name. */
		struct  sockaddr_un unix_addr;
		socketd = socket(AF_UNIX,SOCK_STREAM,0);
		if (socketd == SOCK_ERR) {
			FREE_SOCK;
			RETURN_FALSE;
		}
	  
		memset(&unix_addr,(char)0,sizeof(unix_addr));
		unix_addr.sun_family = AF_UNIX;
		strcpy(unix_addr.sun_path, args[0]->value.str.val);

		if (connect(socketd, (struct sockaddr *) &unix_addr, sizeof(unix_addr)) == SOCK_CONN_ERR) {
			FREE_SOCK;
			if(arg_count>2) args[2]->value.lval = errno;
			if(arg_count>3) {
				args[3]->value.str.val = estrdup(strerror(errno));
				args[3]->value.str.len = strlen(args[3]->value.str.val);
			}
			RETURN_FALSE;
		}
#endif /* AF_UNIX */
	}

#if 0
	if ((fp = fdopen (socketd, "r+")) == NULL){
		RETURN_LONG(-6);  /* FIXME */
	}

#ifdef HAVE_SETVBUF  
	if ((setvbuf(fp, NULL, _IONBF, 0)) != 0){
		RETURN_LONG(-7);  /* FIXME */
	}
#endif
#endif

	*sock=socketd;
	if (persistent) {
		_php3_hash_update(&ht_keys, key, strlen(key) + 1, 
				sock, sizeof(*sock), NULL);
		_php3_hash_update(&ht_socks, (char *) sock, sizeof(*sock),
				key, strlen(key) + 1, NULL);
	}
	if(key) efree(key);
	id = php3_list_insert(sock,GLOBAL(wsa_fp));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int fsockopen(string hostname, int port [, int errno [, string errstr]])
   Open Internet or Unix domain socket connection */
PHP_FUNCTION(fsockopen) 
{
	_php3_fsockopen(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int pfsockopen(string hostname, int port [, int errno [, string errstr]])
   Open persistent Internet or Unix domain socket connection */
PHP_FUNCTION(pfsockopen) 
{
	_php3_fsockopen(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

int _php3_sock_fgets(char *buf, int maxlen, int socket)
{
	int retval;
	int count=0;
	
	while (count<maxlen && (retval=recv(socket,buf,1,0))!=SOCK_RECV_ERR && retval!=EWOULDBLOCK && retval!=0) {
		if (*buf=='\n') {
			buf++;
			count++;
			break;
		}
		buf++;
		count++;
	}
	
	*buf = '\0';
	return count;
}


int _php3_sock_fread(char *buf, int maxlen, int socket)
{
	int retval;
	int count=0;
	
	while (count<maxlen && (retval=recv(socket,buf,1,0))!=SOCK_RECV_ERR && retval!=EWOULDBLOCK && retval!=0) {
		buf++;
		count++;
	}
	
	*buf = '\0';
	return count;
}

/* {{{ module start/shutdown functions */

	/* {{{ _php3_sock_destroy */
#ifndef THREAD_SAFE
static void _php3_sock_destroy(void *data)
{
	int *sock = (int *) data;

	close(*sock);
}
#endif
/* }}} */
	/* {{{ php3_minit_fsock */

static int php3_minit_fsock(INIT_FUNC_ARGS)
{
#ifndef THREAD_SAFE
	_php3_hash_init(&ht_keys, 0, NULL, NULL, 1);
	_php3_hash_init(&ht_socks, 0, NULL, _php3_sock_destroy, 1);
#endif
	return SUCCESS;
}
/* }}} */
	/* {{{ php3_mshutdown_fsock */

static int php3_mshutdown_fsock(void)
{
#ifndef THREAD_SAFE
	_php3_hash_destroy(&ht_socks);
	_php3_hash_destroy(&ht_keys);
#endif
	return SUCCESS;
}
/* }}} */
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
