%{

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


/* $Id: language-scanner.lex,v 1.118 1998/02/01 23:53:05 zeev Exp $ */

%}

%x IN_PHP
%x DOUBLE_QUOTES
%x SINGLE_QUOTE
%x BACKQUOTE

%{

#if WIN32|WINNT
#include <winsock.h>
#endif

#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "language-parser.tab.h"
#include "language-scanner.h"
#include "main.h"
#include "control_structures.h"
#include "highlight.h"
#include "functions/post.h"
#if WIN32|WINNT
/*
  just redefine what we need from winsock.h so
  we dont have to deal with typedef conflicts
  between flex and winsock.h
*/
#endif

#define YY_DECL int lex_scan(YYSTYPE *phplval)
#define ECHO { PHPWRITE( yytext, yyleng ); }

#ifdef __cplusplus
#  define MY_INPUT yyinput
#else
#  define MY_INPUT input
#endif

#ifndef THREAD_SAFE
extern Stack input_source_stack;
int php3_display_source; /* whether or not we're in display-source mode */
int php3_preprocess;  /* whether we're in preprocess mode */
int phplineno=1;
#endif

#ifndef YY_TLS_VARS
/* NOTE, YY_TLS_VARS CANNOT have a semicolon after in
   in the code or it will break compilation in msvc5 */
#define YY_TLS_VARS
#endif

#ifndef TLS_VARS /* just to make sure ;) */
#define TLS_VARS
#endif

/* Macros for starting/ending syntax coloring blocks */
#define BEGIN_COLOR(c)	{ PUTS("<FONT color=\""); PUTS(c); PUTS("\">\n"); }
#define BEGIN_COLOR_SIZE(c,s) { PUTS("<FONT color=\""); PUTS(c); PUTS("\" size=\""); PUTS(s); PUTS("\">\n"); }
#define END_COLOR()	{ PUTS("</FONT>"); }
#define BEGIN_ITALIC()	{ PUTS("<I>"); }
#define END_ITALIC()	{ PUTS("</I>"); }
#define BEGIN_STRONG()	{ PUTS("<STRONG>"); }
#define END_STRONG()	{ PUTS("</STRONG>"); }

void reset_scanner()
{
	YY_TLS_VARS
	TLS_VARS;

	BEGIN(INITIAL);
	GLOBAL(phplineno)=1;
}

/* Used to make sure that a buffer is allocated for yyin */
void initialize_input_file_buffer(FILE *f)
{
	yyrestart(f);
}


int include_file(YYSTYPE *file, int display_source)
{ 
	FILE *tmp;
	YY_TLS_VARS
	YY_BUFFER_STATE buffer_state = YY_CURRENT_BUFFER;
	PHPLexState lex_state;
	char *filename;
	SOCK_VARS; /* They don't actually get used, but we have to have 'em */
	TLS_VARS;

	convert_to_string(file);

	if (file->type!=IS_STRING) {
		return FAILURE;
	}
	memcpy(&lex_state.buffer_state,&buffer_state,sizeof(YY_BUFFER_STATE));
	lex_state.type = (display_source?LEX_STATE_DISPLAY_SOURCE:LEX_STATE_INCLUDE);
	lex_state.lineno = GLOBAL(phplineno);
	lex_state.state = YYSTATE;

	stack_push(&GLOBAL(input_source_stack),&lex_state,sizeof(PHPLexState));

	tmp = php3_fopen_wrapper(file->value.strval, "r", USE_PATH|IGNORE_URL_WIN SOCK_PARG);
	if (!tmp) {
		php3_error(E_ERROR,"Failed opening required '%s'",
			    php3_strip_url_passwd(file->value.strval));
		stack_del_top(&GLOBAL(input_source_stack));
		return FAILURE;
	}
	yyin = tmp;
	yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
	BEGIN(INITIAL);
	GLOBAL(include_count)++;
	GLOBAL(phplineno)=1+MAX_TOKENS_PER_CACHE*GLOBAL(include_count);
	filename = estrndup(file->value.strval,file->strlen);
	hash_index_update(&GLOBAL(include_names),GLOBAL(include_count),(void *) &filename,sizeof(char *),NULL);

	return SUCCESS;
}


void eval_string(YYSTYPE *str,YYSTYPE *return_offset INLINE_TLS)
{
	PHPLexState lex_state,*ls;
	YY_TLS_VARS
	YY_BUFFER_STATE buffer_state = YY_CURRENT_BUFFER;

	convert_to_string(str);
	
	if (str->value.strval[0]=='\0') { /* optimize empty eval's */
		return;
	}
	/* enforce two trailing NULLs for flex... */
	str->value.strval = (char *) erealloc(str->value.strval,str->strlen+2);
	str->value.strval[str->strlen+1]=0;

	memcpy(&lex_state.buffer_state,&buffer_state,sizeof(YY_BUFFER_STATE));
	lex_state.type = LEX_STATE_EVAL;
	lex_state.state = YYSTATE;
	lex_state.return_offset = return_offset->offset+1;
	lex_state.eval_string = str->value.strval;
	stack_push(&GLOBAL(input_source_stack),&lex_state,sizeof(PHPLexState));

	tcm_new(&GLOBAL(token_cache_manager));

	yy_scan_buffer(str->value.strval, str->strlen+2);
}


int conditional_include_file(YYSTYPE *file,YYSTYPE *return_offset INLINE_TLS)
{
	PHPLexState lex_state,*ls;
	YY_TLS_VARS
	YY_BUFFER_STATE buffer_state = YY_CURRENT_BUFFER;
	FILE *tmp;
	char *filename;
	SOCK_VARS;

	convert_to_string(file);

	memcpy(&lex_state.buffer_state,&buffer_state,sizeof(YY_BUFFER_STATE));
	lex_state.type = LEX_STATE_CONDITIONAL_INCLUDE;
	lex_state.state = YYSTATE;
	lex_state.return_offset = return_offset->offset+1;
	lex_state.lineno = GLOBAL(phplineno);
	stack_push(&GLOBAL(input_source_stack),&lex_state,sizeof(PHPLexState));

	tmp = php3_fopen_wrapper(file->value.strval, "r", USE_PATH|IGNORE_URL_WIN SOCK_PARG);
	if (!tmp) {
		php3_error(E_WARNING,"Failed opening '%s' for inclusion",
			    php3_strip_url_passwd(file->value.strval));
		stack_del_top(&GLOBAL(input_source_stack));
		yystype_destructor(file _INLINE_TLS);
		return FAILURE;
	}
	yyin = tmp;
	yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
	BEGIN(INITIAL);
	GLOBAL(include_count)++;
	GLOBAL(phplineno)=1+MAX_TOKENS_PER_CACHE*GLOBAL(include_count);
	filename = file->value.strval;
	hash_index_update(&GLOBAL(include_names),GLOBAL(include_count),(void *) &filename,sizeof(char *),NULL);
	
	tcm_new(&GLOBAL(token_cache_manager));
	return SUCCESS;
}

/* redefine YY_INPUT to handle urls for win32*/
#if 0 /*WIN32|WINNT*/
#define YY_INPUT(buf,result,max_size) \
	if ( yyin->_tmpfname != "url" ){ \
	if ( yy_current_buffer->yy_is_interactive ) \
		{ \
		int c = '*', n; \
		for ( n = 0; n < max_size && \
			     (c = getc( yyin )) != EOF && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( c == EOF && ferror( yyin ) ) \
			YY_FATAL_ERROR( "input in flex scanner failed" ); \
		result = n; \
		} \
	else if ( ((result = fread( buf, 1, max_size, yyin )) == 0) \
		  && ferror( yyin ) ) \
		YY_FATAL_ERROR( "input in flex scanner failed" ); \
	}else{ /* this is a url */ \
	int recv_char=0,socketd=0; \
	/*memset(buf,0,max_size);*/ \
	socketd=yyin->_file; \
	if ( yy_current_buffer->yy_is_interactive ) \
		{ \
		int c = '*', n; \
		for ( n = 0; n < max_size && \
			     ( (recv_char=recv( socketd,(char *)&c,1,0 ))) >0 && c != '\n'; ++n ) \
			buf[n] = (char) c; \
		if ( c == '\n' ) \
			buf[n++] = (char) c; \
		if ( recv_char == SOCKET_ERROR ) \
			YY_FATAL_ERROR( "input from url in flex scanner failed" ); \
		result = n; \
		} \
	else if ((result = recv( socketd, (char *)buf, max_size, 0)) == SOCKET_ERROR) \
		  	YY_FATAL_ERROR( "input from url read in flex scanner failed" ); \
	}
#endif

%}

LNUM	[0-9]+
DNUM	[0-9]*[\.][0-9]+
HNUM	"0x"[0-9a-fA-F]+
LABEL	[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*
WHITESPACE [ \n\r\t]+
TOKENS [;:,.\[\]()|^&+-/*=%!~$<>{}?@]
ENCAPSED_TOKENS [\[\]{}$]
ESCAPED_AND_WHITESPACE [\n\t\r #'.:;,()|^&+-/*=%!~<>?@]+

%option noyylineno
%option noyywrap
%%
%{
TLS_VARS;
%}

<IN_PHP>"exit" {
	return EXIT;
}

<IN_PHP>"die" {
	return EXIT;
}

<IN_PHP>"old_function" {
	return OLD_FUNCTION;
}

<IN_PHP>"function"|"cfunction" {
	return FUNCTION;
}

<IN_PHP>"return" {
	return RETURN;
}

<IN_PHP>"if" {
	return IF;
}

<IN_PHP>"elseif" {
	return ELSEIF;
}

<IN_PHP>"endif" {
	return ENDIF;
}

<IN_PHP>"else" {
	return ELSE;
}

<IN_PHP>"while" {
	return WHILE;
}

<IN_PHP>"endwhile" {
	return ENDWHILE;
}

<IN_PHP>"do" {
	return DO;
}

<IN_PHP>"for" {
	return FOR;
}

<IN_PHP>"switch" {
	return SWITCH;
}

<IN_PHP>"endswitch" {
	return ENDSWITCH;
}

<IN_PHP>"case" {
	return CASE;
}

<IN_PHP>"default" {
	return DEFAULT;
}

<IN_PHP>"break" {
	return BREAK;
}

<IN_PHP>"continue" {
	return CONTINUE;
}

<IN_PHP>"echo" {
	return PHP_ECHO;
}

<IN_PHP>"print" {
	return PHP_PRINT;
}

<IN_PHP>"class" {
	return CLASS;
}

<IN_PHP>"extends" {
	return EXTENDS;
}

<IN_PHP,DOUBLE_QUOTES,BACKQUOTE>"->" {
	return PHP_CLASS_OPERATOR;
}

<IN_PHP>"new" {
	return NEW;
}

<IN_PHP>"var" {
	return VAR;
}

<IN_PHP>"(int)" {
	return INT_CAST;
}

<IN_PHP>"(double)" {
	return DOUBLE_CAST;
}

<IN_PHP>"(string)" {
	return STRING_CAST;
}

<IN_PHP>"eval" {
	return EVAL;
}

<IN_PHP>"include" {
	return INCLUDE;
}

<IN_PHP>"require" {
	return REQUIRE;
}

<IN_PHP>"show_source" {
	return SHOW_SOURCE;
}

<IN_PHP>"global" {
	return PHP_GLOBAL;
}

<IN_PHP>"isset" {
	return PHP_ISSET;
}

<IN_PHP>"static" {
	return PHP_STATIC;
}

<IN_PHP>"unset" {
	return PHP_UNSET;
}

<IN_PHP>"=>" {
	return PHP_DOUBLE_ARROW;
}

<IN_PHP>"list" {
	return PHP_LIST;
}

<IN_PHP>"array" {
	return PHP_ARRAY;
}

<IN_PHP>"++" {
	return INCREMENT;
}

<IN_PHP>"--" {
	return DECREMENT;
}

<IN_PHP>"==" {
	return IS_EQUAL;
}

<IN_PHP>"!="|"<>" {
	return IS_NOT_EQUAL;
}

<IN_PHP>"<=" {
	return IS_SMALLER_OR_EQUAL;
}

<IN_PHP>">=" {
	return IS_GREATER_OR_EQUAL;
}

<IN_PHP>"+=" {
	return PLUS_EQUAL;
}

<IN_PHP>"-=" {
	return MINUS_EQUAL;
}

<IN_PHP>"*=" {
	return MUL_EQUAL;
}

<IN_PHP>"/=" {
	return DIV_EQUAL;
}

<IN_PHP>".=" {
	return CONCAT_EQUAL;
}


<IN_PHP>"%=" {
	return MOD_EQUAL;
}

<IN_PHP>"&=" {
	return AND_EQUAL;
}

<IN_PHP>"|=" {
	return OR_EQUAL;
}

<IN_PHP>"^=" {
	return XOR_EQUAL;
}

<IN_PHP>"||" {
	return BOOLEAN_OR;
}

<IN_PHP>"&&" {
	return BOOLEAN_AND;
}

<IN_PHP>"OR" {
	return LOGICAL_OR;
}

<IN_PHP>"AND" {
	return LOGICAL_AND;
}

<IN_PHP>{TOKENS} {
	return yytext[0];
}

<IN_PHP>{LNUM}|{HNUM} {
	phplval->value.lval = strtol(yytext, NULL, 0);
	phplval->type = IS_LONG;
	return LNUMBER;
}

<DOUBLE_QUOTES,BACKQUOTE>{LNUM}|{HNUM} { /* treat numbers (almost) as strings inside encapsulated strings */
	phplval->value.strval = (char *)estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return NUM_STRING;
}

<IN_PHP>{DNUM} {
	phplval->value.dval = atof(yytext);
	phplval->type = IS_DOUBLE;
	return DNUMBER;
}

<IN_PHP>"__LINE__" {
	phplval->value.lval = php3_get_lineno(GLOBAL(current_lineno));
	phplval->type = IS_LONG;
	return PHP_LINE;
}

<IN_PHP>"__FILE__" {
	char *filename = php3_get_filename(GLOBAL(current_lineno));
	
	phplval->strlen = strlen(filename);
	phplval->value.strval = estrndup(filename,phplval->strlen);
	phplval->type = IS_STRING;
	return PHP_FILE;
}


<INITIAL>(([^<]|"<"[^?s]){1,400})|"<s" {
	int i;
	
	phplval->value.strval = (char *) estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	for (i=0; i<yyleng; i++) {
		if (yytext[i] == '\n') {
			GLOBAL(phplineno)++;
		}
	}
	return INLINE_HTML;
}

<INITIAL>"<?"|"<script"" "+"language"" "*"="" "*("php"|"\"php\""|"\'php\'")" "*">" {
	if (php3_ini.short_open_tag) {
		if (!(GLOBAL(initialized) & INIT_ENVIRONMENT)) {
			hash_environment();
		}
		BEGIN(IN_PHP);
		if (GLOBAL(php3_display_source)) {
			BEGIN_COLOR(php3_ini.highlight_default);
			PUTS("&lt;?");
		}
	} else {
		phplval->value.strval = (char *) estrndup(yytext, yyleng);
		phplval->strlen = yyleng;
		phplval->type = IS_STRING;
		return INLINE_HTML;
	}
}

<INITIAL>"<?php"[ \n\r\t] {
	if (yytext[yyleng-1] == '\n') {
		GLOBAL(phplineno)++;
	}
	if (!(GLOBAL(initialized) & INIT_ENVIRONMENT)) {
		hash_environment();
	}
	BEGIN(IN_PHP);
	if (GLOBAL(php3_display_source)) {
		BEGIN_COLOR(php3_ini.highlight_default);
		PUTS("&lt;?php ");
	}
}

<INITIAL>"<?php_track_vars?>"([\n]|"\r\n")? {
	GLOBAL(php3_track_vars)=1;
	if (yytext[yyleng-1]=='\n') {
	  GLOBAL(phplineno)++;
	}
	if (GLOBAL(php3_display_source)) {
		BEGIN_COLOR(php3_ini.highlight_default);
		PUTS("&lt;?php_track_vars?&gt; ");
	}
}


<IN_PHP,DOUBLE_QUOTES,BACKQUOTE>{LABEL} {
	phplval->value.strval = (char *)estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return STRING;
}


<IN_PHP>{WHITESPACE} {
	int i;

	for (i=0; i<yyleng; i++) {
		if (yytext[i] == '\n') {
			GLOBAL(phplineno)++;
		}
	}
	if (GLOBAL(php3_display_source)) {
		register int i;
		
		for (i=0; i<yyleng; i++) {
			html_putc(yytext[i]);
		}
	}
}


<IN_PHP>([#]|"//")([^\n\r?]|"?"[^>\n\r])*("?\n"|"?\r\n")? { /* eat one line comments */
	if (yytext[yyleng-1] == '\n') {
		GLOBAL(phplineno)++;
	}
	if (GLOBAL(php3_display_source)) {
		END_COLOR();
		BEGIN_ITALIC();
		BEGIN_COLOR_SIZE(php3_ini.highlight_comment, "-1");
		PUTS(yytext);
		END_COLOR();
		END_ITALIC();
		BEGIN_COLOR(php3_ini.highlight_default);
	}
}

<IN_PHP>"/*" {
	/* Eat C-style comments */
	char c;
	int start_lineno;

	start_lineno=GLOBAL(phplineno);

	if (GLOBAL(php3_display_source)) {
		END_COLOR();
		BEGIN_ITALIC();
		BEGIN_COLOR_SIZE(php3_ini.highlight_comment, "-1");
		PUTS(yytext);
	}
	for (;;) {
		if (GLOBAL(php3_display_source)) {
			while((c=MY_INPUT())!='*' && c!=EOF) {
				if (c == '\n') {
					GLOBAL(phplineno)++;
				}
				html_putc(c);
			}
		} else {
			while ((c=MY_INPUT())!='*' && c!=EOF) { /* eat up text of comment */
				if (c == '\n') {
					GLOBAL(phplineno)++;
				}
			}
		}
		if (c=='*') {
			if (GLOBAL(php3_display_source)) {
				html_putc(c);
				while ((c=MY_INPUT())=='*') {
					html_putc(c);
				}
			} else {
				while ((c=MY_INPUT())=='*');
			}
			if (GLOBAL(php3_display_source)) {
				html_putc(c);
			}
			if (c=='/') {
				if (GLOBAL(php3_display_source)) {
					END_COLOR(); END_ITALIC();
					BEGIN_COLOR(php3_ini.highlight_default);
				}
				break;	/* found the end */
			}
		}

		if (c==EOF) {
			php3_error(E_WARNING,"Unterminated comment starting line %d.\n",start_lineno);
			break;
		} else if (c=='\n') {
			GLOBAL(phplineno)++;
		}
	}
}

<IN_PHP>("?>"|"</script"" "*">")([\n]|"\r\n")? {
	if (yytext[yyleng-1] == '\n') {
		GLOBAL(phplineno)++;
	}
	BEGIN(INITIAL);
	return ';';  /* implicit ';' at php-end tag */
}

<IN_PHP>["] {
	BEGIN(DOUBLE_QUOTES);
	if (GLOBAL(php3_display_source)) {
		BEGIN_COLOR(php3_ini.highlight_string);
		BEGIN_STRONG();
	}
	return '\"';
}

<IN_PHP>[`] {
	BEGIN(BACKQUOTE);
	if (GLOBAL(php3_display_source)) {
		BEGIN_COLOR(php3_ini.highlight_string);
		BEGIN_STRONG();
	}
	return '`';
}


<IN_PHP>['] {
	BEGIN(SINGLE_QUOTE);
	if (GLOBAL(php3_display_source)) {
		BEGIN_COLOR(php3_ini.highlight_string);
		BEGIN_STRONG();
	}
	return '\'';
}


<DOUBLE_QUOTES,BACKQUOTE>{ESCAPED_AND_WHITESPACE} {
	int i;

	for (i=0; i<yyleng; i++) {
		if (yytext[i] == '\n') {
			GLOBAL(phplineno)++;
		}
	}
	phplval->value.strval = (char *) estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}

<SINGLE_QUOTE>([^'\\]|\\[^'\\])+ {
	int i;

	for (i=0; i<yyleng; i++) {
		if (yytext[i] == '\n') {
			GLOBAL(phplineno)++;
		}
	}
	phplval->value.strval = (char *) estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<DOUBLE_QUOTES>[`]+ {
	int i;

	phplval->value.strval = (char *) estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<BACKQUOTE>["]+ {
	int i;

	phplval->value.strval = (char *) estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<DOUBLE_QUOTES,BACKQUOTE>"$"[^a-zA-Z_\x7f-\xff{] {
	phplval->value.chval = yytext[0];
	if (yyleng == 2) {
		unput(yytext[1]);
	}
	return CHARACTER;	
}

<DOUBLE_QUOTES,BACKQUOTE>{ENCAPSED_TOKENS} {
	phplval->value.chval = yytext[0];
	return yytext[0];
}

<SINGLE_QUOTE>"\\'" {
	phplval->value.chval='\'';
	return CHARACTER;
}

<SINGLE_QUOTE>"\\\\" {
	phplval->value.chval='\\';
	return CHARACTER;
}

<DOUBLE_QUOTES>"\\\"" {
	phplval->value.chval='"';
	return CHARACTER;
}

<BACKQUOTE>"\\`" {
	phplval->value.chval='`';
	return CHARACTER;
}

<DOUBLE_QUOTES,BACKQUOTE>"\\". {
	switch (yytext[1]) {
		case 'n':
			phplval->value.chval='\n';
			break;
		case 't':
			phplval->value.chval='\t';
			break;
		case 'r':
			phplval->value.chval='\r';
			break;
		case '\\':
			phplval->value.chval='\\';
			break;
		case '$':
		case '[':
		case ']':
			phplval->value.chval=yytext[1];
			break;
		default:
			phplval->value.strval = estrndup(yytext,yyleng);
			phplval->strlen = yyleng;
			phplval->type = IS_STRING;
			return BAD_CHARACTER;
			break;
	}
	return CHARACTER;
}

<DOUBLE_QUOTES>["] {
	BEGIN(IN_PHP);
	if (GLOBAL(php3_display_source)) {
		END_STRONG(); END_COLOR();
	}
	return '\"';
}


<BACKQUOTE>[`] {
	BEGIN(IN_PHP);
	if (GLOBAL(php3_display_source)) {
		END_STRONG(); END_COLOR();
	}
	return '`';
}


<SINGLE_QUOTE>['] {
	BEGIN(IN_PHP);
	if (GLOBAL(php3_display_source)) {
		END_STRONG(); END_COLOR();
	}
	return '\'';
}


<DOUBLE_QUOTES,BACKQUOTE,INITIAL,IN_PHP><<EOF>> {
	if (stack_is_empty(&GLOBAL(input_source_stack))) {
		yyterminate();
	} else {
		PHPLexState *lex_state;

		yy_delete_buffer(YY_CURRENT_BUFFER);
		stack_top(&GLOBAL(input_source_stack),(void **) &lex_state);

		switch(lex_state->type) {
			case LEX_STATE_CONDITIONAL_INCLUDE: /* switching out of include() */
				seek_token(&GLOBAL(token_cache_manager),lex_state->return_offset);
#if WIN32|WINNT
				if(yyin->_tmpfname=="url"){
					closesocket(yyin->_file);
					efree(yyin);
				}else{
#endif
				fclose(yyin);
#if WIN32|WINNT
				}
#endif
				BEGIN(lex_state->state);
				yy_switch_to_buffer(lex_state->buffer_state);
				GLOBAL(phplineno) = lex_state->lineno;
				stack_del_top(&GLOBAL(input_source_stack));
				return DONE_EVAL;
				break;
			case LEX_STATE_EVAL: /* switching out of eval() */
				seek_token(&GLOBAL(token_cache_manager),lex_state->return_offset);
				BEGIN(lex_state->state);
				yy_switch_to_buffer(lex_state->buffer_state);
				STR_FREE(lex_state->eval_string);
				stack_del_top(&GLOBAL(input_source_stack));
				return DONE_EVAL;
				break;
			case LEX_STATE_DISPLAY_SOURCE:
				GLOBAL(php3_display_source)=0;
				GLOBAL(ExecuteFlag) = stack_int_top(&GLOBAL(css));
				stack_del_top(&GLOBAL(css));
				GLOBAL(Execute) = SHOULD_EXECUTE;
				END_COLOR();
				/* break missing intentionally */
			case LEX_STATE_INCLUDE:
#if WIN32|WINNT
				if(yyin->_tmpfname=="url"){
					closesocket(yyin->_file);
					efree(yyin);
				}else{
#endif
				fclose(yyin);
#if WIN32|WINNT
				}
#endif
				BEGIN(lex_state->state);
				yy_switch_to_buffer(lex_state->buffer_state);
				GLOBAL(phplineno) = lex_state->lineno;
				stack_del_top(&GLOBAL(input_source_stack));
				break;
		}
	}
}


<IN_PHP,INITIAL,DOUBLE_QUOTES,BACKQUOTE,SINGLE_QUOTE>. {
	php3_error(E_WARNING,"Unexpected character in input:  '%c' (ASCII=%d) state=%d",yytext[0],yytext[0],YYSTATE);
}
