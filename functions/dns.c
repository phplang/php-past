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
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id: dns.c,v 1.32 1998/01/24 05:25:27 jim Exp $ */

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if MSVC5
#include <winsock.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include "internal_functions.h"
#include "dns.h"

function_entry dns_functions[] = {
	{"gethostbyaddr",	php3_gethostbyaddr,		NULL},
	{"gethostbyname",	php3_gethostbyname,		NULL},
	{NULL, NULL, NULL}
};

char *_php3_gethostbyaddr(char *ip);
char *_php3_gethostbyname(char *name);

void php3_gethostbyaddr(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	return_value->value.strval = _php3_gethostbyaddr(arg->value.strval);
	return_value->strlen = strlen(return_value->value.strval);
	return_value->type = IS_STRING;
}


char *_php3_gethostbyaddr(char *ip)
{
	unsigned long addr;
	struct hostent *hp;

	if ((int) (addr = inet_addr(ip)) == -1) {
#if DEBUG
		php3_error(E_WARNING, "address not in a.b.c.d form");
#endif
		return estrdup(ip);
	}
	hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
	if (!hp) {
#if DEBUG
		php3_error(E_WARNING, "Unable to resolve %s\n", ip);
#endif
		return estrdup(ip);
	}
	return estrdup(hp->h_name);
}


void php3_gethostbyname(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arg;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg);

	return_value->value.strval = _php3_gethostbyname(arg->value.strval);
	return_value->strlen = strlen(return_value->value.strval);
	return_value->type = IS_STRING;
}


char *_php3_gethostbyname(char *name)
{
	struct hostent *hp;
	struct in_addr in;

	hp = gethostbyname(name);
	if (!hp || !hp->h_addr_list) {
		php3_error(E_WARNING, "Unable to resolve %s\n", name);
		return estrdup(name);
	}
	memcpy(&in.s_addr, *(hp->h_addr_list), sizeof(in.s_addr));
	return estrdup(inet_ntoa(in));
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
