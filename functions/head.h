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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
#ifndef _HEAD_H
#define _HEAD_H
#include "modules.h"

/* 
   We are still using a PHP2-style Push/Pop list here as opposed
   to the PHP3 built-in list functionality because of the nature
   of this particular list.  It is just used as a structured
   buffer.  Doing this with the built-in list code would require
   some changes to allow a search for the first item with a
   certain type.  This type of search would not be optimal.
   Private list management makes more sense here
*/
typedef struct CookieList {
	char *name;
	char *value;
	time_t expires;
	char *path;
	char *domain;
	int secure;
	struct CookieList *next;
} CookieList;

extern php3_module_entry php3_header_module_entry;
#define php3_header_module_ptr &php3_header_module_entry

extern int php3_init_head(INITFUNCARG);
extern void php3_Header(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_SetCookie(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_noheader(void);
extern PHPAPI int php3_header(int type, char *str);
extern void php3_noheader(void);
extern int php3_headers_unsent(void);

#endif
