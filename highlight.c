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

#ifdef THREAD_SAFE
#include "tls.h"
#else
#define YY_TLS_VARS
extern char *phptext;
extern int phpleng;
extern int ExecuteFlag,Execute; /* remove me */
#endif

#include "parser.h"
#include "language-parser.tab.h"
#include "main.h"
#include "highlight.h"

void syntax_highlight(Token *next_token)
{
	register int i;
	YY_TLS_VARS;
	TLS_VARS;
	switch (next_token->token_type) {
		case FUNCTION:
		case OLD_FUNCTION:
		case CLASS:
		case EXTENDS:
		case VAR:
		case NEW:
		case PHP_ECHO:
		case PHP_PRINT:
		case IF:
		case ELSE:
		case ELSEIF:
		case ENDIF:
		case WHILE:
		case ENDWHILE:
		case DO:
		case FOR:
		case BREAK:
		case CONTINUE:
		case INCLUDE:
		case SHOW_SOURCE:
		case EVAL:
		case PHP_ARRAY:
		case PHP_LIST:
		case '{':
		case '}':
		case '(':
		case ')':
		case '"':
		case '\'':
		case '`':
			PUTS("<font color=\"");
			PUTS(php3_ini.highlight_keyword);
			PUTS("\">");
			break;
		case ';':
			if (phpleng == 1) {
				PUTS("<font color=\"");
				PUTS(php3_ini.highlight_keyword);
				PUTS("\">");
			}
			break;
	}
	for (i = 0; i < phpleng; i++) {
		html_putc(phptext[i]);
	}
	switch (next_token->token_type) {
		case FUNCTION:
		case OLD_FUNCTION:
		case CLASS:
		case EXTENDS:
		case VAR:
		case NEW:
		case PHP_ECHO:
		case PHP_PRINT:
		case IF:
		case ELSE:
		case ELSEIF:
		case ENDIF:
		case WHILE:
		case ENDWHILE:
		case DO:
		case FOR:
		case BREAK:
		case CONTINUE:
		case INCLUDE:
		case SHOW_SOURCE:
		case EVAL:
		case '{':
		case '}':
		case '(':
		case ')':
		case '"':
		case '\'':
		case '`':
		case ';':
			PUTS("</font>");
			break;
	}
	if (next_token->phplval.type==IS_STRING) {
		efree(next_token->phplval.value.strval);
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
