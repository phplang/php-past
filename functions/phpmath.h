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
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */


/* $Id: phpmath.h,v 1.7 1998/01/25 17:29:24 ssb Exp $ */

#ifndef _PHPMATH_H
#define _PHPMATH_H
extern void php3_sin(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cos(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_tan(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_asin(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_acos(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_atan(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pi(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_exp(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_log(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_log10(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_pow(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_sqrt(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_srand(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_rand(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_getrandmax(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_abs(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ceil(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_floor(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_round(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_decbin(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dechex(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_decoct(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_bindec(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hexdec(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_octdec(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _PHPMATH_H */
