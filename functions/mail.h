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


/* $Id: mail.h,v 1.5 1998/02/03 19:40:00 shane Exp $ */

#ifndef _MAIL_H
#define _MAIL_H
#if HAVE_SENDMAIL
extern php3_module_entry mail_module_entry;
#define mail_module_ptr &mail_module_entry

extern void php3_mail(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_info_mail(void);
extern int _php3_mail(char *to, char *subject, char *message, char *headers);

#else
#define mail_module_ptr NULL
#endif
#endif /* _MAIL_H */
