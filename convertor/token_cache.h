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
   | Authors: Andi Gutmans <andi@vipe.technion.ac.il>                     |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id: token_cache.h,v 1.11 2000/02/07 23:54:50 zeev Exp $ */


#ifndef _TOKEN_CACHE
#define _TOKEN_CACHE

#include "php_alloc.h"

#define uint unsigned int

#define SUCCESS 0
#define FAILURE -1

#define GLOBAL(x) x

#define MAX(a,b) (((a)>(b))?(a):(b))

/* data types */
#define IS_LONG 0x1
#define IS_DOUBLE 0x2
#define IS_STRING 0x4
#define IS_ARRAY 0x8
#define IS_EMPTY 0x10
#define IS_USER_FUNCTION 0x20
#define IS_INTERNAL_FUNCTION 0x40
#define IS_TEMPORARY_INTERNAL_FUNCTION 0x80
#define IS_UNSUPPORTED_FUNCTION 0x100
#define IS_CLASS 0x200
#define IS_OBJECT 0x400
#define IS_NULL 0x800

typedef union {
	unsigned char switched;
	unsigned char included;
	unsigned short function_call_type;
	unsigned char array_write;
	unsigned char error_reporting;
} control_structure_data;		/* control-structure data */


typedef struct _pvalue_struct _pvalue;
#define pval _pvalue
#define YYSTYPE pval

typedef union {
	long lval;					/* long value */
	double dval;				/* double value */
	char *strval;				/* string value */
	char chval;					/* char value */
	void *pvalue_ptr;  /* used for implementation of multi-dimensional arrays */
} pvalue_value;

struct _pvalue_struct {
	/* Variable information */
	unsigned short type;		/* active type */

	/* Control structures */
	control_structure_data cs_data;
	unsigned int offset;

	unsigned char *func_arg_types;  /* optionally used to force passing by reference */

	pvalue_value value;		/* value */
	int strlen;		/* string length */
};


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
#define TOKEN_CACHES_BLOCK_SIZE 4
#define MAX_TOKENS_PER_CACHE 1048576

extern int tcm_init(TokenCacheManager *tcm);
extern int tc_init(TokenCache *tc,int block_size);
extern int read_next_token(TokenCacheManager *tcm, Token **token, pval *phplval);
extern int seek_token(TokenCacheManager *tcm, int offset);
extern int tc_switch(TokenCacheManager *tcm, int start, int end, int middle);
extern int tc_set_switched(TokenCacheManager *tcm, int offset);
extern int tc_set_included(TokenCacheManager *tcm, int offset);
extern int tc_destroy(TokenCache *tc);
extern void tcm_destroy(TokenCacheManager *tcm);
extern int tcm_new(TokenCacheManager *tcm);
extern void tcm_save(TokenCacheManager *tcm);
extern int tcm_load(TokenCacheManager *tcm);

#endif
