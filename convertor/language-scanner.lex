%{

/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
*/


/* $Id: language-scanner.lex,v 1.12 1998/05/15 10:56:55 zeev Exp $ */

%}

%x IN_PHP
%x ENCAPS
%x SINGLE_QUOTE

%{

#include "token_cache.h"
#include "language-parser.tab.h"
#include "language-scanner.h"
#include "main.h"

#if WIN32|WINNT
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#endif

#define YY_DECL int lex_scan(pval *phplval)

#ifdef __cplusplus
#  define MY_INPUT yyinput
#else
#  define MY_INPUT input
#endif

void end_php()
{
	BEGIN(INITIAL);
}

%}

LNUM	[0-9]+
DNUM	[0-9]*[\.][0-9]+
HNUM	"0x"[0-9a-fA-F]+
LABEL	[a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*
WHITESPACE [ \n\r\t]+
TOKENS [;:,.\[\]()|^&+-/*=%!~$<>{}?@]
ENCAPSED_TOKENS [\[\]\\$]
ESCAPED_AND_WHITESPACE [\n\t\r #`'.:;,()|^&+-/*=%!~<>{}?@]+

%option noyylineno
%option noyywrap
%%

<IN_PHP>"exit" {
	return EXIT;
}

<IN_PHP>"die" {
	return EXIT;
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
	return PHPECHO;
}

<IN_PHP>"header" {
	return PHPHEADER;
}

<IN_PHP>"class" {
	return CLASS;
}

<IN_PHP>"extends" {
	return EXTENDS;
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

<IN_PHP>"_include" {
	return _INCLUDE;
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

<IN_PHP>{DNUM} {
	phplval->value.dval = atof(yytext);
	phplval->type = IS_DOUBLE;
	return DNUMBER;
}

<INITIAL>([^<]|"<"[^?]){1,400} {
	int i;
	
	phplval->value.strval = (char *) estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return INLINE_HTML;
}

<INITIAL>"<?" {
	BEGIN(IN_PHP);
	printf(yytext);
}

<IN_PHP>"?>" {
	return END_PHP3;
}

<IN_PHP>{LABEL} {
	phplval->value.strval = (char *)estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return STRING;
}

<ENCAPS>[^"\\\n] {
	phplval->value.strval = (char *)estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return STRING;
}	

<SINGLE_QUOTE>[^'\\\n] {
	phplval->value.strval = (char *)estrndup(yytext, yyleng);
	phplval->strlen = yyleng;
	phplval->type = IS_STRING;
	return STRING;
}


<IN_PHP>{WHITESPACE} {
	printf("%s",yytext);
}

<IN_PHP>"#"[^\n]*"\n" { /* eat one line comments */ printf("%s",yytext); }


<IN_PHP>"/*"([^*]|[*]+[^/])*"*/"([ ]*";")? {
	printf(yytext);
}

<IN_PHP>["] {
	BEGIN(ENCAPS);
	return '\"';
}

<IN_PHP>['] {
	BEGIN(SINGLE_QUOTE);
	return '\'';
}

<ENCAPS>"\\\"" {
	phplval->value.chval = '"';
	return CHAR;
}

<SINGLE_QUOTE>"\\\'" {
	phplval->value.chval = '\'';
	return CHAR;
}

<ENCAPS>"\\n"|"\\t"|"\\r"|"\\\\"|"\\$"|"\\" {
	phplval->strlen = yyleng;
	phplval->value.strval = estrndup(yytext,yyleng);
	phplval->type = IS_STRING;
	return STRING;
}

<ENCAPS>["] {
	BEGIN(IN_PHP);
	return '\"';
}

<SINGLE_QUOTE>['] {
	BEGIN(IN_PHP);
	return '\'';
}


<ENCAPS,SINGLE_QUOTE,INITIAL,IN_PHP><<EOF>> {
	yyterminate();
}



<IN_PHP,INITIAL,ENCAPS,SINGLE_QUOTE>. {
	/*php3_error(E_WARNING,"Unexpected character in input:  '%c' (ASCII=%d) state=%d",yytext[0],yytext[0],YYSTATE);*/
}
