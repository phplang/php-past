
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
#define	BOOLEAN_OR	271
#define	BOOLEAN_AND	272
#define	IS_EQUAL	273
#define	IS_NOT_EQUAL	274
#define	IS_SMALLER_OR_EQUAL	275
#define	IS_GREATER_OR_EQUAL	276
#define	SHIFT_LEFT	277
#define	SHIFT_RIGHT	278
#define	INCREMENT	279
#define	DECREMENT	280
#define	INT_CAST	281
#define	DOUBLE_CAST	282
#define	STRING_CAST	283
#define	ARRAY_CAST	284
#define	OBJECT_CAST	285
#define	NEW	286
#define	EXIT	287
#define	IF	288
#define	ELSEIF	289
#define	ELSE	290
#define	ENDIF	291
#define	LNUMBER	292
#define	DNUMBER	293
#define	STRING	294
#define	NUM_STRING	295
#define	INLINE_HTML	296
#define	CHARACTER	297
#define	BAD_CHARACTER	298
#define	ENCAPSED_AND_WHITESPACE	299
#define	PHP_ECHO	300
#define	DO	301
#define	WHILE	302
#define	ENDWHILE	303
#define	FOR	304
#define	ENDFOR	305
#define	SWITCH	306
#define	ENDSWITCH	307
#define	CASE	308
#define	DEFAULT	309
#define	BREAK	310
#define	CONTINUE	311
#define	CONTINUED_WHILE	312
#define	CONTINUED_DOWHILE	313
#define	CONTINUED_FOR	314
#define	OLD_FUNCTION	315
#define	FUNCTION	316
#define	PHP_CONST	317
#define	RETURN	318
#define	INCLUDE	319
#define	REQUIRE	320
#define	HIGHLIGHT_FILE	321
#define	HIGHLIGHT_STRING	322
#define	PHP_GLOBAL	323
#define	PHP_STATIC	324
#define	PHP_UNSET	325
#define	PHP_ISSET	326
#define	PHP_EMPTY	327
#define	CLASS	328
#define	EXTENDS	329
#define	PHP_CLASS_OPERATOR	330
#define	PHP_DOUBLE_ARROW	331
#define	PHP_LIST	332
#define	PHP_ARRAY	333
#define	VAR	334
#define	EVAL	335
#define	DONE_EVAL	336
#define	PHP_LINE	337
#define	PHP_FILE	338
#define	STRING_CONSTANT	339

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


/* $Id: language-parser.y,v 1.156 1998/05/30 18:01:02 zeev Exp $ */


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
	
	return hash_init(&GLOBAL(list), 0, NULL, list_entry_destructor, 0);
}

int init_resource_plist(void)
{
	TLS_VARS;
	
	return hash_init(&GLOBAL(plist), 0, NULL, plist_entry_destructor, 1);
}

void destroy_resource_list(void)
{
	TLS_VARS;
	
	hash_destroy(&GLOBAL(list));
}

void destroy_resource_plist(void)
{
	TLS_VARS;
	
	hash_destroy(&GLOBAL(plist));
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
		hash_apply_with_argument(&GLOBAL(plist), (int (*)(void *,void *)) clean_module_resource, (void *) &(ld->resource_id));
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



#define	YYFINAL		602
#define	YYFLAG		-32768
#define	YYNTBASE	114

#define YYTRANSLATE(x) ((unsigned)(x) <= 339 ? yytranslate[x] : 215)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    39,   111,     2,   109,    38,    24,   112,   106,
   107,    36,    33,     3,    34,    35,    37,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    19,   108,    27,
     8,    29,    18,    48,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    49,     2,   113,    23,     2,   110,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   104,    22,   105,    40,     2,     2,     2,     2,
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
    20,    21,    25,    26,    28,    30,    31,    32,    41,    42,
    43,    44,    45,    46,    47,    50,    51,    52,    53,    54,
    55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
    75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
    95,    96,    97,    98,    99,   100,   101,   102,   103
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
   552,   556,   560,   564,   567,   570,   573,   576,   577,   582,
   583,   588,   589,   594,   595,   600,   604,   608,   612,   616,
   620,   624,   628,   632,   636,   640,   643,   646,   649,   652,
   656,   660,   664,   668,   672,   676,   680,   684,   685,   686,
   694,   695,   696,   704,   706,   707,   708,   719,   722,   725,
   728,   731,   734,   737,   739,   743,   748,   749,   753,   756,
   758,   763,   767,   770,   772,   774,   778,   782,   784,   786,
   788,   790,   792,   795,   798,   800,   802,   804,   806,   810,
   812,   816,   820,   822,   824,   828,   830,   833,   834,   837,
   840,   845,   846,   854,   857,   859,   862,   863,   868,   873,
   877,   878,   881,   882,   886,   887,   889,   895,   899,   903,
   905,   909,   916,   923,   931,   937,   943,   952,   961,   971,
   979,   986,   989,   992,   995,   998,  1001,  1004,  1007,  1010,
  1013,  1016,  1017,  1022,  1027,  1032,  1033,  1041
};

