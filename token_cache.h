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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id: token_cache.h,v 1.38 2000/04/10 19:29:36 andi Exp $ */


#ifndef _TOKEN_CACHE
#define _TOKEN_CACHE

typedef struct {
	pval phplval;
	int token_type;
	unsigned int lineno;
} Token;

typedef struct {
	Token *tokens;				/* tokens array */
	int count;					/* token count */
	int pos;					/* current position in the tokens array */
	int max_tokens;				/* (current) max tokens number (respectively) */
	int block_size;				/* token block size */
} TokenCache;

typedef struct {
	TokenCache *token_caches;
	int active;
	int max;
	int initialized;
} TokenCacheManager;

#define TOKEN_CACHE_BLOCK_SIZE 8192
#define TOKEN_CACHE_EVAL_BLOCK_SIZE 32
#define TOKEN_CACHE_INCLUDE_BLOCK_SIZE 512

#define TOKEN_CACHES_BLOCK_SIZE 4

#define TOKEN_BITS 20
#define TC_BITS ((sizeof(int))*8-TOKEN_BITS)

#define TC_OFFSET(token_number)		(token_number>>TOKEN_BITS)
#define TOKEN_OFFSET(token_number)	(token_number & ((1<<TOKEN_BITS)-1))
#define FILE_OFFSET TC_OFFSET
#define LINE_OFFSET TOKEN_OFFSET

extern int tcm_init(TokenCacheManager *tcm);
extern int tc_init(TokenCache *tc,int block_size);
extern int read_next_token(TokenCacheManager *tcm, Token **token, pval *phplval);
extern int seek_token(TokenCacheManager *tcm, int offset, int *yychar);
extern int tc_switch(TokenCacheManager *tcm, int start, int end, int middle);
extern inline int tc_set_token(TokenCacheManager *tcm, int offset, int type);
extern inline int tc_get_token(TokenCacheManager *tcm, int offset);
extern inline int tc_set_switched(TokenCacheManager *tcm, int offset);
extern inline int tc_set_included(TokenCacheManager *tcm, int offset);
extern int tc_get_current_offset(TokenCacheManager *tcm);
extern int tc_destroy(TokenCache *tc);
extern void tcm_destroy(TokenCacheManager *tcm);
extern int tcm_new(TokenCacheManager *tcm, int block_size);
extern void tcm_save(TokenCacheManager *tcm);
#if FHTTPD
extern int tcm_load(TokenCacheManager *tcm, FILE *input);
#else
extern int tcm_load(TokenCacheManager *tcm);
#endif
extern void clear_lookahead(int *yychar);

extern inline int last_token_suggests_variable_reference(void);

#endif
