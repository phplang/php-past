/***[echo.c]******************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996,1997 Rasmus Lerdorf                                    *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License as published by      *
*  the Free Software Foundation; either version 2 of the License, or         *
*  (at your option) any later version.                                       *
*                                                                            *
*  This program is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU General Public License for more details.                              *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with this program; if not, write to the Free Software               *
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
*                                                                            *
\****************************************************************************/
/* $Id: echo.c,v 1.40 1997/06/12 04:46:26 rasmus Exp $ */
#include <stdlib.h>
#include "php.h"
#include "parse.h"
#if APACHE
#include "http_protocol.h"
#endif
#include <stdarg.h>

void Echo(char *format, int args) {
	Stack *s=NULL;
	Stack sarg[5]; /* Max 5 args to keep things simple in the parser */
#if APACHE
	char *buf;
#endif
	int num=args, done=0, type;
	char *t,*st,*beg,*fmt;

	php_header(0,NULL);
	while(num) {	
		s = Pop();
		if(!s) {
			return;
		}
		num--;
		memcpy(&(sarg[num]),s,sizeof(Stack));
		if(s->strval) sarg[num].strval = estrdup(1,s->strval);
	}
	if(!format) {
		for(num=0; num<args; num++) {
			if(sarg[num].strval) {
				ParseEscapes(sarg[num].strval);
				StripSlashes(sarg[num].strval);
				if(PUTS(sarg[num].strval) < 0) Exit(0);
			}
		}
		return;
	}

        /* if format contains $var, we must expand it */
	if (strchr(format, VAR_INIT_CHAR))
	{
		Stack * var;

		Push(format, STRING);
		var = Pop();
		if (var && var->strval)  format = estrdup(1, var->strval);
	}
	ParseEscapes(format);
	StripSlashes(format);
	t = format;
	num=0;
	while(num<args && !done) {	
		st = t;
		type = FormatCheck(&t,&beg,&fmt);	
		if(type==0 || type==-1) break;
		if(beg && *beg) {
			if(PUTS(beg)<0) { Exit(0); break; }
		}
		if(type==1) {
			if(PUTS("%")<0) { Exit(0); break; }
			continue;
		}
		switch(type) {
		case LNUMBER:
			ParseEscapes(fmt);
			StripSlashes(fmt);
#if APACHE
			buf = emalloc(1,strlen(fmt)+strlen(sarg[num].strval)+ECHO_BUF);
			sprintf(buf,fmt,sarg[num].intval);
			if(PUTS(buf)<0) { Exit(0); done=1; }
#else
			if(printf(fmt,sarg[num].intval)<0) { Exit(0); done=1; }
#endif
			num++;
			break;
		case DNUMBER:
			ParseEscapes(fmt);
			StripSlashes(fmt);
#if APACHE
			buf = emalloc(1,strlen(fmt)+strlen(sarg[num].strval)+ECHO_BUF);
			sprintf(buf,fmt,sarg[num].douval);
			if(PUTS(buf)<0) { Exit(0); done=1; }
#else
			if(printf(fmt,sarg[num].douval) < 0) { Exit(0); done=1; }
#endif
			num++;
			break;
		case STRING:
			ParseEscapes(fmt);
			StripSlashes(fmt);
			ParseEscapes(sarg[num].strval);
			StripSlashes(sarg[num].strval);
#if APACHE
			buf = emalloc(1,strlen(fmt)+strlen(sarg[num].strval)+ECHO_BUF);
			sprintf(buf,fmt,sarg[num].strval);
			if(PUTS(buf)<0) Exit(0);
#else
			if(printf(fmt,sarg[num].strval) < 0) Exit(0);
#endif
			num++;
			break;
		}
	}
	if(t && *t) {
			if(PUTS(t)<0) Exit(0);
	}
}

