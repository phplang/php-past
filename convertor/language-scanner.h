
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
   | Authors: Andi Gutmans <andi@vipe.technion.ac.il>                     |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: language-scanner.h,v 1.3 1997/12/31 15:55:48 rasmus Exp $ */


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
