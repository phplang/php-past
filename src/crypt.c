/***[crypt.c]*****************************************************[TAB=4]****\
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
/* $Id: crypt.c,v 1.5 1997/01/04 15:16:50 rasmus Exp $ */
#include "php.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif
#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif
#include "parse.h"

/*
 * If mode is non-zero, a salt is expected.
 * If mode is zero, a pseudo-random salt will be selected.
 */
void Crypt(int mode) {
#ifdef HAVE_CRYPT
	Stack *s;
	char salt[8];
	char *enc;
	
	salt[0] = '\0';
	if(mode) {
		s = Pop();
		if(!s) {
			Error("Stack error in crypt");
			return;
		}
		if(s->strval) strncpy(salt,s->strval,2);
	} 
	s = Pop();
	if(!s) {
		Error("Stack error in crypt");
		return;
	}
	if(!salt[0]) {
		salt[0] = 'A' + (time(NULL) % 26);
		salt[1] = 'a' + (time(NULL) % 26);
		salt[2] = '\0';
	}
	StripSlashes(s->strval);
	enc = (char *)crypt(s->strval,salt);
#if DEBUG
	Debug("Crypt returned [%s]\n",enc);
#endif
	Push(enc,STRING);	

#else
	Error("No crypt support compiled into this version");
#endif
}	
