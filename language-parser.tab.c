
/*  A Bison parser, made from language-parser.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define yyparse phpparse
#define yylex phplex
#define yyerror phperror
#define yylval phplval
#define yychar phpchar
#define yydebug phpdebug
#define yynerrs phpnerrs
#define	LOGICAL_OR	258
#define	LOGICAL_XOR	259
#define	LOGICAL_AND	260
#define	PHP_PRINT	261
#define	PLUS_EQUAL	262
#define	MINUS_EQUAL	263
#define	MUL_EQUAL	264
#define	DIV_EQUAL	265
#define	CONCAT_EQUAL	266
#define	MOD_EQUAL	267
#define	AND_EQUAL	268
#define	OR_EQUAL	269
#define	XOR_EQUAL	270
#define	SHIFT_LEFT_EQUAL	271
#define	SHIFT_RIGHT_EQUAL	272
#define	BOOLEAN_OR	273
#define	BOOLEAN_AND	274
#define	IS_EQUAL	275
#define	IS_NOT_EQUAL	276
#define	IS_SMALLER_OR_EQUAL	277
#define	IS_GREATER_OR_EQUAL	278
#define	SHIFT_LEFT	279
#define	SHIFT_RIGHT	280
#define	INCREMENT	281
#define	DECREMENT	282
#define	INT_CAST	283
#define	DOUBLE_CAST	284
#define	STRING_CAST	285
#define	ARRAY_CAST	286
#define	OBJECT_CAST	287
#define	NEW	288
#define	EXIT	289
#define	IF	290
#define	ELSEIF	291
#define	ELSE	292
#define	ENDIF	293
#define	LNUMBER	294
#define	DNUMBER	295
#define	STRING	296
#define	NUM_STRING	297
#define	INLINE_HTML	298
#define	CHARACTER	299
#define	BAD_CHARACTER	300
#define	ENCAPSED_AND_WHITESPACE	301
#define	PHP_ECHO	302
#define	DO	303
#define	WHILE	304
#define	ENDWHILE	305
#define	FOR	306
#define	ENDFOR	307
#define	SWITCH	308
#define	ENDSWITCH	309
#define	CASE	310
#define	DEFAULT	311
#define	BREAK	312
#define	CONTINUE	313
#define	CONTINUED_WHILE	314
#define	CONTINUED_DOWHILE	315
#define	CONTINUED_FOR	316
#define	OLD_FUNCTION	317
#define	FUNCTION	318
#define	PHP_CONST	319
#define	RETURN	320
#define	INCLUDE	321
#define	REQUIRE	322
#define	HIGHLIGHT_FILE	323
#define	HIGHLIGHT_STRING	324
#define	PHP_GLOBAL	325
#define	PHP_STATIC	326
#define	PHP_UNSET	327
#define	PHP_ISSET	328
#define	PHP_EMPTY	329
#define	CLASS	330
#define	EXTENDS	331
#define	PHP_CLASS_OPERATOR	332
#define	PHP_DOUBLE_ARROW	333
#define	PHP_LIST	334
#define	PHP_ARRAY	335
#define	VAR	336
#define	EVAL	337
#define	DONE_EVAL	338
#define	PHP_LINE	339
#define	PHP_FILE	340
#define	STRING_CONSTANT	341

#line 1 "language-parser.y"


/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
*/


/* $Id: language-parser.y,v 1.159 1998/07/03 06:21:25 zeev Exp $ */


/* 
 * LALR shift/reduce conflicts and how they are resolved:
 *
 * - 2 shift/reduce conflicts due to the dangeling elseif/else ambiguity.  Solved by shift.
 * - 1 shift/reduce conflict due to arrays within encapsulated strings. Solved by shift. 
 * - 1 shift/reduce conflict due to objects within encapsulated strings.  Solved by shift.
 * - 1 shift/reduce conflict due to while loop nesting ambiguity.
 * - 1 shift/reduce conflict due to for loop nesting ambiguity.
 * 
 */

#ifdef THREAD_SAFE
#include "tls.h"
#define PHP3_THREAD_SAFE
#else
extern char *phptext;
extern int phpleng;
#define YY_TLS_VARS
#endif
#include "php.h"
#include "php3_list.h"
#include "control_structures.h"
#include "control_structures_inline.h"

#include "main.h"
#include "functions/head.h"


#define YYERROR_VERBOSE

#ifndef THREAD_SAFE
HashTable symbol_table;		/* main symbol table */
HashTable function_table;	/* function symbol table */
HashTable include_names;	/* included file names are stored here, for error messages */
TokenCacheManager token_cache_manager;
Stack css;					/* Control Structure Stack, used to store execution states */
Stack for_stack;			/* For Stack, used in order to determine
							 * whether we're in the first iteration of 
							 * a for loop or not
							 */
Stack input_source_stack;		/* Include Stack, used to keep track of
							 * of the file pointers when include()'ing
							 * files.
							 */
Stack function_state_stack;	/* Function State Stack, used to keep inforation
							 * about the function call, such as its
							 * loop nesting level, symbol table, etc.
							 */
Stack switch_stack;			/* Switch stack, used by the switch() control
							 * to determine whether a case was already
							 * matched.
							 */

Stack variable_unassign_stack; /* Stack used to unassign variables that weren't properly defined */

HashTable *active_symbol_table;  /* this always points to the
								  * currently active symbol
								  * table.
								  */

int Execute;	/* This flag is used by all parts of the program, in order
				 * to determine whether or not execution should take place.
				 * It's value is dependant on the value of the ExecuteFlag,
				 * the loop_change_level (if we're during a break or 
				 * continue) and the function_state.returned (whether
				 * our current function has been return()ed from).
				 * In order to restore the correct value, after changing
				 * one of the above (e.g. when popping the css),
				 * one should use the macro Execute = SHOULD_EXECUTE;
				 */
int ExecuteFlag;
int current_lineno;		/* Current line number, broken with include()s */
int include_count;

/* This structure maintains information about the current active scope.
 * Kept are the loop nesting level, information about break's and continue's
 * we might be in, whether or not our current function has been return()'d
 * from, and also, a pointer to the active symbol table.  During variable
 * -passing in a function call, it also contains a pointer to the
 * soon-to-be-called function symbol table.
 */
FunctionState function_state;

/* The following two variables are used when inside class definitions. */
char *class_name=NULL;
HashTable *class_symbol_table=NULL;

/* Variables used in function calls */
pval globals;
unsigned int param_index;  /* used during function calls */

/* Temporary variable used for multi dimensional arrays */
pval *array_ptr;

extern int shutdown_requested;
#endif /* THREAD SAFE*/

int init_resource_list(void)
{
	TLS_VARS;
	
	return _php3_hash_init(&GLOBAL(list), 0, NULL, list_entry_destructor, 0);
}

int init_resource_plist(void)
{
	TLS_VARS;
	
	return _php3_hash_init(&GLOBAL(plist), 0, NULL, plist_entry_destructor, 1);
}

void destroy_resource_list(void)
{
	TLS_VARS;
	
	_php3_hash_destroy(&GLOBAL(list));
}

void destroy_resource_plist(void)
{
	TLS_VARS;
	
	_php3_hash_destroy(&GLOBAL(plist));
}

int clean_module_resource(list_entry *le, int *resource_id)
{
	if (le->type == *resource_id) {
		printf("Cleaning resource %d\n",*resource_id);
		return 1;
	} else {
		return 0;
	}
}


int clean_module_resource_destructors(list_destructors_entry *ld, int *module_number)
{
	TLS_VARS;
	if (ld->module_number == *module_number) {
		_php3_hash_apply_with_argument(&GLOBAL(plist), (int (*)(void *,void *)) clean_module_resource, (void *) &(ld->resource_id));
		return 1;
	} else {
		return 0;
	}
}

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		610
#define	YYFLAG		-32768
#define	YYNTBASE	116

#define YYTRANSLATE(x) ((unsigned)(x) <= 341 ? yytranslate[x] : 217)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    41,   113,     2,   111,    40,    26,   114,   108,
   109,    38,    35,     3,    36,    37,    39,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    21,   110,    29,
     8,    31,    20,    50,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    51,     2,   115,    25,     2,   112,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   106,    24,   107,    42,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     4,     5,     6,
     7,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    22,    23,    27,    28,    30,    32,    33,    34,
    43,    44,    45,    46,    47,    48,    49,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
    95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
   105
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     4,     8,     9,    18,    19,    20,    33,    34,
    35,    44,    45,    46,    47,    59,    60,    61,    62,    63,
    64,    80,    81,    88,    91,    95,    98,   102,   103,   112,
   113,   123,   126,   130,   133,   136,   137,   144,   145,   154,
   158,   160,   163,   164,   169,   173,   179,   187,   193,   197,
   199,   201,   206,   210,   215,   220,   226,   228,   233,   234,
   235,   243,   244,   245,   246,   257,   258,   259,   260,   261,
   262,   277,   278,   279,   280,   281,   291,   292,   293,   294,
   295,   306,   307,   308,   312,   313,   314,   319,   320,   321,
   322,   330,   331,   336,   337,   338,   346,   347,   352,   353,
   356,   357,   360,   364,   368,   373,   378,   384,   390,   397,
   399,   400,   402,   404,   407,   411,   415,   420,   425,   428,
   433,   440,   443,   448,   450,   454,   457,   458,   462,   463,
   472,   473,   483,   488,   495,   498,   503,   504,   508,   510,
   511,   515,   517,   524,   528,   532,   536,   540,   544,   548,
   552,   556,   560,   564,   568,   572,   575,   578,   581,   584,
   585,   590,   591,   596,   597,   602,   603,   608,   612,   616,
   620,   624,   628,   632,   636,   640,   644,   648,   652,   656,
   659,   662,   665,   668,   672,   676,   680,   684,   688,   692,
   696,   700,   701,   702,   710,   711,   712,   720,   722,   723,
   724,   735,   738,   741,   744,   747,   750,   753,   755,   759,
   764,   765,   769,   772,   774,   779,   783,   786,   788,   790,
   794,   798,   800,   802,   804,   806,   808,   811,   814,   816,
   818,   820,   822,   826,   828,   832,   836,   838,   840,   844,
   846,   849,   850,   853,   856,   861,   862,   870,   873,   875,
   878,   879,   884,   889,   893,   894,   897,   898,   902,   903,
   905,   911,   915,   919,   921,   925,   932,   939,   947,   953,
   959,   968,   977,   987,   995,  1002,  1005,  1008,  1011,  1014,
  1017,  1020,  1023,  1026,  1029,  1032,  1033,  1038,  1043,  1048,
  1049,  1057
};

