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


/* $Id: token_cache.c,v 1.10 2000/02/07 23:54:50 zeev Exp $ */

#include "token_cache.h"
#include "language-parser.tab.h"
#include "main.h"

#if HAVE_STRING_H | (WIN32|WINNT)
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>

extern pval phplval;
extern int lex_scan(pval *phplval);

TokenCache *tc; /* active token cache */

extern char *phptext;
extern int phpleng;

static int is_reserved_word(int token_type);

/* initialize the Token Cache Manager */
int tcm_init(TokenCacheManager *tcm)
{
	tcm->active = 0;
	tcm->token_caches = (TokenCache *) emalloc(sizeof(TokenCache) * TOKEN_CACHES_BLOCK_SIZE);
	if (!tcm->token_caches) {
		return FAILURE;
	} else {
		tcm->max = TOKEN_CACHES_BLOCK_SIZE;
	}
	if (tc_init(&tcm->token_caches[0],TOKEN_CACHE_BLOCK_SIZE) == FAILURE) {
		return FAILURE;
	}
	tcm->initialized = 1;
	tc = &tcm->token_caches[0];
	return SUCCESS;
}


/* prepare a new token cache */
int tcm_new(TokenCacheManager *tcm)
{
	tcm->initialized++;
	if (tcm->initialized >= tcm->max) {
		tcm->token_caches = (TokenCache *) erealloc(tcm->token_caches, (tcm->max + TOKEN_CACHES_BLOCK_SIZE) * sizeof(TokenCache));
		if (!tcm->token_caches) {
			return FAILURE;
		}
		tcm->max += TOKEN_CACHES_BLOCK_SIZE;
	}
	tcm->active = tcm->initialized - 1;
	if (tc_init(&tcm->token_caches[tcm->active],TOKEN_CACHE_EVAL_BLOCK_SIZE) == FAILURE) {
		return FAILURE;
	}
	tc = &tcm->token_caches[tcm->active];
	return SUCCESS;
}


/* initialize a token cache */
int tc_init(TokenCache *tc,int block_size)
{
	tc->pos = tc->count = 0;
	tc->block_size = block_size;
	tc->tokens = (Token *) emalloc(sizeof(Token) * tc->block_size);
	if (tc->tokens) {
		tc->max_tokens = tc->block_size;
		return SUCCESS;
	} else {
		tc->max_tokens = 0;
		return FAILURE;
	}
}


int read_next_token(TokenCacheManager *tcm, Token **token, pval *phplval)
{
	if (tc->count == tc->pos) {	
		/* we need to read from the lexical scanner */
		Token next_token;

		phplval->type = IS_EMPTY;	/* the lex scanner doesn't always set phplval->type, make sure the type is not 'dirty' */
		phplval->cs_data.switched = 0;
		next_token.token_type = lex_scan(phplval);
		if (next_token.token_type == DONE_EVAL) {
			return DONE_EVAL;
		}
		if (tc->count >= tc->max_tokens) {	/* we need more memory */
			tc->tokens = (Token *) erealloc(tc->tokens,
										   (sizeof(Token) * (tc->max_tokens += tc->block_size)));
			if (!tc->tokens) {
				return FAILURE;
			}
		}
		next_token.phplval = *phplval;
		next_token.phplval.offset = tcm->active * MAX_TOKENS_PER_CACHE + tc->count;
		
		/* ugly hack to support $keyword */
		if (tc->count>0 && tc->tokens[tc->count-1].token_type=='$'
			&& is_reserved_word(next_token.token_type)) {
			next_token.phplval.value.strval = estrndup(phptext,phpleng);
			next_token.phplval.strlen = phpleng;
			next_token.phplval.type = IS_STRING;
			next_token.token_type = STRING;
		}
		/* end of ugly hack */
		
		switch (next_token.token_type) {
			case ';':
			case '>':
				break;
			case FUNCTION:
				printf("old_function");
				break;
			default:
				printf("%s",phptext);
				break;
		}
		tc->tokens[tc->count] = next_token;
		tc->count++;
	}
	tc->pos++;
	*token = &tc->tokens[tc->pos - 1];
	return (*token)->token_type;
}


