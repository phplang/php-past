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


/* $Id: token_cache.c,v 1.97 2000/04/10 19:29:36 andi Exp $ */

extern char *phptext;
extern int phpleng;

#include "php.h"
#include "language-parser.tab.h"
#include "control_structures.h"
#include "main.h"
#include "highlight.h"

#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <stdio.h>

extern pval phplval;

TokenCache *tc; /* active token cache */

static int is_reserved_word(int token_type);
static int last_token_type;

/* initialize the Token Cache Manager */
int tcm_init(TokenCacheManager *tcm)
{
	TLS_VARS;

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
	GLOBAL(tc) = &tcm->token_caches[0];
	last_token_type=-1;
	return SUCCESS;
}


/* prepare a new token cache */
int tcm_new(TokenCacheManager *tcm, int block_size)
{
	TLS_VARS;
	
	if (tcm->initialized >= (1<<TC_BITS)) {
		return FAILURE;
	}
	tcm->initialized++;
	if (tcm->initialized >= tcm->max) {
		tcm->token_caches = (TokenCache *) erealloc(tcm->token_caches, (tcm->max + TOKEN_CACHES_BLOCK_SIZE) * sizeof(TokenCache));
		if (!tcm->token_caches) {
			return FAILURE;
		}
		tcm->max += TOKEN_CACHES_BLOCK_SIZE;
	}
	tcm->active = tcm->initialized - 1;
	if (tc_init(&tcm->token_caches[tcm->active],block_size) == FAILURE) {
		return FAILURE;
	}
	GLOBAL(tc) = &tcm->token_caches[tcm->active];
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
/*	YY_TLS_VARS;
	TLS_VARS; */

	if (GLOBAL(tc)->count == GLOBAL(tc)->pos || GLOBAL(php3_display_source)) {
		/* we need to read from the lexical scanner */
		Token next_token;

		phplval->type = IS_LONG;	/* the lex scanner doesn't always set phplval->type, make sure the type is not 'dirty' */
		phplval->cs_data.switched = 0;
		next_token.token_type = lex_scan(phplval);
		/*printf("Read token:  %c (%d)\n",next_token.token_type,next_token.token_type);*/
		if (next_token.token_type == DONE_EVAL) {
			return DONE_EVAL;
		}
		if (GLOBAL(tc)->count >= GLOBAL(tc)->max_tokens) {	/* we need more memory */
			GLOBAL(tc)->tokens = (Token *) erealloc(GLOBAL(tc)->tokens,
										   (sizeof(Token) * (GLOBAL(tc)->max_tokens += GLOBAL(tc)->block_size)));
			if (!GLOBAL(tc)->tokens) {
				return FAILURE;
			}
		}
		next_token.phplval = *phplval;
		next_token.lineno = GLOBAL(phplineno);
		next_token.phplval.offset = (tcm->active<<TOKEN_BITS) + GLOBAL(tc)->count;
		
		/* ugly hack to support $keyword */
		if (last_token_suggests_variable_reference() && is_reserved_word(next_token.token_type)) {
			next_token.phplval.value.str.val = estrndup(phptext,phpleng);
			next_token.phplval.value.str.len = phpleng;
			next_token.phplval.type = IS_STRING;
			next_token.token_type = STRING;
		}
		/* end of ugly hack */
		last_token_type=next_token.token_type;
		
		if (GLOBAL(php3_display_source)) {
			syntax_highlight(&next_token);
			*token = &next_token;
			return (*token)->token_type;
		}
		GLOBAL(tc)->tokens[GLOBAL(tc)->count] = next_token;
		GLOBAL(tc)->count++;
	}
	*token = &GLOBAL(tc)->tokens[GLOBAL(tc)->pos++];
	return (*token)->token_type;
}


int seek_token(TokenCacheManager *tcm, int token_number, int *yychar)
{
	int t_offset, tc_offset;
	TLS_VARS;
	
	clear_lookahead(yychar);
	
	
	tc_offset = TC_OFFSET(token_number);
	
	if (tc_offset >= tcm->initialized) {
		return FAILURE;
	}
	t_offset = TOKEN_OFFSET(token_number);
	if (t_offset > tcm->token_caches[tc_offset].count) {
		return FAILURE;
	}
	tcm->active = tc_offset;
	tcm->token_caches[tc_offset].pos = t_offset;
	GLOBAL(tc) = &tcm->token_caches[tc_offset];
	return SUCCESS;
}