static const short yyrhs[] = {   114,
   115,     0,     0,   104,   114,   105,     0,     0,    52,   106,
   197,   107,   116,   115,   148,   156,     0,     0,     0,    52,
   106,   197,   107,    19,   117,   114,   152,   158,   118,    55,
   108,     0,     0,     0,    66,   106,   197,   107,   119,   137,
   120,   138,     0,     0,     0,     0,    65,   121,   115,    66,
   122,   106,   197,   107,   108,   123,   140,     0,     0,     0,
     0,     0,     0,    68,   124,   106,   181,   125,   108,   181,
   126,   108,   181,   127,   107,   135,   128,   143,     0,     0,
    70,   106,   197,   107,   129,   136,     0,    74,   108,     0,
    74,   197,   108,     0,    75,   108,     0,    75,   197,   108,
     0,     0,    79,    58,   130,   167,   106,   114,   107,   108,
     0,     0,    80,    58,   106,   131,   167,   107,   104,   114,
   105,     0,    82,   108,     0,    82,   197,   108,     0,    87,
   172,     0,    88,   173,     0,     0,    92,    58,   132,   104,
   175,   105,     0,     0,    92,    58,    93,    58,   133,   104,
   175,   105,     0,    64,   180,   108,     0,    60,     0,   197,
   108,     0,     0,    84,   134,   197,   108,     0,    85,   197,
   108,     0,    86,   106,   197,   107,   108,     0,    86,   106,
   197,     3,   197,   107,   108,     0,    99,   106,   197,   107,
   108,     0,    83,   197,   108,     0,   108,     0,   115,     0,
    19,   114,    69,   108,     0,   104,   160,   105,     0,   104,
   108,   160,   105,     0,    19,   160,    71,   108,     0,    19,
   108,   160,    71,   108,     0,   115,     0,    19,   114,    67,
   108,     0,     0,     0,   138,    76,   106,   197,   107,   139,
   137,     0,     0,     0,     0,   140,    77,   141,   115,    66,
   142,   106,   197,   107,   108,     0,     0,     0,     0,     0,
     0,   143,    78,   144,   106,   181,   145,   108,   181,   146,
   108,   181,   147,   107,   135,     0,     0,     0,     0,     0,
   148,    53,   106,   149,   197,   150,   107,   151,   115,     0,
     0,     0,     0,     0,   152,    53,   106,   153,   197,   154,
   107,    19,   155,   114,     0,     0,     0,    54,   157,   115,
     0,     0,     0,    54,    19,   159,   114,     0,     0,     0,
     0,    72,   197,    19,   161,   114,   162,   160,     0,     0,
    73,    19,   163,   114,     0,     0,     0,    72,   197,   108,
   164,   114,   165,   160,     0,     0,    73,   108,   166,   114,
     0,     0,   168,   169,     0,     0,   109,   196,     0,    24,
   109,   196,     0,    81,   109,   196,     0,   109,   196,     8,
   195,     0,   169,     3,   109,   196,     0,   169,     3,    24,
   109,   196,     0,   169,     3,    81,   109,   196,     0,   169,
     3,   109,   196,     8,   195,     0,   171,     0,     0,   182,
     0,   202,     0,    24,   202,     0,   171,     3,   182,     0,
   171,     3,   202,     0,   171,     3,    24,   202,     0,   172,
     3,   109,   204,     0,   109,   204,     0,   173,     3,   109,
   204,     0,   173,     3,   109,   204,     8,   174,     0,   109,
   204,     0,   109,   204,     8,   174,     0,   195,     0,   106,
   197,   107,     0,   175,   176,     0,     0,    98,   179,   108,
     0,     0,    79,    58,   177,   167,   106,   114,   107,   108,
     0,     0,    80,    58,   106,   178,   167,   107,   104,   114,
   105,     0,   179,     3,   109,   196,     0,   179,     3,   109,
   196,     8,   197,     0,   109,   196,     0,   109,   196,     8,
   197,     0,     0,   180,     3,   197,     0,   197,     0,     0,
   181,     3,   197,     0,   197,     0,    96,   106,   201,   107,
     8,   197,     0,   202,     8,   197,     0,   202,     9,   197,
     0,   202,    10,   197,     0,   202,    11,   197,     0,   202,
    12,   197,     0,   202,    13,   197,     0,   202,    14,   197,
     0,   202,    15,   197,     0,   202,    16,   197,     0,   202,
    17,   197,     0,   202,    41,     0,    41,   202,     0,   202,
    42,     0,    42,   202,     0,     0,   197,    20,   183,   197,
     0,     0,   197,    21,   184,   197,     0,     0,   197,     4,
   185,   197,     0,     0,   197,     6,   186,   197,     0,   197,
     5,   197,     0,   197,    22,   197,     0,   197,    23,   197,
     0,   197,    24,   197,     0,   197,    35,   197,     0,   197,
    33,   197,     0,   197,    34,   197,     0,   197,    36,   197,
     0,   197,    37,   197,     0,   197,    38,   197,     0,    33,
   197,     0,    34,   197,     0,    39,   197,     0,    40,   197,
     0,   197,    25,   197,     0,   197,    26,   197,     0,   197,
    27,   197,     0,   197,    28,   197,     0,   197,    29,   197,
     0,   197,    30,   197,     0,   106,   197,   107,     0,   106,
   197,     1,     0,     0,     0,   197,    18,   187,   197,    19,
   188,   197,     0,     0,     0,   204,   189,   106,   170,   107,
   190,   214,     0,   213,     0,     0,     0,   109,   200,    94,
   204,   191,   106,   170,   107,   192,   214,     0,    50,   204,
     0,    43,   197,     0,    44,   197,     0,    45,   197,     0,
    46,   197,     0,    47,   197,     0,    51,     0,    51,   106,
   107,     0,    51,   106,   197,   107,     0,     0,    48,   193,
   197,     0,    48,     1,     0,   194,     0,    97,   106,   210,
   107,     0,   110,   212,   110,     0,     7,   197,     0,    56,
     0,    57,     0,   111,   212,   111,     0,   112,   212,   112,
     0,   103,     0,    58,     0,   101,     0,   102,     0,   194,
     0,    33,   195,     0,    34,   195,     0,    58,     0,   202,
     0,   182,     0,   204,     0,   104,   197,   105,     0,   196,
     0,   104,   197,   105,     0,   200,    94,   196,     0,   205,
     0,   196,     0,   201,     3,   202,     0,   202,     0,   201,
     3,     0,     0,   109,   198,     0,   109,   205,     0,   109,
   200,    94,   198,     0,     0,   109,   200,    94,   199,    49,
   203,   207,     0,   109,   198,     0,   196,     0,   109,   205,
     0,     0,   199,    49,   206,   207,     0,   207,    49,   197,
   113,     0,   207,    49,   113,     0,     0,   208,   113,     0,
     0,   209,   197,   113,     0,     0,   211,     0,   211,     3,
   197,    95,   197,     0,   211,     3,   197,     0,   197,    95,
   197,     0,   197,     0,   212,   109,    58,     0,   212,   109,
    58,    49,    58,   113,     0,   212,   109,    58,    49,    59,
   113,     0,   212,   109,    58,    49,   109,    58,   113,     0,
   212,   109,    58,    94,    58,     0,   212,   109,   104,    58,
   105,     0,   212,   109,   104,    58,    49,    58,   113,   105,
     0,   212,   109,   104,    58,    49,    59,   113,   105,     0,
   212,   109,   104,    58,    49,   109,    58,   113,   105,     0,
   212,   109,   104,    58,    94,    58,   105,     0,   212,   109,
   104,   109,    58,   105,     0,   212,    58,     0,   212,    59,
     0,   212,    63,     0,   212,    61,     0,   212,    62,     0,
   212,    49,     0,   212,   113,     0,   212,   104,     0,   212,
   105,     0,   212,    94,     0,     0,    89,   106,   202,   107,
     0,    90,   106,   202,   107,     0,    91,   106,   202,   107,
     0,     0,    79,    58,   167,   106,   114,   107,   108,     0,
    80,    58,   106,   167,   107,   104,   114,   105,     0
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
   486,   487,   488,   489,   490,   491,   492,   493,   493,   494,
   494,   495,   495,   496,   496,   497,   498,   499,   500,   501,
   502,   503,   504,   505,   506,   507,   508,   509,   510,   511,
   512,   513,   514,   515,   516,   517,   518,   519,   520,   521,
   522,   523,   524,   525,   526,   527,   528,   529,   530,   531,
   532,   533,   534,   535,   536,   537,   538,   538,   539,   540,
   541,   542,   543,   547,   549,   550,   551,   552,   553,   554,
   555,   558,   560,   561,   564,   570,   572,   576,   578,   582,
   584,   589,   591,   592,   595,   603,   612,   619,   633,   635,
   636,   637,   637,   641,   643,   644,   647,   648,   652,   654,
   655,   655,   656,   656,   660,   662,   665,   667,   668,   669,
   673,   675,   676,   677,   678,   679,   680,   681,   682,   683,
   684,   685,   686,   687,   688,   689,   690,   691,   692,   693,
   694,   695,   699,   701,   702,   706,   708,   709
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","','","LOGICAL_OR",
"LOGICAL_XOR","LOGICAL_AND","PHP_PRINT","'='","PLUS_EQUAL","MINUS_EQUAL","MUL_EQUAL",
"DIV_EQUAL","CONCAT_EQUAL","MOD_EQUAL","AND_EQUAL","OR_EQUAL","XOR_EQUAL","'?'",
"':'","BOOLEAN_OR","BOOLEAN_AND","'|'","'^'","'&'","IS_EQUAL","IS_NOT_EQUAL",
"'<'","IS_SMALLER_OR_EQUAL","'>'","IS_GREATER_OR_EQUAL","SHIFT_LEFT","SHIFT_RIGHT",
"'+'","'-'","'.'","'*'","'/'","'%'","'!'","'~'","INCREMENT","DECREMENT","INT_CAST",
"DOUBLE_CAST","STRING_CAST","ARRAY_CAST","OBJECT_CAST","'@'","'['","NEW","EXIT",
"IF","ELSEIF","ELSE","ENDIF","LNUMBER","DNUMBER","STRING","NUM_STRING","INLINE_HTML",
"CHARACTER","BAD_CHARACTER","ENCAPSED_AND_WHITESPACE","PHP_ECHO","DO","WHILE",
"ENDWHILE","FOR","ENDFOR","SWITCH","ENDSWITCH","CASE","DEFAULT","BREAK","CONTINUE",
"CONTINUED_WHILE","CONTINUED_DOWHILE","CONTINUED_FOR","OLD_FUNCTION","FUNCTION",
"PHP_CONST","RETURN","INCLUDE","REQUIRE","HIGHLIGHT_FILE","HIGHLIGHT_STRING",
"PHP_GLOBAL","PHP_STATIC","PHP_UNSET","PHP_ISSET","PHP_EMPTY","CLASS","EXTENDS",
"PHP_CLASS_OPERATOR","PHP_DOUBLE_ARROW","PHP_LIST","PHP_ARRAY","VAR","EVAL",
"DONE_EVAL","PHP_LINE","PHP_FILE","STRING_CONSTANT","'{'","'}'","'('","')'",
"';'","'$'","'`'","'\"'","'''","']'","statement_list","statement","@1","@2",
"@3","@4","@5","@6","@7","@8","@9","@10","@11","@12","@13","@14","@15","@16",
"@17","@18","@19","for_statement","switch_case_list","while_statement","while_iterations",
"@20","do_while_iterations","@21","@22","for_iterations","@23","@24","@25","@26",
"elseif_list","@27","@28","@29","new_elseif_list","@30","@31","@32","else_single",
"@33","new_else_single","@34","case_list","@35","@36","@37","@38","@39","@40",
"parameter_list","@41","non_empty_parameter_list","function_call_parameter_list",
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
   114,   114,   115,   116,   115,   117,   118,   115,   119,   120,
   115,   121,   122,   123,   115,   124,   125,   126,   127,   128,
   115,   129,   115,   115,   115,   115,   115,   130,   115,   131,
   115,   115,   115,   115,   115,   132,   115,   133,   115,   115,
   115,   115,   134,   115,   115,   115,   115,   115,   115,   115,
   135,   135,   136,   136,   136,   136,   137,   137,   138,   139,
   138,   140,   141,   142,   140,   143,   144,   145,   146,   147,
   143,   148,   149,   150,   151,   148,   152,   153,   154,   155,
   152,   156,   157,   156,   158,   159,   158,   160,   161,   162,
   160,   163,   160,   164,   165,   160,   166,   160,   168,   167,
   167,   169,   169,   169,   169,   169,   169,   169,   169,   170,
   170,   171,   171,   171,   171,   171,   171,   172,   172,   173,
   173,   173,   173,   174,   174,   175,   175,   176,   177,   176,
   178,   176,   179,   179,   179,   179,   180,   180,   180,   181,
   181,   181,   182,   182,   182,   182,   182,   182,   182,   182,
   182,   182,   182,   182,   182,   182,   182,   183,   182,   184,
   182,   185,   182,   186,   182,   182,   182,   182,   182,   182,
   182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
   182,   182,   182,   182,   182,   182,   182,   187,   188,   182,
   189,   190,   182,   182,   191,   192,   182,   182,   182,   182,
   182,   182,   182,   182,   182,   182,   193,   182,   182,   182,
   182,   182,   182,   194,   194,   194,   194,   194,   194,   194,
   194,   195,   195,   195,   196,   197,   197,   198,   198,   199,
   199,   200,   200,   200,   201,   201,   201,   201,   202,   202,
   202,   203,   202,   204,   204,   204,   206,   205,   207,   207,
   208,   207,   209,   207,   210,   210,   211,   211,   211,   211,
   212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
   212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
   212,   212,   213,   213,   213,   214,   214,   214
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
     3,     3,     3,     2,     2,     2,     2,     0,     4,     0,
     4,     0,     4,     0,     4,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     2,     2,     2,     2,     3,
     3,     3,     3,     3,     3,     3,     3,     0,     0,     7,
     0,     0,     7,     1,     0,     0,    10,     2,     2,     2,
     2,     2,     2,     1,     3,     4,     0,     3,     2,     1,
     4,     3,     2,     1,     1,     3,     3,     1,     1,     1,
     1,     1,     2,     2,     1,     1,     1,     1,     3,     1,
     3,     3,     1,     1,     3,     1,     2,     0,     2,     2,
     4,     0,     7,     2,     1,     2,     0,     4,     4,     3,
     0,     2,     0,     3,     0,     1,     5,     3,     3,     1,
     3,     6,     6,     7,     5,     5,     8,     8,     9,     7,
     6,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     0,     4,     4,     4,     0,     7,     8
};

