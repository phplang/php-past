/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf                                              |
   |                                                                      |
   +----------------------------------------------------------------------+
 */
/* $Id: mime.c,v 1.44 1998/02/02 08:07:21 shane Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include <stdio.h>
#include "parser.h"
#include "internal_functions.h"
#include "type.h"
#include "post.h"
#include "mime.h"


/*
 * Split raw mime stream up into appropriate components
 */
void php3_mime_split(char *buf, int cnt, char *boundary)
{
	char *ptr, *loc, *loc2, *s, *name, *filename, *u, *fn;
	int len, state = 0, Done = 0, rem, urem;
	long bytes, max_file_size = 0;
	char namebuf[128], filenamebuf[128], lbuf[256];
	FILE *fp;
	int itype;
	TLS_VARS;
	
	ptr = buf;
	rem = cnt;
	len = strlen(boundary);
	while ((ptr - buf < cnt) && !Done) {
		switch (state) {
			case 0:			/* Looking for mime boundary */
				loc = memchr(ptr, *boundary, cnt);
				if (loc) {
					if (!strncmp(loc, boundary, len)) {
						state = 1;
						rem -= (loc - ptr) + len + 2;
						ptr = loc + len + 2;
					} else {
						rem -= (loc - ptr) + 1;
						ptr = loc + 1;
					}
				} else {
					Done = 1;
				}
				break;
			case 1:			/* Check content-disposition */
				if (strncasecmp(ptr, "Content-Disposition: form-data;", 31)) {
					if (rem < 31)
						return;
					php3_error(E_WARNING, "File Upload Mime headers garbled [%c%c%c%c%c]", *ptr, *(ptr + 1), *(ptr + 2), *(ptr + 3), *(ptr + 4));
					return;
				}
				loc = memchr(ptr, '\n', rem);
				name = strstr(ptr, " name=\"");
				if (name && name < loc) {
					name += 7;
					s = memchr(name, '\"', loc - name);
					if (!s) {
						php3_error(E_WARNING, "File Upload Mime headers garbled [%c%c%c%c%c]", *name, *(name + 1), *(name + 2), *(name + 3), *(name + 4));
						return;
					}
					strncpy(namebuf, name, s - name);
					namebuf[s - name] = '\0';
					state = 2;
					loc2 = memchr(loc + 1, '\n', rem);
					rem -= (loc2 - ptr) + 1;
					ptr = loc2 + 1;
				} else {
					php3_error(E_WARNING, "File upload error - no name component in content disposition");
					return;
				}
				filename = strstr(s, " filename=\"");
				if (filename && filename < loc) {
					filename += 11;
					s = memchr(filename, '\"', loc - filename);
					if (!s) {
						php3_error(E_WARNING, "File Upload Mime headers garbled [%c%c%c%c%c]", *filename, *(filename + 1), *(filename + 2), *(filename + 3), *(filename + 4));
						return;
					}
					strncpy(filenamebuf, filename, s - filename);
					filenamebuf[s - filename] = '\0';
					sprintf(lbuf, "%s_name", namebuf);
					s = strrchr(filenamebuf, '\\');
					if (s && s > filenamebuf) {
						SET_VAR_STRING(lbuf, estrdup(s + 1));
					} else {
						SET_VAR_STRING(lbuf, estrdup(filenamebuf));
					}
					state = 3;
					if ((loc2 - loc) > 2) {
						if (!strncasecmp(loc + 1, "Content-Type:", 13)) {
							*(loc2 - 1) = '\0';
							sprintf(lbuf, "%s_type", namebuf);
							SET_VAR_STRING(lbuf, estrdup(loc + 15));
							*(loc2 - 1) = '\n';
						}
						rem -= 2;
						ptr += 2;
					}
				}
				break;

			case 2:			/* handle form-data fields */
				loc = memchr(ptr, *boundary, rem);
				u = ptr;
				while (loc) {
					if (!strncmp(loc, boundary, len))
						break;
					u = loc + 1;
					urem = rem - (loc - ptr) - 1;
					loc = memchr(u, *boundary, urem);
				}
				if (!loc) {
					php3_error(E_WARNING, "File Upload Field Data garbled");
					return;
				}
				*(loc - 4) = '\0';
				/* Magic function that figures everything out */
				_php3_parse_gpc_data(ptr,namebuf,NULL);

				/* And a little kludge to pick out special MAX_FILE_SIZE */
				itype = php3_CheckIdentType(namebuf);
				if (itype) {
					u = strchr(namebuf, '[');
					if (u)
						*u = '\0';
				}
				if (!strcmp(namebuf, "MAX_FILE_SIZE")) {
					max_file_size = atol(ptr);
				}
				if (itype) {
					if (u)
						*u = '[';
				}
				rem	-= (loc - ptr);
				ptr = loc;
				state = 0;
				break;

			case 3:			/* Handle file */
				loc = memchr(ptr, *boundary, rem);
				u = ptr;
				while (loc) {
					if (!strncmp(loc, boundary, len))
						break;
					u = loc + 1;
					urem = rem - (loc - ptr) - 1;
					loc = memchr(u, *boundary, urem);
				}
				if (!loc) {
					php3_error(E_WARNING, "File Upload Error - No Mime boundary found after start of file header");
					return;
				}
				fn = tempnam(php3_ini.upload_tmp_dir, "php");
				if (max_file_size && ((loc - ptr - 4) > max_file_size)) {
					php3_error(E_WARNING, "Max file size exceeded - file [%s] not saved", namebuf);
					bytes = 0;
					SET_VAR_STRING(namebuf, estrdup("none"));
				} else if ((loc - ptr - 4) <= 0) {
					bytes = 0;
					SET_VAR_STRING(namebuf, estrdup("none"));
				} else {
					fp = fopen(fn, "w");
					if (!fp) {
						php3_error(E_WARNING, "File Upload Error - Unable to open temporary file [%s]", fn);
						return;
					}
					bytes = fwrite(ptr, 1, loc - ptr - 4, fp);
					fclose(fp);
					if (bytes < (loc - ptr - 4)) {
						php3_error(E_WARNING, "Only %d bytes were written, expected to write %ld", bytes, loc - ptr - 4);
					}
					SET_VAR_STRING(namebuf, estrdup(fn));
				}
				sprintf(lbuf, "%s_size", namebuf);
				SET_VAR_LONG(lbuf, bytes);
				state = 0;
				rem -= (loc - ptr);
				ptr = loc;
				break;
		}
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
