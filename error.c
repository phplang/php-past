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

void html_error(char *str1, char *str2) {
	puts("Content-type: text/html");
	puts("");
	printf("<html><head><title>%s</title></head><body>\n",str1);
	printf("<h2>%s</h2>\n",str1);
	if(str2) printf("<b>%s</b>\n",str2);
	puts("</body></html>");
}

void html_head(char *str) {
	puts("Content-type: text/html");
	puts("");
	if(str) printf("<html><head><title>%s</title></head>\n",str);
}