static const short yydefact[] = {     2,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   204,     0,   214,   215,   219,
    41,   137,    12,     0,    16,     0,     0,     0,     0,     0,
     0,     0,    43,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   220,   221,   218,     2,     0,    50,
     0,   282,   282,   282,     1,   227,   210,   245,     0,   226,
   191,   194,   213,   176,   177,   178,   179,     0,   155,   157,
   199,   200,   201,   202,   203,   209,     0,   225,     0,   198,
     0,     0,     0,   139,     0,     0,     0,     0,    24,     0,
    26,     0,    28,     0,    32,     0,     0,     0,     0,     0,
     0,    34,     0,    35,     0,     0,     0,    36,   238,   255,
     0,     0,     0,     0,   245,   239,     0,     0,   228,   240,
     0,     0,     0,   162,     0,   164,   188,   158,   160,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    42,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   154,   156,     0,   239,     0,
   240,   208,   245,   244,   246,   205,     0,     0,     0,    40,
     0,     0,   140,     0,    25,    27,    99,    30,    33,    49,
     0,    45,     0,   119,     0,   122,     0,     0,     0,     0,
     0,     0,     0,   236,   260,     0,   256,     0,     3,   187,
   186,     0,   247,     0,   277,   272,   273,   275,   276,   274,
   281,   279,   280,     0,   212,   278,   216,   217,     0,   166,
     0,     0,     0,     0,   167,   168,   169,   180,   181,   182,
   183,   184,   185,   171,   172,   170,   173,   174,   175,   144,
   145,   146,   147,   148,   149,   150,   151,   152,   153,   111,
     0,   206,     4,   138,    13,     9,    17,   142,    22,     0,
     0,    99,    44,     0,     0,     0,     0,     0,   283,   284,
   285,    38,   127,   237,     0,     0,   211,     0,     0,   229,
   253,   245,   241,     0,   228,   261,     0,   163,   165,     0,
   159,   161,     0,     0,   110,   227,     0,   226,     6,     0,
     0,     0,     0,     0,     0,     2,     0,     0,     0,   100,
     0,     0,    46,   118,     0,     0,   219,     0,   123,   222,
   124,   120,     0,     0,   235,     0,   259,   258,    48,   248,
     0,     0,   242,     0,     0,     0,     0,     0,   189,   114,
   192,     0,     2,    72,     0,     2,    57,    10,   141,   140,
    88,    88,    23,     0,     0,     0,   102,     0,     0,     0,
   223,   224,     0,     0,   127,     0,     0,     0,    37,   126,
   143,     0,     0,   252,     0,   253,   111,     0,     0,     0,
   265,     0,     0,   266,     0,     0,   286,     0,   227,   226,
    77,    82,     0,     0,    59,    18,     0,     0,    88,     0,
    88,     0,     0,   103,   104,     0,     0,     0,     0,     2,
    47,   125,   121,     0,   129,     0,     0,     0,   257,   250,
     0,   254,   243,     0,   262,   263,     0,     0,     0,     0,
     0,   271,   190,     0,     0,   193,   117,    85,     0,    83,
     5,     0,     0,    11,     0,     0,    92,    97,     0,     0,
     0,    53,    29,   105,     0,     0,   106,     0,    39,    99,
   131,   135,     0,   128,   249,   196,   264,     0,     0,     0,
   270,    99,     0,     0,     0,     7,    73,     0,    14,    58,
     0,   140,    89,    94,     2,     2,     0,    55,    54,   107,
   108,     0,    31,     0,    99,     0,     0,   286,   267,   268,
     0,     0,    99,    78,    86,     0,     0,    84,    62,     0,
    19,     2,     2,    93,    98,    56,   109,     2,     0,   136,
   133,   197,   269,     2,     0,     0,     2,     0,    74,    15,
     0,     0,    90,    95,     0,     0,     0,     0,     0,    79,
    87,     8,     0,    63,    60,     0,    88,    88,     0,     2,
   134,     0,     2,     0,    75,     0,     0,     2,    51,    20,
    91,    96,   130,     0,   287,     0,     0,     0,     0,    61,
     0,    66,   132,   288,    80,    76,    64,     0,    21,     2,
     0,    52,    67,    81,     0,     0,     0,   140,     0,    68,
    65,     0,   140,    69,     0,   140,    70,     0,     0,    71,
     0,     0
};

static const short yydefgoto[] = {     1,
    55,   300,   343,   506,   302,   395,    85,   301,   509,    87,
   304,   445,   532,   572,   305,   177,   262,   192,   323,    98,
   560,   353,   348,   444,   557,   530,   556,   581,   579,   586,
   592,   595,   598,   392,   507,   543,   568,   438,   526,   554,
   580,   441,   478,   476,   527,   400,   512,   547,   485,   513,
   548,   486,   260,   261,   310,   294,   295,   102,   104,   319,
   324,   370,   460,   495,   418,    83,   257,    56,   223,   224,
   219,   221,   222,   386,   158,   387,   334,   498,    77,    57,
   321,    58,    59,   283,   117,   118,   193,    60,   376,    61,
   120,   281,   330,   331,   332,   196,   197,   121,    62,   436
};

static const short yypact[] = {-32768,
   556,  2116,  2116,  2116,  2116,  2116,   -76,   -76,  2116,  2116,
  2116,  2116,  2116,   649,   -21,   -55,   -53,-32768,-32768,   -51,
-32768,  2116,-32768,   -42,-32768,   -33,  1792,  1820,    27,    29,
  1900,  2116,-32768,  2116,   -25,   -20,   -18,   -13,    -7,     0,
    45,    48,    54,    66,-32768,-32768,-32768,-32768,  2116,-32768,
   -27,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  2260,   661,
-32768,-32768,  3134,   161,   161,-32768,-32768,   -27,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  2116,-32768,   -27,-32768,
  1928,  2116,    16,  3062,  1706,  2116,    69,  2116,-32768,  2301,
-32768,  2336,-32768,    71,-32768,  2377,  2412,  2116,  2453,  2116,
   -21,   118,   -21,   197,   -76,   -76,   -76,   108,   -76,  2116,
  2116,   758,   544,  2116,     7,    98,   164,   120,-32768,   -40,
   232,  2931,   434,-32768,  2116,-32768,-32768,-32768,-32768,  2116,
  2116,  2116,  2116,  2116,  2116,  2116,  2116,  2116,  2116,  2116,
  2116,  2116,  2116,  2116,-32768,  2116,  2116,  2116,  2116,  2116,
  2116,  2116,  2116,  2116,  2116,-32768,-32768,   109,-32768,   122,
   124,-32768,   175,-32768,-32768,-32768,  2529,  2564,  2116,-32768,
   160,  2604,  2116,  2639,-32768,-32768,   121,-32768,-32768,-32768,
  2488,-32768,  2165,-32768,   125,   221,   140,   126,   145,   148,
   193,   154,    15,-32768,  2939,   153,   258,  2679,-32768,-32768,
-32768,  2904,-32768,   -27,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    -9,-32768,-32768,-32768,-32768,  2116,  3113,
  2116,  2116,  2116,  2116,  3204,  3219,  3233,  3247,  3247,   285,
   285,   285,   285,   161,   161,   161,-32768,-32768,-32768,  3134,
  3134,  3134,  3134,  3134,  3134,  3134,  3134,  3134,  3134,  2008,
   -27,-32768,   247,  3062,-32768,-32768,   265,  3062,-32768,   166,
    93,   170,-32768,  2116,   165,   -21,   335,   -21,-32768,-32768,
-32768,-32768,-32768,   -76,   270,  2116,-32768,  2116,   172,   236,
   173,    11,-32768,   239,   183,    13,   -12,  3088,  3134,  3027,
  3171,  3188,   -76,   185,   293,    18,  3062,   228,-32768,  1706,
   191,   846,  2116,   190,    23,-32768,   195,   196,   241,   303,
   200,  2714,-32768,-32768,   347,   347,-32768,  2116,-32768,-32768,
-32768,   300,   205,   130,-32768,  2116,  3062,  2977,-32768,   268,
   212,  2116,-32768,   222,   -19,   269,   -22,   271,-32768,-32768,
-32768,  2036,-32768,-32768,  2116,-32768,-32768,-32768,  3062,  2116,
    -4,    97,-32768,   932,   241,   241,   323,   144,   229,   224,
-32768,-32768,  2754,   335,-32768,   276,   280,   230,-32768,-32768,
  3134,  2116,   677,-32768,   158,   173,  2008,   231,   233,   289,
-32768,   -15,   292,-32768,   248,  2116,    31,   -76,    49,   346,
  1706,     4,  2789,  1018,-32768,   265,  2116,     5,    85,   281,
    85,   260,   266,-32768,-32768,   347,   257,   264,   241,-32768,
-32768,-32768,-32768,   177,-32768,   272,   241,    35,  3062,-32768,
   396,-32768,   268,   275,-32768,-32768,   263,   277,   283,   319,
   274,-32768,  3153,   325,   327,-32768,-32768,   112,   288,-32768,
-32768,   290,   291,   313,   298,  2225,-32768,-32768,   336,   301,
   307,-32768,-32768,-32768,   241,   241,   405,  1104,-32768,   121,
-32768,   407,   318,-32768,-32768,-32768,-32768,   330,   334,   315,
-32768,   121,   338,   339,   421,-32768,-32768,  1706,-32768,-32768,
   345,  2116,-32768,-32768,-32768,-32768,   348,-32768,-32768,-32768,
-32768,   347,-32768,   349,   170,  2116,   241,    31,-32768,-32768,
   352,   354,   170,-32768,-32768,   399,  2116,-32768,-32768,  2116,
   265,-32768,-32768,  1706,  1706,-32768,-32768,-32768,   355,  3062,
   453,-32768,-32768,-32768,   356,  2116,-32768,   357,  3062,   387,
  2829,   359,  1706,  1706,  1190,   363,  2116,  1276,   364,  3062,
  1706,-32768,   365,-32768,-32768,  1362,    85,    85,   366,-32768,
  3062,   369,-32768,   371,-32768,  1706,   846,-32768,-32768,-32768,
-32768,-32768,-32768,  1448,-32768,  1534,   452,  1706,   413,-32768,
  1620,-32768,-32768,-32768,-32768,-32768,-32768,   374,   395,-32768,
   379,-32768,-32768,  1706,  2116,   382,  2864,  2116,   381,   265,
-32768,   383,  2116,   265,   391,  2116,   265,   393,  1362,-32768,
   490,-32768
};

static const short yypgoto[] = {   -43,
   -70,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   -98,-32768,   -54,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,  -340,-32768,-32768,-32768,-32768,
-32768,-32768,  -249,-32768,-32768,   127,-32768,-32768,-32768,   138,
   141,-32768,-32768,-32768,-32768,-32768,  -309,  -222,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  -245,
  -280,   -45,    -2,    -3,  -159,   443,-32768,     9,-32768,    -1,
    -5,-32768,   137,-32768,-32768,-32768,-32768,   136,-32768,    14
};


#define	YYLAST		3285


