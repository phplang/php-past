/* $Id: mime.h,v 1.7 1998/12/28 09:43:58 sas Exp $ */

#ifndef _MIME_H
#define _MIME_H

extern void php3_mime_split(char *buf, int cnt, char *boundary, pval *http_post_vars);

#endif
