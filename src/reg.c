/***[reg.c]*******************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996 Rasmus Lerdorf                                         *
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
/* $Id: reg.c,v 1.11 1996/07/18 18:20:28 rasmus Exp $ */
#include <stdlib.h>
#include "regexpr.h"
#include "php.h"
#include "parse.h"

/*
 * reg_match(regular_expression, argument_string)
 *
 * Pushes non-zero if regular expression was met and zero otherwise
 *
 * This only matches the regular epxression against the start of the 
 * argument string.
 */
void RegMatch(char *reg_name) {
	Stack *s;	
	struct re_pattern_buffer exp;
	regexp_registers_t regs;
	char *string;
	char temp[1] = { '\0' };
	char temp2[16];
	char *temp3=NULL;
	char fastmap[256];
	char *cp, *buf=NULL;
	int ret, start, end, i, l;
 
	exp.allocated = 0;
	exp.buffer = 0;
	exp.translate = NULL;
	exp.fastmap = fastmap;

	s = Pop();
	if(!s) {
		Error("Stack error in reg_match");
		return;
	}
	if(s->strval) string = (char *)estrdup(1,s->strval);
	else string = temp;

	s = Pop();
	if(!s) {
		Error("Stack error in reg_match");
		return;
	}
	if(s->type==STRING)
		cp = php_re_compile_pattern(s->strval,strlen(s->strval),&exp);
	else {
		temp3 = emalloc(1,sizeof(char)*2);
		sprintf(temp3,"%c",(int)s->intval);
		cp = php_re_compile_pattern(temp3,strlen(temp3),&exp);
	}
		
	if(cp) {
		Error("reg_match error: %s",cp);
		Push("0",LNUMBER);
		return;	
	}	
	php_re_compile_fastmap(&exp);
	regs = emalloc(1,sizeof(struct re_registers));

	ret = php_re_match(&exp,string,strlen(string),0,regs);
	if(reg_name) {
		l = strlen(string)+1;
		buf = emalloc(1,l);
		for(i=0;i<10;i++) {
			sprintf(temp2,"%d",i);
			Push(temp2,STRING);
			*buf='\0';
			start = regs->start[i];
			end = regs->end[i];
#if DEBUG
			Debug("string=[%s], i=%d, start=%d, end=%d\n",string,i,start,end);
#endif
			if(start!=-1 && end>0 && start<l && end<l && start<end) {
				strncat(buf,&string[start],end-start);
				Push(buf,STRING);
			} else {
				Push("",STRING);
			}
			SetVar(reg_name,2,0); 
		}
	}
	if(ret<0) {
		Push("0",LNUMBER);
	} else {
		sprintf(temp2,"%d",ret);
		Push(temp2,LNUMBER);
	}
}

/*
 * reg_search(regular_expression, argument_string)
 * 
 * Searches the argument string for any matches to the regular expression
 * Pushes the part of the argument string that first matches the expression
 * If no match, a zero-length string is pushed
 */
void RegSearch(char *reg_name) {
	Stack *s;	
	struct re_pattern_buffer exp;
	regexp_registers_t regs;
	char *string;
	char temp[1] = { '\0' };
	char temp2[16];
	char fastmap[256];
	char *cp, *buf;
	int ret, start, end, i;
 
	exp.allocated = 0;
	exp.buffer = 0;
	exp.translate = NULL;
	exp.fastmap = fastmap;

	s = Pop();
	if(!s) {
		Error("Stack error in reg_match");
		return;
	}
	if(s->strval) string = (char *)estrdup(1,s->strval);
	else string = temp;

	s = Pop();
	if(!s) {
		Error("Stack error in reg_match");
		return;
	}
	cp = php_re_compile_pattern(s->strval,strlen(s->strval),&exp);
	if(cp) {
		Error("reg_match error: %s",cp);
		Push("0",LNUMBER);
		return;	
	}	
	php_re_compile_fastmap(&exp);
	regs = emalloc(1,sizeof(struct re_registers));
	ret = php_re_search(&exp,string,strlen(string),0,strlen(string),regs);
	if(ret<0) {
		Push("",STRING);
	} else {
		buf = emalloc(1,strlen(string)+1);
		if(reg_name) {
			for(i=0;i<10;i++) {
				sprintf(temp2,"%d",i);
				Push(temp2,STRING);
				*buf='\0';
				start = regs->start[i];
				end = regs->end[i];
#if DEBUG
				Debug("string=[%s], i=%d, start=%d, end=%d\n",string,i,start,end);
#endif
				if(start!=-1) {
					strncat(buf,&string[start],end-start);
					Push(buf,STRING);
				} else {
					Push("",STRING);
				}
				SetVar(reg_name,2,0); 
			}
		}
		strcpy(buf,&string[ret]);
		Push(buf,STRING);
	}
}