/* 
 * Originally from Perl 5, and copyright'd to Larry Wall, but I've rewritten
 * so much now that, aside from the quote below and a few odd
 * ideosyncrasies, it is entirely unrecognizable.
 *
 * Please note that this is not an example of my finest coding abilities. :( 
 * Tim Vanderhoek (ac199@freenet.hamilton.on.ca)
 *
 * Ripped the guts out of this one and turned it into a simple format string
 * checker as opposed to the more ambitious general-purpose string format
 * printer that Tim wrote. -Rasmus
 */

/* 
 * FormatCheck - Scan through format string until first formatter is
 * found and return the type, or -1 on an error.  If no type-checking
 * needs to be done it returns 1, or if no formatting sequences were found, 
 * it returns 0.
 * It also advances the format pointer to one past the found formatting
 * sequence and returns the skipped characters along with the formatting
 * sequence itself in *beg and *fmt respectively.
 */
int FormatCheck(char **format, char **beg, char **fmt) {
	char *t, *f;
	char dolong; /* quad in % command */
	char fieldpre; /* field or precision specifier in % command */
	char pfflg; /* any flag in % command */
	char nonj; /* flag other than '-' in % command */
	int done;
	int skipped, fmtcnt=1;

	t = *format;
	/* point t further along the string until it finds a '%' */
	for(skipped=0; *t != '\0' && *t != '%'; t++, skipped++);
	if(beg) {
		*beg = emalloc(1,skipped+1);
		if(skipped) strncpy(*beg, *format, skipped);
		(*beg)[skipped]='\0';
	}
	if(fmt) *fmt = NULL;
	if (*t == '\0') {
		return(0);
	}
	/* f is the whole escape sequence, t points to the char we are
 	 * currently evaluating. */
	f = t; done = 0; dolong = 0; fieldpre = 0; pfflg = 0; nonj = 0;
	for (t++; !done; t++, fmtcnt++) {
		switch (*t) {
		case '\0':
			Error("Unterminated escape sequence");
			done=-1;
			break;

			/* all the conversions (signify end of % command) */
		case 'd': case 'i': case 'o' : case 'u' : case 'x' : case 'X':
			/* the ones that take ints (unless modified) */
			done = LNUMBER;
			break;

		case 'f': case 'e': case 'E' : case 'g' : case 'G' :
			/* the ones that take floating points */
			if (dolong) {
				Error ("Type modifier long in conjuction with floating point conversion not supported in echo format string");
				done=-1;
			} else {
				done=DNUMBER;
			}
			break;

		case 'c' :
			if (nonj || dolong) {
				Error ("Unsupported flag or modifier in %%c in echo format string");
				done=-1;
			} else {
				done=LNUMBER;
			}
			break;

		case 's' :
			if (nonj || dolong) {
				Error ("Unsupported flag or modifier in %%s in echo format string");
				done=-1;
			} else {
				done=STRING;
			}
			break;

		case 'm': case 'p':
			/* print the string corresponding to errno.  GNU extension */
			Error ("%%%c unsupported in echo format string", *t);
			done=-1;
			break;

		case '%':
			/* print a literal '%' */
			if (dolong || fieldpre || pfflg) {
				Error("Unsupported echo format string");
				done=-1;
			} else {
				done=1;
			}
			break;

		case '-': /* flags in the % command */
			pfflg = 1;
			break;

		/* Would be nice to outlaw these when used with
		 * printf's not supporting them. */
		case '+' : case ' ' : case '#' : case '\'' :
			pfflg = 1; nonj = 1;
			break;

		case '0' :
			/* note that the zero can be ambigious */
			if (!fieldpre) { /* a zero after fieldpre is not a flag */
				pfflg = 1; /* flag denoting printf format string flags */
				nonj = 1; /* flag denoting non left-justification printf format string */
			}
			break;

		/* type modifiers in % format command */
		case 'l':
			if (dolong) { /* two 'l's, specifying a quad */
				Error("%ll not supported in echo format string");
				done = -1;
			} else {
				dolong = 1; /* flag denoting 'l' */
			}
			break;

			/* field length and precision */
		case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9': case '.':
			fieldpre = 1; /* flag denoting use of above */
			break;

		case 'h': case 'L': case 'q': case 'Z': default:
			/* unrecognized printf format directions */
			Error ("%c not supported in echo format string", *t);
			done = -1;
			break;
		}
	}
	*format=t;
	if(*t) format++;
	if(fmt) {
		*fmt = emalloc(1,fmtcnt+1);
		strncpy(*fmt, f, fmtcnt);
		(*fmt)[fmtcnt] = '\0';
	}
	return(done);
}

