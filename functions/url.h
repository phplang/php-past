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
   | Authors: Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
 */
/* $Id: url.h,v 1.10 1998/01/17 09:15:06 jaakko Exp $ */

typedef struct url {
	char *scheme;
	char *user;
	char *pass;
	char *host;
	unsigned short port;
	char *path;
	char *query;
	char *fragment;
} url;

extern void free_url(url *);
extern url *url_parse(char *);
extern void _php3_urldecode(char *);
extern char *_php3_urlencode(char *);
extern void _php3_rawurldecode(char *);
extern char *_php3_rawurlencode(char *);

extern void php3_parse_url(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_urlencode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_urldecode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rawurlencode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rawurldecode(INTERNAL_FUNCTION_PARAMETERS);

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