static const short yyrhs[] = {   116,
   117,     0,     0,   106,   116,   107,     0,     0,    54,   108,
   199,   109,   118,   117,   150,   158,     0,     0,     0,    54,
   108,   199,   109,    21,   119,   116,   154,   160,   120,    57,
   110,     0,     0,     0,    68,   108,   199,   109,   121,   139,
   122,   140,     0,     0,     0,     0,    67,   123,   117,    68,
   124,   108,   199,   109,   110,   125,   142,     0,     0,     0,
     0,     0,     0,    70,   126,   108,   183,   127,   110,   183,
   128,   110,   183,   129,   109,   137,   130,   145,     0,     0,
    72,   108,   199,   109,   131,   138,     0,    76,   110,     0,
    76,   199,   110,     0,    77,   110,     0,    77,   199,   110,
     0,     0,    81,    60,   132,   169,   108,   116,   109,   110,
     0,     0,    82,    60,   108,   133,   169,   109,   106,   116,
   107,     0,    84,   110,     0,    84,   199,   110,     0,    89,
   174,     0,    90,   175,     0,     0,    94,    60,   134,   106,
   177,   107,     0,     0,    94,    60,    95,    60,   135,   106,
   177,   107,     0,    66,   182,   110,     0,    62,     0,   199,
   110,     0,     0,    86,   136,   199,   110,     0,    87,   199,
   110,     0,    88,   108,   199,   109,   110,     0,    88,   108,
   199,     3,   199,   109,   110,     0,   101,   108,   199,   109,
   110,     0,    85,   199,   110,     0,   110,     0,   117,     0,
    21,   116,    71,   110,     0,   106,   162,   107,     0,   106,
   110,   162,   107,     0,    21,   162,    73,   110,     0,    21,
   110,   162,    73,   110,     0,   117,     0,    21,   116,    69,
   110,     0,     0,     0,   140,    78,   108,   199,   109,   141,
   139,     0,     0,     0,     0,   142,    79,   143,   117,    68,
   144,   108,   199,   109,   110,     0,     0,     0,     0,     0,
     0,   145,    80,   146,   108,   183,   147,   110,   183,   148,
   110,   183,   149,   109,   137,     0,     0,     0,     0,     0,
   150,    55,   108,   151,   199,   152,   109,   153,   117,     0,
     0,     0,     0,     0,   154,    55,   108,   155,   199,   156,
   109,    21,   157,   116,     0,     0,     0,    56,   159,   117,
     0,     0,     0,    56,    21,   161,   116,     0,     0,     0,
     0,    74,   199,    21,   163,   116,   164,   162,     0,     0,
    75,    21,   165,   116,     0,     0,     0,    74,   199,   110,
   166,   116,   167,   162,     0,     0,    75,   110,   168,   116,
     0,     0,   170,   171,     0,     0,   111,   198,     0,    26,
   111,   198,     0,    83,   111,   198,     0,   111,   198,     8,
   197,     0,   171,     3,   111,   198,     0,   171,     3,    26,
   111,   198,     0,   171,     3,    83,   111,   198,     0,   171,
     3,   111,   198,     8,   197,     0,   173,     0,     0,   184,
     0,   204,     0,    26,   204,     0,   173,     3,   184,     0,
   173,     3,   204,     0,   173,     3,    26,   204,     0,   174,
     3,   111,   206,     0,   111,   206,     0,   175,     3,   111,
   206,     0,   175,     3,   111,   206,     8,   176,     0,   111,
   206,     0,   111,   206,     8,   176,     0,   197,     0,   108,
   199,   109,     0,   177,   178,     0,     0,   100,   181,   110,
     0,     0,    81,    60,   179,   169,   108,   116,   109,   110,
     0,     0,    82,    60,   108,   180,   169,   109,   106,   116,
   107,     0,   181,     3,   111,   198,     0,   181,     3,   111,
   198,     8,   199,     0,   111,   198,     0,   111,   198,     8,
   199,     0,     0,   182,     3,   199,     0,   199,     0,     0,
   183,     3,   199,     0,   199,     0,    98,   108,   203,   109,
     8,   199,     0,   204,     8,   199,     0,   204,     9,   199,
     0,   204,    10,   199,     0,   204,    11,   199,     0,   204,
    12,   199,     0,   204,    13,   199,     0,   204,    14,   199,
     0,   204,    15,   199,     0,   204,    16,   199,     0,   204,
    17,   199,     0,   204,    18,   199,     0,   204,    19,   199,
     0,   204,    43,     0,    43,   204,     0,   204,    44,     0,
    44,   204,     0,     0,   199,    22,   185,   199,     0,     0,
   199,    23,   186,   199,     0,     0,   199,     4,   187,   199,
     0,     0,   199,     6,   188,   199,     0,   199,     5,   199,
     0,   199,    24,   199,     0,   199,    25,   199,     0,   199,
    26,   199,     0,   199,    37,   199,     0,   199,    35,   199,
     0,   199,    36,   199,     0,   199,    38,   199,     0,   199,
    39,   199,     0,   199,    40,   199,     0,   199,    33,   199,
     0,   199,    34,   199,     0,    35,   199,     0,    36,   199,
     0,    41,   199,     0,    42,   199,     0,   199,    27,   199,
     0,   199,    28,   199,     0,   199,    29,   199,     0,   199,
    30,   199,     0,   199,    31,   199,     0,   199,    32,   199,
     0,   108,   199,   109,     0,   108,   199,     1,     0,     0,
     0,   199,    20,   189,   199,    21,   190,   199,     0,     0,
     0,   206,   191,   108,   172,   109,   192,   216,     0,   215,
     0,     0,     0,   111,   202,    96,   206,   193,   108,   172,
   109,   194,   216,     0,    52,   206,     0,    45,   199,     0,
    46,   199,     0,    47,   199,     0,    48,   199,     0,    49,
   199,     0,    53,     0,    53,   108,   109,     0,    53,   108,
   199,   109,     0,     0,    50,   195,   199,     0,    50,     1,
     0,   196,     0,    99,   108,   212,   109,     0,   112,   214,
   112,     0,     7,   199,     0,    58,     0,    59,     0,   113,
   214,   113,     0,   114,   214,   114,     0,   105,     0,    60,
     0,   103,     0,   104,     0,   196,     0,    35,   197,     0,
    36,   197,     0,    60,     0,   204,     0,   184,     0,   206,
     0,   106,   199,   107,     0,   198,     0,   106,   199,   107,
     0,   202,    96,   198,     0,   207,     0,   198,     0,   203,
     3,   204,     0,   204,     0,   203,     3,     0,     0,   111,
   200,     0,   111,   207,     0,   111,   202,    96,   200,     0,
     0,   111,   202,    96,   201,    51,   205,   209,     0,   111,
   200,     0,   198,     0,   111,   207,     0,     0,   201,    51,
   208,   209,     0,   209,    51,   199,   115,     0,   209,    51,
   115,     0,     0,   210,   115,     0,     0,   211,   199,   115,
     0,     0,   213,     0,   213,     3,   199,    97,   199,     0,
   213,     3,   199,     0,   199,    97,   199,     0,   199,     0,
   214,   111,    60,     0,   214,   111,    60,    51,    60,   115,
     0,   214,   111,    60,    51,    61,   115,     0,   214,   111,
    60,    51,   111,    60,   115,     0,   214,   111,    60,    96,
    60,     0,   214,   111,   106,    60,   107,     0,   214,   111,
   106,    60,    51,    60,   115,   107,     0,   214,   111,   106,
    60,    51,    61,   115,   107,     0,   214,   111,   106,    60,
    51,   111,    60,   115,   107,     0,   214,   111,   106,    60,
    96,    60,   107,     0,   214,   111,   106,   111,    60,   107,
     0,   214,    60,     0,   214,    61,     0,   214,    65,     0,
   214,    63,     0,   214,    64,     0,   214,    51,     0,   214,
   115,     0,   214,   106,     0,   214,   107,     0,   214,    96,
     0,     0,    91,   108,   204,   109,     0,    92,   108,   204,
   109,     0,    93,   108,   204,   109,     0,     0,    81,    60,
   169,   108,   116,   109,   110,     0,    82,    60,   108,   169,
   109,   106,   116,   107,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   266,   268,   272,   274,   274,   275,   275,   275,   276,   276,
   277,   277,   277,   277,   278,   278,   279,   280,   281,   282,
   283,   283,   283,   284,   285,   286,   287,   288,   289,   290,
   291,   292,   293,   294,   295,   296,   296,   297,   297,   298,
   299,   300,   301,   301,   302,   303,   304,   305,   306,   307,
   311,   313,   316,   318,   319,   320,   323,   325,   328,   330,
   330,   334,   336,   336,   336,   340,   342,   343,   344,   345,
   346,   350,   352,   353,   353,   354,   357,   359,   360,   360,
   361,   364,   366,   367,   370,   372,   373,   376,   378,   378,
   379,   379,   379,   380,   380,   381,   381,   381,   385,   387,
   387,   391,   393,   394,   395,   396,   397,   398,   399,   403,
   405,   409,   411,   412,   413,   414,   415,   418,   420,   424,
   426,   427,   428,   433,   435,   439,   441,   445,   447,   448,
   449,   450,   455,   457,   458,   459,   463,   465,   466,   470,
   472,   473,   477,   479,   480,   481,   482,   483,   484,   485,
   486,   487,   488,   489,   490,   491,   492,   493,   494,   495,
   495,   496,   496,   497,   497,   498,   498,   499,   500,   501,
   502,   503,   504,   505,   506,   507,   508,   509,   510,   511,
   512,   513,   514,   515,   516,   517,   518,   519,   520,   521,
   522,   523,   524,   525,   526,   527,   528,   529,   530,   531,
   532,   533,   534,   535,   536,   537,   538,   539,   540,   541,
   542,   542,   543,   544,   545,   546,   547,   551,   553,   554,
   555,   556,   557,   558,   559,   562,   564,   565,   568,   574,
   576,   580,   582,   586,   588,   593,   595,   596,   599,   607,
   616,   623,   637,   639,   640,   641,   641,   645,   647,   648,
   651,   652,   656,   658,   659,   659,   660,   660,   664,   666,
   669,   671,   672,   673,   677,   679,   680,   681,   682,   683,
   684,   685,   686,   687,   688,   689,   690,   691,   692,   693,
   694,   695,   696,   697,   698,   699,   703,   705,   706,   710,
   712,   713
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","','","LOGICAL_OR",
"LOGICAL_XOR","LOGICAL_AND","PHP_PRINT","'='","PLUS_EQUAL","MINUS_EQUAL","MUL_EQUAL",
"DIV_EQUAL","CONCAT_EQUAL","MOD_EQUAL","AND_EQUAL","OR_EQUAL","XOR_EQUAL","SHIFT_LEFT_EQUAL",
"SHIFT_RIGHT_EQUAL","'?'","':'","BOOLEAN_OR","BOOLEAN_AND","'|'","'^'","'&'",
"IS_EQUAL","IS_NOT_EQUAL","'<'","IS_SMALLER_OR_EQUAL","'>'","IS_GREATER_OR_EQUAL",
"SHIFT_LEFT","SHIFT_RIGHT","'+'","'-'","'.'","'*'","'/'","'%'","'!'","'~'","INCREMENT",
"DECREMENT","INT_CAST","DOUBLE_CAST","STRING_CAST","ARRAY_CAST","OBJECT_CAST",
"'@'","'['","NEW","EXIT","IF","ELSEIF","ELSE","ENDIF","LNUMBER","DNUMBER","STRING",
"NUM_STRING","INLINE_HTML","CHARACTER","BAD_CHARACTER","ENCAPSED_AND_WHITESPACE",
"PHP_ECHO","DO","WHILE","ENDWHILE","FOR","ENDFOR","SWITCH","ENDSWITCH","CASE",
"DEFAULT","BREAK","CONTINUE","CONTINUED_WHILE","CONTINUED_DOWHILE","CONTINUED_FOR",
"OLD_FUNCTION","FUNCTION","PHP_CONST","RETURN","INCLUDE","REQUIRE","HIGHLIGHT_FILE",
"HIGHLIGHT_STRING","PHP_GLOBAL","PHP_STATIC","PHP_UNSET","PHP_ISSET","PHP_EMPTY",
"CLASS","EXTENDS","PHP_CLASS_OPERATOR","PHP_DOUBLE_ARROW","PHP_LIST","PHP_ARRAY",
"VAR","EVAL","DONE_EVAL","PHP_LINE","PHP_FILE","STRING_CONSTANT","'{'","'}'",
"'('","')'","';'","'$'","'`'","'\"'","'''","']'","statement_list","statement",
"@1","@2","@3","@4","@5","@6","@7","@8","@9","@10","@11","@12","@13","@14","@15",
"@16","@17","@18","@19","for_statement","switch_case_list","while_statement",
"while_iterations","@20","do_while_iterations","@21","@22","for_iterations",
"@23","@24","@25","@26","elseif_list","@27","@28","@29","new_elseif_list","@30",
"@31","@32","else_single","@33","new_else_single","@34","case_list","@35","@36",
"@37","@38","@39","@40","parameter_list","@41","non_empty_parameter_list","function_call_parameter_list",
"non_empty_function_call_parameter_list","global_var_list","static_var_list",
"unambiguous_static_assignment","class_statement_list","class_statement","@42",
"@43","class_variable_decleration","echo_expr_list","for_expr","expr_without_variable",
"@44","@45","@46","@47","@48","@49","@50","@51","@52","@53","@54","scalar","signed_scalar",
"varname_scalar","expr","unambiguous_variable_name","unambiguous_array_name",
"unambiguous_class_name","assignment_list","assignment_variable_pointer","@55",
"var","multi_dimensional_array","@56","dimensions","@57","@58","array_pair_list",
"non_empty_array_pair_list","encaps_list","internal_functions_in_yacc","possible_function_call", NULL
};
#endif