void _StripSlashes(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in StripSlashes()\n");
		return;
	}
	StripSlashes(s->strval);
	Push(s->strval,STRING);
}

void StripSlashes(char *string) {
	char *s,*t;
	int l;

	l = strlen(string); 
	s = string;
	t = string;
	while(*t && l>0) {
		if(*t=='\\') {
			t++;
			*s++=*t++;
			l-=2;
		} else {
			if(s!=t) *s++=*t++;
			else { s++; t++; }
			l--;
		}
	}
	if(s!=t) *s='\0';
}

void StripDollarSlashes(char *string) {
	char *s,*t;
	int l;

	l = strlen(string); 
	s = string;
	t = string;
	while(*t && l>0) {
		if(*t=='\\' && *(t+1)=='$') {
			t++;
			*s++=*t++;
			l-=2;
		} else if(*t=='\\' && *(t+1)=='\\') {
			if(s!=t) *s++=*t++;
			else { s++; t++; }
			l--;
			if(s!=t) *s++=*t++;
			else { s++; t++; }
			l--;
		} else {
			if(s!=t) *s++=*t++;
			else { s++; t++; }
			l--;
		}
	}
	if(s!=t) *s='\0';
}

void _AddSlashes(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack error in AddSlashes()\n");
		return;
	}
	Push(AddSlashes(s->strval,0),STRING);
}

/* 
 * If freeit is non-zero, then this function is allowed to free the
 * argument string.  If zero, it cannot free it.
 */
char *AddSlashes(char *string, int freeit) {
	static char *temp=NULL;

	if(strchr(string,'\\')) {
		temp = _RegReplace("\\\\","\\\\",string);
		if(freeit) {
			if(temp!=string) string=temp;
		} else {
			if(temp!=string) strcpy(string,temp);
		}
	}
	if(strchr(string,'$')) {
		temp = _RegReplace("\\$","\\$",string);
		if(freeit) {
			if(temp!=string) string=temp;
		} else {
			if(temp!=string) strcpy(string,temp);
		}
	}
#if MAGIC_QUOTES
	if(strchr(string,'\'')) {
		temp = _RegReplace("'","\\'",string);
		if(freeit) {
			if(temp!=string) string=temp;
		} else {
			if(temp!=string) strcpy(string,temp);
		}
	}
#if HAVE_LIBPQ
	if(strchr(string,'\"')) {
		temp = _RegReplace("\"","\\\"",string);
		if(freeit) {
			if(temp!=string) string=temp;
		} else {
			if(temp!=string) strcpy(string,temp);
		}
	}
#endif
#endif
	return(string);
}

