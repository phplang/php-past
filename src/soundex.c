/***[soundex.c]***************************************************[TAB=4]****\
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
 * Contributed by Bjørn Borud - Guardian Networks AS <borud@guardian.no>
 *
 * $Source: /repository/phpfi/src/soundex.c,v $
 * $Id: soundex.c,v 1.2 1997/01/21 01:12:53 rasmus Exp $
 *
 */

#include "php.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "parse.h"

/* Simple soundex algorith as described by Knuth in TAOCP, vol 3 */
void Soundex(void) {
  char i, j, n;
  char l, u;
  char *somestring;

  /* pad with '0' and terminate with 0 ;-) */
  char soundex[5] = {'0','0','0','0',0};

  static char soundex_table[26] = {0,   /* A */
				   '1', /* B */
				   '2', /* C */
				   '3', /* D */
				   0,   /* E */
				   '1', /* F */
				   '2', /* G */
				   0,   /* H */
				   0,   /* I */
				   '2', /* J */
				   '2', /* K */
				   '4', /* L */
				   '5', /* M */
				   '5', /* N */
				   0,   /* O */
				   '1', /* P */
				   '2', /* Q */
				   '6', /* R */
				   '2', /* S */
				   '3', /* T */
				   0,   /* U */
				   '1', /* V */
				   0,   /* W */
				   '2', /* X */
				   0,   /* Y */
				   '2'};/* Z */
  Stack *s;


  s = Pop();
  if (!s) {
    Error("Stack error in soundex function");
    return;
  }
  somestring = s->strval;


  n = strlen(somestring);

  /* convert chars to upper case and strip non-letter chars */
  j = 0;
  for (i = 0; i < n; i++) {
    u = toupper(somestring[i]);
    if ((u > 64) && (u < 91)) {
      somestring[j] = u;
      j++;
    }
  }

  /* null-terminate string */
  somestring[j] = 0;

  n = strlen(somestring);

  /* prefix soundex string with first valid char */
  soundex[0] = somestring[0];

  /* remember first char */
  l = soundex_table[((somestring[0]) - 65)];

  j = 1;

  /* build soundex string */
  for (i = 1; i < n && j < 4; i++) {
    u = soundex_table[((somestring[i]) - 65)];

    if (u != l) {
      if (u != 0) {
	soundex[(int)j++] = u;
      }
      l = u;
    }
  }
  Push(soundex, STRING);
}
