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
   | Authors: Andi Gutmans <andi@vipe.technion.ac.il>                     |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: token_cache.h,v 1.2 1997/12/31 15:55:50 rasmus Exp $ */


#ifndef _TOKEN_CACHE
#define _TOKEN_CACHE

#include "alloc.h"

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


typedef struct _yystype_struct _yystype;
#define YYSTYPE _yystype

typedef union {
	long lval;					/* long value */
	double dval;				/* double value */
	char *strval;				/* string value */
	char chval;					/* char value */
	void *yystype_ptr;  /* used for implementation of multi-dimensional arrays */
} yystype_value;

struct _yystype_struct {
	/* Variable information */
	unsigned short type;		/* active type */

	/* Control structures */
	control_structure_data cs_data;
	unsigned int offset;

	unsigned char *func_arg_types;  /* optionally used to force passing by reference */

	yystype_value value;		/* value */
	int strlen;		/* string length */
};


typedef struct {
	YYSTYPE phplval;
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
extern int read_next_token(TokenCacheManager *tcm, Token **token, YYSTYPE *phplval);
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
