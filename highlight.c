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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */

/* $Id: highlight.c,v 1.41 2000/04/10 19:29:36 andi Exp $ */

#define YY_TLS_VARS
extern char *phptext;
extern int phpleng;
extern int ExecuteFlag,Execute; /* remove me */

#include "php.h"
#include "language-parser.tab.h"
#include "main.h"
#include "highlight.h"

void syntax_highlight(Token *next_token)
{
	register int i;
	int highlight_keyword;
	YY_TLS_VARS;
	TLS_VARS;
	
	switch (next_token->token_type) {
		case FUNCTION:
		case OLD_FUNCTION:
		case PHP_CONST:
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
		case ENDFOR:
		case BREAK:
		case CONTINUE:
		case INCLUDE:
		case HIGHLIGHT_FILE:
		case HIGHLIGHT_STRING:
		case EVAL:
		case PHP_ARRAY:
		case PHP_LIST:
		case PHP_LINE:
		case PHP_FILE:
		case '{':
		case '}':
		case '(':
		case ')':
			highlight_keyword=1;
			break;
		case ';':
			if (phpleng == 1) { /* a real ;, as opposed to an implicit ; at the end of a PHP block */
				PUTS("<FONT color=\"");
				PUTS(php3_ini.highlight_keyword);
				PUTS("\">");
				html_putc(';');
				PUTS("</FONT>");
			}
			return;
			break;
		case '\'':
		case '"':
		case '`':
			return; /* handled in the scanner */
			break;
		default:
			highlight_keyword=0;
			break;
	}
	if (highlight_keyword) {
		PUTS("<FONT color=\"");
		PUTS(php3_ini.highlight_keyword);
		PUTS("\">");
	}
	for (i = 0; i < phpleng; i++) {
		html_putc(phptext[i]);
	}
	if (highlight_keyword) {
		PUTS("</FONT>");
	}
	if (next_token->phplval.type==IS_STRING) {
		STR_FREE(next_token->phplval.value.str.val);
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
