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
   | Authors: Rasmus Lerdorf                                              |
   |                                                                      |
   +----------------------------------------------------------------------+
 */
/* $Id: type.c,v 1.12 1997/12/31 15:56:52 rasmus Exp $ */
#include "parser.h"
#include "type.h"

/*
 * Determines if 'str' is an integer (long), real number or a string
 *
 * Note that leading zeroes automatically force a STRING type
 */
int php3_CheckType(char *str)
{
	char *s;
	int type = IS_LONG;

	s = str;
	if (*s == '0' && *(s + 1) && *(s + 1) != '.')
		return (IS_STRING);
	if (*s == '+' || *s == '-' || (*s >= '0' && *s <= '9') || *s == '.') {
		if (*s == '.')
			type = IS_DOUBLE;
		s++;
		while (*s) {
			if (*s >= '0' && *s <= '9') {
				s++;
				continue;
			} else if (*s == '.' && type == IS_LONG) {
				type = IS_DOUBLE;
				s++;
				continue;
			} else
				return (IS_STRING);
		}
	} else
		return (IS_STRING);

	return (type);
}								/* php3_CheckType */

/*
 * 0 - simple variable
 * 1 - non-index array
 * 2 - index array
 */
int php3_CheckIdentType(char *str)
{
	char *s;

	if (!(s = (char *) strchr(str, '[')))
		return (0);
	s++;
	while (*s == ' ' || *s == '\t' || *s == '\n')
		s++;
	if (*s == ']')
		return (1);
	return (2);
}

char *php3_GetIdentIndex(char *str)
{
	char *temp;
	char *s, *t;
	char o;

	temp = emalloc(strlen(str));
	temp[0] = '\0';
	s = (char *) strchr(str, '[');
	if (s) {
		t = (char *) strrchr(str, ']');
		if (t) {
			o = *t;
			*t = '\0';
			strcpy(temp, s + 1);
			*t = o;
		}
	}
	return (temp);
}
