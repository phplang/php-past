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
   | Authors: Andi Gutmans <andi@vipe.technion.ac.il>                     |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */

/* $Id: main.c,v 1.4 1997/12/31 15:55:49 rasmus Exp $ */

#include "token_cache.h"

extern int phpparse(void);

TokenCacheManager tcm;

void phperror(char *str)
{
	fprintf(stderr,str);
}

/* phpparse()'s front end to the token cache */
int phplex(YYSTYPE *phplval)
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
