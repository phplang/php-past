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

#ifndef _PHP3_RPC_PROTOCOL_H
# define _PHP3_RPC_PROTOCOL_H

typedef unsigned short rpc_opcode_t;
typedef unsigned short rpc_error_t;

# define RPC_ERROR_OPCODE         0

# define RPC_NOP                  1

/* function calling opcodes */
# define RPC_FUNCTION_CALL        2
# define RPC_FUNCTION_NPARAMS     3
# define RPC_FUNCTION_PARAMETER   4
# define RPC_RETURN_VALUE         5
# define RPC_ERROR                6

/* value passing opcodes */
# define RPC_TYPE_LONG            10
# define RPC_TYPE_STRING          11
# define RPC_TYPE_DOUBLE          12
# define RPC_TYPE_ARRAY           13

# define RPC_DONE                 99

# define rpc_put_opcode rpc_put_short
# define rpc_put_error rpc_put_short

void rpc_error(int, rpc_error_t);
void rpc_return_string(int, char *);
unsigned short rpc_get_short(char *, int *, int);
rpc_opcode_t rpc_get_opcode(char *, int *, int);
rpc_error_t rpc_get_error(char *, int *, int);
unsigned long rpc_get_long(char *, int *, int);
pval *rpc_get_pval(char *, int *, int);
void rpc_put_short(char **, int *, int *, unsigned short);
void rpc_put_long(char **, int *, int *, long);
void rpc_put_pval(char **, int *, int *, pval *);
void rpc_put_raw(char **, int *, int *, char *, int);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
