/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   +----------------------------------------------------------------------+
 */

/* $Id: rpc.c,v 1.3 1999/12/26 18:44:48 rasmus Exp $ */
#define IS_EXT_MODULE

#include "php.h"
#include "internal_functions.h"

#include <sys/types.h>
#include <sys/socket.h>
#ifdef HAVE_SYS_UN_H
# include <sys/un.h>
#endif
#include <netinet/in.h>
#include <stdio.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "php3_rpc.h"
#include "rpc_protocol.h"

/* TODO:
 *
 * - Should drop htons() and similar functions in favor of
 *   of own data types with well-defined size in octets.
 *   The current version will not work between machines with
 *   different "unsigned short" and "unsigned long" sizes.
 *
 */

#define RPC_GLOBAL(a) a
#define RPC_TLS_VARS

rpc_module php3_rpc_module;

/* {{{ dynamic loading stuff */

#if COMPILE_DL

php3_module_entry *get_module() { return &rpc_module_entry; };

#endif /* COMPILE_DL */

/* }}} */

int php3_minit_rpc(INIT_FUNC_ARGS);
int php3_mshutdown_rpc(void);
void php3_info_rpc(void);
#ifdef HAVE_SYS_UN_H
static int rpc_connect_unix(char *);
#endif
static int rpc_connect_tcp(char *, int);


function_entry rpc_functions[] = {
	{ "rpc_call",     php3_rpc_call,    NULL },
	{ NULL,           NULL,             NULL }
};

php3_module_entry rpc_module_entry = {
    "PHP RPC",            /* extension name */
    rpc_functions,        /* extension function list */
    php3_minit_rpc,       /* extension-wide startup function */
    php3_mshutdown_rpc,   /* extension-wide shutdown function */
    NULL,                 /* per-request startup function */
    NULL,                 /* per-request shutdown function */
    php3_info_rpc,        /* information function */
    STANDARD_MODULE_PROPERTIES
};


int
php3_minit_rpc(INIT_FUNC_ARGS)
{
	RPC_TLS_VARS;

	RPC_GLOBAL(php3_rpc_module).enabled = 1;
	/* RPC_GLOBAL(php3_rpc_module).host = "/tmp/prc.sock"; */
	RPC_GLOBAL(php3_rpc_module).host = "localhost";
	RPC_GLOBAL(php3_rpc_module).port = 1200; /* -1 means unix domain socket */

	if (RPC_GLOBAL(php3_rpc_module).enabled) {
		int sock;

#ifdef HAVE_SYS_UN_H
		if (RPC_GLOBAL(php3_rpc_module).port == -1) {
			sock = rpc_connect_unix(RPC_GLOBAL(php3_rpc_module).host);
		} else {
			sock = rpc_connect_tcp(RPC_GLOBAL(php3_rpc_module).host,
								   RPC_GLOBAL(php3_rpc_module).port);
		}
#else
		sock = rpc_connect_tcp(RPC_GLOBAL(php3_rpc_module).host,
							   RPC_GLOBAL(php3_rpc_module).port);
#endif
		if (sock != -1) {
			RPC_GLOBAL(php3_rpc_module).socket = sock;
		} else {
			/* fail silently */
			RPC_GLOBAL(php3_rpc_module).socket = -1;
		}
	}

	return SUCCESS;
}


int
php3_mshutdown_rpc()
{
	if (RPC_GLOBAL(php3_rpc_module).socket != -1) {
		close(RPC_GLOBAL(php3_rpc_module).socket);
		RPC_GLOBAL(php3_rpc_module).socket = -1;
	}
	return SUCCESS;
}


