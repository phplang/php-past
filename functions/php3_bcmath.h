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
   +----------------------------------------------------------------------+
 */


/* $Id: php3_bcmath.h,v 1.13 1998/01/17 21:24:01 andi Exp $ */

#ifndef _PHP3_BCMATH_H
#define _PHP3_BCMATH_H

#if COMPILE_DL
#undef WITH_BCMATH
#define WITH_BCMATH 1
#endif

#if WITH_BCMATH

extern php3_module_entry bcmath_module_entry;
#define bcmath_module_ptr &bcmath_module_entry

extern int php3_rinit_bcmath(INITFUNCARG);
extern int php3_rend_bcmath(void);
extern void php3_bcmath_add(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bcmath_sub(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bcmath_mul(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bcmath_div(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bcmath_mod(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bcmath_pow(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bcmath_sqrt(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bcmath_comp(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bcmath_set_scale(INTERNAL_FUNCTION_PARAMETERS);

#else

#define bcmath_module_ptr NULL

#endif

#endif /* _PHP3_BCMATH_H */