/* switch tokens between start to middle-1, and middle+1 to end  */
int tc_switch(TokenCacheManager *tcm, int start, int end, int middle)
{
	int active = TC_OFFSET(start);
	TokenCache *tc = &tcm->token_caches[active];
	Token *temp_tokens, middle_token;
	int first_chunk, second_chunk, i;


	start = TOKEN_OFFSET(start);
	end = TOKEN_OFFSET(end);
	middle = TOKEN_OFFSET(middle);
	
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
		tc->tokens[i].phplval.offset = (active<<TOKEN_BITS) + i;
	}
	return SUCCESS;
}


inline int tc_set_token(TokenCacheManager *tcm, int offset, int type)
{
	TokenCache *tc = &tcm->token_caches[TC_OFFSET(offset)];
	
	offset = TOKEN_OFFSET(offset);
	if (offset < 0 || offset >= tc->count) {
		return FAILURE;
	}
	tc->tokens[offset].token_type = type;

	return SUCCESS;
}


inline int tc_get_token(TokenCacheManager *tcm, int offset)
{
	TokenCache *tc = &tcm->token_caches[TC_OFFSET(offset)];
	
	offset = TOKEN_OFFSET(offset);
	if (offset < 0 || offset >= tc->count) {
		return FAILURE;
	}
	return tc->tokens[offset].token_type;
}


int tc_get_current_offset(TokenCacheManager *tcm)
{
	return (tcm->active<<TOKEN_BITS) + GLOBAL(tc)->pos;
}


inline int tc_set_switched(TokenCacheManager *tcm, int offset)
{
	TokenCache *tc = &tcm->token_caches[TC_OFFSET(offset)];
	
	offset = TOKEN_OFFSET(offset);
	if (offset < 0 || offset >= tc->count) {
		return FAILURE;
	}
	tc->tokens[offset].phplval.cs_data.switched = 1;

	return SUCCESS;
}


inline int tc_set_included(TokenCacheManager *tcm, int offset)
{
	TokenCache *tc = &tcm->token_caches[TC_OFFSET(offset)];
	
	offset = TOKEN_OFFSET(offset);
	if (offset < 0 || offset >= tc->count) {
		return FAILURE;
	}
	tc->tokens[offset].phplval.cs_data.included = 1;

	return SUCCESS;
}


