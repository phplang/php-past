/***[microtime.c]*************************************************[TAB=4]****\
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
 * Contributed by Paul Panotzki - Bunyip Information Systems
 */
/* $Id: microtime.c,v 1.11 1997/09/13 15:57:19 shane Exp $ */
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

#define NUL  '\0'
#define MICRO_IN_SEC 1000000.00

void MicroTime(void) {
#ifdef HAVE_GETTIMEOFDAY
	struct timeval tp;
	long sec=0L;
	double msec = 0.0;
	char *ret;
  
	ret=(char *)emalloc(1,sizeof(char)*64);
	*ret='\0';
  
	if ( gettimeofday((struct timeval *)&tp, (NUL)) == 0 ) 
	{
		msec = (double)(tp.tv_usec/MICRO_IN_SEC);
		sec = tp.tv_sec;
	}
  
	sprintf(ret,"%.8f %ld",msec , sec);
	Push(ret, STRING);
#else
	Error("MicroTime function not available");	
	Push("0 0",STRING);
#endif
}
