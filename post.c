/*
 * PHP Tools
 *
 * Personal Home Page Tools.
 *
 * Copyright (C) 1995, Rasmus Lerdorf <rasmus@io.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * If you do use this software, please let me know.  I'd like to maintain
 * a list of URL's on my home page just to see what interesting things
 * people are doing with their home pages these days.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "html_common.h"

#define ishex(x) (((x) >= '0' && (x) <= '9') || ((x) >= 'a' && (x) <= 'f') || \
                  ((x) >= 'A' && (x) <= 'F'))

int htoi(unsigned char *s) {
        int     value;
        char    c;

        c = s[0];
        if (isupper(c))
                c = tolower(c);
        value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;

        c = s[1];
        if (isupper(c))
                c = tolower(c);
        value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;

        return (value);
}

void parse_url(unsigned char *data) {
    unsigned char *dest = data;

    while(*data) {
        if(*data=='+') *dest=' ';
        else if(*data== '%' && ishex(*(data+1)) && ishex(*(data+2))) {
            *dest = (unsigned char) htoi(data + 1);
            data+=2;
        } else *dest = *data;
		if(*dest=='<') *dest=' ';
		if(*dest=='>') *dest=' ';
        data++;
        dest++;
    }
    *dest = '\0';
}

/*
 * getpost()
 *
 * This reads the post form data into a string.
 * Remember to free this pointer when done with it.
 */
char *getpost(void) {
	static char *buf=NULL;
	int bytes, length, cnt=0;
	
	buf = getenv("CONTENT_TYPE");
	if(buf==NULL || strcmp(buf,"application/x-www-form-urlencoded")) {
		html_error("POST Error","No Content-type");
		return(NULL);
	}

	buf = getenv("CONTENT_LENGTH");
	if(buf==NULL) {
		html_error("POST Error","No Content Length");
		return(NULL);
	}
	
	length = atoi(buf);
	buf = malloc((length+1)*sizeof(char));
	if(!buf) {
		html_error("Malloc Error","Unable to allocate memory in getpost()");
		return(NULL);
	}
	do {
		bytes = fread(buf + cnt, 1, length - cnt, stdin);
		cnt += bytes;
	} while(bytes && cnt<length);
	buf[cnt]='\0';

	return(buf);
}

/*
 * This function returns 0 if no fields of a submitted form contain
 * any data and 1 otherwise.
 */
int CheckResult(char *res) {
	char *s, *t;
	int done=0;
 
	if(!res) return(0);
	if((int)strlen(res) < 3) return(0);
	t=res;
	while(!done) {
		s=strchr(t,'=');
		if(!s) return(0);
		t=strchr(s,'&');
		if(!t) {
			if((int)strlen(s) > 1) return(1);
			else return(0);
		} else {
			if(t-s>1) return(1);
			t++;
		}
	}
	return(0); /* never reached */
}
