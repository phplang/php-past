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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: basic_functions.h,v 1.17 1998/02/20 20:57:08 shane Exp $ */

#ifndef _BASIC_FUNCTIONS_H
#define _BASIC_FUNCTIONS_H

extern php3_module_entry basic_functions_module;
#define basic_functions_module_ptr &basic_functions_module

extern int php3_rinit_basic(INITFUNCARG);
extern int php3_rshutdown_basic(void);
extern void int_value(INTERNAL_FUNCTION_PARAMETERS);
extern void double_value(INTERNAL_FUNCTION_PARAMETERS);
extern void string_value(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_toggle_short_open_tag(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sleep(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_usleep(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_key_sort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_asort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_arsort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rsort(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_count(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_flush(INTERNAL_FUNCTION_PARAMETERS);
extern void array_end(INTERNAL_FUNCTION_PARAMETERS);
extern void array_prev(INTERNAL_FUNCTION_PARAMETERS);
extern void array_next(INTERNAL_FUNCTION_PARAMETERS);
extern void array_reset(INTERNAL_FUNCTION_PARAMETERS);
extern void array_current(INTERNAL_FUNCTION_PARAMETERS);
extern void array_current_key(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_gettype(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_settype(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_min(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_max(INTERNAL_FUNCTION_PARAMETERS);

/* system functions */
extern void php3_getenv(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_putenv(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_error_reporting(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_get_current_user(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_set_time_limit(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_get_cfg_var(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_set_magic_quotes_runtime(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_is_type(INTERNAL_FUNCTION_PARAMETERS, int type);
extern void php3_is_long(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_is_double(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_is_string(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_is_array(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_is_object(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_leak(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_error_log(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _BASIC_FUNCTIONS_H */
