/*
   +----------------------------------------------------------------------+
   | Php HTML Embedded Scripting Language Version 3.0                     |
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
   | Authors: Hartmut Holzgraefe <hartmut@six.de>                         |
   +----------------------------------------------------------------------+
 */

/* $Id: levenshtein.c,v 1.3 2000/05/23 20:13:41 hholzgra Exp $ */

#include <stdio.h>
#include "php.h"
#include "internal_functions.h"
#include "reg.h"
#include "post.h"
#include "php3_string.h"

static int calc_levdist(const char *s1, const char *s2) /* faster, but obfuscated */
{
	register char *p1,*p2;
	register int i,j,n;
	int l1=0,l2=0;
	char r[512];
	const char *tmp;

	/* skip equal start sequence, if any */
	while(*s1==*s2) {
		if(!*s1) break;
		s1++; s2++;
	}
	
	/* if we already used up one string, then
      the result is the length of the other */
	if(*s1=='\0') return strlen(s2);
	if(*s2=='\0') return strlen(s1);

	/* length count */
	while(*s1++) l1++;
	while(*s2++) l2++;
	
	/* cut of equal tail sequence, if any */
	while(*--s1 == *--s2) {
		l1--; l2--;		
	}
	
	/* reset pointers, adjust length */
	s1-=l1++;
	s2-=l2++;
  	
	/* possible dist to great? */
 	if(abs(l1-l2)>=255) return -1;

	/* swap if l2 longer than l1 */
	if(l1<l2) {
		tmp=s1; s1=s2; s2=tmp;
		l1 ^= l2; l2 ^= l1; l1 ^= l2;
	}

	
	/* fill initial row */
	n=(*s1!=*s2);
	for(i=0,p1=r;i<l1;i++,*p1++=n++,p1++) {/*empty*/}
	
	/* calc. rowwise */
	for(j=1;j<l2;j++) {
		/* init pointers and col#0 */
		p1 = r + !(j&1);
		p2 = r + (j&1);
		n=*p1+1;
		*p2++=n;p2++;
		s2++;
		
		/* foreach column */
		for(i=1;i<l1;i++) {
			if(*p1<n) n=*p1+(*(s1+i)!=*(s2)); /* replace cheaper than delete? */
			p1++;
			if(*++p1<n) n=*p1+1; /* insert cheaper then insert ? */
			*p2++=n++; /* update field and cost for next col's delete */
			p2++;
		}	
	}

	/* return result */
	return n-1;
}


/* {{{ proto int levenshtein(string str1, string str2)
   Calculate Levenshtein distance between two strings */
void php3_levenshtein(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *str1, *str2;
	int l;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &str1, &str2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str1);
	convert_to_string(str2);
	
	l = calc_levdist(str1->value.str.val, str2->value.str.val);

	if(l<0) {
		php3_error(E_WARNING,"levenshtein(): argument string(s) too long");
	}

	RETURN_LONG(l);
}
/* }}} */

