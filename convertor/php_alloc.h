/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
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
   | Authors: Andi Gutmans   <andi@vipe.technion.ac.il>                   |
   |          Zeev Suraski   <zeev@zend.com>                              |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */

/* $Id: php_alloc.h,v 1.2 2000/02/07 23:54:50 zeev Exp $ */

#ifndef _ALLOC_H
#define _ALLOC_H
#include <stdio.h>

typedef struct _mem_header {
    struct _mem_header *pNext;
    struct _mem_header *pLast;
#if DEBUG
	char *filename;
	uint lineno;
	int size;
#endif
} mem_header;

typedef union {
	void *ptr;
	double dbl;
	long lng;
} align_test;

#define PLATFORM_ALIGNMENT (sizeof(align_test))
#define PLATFORM_PADDING (((PLATFORM_ALIGNMENT-sizeof(mem_header))%PLATFORM_ALIGNMENT+PLATFORM_ALIGNMENT)%PLATFORM_ALIGNMENT)

extern char *strndup(const char *s, unsigned int length);

#if DEBUG
extern void *_emalloc(size_t size,char *filename,uint lineno);
extern void _efree(void *ptr,char *filename,uint lineno);
extern void *_ecalloc(size_t nmemb, size_t size,char *filename,uint lineno);
extern void *_erealloc(void *ptr, size_t size,char *filename,uint lineno);
extern char *_estrdup(const char *s,char *filename,uint lineno);
extern char *_estrndup(const char *s, unsigned int length,char *filename,uint lineno);
#define emalloc(size)			_emalloc((size),__FILE__,__LINE__)
#define efree(ptr)				_efree((ptr),__FILE__,__LINE__)
#define ecalloc(nmemb,size)		_ecalloc((nmemb),(size),__FILE__,__LINE__)
#define erealloc(ptr,size)		_erealloc((ptr),(size),__FILE__,__LINE__)
#define estrdup(s)				_estrdup((s),__FILE__,__LINE__)
#define estrndup(s,length)		_estrndup((s),(length),__FILE__,__LINE__)
#else
# if CGI_BINARY || FHTTPD
#define emalloc(size)			malloc((size))
#define efree(ptr)				free((ptr))
#define ecalloc(nmemb,size)		calloc((nmemb),(size))
#define erealloc(ptr,size)		realloc((ptr),(size))
#define estrdup(s)				strdup((s))
#define estrndup(s,length)		strndup((s),(length))
# else
extern void *_emalloc(size_t size);
extern void _efree(void *ptr);
extern void *_ecalloc(size_t nmemb, size_t size);
extern void *_erealloc(void *ptr, size_t size);
extern char *_estrdup(const char *s);
extern char *_estrndup(const char *s, unsigned int length);
#define emalloc(size)			_emalloc((size))
#define efree(ptr)				_efree((ptr))
#define ecalloc(nmemb,size)		_ecalloc((nmemb),(size))
#define erealloc(ptr,size)		_erealloc((ptr),(size))
#define estrdup(s)				_estrdup((s))
#define estrndup(s,length)		_estrndup((s),(length))
# endif
#endif

#define pemalloc(size,persistent) ((persistent)?malloc(size):emalloc(size))
#define pefree(ptr,persistent)  ((persistent)?free(ptr):efree(ptr))
#define pecalloc(nmemb,size,persistent) ((persistent)?calloc((nmemb),(size)):ecalloc((nmemb),(size)))
#define perealloc(ptr,size,persistent) ((persistent)?realloc((ptr),(size)):erealloc((ptr),(size)))
#define pestrdup(s,persistent) ((persistent)?strdup(s):estrdup(s))

#define safe_estrdup(ptr)  ((ptr)?(estrdup(ptr)):(empty_string))
#define safe_estrndup(ptr,len) ((ptr)?(estrndup((ptr),(len))):(empty_string))

extern void start_memory_manager(void);
extern void shutdown_memory_manager(void);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
