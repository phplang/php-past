/*
 * This file is a part of the Zend scripting language engine.  It may
 * be freely distributed as an integrated part of the PHP scripting language,
 * or under the Zend license, which is not yet available at this time.
 *
 * Authors:  Andi Gutmans <andi@zend.com>, Zeev Suraski <zeev@zend.com>
 */


#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "alloc.h"
#include "php.h"

#include "main.h"

#if DEBUG
# define END_MAGIC_SIZE sizeof(long)
# define END_ALIGNMENT(size) (((size)%PLATFORM_ALIGNMENT)?(PLATFORM_ALIGNMENT-((size)%PLATFORM_ALIGNMENT)):0)
#else
# define END_MAGIC_SIZE 0
# define END_ALIGNMENT(size) 0
#endif


#ifndef THREAD_SAFE
static mem_header *head;
void *cache[MAX_CACHED_MEMORY][MAX_CACHED_ENTRIES];
unsigned char cache_count[MAX_CACHED_MEMORY];

# if MEMORY_LIMIT
static unsigned int allocated_memory;
static unsigned char memory_exhausted;
#endif
#endif

# if MEMORY_LIMIT
#  if DEBUG
#define CHECK_MEMORY_LIMIT(s) _CHECK_MEMORY_LIMIT(s,filename,lineno)
#  else
#define CHECK_MEMORY_LIMIT(s)	_CHECK_MEMORY_LIMIT(s,NULL,0)
#  endif

#define _CHECK_MEMORY_LIMIT(s,file,lineno) { allocated_memory += (s);\
								if (php3_ini.memory_limit<allocated_memory) {\
									if (allocated_memory>php3_ini.memory_limit+1048576) {\
										if (file) { \
											char buf[64]; \
											snprintf(buf,64,"Called exit() on %s:%d (tried to allocate %d bytes)\n",file,lineno,s); \
											php3_log_err(buf); \
										} \
										exit(1); \
									} else if (memory_exhausted) { \
										/* do nothing, let it go through */ \
									} else { \
										if (!GLOBAL(shutdown_requested)) { \
											memory_exhausted=1;	\
											if (!file) { \
												php3_error(E_ERROR,"Allowed memory size of %d bytes exhausted (tried to allocate %d bytes)",php3_ini.memory_limit,s); \
											} else { \
												php3_error(E_ERROR,"Allowed memory size of %d bytes exhausted at %s:%d (tried to allocate %d bytes)",php3_ini.memory_limit,file,lineno,s); \
											} \
										} \
									} \
								}\
							  }
# endif

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

#if DEBUG
	if (!(initialized & INIT_MEMORY_MANAGER)) {
		fprintf(stderr,"WARNING:  Call to emalloc() before memory manager is started from %s:%u (%ul bytes)\n",filename,lineno, size);
	}
#endif

	BLOCK_INTERRUPTIONS;

	if ((size < MAX_CACHED_MEMORY) && (GLOBAL(cache_count[size]) > 0)) {
		p = GLOBAL(cache[size][--GLOBAL(cache_count[size])]);
#if DEBUG
		p->filename = filename;
		p->lineno = lineno;
		p->magic = MEM_BLOCK_START_MAGIC;
#endif
		UNBLOCK_INTERRUPTIONS;
		return (void *)((char *)p + sizeof(mem_header) + PLATFORM_PADDING);
	} else {
		p  = (mem_header *) malloc(sizeof(mem_header) + size + PLATFORM_PADDING + END_ALIGNMENT(size) + END_MAGIC_SIZE);
	}

	if (!p) {
		fprintf(stderr,"FATAL:  emalloc():  Unable to allocate %ul bytes\n", size);
		exit(1);
		UNBLOCK_INTERRUPTIONS;
		return (void *)p;
	}
	p->pNext = GLOBAL(head);
	if (GLOBAL(head)) {
		GLOBAL(head)->pLast = p;
	}
	p->pLast = (mem_header *) NULL;
	GLOBAL(head) = p;
	p->size = size;
