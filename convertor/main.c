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
   | Authors: Andi Gutmans <andi@vipe.technion.ac.il>                     |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */

/* $Id: main.c,v 1.10 2000/02/07 23:54:50 zeev Exp $ */

#include "token_cache.h"

extern int phpparse(void);

TokenCacheManager tcm;

void phperror(char *str)
{
	fprintf(stderr,str);
}

/* phpparse()'s front end to the token cache */
int phplex(pval *phplval)
{
    Token *token;
    
    switch (read_next_token(&tcm, &token, phplval)) {
        case FAILURE:
            phperror("Unable to read next token!\n");
            return 0;
            break;
    } 
    *phplval = token->phplval;
    return token->token_type;
}         


main()
{
	tcm_init(&tcm);
	phpparse();
	return 0;
}	