void php3_info_rpc()
{
	if (RPC_GLOBAL(php3_rpc_module).socket == -1) {
		PUTS("No connection");
	} else {
#if HAVE_SYS_UN_H
		if (RPC_GLOBAL(php3_rpc_module).port == -1) {
			php3_printf("Connected to %s",
						RPC_GLOBAL(php3_rpc_module).host);
		} else {
			php3_printf("Connected to %s port %d",
						RPC_GLOBAL(php3_rpc_module).host,
						RPC_GLOBAL(php3_rpc_module).port);
		}
#else
		php3_printf("Connected to %s port %d",
					RPC_GLOBAL(php3_rpc_module).host,
					RPC_GLOBAL(php3_rpc_module).port);
#endif
	}
}


#ifdef HAVE_SYS_UN_H
static int
rpc_connect_unix(char *path)
{
	struct sockaddr_un saddr;
	int fd;

	saddr.sun_family = AF_UNIX;
	strncpy(saddr.sun_path, path, sizeof(saddr.sun_path));
	saddr.sun_path[sizeof(saddr.sun_path) - 1] = '\0';
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (connect(fd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		return -1;
	}
	return fd;
}
#endif


static int
rpc_connect_tcp(char *host, int port)
{
	struct sockaddr_in saddr;
	int fd;

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_addr.s_addr = lookup_hostname(host);
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons((unsigned short)port);
	fd = socket(saddr.sin_family, SOCK_STREAM, 0);
	if (connect(fd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		return -1;
	}
	return fd;
}


/* TODO:
 * - check for references and use placeholders for those parameters
 */
void
php3_rpc_call(INTERNAL_FUNCTION_PARAMETERS)
{
	pval **argv;
	int argc, i, size, len, sock, readlen, writelen, pos;
	char *buf, readbuf[1024]; /* XXX FIXME hardcoded limit! */
	rpc_opcode_t opcode;
	RPC_TLS_VARS;

	argc = ARG_COUNT(ht);
	if (argc < 1) {
		WRONG_PARAM_COUNT;
	}
	argv = emalloc(argc * sizeof(pval *));
	if (getParametersArray(ht, argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	sock = RPC_GLOBAL(php3_rpc_module).socket;
	if (sock == -1) {
		php3_error(E_WARNING, "no RPC server connection");
		RETURN_FALSE;
	}
	convert_to_string(argv[0]);
	size = 50; /* size will be doubled every time the buffer is too small */
	buf = emalloc(size);
	len = 0;
	rpc_put_opcode(&buf, &len, &size, RPC_FUNCTION_CALL);
	rpc_put_pval(&buf, &len, &size, argv[0]);
	rpc_put_opcode(&buf, &len, &size, RPC_FUNCTION_NPARAMS);
	rpc_put_short(&buf, &len, &size, argc - 1);
	for (i = 1; i < argc; i++) {
		rpc_put_opcode(&buf, &len, &size, RPC_FUNCTION_PARAMETER);
		rpc_put_pval(&buf, &len, &size, argv[i]);
	}
	writelen = write(sock, buf, len);
	if (writelen != len) {
		php3_error(E_WARNING, "XXX FIXME have to make a write loop");
		RETURN_FALSE;
	}
	/* XXX FIXME not efficient use of read() here */
	readlen = read(sock, (char *)&opcode, sizeof(opcode));
	if (readlen != sizeof(opcode)) {
		php3_error(E_WARNING, "malformed RPC response");
		RETURN_FALSE;
	}
	php3_printf("opcode=%d\n", opcode);
	switch (opcode) {
		case RPC_RETURN_VALUE: {
			pval *val;

			val = rpc_get_pval(readbuf, &pos, readlen);
			if (val == NULL) {
				RETVAL_FALSE;
			} else {
				memcpy(return_value, val, sizeof(pval));
			}
			break;
		}
		case RPC_ERROR: {
			rpc_error_t error;

			error = rpc_get_opcode(readbuf, &pos, readlen);
			php3_error(E_WARNING, "RPC error %d", error);
			RETVAL_FALSE;
			break;
		}
		default: {
			php3_error(E_WARNING, "malformed RPC response");
			RETVAL_FALSE;
			break;
		}
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
