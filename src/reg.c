/***[reg.c]*******************************************************[TAB=4]****\
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
/* $Id: reg.c,v 1.23 1997/05/15 14:16:07 rasmus Exp $ */
#include <stdlib.h>
#include "php.h"
#include "parse.h"

#define  NS  10

/*
 * reg_eprint - convert error number to name
 */
char *reg_eprint(int err) {
	static char epbuf[150];
	size_t len;

#ifdef REG_ITOA
	len = regerror(REG_ITOA|err, (regex_t *)NULL, epbuf, sizeof(epbuf));
#else
	len = regerror(err, (regex_t *)NULL, epbuf, sizeof(epbuf));
#endif
	if(len > sizeof(epbuf)) {
		epbuf[sizeof(epbuf)-1]='\0';
	}
	return(epbuf);
}

void RegMatch(char *reg_name, int search) {
	Stack *s;	
	regex_t re;
	regmatch_t subs[NS];
	int err, len, i, l;
	char erbuf[150];
	char *string;
	char temp[1] = { '\0' };
	char temp2[16];
	char *temp3=NULL;
	int copts = 0;
	off_t start, end;
	char *buf=NULL;
	char *pattern=NULL;
	
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
	if(!search) {
		if(*(s->strval) != '^') {
			pattern = emalloc(1,strlen(s->strval)+2);
			sprintf(pattern,"^%s",s->strval);
		}
	}
	if(s->type == STRING) {
		err = regcomp(&re, (pattern)?pattern:s->strval, copts);
	} else {
		temp3 = emalloc(1,sizeof(char)*2);
		sprintf(temp3,"%c",(int)s->intval);
		err = regcomp(&re, temp3, copts);
	}
	if(err) {
		len = regerror(err, &re, erbuf, sizeof(erbuf));
		Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
		regfree(&re);
		return;
	}	
	err = regexec(&re, string, (size_t)NS, subs, 0);
	if(err && err!=REG_NOMATCH) {
		len = regerror(err, &re, erbuf, sizeof(erbuf));
		Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
		regfree(&re);
		return;
	}

	l = strlen(string)+1;
	len = (int)(subs[0].rm_eo - subs[0].rm_so);
	if(len<0 || len>=l) len=0;
	if(reg_name && err != REG_NOMATCH) {
		if(l>150) {
			buf = emalloc(1,l);
		} else {
			buf = erbuf;
		}
		for(i=0; i < NS; i++) {
			sprintf(temp2,"%d",i);
			Push(temp2,STRING);
			*buf='\0';
			start = subs[i].rm_so;
			end = subs[i].rm_eo;
			if(start!=-1 && end>0 && start<l && end<l && start<end) {
				strncat(buf,&string[start],end-start);
				Push(buf,STRING);
			} else {
				Push("",STRING);
			}
			SetVar(reg_name,2,0); 
		}
	}
	
	if(!search) {	
		if(err==REG_NOMATCH) {
			Push("0",LNUMBER);
		} else {
			sprintf(temp2,"%d",len);
			Push(temp2,LNUMBER);
		}
	} else {
		if(err==REG_NOMATCH) {
			Push("",STRING);
		} else {
			Push(&string[subs[0].rm_so],STRING);
		}
	}
	regfree(&re);
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
		Error("Stack error in reg_replace");
		return;
	}
	if(s->strval) string = (char *)estrdup(1,s->strval);
	else string = temp;
	s = Pop();
	if(!s) {
		Error("Stack error in reg_replace");
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
		Error("Stack error in reg_replace");
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
	char *buf, *nbuf;
	char o;
	int i,l,ll,new_l,allo;
	regex_t re;
	regmatch_t subs[NS];
	char erbuf[150];
	int err, len;
#ifdef HAVE_REGCOMP
	char oo;
#endif

	l = strlen(string);
	if(!l) return(string);

	err = regcomp(&re, pattern, 0);
	if(err) {
		len = regerror(err, &re, erbuf, sizeof(erbuf));
		Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
		return((char *)-1);
	}	

	buf = emalloc(1,l*2*sizeof(char)+1);
	if(!buf) {
		Error("Unable to allocate memory in _RegReplace");
		regfree(&re);
		return((char *)-1);
	}
	err = 0;
	i = 0;
	allo = 2*l+1;
	buf[0] = '\0';	
	ll = strlen(replace);
	while(!err) {
#ifndef HAVE_REGCOMP
		subs[0].rm_so = i;
		subs[0].rm_eo = l;
		err = regexec(&re, string, (size_t)NS, subs, REG_STARTEND);
#else
		oo = string[l];
		string[l] = '\0';
		err = regexec(&re, &string[i], (size_t)NS, subs, 0);
		string[l] = oo;				
		subs[0].rm_so += i;
		subs[0].rm_eo += i;
#endif
		if(err && err!=REG_NOMATCH) {
			len = regerror(err, &re, erbuf, sizeof(erbuf));
			Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
			regfree(&re);
			return((char *)-1);
		}
		if(!err) {
			o = string[subs[0].rm_so];
			string[subs[0].rm_so]='\0';
			new_l = strlen(buf)+strlen(replace)+strlen(&string[i]);
			if(new_l > allo) {
				nbuf = emalloc(1,1+allo+2*new_l);
				allo = 1+allo + 2*new_l;
				strcpy(nbuf,buf);
				buf=nbuf;
			}	
			strcat(buf,&string[i]);
			strcat(buf,replace);

			string[subs[0].rm_so] = o;
			i = subs[0].rm_eo;
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
		if(subs[0].rm_so==0 && subs[0].rm_eo==0) break;
	}
	regfree(&re);
	return(buf);
}

void EReg(char *reg_name, int icase) {
	Stack *s;	
	regex_t re;
	regmatch_t subs[NS];
	int err, len, i, l;
	char erbuf[150];
	char *string;
	char temp[1] = { '\0' };
	char temp2[16];
	char *temp3=NULL;
	int copts = 0;
	off_t start, end;
	char *buf=NULL;
	
	s = Pop();
	if(!s) {
		Error("Stack error in ereg");
		return;
	}
	if(s->strval) string = (char *)estrdup(1,s->strval);
	else string = temp;

	s = Pop();
	if(!s) {
		Error("Stack error in ereg");
		return;
	}
	if(!reg_name) copts |= REG_NOSUB;
	if(icase) copts |= REG_ICASE;
	if(s->type == STRING) {
		err = regcomp(&re, s->strval, REG_EXTENDED | copts);
	} else {
		temp3 = emalloc(1,sizeof(char)*2);
		sprintf(temp3,"%c",(int)s->intval);
		err = regcomp(&re, temp3, copts);
	}
	if(err) {
		len = regerror(err, &re, erbuf, sizeof(erbuf));
		Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
		return;
	}	
	err = regexec(&re, string, (size_t)NS, subs, 0);
	if(err && err!=REG_NOMATCH) {
		len = regerror(err, &re, erbuf, sizeof(erbuf));
		Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
		regfree(&re);
		return;
	}

	len = 1;
	if(reg_name && err != REG_NOMATCH) {
		len = (int)(subs[0].rm_eo - subs[0].rm_so);
		l = strlen(string)+1;
		if(l>150) {
			buf = emalloc(1,l);
		} else {
			buf = erbuf;
		}
		for(i=0; i < NS; i++) {
			sprintf(temp2,"%d",i);
			Push(temp2,STRING);
			*buf='\0';
			start = subs[i].rm_so;
			end = subs[i].rm_eo;
			if(start!=-1 && end>0 && start<l && end<l && start<end) {
				strncat(buf,&string[start],end-start);
				Push(buf,STRING);
			} else {
				Push("",STRING);
			}
			SetVar(reg_name,2,0); 
		}
	}
	
	if(err==REG_NOMATCH) {
		Push("0",LNUMBER);
	} else {
		if(len==0) len=1;
		sprintf(temp2,"%d",len);
		Push(temp2,LNUMBER);
	}
	regfree(&re);
}

void ERegReplace(void) {
	Stack *s;	
	char *pattern;
	char *string;
	char *replace;
	char temp[1] = { '\0' };
	char *ret;

	s = Pop();
	if(!s) {
		Error("Stack error in ereg_replace");
		return;
	}
	if(s->strval) string = (char *)estrdup(1,s->strval);
	else string = temp;
	s = Pop();
	if(!s) {
		Error("Stack error in ereg_replace");
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
		Error("Stack error in ereg_replace");
		return;
	}
	if(s->type==STRING) {
		if(s->strval) pattern = (char *)estrdup(1,s->strval);
		else pattern = temp;
	} else {
		pattern = emalloc(1,2*sizeof(char));
		sprintf(pattern,"%c",(int)s->intval);
	}
	ret = _ERegReplace(pattern, replace, string, 0);
	if(ret==(char *)-1) {	
		Push("0",LNUMBER);
		return;	
	}
	Push(ret,STRING);
}

void ERegiReplace(void) {
	Stack *s;	
	char *pattern;
	char *string;
	char *replace;
	char temp[1] = { '\0' };
	char *ret;

	s = Pop();
	if(!s) {
		Error("Stack error in eregi_replace");
		return;
	}
	if(s->strval) string = (char *)estrdup(1,s->strval);
	else string = temp;
	s = Pop();
	if(!s) {
		Error("Stack error in eregi_replace");
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
		Error("Stack error in eregi_replace");
		return;
	}
	if(s->type==STRING) {
		if(s->strval) pattern = (char *)estrdup(1,s->strval);
		else pattern = temp;
	} else {
		pattern = emalloc(1,2*sizeof(char));
		sprintf(pattern,"%c",(int)s->intval);
	}
	ret = _ERegReplace(pattern, replace, string,1);
	if(ret==(char *)-1) {	
		Push("0",LNUMBER);
		return;	
	}
	Push(ret,STRING);
}

char *_ERegReplace(char *pattern, char *replace, char *string, int icase) {
	char *buf, *nbuf;
	char o;
	int i,ni,l,ll,new_l,allo;
	regex_t re;
	regmatch_t subs[NS];
	char erbuf[150];
	int err, len, copts=0;
#ifdef HAVE_REGCOMP
	char oo;
#endif

	l = strlen(string);
	if(!l) return(string);

	if(icase) copts = REG_ICASE;
	err = regcomp(&re, pattern, REG_EXTENDED | copts);
	if(err) {
		len = regerror(err, &re, erbuf, sizeof(erbuf));
		Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
		return((char *)-1);
	}	

	buf = emalloc(1,l*2*sizeof(char)+1);
	if(!buf) {
		Error("Unable to allocate memory in _RegReplace");
		regfree(&re);
		return((char *)-1);
	}
	err = 0;
	i = 0;
	ni = 0;
	allo = 2*l+1;
	buf[0] = '\0';	
	ll = strlen(replace);
	while(!err) {
#ifndef HAVE_REGCOMP
		subs[0].rm_so = i;
		subs[0].rm_eo = l;
		err = regexec(&re, string, (size_t)NS, subs, REG_STARTEND);
#else
		oo = string[l];
		string[l] = '\0';
		err = regexec(&re, &string[i], (size_t)NS, subs, 0);
		string[l] = oo;				
		if(!err) {
			subs[0].rm_so += i;
			subs[0].rm_eo += i;
		} else {
			subs[0].rm_so = 0;
			subs[0].rm_eo = 0;
		}
#endif
		if(err && err!=REG_NOMATCH) {
			len = regerror(err, &re, erbuf, sizeof(erbuf));
			Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
			regfree(&re);
			return((char *)-1);
		}
		if(!err) {
			o = string[subs[0].rm_so];
			string[subs[0].rm_so]='\0';
			new_l = strlen(buf)+strlen(replace)+strlen(&string[i]);
			if(new_l > allo) {
				nbuf = emalloc(1,1+allo+2*new_l);
				allo = 1+allo + 2*new_l;
				strcpy(nbuf,buf);
				buf=nbuf;
			}	
			strcat(buf,&string[i]);
			strcat(buf,replace);
			if(subs[0].rm_so == subs[0].rm_eo) {
				if(subs[0].rm_so >= l) break;
				ni = subs[0].rm_eo + 1;	
			} else {
				ni = subs[0].rm_eo;
			}
			string[subs[0].rm_so] = o;
			
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
		if(*pattern=='^') {
			new_l = strlen(buf)+strlen(&string[subs[0].rm_eo]);
			if(new_l > allo) {
				nbuf = emalloc(1,1+allo+2*new_l);
				allo = 1+allo + 2*new_l;
				strcpy(nbuf,buf);
				buf=nbuf;
			}	
			strcat(buf,&string[subs[0].rm_eo]);
			break;
		}
		i = ni;
	}
	regfree(&re);
	return(buf);
}
