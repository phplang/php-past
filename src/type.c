/***[type.c]******************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996,1997 Rasmus Lerdorf                                    *
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
/* $Id: type.c,v 1.5 1997/08/09 01:42:51 rasmus Exp $ */
#include <stdio.h>
#include "php.h"
#include "parse.h"

/*
 * Determines if 'str' is an integer, real number or a string
 *
 * Note that leading zeroes automatically force a STRING type
 */
int CheckType(char *str) {
	char *s;
	int type=LNUMBER;

	s = str;
	if(*s=='0' && *(s+1) && *(s+1)!='.') return(STRING);
	if(*s=='+' || *s=='-' || (*s>='0' && *s <='9') || *s=='.' ) {
		if(*s=='.') type=DNUMBER;
		s++;
		while(*s) {
			if(*s>='0' && *s<='9') { 
				s++; 
				continue; 
			}
			else if(*s=='.' && type==LNUMBER) { type=DNUMBER; s++; continue; }
			else return(STRING);
		}
	} else return(STRING);
	return(type);
} /* CheckType */

/*
 * 0 - simple variable
 * 1 - non-index array
 * 2 - index array
 */
int CheckIdentType(char *str) {
	char *s;

	if(!(s=(char *)strchr(str,'['))) return(0);
	s++;
	while(*s==' ' || *s=='\t' || *s=='\n') s++;
	if(*s == ']') return(1);
	return(2);
}

char *GetIdentIndex(char *str) {
	static char temp[256];
	char *s, *t;
	char o;

	temp[0] = '\0';
	s = (char *)strchr(str,'[');
	if(s) {
		t = (char *)strrchr(str,']');
		if(t) {
			o=*t;
			*t='\0';
			strcpy(temp,s+1);
			*t=o;
		}
	}
	return(temp);
}
			
