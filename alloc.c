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
   | Authors: Andi Gutmans   <andi@php.net>                               |
   |          Zeev Suraski   <bourbon@netvision.net.il>                   |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */


#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "alloc.h"
#include "parser.h"

#include "main.h"

#define MAX_CACHED_MEMORY	64
#define MAX_CACHED_ENTRIES	16

#ifndef THREAD_SAFE
static mem_header *head;
void *cache[MAX_CACHED_MEMORY][MAX_CACHED_ENTRIES];
unsigned char cache_count[MAX_CACHED_MEMORY];

#if MEMORY_LIMIT
static unsigned int allocated_memory;
#define CHECK_MEMORY_LIMIT(s) { allocated_memory += (s);\
								if (php3_ini.memory_limit<allocated_memory) {\
									if (allocated_memory>php3_ini.memory_limit+1048576) {\
										exit(1); \
									} else {\
										if (!GLOBAL(shutdown_requested)) \
											php3_error(E_ERROR,"Allowed memory size of %d bytes exhausted",php3_ini.memory_limit); \
									}\
								}\
							  }
#endif
#endif

#ifndef CHECK_MEMORY_LIMIT
#define CHECK_MEMORY_LIMIT(s)
#endif


#define REMOVE_POINTER_FROM_LIST(p) \
	if (p == GLOBAL(head)) { \
		GLOBAL(head) = p->pNext; \
	} else { \
		p->pLast->pNext = p->pNext; \
	} \
	if (p->pNext) { \
		p->pNext->pLast = p->pLast; \
	}


/* used by ISAPI and NSAPI */

