/***[mime.c]******************************************************[TAB=4]****\
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
/* $Id: mime.c,v 1.12 1997/01/04 15:17:00 rasmus Exp $ */
#include <stdlib.h>
#include "php.h"
#include "parse.h"

#if APACHE
static char *UploadTmpDir=NULL;

void php_init_mime(php_module_conf *conf) {
	if(conf) UploadTmpDir = conf->UploadTmpDir;
}
#endif

/*
 * Split raw mime stream up into appropriate components
 */
void mime_split(char *buf, int cnt, char *boundary) {
#if FILE_UPLOAD
	char *ptr, *loc, *loc2, *s, *name, *filename, *ind, *tmp, *u, *fn, *ret;
	int len, state=0, Done=0, rem, itype, urem;
	long bytes, max_file_size=0;
	char namebuf[128], filenamebuf[128], lbuf[256];
	FILE *fp;

	ptr = buf;
	rem = cnt;
	len = strlen(boundary);
	while((ptr-buf < cnt) && !Done) {
		switch(state) {
			case 0: /* Looking for mime boundary */
				loc = memchr(ptr,*boundary,cnt);
				if(loc) {
					if(!strncmp(loc,boundary,len)) {
						state=1;
						rem -= (loc-ptr)+len+2;
						ptr=loc+len+2;	
					} else {
						rem -= (loc-ptr)+1;
						ptr=loc+1;	
					}
				} else {
					Done=1;
				}
				break;
			case 1: /* Check content-disposition */
				if(strncasecmp(ptr,"Content-Disposition: form-data;",31)) {
					if(rem < 31) return;
					Error("1 - File Upload Mime headers garbled [%c%c%c%c%c]",*ptr,*(ptr+1),*(ptr+2),*(ptr+3),*(ptr+4));
					return;
				}
				loc=memchr(ptr,'\n',rem);
				name = strstr(ptr," name=\"");	
				if(name && name < loc) {
					name += 7;
					s = memchr(name,'\"',loc-name);
					if(!s) {
						Error("2 - File Upload Mime headers garbled [%c%c%c%c%c]",*name,*(name+1),*(name+2),*(name+3),*(name+4));
						return;
					}
					strncpy(namebuf,name,s-name);
					namebuf[s-name]='\0';
					state=2;
					loc2=memchr(loc+1,'\n',rem);
					rem -= (loc2-ptr)+1;
					ptr=loc2+1;
				} else {
					Error("File upload error - no name component in content disposition");
					return;
				}
				filename = strstr(s," filename=\"");
				if(filename && filename < loc) {
					filename += 11;
					s = memchr(filename,'\"',loc-filename);
					if(!s) {
						Error("3 - File Upload Mime headers garbled [%c%c%c%c%c]",*filename,*(filename+1),*(filename+2),*(filename+3),*(filename+4));
						return;
					}
					strncpy(filenamebuf,filename,s-filename);
					filenamebuf[s-filename]='\0';
					sprintf(lbuf,"%s_name",namebuf);
					s = strrchr(filenamebuf,'\\');
					if(s && s >filenamebuf) Push(s+1,STRING);
					else Push(filenamebuf,STRING);
					SetVar(lbuf,0,0);
					state=3;
					if((loc2-loc)>2) {
						if(!strncasecmp(loc+1,"Content-Type:",13)) {
							*(loc2-1)='\0';
							sprintf(lbuf,"%s_type",namebuf);
							Push(loc+15,STRING);
							SetVar(lbuf,0,0);
							*(loc2-1)='\n';
						}
						rem-=2;
						ptr+=2;
					}
				}	
				break;
			case 2: /* handle form-data fields */
				loc = memchr(ptr,*boundary,rem);
				u=ptr;
				while(loc) {
					if(!strncmp(loc,boundary,len)) break;
					u=loc+1;
					urem=rem-(loc-ptr)-1;
					loc = memchr(u,*boundary,urem);
				}
				if(!loc) {
					Error("File Upload Field Data garbled");
					return;
				}
				*(loc-4)='\0';
				itype = CheckIdentType(namebuf);
				if(itype==2) {
					ind=GetIdentIndex(namebuf);
					tmp = estrdup(1,ind);
					Push((ret=AddSlashes(tmp,1)),STRING);
				}
				if(itype) {
					u = strchr(namebuf,'[');
					if(u) *u='\0';
				}
				tmp = estrdup(1,ptr);
				Push((ret=AddSlashes(tmp,1)),CheckType(ptr));
				SetVar(namebuf,itype,0);
				if(!strcmp(namebuf,"MAX_FILE_SIZE")) {
					max_file_size = atol(ptr);
				}	
				rem-=(loc-ptr);
				ptr=loc;
				state=0;
				break;
			case 3: /* Handle file */
				loc = memchr(ptr,*boundary,rem);
				u=ptr;
				while(loc) {
					if(!strncmp(loc,boundary,len)) break;
					u=loc+1;
					urem=rem-(loc-ptr)-1;
					loc = memchr(u,*boundary,urem);
				}
				if(!loc) {
					Error("File Upload Error - No Mime boundary found after start of file header");
					return;
				}	
#if APACHE
				fn = tempnam(UploadTmpDir,"phpfi");
#else
#ifdef UPLOAD_TMPDIR
				fn = tempnam(UPLOAD_TMPDIR,"phpfi");
#else
				fn = tempnam(NULL,"phpfi");
#endif
#endif
				if(max_file_size && ((loc-ptr-4) > max_file_size)) {
					Error("Max file size exceeded - file [%s] not saved",namebuf);
					bytes=0;
					Push("none",STRING);
					SetVar(namebuf,0,0);
				} else if((loc-ptr-4) <= 0) {
					bytes=0;
					Push("none",STRING);
					SetVar(namebuf,0,0);
				} else {
					fp = fopen(fn,"w");
					if(!fp) {
						Error("File Upload Error - Unable to open temporary file [%s]",fn);
						return;
					}
					bytes=fwrite(ptr,1,loc-ptr-4,fp);
					fclose(fp);		
					if(bytes<(loc-ptr-4)) {
						Error("Only %d bytes were written, expected to write %ld",bytes,loc-ptr-4);
					}
					Push(fn,STRING);
					SetVar(namebuf,0,0);
				}
				sprintf(lbuf,"%ld",bytes);
				Push(lbuf,LNUMBER);
				sprintf(lbuf,"%s_size",namebuf);
				SetVar(lbuf,0,0);
				state=0;
				rem-=(loc-ptr);
				ptr=loc;
				break;
		}
	}
#endif
}