static const short yyr1[] = {     0,
   116,   116,   117,   118,   117,   119,   120,   117,   121,   122,
   117,   123,   124,   125,   117,   126,   127,   128,   129,   130,
   117,   131,   117,   117,   117,   117,   117,   132,   117,   133,
   117,   117,   117,   117,   117,   134,   117,   135,   117,   117,
   117,   117,   136,   117,   117,   117,   117,   117,   117,   117,
   137,   137,   138,   138,   138,   138,   139,   139,   140,   141,
   140,   142,   143,   144,   142,   145,   146,   147,   148,   149,
   145,   150,   151,   152,   153,   150,   154,   155,   156,   157,
   154,   158,   159,   158,   160,   161,   160,   162,   163,   164,
   162,   165,   162,   166,   167,   162,   168,   162,   170,   169,
   169,   171,   171,   171,   171,   171,   171,   171,   171,   172,
   172,   173,   173,   173,   173,   173,   173,   174,   174,   175,
   175,   175,   175,   176,   176,   177,   177,   178,   179,   178,
   180,   178,   181,   181,   181,   181,   182,   182,   182,   183,
   183,   183,   184,   184,   184,   184,   184,   184,   184,   184,
   184,   184,   184,   184,   184,   184,   184,   184,   184,   185,
   184,   186,   184,   187,   184,   188,   184,   184,   184,   184,
   184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
   184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
   184,   189,   190,   184,   191,   192,   184,   184,   193,   194,
   184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
   195,   184,   184,   184,   184,   184,   184,   196,   196,   196,
   196,   196,   196,   196,   196,   197,   197,   197,   198,   199,
   199,   200,   200,   201,   201,   202,   202,   202,   203,   203,
   203,   203,   204,   204,   204,   205,   204,   206,   206,   206,
   208,   207,   209,   209,   210,   209,   211,   209,   212,   212,
   213,   213,   213,   213,   214,   214,   214,   214,   214,   214,
   214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
   214,   214,   214,   214,   214,   214,   215,   215,   215,   216,
   216,   216
};

static const short yyr2[] = {     0,
     2,     0,     3,     0,     8,     0,     0,    12,     0,     0,
     8,     0,     0,     0,    11,     0,     0,     0,     0,     0,
    15,     0,     6,     2,     3,     2,     3,     0,     8,     0,
     9,     2,     3,     2,     2,     0,     6,     0,     8,     3,
     1,     2,     0,     4,     3,     5,     7,     5,     3,     1,
     1,     4,     3,     4,     4,     5,     1,     4,     0,     0,
     7,     0,     0,     0,    10,     0,     0,     0,     0,     0,
    14,     0,     0,     0,     0,     9,     0,     0,     0,     0,
    10,     0,     0,     3,     0,     0,     4,     0,     0,     0,
     7,     0,     4,     0,     0,     7,     0,     4,     0,     2,
     0,     2,     3,     3,     4,     4,     5,     5,     6,     1,
     0,     1,     1,     2,     3,     3,     4,     4,     2,     4,
     6,     2,     4,     1,     3,     2,     0,     3,     0,     8,
     0,     9,     4,     6,     2,     4,     0,     3,     1,     0,
     3,     1,     6,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     2,     2,     2,     2,     0,
     4,     0,     4,     0,     4,     0,     4,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
     2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
     3,     0,     0,     7,     0,     0,     7,     1,     0,     0,
    10,     2,     2,     2,     2,     2,     2,     1,     3,     4,
     0,     3,     2,     1,     4,     3,     2,     1,     1,     3,
     3,     1,     1,     1,     1,     1,     2,     2,     1,     1,
     1,     1,     3,     1,     3,     3,     1,     1,     3,     1,
     2,     0,     2,     2,     4,     0,     7,     2,     1,     2,
     0,     4,     4,     3,     0,     2,     0,     3,     0,     1,
     5,     3,     3,     1,     3,     6,     6,     7,     5,     5,
     8,     8,     9,     7,     6,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     0,     4,     4,     4,     0,
     7,     8
};

static const short yydefact[] = {     2,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   208,     0,   218,   219,   223,
    41,   137,    12,     0,    16,     0,     0,     0,     0,     0,
     0,     0,    43,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   224,   225,   222,     2,     0,    50,
     0,   286,   286,   286,     1,   231,   214,   249,     0,   230,
   195,   198,   217,   180,   181,   182,   183,     0,   157,   159,
   203,   204,   205,   206,   207,   213,     0,   229,     0,   202,
     0,     0,     0,   139,     0,     0,     0,     0,    24,     0,
    26,     0,    28,     0,    32,     0,     0,     0,     0,     0,
     0,    34,     0,    35,     0,     0,     0,    36,   242,   259,
     0,     0,     0,     0,   249,   243,     0,     0,   232,   244,
     0,     0,     0,   164,     0,   166,   192,   160,   162,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    42,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   156,
   158,     0,   243,     0,   244,   212,   249,   248,   250,   209,
     0,     0,     0,    40,     0,     0,   140,     0,    25,    27,
    99,    30,    33,    49,     0,    45,     0,   119,     0,   122,
     0,     0,     0,     0,     0,     0,     0,   240,   264,     0,
   260,     0,     3,   191,   190,     0,   251,     0,   281,   276,
   277,   279,   280,   278,   285,   283,   284,     0,   216,   282,
   220,   221,     0,   168,     0,     0,     0,     0,   169,   170,
   171,   184,   185,   186,   187,   188,   189,   178,   179,   173,
   174,   172,   175,   176,   177,   144,   145,   146,   147,   148,
   149,   150,   151,   152,   153,   154,   155,   111,     0,   210,
     4,   138,    13,     9,    17,   142,    22,     0,     0,    99,
    44,     0,     0,     0,     0,     0,   287,   288,   289,    38,
   127,   241,     0,     0,   215,     0,     0,   233,   257,   249,
   245,     0,   232,   265,     0,   165,   167,     0,   161,   163,
     0,     0,   110,   231,     0,   230,     6,     0,     0,     0,
     0,     0,     0,     2,     0,     0,     0,   100,     0,     0,
    46,   118,     0,     0,   223,     0,   123,   226,   124,   120,
     0,     0,   239,     0,   263,   262,    48,   252,     0,     0,
   246,     0,     0,     0,     0,     0,   193,   114,   196,     0,
     2,    72,     0,     2,    57,    10,   141,   140,    88,    88,
    23,     0,     0,     0,   102,     0,     0,     0,   227,   228,
     0,     0,   127,     0,     0,     0,    37,   126,   143,     0,
     0,   256,     0,   257,   111,     0,     0,     0,   269,     0,
     0,   270,     0,     0,   290,     0,   231,   230,    77,    82,
     0,     0,    59,    18,     0,     0,    88,     0,    88,     0,
     0,   103,   104,     0,     0,     0,     0,     2,    47,   125,
   121,     0,   129,     0,     0,     0,   261,   254,     0,   258,
   247,     0,   266,   267,     0,     0,     0,     0,     0,   275,
   194,     0,     0,   197,   117,    85,     0,    83,     5,     0,
     0,    11,     0,     0,    92,    97,     0,     0,     0,    53,
    29,   105,     0,     0,   106,     0,    39,    99,   131,   135,
     0,   128,   253,   200,   268,     0,     0,     0,   274,    99,
     0,     0,     0,     7,    73,     0,    14,    58,     0,   140,
    89,    94,     2,     2,     0,    55,    54,   107,   108,     0,
    31,     0,    99,     0,     0,   290,   271,   272,     0,     0,
    99,    78,    86,     0,     0,    84,    62,     0,    19,     2,
     2,    93,    98,    56,   109,     2,     0,   136,   133,   201,
   273,     2,     0,     0,     2,     0,    74,    15,     0,     0,
    90,    95,     0,     0,     0,     0,     0,    79,    87,     8,
     0,    63,    60,     0,    88,    88,     0,     2,   134,     0,
     2,     0,    75,     0,     0,     2,    51,    20,    91,    96,
   130,     0,   291,     0,     0,     0,     0,    61,     0,    66,
   132,   292,    80,    76,    64,     0,    21,     2,     0,    52,
    67,    81,     0,     0,     0,   140,     0,    68,    65,     0,
   140,    69,     0,   140,    70,     0,     0,    71,     0,     0
};

static const short yydefgoto[] = {     1,
    55,   308,   351,   514,   310,   403,    85,   309,   517,    87,
   312,   453,   540,   580,   313,   181,   270,   196,   331,    98,
   568,   361,   356,   452,   565,   538,   564,   589,   587,   594,
   600,   603,   606,   400,   515,   551,   576,   446,   534,   562,
   588,   449,   486,   484,   535,   408,   520,   555,   493,   521,
   556,   494,   268,   269,   318,   302,   303,   102,   104,   327,
   332,   378,   468,   503,   426,    83,   265,    56,   227,   228,
   223,   225,   226,   394,   162,   395,   342,   506,    77,    57,
   329,    58,    59,   291,   117,   118,   197,    60,   384,    61,
   120,   289,   338,   339,   340,   200,   201,   121,    62,   444
};

static const short yypact[] = {-32768,
   562,  2161,  2161,  2161,  2161,  2161,   -80,   -80,  2161,  2161,
  2161,  2161,  2161,   657,   -33,   -57,   -45,-32768,-32768,   -40,
-32768,  2161,-32768,   -31,-32768,    -5,  1828,  1854,    47,    53,
  1936,  2161,-32768,  2161,    12,    51,    59,    56,    70,    76,
   125,    78,    96,   104,-32768,-32768,-32768,-32768,  2161,-32768,
   -37,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  2310,   669,
-32768,-32768,  3322,   134,   134,-32768,-32768,   -37,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  2161,-32768,   -37,-32768,
  1962,  2161,    16,  3230,  1740,  2161,   107,  2161,-32768,  2361,
-32768,  2399,-32768,   108,-32768,  2450,  2488,  2161,  2539,  2161,
   -33,   214,   -33,   215,   -80,   -80,   -80,   142,   -80,  2161,
  2161,   765,   548,  2161,   -11,   112,   189,   145,-32768,   -47,
   256,  2351,  2440,-32768,  2161,-32768,-32768,-32768,-32768,  2161,
  2161,  2161,  2161,  2161,  2161,  2161,  2161,  2161,  2161,  2161,
  2161,  2161,  2161,  2161,  2161,  2161,-32768,  2161,  2161,  2161,
  2161,  2161,  2161,  2161,  2161,  2161,  2161,  2161,  2161,-32768,
-32768,   138,-32768,   146,   152,-32768,   203,-32768,-32768,-32768,
  2628,  2666,  2161,-32768,   187,  2716,  2161,  2754,-32768,-32768,
   149,-32768,-32768,-32768,  2577,-32768,  2273,-32768,   148,   250,
   153,   154,   156,   157,   208,   163,    18,-32768,  3106,   165,
   269,  2804,-32768,-32768,-32768,  3068,-32768,   -37,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -19,-32768,-32768,
-32768,-32768,  2161,  3301,  2161,  2161,  2161,  2161,  3392,  3407,
  2324,  2591,  2591,  1381,  1381,  1381,  1381,   304,   304,   134,
   134,   134,-32768,-32768,-32768,  3322,  3322,  3322,  3322,  3322,
  3322,  3322,  3322,  3322,  3322,  3322,  3322,  2051,   -37,-32768,
   254,  3230,-32768,-32768,   273,  3230,-32768,   172,     8,   174,
-32768,  2161,   175,   -33,   130,   -33,-32768,-32768,-32768,-32768,
-32768,   -80,   278,  2161,-32768,  2161,   177,   239,   191,     4,
-32768,   241,   204,     5,   -14,  3266,  3322,  3191,  3359,  3376,
   -80,   205,   310,    49,  3230,   184,-32768,  1740,   207,   853,
  2161,   212,    55,-32768,   216,   217,   258,   320,   222,  2842,
-32768,-32768,   147,   147,-32768,  2161,-32768,-32768,-32768,   317,
   220,   -17,-32768,  2161,  3230,  3154,-32768,   282,   221,  2161,
-32768,   229,   -18,   285,   -15,   287,-32768,-32768,-32768,  2081,
-32768,-32768,  2161,-32768,-32768,-32768,  3230,  2161,    -8,    14,
-32768,   941,   258,   258,   338,    11,   243,   240,-32768,-32768,
  2892,   130,-32768,   293,   294,   244,-32768,-32768,  3322,  2161,
   683,-32768,   393,   191,  2051,   242,   245,   298,-32768,   -16,
   301,-32768,   257,  2161,   -24,   -80,    50,   286,  1740,   113,
  2930,  1029,-32768,   273,  2161,     7,    36,   292,    36,   259,
   260,-32768,-32768,   147,   261,   262,   258,-32768,-32768,-32768,
-32768,   132,-32768,   266,   258,    35,  3230,-32768,   432,-32768,
   282,   267,-32768,-32768,   265,   270,   271,   309,   274,-32768,
  3341,   322,   324,-32768,-32768,   121,   279,-32768,-32768,   280,
   281,   299,   283,  2211,-32768,-32768,   316,   290,   295,-32768,
-32768,-32768,   258,   258,   396,  1117,-32768,   149,-32768,   398,
   297,-32768,-32768,-32768,-32768,   302,   303,   296,-32768,   149,
   306,   331,   380,-32768,-32768,  1740,-32768,-32768,   332,  2161,
-32768,-32768,-32768,-32768,   333,-32768,-32768,-32768,-32768,   147,
-32768,   334,   174,  2161,   258,   -24,-32768,-32768,   305,   336,
   174,-32768,-32768,   384,  2161,-32768,-32768,  2161,   273,-32768,
-32768,  1740,  1740,-32768,-32768,-32768,   337,  3230,   437,-32768,
-32768,-32768,   339,  2161,-32768,   343,  3230,   368,  2980,   340,
  1740,  1740,  1205,   367,  2161,  1293,   373,  3230,  1740,-32768,
   365,-32768,-32768,  1388,    36,    36,   370,-32768,  3230,   371,
-32768,   375,-32768,  1740,   853,-32768,-32768,-32768,-32768,-32768,
-32768,  1476,-32768,  1564,   461,  1740,   419,-32768,  1652,-32768,
-32768,-32768,-32768,-32768,-32768,   381,   410,-32768,   385,-32768,
-32768,  1740,  2161,   386,  3018,  2161,   388,   273,-32768,   389,
  2161,   273,   390,  2161,   273,   387,  1388,-32768,   495,-32768
};

