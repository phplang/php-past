/***[string.c]****************************************************[TAB=4]****\
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
/* $Id: string.c,v 1.30 1997/09/18 20:31:16 shane Exp $ */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "php.h"
#include "parse.h"

static char nullstr[1] = {'\0'};

void StrLen(void) {
	Stack *s;
	char temp[32];

	s = Pop();
	if(!s) {
		Error("Stack Error in strlen function");
		return;
	}
	sprintf(temp,"%d",(int)strlen(s->strval));
	Push(temp,LNUMBER);
}

void StrVal(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack Error in strval function");
		return;
	}
	Push(s->strval,STRING);
}

void GetType(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack Error in gettype function");
		return;
	}
	switch(s->type) {
	case LNUMBER:
		Push("integer",STRING);
		break;
	case DNUMBER:
		Push("double",STRING);
		break;
	case STRING:
		Push("string",STRING);
		break;
	}
}
	
void SetType(void) {
	Stack *s;
	char new_type[32];

	s = Pop();
	if(!s) {
		Error("Stack Error in settype function");
		return;
	}
	strcpy(new_type,s->strval);
	s = Pop();
	if(!s) {
		Error("Stack Error in settype function");
		return;
	}
	if(!s->var) {
		return;
	}
	if(!strcasecmp(new_type,"integer")) s->var->type = LNUMBER;
	else if(!strcasecmp(new_type,"double")) s->var->type = DNUMBER;
	else if(!strcasecmp(new_type,"string")) s->var->type = STRING;
}

void IntVal(void) {
	Stack *s;
	char temp[64];

	s = Pop();
	if(!s) {
		Error("Stack Error in intval function");
		return;
	}
	sprintf(temp,"%ld",s->intval);
	Push(temp,LNUMBER);
}

void DoubleVal(void) {
	Stack *s;
	char temp[128];

	s = Pop();
	if(!s) {
		Error("Stack Error in doubleval function");
		return;
	}
	sprintf(temp,"%.10f",s->douval);
	Push(temp,DNUMBER);
}

void StrTok(int init) {
	Stack *s;
	static char *str=NULL;
	static char *pos1=NULL;
	static char *pos2=NULL;
	char *temp=NULL;
	char *token=NULL;
	char *first=NULL;

	s = Pop();
	if(!s) {
		Error("Stack Error in strtok function");
		return;
	}
	if(s->type==STRING) {
		token = estrdup(1,s->strval);
	} else {
		temp = emalloc(1,sizeof(char) * 8);
		sprintf(temp,"%c",(int)s->intval);
		token = temp;
	}

	if(init) {
		if(str) str=NULL;
		s = Pop();
		if(!s) {
			Error("Stack Error in strtok function");
			return;
		}
		str = estrdup(0,s->strval);
		pos1=str;
		pos2=NULL;
	}
	if(pos1 && *pos1) {
		for (/* NOP */;token && *token;token++) {
			pos2 = strchr(pos1,(int) *token);
			if (!first || (pos2 && pos2 < first))
				first = pos2;
		} /* NB: token is unusable now */
		pos2 = first;
		if(pos2) {
			*pos2='\0';
		}	
		Push(pos1,STRING);
		if(pos2) pos1 = pos2+1;
		else pos1=NULL;
	} else {
		Push("",STRING);
	}
}

char *_strtoupper(char *s) {
	char *c;
	int ch;

	c = s;
	while(*c) {
		ch = toupper(*c);
		*c++ = ch;
	}
	return(s);
}
	
void StrToUpper(void) {
	Stack *s;
	
	s = Pop();
	if(!s) {
		Error("Stack Error in strtoupper function");
		return;
	}
	Push(_strtoupper(s->strval),STRING);
}

char *_strtolower(char *s) {
	register int ch;
	char *c;

	c = s;
	while(*c) {
		ch=tolower(*c);
		*c++ = ch;
	}
	return(s);
}

void StrToLower(void) {
	Stack *s;
	
	s = Pop();
	if(!s) {
		Error("Stack Error in strtolower function");
		return;
	}
	Push(_strtolower(s->strval),STRING);
}

void StrStr(void) {
	Stack *s;
	char *a, *b;
	char *c;

	s = Pop();
	if(!s) {
		Error("Stack Error in strstr function");
		return;
	}
	if(s->type == STRING) b = estrdup(1,s->strval);
	else { 
		b = emalloc(1,6);
		sprintf(b,"%c",(int)s->intval);
	}
		
	s = Pop();
	if(!s) {
		Error("Stack Error in strstr function");
		return;
	}
	a = estrdup(1,s->strval);

	c = strstr(a,b);

	if(c) Push(c,STRING);
	else Push(nullstr,STRING);
}	

