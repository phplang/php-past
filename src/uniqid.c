/***[uniqid.c]****************************************************[TAB=4]****\
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
/*
 * Contributed by Stig Bakken - Guardian Networks AS <ssb@guardian.no>
 *
 * $Source: /repository/phpfi/src/uniqid.c,v $
 * $Id: uniqid.c,v 1.4 1997/09/13 15:59:36 shane Exp $
 *
 */

#include "php.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#include "parse.h"

#include <stdio.h>
#ifdef HAVE_GETTIMEOFDAY
#if WINNT|WIN32
#include "win32/time.h"
#else
#include <sys/time.h>
#endif
#endif

void UniqId(void)
{
#ifdef HAVE_GETTIMEOFDAY
    char *prefix;
    char uniqid[128];
    int sec, usec;
    struct timeval tv;
    Stack *s;

    s = Pop();
    if (!s) {
	Error("Stack Error in uniqid function");
	return;
    }

    prefix = s->strval;

    /* Do some bounds checking since we are using a char array.
     */
    if (strlen(prefix) > 114) {
	Error("The prefix to uniqid should not be more than 114 characters.");
	Push("", STRING);
	return;
    }

    gettimeofday((struct timeval *)&tv, (struct timezone *)NULL);
    sec = (int)tv.tv_sec;
    usec = (int)(tv.tv_usec % 1000000);

    /* The max value usec can have is 0xF423F, so we use only five hex
     * digits for usecs:
     */
    sprintf(uniqid, "%s%08x%05x", prefix, sec, usec);
    Push(uniqid, STRING);

#else
    Error("UniqId function not available");	
    Push("0",STRING);
#endif    
}