static const short yypgoto[] = {   -43,
   -79,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
  -106,-32768,   -62,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,  -347,-32768,-32768,-32768,-32768,
-32768,-32768,  -258,-32768,-32768,   119,-32768,-32768,-32768,   133,
   136,-32768,-32768,-32768,-32768,-32768,  -323,  -225,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -251,
  -309,   -29,    -2,   168,  -160,   438,-32768,     9,-32768,     3,
    -9,-32768,   123,-32768,-32768,-32768,-32768,   127,-32768,     6
};


#define	YYLAST		3447


static const short yytable[] = {    63,
    64,    65,    66,    67,   112,   175,    71,    72,    73,    74,
    75,   319,   410,   369,   370,    69,    70,    80,   173,    84,
   282,   115,    78,   328,    90,    92,    78,   455,    96,    97,
    68,    99,   304,   315,   404,   390,   415,   471,   115,  -234,
   294,   386,   387,   436,   437,   345,   113,   292,  -237,   167,
    81,  -112,  -115,   119,  -234,   343,   442,   443,   165,   457,
  -250,   459,    82,   374,   375,   405,   406,  -229,   114,   169,
   119,   328,   328,    79,   166,   359,    86,    79,   171,   172,
   391,   119,   376,   176,  -238,   178,   295,   405,   406,   377,
   316,   392,   388,   416,   438,   185,   346,   187,   292,  -236,
   344,   407,    88,   188,   462,   190,    93,   199,   202,   405,
   406,   206,    94,   192,   193,   194,   456,   198,   317,   100,
   328,   417,   224,   409,   397,   174,   283,   229,   230,   231,
   232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
   242,   243,   244,   245,   472,   246,   247,   248,   249,   250,
   251,   252,   253,   254,   255,   256,   257,  -112,  -115,   304,
   360,   101,   328,   105,   323,   324,   519,   447,   448,   103,
   262,   144,   145,   146,   266,   482,   483,   106,   290,   122,
   123,   323,   324,   107,   108,   109,  -113,    18,    19,   325,
   525,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   158,   159,   110,    18,    19,   325,   569,   570,   502,
   293,   111,   374,   375,   177,   182,   189,   191,   116,  -248,
   296,   510,   297,   298,   299,   300,   160,   161,   352,   290,
   355,   376,    45,    46,    47,   163,   195,   326,   467,   207,
   208,   259,    53,    54,   527,   258,   168,  -237,   328,    45,
    46,    47,   533,  -234,   263,   305,  -101,   275,   274,    53,
    54,   119,   277,   276,   278,   279,   306,   280,   281,   320,
   362,   286,   598,   285,   307,   311,   322,   602,   330,   314,
   605,   335,  -101,   336,   321,   334,   337,   365,  -116,  -235,
   333,   341,  -113,   148,   149,   150,   151,   152,   153,   154,
   155,   156,   157,   158,   159,  -255,   209,   399,   357,   348,
   402,  -199,   350,   349,   353,   210,   211,    78,   212,   213,
   214,   358,   366,   371,   372,   373,   363,   364,   160,   161,
   367,   379,   381,   412,   413,   382,   385,   383,   141,   142,
   143,   144,   145,   146,   389,   414,   393,   305,   418,   419,
   401,   215,   423,   424,   425,   266,   433,   435,   398,   434,
   439,   216,   217,   440,   458,   460,   218,   219,   478,   461,
   220,   463,   464,   469,   466,   474,   489,   427,   429,   475,
   479,   480,   305,   481,   476,   477,   485,   465,   495,   487,
   488,   441,   490,   306,  -116,   470,   124,   125,   126,   496,
   513,   497,   454,   500,   445,   504,   516,   505,   507,   508,
   509,   531,   127,   511,   128,   129,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   498,   499,   124,   125,   126,   512,   518,
   536,   526,   524,   532,   545,   544,   552,   547,   554,   522,
   523,   127,   550,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   558,   563,   567,   529,   541,   542,   561,   571,
   573,   583,   543,   575,   577,   355,   585,   266,   546,   591,
   590,   549,   593,   596,   610,   607,   584,   599,   601,   604,
   608,   528,   578,   432,   421,   164,   431,   430,   422,     0,
     0,   530,   537,     0,   572,   539,     0,   574,     0,     0,
     0,     0,   579,     0,     0,     0,     0,   567,     0,     0,
     0,   548,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   559,     0,   592,     0,   473,     0,   204,     0,
     0,   124,   125,   126,     0,     0,     0,     0,     0,     0,
     0,   609,     0,     0,     0,     0,     0,   127,     2,   128,
   129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,     0,     0,
   595,     0,     0,   266,     0,     0,     3,     4,   266,     0,
     0,   266,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,     0,    15,    16,    17,     0,     0,     0,    18,
    19,    20,     0,    21,     0,     0,     0,    22,    23,    24,
     0,    25,     0,    26,     0,     0,     0,    27,    28,     0,
     0,     0,    29,    30,     0,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,   205,    76,     0,    42,
    43,     0,    44,  -211,    45,    46,    47,    48,     0,    49,
     0,    50,    51,    52,    53,    54,   148,   149,   150,   151,
   152,   153,   154,   155,   156,   157,   158,   159,     0,     2,
     0,  -211,  -211,     0,     0,     0,     0,  -211,  -211,  -211,
  -211,  -211,  -211,  -211,  -211,  -211,  -211,     0,  -211,  -211,
     0,   160,   161,     0,  -211,  -211,  -211,     3,     4,     0,
     0,     0,     0,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,     0,    15,    16,     0,     0,     0,     0,
    18,    19,    20,     0,     0,     0,     0,  -211,  -211,  -211,
     0,     0,     0,     0,  -211,  -211,     0,     0,     0,  -211,
  -211,  -211,     0,     0,  -211,     0,     0,  -211,  -211,  -211,
  -211,     2,     0,    38,    39,    40,     0,     0,     0,     0,
    42,    43,     0,     0,     0,    45,    46,    47,     0,     0,
    49,     0,     0,    51,    52,    53,    54,   428,     0,     3,
     4,     0,     0,     0,     0,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,     0,    15,    16,    17,     0,
     0,     0,    18,    19,    20,     0,    21,     0,     0,     0,
    22,    23,    24,     0,    25,     0,    26,     0,     0,     0,
    27,    28,     0,     0,     0,    29,    30,     0,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,     2,
     0,     0,    42,    43,     0,    44,     0,    45,    46,    47,
    48,   203,    49,   354,    50,    51,    52,    53,    54,     0,
     0,     0,     0,     0,     0,     0,     0,     3,     4,     0,
     0,     0,     0,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,     0,    15,    16,    17,     0,     0,     0,
    18,    19,    20,     0,    21,     0,     0,     0,    22,    23,
    24,     0,    25,     0,    26,     0,     0,     0,    27,    28,
     0,     0,     0,    29,    30,     0,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,     2,     0,     0,
    42,    43,     0,    44,     0,    45,    46,    47,    48,     0,
    49,     0,    50,    51,    52,    53,    54,     0,     0,     0,
     0,     0,     0,     0,     0,     3,     4,     0,     0,     0,
     0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,     0,    15,    16,    17,     0,     0,     0,    18,    19,
    20,     0,    21,     0,     0,     0,    22,    23,    24,     0,
    25,     0,    26,     0,     0,     0,    27,    28,     0,     0,
     0,    29,    30,     0,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,     2,     0,     0,    42,    43,
     0,    44,     0,    45,    46,    47,    48,     0,    49,   411,
    50,    51,    52,    53,    54,     0,     0,     0,     0,     0,
     0,     0,     0,     3,     4,     0,     0,     0,     0,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,     0,
    15,    16,    17,     0,     0,     0,    18,    19,    20,     0,
    21,     0,     0,     0,    22,    23,    24,   451,    25,     0,
    26,     0,     0,     0,    27,    28,     0,     0,     0,    29,
    30,     0,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    41,     2,     0,     0,    42,    43,     0,    44,
     0,    45,    46,    47,    48,     0,    49,     0,    50,    51,
    52,    53,    54,     0,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     0,     0,     0,     0,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,     0,    15,    16,
    17,     0,     0,     0,    18,    19,    20,     0,    21,     0,
     0,     0,    22,    23,    24,     0,    25,     0,    26,     0,
     0,     0,    27,    28,     0,     0,     0,    29,    30,     0,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,     2,     0,     0,    42,    43,     0,    44,     0,    45,
    46,    47,    48,   501,    49,     0,    50,    51,    52,    53,
    54,     0,     0,     0,     0,     0,     0,     0,     0,     3,
     4,     0,     0,     0,     0,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,     0,    15,    16,    17,     0,
     0,     0,    18,    19,    20,     0,    21,     0,     0,     0,
    22,    23,    24,     0,    25,     0,    26,     0,     0,     0,
    27,    28,     0,     0,     0,    29,    30,     0,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,     2,
     0,     0,    42,    43,     0,    44,     0,    45,    46,    47,
    48,     0,    49,   557,    50,    51,    52,    53,    54,     0,
     0,     0,     0,     0,     0,     0,     0,     3,     4,     0,
     0,     0,     0,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,     0,    15,    16,    17,     0,     0,     0,
    18,    19,    20,     0,    21,     0,     0,     0,    22,    23,
    24,     0,    25,     0,    26,     0,     0,     0,    27,    28,
     0,     0,     0,    29,    30,     0,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,     0,     0,     0,
    42,    43,     0,    44,     2,    45,    46,    47,    48,     0,
    49,   560,    50,    51,    52,    53,    54,     0,   566,-32768,
-32768,-32768,-32768,   139,   140,   141,   142,   143,   144,   145,
   146,     0,     3,     4,     0,     0,     0,     0,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,    17,     0,     0,     0,    18,    19,    20,     0,    21,
     0,     0,     0,    22,    23,    24,     0,    25,     0,    26,
     0,     0,     0,    27,    28,     0,     0,     0,    29,    30,
     0,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,     2,     0,     0,    42,    43,     0,    44,     0,
    45,    46,    47,    48,     0,    49,     0,    50,    51,    52,
    53,    54,     0,     0,     0,     0,     0,     0,     0,     0,
     3,     4,     0,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,    17,
     0,     0,     0,    18,    19,    20,     0,    21,     0,     0,
     0,    22,    23,    24,     0,    25,     0,    26,     0,     0,
     0,    27,    28,     0,     0,     0,    29,    30,     0,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
     2,     0,     0,    42,    43,     0,    44,     0,    45,    46,
    47,    48,   581,    49,     0,    50,    51,    52,    53,    54,
     0,     0,     0,     0,     0,     0,     0,     0,     3,     4,
     0,     0,     0,     0,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,    17,     0,     0,
     0,    18,    19,    20,     0,    21,     0,     0,     0,    22,
    23,    24,     0,    25,     0,    26,     0,     0,     0,    27,
    28,     0,     0,     0,    29,    30,     0,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,     2,     0,
     0,    42,    43,     0,    44,     0,    45,    46,    47,    48,
   582,    49,     0,    50,    51,    52,    53,    54,     0,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     0,     0,
     0,     0,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,     0,    15,    16,    17,     0,     0,     0,    18,
    19,    20,     0,    21,     0,     0,     0,    22,    23,    24,
     0,    25,   586,    26,     0,     0,     0,    27,    28,     0,
     0,     0,    29,    30,     0,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,     2,     0,     0,    42,
    43,     0,    44,     0,    45,    46,    47,    48,     0,    49,
     0,    50,    51,    52,    53,    54,     0,     0,     0,     0,
     0,     0,     0,     0,     3,     4,     0,     0,     0,     0,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     0,    15,    16,    17,     0,     0,     0,    18,    19,    20,
     0,    21,     0,     0,     0,    22,    23,    24,     0,    25,
     0,    26,     0,     0,     0,    27,    28,     0,     0,     0,
    29,    30,     0,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,     2,     0,     0,    42,    43,     0,
    44,     0,    45,    46,    47,    48,     0,    49,     0,    50,
    51,    52,    53,    54,     0,     0,     0,     0,     0,     0,
     2,     0,     3,     4,     0,     0,     0,     0,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,     0,     0,     0,     0,    18,    19,    20,     3,     4,
     0,     0,     0,     0,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,     0,     0,     0,
     0,    18,    19,    20,     0,     0,     0,     0,    38,    39,
    40,     0,     0,     0,     0,    42,    43,     0,     0,     0,
    45,    46,    47,     0,     0,    49,     0,    89,    51,    52,
    53,    54,     2,     0,    38,    39,    40,     0,     0,     0,
     0,    42,    43,     0,     0,     0,    45,    46,    47,     0,
     0,    49,     0,    91,    51,    52,    53,    54,     2,     0,
     3,     4,     0,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,     0,
     0,     0,     0,    18,    19,    20,     3,     4,     0,     0,
     0,     0,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,     0,    15,    16,     0,     0,     0,     0,    18,
    19,    20,     0,     0,     0,     0,    38,    39,    40,     0,
     0,     0,     0,    42,    43,     0,     0,     0,    45,    46,
    47,     0,     0,    49,     0,    95,    51,    52,    53,    54,
     0,     0,    38,    39,    40,     0,     0,     2,     0,    42,
    43,     0,     0,     0,    45,    46,    47,     0,     0,    49,
   170,     0,    51,    52,    53,    54,   301,     0,     0,     0,
     0,     0,     0,     0,     0,     3,     4,     2,     0,     0,
     0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,     0,    15,    16,     0,     0,   396,     0,    18,    19,
    20,     0,     0,     0,     0,     3,     4,     0,     0,     0,
     0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,     0,    15,    16,     0,     0,     0,     0,    18,    19,
    20,    38,    39,    40,     0,     0,     0,     0,    42,    43,
     0,     0,     0,    45,    46,    47,     0,     0,    49,     0,
     0,    51,    52,    53,    54,     0,     0,     2,     0,     0,
     0,    38,    39,    40,     0,     0,     0,     0,    42,    43,
     0,     0,     0,    45,    46,    47,     0,     0,    49,     0,
     0,    51,    52,    53,    54,     3,     4,     0,     0,     0,
     0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,     0,    15,    16,   124,   125,   126,     0,    18,    19,
    20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   127,   491,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,    38,    39,    40,     0,     0,     0,     0,    42,    43,
     0,     0,     0,    45,    46,    47,     0,     0,    49,     0,
     0,    51,    52,    53,    54,   272,   124,   125,   126,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   127,     0,   128,   129,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   124,   125,   126,     0,     0,     0,     0,
   492,     0,     0,     0,     0,     0,     0,     0,     0,   127,
     0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   145,   146,   124,   125,   126,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   127,   273,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   209,   124,   125,   126,     0,     0,     0,     0,     0,
   210,   211,     0,   212,   213,   214,     0,     0,   127,   147,
   128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
   138,   139,   140,   141,   142,   143,   144,   145,   146,     0,
     0,     0,     0,     0,     0,     0,   215,     0,     0,     0,
     0,     0,     0,   124,   125,   126,   216,   217,     0,     0,
     0,   218,     0,   221,     0,   220,     0,     0,     0,   127,
   179,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
   209,   124,   125,   126,     0,     0,     0,     0,     0,   210,
   211,     0,   212,   213,   214,     0,     0,   127,   180,   128,
   129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,     0,     0,
     0,     0,     0,     0,     0,   215,     0,     0,     0,     0,
     0,     0,   124,   125,   126,   216,   217,     0,     0,     0,
   218,     0,     0,   222,   220,     0,     0,     0,   127,   183,
   128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
   138,   139,   140,   141,   142,   143,   144,   145,   146,     0,
   124,   125,   126,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   127,   184,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
   140,   141,   142,   143,   144,   145,   146,-32768,-32768,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   124,   125,   126,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   127,   186,   128,
   129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,     0,   124,
   125,   126,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   127,   271,   128,   129,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   124,
   125,   126,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   127,   260,   128,   129,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,     0,   124,   125,   126,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   127,   261,   128,   129,   130,   131,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   145,   146,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   124,   125,   126,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   127,   264,   128,   129,   130,   131,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   145,   146,     0,   124,   125,   126,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   127,   267,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   124,   125,   126,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   127,   287,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,     0,   124,   125,   126,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   127,
   368,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   124,   125,   126,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   127,
   420,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     0,   124,   125,   126,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   127,   450,   128,
   129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   124,   125,   126,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   127,   553,   128,
   129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,     0,   124,
   125,   126,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   127,   597,   128,   129,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   124,   125,   126,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   127,   288,   128,   129,   130,   131,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   145,   146,   124,   125,   126,     0,     0,     0,
     0,     0,   284,     0,     0,     0,     0,     0,     0,     0,
   127,   347,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,     0,     0,   124,   125,   126,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   127,
   380,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
   125,   126,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   127,     0,   128,   129,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,   126,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   127,     0,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   127,     0,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   129,   130,   131,   132,   133,   134,   135,   136,   137,
   138,   139,   140,   141,   142,   143,   144,   145,   146,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   132,   133,   134,   135,   136,   137,   138,   139,
   140,   141,   142,   143,   144,   145,   146
};