void StrrChr(void) {
	Stack *s;
	char *a, *b;
	char *c;

	s = Pop();
	if(!s) {
		Error("Stack Error in strchr function");
		return;
	}
	if(s->type == STRING) b = estrdup(1,s->strval);
	else { 
		b = emalloc(1,6);
		sprintf(b,"%c",(int)s->intval);
	}

	s = Pop();
	if(!s) {
		Error("Stack Error in strchr function");
		return;
	}
	a = estrdup(1,s->strval);

	c = strrchr(a,*b);

	if(c) Push(c,STRING);
	else Push(nullstr,STRING);
}	

/* args s,m,n */
void SubStr(void) {
	Stack *s;
#if WINNT|WIN32
	unsigned int m,n;
#else
	int m,n;
#endif
	char *str;

	s = Pop();
	if(!s) {
		Error("Stack Error in substr function");
		return;
	}
	n = s->intval;

	s = Pop();
	if(!s) {
		Error("Stack Error in substr function");
		return;
	}
	m = s->intval;

	s = Pop();
	if(!s) {
		Error("Stack Error in substr function");
		return;
	}
	if(m>strlen(s->strval)) {
		Push("",STRING);
		return;
	}	
	str = (char *)estrdup(1,s->strval);
	if(m+n > strlen(str)) {
		Push(&str[m],STRING);
		return;
	}
	str[m+n]='\0';	
	Push(&str[m],STRING);
}

char *php_urlencode(char *s) {
	register int x,y;
	char *str;

	str = emalloc(1,3 * strlen(s) + 1); 
    for(x=0,y=0; s[x]; x++,y++) {
		str[y] = s[x];
		if(str[y]==' ') {
			str[y]='+';
		} else if((str[y] < '0' && str[y]!='-' && str[y]!='.' && str[y]!='/') ||
		   (str[y] < 'A' && str[y] >'9') ||
		   (str[y] > 'Z' && str[y] <'a' && str[y]!='_') ||
		   (str[y] > 'z')) {
            sprintf(&str[y],"%%%02x",(unsigned char)s[x]);
            y+=2;
        }
    }
    str[y] = '\0';
	return(str);
}

void UrlEncode(void) {
	char *str;
   	Stack *s;        
	
	s = Pop();
	if(!s) {
		Error("Stack Error in urlencode function");
		return;
	}
	if(!*s->strval) {
		Push("",STRING);
		return;
	}
	str = php_urlencode(s->strval);
	Push(str,STRING);
}

void UrlDecode(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack Error in urldecode function");
		return;
	}
	if(!*s->strval) {
		Push("",STRING);
		return;
	}
	parse_url(s->strval);
	Push(s->strval, STRING);	
}

void QuoteMeta(void) {
	register int x,y;
	char *str;
   	Stack *s;        
	
	s = Pop();
	if(!s) {
		Error("Stack Error in quotemeta function");
		return;
	}
	if(!*s->strval) {
		Push("",STRING);
		return;
	}
	str = emalloc(1,2 * strlen(s->strval) + 1); 
    for(x=0,y=0; s->strval[x]; x++,y++) {
		str[y] = s->strval[x];
		if(str[y]=='.' || str[y]=='\\' || str[y]=='+' ||
		   str[y]=='*' || str[y]=='?' || str[y]=='[' ||
		   str[y]=='^' || str[y]=='$' || str[y]=='(' || str[y]==')') {
            sprintf(&str[y],"\\%c",s->strval[x]);
            y+=1;
        }
    }
    str[y] = '\0';
	Push(str,STRING);
}

void Ord(void) {
	Stack *s;
	char temp[8];

	s = Pop();
	if(!s) {
		Error("Stack Error in ord function");
		return;
	}
	if(!*s->strval) {
		Push("-1",LNUMBER);
		return;
	}
	sprintf(temp,"%d",*s->strval);
	Push(temp,LNUMBER);
}

void UcFirst(void) {
	Stack *s;

	s = Pop();
	if(!s) {
		Error("Stack Error in ucfirst function");
		return;
	}
	if(!*s->strval) {
		Push("",STRING);
		return;
	}
	*s->strval = toupper(*s->strval);
	Push(s->strval,s->type);
}