void ParseEscapes(char *string) {
	char *s,*t, *r;
	char sv;
	int l,i;

	l = strlen(string); 
	s = string;
	t = string;
	while(*t && l>0) {
		if (*t=='\\' && *(t+1)=='\\') {
			t+=2; 
			*s++='\\';
			*s++='\\';
			l-=2;
		} else if(*t=='\\' && *(t+1)=='n') {
			t+=2;
			*s++='\n';
			l-=2;
		} else if(*t=='\\' && *(t+1)=='t') {
			t+=2;
			*s++='\t';
			l-=2;
		} else if(*t=='\\' && *(t+1)=='r') {
			t+=2;
			*s++='\r';
			l-=2;
		} else if(*t=='\\' && *(t+1)=='a') {
			t+=2;
			*s++=7;
			l-=2;
		} else if(*t=='\\' && *(t+1)=='b') {
			t+=2;
			*s++=8;
			l-=2;
		} else if(*t=='\\' && *(t+1)>='0' && *(t+1)<='7') {
			r=t+1;
			i=0;
			while(*r >='0' && *r <='7' && i<3) {
				i++;
				r++;
			}
			sv = *r;
			*r='\0';	
			*s++=_OctDec(t+1);
			t+=1+i;
			*r = sv;	
			l-=2;
		} else if(*t=='\\' && (*(t+1)=='x' || *(t+1)=='X')) {
			r=t+2;
			i=0;
			while(((*r >='0' && *r <='9') || (*r >='a' && *r <='f') || (*r >='A' && *r <='F')) && i<2) {
				i++;
				r++;
			}
			sv = *r;
			*r='\0';	
			*s++=_HexDec(t+1);
			t+=2+i;
			*r = sv;	
			l-=2;
		} else if(*t=='\\') {
			*s++=*(t+1);
			t+=2;
			l-=2;
		} else {
			if(s!=t) *s++=*t++;
			else { s++; t++; }
			l--;
		}
	}
	if(s!=t) *s='\0';
}

typedef struct php_extra_ents {
	int code;
	char str[6];
} php_extra_ents;

void HtmlSpecialChars(void) {
	Stack *s;
	int i;
	char lookfor[2];
	char replace[10];
	char *new, *work;
	char EntTable[][7] = {"nbsp","iexcl","cent","pound","curren","yen","brvbar",
	                      "sect","uml","copy","ordf","laquo","not","shy","reg",
	                      "macr","deg","plusmn","sup2","sup3","acute","micro",
	                      "para","middot","cedil","sup1","ordm","raquo","frac14",
	                      "frac12","frac34","iquest","Agrave","Aacute","Acirc",
	                      "Atilde","Auml","Aring","AElig","Ccedil","Egrave",
	                      "Eacute","Ecirc","Euml","Igrave","Iacute","Icirc",
	                      "Iuml","ETH","Ntilde","Ograve","Oacute","Ocirc","Otilde",
						  "Ouml","times","Oslash","Ugrave","Uacute","Ucirc","Uuml",
	                      "Yacute","THORN","szlig","agrave","aacute","acirc",
	                      "atilde","auml","aring","aelig","ccedil","egrave",
	                      "eacute","ecirc","euml","igrave","iacute","icirc",
	                      "iuml","eth","ntilde","ograve","oacute","ocirc","otilde",
	                      "ouml","divide","oslash","ugrave","uacute","ucirc",
	                      "uuml","yacute","thorn","yuml"};
	php_extra_ents ExtraEnts[] = { { 38, "amp" },  /* this one needs to be first */
								   { 34, "quot" },
								   { 60, "lt" },
								   { 62, "gt" },
								   { 0, "" } };
	s = Pop();
	if(!s) {
		Error("stack error in HtmlSpecialChars()");
		return;
	}
	lookfor[1]='\0';
	work = estrdup(1,s->strval);
	i=0;
	while(ExtraEnts[i].code) {
		if(strchr(work,(char)ExtraEnts[i].code)) {
			lookfor[0] = (char)ExtraEnts[i].code;
			sprintf(replace,"&%s;",ExtraEnts[i].str);
			new = _RegReplace(lookfor,replace,work);
			if(new!=work) work = new;
		}
		i++;
	}
	for(i=160;i<256;i++) {
		if(strchr(work,(char)i)) {
			lookfor[0] = (char)i;
			sprintf(replace,"&%s;",EntTable[i-160]);
			new = _RegReplace(lookfor,replace,work);
			if(new!=work) work = new;
		}
	}
	Push(work,STRING);
}