#if DEBUG
PHPAPI void *_emalloc(size_t size, char *filename, uint lineno)
#else
PHPAPI void *_emalloc(size_t size)
#endif
{
	mem_header *p;
	TLS_VARS;

	BLOCK_INTERRUPTIONS;

	if ((size < MAX_CACHED_MEMORY) && (cache_count[size] > 0)) {
		p = cache[size][--cache_count[size]];
#if DEBUG
		p->filename = strdup(filename);
		p->lineno = lineno;
#endif
		UNBLOCK_INTERRUPTIONS;
		return (void *)((char *)p + sizeof(mem_header) + PLATFORM_PADDING);
	} else {
		p  = (mem_header *) malloc(sizeof(mem_header) + size + PLATFORM_PADDING);
	}

	if (!p) {
		UNBLOCK_INTERRUPTIONS;
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
#endif
	p->size = size;
#if MEMORY_LIMIT
	CHECK_MEMORY_LIMIT(size);
#endif
	UNBLOCK_INTERRUPTIONS;
	return (void *)((char *)p + sizeof(mem_header) + PLATFORM_PADDING);
}


#if DEBUG
PHPAPI void _efree(void *ptr, char *filename, uint lineno)
#else
PHPAPI void _efree(void *ptr)
#endif
{
	mem_header *p = (mem_header *) ((char *)ptr - sizeof(mem_header) - PLATFORM_PADDING);
	TLS_VARS;
	
	if ((p->size < MAX_CACHED_MEMORY) && (cache_count[p->size] < MAX_CACHED_ENTRIES)) {
		cache[p->size][cache_count[p->size]++] = p;
#if DEBUG
		free(p->filename);
		p->filename = NULL;
#endif
		return;
	}
	BLOCK_INTERRUPTIONS;
	REMOVE_POINTER_FROM_LIST(p);
#if DEBUG
	free(p->filename);
#endif

#if MEMORY_LIMIT
	allocated_memory -= p->size;
#endif
	
	free(p);
	UNBLOCK_INTERRUPTIONS;
}


#if DEBUG
PHPAPI void *_ecalloc(size_t nmemb, size_t size, char *filename, uint lineno)
#else
PHPAPI void *_ecalloc(size_t nmemb, size_t size)
#endif
{
	void *p;
	int final_size=size*nmemb;
	TLS_VARS;

	BLOCK_INTERRUPTIONS;
#if DEBUG
	p = (void *) _emalloc(final_size,filename,lineno);
#else
	p = (char *) emalloc(final_size);
#endif
	if (!p) {
		UNBLOCK_INTERRUPTIONS;
		return (void *) p;
	}
	memset(p,(int)NULL,final_size);
	UNBLOCK_INTERRUPTIONS;
	return p;
}


#if DEBUG
PHPAPI void *_erealloc(void *ptr, size_t size, char *filename, uint lineno)
#else
PHPAPI void *_erealloc(void *ptr, size_t size)
#endif
{
	mem_header *p = (mem_header *) ((char *)ptr-sizeof(mem_header)-PLATFORM_PADDING);
	mem_header *orig = p;
	TLS_VARS;
	
	BLOCK_INTERRUPTIONS;
	REMOVE_POINTER_FROM_LIST(p);
	p = (mem_header *) realloc(p,sizeof(mem_header)+size+PLATFORM_PADDING);
	if (!p) {
		orig->pNext = GLOBAL(head);
		if (GLOBAL(head)) {
			GLOBAL(head)->pLast = orig;
		}
		orig->pLast = (mem_header *) NULL;
		GLOBAL(head) = orig;
		UNBLOCK_INTERRUPTIONS;
		return (void *)NULL;
	}
	p->pNext = GLOBAL(head);
	if (GLOBAL(head)) {
		GLOBAL(head)->pLast = p;
	}
	p->pLast = (mem_header *) NULL;
	GLOBAL(head) = p;
#if DEBUG
	free(p->filename);
	p->filename = strdup(filename);
	p->lineno = lineno;
#endif	
#if MEMORY_LIMIT
	CHECK_MEMORY_LIMIT(size - p->size);
#endif
	p->size = size;

	UNBLOCK_INTERRUPTIONS;
	return (void *)((char *)p+sizeof(mem_header)+PLATFORM_PADDING);
}


#if DEBUG
PHPAPI char *_estrdup(const char *s, char *filename, uint lineno)
#else
PHPAPI char *_estrdup(const char *s)
#endif
{
	int length;
	char *p;

	length = strlen(s)+1;
	BLOCK_INTERRUPTIONS;
#if DEBUG
	p = (char *) _emalloc(length,filename,lineno);
#else
	p = (char *) emalloc(length);
#endif
	if (!p) {
		UNBLOCK_INTERRUPTIONS;
		return (char *)NULL;
	}
	UNBLOCK_INTERRUPTIONS;
	memcpy(p,s,length);
	return p;
}


#if DEBUG
PHPAPI char *_estrndup(const char *s, uint length, char *filename, uint lineno)
#else
PHPAPI char *_estrndup(const char *s, uint length)
#endif
{
	char *p;

	BLOCK_INTERRUPTIONS;
#if DEBUG
	p = (char *) _emalloc(length+1,filename,lineno);
#else
	p = (char *) emalloc(length+1);
#endif
	if (!p) {
		UNBLOCK_INTERRUPTIONS;
		return (char *)NULL;
	}
	UNBLOCK_INTERRUPTIONS;
	memcpy(p,s,length);
	p[length]=0;
	return p;
}

PHPAPI char *php3_strndup(const char *s, uint length)
{
	char *p;

	p = (char *) malloc(length+1);
	if (!p) {
		return (char *)NULL;
	}
	if (length) {
		memcpy(p,s,length);
	}
	p[length]=0;
	return p;
}

void start_memory_manager(void)
{
	TLS_VARS;

	GLOBAL(head) = NULL;
	
#if MEMORY_LIMIT
	allocated_memory=0;
#endif

	memset(cache_count,0,MAX_CACHED_MEMORY*sizeof(unsigned char));
}

void shutdown_memory_manager(void)
{
	mem_header *p, *t;
	unsigned int i,j;
	TLS_VARS;
	
	BLOCK_INTERRUPTIONS;
	for (i=0; i<MAX_CACHED_MEMORY; i++) {
		for (j=0; j<cache_count[i]; j++) {
			p = cache[i][j];
			REMOVE_POINTER_FROM_LIST(p);
#if DEBUG
			if (p->filename) {
				free(p->filename);
			}
#endif
			free(p);
		}
	}
	UNBLOCK_INTERRUPTIONS;
	
	p=GLOBAL(head);
	t=GLOBAL(head);
	while (t) {
#if DEBUG
		/* does not use php3_error() *intentionally* */
		if (GLOBAL(error_reporting) & E_WARNING && GLOBAL(shutdown_requested)!=ABNORMAL_SHUTDOWN) {
# if APACHE  /* log into the errorlog, since at this time we can't send messages to the browser */
			char memory_leak_buf[512];

			snprintf(memory_leak_buf,512,"Possible PHP3 memory leak detected (harmless):  %d bytes from %s:%d",t->size,t->filename,t->lineno);
			log_error(memory_leak_buf,GLOBAL(php3_rqst)->server);
# else
			php3_printf("Freeing %x (%d bytes), allocated in %s on line %d<br>\n",(void *)((char *)t+sizeof(mem_header)+PLATFORM_PADDING),t->size,t->filename,t->lineno);
# endif			
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