void Sprintf(int argc) {
	Stack *s=NULL;
	Stack sarg[5]; /* Max 5 args to keep things simple in the parser */
	int num=0, done=0, type, concat_len=0;
	char *format,*t,*beg,*fmt,*buf,*concat;

	num = argc;
	while(num) {
		s = Pop();
		if(!s) {
			Error("Stack error in sprintf");
			return;
		}
		num--;
		memcpy(&(sarg[num]),s,sizeof(Stack));
		if(s->strval) sarg[num].strval = estrdup(1,s->strval);
	}
	s = Pop();
	if(!s) {
		Error("No format string");
		Push("", STRING);
		return;
	}
	format = estrdup(1,s->strval);	
	t = format;
	num=0;
	concat = (char *) "";  /* some dummy starting value */
	while(num<argc && !done) {
		type = FormatCheck(&t,&beg,&fmt);
		if(type==0 || type==-1) break;
		if(beg && *beg) {
			buf = emalloc(1,strlen(concat)+strlen(beg) + ECHO_BUF);
			strcpy(buf, concat);
			strcat(buf, beg); 
			concat = buf;
		}
		if(type==1) {
			buf = emalloc(1,strlen(concat)+strlen("%") + ECHO_BUF);
			strcpy(buf, concat);
			strcat(buf, "%");
			concat = buf;
			continue;
		}
		switch(type) {
			case LNUMBER:
                        ParseEscapes(fmt);
                        StripSlashes(fmt);
			concat_len = strlen(concat);
                        buf = emalloc(1,concat_len+strlen(fmt)+strlen(sarg[num].strval)+ECHO_BUF);
                        strcpy(buf, concat);
                        sprintf(&buf[concat_len],fmt,sarg[num].intval);
                        concat = buf;
                        num++;
                        break;
			case DNUMBER:
                        ParseEscapes(fmt);
                        StripSlashes(fmt);
			concat_len = strlen(concat);
                        buf = emalloc(1,concat_len+strlen(fmt)+strlen(sarg[num].strval)+ECHO_BUF);
                        strcpy(buf, concat);
                        sprintf(&buf[concat_len],fmt,sarg[num].douval);
                        concat = buf;
                        num++;
                        break;
			case STRING:
                        ParseEscapes(fmt);
                        StripSlashes(fmt);
                        ParseEscapes(sarg[num].strval);
                        StripSlashes(sarg[num].strval);
			concat_len = strlen(concat);
                        buf = emalloc(1,concat_len+strlen(fmt)+strlen(sarg[num].strval)+ECHO_BUF);
                        strcpy(buf, concat);
                        sprintf(&buf[concat_len],fmt,sarg[num].strval);
                        concat = buf;
                        num++;
                        break;
		}
	}
	if(t && *t) {
		buf = emalloc(1,strlen(concat) + strlen(t) + ECHO_BUF);
		strcpy(buf, concat);
		strcat(buf, t);
		concat = buf;
	}
	Push(concat, STRING);
}

void Chr(void) {
	Stack *s;
	char temp[8];

	s = Pop();
	if(!s) {
		Error("Stack Error in chr function");
		return;
	}
	if((s->intval < 0) || (s->intval > 255)){
		Push("-1",LNUMBER);
		return;
	}
	sprintf(temp,"%c",(char)s->intval);
	Push(temp,STRING);
}

void Chop(void) {
	Stack *s;
	char *ns,*p;

	s = Pop();
	if(!s) {
		Error("Stack error in Chop function");
		return;
	}
	ns = estrdup(1,s->strval);
	p = ns + strlen(ns) - 1;
	while(isspace(*p) && p>=ns) p--;
	*(p+1)='\0';	
	Push(ns,STRING);
}

char *_StrTr(char *string, char *str_from, char *str_to) { 
	char xlat[256];
#if WINNT|WIN32
	unsigned int i, len1, len2;
#else
	int i,len1,len2;
#endif
	len1=strlen(str_from);
	len2=strlen(str_to);

	if (len1>len2) {
		str_from[len2]='\0';
		len1=len2;
	}
   
	for(i=0;i<256;xlat[i]=i,i++);
	for(i=0;i<len1;xlat[(unsigned char)str_from[i]]=str_to[i],i++);
	for(i=0;i<strlen(string);string[i]=xlat[(unsigned char)string[i]],i++);

	return string;
}

void StrTr(void) { /* strtr(STRING,FROM,TO) */
	Stack *s;
	char *string;
	char *str_from;
	char *str_to;
	char temp[1] = { '\0' };
                
	s = Pop();
	if(!s) {
		Error("Stack error in strtr");
		return;
	}
	if(s->strval) str_to = (char *)estrdup(1,s->strval);
	else str_to = temp;
	s = Pop();
	if(!s) {
		Error("Stack error in strtr");
		return;
	}
	if(s->strval) str_from = (char *)estrdup(1,s->strval);
	else str_from = temp;
	s = Pop();
	if(!s) {  
		Error("Stack error in strtr");
		return;
	}
	if(s->strval) string = (char *)estrdup(1,s->strval);
	else string = temp;

	Push(_StrTr(string , str_from, str_to),STRING);
}
