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
#include <string.h>
#include "html_common.h"

#if DEBUG
extern FILE *fperr;
#endif

/*
 * SubVar
 *
 * This function substitutes variables designated by the leading separator (sep)
 * with values with matching names.  The string buffer will be realloced in 512-byte
 * chunks if needed.
 */
char *SubVar(char sep, char *var_name, char *var_value, char *oldbuf, int *bufsize) {
    char *buf, *s, *t;
    int l, n;
    char o;
    char *local_buf;
#if MSQL
	char *u;
	int i,j;
    int numi, numj;
    char temp[1024];
    m_result *result=NULL;
    m_row record;
    int new_connection=0;
#endif
 
    l=strlen(oldbuf);
    n=strlen(var_value);
 
    if(l-(int)strlen(var_name)-1+(int)strlen(var_value) >(int)(*bufsize-1)) {

#if DEBUG
        fprintf(fperr,"Query buffer re-allocated\n");
        fflush(fperr);
#endif

        buf=realloc(oldbuf,*bufsize+((n>512)?n:512));
        *bufsize+=((n>512)?n:512);
    } else buf=oldbuf;
 
    /* I don't trust strdup() on many systems */
    local_buf=malloc(*bufsize*sizeof(char));
    strcpy(local_buf,buf);
 
    if(!local_buf) {
        html_error("FI Error","Unable to allocate memory");
        exit(-1);
    }
    s=local_buf;
 
    while(*s!='\0') {
        if(*s==sep) {
            if(s>local_buf && *(s-1)=='\\') {  /* Allow for escaping the separator */
                s++;
                continue;
            }
            n=strcspn(s+1," ,()'\n:;.{}[]-+=~!@#$%^*<>\"");
 
            t=s+n+1;
            o=*t;
            *t='\0';
            if(!strcasecmp(var_name,s+1)) {
                *s='\0';
#if MSQL
                if(!strcasecmp(var_name,"sql")) {
                    /* looking for database name */
 
#if DEBUG
                    fprintf(fperr,"var_name=[%s], var_value=[%s]\n",var_name,var_value);
                    fflush(fperr);
#endif
 
                    u=strchr(var_value,' ');
                    if(u) {
                        *u='\0';
                        if(strlen(current_db)==0) {
                            if(dbsock==-1) {
                                dbsock=msqlConnect(NULL);
                                new_connection=1;
                            }
                            if(msqlSelectDB(dbsock,var_value)<0) {
                                strcpy(temp,msqlErrMsg);
                                current_db[0]='\0';
                            } else {
                                strcpy(current_db,var_value);
                            }
                        }
                        if(msqlQuery(dbsock,u+1)<0) strcpy(temp,msqlErrMsg);
                        else {
                            temp[0]='\0';
                            msqlStoreResult(result);
                            numi=msqlNumRows(result);
                            for(i=0;i<numi;i++) {
                                if(strlen(before_record)) strcat(temp,before_record);
                                msqlDataSeek(result,i);
                                numj=msqlNumFields(result);
                                record=msqlFetchRow(result);
                                for(j=0;j<numj;j++) {
                                    if(strlen(before_element)) strcat(temp,before_element);
                                    strcat(temp,record[j]);
                                    if(strlen(after_element)) strcat(temp,after_element);
                                }
                                if(strlen(after_record)) strcat(temp,after_record);
                            }
                            msqlFreeResult(result);
                            if(new_connection) {
                                msqlClose(dbsock);
                                dbsock=-1;
                                current_db[0]='\0';
                            }
                        }
                    }
                }
#endif
                sprintf(buf,"%s%s%c%s",local_buf,var_value,o,t+1);
                strcpy(local_buf,buf);
            } else {
                *t=o;
            }
            s=t+1;
        } else s++;
    }
    free(local_buf);
    return(buf);
}