static const short yytable[] = {    63,
    64,    65,    66,    67,   112,   115,    71,    72,    73,    74,
    75,   402,   311,    80,   171,    69,    70,   274,   169,    84,
  -112,   320,   115,   447,    90,    92,   382,   296,    96,    97,
    78,    99,    68,   163,   361,   362,    78,   463,   378,   379,
   396,   351,   428,   429,   284,   337,   113,   116,   286,   119,
    81,  -115,    82,  -233,  -225,  -230,   439,   440,   449,  -230,
   451,   335,   161,    86,   159,  -246,   119,   397,   398,   320,
   320,   383,    88,   165,   162,   164,   114,   119,   167,   168,
   100,    79,   384,   172,    93,   174,    94,    79,   101,   380,
   103,   284,   105,   430,   287,   181,   338,   183,   106,   184,
  -234,   186,   108,   399,  -232,   107,   336,   195,   198,   434,
   435,   202,   448,   188,   189,   190,   307,   194,   320,   389,
   185,   275,   220,   170,  -112,   454,   352,   225,   226,   227,
   228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
   238,   239,   464,   240,   241,   242,   243,   244,   245,   246,
   247,   248,   249,   109,   296,  -115,   397,   398,   282,   110,
   320,   124,   125,   126,   474,   475,   254,   407,   397,   398,
   258,   111,   511,   308,   173,   127,   178,   128,   129,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   122,   123,
   139,   140,   141,   142,   143,   144,   142,   143,   144,   187,
   191,   309,   285,  -244,   401,   282,   561,   562,   366,   367,
   494,   517,   203,   204,   250,   251,   288,  -233,   289,   290,
   291,   292,   502,  -230,   408,   255,  -101,   368,   267,   344,
  -113,   347,   269,   266,   369,   146,   147,   148,   149,   150,
   151,   152,   153,   154,   155,   519,   320,   297,   268,   119,
   272,   270,   409,   525,   271,   366,   367,   273,   298,   277,
   278,   312,   354,   357,   314,   299,   322,   303,   156,   157,
   422,   306,   313,   327,   368,   328,  -101,   326,   590,   329,
   205,   459,   325,   594,  -231,  -251,   597,   333,  -195,   206,
   207,   341,   208,   209,   210,   342,   345,   350,    78,   391,
   349,   340,   394,   355,   356,   358,   359,   364,   365,   404,
   405,-32768,-32768,-32768,-32768,   363,   373,   139,   140,   141,
   142,   143,   144,   371,   374,   211,   381,   377,   385,   375,
   406,   411,   410,   415,  -113,   212,   213,   416,   417,   297,
   214,   215,   393,   425,   216,   426,   427,   258,  -116,   431,
   390,   450,   432,   146,   147,   148,   149,   150,   151,   152,
   153,   154,   155,   457,   452,   455,   458,   315,   316,   419,
   421,   462,   456,   453,   297,   467,   470,   461,   471,   315,
   316,   466,   472,   433,   473,   298,   156,   157,   481,   468,
    18,    19,   317,   477,   446,   469,   437,   479,   480,   124,
   125,   126,    18,    19,   317,   482,   487,   508,   488,   490,
   491,   489,   492,   127,   496,   128,   129,   130,   131,   132,
   133,   134,   135,   136,   137,   138,   497,   501,   139,   140,
   141,   142,   143,   144,   499,    45,    46,    47,   500,   505,
   318,   514,   515,   503,   504,    53,    54,    45,    46,    47,
   510,   521,  -116,   528,   518,   516,   523,    53,    54,   524,
   537,   536,   539,   544,   542,   546,   550,   553,   533,   534,
   575,   555,   583,   563,   535,   559,   565,   567,   577,   258,
   538,   582,   205,   541,   585,   569,   347,   588,   591,   602,
   593,   206,   207,   520,   208,   209,   210,   576,   596,   599,
   600,   413,   570,   424,   529,   414,   564,   531,   465,   566,
   160,   522,   423,     0,   571,     0,     0,     0,     0,     0,
     0,     0,     0,   540,     0,     0,     0,   211,   559,     0,
     0,     0,     0,     0,   551,     0,   584,   212,   213,     0,
     0,     0,   214,     0,   200,   218,   216,   124,   125,   126,
     0,     0,     0,     0,     0,   601,     0,     0,     0,     0,
     0,   127,     2,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,     0,     0,   139,   140,   141,   142,
   143,   144,   587,     0,     0,   258,     0,     0,     3,     4,
   258,     0,     0,   258,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,    17,     0,     0,
     0,    18,    19,    20,     0,    21,     0,     0,     0,    22,
    23,    24,     0,    25,     0,    26,     0,     0,     0,    27,
    28,     0,     0,     0,    29,    30,     0,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,     0,    76,
   201,    42,    43,     0,    44,  -207,    45,    46,    47,    48,
     0,    49,     0,    50,    51,    52,    53,    54,   146,   147,
   148,   149,   150,   151,   152,   153,   154,   155,     0,     0,
     0,  -207,  -207,     2,     0,     0,     0,  -207,  -207,  -207,
  -207,  -207,  -207,  -207,  -207,  -207,  -207,     0,  -207,  -207,
     0,   156,   157,     0,  -207,  -207,  -207,     0,     0,     3,
     4,     0,     0,     0,     0,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,     0,    15,    16,     0,     0,
     0,     0,    18,    19,    20,     0,     0,  -207,  -207,  -207,
     0,     0,     0,     0,  -207,  -207,     0,     0,     0,  -207,
  -207,  -207,     0,     0,  -207,     0,     0,  -207,  -207,  -207,
  -207,     0,     0,     0,     2,    38,    39,    40,     0,     0,
     0,     0,    42,    43,     0,     0,     0,    45,    46,    47,
     0,     0,    49,     0,     0,    51,    52,    53,    54,   420,
     3,     4,     0,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,    17,
     0,     0,     0,    18,    19,    20,     0,    21,     0,     0,
     0,    22,    23,    24,     0,    25,     0,    26,     0,     0,
     0,    27,    28,     0,     0,     0,    29,    30,     0,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
     0,     0,     2,    42,    43,     0,    44,     0,    45,    46,
    47,    48,   199,    49,   346,    50,    51,    52,    53,    54,
     0,     0,     0,     0,     0,     0,     0,     0,     3,     4,
     0,     0,     0,     0,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,    17,     0,     0,
     0,    18,    19,    20,     0,    21,     0,     0,     0,    22,
    23,    24,     0,    25,     0,    26,     0,     0,     0,    27,
    28,     0,     0,     0,    29,    30,     0,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,     2,     0,
     0,    42,    43,     0,    44,     0,    45,    46,    47,    48,
     0,    49,     0,    50,    51,    52,    53,    54,     0,     0,
     0,     0,     0,     0,     3,     4,     0,     0,     0,     0,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     0,    15,    16,    17,     0,     0,     0,    18,    19,    20,
     0,    21,     0,     0,     0,    22,    23,    24,     0,    25,
     0,    26,     0,     0,     0,    27,    28,     0,     0,     0,
    29,    30,     0,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,     2,     0,     0,    42,    43,     0,
    44,     0,    45,    46,    47,    48,     0,    49,   403,    50,
    51,    52,    53,    54,     0,     0,     0,     0,     0,     0,
     3,     4,     0,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,    17,
     0,     0,     0,    18,    19,    20,     0,    21,     0,     0,
     0,    22,    23,    24,   443,    25,     0,    26,     0,     0,
     0,    27,    28,     0,     0,     0,    29,    30,     0,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
     2,     0,     0,    42,    43,     0,    44,     0,    45,    46,
    47,    48,     0,    49,     0,    50,    51,    52,    53,    54,
     0,     0,     0,     0,     0,     0,     3,     4,     0,     0,
     0,     0,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,     0,    15,    16,    17,     0,     0,     0,    18,
    19,    20,     0,    21,     0,     0,     0,    22,    23,    24,
     0,    25,     0,    26,     0,     0,     0,    27,    28,     0,
     0,     0,    29,    30,     0,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,     2,     0,     0,    42,
    43,     0,    44,     0,    45,    46,    47,    48,   493,    49,
     0,    50,    51,    52,    53,    54,     0,     0,     0,     0,
     0,     0,     3,     4,     0,     0,     0,     0,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,    17,     0,     0,     0,    18,    19,    20,     0,    21,
     0,     0,     0,    22,    23,    24,     0,    25,     0,    26,
     0,     0,     0,    27,    28,     0,     0,     0,    29,    30,
     0,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,     2,     0,     0,    42,    43,     0,    44,     0,
    45,    46,    47,    48,     0,    49,   549,    50,    51,    52,
    53,    54,     0,     0,     0,     0,     0,     0,     3,     4,
     0,     0,     0,     0,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,    17,     0,     0,
     0,    18,    19,    20,     0,    21,     0,     0,     0,    22,
    23,    24,     0,    25,     0,    26,     0,     0,     0,    27,
    28,     0,     0,     0,    29,    30,     0,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,     2,     0,
     0,    42,    43,     0,    44,     0,    45,    46,    47,    48,
   558,    49,   552,    50,    51,    52,    53,    54,     0,     0,
     0,     0,     0,     0,     3,     4,     0,     0,     0,     0,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     0,    15,    16,    17,     0,     0,     0,    18,    19,    20,
     0,    21,     0,     0,     0,    22,    23,    24,     0,    25,
     0,    26,     0,     0,     0,    27,    28,     0,     0,     0,
    29,    30,     0,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,     2,     0,     0,    42,    43,     0,
    44,     0,    45,    46,    47,    48,     0,    49,     0,    50,
    51,    52,    53,    54,     0,     0,     0,     0,     0,     0,
     3,     4,     0,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,    17,
     0,     0,     0,    18,    19,    20,     0,    21,     0,     0,
     0,    22,    23,    24,     0,    25,     0,    26,     0,     0,
     0,    27,    28,     0,     0,     0,    29,    30,     0,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
     2,     0,     0,    42,    43,     0,    44,     0,    45,    46,
    47,    48,   573,    49,     0,    50,    51,    52,    53,    54,
     0,     0,     0,     0,     0,     0,     3,     4,     0,     0,
     0,     0,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,     0,    15,    16,    17,     0,     0,     0,    18,
    19,    20,     0,    21,     0,     0,     0,    22,    23,    24,
     0,    25,     0,    26,     0,     0,     0,    27,    28,     0,
     0,     0,    29,    30,     0,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,     2,     0,     0,    42,
    43,     0,    44,     0,    45,    46,    47,    48,   574,    49,
     0,    50,    51,    52,    53,    54,     0,     0,     0,     0,
     0,     0,     3,     4,     0,     0,     0,     0,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,    17,     0,     0,     0,    18,    19,    20,     0,    21,
     0,     0,     0,    22,    23,    24,     0,    25,   578,    26,
     0,     0,     0,    27,    28,     0,     0,     0,    29,    30,
     0,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,     2,     0,     0,    42,    43,     0,    44,     0,
    45,    46,    47,    48,     0,    49,     0,    50,    51,    52,
    53,    54,     0,     0,     0,     0,     0,     0,     3,     4,
     0,     0,     0,     0,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,    17,     0,     0,
     0,    18,    19,    20,     0,    21,     0,     0,     0,    22,
    23,    24,     0,    25,     0,    26,     0,     0,     0,    27,
    28,     0,     0,     0,    29,    30,     0,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,     2,     0,
     0,    42,    43,     0,    44,     0,    45,    46,    47,    48,
     0,    49,     0,    50,    51,    52,    53,    54,     0,     0,
     0,     0,     0,     0,     3,     4,     2,     0,     0,     0,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     0,    15,    16,     0,     0,     0,     0,    18,    19,    20,
     0,     0,     3,     4,     0,     0,     0,     0,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,     0,     0,     0,     0,    18,    19,    20,     0,     0,
    38,    39,    40,     0,     0,     0,     0,    42,    43,     0,
     0,     0,    45,    46,    47,     0,     0,    49,     0,    89,
    51,    52,    53,    54,     0,     0,     2,     0,    38,    39,
    40,     0,     0,     0,     0,    42,    43,     0,     0,     0,
    45,    46,    47,     0,     0,    49,     0,    91,    51,    52,
    53,    54,     3,     4,     2,     0,     0,     0,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,     0,     0,     0,     0,    18,    19,    20,     0,     0,
     3,     4,     0,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,     0,
     0,     0,     0,    18,    19,    20,     0,     0,    38,    39,
    40,     0,     0,     0,     0,    42,    43,     0,     0,     0,
    45,    46,    47,     0,     0,    49,     0,    95,    51,    52,
    53,    54,     0,     0,     2,     0,    38,    39,    40,     0,
     0,     0,     0,    42,    43,     0,     0,     0,    45,    46,
    47,   293,     0,    49,   166,     0,    51,    52,    53,    54,
     3,     4,     2,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,   388,
     0,     0,     0,    18,    19,    20,     0,     0,     3,     4,
     0,     0,     0,     0,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,     0,     0,     0,
     0,    18,    19,    20,     0,     0,    38,    39,    40,     0,
     0,     0,     0,    42,    43,     0,     0,     0,    45,    46,
    47,     0,     0,    49,     0,     0,    51,    52,    53,    54,
     0,     0,     2,     0,    38,    39,    40,     0,     0,     0,
     0,    42,    43,     0,     0,     0,    45,    46,    47,     0,
     0,    49,     0,     0,    51,    52,    53,    54,     3,     4,
     0,     0,     0,     0,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,   264,   124,   125,
   126,    18,    19,    20,     0,     0,     0,     0,     0,     0,
     0,     0,   127,     0,   128,   129,   130,   131,   132,   133,
   134,   135,   136,   137,   138,     0,     0,   139,   140,   141,
   142,   143,   144,     0,    38,    39,    40,     0,     0,     0,
     0,    42,    43,     0,     0,     0,    45,    46,    47,     0,
     0,    49,     0,     0,    51,    52,    53,    54,   124,   125,
   126,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   127,   483,   128,   129,   130,   131,   132,   133,
   134,   135,   136,   137,   138,     0,     0,   139,   140,   141,
   142,   143,   144,   124,   125,   126,     0,     0,     0,     0,
     0,   265,     0,     0,     0,     0,     0,   127,     0,   128,
   129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     0,     0,   139,   140,   141,   142,   143,   144,     0,     0,
     0,     0,     0,     0,   124,   125,   126,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   127,     0,
   128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
   138,     0,   484,   139,   140,   141,   142,   143,   144,   124,
   125,   126,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   127,     0,   128,   129,   130,   131,   132,
   133,   134,   135,   136,   137,   138,     0,   145,   139,   140,
   141,   142,   143,   144,     0,     0,     0,     0,     0,     0,
   124,   125,   126,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   127,     0,   128,   129,   130,   131,
   132,   133,   134,   135,   136,   137,   138,     0,   175,   139,
   140,   141,   142,   143,   144,   124,   125,   126,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   127,
     0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,     0,   176,   139,   140,   141,   142,   143,   144,
     0,     0,     0,     0,     0,     0,   124,   125,   126,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   127,     0,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,     0,   179,   139,   140,   141,   142,   143,
   144,   124,   125,   126,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   127,     0,   128,   129,   130,
   131,   132,   133,   134,   135,   136,   137,   138,     0,   180,
   139,   140,   141,   142,   143,   144,     0,     0,     0,     0,
     0,     0,   124,   125,   126,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   127,     0,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   137,   138,     0,
   182,   139,   140,   141,   142,   143,   144,   124,   125,   126,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   127,     0,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,     0,   263,   139,   140,   141,   142,
   143,   144,     0,     0,     0,     0,     0,   124,   125,   126,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   127,     0,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,     0,   252,   139,   140,   141,   142,
   143,   144,   124,   125,   126,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   127,     0,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   137,   138,     0,
   253,   139,   140,   141,   142,   143,   144,     0,     0,     0,
     0,     0,   124,   125,   126,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   127,     0,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   137,   138,     0,
   256,   139,   140,   141,   142,   143,   144,   124,   125,   126,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   127,     0,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,     0,   259,   139,   140,   141,   142,
   143,   144,     0,     0,     0,     0,     0,   124,   125,   126,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   127,     0,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,     0,   279,   139,   140,   141,   142,
   143,   144,   124,   125,   126,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   127,     0,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   137,   138,     0,
   360,   139,   140,   141,   142,   143,   144,     0,     0,     0,
     0,     0,   124,   125,   126,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   127,     0,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   137,   138,     0,
   412,   139,   140,   141,   142,   143,   144,   124,   125,   126,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   127,     0,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,     0,   442,   139,   140,   141,   142,
   143,   144,     0,     0,     0,     0,     0,   124,   125,   126,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   127,     0,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,     0,   545,   139,   140,   141,   142,
   143,   144,   124,   125,   126,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   127,     0,   128,   129,
   130,   131,   132,   133,   134,   135,   136,   137,   138,     0,
   589,   139,   140,   141,   142,   143,   144,     0,     0,   205,
   124,   125,   126,     0,     0,     0,     0,     0,   206,   207,
     0,   208,   209,   210,   127,     0,   128,   129,   130,   131,
   132,   133,   134,   135,   136,   137,   138,     0,   280,   139,
   140,   141,   142,   143,   144,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   211,     0,     0,     0,     0,     0,
   124,   125,   126,   276,   212,   213,     0,     0,     0,   214,
     0,   217,     0,   216,   127,   339,   128,   129,   130,   131,
   132,   133,   134,   135,   136,   137,   138,     0,     0,   139,
   140,   141,   142,   143,   144,   124,   125,   126,     0,     0,
     0,   372,     0,     0,     0,     0,     0,     0,     0,   127,
     0,   128,   129,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   125,   126,   139,   140,   141,   142,   143,   144,
     0,     0,     0,     0,     0,   127,     0,   128,   129,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   126,     0,
   139,   140,   141,   142,   143,   144,     0,     0,     0,     0,
   127,     0,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,     0,     0,   139,   140,   141,   142,   143,
   144,   127,     0,   128,   129,   130,   131,   132,   133,   134,
   135,   136,   137,   138,     0,     0,   139,   140,   141,   142,
   143,   144,   128,   129,   130,   131,   132,   133,   134,   135,
   136,   137,   138,     0,     0,   139,   140,   141,   142,   143,
   144,   129,   130,   131,   132,   133,   134,   135,   136,   137,
   138,     0,     0,   139,   140,   141,   142,   143,   144,   130,
   131,   132,   133,   134,   135,   136,   137,   138,     0,     0,
   139,   140,   141,   142,   143,   144,   131,   132,   133,   134,
   135,   136,   137,   138,     0,     0,   139,   140,   141,   142,
   143,   144,   132,   133,   134,   135,   136,   137,   138,     0,
     0,   139,   140,   141,   142,   143,   144,   133,   134,   135,
   136,   137,   138,     0,     0,   139,   140,   141,   142,   143,
   144,-32768,-32768,   135,   136,   137,   138,     0,     0,   139,
   140,   141,   142,   143,   144
};

