/***[tmagic.c]****************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996 Rasmus Lerdorf                                         *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License as published by      *
*  the Free Software Foundation; either version 2 of the License, or         *
*  (at your option) any later version.                                       *
*                                                                            *
*  This program is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU General Public License for more details.                              *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with this program; if not, write to the Free Software               *
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
*                                                                            *
\****************************************************************************/
/* $Id: tmagic.c,v 1.3 1996/07/18 18:20:30 rasmus Exp $ */
#include "php.h"
#include "parse.h"
#if APACHE
#include "http_protocol.h"
#endif

#if TEXT_MAGIC
void text_magic(char *str) {
	VarTree *var;
	int cnt;
	char *new;

	var = GetVar("tm",NULL,0);
	if(!var) {
#if DEBUG
		Debug("tm is not set\n");
#endif
		return;
	}
	cnt = var->count;	
	while(cnt && var) {
		if(!var->deleted) {
			new = _RegReplace(var->strval,"XXXXXXX",str);
			if(new!=str) strcpy(str,new);
			php_pool_clear(1);
		}
		cnt--;
		var=var->next;
	}
}
#endif
