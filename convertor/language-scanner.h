
/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@vipe.technion.ac.il>                     |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id: language-scanner.h,v 1.8 2000/02/07 23:54:50 zeev Exp $ */


#ifndef _LANGUAGE_SCANNER_H
#define _LANGUAGE_SCANNER_H

typedef struct {
	YY_BUFFER_STATE buffer_state;
	int state;
	uint return_offset;
	unsigned char type;
	uint lineno;
	char *eval_string;
} PHPLexState;

#define LEX_STATE_INCLUDE 0
#define LEX_STATE_EVAL 1
#define LEX_STATE_DISPLAY_SOURCE 2
#define LEX_STATE_CONDITIONAL_INCLUDE 3

extern TokenCacheManager token_cache_manager;

extern void end_php();

#endif