#if DEBUG
	p->filename = filename;
	p->lineno = lineno;
	p->magic = MEM_BLOCK_START_MAGIC;
	*((long *)(((char *) p) + sizeof(mem_header)+size+PLATFORM_PADDING+END_ALIGNMENT(size))) = MEM_BLOCK_END_MAGIC;
#endif
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

#if DEBUG
	_mem_block_check(ptr, 1, filename, lineno);
	memset(ptr, 0x5a, p->size);
#endif

	if ((p->size < MAX_CACHED_MEMORY) && (GLOBAL(cache_count[p->size]) < MAX_CACHED_ENTRIES)) {
		GLOBAL(cache[p->size][GLOBAL(cache_count[p->size]++)]) = p;
#if DEBUG
		p->magic = MEM_BLOCK_CACHED_MAGIC;
#endif
		return;
	}
	BLOCK_INTERRUPTIONS;
	REMOVE_POINTER_FROM_LIST(p);

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
	p = _emalloc(final_size,filename,lineno);
#else
	p = emalloc(final_size);
#endif
	if (!p) {
		UNBLOCK_INTERRUPTIONS;
		return (void *) p;
	}
	memset(p,0,final_size);
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

	if (!ptr) {
#if DEBUG
		return _emalloc(size, filename, lineno);
#else
		return emalloc(size);
#endif
	}
	BLOCK_INTERRUPTIONS;
	REMOVE_POINTER_FROM_LIST(p);
	p = (mem_header *) realloc(p,sizeof(mem_header)+size+PLATFORM_PADDING+END_ALIGNMENT(size)+END_MAGIC_SIZE);
	if (!p) {
		fprintf(stderr,"FATAL:  erealloc():  Unable to allocate %ul bytes\n", size);
		exit(1);
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
	p->filename = filename;
	p->lineno = lineno;
	p->magic = MEM_BLOCK_START_MAGIC;
	*((long *)(((char *) p) + sizeof(mem_header)+size+PLATFORM_PADDING+END_ALIGNMENT(size))) = MEM_BLOCK_END_MAGIC;
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
	memory_exhausted=0;
#endif

	memset(GLOBAL(cache_count),0,MAX_CACHED_MEMORY*sizeof(unsigned char));
	GLOBAL(initialized) |= INIT_MEMORY_MANAGER;
}

void shutdown_memory_manager(void)
{
	mem_header *p, *t;
	unsigned int i,j;
	TLS_VARS;
	
	BLOCK_INTERRUPTIONS;
	for (i=0; i<MAX_CACHED_MEMORY; i++) {
		for (j=0; j<GLOBAL(cache_count[i]); j++) {
			p = GLOBAL(cache[i][j]);
			REMOVE_POINTER_FROM_LIST(p);
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
#if MODULE_MAGIC_NUMBER >= 19970831
			aplog_error(NULL, 0, APLOG_ERR | APLOG_NOERRNO, GLOBAL(php3_rqst)->server, memory_leak_buf);
#else
			log_error(memory_leak_buf,GLOBAL(php3_rqst)->server);
#endif
# else
			php3_printf("Freeing 0x%0.8X (%d bytes), allocated in %s on line %d<br>\n",(void *)((char *)t+sizeof(mem_header)+PLATFORM_PADDING),t->size,t->filename,t->lineno);
# endif			
		}
#endif
		p = t->pNext;
		free(t);
		t = p;
	}
	GLOBAL(initialized) &= ~INIT_MEMORY_MANAGER;
}


