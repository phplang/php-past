%{

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
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
*/

/* $Id: language-scanner.lex,v 1.187 2000/08/11 22:17:34 martin Exp $ */

%}

%x IN_PHP
%x DOUBLE_QUOTES
%x SINGLE_QUOTE
%x BACKQUOTE

%{

#if WIN32|WINNT
#include <winsock.h>
#endif

#include <errno.h>

#include "php.h"
#include "language-parser.tab.h"
#include "language-scanner.h"
#include "main.h"
#include "modules.h"
#include "control_structures.h"
#include "highlight.h"
#include "functions/post.h"
#include "functions/fsock.h"
#include "constants.h"

#define YY_DECL int lex_scan(pval *phplval)
#define ECHO { PHPWRITE( yytext, yyleng ); }

#ifdef __cplusplus
#  define MY_INPUT yyinput
#else
#  define MY_INPUT input
#endif

extern Stack input_source_stack;
int php3_display_source; /* whether or not we're in display-source mode */
int php3_preprocess;  /* whether we're in preprocess mode */
int phplineno;
static int in_eval;

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

#if HAVE_DISPLAY_SOURCE
#define IF_DISPLAY_SOURCE(a) \
	if (GLOBAL(php3_display_source)) { a }
#else
#define IF_DISPLAY_SOURCE(a) if(0) { }
#endif
	
#define HANDLE_NEWLINES(s,l) \
do { \
	char *p = (s),*boundary = p+(l); \
\
	if (GLOBAL(in_eval)) { \
		break; \
	} \
	while(p<boundary) { \
		if (*p++=='\n') { \
			GLOBAL(phplineno)++; \
		} \
	} \
} while(0)

#define HANDLE_NEWLINE(c) \
{ \
	if (c=='\n' && !GLOBAL(in_eval)) { \
		GLOBAL(phplineno)++; \
	} \
}
		
	
void reset_scanner()
{
	YY_TLS_VARS
	TLS_VARS;

	BEGIN(INITIAL);
	GLOBAL(phplineno)=1;
	GLOBAL(in_eval)=0;
}

/* Used to make sure that a buffer is allocated for yyin */
void initialize_input_file_buffer(FILE *f)
{
	yyrestart(f);
}


static void html_puts(char *s, uint len)
{
	register char *ptr=s, *end=s+len;
	int add_lineno=0;
	
	while (ptr<end) {
		html_putc(*ptr++);
	}
}


int include_file(pval *file, int display_source)
{ 
	FILE *tmp;
	YY_TLS_VARS
	YY_BUFFER_STATE buffer_state = YY_CURRENT_BUFFER;
	PHPLexState lex_state;
	char *filename;
	int issock=0, socketd=0;
	int oldcs;
	TLS_VARS;

	convert_to_string(file);

	if (file->type!=IS_STRING) {
		return FAILURE;
	}
	memcpy(&lex_state.buffer_state,&buffer_state,sizeof(YY_BUFFER_STATE));
	lex_state.type = (display_source?LEX_STATE_HIGHLIGHT_FILE:LEX_STATE_INCLUDE);
	lex_state.lineno = GLOBAL(phplineno);
	lex_state.state = YYSTATE;
	lex_state.in_eval = GLOBAL(in_eval);
	lex_state.in = yyin;
	GLOBAL(in_eval)=0;
	
	php3i_stack_push(&GLOBAL(input_source_stack),&lex_state,sizeof(PHPLexState));

	oldcs = _php3_sock_set_def_chunk_size(1);
	tmp = php3_fopen_wrapper(file->value.str.val, "r", USE_PATH|IGNORE_URL_WIN, &issock, &socketd);
	if(issock) {
		tmp = fdopen(socketd,"r");
	}
	_php3_sock_set_def_chunk_size(oldcs);
	if (!tmp) {
		php3_error(E_ERROR,"Failed opening required '%s'",
			    php3_strip_url_passwd(file->value.str.val));
		php3i_stack_del_top(&GLOBAL(input_source_stack));
		return FAILURE;
	}
	yyin = tmp;
	yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
	BEGIN(INITIAL);
	GLOBAL(include_count)++;
	GLOBAL(phplineno) = 1 + (GLOBAL(include_count)<<TOKEN_BITS);
	filename = estrndup(file->value.str.val,file->value.str.len);
	_php3_hash_index_update(&GLOBAL(include_names),GLOBAL(include_count),(void *) &filename,sizeof(char *),NULL);
	
	return SUCCESS;
}


void eval_string(pval *str,pval *return_offset, int display_source INLINE_TLS)
{
	PHPLexState lex_state;
	YY_TLS_VARS
	YY_BUFFER_STATE buffer_state = YY_CURRENT_BUFFER;

	convert_to_string(str);
	
	if (str->value.str.val[0]=='\0') { /* optimize empty eval's */
		return;
	}
	/* enforce two trailing NULLs for flex... */
	str->value.str.val = (char *) erealloc(str->value.str.val,str->value.str.len+2);
	str->value.str.val[str->value.str.len+1]=0;

	memcpy(&lex_state.buffer_state,&buffer_state,sizeof(YY_BUFFER_STATE));
	if (!display_source) {
		lex_state.type = LEX_STATE_EVAL;
		lex_state.return_offset = return_offset->offset+1;
	} else {
		lex_state.type = LEX_STATE_HIGHLIGHT_STRING;
	}
	lex_state.state = YYSTATE;
	lex_state.eval_string = str->value.str.val;
	lex_state.lineno = GLOBAL(phplineno);
	lex_state.in_eval = GLOBAL(in_eval);
	lex_state.in = yyin;
	GLOBAL(phplineno) = GLOBAL(current_lineno);
	GLOBAL(in_eval)=1;
	php3i_stack_push(&GLOBAL(input_source_stack),&lex_state,sizeof(PHPLexState));
	yyin=NULL;

	if (!display_source) {
		if (tcm_new(&GLOBAL(token_cache_manager),TOKEN_CACHE_EVAL_BLOCK_SIZE)==FAILURE) {
			php3_error(E_ERROR,"Unable to initialize a new token cache");
			return;
		}
	} else {
		if (display_source==2 && !pval_is_true(return_offset)) {
			display_source=1;
		}
		if (display_source==1) {
			BEGIN(INITIAL);
			start_display_source(0 _INLINE_TLS);
		} else {
			start_display_source(1 _INLINE_TLS);
		}
	}

	yy_scan_buffer(str->value.str.val, str->value.str.len+2);
}


int conditional_include_file(pval *file,pval *return_offset INLINE_TLS)
{
	PHPLexState lex_state;
	YY_TLS_VARS
	YY_BUFFER_STATE buffer_state = YY_CURRENT_BUFFER;
	FILE *tmp;
	char *filename;
	int issock=0, socketd=0;
	int oldcs;

	convert_to_string(file);

	memcpy(&lex_state.buffer_state,&buffer_state,sizeof(YY_BUFFER_STATE));
	lex_state.type = LEX_STATE_CONDITIONAL_INCLUDE;
	lex_state.state = YYSTATE;
	lex_state.return_offset = return_offset->offset+1;
	lex_state.lineno = GLOBAL(phplineno);
	lex_state.in_eval = GLOBAL(in_eval);
	lex_state.in = yyin;
	GLOBAL(in_eval) = 0;
	php3i_stack_push(&GLOBAL(input_source_stack),&lex_state,sizeof(PHPLexState));

	oldcs = _php3_sock_set_def_chunk_size(1);
	tmp = php3_fopen_wrapper(file->value.str.val, "r", USE_PATH|IGNORE_URL_WIN, &issock, &socketd);
	if(issock) {
		tmp = fdopen(socketd,"r");
	}
	_php3_sock_set_def_chunk_size(oldcs);
	if (!tmp) {
		php3_error(E_WARNING,"Failed opening '%s' for inclusion",
			    php3_strip_url_passwd(file->value.str.val));
		php3i_stack_del_top(&GLOBAL(input_source_stack));
		pval_destructor(file _INLINE_TLS);
		return FAILURE;
	}
	yyin = tmp;
	yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));
	BEGIN(INITIAL);
	GLOBAL(include_count)++;
	GLOBAL(phplineno) = 1 + (GLOBAL(include_count)<<TOKEN_BITS);
	filename = file->value.str.val;
	_php3_hash_index_update(&GLOBAL(include_names),GLOBAL(include_count),(void *) &filename,sizeof(char *),NULL);
	
	if (tcm_new(&GLOBAL(token_cache_manager),TOKEN_CACHE_INCLUDE_BLOCK_SIZE)==FAILURE) {
		php3_error(E_ERROR, "Unable to initialize a new token cache");
		return FAILURE;
	} else {
		return SUCCESS;
	}
}


