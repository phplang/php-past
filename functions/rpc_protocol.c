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

#include "php.h"

#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "rpc_protocol.h"
#include "php3_var.h"


void
rpc_error(int conn, rpc_error_t code)
{
	rpc_opcode_t opcode = RPC_ERROR;
	int len = sizeof(opcode) + sizeof(code);
	char *ptr;

	ptr = emalloc(len);
	memcpy(ptr, (char *)&opcode, sizeof(opcode));
	memcpy(ptr+sizeof(opcode), (char *)&code, sizeof(code));
	write(conn, ptr, len);
	efree(ptr);
}


void
rpc_return_string(int conn, char *string)
{
	int size = 20, len = 0;
	char *ptr = emalloc(size);
    pval *return_value = emalloc(sizeof(pval));

    rpc_put_opcode(&ptr, &len, &size, RPC_RETURN_VALUE);
    RETVAL_STRING(string, 0);
    rpc_put_pval(&ptr, &len, &size, return_value);
	write(conn, ptr, len);
    efree(return_value);
    efree(ptr);
}


unsigned short
rpc_get_short(char *data, int *pos, int size)
{
    unsigned short nvalue;
    char *ptr  = (char *)&nvalue;
    int len = sizeof(nvalue);

    if (size - *pos <= 0) {
		return RPC_ERROR_OPCODE;
    }

    memcpy(ptr, &data[*pos], len);
    *pos += len;

    return ntohs(nvalue);
}


rpc_opcode_t
rpc_get_opcode(char *data, int *pos, int size)
{
	return (rpc_opcode_t)rpc_get_short(data, pos, size);
}


rpc_error_t
rpc_get_error(char *data, int *pos, int size)
{
	return (rpc_error_t)rpc_get_short(data, pos, size);
}


unsigned long
rpc_get_long(char *data, int *pos, int size)
{
    unsigned long nvalue;
    char *ptr  = (char *)&nvalue;
    int len = sizeof(nvalue);

    if (size - *pos <= 0) {
		return RPC_ERROR_OPCODE;
    }

    memcpy(ptr, &data[*pos], len);
    *pos += len;

    return ntohl(nvalue);
}


pval *
rpc_get_pval(char *data, int *pos, int size)
{
    unsigned short ntype, type;
    pval *val;


    if (size < (*pos) + sizeof(ntype)) {
		printf("\trpc_get_pval: not enough data 1\n");
		return NULL;
    }
    val = emalloc(sizeof(pval));
    memcpy((char *)&ntype, &data[*pos], sizeof(ntype));
    *pos += sizeof(ntype);
    type = ntohs(ntype);
    switch (type) {
		case RPC_TYPE_STRING: {
			unsigned long nlen, len;

			val->type = IS_STRING;
			if (size < (*pos) + sizeof(nlen)) {
				printf("\trpc_get_pval: not enough data 2\n");
				return NULL;
			}
			memcpy((char *)&nlen, &data[*pos], sizeof(nlen));
			len = ntohl(nlen);
			*pos += sizeof(nlen);
			if (size < (*pos) + len) {
				printf("\trpc_get_pval: not enough data 3\n");
				return NULL;
			}
			val->value.str.val = emalloc(len);
			memcpy(val->value.str.val, &data[*pos], len);
			val->value.str.len = len;
			*pos += len;
			break;
		}
		case RPC_TYPE_LONG: {
			unsigned long nvalue, value;

			val->type = IS_LONG;
			if (size < (*pos) + sizeof(nvalue)) {
				printf("\trpc_get_pval: not enough data 4\n");
				return NULL;
			}
			memcpy((char *)&nvalue, &data[*pos], sizeof(nvalue));
			value = ntohl(nvalue);
			val->value.lval = (long)value;
			*pos += sizeof(nvalue);
			break;
		}
		default:
			/* unsupported type */
			printf("\trpc_get_pval: unsupported type %d\n", type);
			return NULL;
    }
    return val;
}


void
rpc_put_short(char **buffer, /* data buffer */
			  int *pos,      /* pointer to current buffer offset */
			  int *size,     /* pointer to buffer size */
			  unsigned short value) /* value to append to the buffer */
{
	int len = sizeof(value);
	char *juks;

	value = htons(value);
	juks = (char *)&value;
	rpc_put_raw(buffer, pos, size, juks, len);
}


void
rpc_put_long(char **buffer, /* data buffer */
			 int *pos,      /* pointer to current buffer offset */
			 int *size,     /* pointer to buffer size */
			 long value)    /* value to append to the buffer */
{
	int len = sizeof(value);
	char *juks;

	value = htonl(value);
	juks = (char *)&value;
	rpc_put_raw(buffer, pos, size, juks, len);
}


void
rpc_put_pval(char **buffer, /* data buffer */
			 int *pos,      /* pointer to current buffer offset */
			 int *size,     /* pointer to buffer size */
			 pval *val)     /* pval to serialize and append */
{
	pval srl;

	php3api_var_serialize(&srl, val);
	if (srl.value.str.len > 0) {
		rpc_put_raw(buffer, pos, size, srl.value.str.val, srl.value.str.len);
		efree(srl.value.str.val);
	}
}


void
rpc_put_raw(char **buffer, /* data buffer */
			int *pos,      /* pointer to current buffer offset */
			int *size,     /* pointer to buffer size */
			char *data,    /* data to append to the buffer */
			int len)       /* number of bytes to append */
{
	if ((*pos + len) >= *size) {
		while ((*pos + len) >= *size) {
			*size *= 2;
		}
		*buffer = erealloc(*buffer, *size);
	}
	memcpy(&(*buffer)[*pos], data, len);
	*pos += len;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