static const short yycheck[] = {     2,
     3,     4,     5,     6,    48,    51,     9,    10,    11,    12,
    13,   352,   262,    15,    85,     7,     8,     3,     3,    22,
     3,   267,    68,    19,    27,    28,    49,   250,    31,    32,
    58,    34,   109,    79,   315,   316,    58,     3,    58,    59,
   350,    19,    58,    59,   204,    58,    49,    51,    58,    51,
   106,     3,   106,    94,   106,    49,    53,    54,   399,    49,
   401,    49,    68,   106,    68,   106,    68,    72,    73,   315,
   316,    94,   106,    79,    77,    79,   104,    79,    81,    82,
   106,   109,   105,    86,    58,    88,    58,   109,   109,   109,
   109,   251,   106,   109,   104,    98,   109,   100,   106,   101,
    94,   103,    58,   108,    94,   106,    94,   110,   111,    79,
    80,   114,   108,   105,   106,   107,    24,   109,   364,   342,
     3,   107,   125,   108,   107,   406,   104,   130,   131,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   108,   146,   147,   148,   149,   150,   151,   152,
   153,   154,   155,   106,   377,   107,    72,    73,   204,   106,
   406,     4,     5,     6,    53,    54,   169,    24,    72,    73,
   173,   106,   482,    81,   106,    18,   106,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    53,    54,
    33,    34,    35,    36,    37,    38,    36,    37,    38,     3,
    93,   109,   204,   106,   108,   251,   547,   548,    79,    80,
   460,   492,    49,    94,   106,    94,   219,    94,   221,   222,
   223,   224,   472,    49,    81,    66,   106,    98,     8,   300,
     3,   302,   107,   109,   105,     8,     9,    10,    11,    12,
    13,    14,    15,    16,    17,   495,   492,   250,   109,   251,
    58,   107,   109,   503,   107,    79,    80,   104,   250,   107,
     3,   264,   306,   309,   266,    19,   268,     3,    41,    42,
   113,   106,   108,   276,    98,   278,   107,     8,   588,   108,
    49,   105,   274,   593,    49,   113,   596,    49,   106,    58,
    59,   107,    61,    62,    63,     3,   106,   108,    58,   343,
   303,   293,   346,   109,   109,     3,   107,     8,   104,   355,
   356,    27,    28,    29,    30,   318,    49,    33,    34,    35,
    36,    37,    38,   326,   113,    94,    58,   106,    58,   332,
     8,   108,   104,    58,   107,   104,   105,    58,   109,   342,
   109,   110,   345,   113,   113,   113,    58,   350,     3,    58,
   342,    71,   105,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,   409,   105,   109,   410,    33,    34,   372,
   373,   417,   109,   108,   377,   113,    58,   106,   105,    33,
    34,   107,    58,   386,    58,   377,    41,    42,    76,   113,
    56,    57,    58,   106,   397,   113,   388,   108,   108,     4,
     5,     6,    56,    57,    58,   108,    71,   478,   108,   455,
   456,   105,     8,    18,     8,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,   109,   113,    33,    34,
    35,    36,    37,    38,   105,   101,   102,   103,   105,    19,
   106,   485,   486,   106,   106,   111,   112,   101,   102,   103,
   106,   497,   107,    55,   106,   108,   105,   111,   112,   106,
     8,   107,   107,    77,   108,   107,   104,   104,   512,   513,
    19,   107,    78,   108,   518,   546,   108,   107,    66,   482,
   524,   108,    49,   527,   106,   556,   557,   106,   108,     0,
   108,    58,    59,   496,    61,    62,    63,   568,   108,   107,
   599,   364,   557,   377,   507,   365,   550,   510,   113,   553,
    68,   498,   376,    -1,   558,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,   526,    -1,    -1,    -1,    94,   599,    -1,
    -1,    -1,    -1,    -1,   537,    -1,   580,   104,   105,    -1,
    -1,    -1,   109,    -1,     1,   112,   113,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,    -1,    -1,
    -1,    18,     7,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,    -1,    33,    34,    35,    36,
    37,    38,   585,    -1,    -1,   588,    -1,    -1,    33,    34,
   593,    -1,    -1,   596,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    -1,    50,    51,    52,    -1,    -1,
    -1,    56,    57,    58,    -1,    60,    -1,    -1,    -1,    64,
    65,    66,    -1,    68,    -1,    70,    -1,    -1,    -1,    74,
    75,    -1,    -1,    -1,    79,    80,    -1,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    -1,     1,
   107,    96,    97,    -1,    99,     7,   101,   102,   103,   104,
    -1,   106,    -1,   108,   109,   110,   111,   112,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    -1,    -1,
    -1,    33,    34,     7,    -1,    -1,    -1,    39,    40,    41,
    42,    43,    44,    45,    46,    47,    48,    -1,    50,    51,
    -1,    41,    42,    -1,    56,    57,    58,    -1,    -1,    33,
    34,    -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,
    44,    45,    46,    47,    48,    -1,    50,    51,    -1,    -1,
    -1,    -1,    56,    57,    58,    -1,    -1,    89,    90,    91,
    -1,    -1,    -1,    -1,    96,    97,    -1,    -1,    -1,   101,
   102,   103,    -1,    -1,   106,    -1,    -1,   109,   110,   111,
   112,    -1,    -1,    -1,     7,    89,    90,    91,    -1,    -1,
    -1,    -1,    96,    97,    -1,    -1,    -1,   101,   102,   103,
    -1,    -1,   106,    -1,    -1,   109,   110,   111,   112,   113,
    33,    34,    -1,    -1,    -1,    -1,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    -1,    50,    51,    52,
    -1,    -1,    -1,    56,    57,    58,    -1,    60,    -1,    -1,
    -1,    64,    65,    66,    -1,    68,    -1,    70,    -1,    -1,
    -1,    74,    75,    -1,    -1,    -1,    79,    80,    -1,    82,
    83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
    -1,    -1,     7,    96,    97,    -1,    99,    -1,   101,   102,
   103,   104,   105,   106,    19,   108,   109,   110,   111,   112,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,
    -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    -1,    50,    51,    52,    -1,    -1,
    -1,    56,    57,    58,    -1,    60,    -1,    -1,    -1,    64,
    65,    66,    -1,    68,    -1,    70,    -1,    -1,    -1,    74,
    75,    -1,    -1,    -1,    79,    80,    -1,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,     7,    -1,
    -1,    96,    97,    -1,    99,    -1,   101,   102,   103,   104,
    -1,   106,    -1,   108,   109,   110,   111,   112,    -1,    -1,
    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
    -1,    50,    51,    52,    -1,    -1,    -1,    56,    57,    58,
    -1,    60,    -1,    -1,    -1,    64,    65,    66,    -1,    68,
    -1,    70,    -1,    -1,    -1,    74,    75,    -1,    -1,    -1,
    79,    80,    -1,    82,    83,    84,    85,    86,    87,    88,
    89,    90,    91,    92,     7,    -1,    -1,    96,    97,    -1,
    99,    -1,   101,   102,   103,   104,    -1,   106,   107,   108,
   109,   110,   111,   112,    -1,    -1,    -1,    -1,    -1,    -1,
    33,    34,    -1,    -1,    -1,    -1,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    -1,    50,    51,    52,
    -1,    -1,    -1,    56,    57,    58,    -1,    60,    -1,    -1,
    -1,    64,    65,    66,    67,    68,    -1,    70,    -1,    -1,
    -1,    74,    75,    -1,    -1,    -1,    79,    80,    -1,    82,
    83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
     7,    -1,    -1,    96,    97,    -1,    99,    -1,   101,   102,
   103,   104,    -1,   106,    -1,   108,   109,   110,   111,   112,
    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,
    -1,    -1,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    -1,    50,    51,    52,    -1,    -1,    -1,    56,
    57,    58,    -1,    60,    -1,    -1,    -1,    64,    65,    66,
    -1,    68,    -1,    70,    -1,    -1,    -1,    74,    75,    -1,
    -1,    -1,    79,    80,    -1,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91,    92,     7,    -1,    -1,    96,
    97,    -1,    99,    -1,   101,   102,   103,   104,   105,   106,
    -1,   108,   109,   110,   111,   112,    -1,    -1,    -1,    -1,
    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    -1,    50,
    51,    52,    -1,    -1,    -1,    56,    57,    58,    -1,    60,
    -1,    -1,    -1,    64,    65,    66,    -1,    68,    -1,    70,
    -1,    -1,    -1,    74,    75,    -1,    -1,    -1,    79,    80,
    -1,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,     7,    -1,    -1,    96,    97,    -1,    99,    -1,
   101,   102,   103,   104,    -1,   106,   107,   108,   109,   110,
   111,   112,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,
    -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    -1,    50,    51,    52,    -1,    -1,
    -1,    56,    57,    58,    -1,    60,    -1,    -1,    -1,    64,
    65,    66,    -1,    68,    -1,    70,    -1,    -1,    -1,    74,
    75,    -1,    -1,    -1,    79,    80,    -1,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,     7,    -1,
    -1,    96,    97,    -1,    99,    -1,   101,   102,   103,   104,
    19,   106,   107,   108,   109,   110,   111,   112,    -1,    -1,
    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
    -1,    50,    51,    52,    -1,    -1,    -1,    56,    57,    58,
    -1,    60,    -1,    -1,    -1,    64,    65,    66,    -1,    68,
    -1,    70,    -1,    -1,    -1,    74,    75,    -1,    -1,    -1,
    79,    80,    -1,    82,    83,    84,    85,    86,    87,    88,
    89,    90,    91,    92,     7,    -1,    -1,    96,    97,    -1,
    99,    -1,   101,   102,   103,   104,    -1,   106,    -1,   108,
   109,   110,   111,   112,    -1,    -1,    -1,    -1,    -1,    -1,
    33,    34,    -1,    -1,    -1,    -1,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    -1,    50,    51,    52,
    -1,    -1,    -1,    56,    57,    58,    -1,    60,    -1,    -1,
    -1,    64,    65,    66,    -1,    68,    -1,    70,    -1,    -1,
    -1,    74,    75,    -1,    -1,    -1,    79,    80,    -1,    82,
    83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
     7,    -1,    -1,    96,    97,    -1,    99,    -1,   101,   102,
   103,   104,   105,   106,    -1,   108,   109,   110,   111,   112,
    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,    -1,    -1,
    -1,    -1,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    -1,    50,    51,    52,    -1,    -1,    -1,    56,
    57,    58,    -1,    60,    -1,    -1,    -1,    64,    65,    66,
    -1,    68,    -1,    70,    -1,    -1,    -1,    74,    75,    -1,
    -1,    -1,    79,    80,    -1,    82,    83,    84,    85,    86,
    87,    88,    89,    90,    91,    92,     7,    -1,    -1,    96,
    97,    -1,    99,    -1,   101,   102,   103,   104,   105,   106,
    -1,   108,   109,   110,   111,   112,    -1,    -1,    -1,    -1,
    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    -1,    50,
    51,    52,    -1,    -1,    -1,    56,    57,    58,    -1,    60,
    -1,    -1,    -1,    64,    65,    66,    -1,    68,    69,    70,
    -1,    -1,    -1,    74,    75,    -1,    -1,    -1,    79,    80,
    -1,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,     7,    -1,    -1,    96,    97,    -1,    99,    -1,
   101,   102,   103,   104,    -1,   106,    -1,   108,   109,   110,
   111,   112,    -1,    -1,    -1,    -1,    -1,    -1,    33,    34,
    -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    -1,    50,    51,    52,    -1,    -1,
    -1,    56,    57,    58,    -1,    60,    -1,    -1,    -1,    64,
    65,    66,    -1,    68,    -1,    70,    -1,    -1,    -1,    74,
    75,    -1,    -1,    -1,    79,    80,    -1,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,     7,    -1,
    -1,    96,    97,    -1,    99,    -1,   101,   102,   103,   104,
    -1,   106,    -1,   108,   109,   110,   111,   112,    -1,    -1,
    -1,    -1,    -1,    -1,    33,    34,     7,    -1,    -1,    -1,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
    -1,    50,    51,    -1,    -1,    -1,    -1,    56,    57,    58,
    -1,    -1,    33,    34,    -1,    -1,    -1,    -1,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    -1,    50,
    51,    -1,    -1,    -1,    -1,    56,    57,    58,    -1,    -1,
    89,    90,    91,    -1,    -1,    -1,    -1,    96,    97,    -1,
    -1,    -1,   101,   102,   103,    -1,    -1,   106,    -1,   108,
   109,   110,   111,   112,    -1,    -1,     7,    -1,    89,    90,
    91,    -1,    -1,    -1,    -1,    96,    97,    -1,    -1,    -1,
   101,   102,   103,    -1,    -1,   106,    -1,   108,   109,   110,
   111,   112,    33,    34,     7,    -1,    -1,    -1,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    -1,    50,
    51,    -1,    -1,    -1,    -1,    56,    57,    58,    -1,    -1,
    33,    34,    -1,    -1,    -1,    -1,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    -1,    50,    51,    -1,
    -1,    -1,    -1,    56,    57,    58,    -1,    -1,    89,    90,
    91,    -1,    -1,    -1,    -1,    96,    97,    -1,    -1,    -1,
   101,   102,   103,    -1,    -1,   106,    -1,   108,   109,   110,
   111,   112,    -1,    -1,     7,    -1,    89,    90,    91,    -1,
    -1,    -1,    -1,    96,    97,    -1,    -1,    -1,   101,   102,
   103,    24,    -1,   106,   107,    -1,   109,   110,   111,   112,
    33,    34,     7,    -1,    -1,    -1,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    -1,    50,    51,    24,
    -1,    -1,    -1,    56,    57,    58,    -1,    -1,    33,    34,
    -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    -1,    50,    51,    -1,    -1,    -1,
    -1,    56,    57,    58,    -1,    -1,    89,    90,    91,    -1,
    -1,    -1,    -1,    96,    97,    -1,    -1,    -1,   101,   102,
   103,    -1,    -1,   106,    -1,    -1,   109,   110,   111,   112,
    -1,    -1,     7,    -1,    89,    90,    91,    -1,    -1,    -1,
    -1,    96,    97,    -1,    -1,    -1,   101,   102,   103,    -1,
    -1,   106,    -1,    -1,   109,   110,   111,   112,    33,    34,
    -1,    -1,    -1,    -1,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    -1,    50,    51,     3,     4,     5,
     6,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    18,    -1,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    -1,    -1,    33,    34,    35,
    36,    37,    38,    -1,    89,    90,    91,    -1,    -1,    -1,
    -1,    96,    97,    -1,    -1,    -1,   101,   102,   103,    -1,
    -1,   106,    -1,    -1,   109,   110,   111,   112,     4,     5,
     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    -1,    -1,    33,    34,    35,
    36,    37,    38,     4,     5,     6,    -1,    -1,    -1,    -1,
    -1,   107,    -1,    -1,    -1,    -1,    -1,    18,    -1,    20,
    21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    -1,    -1,    33,    34,    35,    36,    37,    38,    -1,    -1,
    -1,    -1,    -1,    -1,     4,     5,     6,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    -1,   108,    33,    34,    35,    36,    37,    38,     4,
     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    18,    -1,    20,    21,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    -1,   108,    33,    34,
    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,    -1,
     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    18,    -1,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    -1,   108,    33,
    34,    35,    36,    37,    38,     4,     5,     6,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,
    -1,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    -1,   108,    33,    34,    35,    36,    37,    38,
    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    18,    -1,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    -1,   108,    33,    34,    35,    36,    37,
    38,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    18,    -1,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    -1,   108,
    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,
    -1,    -1,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
   108,    33,    34,    35,    36,    37,    38,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    18,    -1,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,   108,    33,    34,    35,    36,
    37,    38,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    18,    -1,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,   107,    33,    34,    35,    36,
    37,    38,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
   107,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
    -1,    -1,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
   107,    33,    34,    35,    36,    37,    38,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    18,    -1,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,   107,    33,    34,    35,    36,
    37,    38,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    18,    -1,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,   107,    33,    34,    35,    36,
    37,    38,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
   107,    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,
    -1,    -1,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
   107,    33,    34,    35,    36,    37,    38,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    18,    -1,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,   107,    33,    34,    35,    36,
    37,    38,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    18,    -1,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,   107,    33,    34,    35,    36,
    37,    38,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    18,    -1,    20,    21,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    -1,
   107,    33,    34,    35,    36,    37,    38,    -1,    -1,    49,
     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    58,    59,
    -1,    61,    62,    63,    18,    -1,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    -1,   105,    33,
    34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,    -1,    -1,
     4,     5,     6,    95,   104,   105,    -1,    -1,    -1,   109,
    -1,   111,    -1,   113,    18,    19,    20,    21,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    -1,    -1,    33,
    34,    35,    36,    37,    38,     4,     5,     6,    -1,    -1,
    -1,    95,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    18,
    -1,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,     5,     6,    33,    34,    35,    36,    37,    38,
    -1,    -1,    -1,    -1,    -1,    18,    -1,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,    30,     6,    -1,
    33,    34,    35,    36,    37,    38,    -1,    -1,    -1,    -1,
    18,    -1,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    -1,    -1,    33,    34,    35,    36,    37,
    38,    18,    -1,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,    -1,    33,    34,    35,    36,
    37,    38,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    -1,    -1,    33,    34,    35,    36,    37,
    38,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    -1,    -1,    33,    34,    35,    36,    37,    38,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    -1,    -1,
    33,    34,    35,    36,    37,    38,    23,    24,    25,    26,
    27,    28,    29,    30,    -1,    -1,    33,    34,    35,    36,
    37,    38,    24,    25,    26,    27,    28,    29,    30,    -1,
    -1,    33,    34,    35,    36,    37,    38,    25,    26,    27,
    28,    29,    30,    -1,    -1,    33,    34,    35,    36,    37,
    38,    25,    26,    27,    28,    29,    30,    -1,    -1,    33,
    34,    35,    36,    37,    38
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"

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

#line 196 "/usr/local/share/bison.simple"

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
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[-1],increment_function,1 _INLINE_TLS); ;
    break;}