static const short yycheck[] = {     2,
     3,     4,     5,     6,    48,    85,     9,    10,    11,    12,
    13,   270,   360,   323,   324,     7,     8,    15,     3,    22,
     3,    51,    60,   275,    27,    28,    60,    21,    31,    32,
   111,    34,   258,    26,   358,    51,    26,     3,    68,    51,
    60,    60,    61,    60,    61,    60,    49,   208,    96,    79,
   108,     3,     3,    51,    51,    51,    81,    82,    68,   407,
   108,   409,   108,    81,    82,    74,    75,   108,   106,    79,
    68,   323,   324,   111,    77,    21,   108,   111,    81,    82,
    96,    79,   100,    86,    96,    88,   106,    74,    75,   107,
    83,   107,   111,    83,   111,    98,   111,   100,   259,    96,
    96,   110,   108,   101,   414,   103,    60,   110,   111,    74,
    75,   114,    60,   105,   106,   107,   110,   109,   111,   108,
   372,   111,   125,   110,   350,   110,   109,   130,   131,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   145,   146,   110,   148,   149,   150,   151,   152,
   153,   154,   155,   156,   157,   158,   159,   109,   109,   385,
   106,   111,   414,   108,    35,    36,   490,    55,    56,   111,
   173,    38,    39,    40,   177,    55,    56,   108,   208,    53,
    54,    35,    36,   108,    60,   108,     3,    58,    59,    60,
   500,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,   108,    58,    59,    60,   555,   556,   468,
   208,   108,    81,    82,   108,   108,     3,     3,    51,   108,
   223,   480,   225,   226,   227,   228,    43,    44,   308,   259,
   310,   100,   103,   104,   105,    68,    95,   108,   107,    51,
    96,    96,   113,   114,   503,   108,    79,    96,   500,   103,
   104,   105,   511,    51,    68,   258,   108,     8,   111,   113,
   114,   259,   109,   111,   109,   109,   258,    60,   106,   272,
   314,     3,   596,   109,    21,     3,   274,   601,   276,   108,
   604,   284,   109,   286,   110,     8,   110,   317,     3,    51,
   282,    51,   109,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    18,    19,   115,    51,   351,   311,   301,
   354,   108,     3,   109,   108,    60,    61,    60,    63,    64,
    65,   110,     3,   326,     8,   106,   111,   111,    43,    44,
   109,   334,    51,   363,   364,   115,   108,   340,    35,    36,
    37,    38,    39,    40,    60,     8,    60,   350,   106,   110,
   353,    96,    60,    60,   111,   358,   115,    60,   350,   115,
    60,   106,   107,   107,    73,   107,   111,   112,    60,   110,
   115,   111,   111,   108,   418,   109,    78,   380,   381,   115,
   107,    60,   385,    60,   115,   115,   108,   417,    73,   110,
   110,   394,   110,   385,   109,   425,     4,     5,     6,   110,
    21,   107,   405,     8,   396,     8,   486,   111,   107,   107,
   115,   107,    20,   108,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,   463,   464,     4,     5,     6,   108,   108,
    57,   108,   110,   108,     8,   109,    79,   109,   109,   493,
   494,    20,   110,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,   106,   109,   554,   505,   520,   521,   106,   110,
   110,    21,   526,   109,   564,   565,    68,   490,   532,    80,
   110,   535,   108,   108,     0,   109,   576,   110,   110,   110,
   607,   504,   565,   385,   372,    68,   384,   115,   373,    -1,
    -1,   506,   515,    -1,   558,   518,    -1,   561,    -1,    -1,
    -1,    -1,   566,    -1,    -1,    -1,    -1,   607,    -1,    -1,
    -1,   534,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,   545,    -1,   588,    -1,   115,    -1,     1,    -1,
    -1,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     0,    -1,    -1,    -1,    -1,    -1,    20,     7,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
   593,    -1,    -1,   596,    -1,    -1,    35,    36,   601,    -1,
    -1,   604,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    -1,    52,    53,    54,    -1,    -1,    -1,    58,
    59,    60,    -1,    62,    -1,    -1,    -1,    66,    67,    68,
    -1,    70,    -1,    72,    -1,    -1,    -1,    76,    77,    -1,
    -1,    -1,    81,    82,    -1,    84,    85,    86,    87,    88,
    89,    90,    91,    92,    93,    94,   109,     1,    -1,    98,
    99,    -1,   101,     7,   103,   104,   105,   106,    -1,   108,
    -1,   110,   111,   112,   113,   114,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    -1,     7,
    -1,    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
    -1,    43,    44,    -1,    58,    59,    60,    35,    36,    -1,
    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    -1,    52,    53,    -1,    -1,    -1,    -1,
    58,    59,    60,    -1,    -1,    -1,    -1,    91,    92,    93,
    -1,    -1,    -1,    -1,    98,    99,    -1,    -1,    -1,   103,
   104,   105,    -1,    -1,   108,    -1,    -1,   111,   112,   113,
   114,     7,    -1,    91,    92,    93,    -1,    -1,    -1,    -1,
    98,    99,    -1,    -1,    -1,   103,   104,   105,    -1,    -1,
   108,    -1,    -1,   111,   112,   113,   114,   115,    -1,    35,
    36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    -1,    52,    53,    54,    -1,
    -1,    -1,    58,    59,    60,    -1,    62,    -1,    -1,    -1,
    66,    67,    68,    -1,    70,    -1,    72,    -1,    -1,    -1,
    76,    77,    -1,    -1,    -1,    81,    82,    -1,    84,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,     7,
    -1,    -1,    98,    99,    -1,   101,    -1,   103,   104,   105,
   106,   107,   108,    21,   110,   111,   112,   113,   114,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,
    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    -1,    52,    53,    54,    -1,    -1,    -1,
    58,    59,    60,    -1,    62,    -1,    -1,    -1,    66,    67,
    68,    -1,    70,    -1,    72,    -1,    -1,    -1,    76,    77,
    -1,    -1,    -1,    81,    82,    -1,    84,    85,    86,    87,
    88,    89,    90,    91,    92,    93,    94,     7,    -1,    -1,
    98,    99,    -1,   101,    -1,   103,   104,   105,   106,    -1,
   108,    -1,   110,   111,   112,   113,   114,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,
    -1,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    -1,    52,    53,    54,    -1,    -1,    -1,    58,    59,
    60,    -1,    62,    -1,    -1,    -1,    66,    67,    68,    -1,
    70,    -1,    72,    -1,    -1,    -1,    76,    77,    -1,    -1,
    -1,    81,    82,    -1,    84,    85,    86,    87,    88,    89,
    90,    91,    92,    93,    94,     7,    -1,    -1,    98,    99,
    -1,   101,    -1,   103,   104,   105,   106,    -1,   108,   109,
   110,   111,   112,   113,   114,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,    -1,    41,
    42,    43,    44,    45,    46,    47,    48,    49,    50,    -1,
    52,    53,    54,    -1,    -1,    -1,    58,    59,    60,    -1,
    62,    -1,    -1,    -1,    66,    67,    68,    69,    70,    -1,
    72,    -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    81,
    82,    -1,    84,    85,    86,    87,    88,    89,    90,    91,
    92,    93,    94,     7,    -1,    -1,    98,    99,    -1,   101,
    -1,   103,   104,   105,   106,    -1,   108,    -1,   110,   111,
   112,   113,   114,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
    54,    -1,    -1,    -1,    58,    59,    60,    -1,    62,    -1,
    -1,    -1,    66,    67,    68,    -1,    70,    -1,    72,    -1,
    -1,    -1,    76,    77,    -1,    -1,    -1,    81,    82,    -1,
    84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
    94,     7,    -1,    -1,    98,    99,    -1,   101,    -1,   103,
   104,   105,   106,   107,   108,    -1,   110,   111,   112,   113,
   114,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
    36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    -1,    52,    53,    54,    -1,
    -1,    -1,    58,    59,    60,    -1,    62,    -1,    -1,    -1,
    66,    67,    68,    -1,    70,    -1,    72,    -1,    -1,    -1,
    76,    77,    -1,    -1,    -1,    81,    82,    -1,    84,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,     7,
    -1,    -1,    98,    99,    -1,   101,    -1,   103,   104,   105,
   106,    -1,   108,   109,   110,   111,   112,   113,   114,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,
    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    -1,    52,    53,    54,    -1,    -1,    -1,
    58,    59,    60,    -1,    62,    -1,    -1,    -1,    66,    67,
    68,    -1,    70,    -1,    72,    -1,    -1,    -1,    76,    77,
    -1,    -1,    -1,    81,    82,    -1,    84,    85,    86,    87,
    88,    89,    90,    91,    92,    93,    94,    -1,    -1,    -1,
    98,    99,    -1,   101,     7,   103,   104,   105,   106,    -1,
   108,   109,   110,   111,   112,   113,   114,    -1,    21,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    -1,    35,    36,    -1,    -1,    -1,    -1,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
    53,    54,    -1,    -1,    -1,    58,    59,    60,    -1,    62,
    -1,    -1,    -1,    66,    67,    68,    -1,    70,    -1,    72,
    -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    81,    82,
    -1,    84,    85,    86,    87,    88,    89,    90,    91,    92,
    93,    94,     7,    -1,    -1,    98,    99,    -1,   101,    -1,
   103,   104,   105,   106,    -1,   108,    -1,   110,   111,   112,
   113,   114,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    -1,    52,    53,    54,
    -1,    -1,    -1,    58,    59,    60,    -1,    62,    -1,    -1,
    -1,    66,    67,    68,    -1,    70,    -1,    72,    -1,    -1,
    -1,    76,    77,    -1,    -1,    -1,    81,    82,    -1,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
     7,    -1,    -1,    98,    99,    -1,   101,    -1,   103,   104,
   105,   106,   107,   108,    -1,   110,   111,   112,   113,   114,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,
    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    -1,    52,    53,    54,    -1,    -1,
    -1,    58,    59,    60,    -1,    62,    -1,    -1,    -1,    66,
    67,    68,    -1,    70,    -1,    72,    -1,    -1,    -1,    76,
    77,    -1,    -1,    -1,    81,    82,    -1,    84,    85,    86,
    87,    88,    89,    90,    91,    92,    93,    94,     7,    -1,
    -1,    98,    99,    -1,   101,    -1,   103,   104,   105,   106,
   107,   108,    -1,   110,   111,   112,   113,   114,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,    -1,
    -1,    -1,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    -1,    52,    53,    54,    -1,    -1,    -1,    58,
    59,    60,    -1,    62,    -1,    -1,    -1,    66,    67,    68,
    -1,    70,    71,    72,    -1,    -1,    -1,    76,    77,    -1,
    -1,    -1,    81,    82,    -1,    84,    85,    86,    87,    88,
    89,    90,    91,    92,    93,    94,     7,    -1,    -1,    98,
    99,    -1,   101,    -1,   103,   104,   105,   106,    -1,   108,
    -1,   110,   111,   112,   113,   114,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,    -1,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    -1,    52,    53,    54,    -1,    -1,    -1,    58,    59,    60,
    -1,    62,    -1,    -1,    -1,    66,    67,    68,    -1,    70,
    -1,    72,    -1,    -1,    -1,    76,    77,    -1,    -1,    -1,
    81,    82,    -1,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,     7,    -1,    -1,    98,    99,    -1,
   101,    -1,   103,   104,   105,   106,    -1,   108,    -1,   110,
   111,   112,   113,   114,    -1,    -1,    -1,    -1,    -1,    -1,
     7,    -1,    35,    36,    -1,    -1,    -1,    -1,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
    53,    -1,    -1,    -1,    -1,    58,    59,    60,    35,    36,
    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    -1,    52,    53,    -1,    -1,    -1,
    -1,    58,    59,    60,    -1,    -1,    -1,    -1,    91,    92,
    93,    -1,    -1,    -1,    -1,    98,    99,    -1,    -1,    -1,
   103,   104,   105,    -1,    -1,   108,    -1,   110,   111,   112,
   113,   114,     7,    -1,    91,    92,    93,    -1,    -1,    -1,
    -1,    98,    99,    -1,    -1,    -1,   103,   104,   105,    -1,
    -1,   108,    -1,   110,   111,   112,   113,   114,     7,    -1,
    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    -1,    52,    53,    -1,
    -1,    -1,    -1,    58,    59,    60,    35,    36,    -1,    -1,
    -1,    -1,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    -1,    52,    53,    -1,    -1,    -1,    -1,    58,
    59,    60,    -1,    -1,    -1,    -1,    91,    92,    93,    -1,
    -1,    -1,    -1,    98,    99,    -1,    -1,    -1,   103,   104,
   105,    -1,    -1,   108,    -1,   110,   111,   112,   113,   114,
    -1,    -1,    91,    92,    93,    -1,    -1,     7,    -1,    98,
    99,    -1,    -1,    -1,   103,   104,   105,    -1,    -1,   108,
   109,    -1,   111,   112,   113,   114,    26,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    35,    36,     7,    -1,    -1,
    -1,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    -1,    52,    53,    -1,    -1,    26,    -1,    58,    59,
    60,    -1,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,
    -1,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    -1,    52,    53,    -1,    -1,    -1,    -1,    58,    59,
    60,    91,    92,    93,    -1,    -1,    -1,    -1,    98,    99,
    -1,    -1,    -1,   103,   104,   105,    -1,    -1,   108,    -1,
    -1,   111,   112,   113,   114,    -1,    -1,     7,    -1,    -1,
    -1,    91,    92,    93,    -1,    -1,    -1,    -1,    98,    99,
    -1,    -1,    -1,   103,   104,   105,    -1,    -1,   108,    -1,
    -1,   111,   112,   113,   114,    35,    36,    -1,    -1,    -1,
    -1,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    -1,    52,    53,     4,     5,     6,    -1,    58,    59,
    60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    91,    92,    93,    -1,    -1,    -1,    -1,    98,    99,
    -1,    -1,    -1,   103,   104,   105,    -1,    -1,   108,    -1,
    -1,   111,   112,   113,   114,     3,     4,     5,     6,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    20,    -1,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,     4,     5,     6,    -1,    -1,    -1,    -1,
   110,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
    -1,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,     4,     5,     6,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    20,   109,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    51,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
    60,    61,    -1,    63,    64,    65,    -1,    -1,    20,   110,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    96,    -1,    -1,    -1,
    -1,    -1,    -1,     4,     5,     6,   106,   107,    -1,    -1,
    -1,   111,    -1,   113,    -1,   115,    -1,    -1,    -1,    20,
   110,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    51,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    60,
    61,    -1,    63,    64,    65,    -1,    -1,    20,   110,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    96,    -1,    -1,    -1,    -1,
    -1,    -1,     4,     5,     6,   106,   107,    -1,    -1,    -1,
   111,    -1,    -1,   114,   115,    -1,    -1,    -1,    20,   110,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    20,   110,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,   110,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,     4,
     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    20,   110,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,
     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    20,   109,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    20,   109,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    20,   109,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    -1,     4,     5,     6,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    20,   109,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,     4,     5,     6,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    20,   109,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    -1,     4,     5,     6,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
   109,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,     4,     5,     6,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
   109,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    -1,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,   109,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,   109,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,     4,
     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    20,   109,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    20,   107,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,     4,     5,     6,    -1,    -1,    -1,
    -1,    -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    -1,    -1,     4,     5,     6,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
    97,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    20,    -1,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,     6,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    20,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    20,    -1,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 196 "/usr/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 4:
#line 274 "language-parser.y"
{ cs_start_if (&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 5:
#line 274 "language-parser.y"
{ cs_end_if ( _INLINE_TLS_VOID); ;
    break;}
case 6:
#line 275 "language-parser.y"
{ cs_start_if (&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 7:
#line 275 "language-parser.y"
{ cs_end_if ( _INLINE_TLS_VOID); ;
    break;}
case 9:
#line 276 "language-parser.y"
{ cs_start_while(&yyvsp[-3], &yyvsp[-1] _INLINE_TLS); ;
    break;}
case 10:
#line 276 "language-parser.y"
{ cs_end_while(&yyvsp[-5],&yychar _INLINE_TLS); ;
    break;}
case 12:
#line 277 "language-parser.y"
{ cs_start_do_while(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 13:
#line 277 "language-parser.y"
{ cs_force_eval_do_while( _INLINE_TLS_VOID); ;
    break;}
case 14:
#line 277 "language-parser.y"
{ cs_end_do_while(&yyvsp[-8],&yyvsp[-2],&yychar _INLINE_TLS); ;
    break;}
case 16:
#line 278 "language-parser.y"
{ for_pre_expr1(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 17:
#line 279 "language-parser.y"
{ if (GLOBAL(Execute)) yystype_destructor(&yyvsp[0] _INLINE_TLS); for_pre_expr2(&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 18:
#line 280 "language-parser.y"
{ for_pre_expr3(&yyvsp[-6],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 19:
#line 281 "language-parser.y"
{ for_pre_statement(&yyvsp[-9],&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 20:
#line 282 "language-parser.y"
{ for_post_statement(&yyvsp[-12],&yyvsp[-7],&yyvsp[-4],&yyvsp[-1],&yychar _INLINE_TLS); ;
    break;}
case 22:
#line 283 "language-parser.y"
{ cs_switch_start(&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 23:
#line 283 "language-parser.y"
{ cs_switch_end(&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 24:
#line 284 "language-parser.y"
{ DO_OR_DIE(cs_break_continue(NULL,DO_BREAK _INLINE_TLS)) ;
    break;}
case 25:
#line 285 "language-parser.y"
{ DO_OR_DIE(cs_break_continue(&yyvsp[-1],DO_BREAK _INLINE_TLS)) ;
    break;}
case 26:
#line 286 "language-parser.y"
{ DO_OR_DIE(cs_break_continue(NULL,DO_CONTINUE _INLINE_TLS)) ;
    break;}
case 27:
#line 287 "language-parser.y"
{ DO_OR_DIE(cs_break_continue(&yyvsp[-1],DO_CONTINUE _INLINE_TLS)) ;
    break;}
case 28:
#line 288 "language-parser.y"
{ start_function_decleration(_INLINE_TLS_VOID); ;
    break;}
case 29:
#line 289 "language-parser.y"
{ end_function_decleration(&yyvsp[-7],&yyvsp[-6] _INLINE_TLS); ;
    break;}
case 30:
#line 290 "language-parser.y"
{ start_function_decleration(_INLINE_TLS_VOID); ;
    break;}
case 31:
#line 291 "language-parser.y"
{ end_function_decleration(&yyvsp[-8],&yyvsp[-7] _INLINE_TLS); ;
    break;}
case 32:
#line 292 "language-parser.y"
{ cs_return(NULL _INLINE_TLS); ;
    break;}
case 33:
#line 293 "language-parser.y"
{ cs_return(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 36:
#line 296 "language-parser.y"
{ cs_start_class_decleration(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 37:
#line 296 "language-parser.y"
{ cs_end_class_decleration( _INLINE_TLS_VOID); ;
    break;}
case 38:
#line 297 "language-parser.y"
{ cs_start_class_decleration(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 39:
#line 297 "language-parser.y"
{ cs_end_class_decleration( _INLINE_TLS_VOID); ;
    break;}
case 41:
#line 299 "language-parser.y"
{ if (GLOBAL(Execute)) { if (php3_header()) PUTS(yyvsp[0].value.str.val); } ;
    break;}
case 42:
#line 300 "language-parser.y"
{ if (GLOBAL(Execute)) yystype_destructor(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 43:
#line 301 "language-parser.y"
{ cs_start_include(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 44:
#line 301 "language-parser.y"
{ cs_end_include(&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 45:
#line 302 "language-parser.y"
{ if (GLOBAL(Execute)) cs_show_source(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 46:
#line 303 "language-parser.y"
{ if (GLOBAL(Execute)) eval_string(&yyvsp[-2],NULL,1 _INLINE_TLS); ;
    break;}
case 47:
#line 304 "language-parser.y"
{ if (GLOBAL(Execute)) eval_string(&yyvsp[-4],&yyvsp[-2],2 _INLINE_TLS); ;
    break;}
case 48:
#line 305 "language-parser.y"
{ if (GLOBAL(Execute)) eval_string(&yyvsp[-2],&yyvsp[0],0 _INLINE_TLS); ;
    break;}
case 49:
#line 306 "language-parser.y"
{ if (GLOBAL(Execute)) conditional_include_file(&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 60:
#line 330 "language-parser.y"
{ cs_start_while(&yyvsp[-3], &yyvsp[-1] _INLINE_TLS); ;
    break;}
case 61:
#line 330 "language-parser.y"
{ cs_end_while(&yyvsp[-5],&yychar _INLINE_TLS); ;
    break;}
case 63:
#line 336 "language-parser.y"
{ cs_start_do_while(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 64:
#line 336 "language-parser.y"
{ cs_force_eval_do_while( _INLINE_TLS_VOID); ;
    break;}
case 65:
#line 336 "language-parser.y"
{ cs_end_do_while(&yyvsp[-8],&yyvsp[-2],&yychar _INLINE_TLS); ;
    break;}
case 67:
#line 342 "language-parser.y"
{ for_pre_expr1(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 68:
#line 343 "language-parser.y"
{ if (GLOBAL(Execute)) yystype_destructor(&yyvsp[0] _INLINE_TLS); for_pre_expr2(&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 69:
#line 344 "language-parser.y"
{ for_pre_expr3(&yyvsp[-6],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 70:
#line 345 "language-parser.y"
{ for_pre_statement(&yyvsp[-9],&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 71:
#line 346 "language-parser.y"
{ for_post_statement(&yyvsp[-12],&yyvsp[-7],&yyvsp[-4],&yyvsp[-1],&yychar _INLINE_TLS); ;
    break;}
case 73:
#line 352 "language-parser.y"
{ cs_elseif_start_evaluate( _INLINE_TLS_VOID); ;
    break;}
case 74:
#line 353 "language-parser.y"
{ cs_elseif_end_evaluate( _INLINE_TLS_VOID); ;
    break;}
case 75:
#line 353 "language-parser.y"
{ cs_start_elseif (&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 78:
#line 359 "language-parser.y"
{ cs_elseif_start_evaluate( _INLINE_TLS_VOID); ;
    break;}
case 79:
#line 360 "language-parser.y"
{ cs_elseif_end_evaluate( _INLINE_TLS_VOID); ;
    break;}
case 80:
#line 360 "language-parser.y"
{ cs_start_elseif (&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 83:
#line 366 "language-parser.y"
{ cs_start_else( _INLINE_TLS_VOID); ;
    break;}
case 86:
#line 372 "language-parser.y"
{ cs_start_else( _INLINE_TLS_VOID); ;
    break;}
case 89:
#line 378 "language-parser.y"
{ cs_switch_case_pre(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 90:
#line 378 "language-parser.y"
{ cs_switch_case_post( _INLINE_TLS_VOID); ;
    break;}
case 92:
#line 379 "language-parser.y"
{ cs_switch_case_pre(NULL _INLINE_TLS); ;
    break;}
case 93:
#line 379 "language-parser.y"
{ cs_switch_case_post( _INLINE_TLS_VOID); ;
    break;}
case 94:
#line 380 "language-parser.y"
{ cs_switch_case_pre(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 95:
#line 380 "language-parser.y"
{ cs_switch_case_post( _INLINE_TLS_VOID); ;
    break;}
case 97:
#line 381 "language-parser.y"
{ cs_switch_case_pre(NULL _INLINE_TLS); ;
    break;}
case 98:
#line 381 "language-parser.y"
{ cs_switch_case_post( _INLINE_TLS_VOID); ;
    break;}
case 99:
#line 386 "language-parser.y"
{ GLOBAL(param_index)=0; ;
    break;}
case 102:
#line 392 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_NONE, NULL _INLINE_TLS); ;
    break;}
case 103:
#line 393 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_FORCE, NULL _INLINE_TLS); ;
    break;}
case 104:
#line 394 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_ALLOW, NULL _INLINE_TLS); ;
    break;}
case 105:
#line 395 "language-parser.y"
{ get_function_parameter(&yyvsp[-2], BYREF_NONE, &yyvsp[0] _INLINE_TLS); ;
    break;}
case 106:
#line 396 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_NONE, NULL _INLINE_TLS); ;
    break;}
case 107:
#line 397 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_FORCE, NULL  _INLINE_TLS); ;
    break;}
case 108:
#line 398 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_ALLOW, NULL _INLINE_TLS); ;
    break;}
case 109:
#line 399 "language-parser.y"
{ get_function_parameter(&yyvsp[-2], BYREF_NONE, &yyvsp[0] _INLINE_TLS); ;
    break;}
case 112:
#line 410 "language-parser.y"
{ pass_parameter_by_value(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 113:
#line 411 "language-parser.y"
{ pass_parameter(&yyvsp[0],0 _INLINE_TLS); ;
    break;}
case 114:
#line 412 "language-parser.y"
{ pass_parameter(&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 115:
#line 413 "language-parser.y"
{ pass_parameter_by_value(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 116:
#line 414 "language-parser.y"
{ pass_parameter(&yyvsp[0],0 _INLINE_TLS); ;
    break;}
case 117:
#line 415 "language-parser.y"
{ pass_parameter(&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 118:
#line 419 "language-parser.y"
{ cs_global_variable(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 119:
#line 420 "language-parser.y"
{ cs_global_variable(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 120:
#line 425 "language-parser.y"
{ cs_static_variable(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 121:
#line 426 "language-parser.y"
{ cs_static_variable(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 122:
#line 427 "language-parser.y"
{ cs_static_variable(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 123:
#line 428 "language-parser.y"
{ cs_static_variable(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 124:
#line 434 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 125:
#line 435 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 129:
#line 447 "language-parser.y"
{ start_function_decleration(_INLINE_TLS_VOID); ;
    break;}
case 130:
#line 448 "language-parser.y"
{ end_function_decleration(&yyvsp[-7],&yyvsp[-6] _INLINE_TLS); ;
    break;}
case 131:
#line 449 "language-parser.y"
{ start_function_decleration(_INLINE_TLS_VOID); ;
    break;}
case 132:
#line 450 "language-parser.y"
{ end_function_decleration(&yyvsp[-8],&yyvsp[-7] _INLINE_TLS); ;
    break;}
case 133:
#line 456 "language-parser.y"
{ declare_class_variable(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 134:
#line 457 "language-parser.y"
{ declare_class_variable(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 135:
#line 458 "language-parser.y"
{ declare_class_variable(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 136:
#line 459 "language-parser.y"
{ declare_class_variable(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 138:
#line 465 "language-parser.y"
{ if (GLOBAL(Execute)) { print_variable(&yyvsp[0] _INLINE_TLS); yystype_destructor(&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 139:
#line 466 "language-parser.y"
{ if (GLOBAL(Execute)) { print_variable(&yyvsp[0] _INLINE_TLS); yystype_destructor(&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 140:
#line 471 "language-parser.y"
{ yyval.value.lval=1;  yyval.type=IS_LONG; ;
    break;}
case 141:
#line 472 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; yystype_destructor(&yyvsp[-2] _INLINE_TLS); } ;
    break;}
case 142:
#line 473 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 143:
#line 478 "language-parser.y"
{ assign_to_list(&yyval, &yyvsp[-3], &yyvsp[0] _INLINE_TLS);;
    break;}
case 144:
#line 479 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 145:
#line 480 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],add_function _INLINE_TLS); ;
    break;}
case 146:
#line 481 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],sub_function _INLINE_TLS); ;
    break;}
case 147:
#line 482 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],mul_function _INLINE_TLS); ;
    break;}
case 148:
#line 483 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],div_function _INLINE_TLS); ;
    break;}
case 149:
#line 484 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],concat_function_with_free _INLINE_TLS); ;
    break;}
case 150:
#line 485 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],mod_function _INLINE_TLS); ;
    break;}
case 151:
#line 486 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],bitwise_and_function _INLINE_TLS); ;
    break;}
case 152:
#line 487 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],bitwise_or_function _INLINE_TLS); ;
    break;}
case 153:
#line 488 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],bitwise_xor_function _INLINE_TLS); ;
    break;}
case 154:
#line 489 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],shift_left_function _INLINE_TLS); ;
    break;}
case 155:
#line 490 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],shift_right_function _INLINE_TLS); ;
    break;}
case 156:
#line 491 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[-1],increment_function,1 _INLINE_TLS); ;
    break;}
case 157:
#line 492 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[0],increment_function,0 _INLINE_TLS); ;
    break;}
case 158:
#line 493 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[-1],decrement_function,1 _INLINE_TLS); ;
    break;}
case 159:
#line 494 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[0],decrement_function,0 _INLINE_TLS); ;
    break;}
case 160:
#line 495 "language-parser.y"
{ cs_pre_boolean_or(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 161:
#line 495 "language-parser.y"
{ cs_post_boolean_or(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 162:
#line 496 "language-parser.y"
{ cs_pre_boolean_and(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 163:
#line 496 "language-parser.y"
{ cs_post_boolean_and(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 164:
#line 497 "language-parser.y"
{ cs_pre_boolean_or(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 165:
#line 497 "language-parser.y"
{ cs_post_boolean_or(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 166:
#line 498 "language-parser.y"
{ cs_pre_boolean_and(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 167:
#line 498 "language-parser.y"
{ cs_post_boolean_and(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 168:
#line 499 "language-parser.y"
{ if (GLOBAL(Execute)) { boolean_xor_function(&yyval,&yyvsp[-2],&yyvsp[0]); } ;
    break;}
case 169:
#line 500 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_or_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 170:
#line 501 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_xor_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 171:
#line 502 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_and_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 172:
#line 503 "language-parser.y"
{ if (GLOBAL(Execute)) concat_function(&yyval,&yyvsp[-2],&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 173:
#line 504 "language-parser.y"
{ if (GLOBAL(Execute)) add_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 174:
#line 505 "language-parser.y"
{ if (GLOBAL(Execute)) sub_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 175:
#line 506 "language-parser.y"
{ if (GLOBAL(Execute)) mul_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 176:
#line 507 "language-parser.y"
{ if (GLOBAL(Execute)) div_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 177:
#line 508 "language-parser.y"
{ if (GLOBAL(Execute)) mod_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 178:
#line 509 "language-parser.y"
{ if (GLOBAL(Execute)) shift_left_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 179:
#line 510 "language-parser.y"
{ if (GLOBAL(Execute)) shift_right_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 180:
#line 511 "language-parser.y"
{ if (GLOBAL(Execute)) { pval tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  add_function(&yyval,&tmp,&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 181:
#line 512 "language-parser.y"
{ if (GLOBAL(Execute)) { pval tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  sub_function(&yyval,&tmp,&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 182:
#line 513 "language-parser.y"
{ if (GLOBAL(Execute)) boolean_not_function(&yyval,&yyvsp[0]); ;
    break;}
case 183:
#line 514 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_not_function(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 184:
#line 515 "language-parser.y"
{ if (GLOBAL(Execute)) is_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 185:
#line 516 "language-parser.y"
{ if (GLOBAL(Execute)) is_not_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 186:
#line 517 "language-parser.y"
{ if (GLOBAL(Execute)) is_smaller_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 187:
#line 518 "language-parser.y"
{ if (GLOBAL(Execute)) is_smaller_or_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 188:
#line 519 "language-parser.y"
{ if (GLOBAL(Execute)) is_greater_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 189:
#line 520 "language-parser.y"
{ if (GLOBAL(Execute)) is_greater_or_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 190:
#line 521 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 191:
#line 522 "language-parser.y"
{  php3_error(E_PARSE,"'(' unmatched",GLOBAL(current_lineno)); if (GLOBAL(Execute)) yyval = yyvsp[-1]; yyerrok; ;
    break;}
case 192:
#line 523 "language-parser.y"
{ cs_questionmark_op_pre_expr1(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 193:
#line 524 "language-parser.y"
{ cs_questionmark_op_pre_expr2(&yyvsp[-4] _INLINE_TLS); ;
    break;}
case 194:
#line 525 "language-parser.y"
{ cs_questionmark_op_post_expr2(&yyval,&yyvsp[-6],&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 195:
#line 526 "language-parser.y"
{ cs_functioncall_pre_variable_passing(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 196:
#line 527 "language-parser.y"
{ cs_functioncall_post_variable_passing(&yyvsp[-4],&yychar _INLINE_TLS); ;
    break;}
case 197:
#line 528 "language-parser.y"
{ cs_functioncall_end(&yyval,&yyvsp[-6],&yyvsp[-2],&yychar _INLINE_TLS);;
    break;}
case 199:
#line 530 "language-parser.y"
{ cs_functioncall_pre_variable_passing(&yyvsp[0],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 200:
#line 531 "language-parser.y"
{ cs_functioncall_post_variable_passing(&yyvsp[-4],&yychar _INLINE_TLS); ;
    break;}
case 201:
#line 532 "language-parser.y"
{ cs_functioncall_end(&yyval,&yyvsp[-6],&yyvsp[-2],&yychar _INLINE_TLS); ;
    break;}
case 202:
#line 533 "language-parser.y"
{ assign_new_object(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 203:
#line 534 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_long(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 204:
#line 535 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_double(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 205:
#line 536 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_string(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 206:
#line 537 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_array(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 207:
#line 538 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_object(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 208:
#line 539 "language-parser.y"
{ if (GLOBAL(Execute)) { php3_header(); GLOBAL(shutdown_requested)=ABNORMAL_SHUTDOWN; yyval.type=IS_LONG; yyval.value.lval=1; } ;
    break;}
case 209:
#line 540 "language-parser.y"
{ if (GLOBAL(Execute)) { php3_header(); GLOBAL(shutdown_requested)=ABNORMAL_SHUTDOWN; yyval.type=IS_LONG; yyval.value.lval=1; } ;
    break;}
case 210:
#line 541 "language-parser.y"
{ if (GLOBAL(Execute)) { if (php3_header()) { convert_to_string(&yyvsp[-1]);  PUTS(yyvsp[-1].value.str.val);  yystype_destructor(&yyvsp[-1] _INLINE_TLS); } GLOBAL(shutdown_requested)=ABNORMAL_SHUTDOWN; yyval.type=IS_LONG; yyval.value.lval=1; } ;
    break;}
case 211:
#line 542 "language-parser.y"
{ yyvsp[0].cs_data.error_reporting=GLOBAL(error_reporting); GLOBAL(error_reporting)=0; ;
    break;}
case 212:
#line 542 "language-parser.y"
{ GLOBAL(error_reporting)=yyvsp[-2].cs_data.error_reporting; yyval = yyvsp[0]; ;
    break;}
case 213:
#line 543 "language-parser.y"
{ php3_error(E_ERROR,"@ operator may only be used on expressions"); ;
    break;}
case 214:
#line 544 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 215:
#line 545 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 216:
#line 546 "language-parser.y"
{ cs_system(&yyval,&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 217:
#line 547 "language-parser.y"
{ if (GLOBAL(Execute)) { print_variable(&yyvsp[0] _INLINE_TLS);  yystype_destructor(&yyvsp[0] _INLINE_TLS);  yyval.value.lval=1; yyval.type=IS_LONG; } ;
    break;}
case 218:
#line 552 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 219:
#line 553 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 220:
#line 554 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 221:
#line 555 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 222:
#line 556 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; COPY_STRING(yyval); } ;
    break;}
case 223:
#line 557 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; COPY_STRING(yyval); php3_error(E_NOTICE,"'%s' is not a valid constant - assumed to be \"%s\"",yyvsp[0].value.str.val,yyvsp[0].value.str.val); } ;
    break;}
case 224:
#line 558 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; } ;
    break;}
case 225:
#line 559 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; COPY_STRING(yyval); } ;
    break;}
case 226:
#line 563 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 227:
#line 564 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 228:
#line 565 "language-parser.y"
{ if (GLOBAL(Execute)) { pval tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  sub_function(&yyval,&tmp,&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 229:
#line 569 "language-parser.y"
{ if (GLOBAL(Execute)){ yyval = yyvsp[0]; };
    break;}
case 230:
#line 575 "language-parser.y"
{ if (GLOBAL(Execute)) read_pointer_value(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 231:
#line 576 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 232:
#line 581 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 233:
#line 582 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 234:
#line 587 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval = yyvsp[0];COPY_STRING(yyval); };
    break;}
case 235:
#line 588 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 236:
#line 594 "language-parser.y"
{ get_object_symtable(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 237:
#line 595 "language-parser.y"
{ if (GLOBAL(Execute)) { if (yyvsp[0].value.varptr.yystype && ((pval *)yyvsp[0].value.varptr.yystype)->type == IS_OBJECT) { yyval=yyvsp[0]; } else { yyval.value.varptr.yystype=NULL; } } ;
    break;}
case 238:
#line 596 "language-parser.y"
{ get_object_symtable(&yyval,NULL,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 239:
#line 600 "language-parser.y"
{

	if (GLOBAL(Execute)) {
		yyval=yyvsp[-2];
		_php3_hash_next_index_insert(yyval.value.ht,&yyvsp[0],sizeof(pval),NULL);
	}
;
    break;}
case 240:
#line 607 "language-parser.y"
{

	if (GLOBAL(Execute)) {
		yyval.value.ht = (HashTable *) emalloc(sizeof(HashTable));
		_php3_hash_init(yyval.value.ht,0,NULL,NULL,0);
		yyval.type = IS_ARRAY;
		_php3_hash_next_index_insert(yyval.value.ht,&yyvsp[0],sizeof(pval),NULL);
	}
;
    break;}
case 241:
#line 616 "language-parser.y"
{
	if (GLOBAL(Execute)) {
		yyval=yyvsp[-1];
		yyvsp[0].value.varptr.yystype = NULL; /* $2 is just used as temporary space */
		_php3_hash_next_index_insert(yyval.value.ht,&yyvsp[0],sizeof(pval),NULL);
	}
;
    break;}
case 242:
#line 623 "language-parser.y"
{
	if (GLOBAL(Execute)) {
		pval tmp;

		yyval.value.ht = (HashTable *) emalloc(sizeof(HashTable));
		_php3_hash_init(yyval.value.ht,0,NULL,NULL,0);
		yyval.type = IS_ARRAY;
		tmp.value.varptr.yystype = NULL;
		_php3_hash_next_index_insert(yyval.value.ht,&tmp,sizeof(pval),NULL);
	}
;
    break;}
case 243:
#line 638 "language-parser.y"
{ get_regular_variable_pointer(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 244:
#line 639 "language-parser.y"
{ if (GLOBAL(Execute)) yyval=yyvsp[0]; ;
    break;}
case 245:
#line 640 "language-parser.y"
{ get_class_variable_pointer(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 246:
#line 641 "language-parser.y"
{ start_array_parsing(&yyvsp[-1],&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 247:
#line 641 "language-parser.y"
{ end_array_parsing(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 248:
#line 646 "language-parser.y"
{ if (GLOBAL(Execute)) get_regular_variable_contents(&yyval,&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 249:
#line 647 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval = yyvsp[0];COPY_STRING(yyval);} ;
    break;}
case 250:
#line 648 "language-parser.y"
{  if (GLOBAL(Execute)) read_pointer_value(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 251:
#line 652 "language-parser.y"
{ start_array_parsing(&yyvsp[-1],NULL _INLINE_TLS); ;
    break;}
case 252:
#line 652 "language-parser.y"
{ end_array_parsing(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 253:
#line 657 "language-parser.y"
{ fetch_array_index(&yyval,&yyvsp[-1],&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 254:
#line 658 "language-parser.y"
{ fetch_array_index(&yyval,NULL,&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 255:
#line 659 "language-parser.y"
{ start_dimensions_parsing(&yyval _INLINE_TLS); ;
    break;}
case 256:
#line 659 "language-parser.y"
{ fetch_array_index(&yyval,NULL,&yyval _INLINE_TLS); ;
    break;}
case 257:
#line 660 "language-parser.y"
{ start_dimensions_parsing(&yyval _INLINE_TLS); ;
    break;}
case 258:
#line 660 "language-parser.y"
{ fetch_array_index(&yyval,&yyvsp[-1],&yyval _INLINE_TLS); ;
    break;}
case 259:
#line 665 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval.value.ht = (HashTable *) emalloc(sizeof(HashTable));  _php3_hash_init(yyval.value.ht,0,NULL,pval_DESTRUCTOR,0); yyval.type = IS_ARRAY; } ;
    break;}
case 260:
#line 666 "language-parser.y"
{ yyval = yyvsp[0]; ;
    break;}
case 261:
#line 670 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval=yyvsp[-4]; add_array_pair_list(&yyval, &yyvsp[-2], &yyvsp[0], 0 _INLINE_TLS);} ;
    break;}
case 262:
#line 671 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval=yyvsp[-2]; add_array_pair_list(&yyval, NULL, &yyvsp[0], 0 _INLINE_TLS);} ;
    break;}
case 263:
#line 672 "language-parser.y"
{ if (GLOBAL(Execute)) add_array_pair_list(&yyval, &yyvsp[-2], &yyvsp[0], 1 _INLINE_TLS); ;
    break;}
case 264:
#line 673 "language-parser.y"
{ if (GLOBAL(Execute)) add_array_pair_list(&yyval, NULL, &yyvsp[0], 1 _INLINE_TLS); ;
    break;}
case 265:
#line 678 "language-parser.y"
{ add_regular_encapsed_variable(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 266:
#line 679 "language-parser.y"
{ add_assoc_array_encapsed_variable(&yyval,&yyvsp[-5],&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 267:
#line 680 "language-parser.y"
{ add_regular_array_encapsed_variable(&yyval,&yyvsp[-5],&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 268:
#line 681 "language-parser.y"
{ add_variable_array_encapsed_variable(&yyval,&yyvsp[-6],&yyvsp[-4],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 269:
#line 682 "language-parser.y"
{ add_encapsed_object_property(&yyval,&yyvsp[-4],&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 270:
#line 683 "language-parser.y"
{ add_regular_encapsed_variable(&yyval,&yyvsp[-4],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 271:
#line 684 "language-parser.y"
{ add_assoc_array_encapsed_variable(&yyval,&yyvsp[-7],&yyvsp[-4],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 272:
#line 685 "language-parser.y"
{ add_regular_array_encapsed_variable(&yyval,&yyvsp[-7],&yyvsp[-4],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 273:
#line 686 "language-parser.y"
{ add_variable_array_encapsed_variable(&yyval,&yyvsp[-8],&yyvsp[-5],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 274:
#line 687 "language-parser.y"
{ add_encapsed_object_property(&yyval,&yyvsp[-6],&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 275:
#line 688 "language-parser.y"
{ add_indirect_encapsed_variable(&yyval,&yyvsp[-5],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 276:
#line 689 "language-parser.y"
{ if (GLOBAL(Execute)) { concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 277:
#line 690 "language-parser.y"
{ if (GLOBAL(Execute)) { concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 278:
#line 691 "language-parser.y"
{ if (GLOBAL(Execute)) { concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 279:
#line 692 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 280:
#line 693 "language-parser.y"
{ if (GLOBAL(Execute)) { php3_error(E_NOTICE,"Bad escape sequence:  %s",yyvsp[0].value.str.val); concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 281:
#line 694 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 282:
#line 695 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 283:
#line 696 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 284:
#line 697 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 285:
#line 698 "language-parser.y"
{  if (GLOBAL(Execute)) { pval tmp;  tmp.value.str.val="->"; tmp.value.str.len=2; tmp.type=IS_STRING; concat_function(&yyval,&yyvsp[-1],&tmp,0 _INLINE_TLS); } ;
    break;}
case 286:
#line 699 "language-parser.y"
{ if (GLOBAL(Execute)) var_reset(&yyval); ;
    break;}
case 287:
#line 704 "language-parser.y"
{ php3_unset(&yyval, &yyvsp[-1]); ;
    break;}
case 288:
#line 705 "language-parser.y"
{ php3_isset(&yyval, &yyvsp[-1]); ;
    break;}
case 289:
#line 706 "language-parser.y"
{ php3_empty(&yyval, &yyvsp[-1]); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 716 "language-parser.y"


inline void clear_lookahead(int *yychar)
{
	if (yychar) {
		*yychar=YYEMPTY;
	}
}
	
