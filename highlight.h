/* $Id: highlight.h,v 1.4 1998/12/28 09:43:53 sas Exp $ */

#ifndef _HILIGHT_H
#define _HILIGHT_H

#define HL_COMMENT_COLOR     "#FF8000"    /* orange */
#define HL_DEFAULT_COLOR     "#0000BB"    /* blue */
#define HL_HTML_COLOR        "#000000"    /* black */
#define HL_STRING_COLOR      "#DD0000"    /* red */
#define HL_BG_COLOR          "#FFFFFF"    /* white */
#define HL_KEYWORD_COLOR     "#007700"    /* green */

extern void syntax_highlight(Token *next_token);

#endif