#if DEBUG
int _mem_block_check(void *ptr, int silent, char *filename, int lineno)
{
	mem_header *p = (mem_header *) ((char *)ptr - sizeof(mem_header) - PLATFORM_PADDING);
	int no_cache_notice=0;
	int valid_beginning=1;
	int had_problems=0;

	if (silent==2) {
		silent=1;
		no_cache_notice=1;
	}
	if (silent==3) {
		silent=0;
		no_cache_notice=1;
	}
	if (!silent) {
		fprintf(stderr,"---------------------------------------\n");
		fprintf(stderr,"Block 0x%0.8lX status at %s:%d:\n", (long) p, filename, lineno);
		fprintf(stderr,"%10s\t","Beginning:  ");
	}

	switch (p->magic) {
		case MEM_BLOCK_START_MAGIC:
			if (!silent) {
				fprintf(stderr, "OK (allocated on %s:%d, %d bytes)\n", p->filename, p->lineno, p->size);
			}
			break; /* ok */
		case MEM_BLOCK_FREED_MAGIC:
			if (!silent) {
				fprintf(stderr,"Freed\n");
				had_problems=1;
			} else {
				return _mem_block_check(ptr, 0, filename, lineno);
			}
			break;
		case MEM_BLOCK_CACHED_MAGIC:
			if (!silent) {
				if (!no_cache_notice) {
					fprintf(stderr,"Cached (allocated on %s:%d, %d bytes)\n", p->filename, p->lineno, p->size);
					had_problems=1;
				}
			} else {
				if (!no_cache_notice) {
					return _mem_block_check(ptr, 0, filename, lineno);
				}
			}
			break;
		default:
			if (!silent) {
				fprintf(stderr,"Overrun (magic=0x%0.8lX, expected=0x%0.8lX)\n", p->magic, MEM_BLOCK_START_MAGIC);
			} else {
				return _mem_block_check(ptr, 0, filename, lineno);
			}
			had_problems=1;
			valid_beginning=0;
			break;
	}


	if (valid_beginning
		&& *((long *)(((char *) p)+sizeof(mem_header)+p->size+PLATFORM_PADDING+END_ALIGNMENT(p->size))) != MEM_BLOCK_END_MAGIC) {
		long magic_num = MEM_BLOCK_END_MAGIC;
		char *overflow_ptr, *magic_ptr=(char *) &magic_num;
		int overflows=0;
		int i;

		if (silent) {
			return _mem_block_check(ptr, 0, filename, lineno);
		}
		had_problems=1;
		overflow_ptr = ((char *) p)+sizeof(mem_header)+p->size+PLATFORM_PADDING;

		for (i=0; i<sizeof(long); i++) {
			if (overflow_ptr[i]!=magic_ptr[i]) {
				overflows++;
			}
		}

		fprintf(stderr,"%10s\t", "End:");
		fprintf(stderr,"Overflown (magic=0x%0.8lX instead of 0x%0.8lX)\n", 
				*((long *)(((char *) p) + sizeof(mem_header)+p->size+PLATFORM_PADDING+END_ALIGNMENT(p->size))), MEM_BLOCK_END_MAGIC);
		fprintf(stderr,"%10s\t","");
		if (overflows>=sizeof(long)) {
			fprintf(stderr, "At least %lu bytes overflown\n", (unsigned long)sizeof(long));
		} else {
			fprintf(stderr, "%d byte(s) overflown\n", overflows);
		}
	} else if (!silent) {
		fprintf(stderr,"%10s\t", "End:");
		if (valid_beginning) {
			fprintf(stderr,"OK\n");
		} else {
			fprintf(stderr,"Unknown\n");
		}
	}
		
	if (!silent) {
		fprintf(stderr,"---------------------------------------\n");
	}
	return ((!had_problems) ? 1 : 0);
}


void _full_mem_check(int silent, char *filename, uint lineno)
{
	mem_header *p = head;
	int errors=0;

	fprintf(stderr,"------------------------------------------------\n");
	fprintf(stderr,"Full Memory Check at %s:%d\n", filename, lineno);

	while (p) {
		if (!_mem_block_check((void *)((char *)p + sizeof(mem_header) + PLATFORM_PADDING), (silent?2:3), filename, lineno)) {
			errors++;
		}
		p = p->pNext;
	}
	fprintf(stderr,"End of full memory check %s:%d (%d errors)\n", filename, lineno, errors);
	fprintf(stderr,"------------------------------------------------\n");
}
#endif



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