case 155:
#line 490 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[0],increment_function,0 _INLINE_TLS); ;
    break;}
case 156:
#line 491 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[-1],decrement_function,1 _INLINE_TLS); ;
    break;}
case 157:
#line 492 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[0],decrement_function,0 _INLINE_TLS); ;
    break;}
case 158:
#line 493 "language-parser.y"
{ cs_pre_boolean_or(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 159:
#line 493 "language-parser.y"
{ cs_post_boolean_or(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 160:
#line 494 "language-parser.y"
{ cs_pre_boolean_and(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 161:
#line 494 "language-parser.y"
{ cs_post_boolean_and(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 162:
#line 495 "language-parser.y"
{ cs_pre_boolean_or(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 163:
#line 495 "language-parser.y"
{ cs_post_boolean_or(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 164:
#line 496 "language-parser.y"
{ cs_pre_boolean_and(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 165:
#line 496 "language-parser.y"
{ cs_post_boolean_and(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 166:
#line 497 "language-parser.y"
{ if (GLOBAL(Execute)) { boolean_xor_function(&yyval,&yyvsp[-2],&yyvsp[0]); } ;
    break;}
case 167:
#line 498 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_or_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 168:
#line 499 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_xor_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 169:
#line 500 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_and_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 170:
#line 501 "language-parser.y"
{ if (GLOBAL(Execute)) concat_function(&yyval,&yyvsp[-2],&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 171:
#line 502 "language-parser.y"
{ if (GLOBAL(Execute)) add_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 172:
#line 503 "language-parser.y"
{ if (GLOBAL(Execute)) sub_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 173:
#line 504 "language-parser.y"
{ if (GLOBAL(Execute)) mul_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 174:
#line 505 "language-parser.y"
{ if (GLOBAL(Execute)) div_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 175:
#line 506 "language-parser.y"
{ if (GLOBAL(Execute)) mod_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 176:
#line 507 "language-parser.y"
{ if (GLOBAL(Execute)) { pval tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  add_function(&yyval,&tmp,&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 177:
#line 508 "language-parser.y"
{ if (GLOBAL(Execute)) { pval tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  sub_function(&yyval,&tmp,&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 178:
#line 509 "language-parser.y"
{ if (GLOBAL(Execute)) boolean_not_function(&yyval,&yyvsp[0]); ;
    break;}
case 179:
#line 510 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_not_function(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 180:
#line 511 "language-parser.y"
{ if (GLOBAL(Execute)) is_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 181:
#line 512 "language-parser.y"
{ if (GLOBAL(Execute)) is_not_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 182:
#line 513 "language-parser.y"
{ if (GLOBAL(Execute)) is_smaller_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 183:
#line 514 "language-parser.y"
{ if (GLOBAL(Execute)) is_smaller_or_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 184:
#line 515 "language-parser.y"
{ if (GLOBAL(Execute)) is_greater_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 185:
#line 516 "language-parser.y"
{ if (GLOBAL(Execute)) is_greater_or_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 186:
#line 517 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 187:
#line 518 "language-parser.y"
{  php3_error(E_PARSE,"'(' unmatched",GLOBAL(current_lineno)); if (GLOBAL(Execute)) yyval = yyvsp[-1]; yyerrok; ;
    break;}
case 188:
#line 519 "language-parser.y"
{ cs_questionmark_op_pre_expr1(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 189:
#line 520 "language-parser.y"
{ cs_questionmark_op_pre_expr2(&yyvsp[-4] _INLINE_TLS); ;
    break;}
case 190:
#line 521 "language-parser.y"
{ cs_questionmark_op_post_expr2(&yyval,&yyvsp[-6],&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 191:
#line 522 "language-parser.y"
{ cs_functioncall_pre_variable_passing(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 192:
#line 523 "language-parser.y"
{ cs_functioncall_post_variable_passing(&yyvsp[-4],&yychar _INLINE_TLS); ;
    break;}
case 193:
#line 524 "language-parser.y"
{ cs_functioncall_end(&yyval,&yyvsp[-6],&yyvsp[-2],&yychar _INLINE_TLS);;
    break;}
case 195:
#line 526 "language-parser.y"
{ cs_functioncall_pre_variable_passing(&yyvsp[0],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 196:
#line 527 "language-parser.y"
{ cs_functioncall_post_variable_passing(&yyvsp[-4],&yychar _INLINE_TLS); ;
    break;}
case 197:
#line 528 "language-parser.y"
{ cs_functioncall_end(&yyval,&yyvsp[-6],&yyvsp[-2],&yychar _INLINE_TLS); ;
    break;}
case 198:
#line 529 "language-parser.y"
{ assign_new_object(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 199:
#line 530 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_long(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 200:
#line 531 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_double(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 201:
#line 532 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_string(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 202:
#line 533 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_array(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 203:
#line 534 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_object(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 204:
#line 535 "language-parser.y"
{ if (GLOBAL(Execute)) { php3_header(); GLOBAL(shutdown_requested)=ABNORMAL_SHUTDOWN; yyval.type=IS_LONG; yyval.value.lval=1; } ;
    break;}
case 205:
#line 536 "language-parser.y"
{ if (GLOBAL(Execute)) { php3_header(); GLOBAL(shutdown_requested)=ABNORMAL_SHUTDOWN; yyval.type=IS_LONG; yyval.value.lval=1; } ;
    break;}
case 206:
#line 537 "language-parser.y"
{ if (GLOBAL(Execute)) { if (php3_header()) { convert_to_string(&yyvsp[-1]);  PUTS(yyvsp[-1].value.str.val);  yystype_destructor(&yyvsp[-1] _INLINE_TLS); } GLOBAL(shutdown_requested)=ABNORMAL_SHUTDOWN; yyval.type=IS_LONG; yyval.value.lval=1; } ;
    break;}
case 207:
#line 538 "language-parser.y"
{ yyvsp[0].cs_data.error_reporting=GLOBAL(error_reporting); GLOBAL(error_reporting)=0; ;
    break;}
case 208:
#line 538 "language-parser.y"
{ GLOBAL(error_reporting)=yyvsp[-2].cs_data.error_reporting; yyval = yyvsp[0]; ;
    break;}
case 209:
#line 539 "language-parser.y"
{ php3_error(E_ERROR,"@ operator may only be used on expressions"); ;
    break;}
case 210:
#line 540 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 211:
#line 541 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 212:
#line 542 "language-parser.y"
{ cs_system(&yyval,&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 213:
#line 543 "language-parser.y"
{ if (GLOBAL(Execute)) { print_variable(&yyvsp[0] _INLINE_TLS);  yystype_destructor(&yyvsp[0] _INLINE_TLS);  yyval.value.lval=1; yyval.type=IS_LONG; } ;
    break;}
case 214:
#line 548 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 215:
#line 549 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 216:
#line 550 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 217:
#line 551 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 218:
#line 552 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; COPY_STRING(yyval); } ;
    break;}
case 219:
#line 553 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; COPY_STRING(yyval); php3_error(E_NOTICE,"'%s' is not a valid constant - assumed to be \"%s\"",yyvsp[0].value.str.val,yyvsp[0].value.str.val); } ;
    break;}
case 220:
#line 554 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; } ;
    break;}
case 221:
#line 555 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; COPY_STRING(yyval); } ;
    break;}
case 222:
#line 559 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 223:
#line 560 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 224:
#line 561 "language-parser.y"
{ if (GLOBAL(Execute)) { pval tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  sub_function(&yyval,&tmp,&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 225:
#line 565 "language-parser.y"
{ if (GLOBAL(Execute)){ yyval = yyvsp[0]; };
    break;}
case 226:
#line 571 "language-parser.y"
{ if (GLOBAL(Execute)) read_pointer_value(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 227:
#line 572 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 228:
#line 577 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 229:
#line 578 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 230:
#line 583 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval = yyvsp[0];COPY_STRING(yyval); };
    break;}
case 231:
#line 584 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 232:
#line 590 "language-parser.y"
{ get_object_symtable(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 233:
#line 591 "language-parser.y"
{ if (GLOBAL(Execute)) { if (yyvsp[0].value.varptr.yystype && ((pval *)yyvsp[0].value.varptr.yystype)->type == IS_OBJECT) { yyval=yyvsp[0]; } else { yyval.value.varptr.yystype=NULL; } } ;
    break;}
case 234:
#line 592 "language-parser.y"
{ get_object_symtable(&yyval,NULL,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 235:
#line 596 "language-parser.y"
{

	if (GLOBAL(Execute)) {
		yyval=yyvsp[-2];
		hash_next_index_insert(yyval.value.ht,&yyvsp[0],sizeof(pval),NULL);
	}
;
    break;}
case 236:
#line 603 "language-parser.y"
{

	if (GLOBAL(Execute)) {
		yyval.value.ht = (HashTable *) emalloc(sizeof(HashTable));
		hash_init(yyval.value.ht,0,NULL,NULL,0);
		yyval.type = IS_ARRAY;
		hash_next_index_insert(yyval.value.ht,&yyvsp[0],sizeof(pval),NULL);
	}
;
    break;}
case 237:
#line 612 "language-parser.y"
{
	if (GLOBAL(Execute)) {
		yyval=yyvsp[-1];
		yyvsp[0].value.varptr.yystype = NULL; /* $2 is just used as temporary space */
		hash_next_index_insert(yyval.value.ht,&yyvsp[0],sizeof(pval),NULL);
	}
;
    break;}
case 238:
#line 619 "language-parser.y"
{
	if (GLOBAL(Execute)) {
		pval tmp;

		yyval.value.ht = (HashTable *) emalloc(sizeof(HashTable));
		hash_init(yyval.value.ht,0,NULL,NULL,0);
		yyval.type = IS_ARRAY;
		tmp.value.varptr.yystype = NULL;
		hash_next_index_insert(yyval.value.ht,&tmp,sizeof(pval),NULL);
	}
;
    break;}
case 239:
#line 634 "language-parser.y"
{ get_regular_variable_pointer(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 240:
#line 635 "language-parser.y"
{ if (GLOBAL(Execute)) yyval=yyvsp[0]; ;
    break;}
case 241:
#line 636 "language-parser.y"
{ get_class_variable_pointer(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 242:
#line 637 "language-parser.y"
{ start_array_parsing(&yyvsp[-1],&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 243:
#line 637 "language-parser.y"
{ end_array_parsing(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 244:
#line 642 "language-parser.y"
{ if (GLOBAL(Execute)) get_regular_variable_contents(&yyval,&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 245:
#line 643 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval = yyvsp[0];COPY_STRING(yyval);} ;
    break;}
case 246:
#line 644 "language-parser.y"
{  if (GLOBAL(Execute)) read_pointer_value(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 247:
#line 648 "language-parser.y"
{ start_array_parsing(&yyvsp[-1],NULL _INLINE_TLS); ;
    break;}
case 248:
#line 648 "language-parser.y"
{ end_array_parsing(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 249:
#line 653 "language-parser.y"
{ fetch_array_index(&yyval,&yyvsp[-1],&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 250:
#line 654 "language-parser.y"
{ fetch_array_index(&yyval,NULL,&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 251:
#line 655 "language-parser.y"
{ start_dimensions_parsing(&yyval _INLINE_TLS); ;
    break;}
case 252:
#line 655 "language-parser.y"
{ fetch_array_index(&yyval,NULL,&yyval _INLINE_TLS); ;
    break;}
case 253:
#line 656 "language-parser.y"
{ start_dimensions_parsing(&yyval _INLINE_TLS); ;
    break;}
case 254:
#line 656 "language-parser.y"
{ fetch_array_index(&yyval,&yyvsp[-1],&yyval _INLINE_TLS); ;
    break;}
case 255:
#line 661 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval.value.ht = (HashTable *) emalloc(sizeof(HashTable));  hash_init(yyval.value.ht,0,NULL,pval_DESTRUCTOR,0); yyval.type = IS_ARRAY; } ;
    break;}
case 256:
#line 662 "language-parser.y"
{ yyval = yyvsp[0]; ;
    break;}
case 257:
#line 666 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval=yyvsp[-4]; add_array_pair_list(&yyval, &yyvsp[-2], &yyvsp[0], 0 _INLINE_TLS);} ;
    break;}
case 258:
#line 667 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval=yyvsp[-2]; add_array_pair_list(&yyval, NULL, &yyvsp[0], 0 _INLINE_TLS);} ;
    break;}
case 259:
#line 668 "language-parser.y"
{ if (GLOBAL(Execute)) add_array_pair_list(&yyval, &yyvsp[-2], &yyvsp[0], 1 _INLINE_TLS); ;
    break;}
case 260:
#line 669 "language-parser.y"
{ if (GLOBAL(Execute)) add_array_pair_list(&yyval, NULL, &yyvsp[0], 1 _INLINE_TLS); ;
    break;}
case 261:
#line 674 "language-parser.y"
{ add_regular_encapsed_variable(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 262:
#line 675 "language-parser.y"
{ add_assoc_array_encapsed_variable(&yyval,&yyvsp[-5],&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 263:
#line 676 "language-parser.y"
{ add_regular_array_encapsed_variable(&yyval,&yyvsp[-5],&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 264:
#line 677 "language-parser.y"
{ add_variable_array_encapsed_variable(&yyval,&yyvsp[-6],&yyvsp[-4],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 265:
#line 678 "language-parser.y"
{ add_encapsed_object_property(&yyval,&yyvsp[-4],&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 266:
#line 679 "language-parser.y"
{ add_regular_encapsed_variable(&yyval,&yyvsp[-4],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 267:
#line 680 "language-parser.y"
{ add_assoc_array_encapsed_variable(&yyval,&yyvsp[-7],&yyvsp[-4],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 268:
#line 681 "language-parser.y"
{ add_regular_array_encapsed_variable(&yyval,&yyvsp[-7],&yyvsp[-4],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 269:
#line 682 "language-parser.y"
{ add_variable_array_encapsed_variable(&yyval,&yyvsp[-8],&yyvsp[-5],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 270:
#line 683 "language-parser.y"
{ add_encapsed_object_property(&yyval,&yyvsp[-6],&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 271:
#line 684 "language-parser.y"
{ add_indirect_encapsed_variable(&yyval,&yyvsp[-5],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 272:
#line 685 "language-parser.y"
{ if (GLOBAL(Execute)) { concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 273:
#line 686 "language-parser.y"
{ if (GLOBAL(Execute)) { concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 274:
#line 687 "language-parser.y"
{ if (GLOBAL(Execute)) { concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 275:
#line 688 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 276:
#line 689 "language-parser.y"
{ if (GLOBAL(Execute)) { php3_error(E_NOTICE,"Bad escape sequence:  %s",yyvsp[0].value.str.val); concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 277:
#line 690 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 278:
#line 691 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 279:
#line 692 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 280:
#line 693 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 281:
#line 694 "language-parser.y"
{  if (GLOBAL(Execute)) { pval tmp;  tmp.value.str.val="->"; tmp.value.str.len=2; tmp.type=IS_STRING; concat_function(&yyval,&yyvsp[-1],&tmp,0 _INLINE_TLS); } ;
    break;}
case 282:
#line 695 "language-parser.y"
{ if (GLOBAL(Execute)) var_reset(&yyval); ;
    break;}
case 283:
#line 700 "language-parser.y"
{ php3_unset(&yyval, &yyvsp[-1]); ;
    break;}
case 284:
#line 701 "language-parser.y"
{ php3_isset(&yyval, &yyvsp[-1]); ;
    break;}
case 285:
#line 702 "language-parser.y"
{ php3_empty(&yyval, &yyvsp[-1]); ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 498 "/usr/local/share/bison.simple"

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
#line 712 "language-parser.y"


inline void clear_lookahead(int *yychar)
{
	if (yychar) {
		*yychar=YYEMPTY;
	}
}
	
