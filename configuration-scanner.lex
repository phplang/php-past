%{

#include "parser.h"
#include "configuration-parser.tab.h"

#define YY_DECL cfglex(YYSTYPE *cfglval)


void init_cfg_scanner()
{
	cfglineno=0;
}


%}

%option noyywrap
%option yylineno

%%

<INITIAL>"true"|"on"|"yes" {
	cfglval->value.strval = php3_strndup("1",1);
	cfglval->strlen = 1;
	cfglval->type = IS_STRING;
	return TRUE;
}


<INITIAL>"false"|"off"|"no" {
	cfglval->value.strval = php3_strndup("",0);
	cfglval->strlen = 0;
	cfglval->type = IS_STRING;
	return FALSE;
}


<INITIAL>[[][^[]+[\]]([\n]?|"\r\n"?) {
	/* SECTION */

	/* eat trailng ] */
	while (yyleng>0 && (yytext[yyleng-1]=='\n' || yytext[yyleng-1]=='\r' || yytext[yyleng-1]==']')) {
		yyleng--;
		yytext[yyleng]=0;
	}

	/* eat leading [ */
	yytext++;
	yyleng--;

	cfglval->value.strval = php3_strndup(yytext,yyleng);
	cfglval->strlen = yyleng;
	cfglval->type = IS_STRING;
	return SECTION;
}


<INITIAL>"extension" {
	return EXTENSION;
}


<INITIAL>[ \t]*["][^\n\r"]*["][ \t]* {
	/* ENCAPSULATED STRING */
	register int i;

	/* eat trailing whitespace */
	for (i=yyleng-1; i>=0; i--) {
		if (yytext[i]==' ' || yytext[i]=='\t') {
			yytext[i]=0;
			yyleng--;
		} else {
			break;
		}
	}
	/* eat leading whitespace */
	while (yytext[0]) {
		if (yytext[0]==' ' || yytext[0]=='\t') {
			yytext++;
			yyleng--;
		} else {
			break;
		}
	}
	/* eat trailing " */
	yytext[yyleng-1]=0;
	
	/* eat leading " */
	yytext++;

	cfglval->value.strval = php3_strndup(yytext,yyleng);
	cfglval->strlen = yyleng;
	cfglval->type = IS_STRING;
	return ENCAPSULATED_STRING;
}


<INITIAL>[^=\n\r;"]+ {
	/* STRING */
	register int i;

	/* eat trailing whitespace */
	for (i=yyleng-1; i>=0; i--) {
		if (yytext[i]==' ' || yytext[i]=='\t') {
			yytext[i]=0;
			yyleng--;
		} else {
			break;
		}
	}
	/* eat leading whitespace */
	while (yytext[0]) {
		if (yytext[0]==' ' || yytext[0]=='\t') {
			yytext++;
			yyleng--;
		} else {
			break;
		}
	}
	if (yyleng!=0) {
		cfglval->value.strval = php3_strndup(yytext,yyleng);
		cfglval->strlen = yyleng;
		cfglval->type = IS_STRING;
		return STRING;
	} else {
		/* whitespace */
	}
}


<INITIAL>[ \t]*["][^\n\r"]*["][ \t] {
	/* ENCAPSULATED STRING */
	register int i;

	/* eat trailing whitespace */
	for (i=yyleng-1; i>=0; i--) {
		if (yytext[i]==' ' || yytext[i]=='\t') {
			yytext[i]=0;
			yyleng--;
		} else {
			break;
		}
	}
	/* eat leading whitespace */
	while (yytext[0]) {
		if (yytext[0]==' ' || yytext[i]=='\t') {
			yytext++;
			yyleng--;
		} else {
			break;
		}
	}
	cfglval->value.strval = php3_strndup(yytext,yyleng);
	cfglval->strlen = yyleng;
	cfglval->type = IS_STRING;
	return ENCAPSULATED_STRING;
}

<INITIAL>[=\n] {
	return yytext[0];
}

<INITIAL>"\r\n" {
	return '\n';
}

<INITIAL>[;][^\r\n]*[\r\n]? {
	/* comment */
	return '\n';
}

<INITIAL>. {
#if DEBUG
	php3_error(E_NOTICE,"Unexpected character on line %d:  '%s'\n",yylineno,yytext);
#endif
}
