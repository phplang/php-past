/***[pool.c]******************************************************[TAB=4]****\
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
/* $Id: pool.c,v 1.24 1997/07/18 01:28:04 rasmus Exp $ */
/*
 * Memory Pool Management with hooks for Apache sub-pool handling
 * for Apache module
 */

/* Figure out word-alignment for this platform */
#define MAXSUBPOOLS 3

#include <stdlib.h>
#include "php.h"
#include "parse.h"

#ifndef APACHE
#define PHP_BLOCK_MINFREE 8192     
#endif

 /* Sub-Pools */
static pool *php_pool[] = { NULL,  /* 0 - Stack pointer - no clear */
                            NULL,  /* 1 - Very temporary stuff - clear in yylex */
                            NULL   /* 2 - Expression Stack pool */
                          };
static long php_pool_size[] = { 0L, 0L, 0L };
static long max_data_space=DEFAULT_MAX_DATA_SPACE;
static int already_over=0;

#if DEBUG
static int memdbg=0;
#endif

#if APACHE
static void php_cleanup(void *ptr) {
	Exit(0);
}
#endif

#if APACHE
void php_init_pool(php_module_conf *conf) {
#else
void php_init_pool(void) {
#endif
	int i;

	for(i=0; i<MAXSUBPOOLS; i++) {
		php_pool[i] = NULL;	
		php_pool_size[i] = 0L;
	}	
#if APACHE
	if (conf->MaxDataSpace) max_data_space = conf->MaxDataSpace*1024;
	else max_data_space = DEFAULT_MAX_DATA_SPACE*1024;
	block_alarms();
	register_cleanup(php_rqst->pool,NULL,php_cleanup,php_cleanup);
	unblock_alarms();
#else
	max_data_space = DEFAULT_MAX_DATA_SPACE*1024;
#endif
	already_over=0;
}

void *php_pool_alloc(
#if DEBUG
char *file, int line,
#endif
int num, int bytes) {
	void *ptr;

#if DEBUG
	if(memdbg && num==0) Debug("%s:%d emalloc(%d,%d)\n",file,line,num,bytes);
#endif

	if(!php_pool[num]) {
#if APACHE
		php_pool[num] = make_sub_pool(php_rqst->pool);
#else
		php_pool[num] = php_make_sub_pool(NULL);  /* top-level pool */
#endif
	}
	php_pool_size[num] += bytes;
	if(php_pool_size[num] > max_data_space && !already_over) {
		already_over=1;
		Error("You have exceeded the Maximum Allowed Data space of %ld bytes",max_data_space);
		Exit(1);
	}
#if APACHE
	ptr = palloc(php_pool[num],bytes);
#else
	ptr = php_palloc(php_pool[num],bytes);	
#endif
	return(ptr);
}

char *php_pool_strdup(
#if DEBUG
char *file, int line,
#endif
int num, char *str) {
	char *ret;
	int l = strlen(str);

#if DEBUG
	if(memdbg && num==0) Debug("%s:%d estrdup(%d,%s)\n",file,line,num,str);
#endif

	if(!php_pool[num]) {
#if APACHE
		php_pool[num] = make_sub_pool(php_rqst->pool);
#else
		php_pool[num] = php_make_sub_pool(NULL); /* top-level pool */
#endif
	}
	php_pool_size[num] += l+1;
	if(php_pool_size[num] > max_data_space && !already_over) {
		already_over=1;
		Error("You have exceeded the Maximum Allowed Data space of %ld bytes",max_data_space);
		Exit(1);
	}
#if APACHE
	ret = pstrdup(php_pool[num],str);
#else
	ret = php_pstrdup(php_pool[num],str);
#endif
	return(ret);
}

void php_pool_clear(int num) {
	if(!num || !php_pool_size[num]) return;
	php_pool_size[num] = 0L;
	if(php_pool[num]) {
#if APACHE
		clear_pool(php_pool[num]);	
#else
		php_clear_pool(php_pool[num]);
#endif
	}
}

#ifndef APACHE
void php_pool_free(int num) {
	if(!num || !php_pool_size[num]) return;
	php_pool_size[num] = 0L;
	php_destroy_pool(php_pool[num]);
}
#endif

#if DEBUG
void php_pool_show(void) {
	int i;

	for(i=0; i<MAXSUBPOOLS; i++) {
		Debug("Pool %d: %ld bytes\n",i,php_pool_size[i]);
	}
	Debug("MaxDataSpace set to %ld\n",max_data_space);
}
#endif

void ShowPool(void) {
	int i;
	char temp[32];

	for(i=0; i<MAXSUBPOOLS; i++) {
		printf("Pool %d: %ld bytes\n",i,php_pool_size[i]);
	}
	printf("MaxDataSpace set to %ld\n",max_data_space);
	sprintf(temp,"%ld",max_data_space);	
	Push(temp,LNUMBER);
	SetVar("maxdataspace",48,0);
}

#ifndef APACHE
/* Portions of the following code was borrowed from the Apache HTTPD
 * server code which carries the copyright included below.  I got
 * tired of worrying about two separate memory handling mechanisms,
 * and since I have no chance of changing the Apache code to use my
 * mechanism, I decided to use a mechanism very similar to Apache's
 * here.  -Rasmus
 */

/* ====================================================================
 * Copyright (c) 1995 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 */

/*
 * Resource allocation code... the code here is responsible for making
 * sure that nothing leaks.
 *
 * rst --- 4/95 --- 6/95
 */

union php_align
{
  void (*f)();
  char *cp;
  long l;
  double d;
  FILE *fp;
};

#define PHP_ALIGN (sizeof(union php_align))

union php_block_hdr
{
  union php_align a;
  struct {
    char *endp;
    union php_block_hdr *next;
    char *first_avail;
  } h;
};

union php_block_hdr *php_block_freelist = NULL;

union php_block_hdr *php_malloc_block(int size) {
	union php_block_hdr *blok = 
		(union php_block_hdr *)malloc(size + sizeof(union php_block_hdr));

	if (blok == NULL) return NULL;
  
	blok->h.next = NULL;
	blok->h.first_avail = (char *)(blok + 1);
	blok->h.endp = size + blok->h.first_avail;
	return blok;
}

/* Free a chain of blocks */
void php_free_blocks (union php_block_hdr *blok) {
	/* First, put new blocks at the head of the free list ---
	 * we'll eventually bash the 'next' pointer of the last block
	 * in the chain to point to the free blocks we already had.
	 */
  
	union php_block_hdr *old_free_list = php_block_freelist;

	if(blok == NULL) return;	/* Sanity check --- freeing empty pool? */
	php_block_freelist = blok;
  
	/*
	 * Next, adjust first_avail pointers of each block --- have to do it
	 * sooner or later, and it simplifies the search in php_new_block to do it
	 * now.
	 */

	while (blok->h.next != NULL) {
		blok->h.first_avail = (char *)(blok + 1);
		blok = blok->h.next;
	}

	blok->h.first_avail = (char *)(blok + 1);

	/* Finally, reset next pointer to get the old free blocks back */
	blok->h.next = old_free_list;
}

/* 
 * Get a new block, from our own free list if possible, from the system
 * if necessary.  
 */
union php_block_hdr *php_new_block (int min_size) {
	union php_block_hdr **lastptr = &php_block_freelist;
	union php_block_hdr *blok = php_block_freelist;
  
	/* First, see if we have anything of the required size
	 * on the free list...
	 */

	min_size += PHP_BLOCK_MINFREE;

	while (blok != NULL) {
		if (min_size <= blok->h.endp - blok->h.first_avail) {
			*lastptr = blok->h.next;
			blok->h.next = NULL;
			return blok;
		} else {
			lastptr = &blok->h.next;
			blok = blok->h.next;
		}
	}
	/* Nope. */
	return php_malloc_block(min_size);
}

/* Each pool structure is allocated in the start of its own first block,
 * so we need to know how many bytes that is (once properly aligned...).
 * This also means that when a pool's sub-pool is destroyed, the storage
 * associated with it is *completely* gone, so we have to make sure it
 * gets taken off the parent's sub-pool list...
 */

#define PHP_POOL_HDR_CLICKS (1 + ((sizeof(struct pool) - 1) / PHP_ALIGN))
#define PHP_POOL_HDR_BYTES (PHP_POOL_HDR_CLICKS * PHP_ALIGN)			 

struct pool *php_make_sub_pool(struct pool *p) {
	union php_block_hdr *blok;
	pool *new_pool;

	blok = php_new_block(0);
	new_pool = (pool *)blok->h.first_avail;
	blok->h.first_avail += PHP_POOL_HDR_BYTES;

	memset ((char *)new_pool, '\0', sizeof (struct pool));
	new_pool->free_first_avail = blok->h.first_avail;
	new_pool->first = new_pool->last = blok;
    
	if (p) {
		new_pool->parent = p;
		new_pool->sub_next = p->sub_pools;
		if (new_pool->sub_next) new_pool->sub_next->sub_prev = new_pool;
		p->sub_pools = new_pool;
	}
	return new_pool;
}

void php_clear_pool(struct pool *a) {
	while (a->sub_pools) php_destroy_pool(a->sub_pools);
    
	a->sub_pools = NULL;
	php_free_blocks (a->first->h.next);    a->first->h.next = NULL;
	a->last = a->first;
	a->first->h.first_avail = a->free_first_avail;
}

void php_destroy_pool (pool *a) {
	php_clear_pool (a);

	if (a->parent) {
		if (a->parent->sub_pools == a) a->parent->sub_pools = a->sub_next;
		if (a->sub_prev) a->sub_prev->sub_next = a->sub_next;
		if (a->sub_next) a->sub_next->sub_prev = a->sub_prev;
	}
	php_free_blocks(a->first);
}

void *php_palloc (struct pool *a, int reqsize) {
	/* Round up requested size to an even number of alignment units (core clicks) */
	int nclicks = 1 + ((reqsize - 1) / PHP_ALIGN);
	int size = nclicks * PHP_ALIGN;
  
	/* First, see if we have space in the block most recently
	 * allocated to this pool
	 */
  
	union php_block_hdr *blok = a->last; 
	char *first_avail = blok->h.first_avail;
	char *new_first_avail;

	if (size <= 0) size = PHP_ALIGN;
	new_first_avail = first_avail + size;
  
	if(new_first_avail <= blok->h.endp) {
		blok->h.first_avail = new_first_avail;
		return (void *)first_avail;
	}

	/* Nope --- get a new one that's guaranteed to be big enough */
	blok = php_new_block (size);
	a->last->h.next = blok;
	a->last = blok;

	first_avail = blok->h.first_avail;
	blok->h.first_avail += size;

	return (void *)first_avail;
}

void *PHP_pcalloc(struct pool *a, int size) {
	void *res = php_palloc (a, size);
	memset (res, '\0', size);
	return res;
}

char *php_pstrdup(struct pool *a, const char *s) {
	char *res;
	if(s == NULL) return NULL;
	res = php_palloc (a, strlen(s) + 1);
	strcpy (res, s);
	return res;
}

#endif