int end_current_file_execution(int *retval)
{
	if (php3i_stack_is_empty(&GLOBAL(input_source_stack))) {
		*retval = 0;
		return 1;
	} else {
		PHPLexState *lex_state;

		yy_delete_buffer(YY_CURRENT_BUFFER);
		php3i_stack_top(&GLOBAL(input_source_stack),(void **) &lex_state);

		GLOBAL(phplineno) = lex_state->lineno;
		GLOBAL(in_eval) = lex_state->in_eval;
		
		switch(lex_state->type) {
			case LEX_STATE_CONDITIONAL_INCLUDE: /* switching out of include() */
				seek_token(&GLOBAL(token_cache_manager),lex_state->return_offset, NULL);
#if WIN32|WINNT
				if(yyin->_tmpfname=="url"){
					closesocket(yyin->_file);
					efree(yyin);
				} else {
					fclose(yyin);
				}
#else
				fclose(yyin);
#endif
				BEGIN(lex_state->state);
				yy_switch_to_buffer(lex_state->buffer_state);
				php3i_stack_del_top(&GLOBAL(input_source_stack));
				*retval = DONE_EVAL;
				return 1;
				break;
			case LEX_STATE_EVAL: /* switching out of eval() */
				seek_token(&GLOBAL(token_cache_manager),lex_state->return_offset, NULL);
				BEGIN(lex_state->state);
				yy_switch_to_buffer(lex_state->buffer_state);
				STR_FREE(lex_state->eval_string);
				php3i_stack_del_top(&GLOBAL(input_source_stack));
				*retval = DONE_EVAL;
				return 1;
				break;
			case LEX_STATE_HIGHLIGHT_STRING:
				GLOBAL(php3_display_source)=0;
				GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
				php3i_stack_del_top(&GLOBAL(css));
				GLOBAL(Execute) = SHOULD_EXECUTE;
				END_COLOR();
				BEGIN(lex_state->state);
				yy_switch_to_buffer(lex_state->buffer_state);
				STR_FREE(lex_state->eval_string);
				php3i_stack_del_top(&GLOBAL(input_source_stack));
				break;
			case LEX_STATE_HIGHLIGHT_FILE:
				GLOBAL(php3_display_source)=0;
				GLOBAL(ExecuteFlag) = php3i_stack_int_top(&GLOBAL(css));
				php3i_stack_del_top(&GLOBAL(css));
				GLOBAL(Execute) = SHOULD_EXECUTE;
				END_COLOR();
				/* break missing intentionally */
			case LEX_STATE_INCLUDE:
#if WIN32|WINNT
				if (yyin->_tmpfname=="url") {
					closesocket(yyin->_file);
					efree(yyin);
				} else {
					fclose(yyin);
				}
#else
				fclose(yyin);
#endif
				BEGIN(lex_state->state);
				yy_switch_to_buffer(lex_state->buffer_state);
				php3i_stack_del_top(&GLOBAL(input_source_stack));
				break;
		}
	}
	return 0;
}


