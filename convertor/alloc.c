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

/* $Id: alloc.c,v 1.9 2000/02/07 23:54:50 zeev Exp $ */

#include "token_cache.h"
#if WIN32|WINNT
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#endif

mem_header *head;


#if DEBUG
void *_emalloc(size_t size, char *filename, uint lineno)
#else
void *_emalloc(size_t size)
#endif
{
	mem_header *p;
	
	p  = (mem_header *) malloc(sizeof(mem_header) + size + PLATFORM_PADDING);

	if (!p) {
		return (void *)p;
	}
	p->pNext = GLOBAL(head);
	if (GLOBAL(head)) {
		GLOBAL(head)->pLast = p;
	}
	p->pLast = (mem_header *) NULL;
	GLOBAL(head) = p;
#if DEBUG
	p->filename = strdup(filename);
	p->lineno = lineno;
	p->size = size;
#endif
	return (void *)((char *)p + sizeof(mem_header) + PLATFORM_PADDING);
}


#if DEBUG
void _efree(void *ptr, char *filename, uint lineno)
#else
void _efree(void *ptr)
#endif
{
	mem_header *p = (mem_header *) ((char *)ptr - sizeof(mem_header) - PLATFORM_PADDING);
	
	if (p == GLOBAL(head)) {
		GLOBAL(head) = p->pNext;
	} else {
		p->pLast->pNext = p->pNext;
	}
	if (p->pNext) {
		p->pNext->pLast = p->pLast;
	}
#if DEBUG
	free(p->filename);
#endif
	free(p);
}


#if DEBUG
void *_ecalloc(size_t nmemb, size_t size, char *filename, uint lineno)
#else
void *_ecalloc(size_t nmemb, size_t size)
#endif
{
	mem_header *p;
	int final_size;

	final_size = sizeof(mem_header) + PLATFORM_PADDING + (nmemb * size);
	p = (mem_header *) malloc(final_size);
	if (!p) {
		return (void *) p;
	}
	memset(p,(int)NULL,final_size);
	p->pNext = GLOBAL(head);
	if (GLOBAL(head)) {
		GLOBAL(head)->pLast = p;
	}
	p->pLast = (mem_header *) NULL;
	GLOBAL(head) = p;
#if DEBUG
	p->filename = strdup(filename);
	p->lineno = lineno;
	p->size = size;
#endif	
	return (void *)((char *)p + sizeof(mem_header) + PLATFORM_PADDING);
}


#if DEBUG
void *_erealloc(void *ptr, size_t size, char *filename, uint lineno)
#else
void *_erealloc(void *ptr, size_t size)
#endif
{
	mem_header *p = (mem_header *) ((char *)ptr-sizeof(mem_header)-PLATFORM_PADDING);
	
	p = (mem_header *) realloc(p,sizeof(mem_header)+size+PLATFORM_PADDING);
	if (!p) {
		return (void *)p;
	}
	if (p->pLast) {
		p->pLast->pNext = p;
	} else {
		GLOBAL(head) = p;
	}
	if (p->pNext) {
		p->pNext->pLast = p;
	}
#if DEBUG
	free(p->filename);
	p->filename = strdup(filename);
	p->lineno = lineno;
	p->size = size;
#endif	
	return (void *)((char *)p+sizeof(mem_header)+PLATFORM_PADDING);
}


#if DEBUG
char *_estrdup(const char *s, char *filename, uint lineno)
#else
char *_estrdup(const char *s)
#endif
{
	int length;
	char *p;

	length = strlen(s)+1;
#if DEBUG
	p = (char *) _emalloc(length,filename,lineno);
#else
	p = (char *) emalloc(length);
#endif
	if (!p) {
		return p;
	}
	memcpy(p,s,length);
	return p;
}


#if DEBUG
char *_estrndup(const char *s, uint length, char *filename, uint lineno)
#else
char *_estrndup(const char *s, uint length)
#endif
{
	char *p;

#if DEBUG
	p = (char *) _emalloc(length+1,filename,lineno);
#else
	p = (char *) emalloc(length+1);
#endif
	if (!p) {
		return p;
	}
	memcpy(p,s,length);
	p[length]=0;
	return p;
}

char *strndup(const char *s, uint length)
{
	char *p;

	p = (char *) malloc(length+1);
	if (!p) {
		return p;
	}
	memcpy(p,s,length);
	p[length]=0;
	return p;
}

void start_memory_manager(void)
{

	GLOBAL(head) = NULL;
}

void shutdown_memory_manager(void)
{
	mem_header *p, *t;
	
	p=GLOBAL(head);
	t=GLOBAL(head);
	while (t) {
#if DEBUG
		/* does not use php3_error() *intentionally* */
		if (error_reporting & E_WARNING) {
			php3_printf("Freeing %x (%d bytes), allocated in %s on line %d<br>\n",(void *)((char *)t+sizeof(mem_header)+PLATFORM_PADDING),t->size,t->filename,t->lineno);
		}
#endif
		p = t->pNext;
#if DEBUG
		free(t->filename);
#endif
		free(t);
		t = p;
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