int tc_destroy(TokenCache *tc)
{
	register int i;
	TLS_VARS;

	for (i = 0; i < tc->count; i++) {
		pval_destructor(&tc->tokens[i].phplval _INLINE_TLS);
	}
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


void tcm_save(TokenCacheManager *tcm)
{
	TokenCache *tc;
	int i;
	char *output_name;
	FILE *output;
	TLS_VARS;
	
	if (tcm->initialized>1) {
		php3_printf("Cannot preprocess scripts that contain eval()\n");
		return;
	}
	
	tc = &tcm->token_caches[0];
	
	if (GLOBAL(request_info).filename) {
		int length=strlen(GLOBAL(request_info).filename);
		
		if (length>(sizeof(".php3")-1) && !strcmp(GLOBAL(request_info).filename+length-sizeof(".php3")+1,".php3")) {
			output_name = (char *) emalloc(length+2);
			strcpy(output_name,GLOBAL(request_info).filename);
			strcat(output_name,"p");
		} else {
			length += sizeof(".php3p");
			output_name = (char *) emalloc(length+1);
			strcpy(output_name,GLOBAL(request_info).filename);
			strcat(output_name,".php3p");
		}
	} else {
		output_name = estrdup("stdin.php3p");
	}
	
	if ((output=fopen(output_name,"wb"))==NULL) {
		php3_printf("Unable to open '%s' for writing.\n",output_name);
		efree(output_name);
		return;
	}
	fwrite("PHP3",sizeof(char),4,output);
	fwrite(tc,sizeof(TokenCache),1,output);
	fwrite(tc->tokens,sizeof(Token),tc->count,output);
	for (i=0; i<tc->count; i++) {
		switch (tc->tokens[i].token_type) {
			case STRING:
			case NUM_STRING:
			case INLINE_HTML:
			case ENCAPSED_AND_WHITESPACE:
			case BAD_CHARACTER:
				fwrite(tc->tokens[i].phplval.value.str.val,sizeof(char),tc->tokens[i].phplval.value.str.len,output);
				break;
		}
	}
	fclose(output);
#if !FHTTPD
	php3_printf("Created %s, %d tokens\n",output_name,tc->count);
#endif
	efree(output_name);
}

#if FHTTPD
int tcm_load(TokenCacheManager *tcm, FILE *input)
{
#else
int tcm_load(TokenCacheManager *tcm)
{
	FILE *input;
#endif
	char buf[32];
	TokenCache *tc;
	int i,len;
	char *str;
	TLS_VARS;

#if !FHTTPD
	if (!GLOBAL(request_info).filename) {
		return FAILURE;
	}
	
	if ((input=fopen(GLOBAL(request_info).filename,"rb"))==NULL) {
		return FAILURE;
	}
#endif	
	tc = &tcm->token_caches[0];

	efree(tc->tokens);  /* need to optimize this away, so that it doesn't get allocated in the first place */
	
	if (fread(buf,sizeof(char),4,input)!=4
		|| memcmp(buf,"PHP3",4)
		|| fread(tc,sizeof(TokenCache),1,input)!=1) {
			php3_printf("This doesn't look like a precompiled PHP 3.0 script\n");
#if !FHTTPD
			fclose(input);
#endif
			return FAILURE;
	}

	tc->tokens = (Token *) emalloc(sizeof(Token)*tc->max_tokens);
	if ((int)fread(tc->tokens,sizeof(Token),tc->count,input)!=tc->count) {
		php3_printf("Corrupted preprocessed script.\n");
#if !FHTTPD
		fclose(input);
#endif
		return FAILURE;
	}
	for (i=0; i<tc->count; i++) {
		switch (tc->tokens[i].token_type) {
			case STRING:
			case NUM_STRING:
			case INLINE_HTML:
			case ENCAPSED_AND_WHITESPACE:
				len = tc->tokens[i].phplval.value.str.len;
				str = (char *) emalloc(len+1);
				if ((int)fread(str,sizeof(char),len,input)!=len) {
					printf("Corrupted strings\n");
#if !FHTTPD
					fclose(input);
#endif
					return FAILURE;
				}
				str[len]=0;
				tc->tokens[i].phplval.value.str.val = str;
				break;
		}
	}
	tc->pos=0;
#if !FHTTPD
	fclose(input);
#endif
	return SUCCESS;
}


static int is_reserved_word(int token_type)
{
	switch (token_type) {
		case LOGICAL_OR:
		case LOGICAL_AND:
		case LOGICAL_XOR:
		case PHP_PRINT:
		case PHP_ECHO:
		case EXIT:
		case IF:
		case ELSEIF:
		case ELSE:
		case ENDIF:
		case DO:
		case WHILE:
		case ENDWHILE:
		case FOR:
		case ENDFOR:
		case SWITCH:
		case ENDSWITCH:
		case CASE:
		case DEFAULT:
		case BREAK:
		case CONTINUE:
		case OLD_FUNCTION:
		case PHP_CONST:
		case FUNCTION:
		case RETURN:
		case INCLUDE:
		case REQUIRE:
		case HIGHLIGHT_FILE:
		case HIGHLIGHT_STRING:
		case PHP_GLOBAL:
		case PHP_STATIC:
		case PHP_UNSET:
		case PHP_ISSET:
		case PHP_EMPTY:
		case CLASS:
		case EXTENDS:
		case PHP_LIST:
		case PHP_ARRAY:
		case NEW:
		case VAR:
		case EVAL:
		case PHP_LINE:
		case PHP_FILE:
			return 1;
			break;
		default:
			return 0;
			break;
	}
}


inline int last_token_suggests_variable_reference(void)
{
	if (last_token_type=='$' || last_token_type==PHP_CLASS_OPERATOR) {
		return 1;
	} else {
		return 0;
	}
}

