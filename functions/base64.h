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
   | Author: Jim Winstead (jimw@php.net)                                  |
   +----------------------------------------------------------------------+
 */
/* $Id: base64.h,v 1.6 1998/01/15 22:01:07 jim Exp $ */

#ifndef _BASE64_h
#define _BASE64_h

extern void php3_base64_decode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_base64_encode(INTERNAL_FUNCTION_PARAMETERS);

extern char *base64_encode(const char *);
extern char *base64_decode(const char *);

#endif /* _BASE64_h */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