int seek_token(TokenCacheManager *tcm, int token_number)
{
	int t_offset, tc_offset;

	tc_offset = token_number / MAX_TOKENS_PER_CACHE;
	if (tc_offset >= tcm->initialized) {
		return FAILURE;
	}
	t_offset = token_number % MAX_TOKENS_PER_CACHE;
	if (t_offset > tcm->token_caches[tc_offset].count) {
		return FAILURE;
	}
	tcm->active = tc_offset;
	tcm->token_caches[tc_offset].pos = t_offset;
	tc = &tcm->token_caches[tc_offset];
	return SUCCESS;
}


/* switch tokens between start to middle-1, and middle+1 to end  */
int tc_switch(TokenCacheManager *tcm, int start, int end, int middle)
{
	TokenCache *tc = &tcm->token_caches[start / MAX_TOKENS_PER_CACHE];
	Token *temp_tokens, middle_token;
	int first_chunk, second_chunk, i;

	start -= (tcm->active)*MAX_TOKENS_PER_CACHE;
	end -= (tcm->active)*MAX_TOKENS_PER_CACHE;
	middle -= (tcm->active)*MAX_TOKENS_PER_CACHE;
	
	if (start < 0 || end >= tc->count || middle < start || middle > end) {
		return FAILURE;
	}
	first_chunk = middle - start;
	second_chunk = end - middle;

	if (first_chunk == 0 && second_chunk == 0) {	/* optimize for(;;)'s */
		return SUCCESS;
	}
	middle_token = tc->tokens[middle];

	temp_tokens = (Token *) emalloc(sizeof(Token) * MAX(first_chunk, second_chunk));
	if (!temp_tokens) {
		return FAILURE;
	}
	if (first_chunk < second_chunk) {
		memcpy(temp_tokens, &tc->tokens[middle + 1], second_chunk * sizeof(Token));		/* save the second chunk */
		memcpy(&tc->tokens[end - first_chunk + 1], &tc->tokens[start], first_chunk * sizeof(Token));
		memcpy(&tc->tokens[start], temp_tokens, second_chunk * sizeof(Token));
	} else {
		memcpy(temp_tokens, &tc->tokens[start], first_chunk * sizeof(Token));
		memcpy(&tc->tokens[start], &tc->tokens[middle + 1], second_chunk * sizeof(Token));
		memcpy(&tc->tokens[end - first_chunk + 1], temp_tokens, first_chunk * sizeof(Token));
	}
	efree(temp_tokens);

	tc->tokens[start + second_chunk] = middle_token;

	for (i = start; i <= end; i++) {
		tc->tokens[i].phplval.offset = (tcm->active) * MAX_TOKENS_PER_CACHE + i;
	}
	return SUCCESS;
}


int tc_set_switched(TokenCacheManager *tcm, int offset)
{
	TokenCache *tc = &tcm->token_caches[offset / MAX_TOKENS_PER_CACHE];
	
	offset -= tcm->active * MAX_TOKENS_PER_CACHE;
	if (offset < 0 || offset >= tc->count) {
		return FAILURE;
	}
	tc->tokens[offset].phplval.cs_data.switched = 1;

	return SUCCESS;
}


int tc_set_included(TokenCacheManager *tcm, int offset)
{
	TokenCache *tc = &tcm->token_caches[offset / MAX_TOKENS_PER_CACHE];
	
	offset -= tcm->active * MAX_TOKENS_PER_CACHE;
	if (offset < 0 || offset >= tc->count) {
		return FAILURE;
	}
	tc->tokens[offset].phplval.cs_data.included = 1;

	return SUCCESS;
}


int tc_destroy(TokenCache *tc)
{
	if (tc->tokens) {
		efree(tc->tokens);
	}
	return SUCCESS;
}


void tcm_destroy(TokenCacheManager *tcm)
{
	register int i;

	for (i = 0; i < tcm->initialized; i++) {
		tc_destroy(&tcm->token_caches[i]);
	}
	efree(tcm->token_caches);
}


static int is_reserved_word(int token_type)
{
	switch (token_type) {
		case FUNCTION:
		case CLASS:
		case EXTENDS:
		case VAR:
		case NEW:
		case PHPECHO:
		case IF:
		case ELSE:
		case ELSEIF:
		case ENDIF:
		case WHILE:
		case ENDWHILE:
		case DO:
		case FOR:
		case BREAK:
		case CONTINUE:
		case INCLUDE:
		case SHOW_SOURCE:
		case EVAL:
		case PHP_ARRAY:
		case PHP_LIST:
			return 1;
			break;
		default:
			return 0;
			break;
	}
}