void clean_input_source_stack(void)
{
	PHPLexState *lex_state;
	YY_TLS_VARS
	TLS_VARS;

	if (yyin) {
		fclose(yyin);
	}
	while (php3i_stack_top(&GLOBAL(input_source_stack), (void **) &lex_state) != FAILURE) {
		switch(lex_state->type) {
			case LEX_STATE_EVAL:
			case LEX_STATE_HIGHLIGHT_STRING:
				STR_FREE(lex_state->eval_string);
				break;
		}
		if (lex_state->in && lex_state->in!=yyin) {
			fclose(lex_state->in);
		}
		yy_delete_buffer(YY_CURRENT_BUFFER);
		yy_switch_to_buffer(lex_state->buffer_state);
		php3i_stack_del_top(&GLOBAL(input_source_stack));
	}
	php3i_stack_destroy(&GLOBAL(input_source_stack));
	GLOBAL(initialized) &= ~INIT_INCLUDE_STACK;
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
DNUM	([0-9]*[\.][0-9]+)|([0-9]+[\.][0-9]*)
EXPONENT_DNUM	(({LNUM}|{DNUM})[eE][+-]?{LNUM})
HNUM	"0x"[0-9a-fA-F]+
LABEL	[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*
WHITESPACE [ \n\r\t]+
TABS_AND_SPACES [ \t]*
TOKENS [-;:,.\[\]()|^&+/*=%!~$<>{}?@]
ENCAPSED_TOKENS [\[\]{}$]
ESCAPED_AND_WHITESPACE [-\n\t\r #'.:;,()|^&+/*=%!~<>?@]+

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

<IN_PHP>"const" {
	return PHP_CONST;
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

<IN_PHP>"endfor" {
	return ENDFOR;
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

<IN_PHP>"("{TABS_AND_SPACES}("int"|"integer"){TABS_AND_SPACES}")" {
	return INT_CAST;
}

<IN_PHP>"("{TABS_AND_SPACES}("real"|"double"|"float"){TABS_AND_SPACES}")" {
	return DOUBLE_CAST;
}

<IN_PHP>"("{TABS_AND_SPACES}"string"{TABS_AND_SPACES}")" {
	return STRING_CAST;
}

<IN_PHP>"("{TABS_AND_SPACES}"array"{TABS_AND_SPACES}")" {
	return ARRAY_CAST;
}

<IN_PHP>"("{TABS_AND_SPACES}"object"{TABS_AND_SPACES}")" {
	return OBJECT_CAST;
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

<IN_PHP>"highlight_file"|"show_source" {
	return HIGHLIGHT_FILE;
}

<IN_PHP>"highlight_string" {
	return HIGHLIGHT_STRING;
}

<IN_PHP>"global" {
	return PHP_GLOBAL;
}

<IN_PHP>"isset" {
	return PHP_ISSET;
}

<IN_PHP>"empty" {
	return PHP_EMPTY;
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

<IN_PHP>"<<=" {
	return SHIFT_LEFT_EQUAL;
}

<IN_PHP>">>=" {
	return SHIFT_RIGHT_EQUAL;
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

<IN_PHP>"XOR" {
	return LOGICAL_XOR;
}

<IN_PHP>"<<" {
	return SHIFT_LEFT;
}

<IN_PHP>">>" {
	return SHIFT_RIGHT;
}

<IN_PHP>{TOKENS} {
	return yytext[0];
}

<IN_PHP>{LNUM}|{HNUM} {
	errno = 0;
	phplval->value.lval = strtol(yytext, NULL, 0);
	if (errno == ERANGE) { /* overflow */
		phplval->value.dval = strtod(yytext,NULL);
		phplval->type = IS_DOUBLE;
		return DNUMBER;
	} else {
		phplval->type = IS_LONG;
		return LNUMBER;
	}
}

<DOUBLE_QUOTES,BACKQUOTE>{LNUM}|{HNUM} { /* treat numbers (almost) as strings inside encapsulated strings */
	phplval->value.str.val = (char *)estrndup(yytext, yyleng);
	phplval->value.str.len = yyleng;
	phplval->type = IS_STRING;
	return NUM_STRING;
}

<IN_PHP>{DNUM}|{EXPONENT_DNUM} {
	phplval->value.dval = strtod(yytext,NULL);
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
	
	phplval->value.str.len = strlen(filename);
	phplval->value.str.val = estrndup(filename,phplval->value.str.len);
	phplval->type = IS_STRING;
	return PHP_FILE;
}


<INITIAL>(([^<]|"<"[^?%s<]){1,400})|"<s"|"<" {
	phplval->value.str.val = (char *) estrndup(yytext, yyleng);
	phplval->value.str.len = yyleng;
	phplval->type = IS_STRING;
	HANDLE_NEWLINES(yytext,yyleng);
	return INLINE_HTML;
}

<INITIAL>"<?"|"<script"{WHITESPACE}+"language"{WHITESPACE}*"="{WHITESPACE}*("php"|"\"php\""|"\'php\'"){WHITESPACE}*">" {
	if (php3_ini.short_open_tag || yyleng>2) { /* yyleng>2 means it's not <? but <script> */
		if (!(GLOBAL(initialized) & INIT_ENVIRONMENT)) {
			_php3_hash_environment();
		}
		BEGIN(IN_PHP);
		IF_DISPLAY_SOURCE(
			BEGIN_COLOR(php3_ini.highlight_default);
			html_puts(yytext,yyleng);
		)
		HANDLE_NEWLINES(yytext,yyleng);
	} else {
		phplval->value.str.val = (char *) estrndup(yytext, yyleng);
		phplval->value.str.len = yyleng;
		phplval->type = IS_STRING;
		HANDLE_NEWLINES(yytext,yyleng);
		return INLINE_HTML;
	}
}

<INITIAL>"<%"("=")? {
	if (php3_ini.asp_tags) {
		if (!(GLOBAL(initialized) & INIT_ENVIRONMENT)) {
			_php3_hash_environment();
		}
		BEGIN(IN_PHP);
		IF_DISPLAY_SOURCE(
			BEGIN_COLOR(php3_ini.highlight_default);
			html_puts(yytext,yyleng);
		)
		if (yyleng==3) { /* this tag is <%=, implicit echo */
			return PHP_ECHO;
		}
	} else {
		phplval->value.str.val = (char *) estrndup(yytext, yyleng);
		phplval->value.str.len = yyleng;
		phplval->type = IS_STRING;
		HANDLE_NEWLINES(yytext,yyleng);
		return INLINE_HTML;
	}
}


<INITIAL>"<?php"[ \n\r\t] {
	HANDLE_NEWLINE(yytext[yyleng-1]);
	if (!(GLOBAL(initialized) & INIT_ENVIRONMENT)) {
		_php3_hash_environment();
	}
	BEGIN(IN_PHP);
	IF_DISPLAY_SOURCE(
		BEGIN_COLOR(php3_ini.highlight_default);
		html_puts(yytext,yyleng);
	)
}

<INITIAL>"<?php_track_vars?>"([\n]|"\r\n")? {
	GLOBAL(php3_track_vars)=1;
	HANDLE_NEWLINE(yytext[yyleng-1]);
	IF_DISPLAY_SOURCE(
		BEGIN_COLOR(php3_ini.highlight_default);
		html_puts(yytext,yyleng);
	)
}


<IN_PHP>{LABEL} {
	pval result;
	
	if (!last_token_suggests_variable_reference() && php3_get_constant(yytext, yyleng, &result)) {
		*phplval = result;
		switch (result.type) {
			case IS_LONG:
				return LNUMBER;
				break;
			case IS_DOUBLE:
				return DNUMBER;
				break;
			case IS_STRING:
				return STRING_CONSTANT;
				break;
		}
	} else {
		phplval->value.str.val = (char *)estrndup(yytext, yyleng);
		phplval->value.str.len = yyleng;
		phplval->type = IS_STRING;
		return STRING;
	}
}


<DOUBLE_QUOTES,BACKQUOTE>{LABEL} {
	phplval->value.str.val = (char *)estrndup(yytext, yyleng);
	phplval->value.str.len = yyleng;
	phplval->type = IS_STRING;
	return STRING;
}


<IN_PHP>{WHITESPACE} {
	HANDLE_NEWLINES(yytext,yyleng);
	IF_DISPLAY_SOURCE(
		html_puts(yytext, yyleng);
	)
}


<IN_PHP>([#]|"//")([^\n\r?]|"?"[^>\n\r])*("?\n"|"?\r\n")? { /* eat one line comments */
	HANDLE_NEWLINE(yytext[yyleng-1]);
	IF_DISPLAY_SOURCE(
		END_COLOR();
		BEGIN_ITALIC();
		BEGIN_COLOR_SIZE(php3_ini.highlight_comment, "-1");
		html_puts(yytext,yyleng);
		END_COLOR();
		END_ITALIC();
		BEGIN_COLOR(php3_ini.highlight_default);
	)
}

<IN_PHP>"/*" {
	/* Eat C-style comments */
	char c;
	int start_lineno;

	start_lineno=GLOBAL(phplineno);

	IF_DISPLAY_SOURCE(
		END_COLOR();
		BEGIN_ITALIC();
		BEGIN_COLOR_SIZE(php3_ini.highlight_comment, "-1");
		html_puts(yytext,yyleng);
	)
	for (;;) {
		IF_DISPLAY_SOURCE(	
			while((c=MY_INPUT())!='*' && c!=EOF) {
				HANDLE_NEWLINE(c);
				html_putc(c);
			}
		) else {
			while ((c=MY_INPUT())!='*' && c!=EOF) { /* eat up text of comment */
				HANDLE_NEWLINE(c);
			}
		}
		if (c=='*') {
			IF_DISPLAY_SOURCE(	
				html_putc(c);
				while ((c=MY_INPUT())=='*') {
					html_putc(c);
				}
			) else {
				while ((c=MY_INPUT())=='*');
			}
			IF_DISPLAY_SOURCE(	
				html_putc(c);
			)
			if (c=='/') {
				IF_DISPLAY_SOURCE(	
					END_COLOR(); END_ITALIC();
					BEGIN_COLOR(php3_ini.highlight_default);
				)
				break;	/* found the end */
			}
		}

		if (c==EOF) {
			php3_error(E_WARNING,"Unterminated comment starting line %d.\n",php3_get_lineno(start_lineno));
			break;
		} else {
			HANDLE_NEWLINE(c);
		}
	}
}

<IN_PHP>("?>"|"</script"{WHITESPACE}*">")([\n]|"\r\n")? {
	HANDLE_NEWLINE(yytext[yyleng-1]);
	BEGIN(INITIAL);
	IF_DISPLAY_SOURCE(
		html_puts(yytext,yyleng);
		END_COLOR();
	)
	return ';';  /* implicit ';' at php-end tag */
}


<IN_PHP>"%>"([\n]|"\r\n")? {
	HANDLE_NEWLINE(yytext[yyleng-1]);
	if (php3_ini.asp_tags) {
		BEGIN(INITIAL);
		IF_DISPLAY_SOURCE(
			html_puts(yytext,yyleng);
			END_COLOR();
		)
		return ';';  /* implicit ';' at php-end tag */
	} else {
		phplval->value.str.val = (char *) estrndup(yytext, yyleng);
		phplval->value.str.len = yyleng;
		phplval->type = IS_STRING;
		HANDLE_NEWLINES(yytext,yyleng);
		return INLINE_HTML;
	}
}


<IN_PHP>["] {
	BEGIN(DOUBLE_QUOTES);
	IF_DISPLAY_SOURCE(
		BEGIN_COLOR(php3_ini.highlight_string);
		BEGIN_STRONG();
		html_putc(yytext[0]);
	)
	return '\"';
}

<IN_PHP>[`] {
	BEGIN(BACKQUOTE);
	IF_DISPLAY_SOURCE(
		BEGIN_COLOR(php3_ini.highlight_string);
		BEGIN_STRONG();
		html_putc(yytext[0]);
	)
	return '`';
}


<IN_PHP>['] {
	BEGIN(SINGLE_QUOTE);
	IF_DISPLAY_SOURCE(
		BEGIN_COLOR(php3_ini.highlight_string);
		BEGIN_STRONG();
		html_putc(yytext[0]);
	)
	return '\'';
}


<DOUBLE_QUOTES,BACKQUOTE>{ESCAPED_AND_WHITESPACE} {
	HANDLE_NEWLINES(yytext,yyleng);
	phplval->value.str.val = (char *) estrndup(yytext, yyleng);
	phplval->value.str.len = yyleng;
	phplval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}

<SINGLE_QUOTE>([^'\\]|\\[^'\\])+ {
	HANDLE_NEWLINES(yytext,yyleng);
	phplval->value.str.val = (char *) estrndup(yytext, yyleng);
	phplval->value.str.len = yyleng;
	phplval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<DOUBLE_QUOTES>[`]+ {
	int i;

	phplval->value.str.val = (char *) estrndup(yytext, yyleng);
	phplval->value.str.len = yyleng;
	phplval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<BACKQUOTE>["]+ {
	int i;

	phplval->value.str.val = (char *) estrndup(yytext, yyleng);
	phplval->value.str.len = yyleng;
	phplval->type = IS_STRING;
	return ENCAPSED_AND_WHITESPACE;
}


<DOUBLE_QUOTES,BACKQUOTE>"$"[^a-zA-Z_\x7f-\xff{] {
	phplval->value.chval = yytext[0];
	if (yyleng == 2) {
		unput(yytext[1]);
		yytext[1] = 0;
		yyleng--;
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

<DOUBLE_QUOTES,BACKQUOTE>"\\"[0-7]{1,3} {
	phplval->value.chval = (char) strtol (yytext+1, NULL, 8);
	return CHARACTER;
}

<DOUBLE_QUOTES,BACKQUOTE>"\\x"[0-9A-Fa-f]{1,2} {
	phplval->value.chval = (char) strtol (yytext+2, NULL, 16);
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
			phplval->value.chval=yytext[1];
			break;
		default:
			phplval->value.str.val = estrndup(yytext,yyleng);
			phplval->value.str.len = yyleng;
			phplval->type = IS_STRING;
			return BAD_CHARACTER;
			break;
	}
	return CHARACTER;
}

<DOUBLE_QUOTES>["] {
	BEGIN(IN_PHP);
	IF_DISPLAY_SOURCE(
		html_putc(yytext[0]);
		END_STRONG(); END_COLOR();
	)
	return '\"';
}


<BACKQUOTE>[`] {
	BEGIN(IN_PHP);
	IF_DISPLAY_SOURCE(
		html_putc(yytext[0]);
		END_STRONG(); END_COLOR();
	)
	return '`';
}


<SINGLE_QUOTE>['] {
	BEGIN(IN_PHP);
	IF_DISPLAY_SOURCE(
		html_putc(yytext[0]);
		END_STRONG(); END_COLOR();
	)
	return '\'';
}


<DOUBLE_QUOTES,BACKQUOTE,INITIAL,IN_PHP><<EOF>> {
	int retval;

	if (end_current_file_execution(&retval)) {
		return retval;
	}
}




<IN_PHP,INITIAL,DOUBLE_QUOTES,BACKQUOTE,SINGLE_QUOTE>. {
	php3_error(E_WARNING,"Unexpected character in input:  '%c' (ASCII=%d) state=%d",yytext[0],yytext[0],YYSTATE);
}