/*
 * reg_replace(regular_expression,replace_with,argument_string)
 *
 * This searches the argument string for matches to the regular expression
 * and substitutes the replacement string in for the matched part of the
 * argument string.
 *
 * The new string with the substitutions performed is pushed.  If no substitutions
 * were made, the original argument string is returned.
 */
void RegReplace(void) {
	Stack *s;	
	char *pattern;
	char *string;
	char *replace;
	char temp[1] = { '\0' };
	char *ret;

	s = Pop();
	if(!s) {
		Error("Stack error in reg_match");
		return;
	}
	if(s->strval) string = (char *)estrdup(1,s->strval);
	else string = temp;
	s = Pop();
	if(!s) {
		Error("Stack error in reg_match");
		return;
	}
	if(s->type==STRING)
		if(s->strval) replace = (char *)estrdup(1,s->strval);
		else replace = temp;
	else {
		if(s->strval) {
			replace = emalloc(1,2*sizeof(char));
			sprintf(replace,"%c",(int)s->intval);
		} else replace = temp;
	}
	
	s = Pop();
	if(!s) {
		Error("Stack error in reg_match");
		return;
	}
	if(s->type==STRING) {
		if(s->strval) pattern = (char *)estrdup(1,s->strval);
		else pattern = temp;
	} else {
		pattern = emalloc(1,2*sizeof(char));
		sprintf(pattern,"%c",(int)s->intval);
	}
	ret = _RegReplace(pattern, replace, string);
	if(ret==(char *)-1) {	
		Push("0",LNUMBER);
		return;	
	}
	Push(ret,STRING);
}

char *_RegReplace(char *pattern, char *replace, char *string) {
	struct re_pattern_buffer exp;
	struct re_registers regs;
	char *buf, *nbuf;
	char fastmap[256];
	char *cp;
	char o;
	int i,l,ll,new_l,allo,ret;

	exp.allocated = 0;
	exp.buffer = 0;
	exp.translate = NULL;
	exp.fastmap = fastmap;

	l = strlen(string);
	if(!l) return(string);
	cp = php_re_compile_pattern(pattern,strlen(pattern),&exp);
	if(cp) {
		Error("reg_match error: %s",cp);
		return((char *)-1);
	}	
	php_re_compile_fastmap(&exp);
	buf = emalloc(1,l*2*sizeof(char)+1);
	if(!buf) {
		Error("Unable to allocate memory in _RegReplace");
		return((char *)-1);
	}
	ret = 0;
	i = 0;
	allo = 2*l+1;
	buf[0] = '\0';	
	ll = strlen(replace);
	while(ret>=0) {
		ret = php_re_search(&exp,string,l,i,l-i,&regs);
		if(ret>=0) {
			o = string[regs.start[0]];
			string[regs.start[0]]='\0';
			new_l = strlen(buf)+strlen(replace)+strlen(&string[i]);
			if(new_l > allo) {
				nbuf = emalloc(1,1+allo+2*new_l);
				allo = 1+allo + 2*new_l;
				strcpy(nbuf,buf);
				buf=nbuf;
			}	
			strcat(buf,&string[i]);
			strcat(buf,replace);

			string[regs.start[0]] = o;
			i = regs.end[0];
#if DEBUG
			Debug("Match buf=[%s]\n",buf);
#endif
		} else {
			new_l = strlen(buf)+strlen(&string[i]);
			if(new_l > allo) {
				nbuf = emalloc(1,1+allo+2*new_l);
				allo = 1+allo + 2*new_l;
				strcpy(nbuf,buf);
				buf=nbuf;
			}	
			strcat(buf,&string[i]);
		}	
		if(regs.start[0]==0 && regs.end[0]==0) break;
	}
	return(buf);
}
