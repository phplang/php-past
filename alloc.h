/*
 * This file is a part of the Zend scripting language engine.  It may
 * be freely distributed as an integrated part of the PHP scripting language,
 * or under the Zend license, which is not yet available at this time.
 *
 * Authors:  Andi Gutmans <andi@zend.com>, Zeev Suraski <zeev@zend.com>
 */

#ifndef _ALLOC_H
#define _ALLOC_H
#include <stdio.h>
#include "php.h"

#define MEM_BLOCK_START_MAGIC	0x7312F8DCL
#define MEM_BLOCK_END_MAGIC		0x2A8FCC84L
#define MEM_BLOCK_FREED_MAGIC	0x99954317L
#define MEM_BLOCK_CACHED_MAGIC	0xFB8277DCL

typedef struct _mem_header {
#if DEBUG
	long magic;
	char *filename;
	uint lineno;
#endif
    struct _mem_header *pNext;
    struct _mem_header *pLast;
	int size;
} mem_header;

typedef union {
	void *ptr;
	double dbl;
	long lng;
} align_test;

#define MAX_CACHED_MEMORY   64
#define MAX_CACHED_ENTRIES	16

#define PLATFORM_ALIGNMENT (sizeof(align_test))
#define PLATFORM_PADDING (((PLATFORM_ALIGNMENT-sizeof(mem_header))%PLATFORM_ALIGNMENT+PLATFORM_ALIGNMENT)%PLATFORM_ALIGNMENT)

extern PHPAPI char *php3_strndup(const char *s, unsigned int length);

#if DEBUG
extern PHPAPI void *_emalloc(size_t size,char *filename,uint lineno);
extern PHPAPI void _efree(void *ptr,char *filename,uint lineno);
extern PHPAPI void *_ecalloc(size_t nmemb, size_t size,char *filename,uint lineno);
extern PHPAPI void *_erealloc(void *ptr, size_t size,char *filename,uint lineno);
extern PHPAPI char *_estrdup(const char *s,char *filename,uint lineno);
extern PHPAPI char *_estrndup(const char *s, unsigned int length,char *filename,uint lineno);
#define emalloc(size)			_emalloc((size),__FILE__,__LINE__)
#define efree(ptr)				_efree((ptr),__FILE__,__LINE__)
#define ecalloc(nmemb,size)		_ecalloc((nmemb),(size),__FILE__,__LINE__)
#define erealloc(ptr,size)		_erealloc((ptr),(size),__FILE__,__LINE__)
#define estrdup(s)				_estrdup((s),__FILE__,__LINE__)
#define estrndup(s,length)		_estrndup((s),(length),__FILE__,__LINE__)
#else
extern PHPAPI void *_emalloc(size_t size);
extern PHPAPI void _efree(void *ptr);
extern PHPAPI void *_ecalloc(size_t nmemb, size_t size);
extern PHPAPI void *_erealloc(void *ptr, size_t size);
extern PHPAPI char *_estrdup(const char *s);
extern PHPAPI char *_estrndup(const char *s, unsigned int length);
#define emalloc(size)			_emalloc((size))
#define efree(ptr)				_efree((ptr))
#define ecalloc(nmemb,size)		_ecalloc((nmemb),(size))
#define erealloc(ptr,size)		_erealloc((ptr),(size))
#define estrdup(s)				_estrdup((s))
#define estrndup(s,length)		_estrndup((s),(length))
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

#if DEBUG
int _mem_block_check(void *ptr, int silent, char *filename, int lineno);
void _full_mem_check(int silent, char *filename, uint lineno);
#define mem_block_check(ptr, silent) _mem_block_check(ptr, silent, __FILE__, __LINE__)
#define mem_block_check(ptr, silent) _mem_block_check(ptr, silent, __FILE__, __LINE__)
#define full_mem_check(silent) _full_mem_check(silent, __FILE__, __LINE__)
#else
#define mem_block_check(type, ptr, silent)
#define full_mem_check(silent)
#endif


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
