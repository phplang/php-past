
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
#define	IC_FUNCTION	319
#define	PHP_CONST	320
#define	RETURN	321
#define	INCLUDE	322
#define	REQUIRE	323
#define	HIGHLIGHT_FILE	324
#define	HIGHLIGHT_STRING	325
#define	PHP_GLOBAL	326
#define	PHP_STATIC	327
#define	PHP_UNSET	328
#define	PHP_ISSET	329
#define	PHP_EMPTY	330
#define	CLASS	331
#define	EXTENDS	332
#define	PHP_CLASS_OPERATOR	333
#define	PHP_DOUBLE_ARROW	334
#define	PHP_LIST	335
#define	PHP_ARRAY	336
#define	VAR	337
#define	EVAL	338
#define	DONE_EVAL	339
#define	PHP_LINE	340
#define	PHP_FILE	341
#define	STRING_CONSTANT	342

#line 1 "language-parser.y"


/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-1999 PHP Development Team (See Credits file)      |
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


/* $Id: language-parser.y,v 1.180 1999/05/14 10:00:24 sas Exp $ */


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
extern int wanted_exit_status;
#define YY_TLS_VARS
#endif
#include "php.h"
#include "php3_list.h"
#include "control_structures.h"


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
FunctionState php3g_function_state_for_require; /* to save state when forcing execution in require() evaluation */

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

#include "control_structures_inline.h"


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
#if DEBUG
		printf("Cleaning resource %d\n",*resource_id);
#endif
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



#define	YYFINAL		626
#define	YYFLAG		-32768
#define	YYNTBASE	117

#define YYTRANSLATE(x) ((unsigned)(x) <= 342 ? yytranslate[x] : 222)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    41,   114,     2,   112,    40,    26,   115,   109,
   110,    38,    35,     3,    36,    37,    39,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    21,   111,    29,
     8,    31,    20,    50,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    51,     2,   116,    25,     2,   113,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,   107,    24,   108,    42,     2,     2,     2,     2,
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
   105,   106
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     3,     4,     8,     9,    18,    19,    20,    33,    34,
    35,    44,    45,    46,    47,    59,    60,    61,    62,    63,
    64,    80,    81,    88,    91,    95,    98,   102,   103,   112,
   113,   123,   124,   134,   137,   141,   144,   147,   148,   155,
   156,   165,   169,   171,   174,   175,   180,   184,   190,   198,
   204,   208,   210,   212,   217,   221,   226,   231,   237,   239,
   244,   245,   246,   254,   255,   256,   257,   268,   269,   270,
   271,   272,   273,   288,   289,   290,   291,   292,   302,   303,
   304,   305,   306,   317,   318,   319,   323,   324,   325,   330,
   331,   332,   333,   341,   342,   347,   348,   349,   357,   358,
   363,   364,   367,   368,   371,   375,   379,   384,   389,   395,
   401,   408,   410,   411,   413,   415,   418,   422,   426,   431,
   436,   439,   444,   451,   454,   459,   461,   465,   468,   469,
   473,   474,   483,   484,   494,   499,   506,   509,   514,   515,
   519,   521,   522,   526,   528,   535,   539,   543,   547,   551,
   555,   559,   563,   567,   571,   575,   579,   583,   586,   589,
   592,   595,   596,   601,   602,   607,   608,   613,   614,   619,
   623,   627,   631,   635,   639,   643,   647,   651,   655,   659,
   663,   667,   670,   673,   676,   679,   683,   687,   691,   695,
   699,   703,   707,   711,   712,   713,   721,   722,   723,   731,
   733,   734,   735,   746,   749,   750,   751,   752,   762,   765,
   768,   771,   774,   777,   779,   783,   788,   789,   793,   796,
   798,   803,   807,   810,   812,   814,   818,   822,   824,   826,
   828,   830,   832,   835,   838,   840,   842,   844,   846,   850,
   852,   856,   860,   862,   864,   868,   870,   873,   874,   877,
   880,   885,   886,   894,   897,   899,   902,   903,   908,   913,
   917,   918,   921,   922,   926,   927,   929,   935,   939,   943,
   945,   949,   956,   963,   971,   977,   983,   992,  1001,  1011,
  1019,  1026,  1029,  1032,  1035,  1038,  1041,  1044,  1047,  1050,
  1053,  1056,  1057,  1062,  1067,  1072,  1073,  1081
};

static const short yyrhs[] = {   117,
   118,     0,     0,   107,   117,   108,     0,     0,    54,   109,
   204,   110,   119,   118,   152,   160,     0,     0,     0,    54,
   109,   204,   110,    21,   120,   117,   156,   162,   121,    57,
   111,     0,     0,     0,    68,   109,   204,   110,   122,   141,
   123,   142,     0,     0,     0,     0,    67,   124,   118,    68,
   125,   109,   204,   110,   111,   126,   144,     0,     0,     0,
     0,     0,     0,    70,   127,   109,   185,   128,   111,   185,
   129,   111,   185,   130,   110,   139,   131,   147,     0,     0,
    72,   109,   204,   110,   132,   140,     0,    76,   111,     0,
    76,   204,   111,     0,    77,   111,     0,    77,   204,   111,
     0,     0,    81,    60,   133,   171,   109,   117,   110,   111,
     0,     0,    82,    60,   109,   134,   171,   110,   107,   117,
   108,     0,     0,    83,    60,   135,   109,   171,   110,   107,
   117,   108,     0,    85,   111,     0,    85,   204,   111,     0,
    90,   176,     0,    91,   177,     0,     0,    95,    60,   136,
   107,   179,   108,     0,     0,    95,    60,    96,    60,   137,
   107,   179,   108,     0,    66,   184,   111,     0,    62,     0,
   204,   111,     0,     0,    87,   138,   204,   111,     0,    88,
   204,   111,     0,    89,   109,   204,   110,   111,     0,    89,
   109,   204,     3,   204,   110,   111,     0,   102,   109,   204,
   110,   111,     0,    86,   204,   111,     0,   111,     0,   118,
     0,    21,   117,    71,   111,     0,   107,   164,   108,     0,
   107,   111,   164,   108,     0,    21,   164,    73,   111,     0,
    21,   111,   164,    73,   111,     0,   118,     0,    21,   117,
    69,   111,     0,     0,     0,   142,    78,   109,   204,   110,
   143,   141,     0,     0,     0,     0,   144,    79,   145,   118,
    68,   146,   109,   204,   110,   111,     0,     0,     0,     0,
     0,     0,   147,    80,   148,   109,   185,   149,   111,   185,
   150,   111,   185,   151,   110,   139,     0,     0,     0,     0,
     0,   152,    55,   109,   153,   204,   154,   110,   155,   118,
     0,     0,     0,     0,     0,   156,    55,   109,   157,   204,
   158,   110,    21,   159,   117,     0,     0,     0,    56,   161,
   118,     0,     0,     0,    56,    21,   163,   117,     0,     0,
     0,     0,    74,   204,    21,   165,   117,   166,   164,     0,
     0,    75,    21,   167,   117,     0,     0,     0,    74,   204,
   111,   168,   117,   169,   164,     0,     0,    75,   111,   170,
   117,     0,     0,   172,   173,     0,     0,   112,   203,     0,
    26,   112,   203,     0,    84,   112,   203,     0,   112,   203,
     8,   202,     0,   173,     3,   112,   203,     0,   173,     3,
    26,   112,   203,     0,   173,     3,    84,   112,   203,     0,
   173,     3,   112,   203,     8,   202,     0,   175,     0,     0,
   186,     0,   209,     0,    26,   209,     0,   175,     3,   186,
     0,   175,     3,   209,     0,   175,     3,    26,   209,     0,
   176,     3,   112,   211,     0,   112,   211,     0,   177,     3,
   112,   211,     0,   177,     3,   112,   211,     8,   178,     0,
   112,   211,     0,   112,   211,     8,   178,     0,   202,     0,
   109,   204,   110,     0,   179,   180,     0,     0,   101,   183,
   111,     0,     0,    81,    60,   181,   171,   109,   117,   110,
   111,     0,     0,    82,    60,   109,   182,   171,   110,   107,
   117,   108,     0,   183,     3,   112,   203,     0,   183,     3,
   112,   203,     8,   204,     0,   112,   203,     0,   112,   203,
     8,   204,     0,     0,   184,     3,   204,     0,   204,     0,
     0,   185,     3,   204,     0,   204,     0,    99,   109,   208,
   110,     8,   204,     0,   209,     8,   204,     0,   209,     9,
   204,     0,   209,    10,   204,     0,   209,    11,   204,     0,
   209,    12,   204,     0,   209,    13,   204,     0,   209,    14,
   204,     0,   209,    15,   204,     0,   209,    16,   204,     0,
   209,    17,   204,     0,   209,    18,   204,     0,   209,    19,
   204,     0,   209,    43,     0,    43,   209,     0,   209,    44,
     0,    44,   209,     0,     0,   204,    22,   187,   204,     0,
     0,   204,    23,   188,   204,     0,     0,   204,     4,   189,
   204,     0,     0,   204,     6,   190,   204,     0,   204,     5,
   204,     0,   204,    24,   204,     0,   204,    25,   204,     0,
   204,    26,   204,     0,   204,    37,   204,     0,   204,    35,
   204,     0,   204,    36,   204,     0,   204,    38,   204,     0,
   204,    39,   204,     0,   204,    40,   204,     0,   204,    33,
   204,     0,   204,    34,   204,     0,    35,   204,     0,    36,
   204,     0,    41,   204,     0,    42,   204,     0,   204,    27,
   204,     0,   204,    28,   204,     0,   204,    29,   204,     0,
   204,    30,   204,     0,   204,    31,   204,     0,   204,    32,
   204,     0,   109,   204,   110,     0,   109,   204,     1,     0,
     0,     0,   204,    20,   191,   204,    21,   192,   204,     0,
     0,     0,   211,   193,   109,   174,   110,   194,   221,     0,
   220,     0,     0,     0,   112,   207,    97,   211,   195,   109,
   174,   110,   196,   221,     0,    52,   211,     0,     0,     0,
     0,    52,   211,   197,   109,   198,   174,   110,   199,   221,
     0,    45,   204,     0,    46,   204,     0,    47,   204,     0,
    48,   204,     0,    49,   204,     0,    53,     0,    53,   109,
   110,     0,    53,   109,   204,   110,     0,     0,    50,   200,
   204,     0,    50,     1,     0,   201,     0,   100,   109,   217,
   110,     0,   113,   219,   113,     0,     7,   204,     0,    58,
     0,    59,     0,   114,   219,   114,     0,   115,   219,   115,
     0,   106,     0,    60,     0,   104,     0,   105,     0,   201,
     0,    35,   202,     0,    36,   202,     0,    60,     0,   209,
     0,   186,     0,   211,     0,   107,   204,   108,     0,   203,
     0,   107,   204,   108,     0,   207,    97,   203,     0,   212,
     0,   203,     0,   208,     3,   209,     0,   209,     0,   208,
     3,     0,     0,   112,   205,     0,   112,   212,     0,   112,
   207,    97,   205,     0,     0,   112,   207,    97,   206,    51,
   210,   214,     0,   112,   205,     0,   203,     0,   112,   212,
     0,     0,   206,    51,   213,   214,     0,   214,    51,   204,
   116,     0,   214,    51,   116,     0,     0,   215,   116,     0,
     0,   216,   204,   116,     0,     0,   218,     0,   218,     3,
   204,    98,   204,     0,   218,     3,   204,     0,   204,    98,
   204,     0,   204,     0,   219,   112,    60,     0,   219,   112,
    60,    51,    60,   116,     0,   219,   112,    60,    51,    61,
   116,     0,   219,   112,    60,    51,   112,    60,   116,     0,
   219,   112,    60,    97,    60,     0,   219,   112,   107,    60,
   108,     0,   219,   112,   107,    60,    51,    60,   116,   108,
     0,   219,   112,   107,    60,    51,    61,   116,   108,     0,
   219,   112,   107,    60,    51,   112,    60,   116,   108,     0,
   219,   112,   107,    60,    97,    60,   108,     0,   219,   112,
   107,   112,    60,   108,     0,   219,    60,     0,   219,    61,
     0,   219,    65,     0,   219,    63,     0,   219,    64,     0,
   219,    51,     0,   219,   116,     0,   219,   107,     0,   219,
   108,     0,   219,    97,     0,     0,    92,   109,   209,   110,
     0,    93,   109,   209,   110,     0,    94,   109,   209,   110,
     0,     0,    81,    60,   171,   109,   117,   110,   111,     0,
    82,    60,   109,   171,   110,   107,   117,   108,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   273,   275,   279,   281,   281,   282,   282,   282,   283,   283,
   284,   284,   284,   284,   285,   285,   286,   287,   288,   289,
   290,   290,   290,   291,   292,   293,   294,   295,   296,   297,
   298,   299,   299,   300,   301,   302,   303,   304,   304,   305,
   305,   306,   307,   308,   309,   309,   310,   311,   312,   313,
   314,   315,   319,   321,   324,   326,   327,   328,   331,   333,
   336,   338,   338,   342,   344,   344,   344,   348,   350,   351,
   352,   353,   354,   358,   360,   361,   361,   362,   365,   367,
   368,   368,   369,   372,   374,   375,   378,   380,   381,   384,
   386,   386,   387,   387,   387,   388,   388,   389,   389,   389,
   393,   395,   395,   399,   401,   402,   403,   404,   405,   406,
   407,   411,   413,   417,   419,   420,   421,   422,   423,   426,
   428,   432,   434,   435,   436,   441,   443,   447,   449,   453,
   455,   456,   457,   458,   463,   465,   466,   467,   471,   473,
   474,   478,   480,   481,   485,   487,   488,   489,   490,   491,
   492,   493,   494,   495,   496,   497,   498,   499,   500,   501,
   502,   503,   503,   504,   504,   505,   505,   506,   506,   507,
   508,   509,   510,   511,   512,   513,   514,   515,   516,   517,
   518,   519,   520,   521,   522,   523,   524,   525,   526,   527,
   528,   529,   530,   531,   532,   533,   534,   535,   536,   537,
   538,   539,   540,   541,   542,   543,   544,   545,   546,   547,
   548,   549,   550,   551,   552,   553,   554,   554,   555,   556,
   557,   558,   559,   562,   564,   565,   566,   567,   568,   569,
   570,   573,   575,   576,   579,   585,   587,   591,   593,   597,
   599,   604,   606,   607,   610,   618,   627,   634,   648,   650,
   651,   652,   652,   656,   658,   659,   662,   663,   667,   669,
   670,   670,   671,   671,   675,   677,   680,   682,   683,   684,
   688,   690,   691,   692,   693,   694,   695,   696,   697,   698,
   699,   700,   701,   702,   703,   704,   705,   706,   707,   708,
   709,   710,   714,   716,   717,   721,   723,   724
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
"OLD_FUNCTION","FUNCTION","IC_FUNCTION","PHP_CONST","RETURN","INCLUDE","REQUIRE",
"HIGHLIGHT_FILE","HIGHLIGHT_STRING","PHP_GLOBAL","PHP_STATIC","PHP_UNSET","PHP_ISSET",
"PHP_EMPTY","CLASS","EXTENDS","PHP_CLASS_OPERATOR","PHP_DOUBLE_ARROW","PHP_LIST",
"PHP_ARRAY","VAR","EVAL","DONE_EVAL","PHP_LINE","PHP_FILE","STRING_CONSTANT",
"'{'","'}'","'('","')'","';'","'$'","'`'","'\"'","'''","']'","statement_list",
"statement","@1","@2","@3","@4","@5","@6","@7","@8","@9","@10","@11","@12","@13",
"@14","@15","@16","@17","@18","@19","@20","for_statement","switch_case_list",
"while_statement","while_iterations","@21","do_while_iterations","@22","@23",
"for_iterations","@24","@25","@26","@27","elseif_list","@28","@29","@30","new_elseif_list",
"@31","@32","@33","else_single","@34","new_else_single","@35","case_list","@36",
"@37","@38","@39","@40","@41","parameter_list","@42","non_empty_parameter_list",
"function_call_parameter_list","non_empty_function_call_parameter_list","global_var_list",
"static_var_list","unambiguous_static_assignment","class_statement_list","class_statement",
"@43","@44","class_variable_decleration","echo_expr_list","for_expr","expr_without_variable",
"@45","@46","@47","@48","@49","@50","@51","@52","@53","@54","@55","@56","@57",
"@58","scalar","signed_scalar","varname_scalar","expr","unambiguous_variable_name",
"unambiguous_array_name","unambiguous_class_name","assignment_list","assignment_variable_pointer",
"@59","var","multi_dimensional_array","@60","dimensions","@61","@62","array_pair_list",
"non_empty_array_pair_list","encaps_list","internal_functions_in_yacc","possible_function_call", NULL
};
#endif

static const short yyr1[] = {     0,
   117,   117,   118,   119,   118,   120,   121,   118,   122,   123,
   118,   124,   125,   126,   118,   127,   128,   129,   130,   131,
   118,   132,   118,   118,   118,   118,   118,   133,   118,   134,
   118,   135,   118,   118,   118,   118,   118,   136,   118,   137,
   118,   118,   118,   118,   138,   118,   118,   118,   118,   118,
   118,   118,   139,   139,   140,   140,   140,   140,   141,   141,
   142,   143,   142,   144,   145,   146,   144,   147,   148,   149,
   150,   151,   147,   152,   153,   154,   155,   152,   156,   157,
   158,   159,   156,   160,   161,   160,   162,   163,   162,   164,
   165,   166,   164,   167,   164,   168,   169,   164,   170,   164,
   172,   171,   171,   173,   173,   173,   173,   173,   173,   173,
   173,   174,   174,   175,   175,   175,   175,   175,   175,   176,
   176,   177,   177,   177,   177,   178,   178,   179,   179,   180,
   181,   180,   182,   180,   183,   183,   183,   183,   184,   184,
   184,   185,   185,   185,   186,   186,   186,   186,   186,   186,
   186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
   186,   187,   186,   188,   186,   189,   186,   190,   186,   186,
   186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
   186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
   186,   186,   186,   191,   192,   186,   193,   194,   186,   186,
   195,   196,   186,   186,   197,   198,   199,   186,   186,   186,
   186,   186,   186,   186,   186,   186,   200,   186,   186,   186,
   186,   186,   186,   201,   201,   201,   201,   201,   201,   201,
   201,   202,   202,   202,   203,   204,   204,   205,   205,   206,
   206,   207,   207,   207,   208,   208,   208,   208,   209,   209,
   209,   210,   209,   211,   211,   211,   213,   212,   214,   214,
   215,   214,   216,   214,   217,   217,   218,   218,   218,   218,
   219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
   219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
   219,   219,   220,   220,   220,   221,   221,   221
};

static const short yyr2[] = {     0,
     2,     0,     3,     0,     8,     0,     0,    12,     0,     0,
     8,     0,     0,     0,    11,     0,     0,     0,     0,     0,
    15,     0,     6,     2,     3,     2,     3,     0,     8,     0,
     9,     0,     9,     2,     3,     2,     2,     0,     6,     0,
     8,     3,     1,     2,     0,     4,     3,     5,     7,     5,
     3,     1,     1,     4,     3,     4,     4,     5,     1,     4,
     0,     0,     7,     0,     0,     0,    10,     0,     0,     0,
     0,     0,    14,     0,     0,     0,     0,     9,     0,     0,
     0,     0,    10,     0,     0,     3,     0,     0,     4,     0,
     0,     0,     7,     0,     4,     0,     0,     7,     0,     4,
     0,     2,     0,     2,     3,     3,     4,     4,     5,     5,
     6,     1,     0,     1,     1,     2,     3,     3,     4,     4,
     2,     4,     6,     2,     4,     1,     3,     2,     0,     3,
     0,     8,     0,     9,     4,     6,     2,     4,     0,     3,
     1,     0,     3,     1,     6,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
     2,     0,     4,     0,     4,     0,     4,     0,     4,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     2,     2,     2,     2,     3,     3,     3,     3,     3,
     3,     3,     3,     0,     0,     7,     0,     0,     7,     1,
     0,     0,    10,     2,     0,     0,     0,     9,     2,     2,
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
     0,     0,     0,     0,     0,   214,     0,   224,   225,   229,
    43,   139,    12,     0,    16,     0,     0,     0,     0,     0,
     0,     0,     0,    45,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   230,   231,   228,     2,     0,
    52,     0,   292,   292,   292,     1,   237,   220,   255,     0,
   236,   197,   200,   223,   182,   183,   184,   185,     0,   159,
   161,   209,   210,   211,   212,   213,   219,     0,   235,     0,
   204,     0,     0,     0,   141,     0,     0,     0,     0,    24,
     0,    26,     0,    28,     0,    32,    34,     0,     0,     0,
     0,     0,     0,    36,     0,    37,     0,     0,     0,    38,
   248,   265,     0,     0,     0,     0,   255,   249,     0,     0,
   238,   250,     0,     0,     0,   166,     0,   168,   194,   162,
   164,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    44,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   158,   160,     0,   249,     0,   250,   218,   255,   254,
   256,     0,   215,     0,     0,     0,    42,     0,     0,   142,
     0,    25,    27,   101,    30,     0,    35,    51,     0,    47,
     0,   121,     0,   124,     0,     0,     0,     0,     0,     0,
     0,   246,   270,     0,   266,     0,     3,   193,   192,     0,
   257,     0,   287,   282,   283,   285,   286,   284,   291,   289,
   290,     0,   222,   288,   226,   227,     0,   170,     0,     0,
     0,     0,   171,   172,   173,   186,   187,   188,   189,   190,
   191,   180,   181,   175,   176,   174,   177,   178,   179,   146,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   113,     0,   206,   216,     4,   140,    13,     9,    17,
   144,    22,     0,     0,   101,   101,    46,     0,     0,     0,
     0,     0,   293,   294,   295,    40,   129,   247,     0,     0,
   221,     0,     0,   239,   263,   255,   251,     0,   238,   271,
     0,   167,   169,     0,   163,   165,     0,     0,   112,   237,
     0,   236,   113,     6,     0,     0,     0,     0,     0,     0,
     2,     0,     0,     0,   102,     0,     0,     0,    48,   120,
     0,     0,   229,     0,   125,   232,   126,   122,     0,     0,
   245,     0,   269,   268,    50,   258,     0,     0,   252,     0,
     0,     0,     0,     0,   195,   116,   198,     0,     0,     2,
    74,     0,     2,    59,    10,   143,   142,    90,    90,    23,
     0,     0,     0,   104,     0,     0,     0,     0,   233,   234,
     0,     0,   129,     0,     0,     0,    39,   128,   145,     0,
     0,   262,     0,   263,   113,     0,     0,     0,   275,     0,
     0,   276,     0,     0,   296,     0,   237,   236,   207,    79,
    84,     0,     0,    61,    18,     0,     0,    90,     0,    90,
     0,     0,   105,   106,     0,     0,     0,     0,     2,     2,
    49,   127,   123,     0,   131,     0,     0,     0,   267,   260,
     0,   264,   253,     0,   272,   273,     0,     0,     0,     0,
     0,   281,   196,     0,     0,   199,   119,   296,    87,     0,
    85,     5,     0,     0,    11,     0,     0,    94,    99,     0,
     0,     0,    55,    29,   107,     0,     0,   108,     0,     0,
    41,   101,   133,   137,     0,   130,   259,   202,   274,     0,
     0,     0,   280,   101,     0,   208,     0,     0,     7,    75,
     0,    14,    60,     0,   142,    91,    96,     2,     2,     0,
    57,    56,   109,   110,     0,    31,    33,     0,   101,     0,
     0,   296,   277,   278,     0,     0,   101,    80,    88,     0,
     0,    86,    64,     0,    19,     2,     2,    95,   100,    58,
   111,     2,     0,   138,   135,   203,   279,     2,     0,     0,
     2,     0,    76,    15,     0,     0,    92,    97,     0,     0,
     0,     0,     0,    81,    89,     8,     0,    65,    62,     0,
    90,    90,     0,     2,   136,     0,     2,     0,    77,     0,
     0,     2,    53,    20,    93,    98,   132,     0,   297,     0,
     0,     0,     0,    63,     0,    68,   134,   298,    82,    78,
    66,     0,    21,     2,     0,    54,    69,    83,     0,     0,
     0,   142,     0,    70,    67,     0,   142,    71,     0,   142,
    72,     0,     0,    73,     0,     0
};

static const short yydefgoto[] = {     1,
    56,   315,   360,   530,   317,   414,    86,   316,   533,    88,
   319,   466,   556,   596,   320,   184,   275,   186,   200,   339,
   100,   584,   370,   365,   465,   581,   554,   580,   605,   603,
   610,   616,   619,   622,   411,   531,   567,   592,   459,   550,
   578,   604,   462,   501,   499,   551,   419,   536,   571,   508,
   537,   572,   509,   273,   274,   325,   308,   309,   104,   106,
   335,   340,   388,   482,   519,   438,    84,   270,    57,   231,
   232,   227,   229,   230,   404,   164,   405,   350,   522,   172,
   313,   458,    78,    58,   337,    59,    60,   297,   119,   120,
   201,    61,   394,    62,   122,   295,   346,   347,   348,   204,
   205,   123,    63,   456
};

static const short yypact[] = {-32768,
   578,  2278,  2278,  2278,  2278,  2278,   -66,   -66,  2278,  2278,
  2278,  2278,  2278,   377,   -11,   -59,   -57,-32768,-32768,   -55,
-32768,  2278,-32768,   -52,-32768,   -32,  1942,  1968,     6,    22,
    24,  2051,  2278,-32768,  2278,   -12,    68,    91,     0,     8,
    17,   110,   109,   111,   124,-32768,-32768,-32768,-32768,  2278,
-32768,   -24,-32768,-32768,-32768,-32768,-32768,-32768,-32768,  2418,
   749,-32768,-32768,  3436,   200,   200,-32768,-32768,   -24,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,  2278,-32768,   -24,
   137,  2077,  2278,    36,  3344,  1853,  2278,   139,  2278,-32768,
  2465,-32768,  2508,-32768,   142,-32768,-32768,  2555,  2598,  2278,
  2645,  2278,   -11,   247,   -11,   251,   -66,   -66,   -66,   162,
   -66,  2278,  2278,   778,   564,  2278,     5,   150,   204,   164,
-32768,   -42,   242,  2455,  2545,-32768,  2278,-32768,-32768,-32768,
-32768,  2278,  2278,  2278,  2278,  2278,  2278,  2278,  2278,  2278,
  2278,  2278,  2278,  2278,  2278,  2278,  2278,  2278,-32768,  2278,
  2278,  2278,  2278,  2278,  2278,  2278,  2278,  2278,  2278,  2278,
  2278,-32768,-32768,   153,-32768,   166,   168,-32768,   220,-32768,
-32768,   170,-32768,  2735,  2778,  2278,-32768,   212,  2824,  2278,
  2867,-32768,-32768,   175,-32768,   176,-32768,-32768,  2688,-32768,
  2329,-32768,   182,   281,   183,   186,   187,   188,   240,   194,
    58,-32768,  3223,   198,   301,  2913,-32768,-32768,-32768,  3180,
-32768,   -24,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,   -13,-32768,-32768,-32768,-32768,  2278,  3415,  2278,  2278,
  2278,  2278,   744,  3505,  3519,  3533,  3533,  1490,  1490,  1490,
  1490,   171,   171,   200,   200,   200,-32768,-32768,-32768,  3436,
  3436,  3436,  3436,  3436,  3436,  3436,  3436,  3436,  3436,  3436,
  3436,  2167,   -24,-32768,-32768,   288,  3344,-32768,-32768,   307,
  3344,-32768,   206,    15,   209,   209,-32768,  2278,   210,   -11,
   177,   -11,-32768,-32768,-32768,-32768,-32768,   -66,   317,  2278,
-32768,  2278,   215,   276,   213,     7,-32768,   277,   221,     9,
    -9,  3380,  3436,  3304,  3473,  3490,   -66,   223,   328,    59,
  3344,   435,  2167,-32768,  1853,   225,   867,  2278,   226,    14,
-32768,   229,   230,   278,   340,   234,   235,  2956,-32768,-32768,
   208,   208,-32768,  2278,-32768,-32768,-32768,   339,   241,   133,
-32768,  2278,  3344,  3267,-32768,   300,   236,  2278,-32768,   244,
   -23,   297,   -22,   299,-32768,-32768,-32768,  2197,   252,-32768,
-32768,  2278,-32768,-32768,-32768,  3344,  2278,    -6,    -3,-32768,
   956,   278,   278,   353,   140,   256,   257,   255,-32768,-32768,
  3002,   177,-32768,   308,   310,   259,-32768,-32768,  3436,  2278,
   696,-32768,   159,   213,  2167,   258,   260,   312,-32768,   -16,
   313,-32768,   267,  2278,    31,   -66,    61,  2417,-32768,  1853,
    73,  3045,  1045,-32768,   307,  2278,    -4,    98,   294,    98,
   269,   268,-32768,-32768,   208,   270,   271,   278,-32768,-32768,
-32768,-32768,-32768,   141,-32768,   272,   278,    50,  3344,-32768,
   690,-32768,   300,   275,-32768,-32768,   264,   274,   279,   332,
   286,-32768,  3455,   336,   337,-32768,-32768,    31,   121,   289,
-32768,-32768,   290,   292,   321,   293,  2375,-32768,-32768,   334,
   298,   302,-32768,-32768,-32768,   278,   278,   397,  1134,  1223,
-32768,   175,-32768,   400,   303,-32768,-32768,-32768,-32768,   320,
   324,   295,-32768,   175,   325,-32768,   333,   395,-32768,-32768,
  1853,-32768,-32768,   346,  2278,-32768,-32768,-32768,-32768,   322,
-32768,-32768,-32768,-32768,   208,-32768,-32768,   347,   209,  2278,
   278,    31,-32768,-32768,   349,   350,   209,-32768,-32768,   384,
  2278,-32768,-32768,  2278,   307,-32768,-32768,  1853,  1853,-32768,
-32768,-32768,   348,  3344,   452,-32768,-32768,-32768,   351,  2278,
-32768,   352,  3344,   383,  3091,   354,  1853,  1853,  1312,   360,
  2278,  1401,   361,  3344,  1853,-32768,   362,-32768,-32768,  1497,
    98,    98,   363,-32768,  3344,   364,-32768,   370,-32768,  1853,
   867,-32768,-32768,-32768,-32768,-32768,-32768,  1586,-32768,  1675,
   464,  1853,   405,-32768,  1764,-32768,-32768,-32768,-32768,-32768,
-32768,   376,   408,-32768,   386,-32768,-32768,  1853,  2278,   387,
  3134,  2278,   390,   307,-32768,   391,  2278,   307,   393,  2278,
   307,   388,  1497,-32768,   497,-32768
};

static const short yypgoto[] = {   -43,
   -70,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,  -117,-32768,   -74,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,  -355,-32768,-32768,-32768,
-32768,-32768,-32768,  -263,-32768,-32768,  -279,-32768,-32768,-32768,
   127,   129,-32768,-32768,-32768,-32768,-32768,  -343,  -235,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,  -258,  -310,   -37,    -2,   152,  -172,   444,
-32768,    11,-32768,   -10,    10,-32768,   120,-32768,-32768,-32768,
-32768,   146,-32768,  -430
};


#define	YYLAST		3573


static const short yytable[] = {    64,
    65,    66,    67,    68,    81,   114,    72,    73,    74,    75,
    76,   326,   327,   421,   117,   178,   468,    70,    71,    85,
   379,   380,   336,   415,    91,    93,   310,   496,   400,    98,
    99,   117,   101,   359,   368,    79,   396,   397,   176,   298,
   322,   121,   169,   448,   449,    69,   300,   115,    79,    82,
   353,    83,   485,  -235,  -243,  -240,    87,  -240,   121,   351,
   288,  -114,   470,  -117,   472,    94,  -256,   416,   417,   121,
   416,   417,   336,   336,   401,   168,    89,   310,   167,   174,
   175,    95,   116,    96,   179,   402,   181,    80,   398,   171,
   298,   546,   192,   301,   194,   450,   102,   189,   323,   191,
    80,  -244,   354,  -242,   418,   352,   469,   420,   107,   203,
   206,   454,   455,   210,   475,   444,   108,   196,   197,   198,
   369,   202,   407,   336,   228,   109,   324,   460,   461,   233,
   234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
   244,   245,   246,   247,   248,   249,   177,   250,   251,   252,
   253,   254,   255,   256,   257,   258,   259,   260,   261,   310,
   486,   535,   126,   127,   128,   426,   336,   289,  -114,   110,
  -117,   416,   417,   267,   296,   497,   498,   271,   129,   103,
   130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
   140,   141,   142,   143,   144,   145,   146,   147,   148,   124,
   125,   299,   105,   118,   541,   143,   144,   145,   146,   147,
   148,   331,   332,   384,   385,   585,   586,   111,   518,   112,
   165,   384,   385,   427,   302,   296,   303,   304,   305,   306,
   526,   170,   113,   386,    18,    19,   333,   146,   147,   148,
   387,   386,   331,   332,   361,  -205,   364,   180,   481,   193,
   185,   428,   121,   195,   211,   543,   336,   199,  -254,   311,
   212,   262,   263,   549,  -243,    18,    19,   333,   614,   330,
  -240,   338,   312,   618,   442,   328,   621,   371,   264,   268,
    46,    47,    48,  -103,   276,   334,   374,   343,   281,   344,
    54,    55,   213,   280,   282,   283,   284,   285,   341,   286,
   287,   214,   215,   292,   216,   217,   218,   291,   314,   318,
   311,    46,    47,    48,   321,   366,   410,   356,  -103,   413,
   329,    54,    55,   312,   342,   345,  -241,   349,  -261,  -201,
   358,   381,   357,   362,   423,   424,   367,    79,   219,   389,
   372,   373,   375,   376,   377,   393,   382,   383,   220,   221,
   391,   392,   395,   222,   223,   311,   399,   224,   403,   412,
   425,   409,   429,   430,   271,   431,   471,   435,   408,   436,
   437,   447,   451,   445,   452,   446,   473,    77,   474,   489,
   483,   476,   477,  -217,   488,   479,   480,   439,   441,   490,
   478,   492,   311,   493,   491,   494,   495,   500,   504,   484,
   502,   453,   503,   505,   515,   312,   510,   520,   511,   512,
   525,  -217,  -217,   467,   521,   529,   457,  -217,  -217,  -217,
  -217,  -217,  -217,  -217,  -217,  -217,  -217,   523,  -217,  -217,
   532,   524,   540,   527,  -217,  -217,  -217,  -115,   513,   514,
   552,   528,   150,   151,   152,   153,   154,   155,   156,   157,
   158,   159,   160,   161,   534,   542,   547,   560,   548,   561,
   563,   568,   566,   570,   538,   539,   574,   577,  -217,  -217,
  -217,   579,   601,   587,   589,  -217,  -217,   162,   163,   591,
  -217,  -217,  -217,   545,   599,  -217,   606,   607,  -217,  -217,
  -217,  -217,   557,   558,   609,   612,   626,   623,   559,   583,
   615,   617,   271,   620,   562,   624,   594,   565,   433,   593,
   364,   434,   166,   443,     0,     0,     0,   544,     0,     0,
     0,   600,     0,     0,     0,     0,     0,     0,   553,     0,
   588,   555,     0,   590,     0,     0,     0,     0,   595,     0,
     0,     0,     0,     0,  -115,     0,     0,   564,     0,     0,
     0,     0,   583,     0,     0,     0,     0,     0,   575,     0,
   608,     0,     0,     0,   208,     0,     0,   126,   127,   128,
     0,     0,     0,     0,     0,     0,     0,   625,     0,     0,
     0,     0,     0,   129,     2,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,     0,     0,   611,     0,     0,   271,
     0,     0,     3,     4,   271,     0,     0,   271,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,    17,     0,     0,     0,    18,    19,    20,     0,    21,
     0,     0,     0,    22,    23,    24,     0,    25,     0,    26,
     0,     0,     0,    27,    28,     0,     0,     0,    29,    30,
    31,     0,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,   209,     0,     0,    43,    44,     0,    45,
     0,    46,    47,    48,    49,     0,    50,     0,    51,    52,
    53,    54,    55,   126,   127,   128,     0,     0,     0,     0,
     0,     0,     2,     0,     0,     0,     0,     0,     0,   129,
     0,   130,   131,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     3,     4,     0,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,     0,
     0,     0,     0,    18,    19,    20,   150,   151,   152,   153,
   154,   155,   156,   157,   158,   159,   160,   161,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,     2,     0,     0,    39,    40,    41,
     0,   162,   163,     0,    43,    44,     0,     0,     0,    46,
    47,    48,     0,     0,    50,   487,     0,    52,    53,    54,
    55,   440,     3,     4,     0,     0,     0,     0,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,    17,     0,     0,     0,    18,    19,    20,     0,    21,
     0,     0,     0,    22,    23,    24,     0,    25,     0,    26,
     0,     0,     0,    27,    28,     0,     0,     0,    29,    30,
    31,     0,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,     2,     0,     0,    43,    44,     0,    45,
     0,    46,    47,    48,    49,   207,    50,   363,    51,    52,
    53,    54,    55,     0,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     0,     0,     0,     0,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,     0,    15,    16,
    17,     0,     0,     0,    18,    19,    20,     0,    21,     0,
     0,     0,    22,    23,    24,     0,    25,     0,    26,     0,
     0,     0,    27,    28,     0,     0,     0,    29,    30,    31,
     0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,     2,     0,     0,    43,    44,     0,    45,     0,
    46,    47,    48,    49,     0,    50,     0,    51,    52,    53,
    54,    55,     0,     0,     0,     0,     0,     0,     0,     0,
     3,     4,     0,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,    17,
     0,     0,     0,    18,    19,    20,     0,    21,     0,     0,
     0,    22,    23,    24,     0,    25,     0,    26,     0,     0,
     0,    27,    28,     0,     0,     0,    29,    30,    31,     0,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
    42,     2,     0,     0,    43,    44,     0,    45,     0,    46,
    47,    48,    49,     0,    50,   422,    51,    52,    53,    54,
    55,     0,     0,     0,     0,     0,     0,     0,     0,     3,
     4,     0,     0,     0,     0,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,     0,    15,    16,    17,     0,
     0,     0,    18,    19,    20,     0,    21,     0,     0,     0,
    22,    23,    24,   464,    25,     0,    26,     0,     0,     0,
    27,    28,     0,     0,     0,    29,    30,    31,     0,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
     2,     0,     0,    43,    44,     0,    45,     0,    46,    47,
    48,    49,     0,    50,     0,    51,    52,    53,    54,    55,
     0,     0,     0,     0,     0,     0,     0,     0,     3,     4,
     0,     0,     0,     0,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,    17,     0,     0,
     0,    18,    19,    20,     0,    21,     0,     0,     0,    22,
    23,    24,     0,    25,     0,    26,     0,     0,     0,    27,
    28,     0,     0,     0,    29,    30,    31,     0,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,    42,     2,
     0,     0,    43,    44,     0,    45,     0,    46,    47,    48,
    49,   516,    50,     0,    51,    52,    53,    54,    55,     0,
     0,     0,     0,     0,     0,     0,     0,     3,     4,     0,
     0,     0,     0,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,     0,    15,    16,    17,     0,     0,     0,
    18,    19,    20,     0,    21,     0,     0,     0,    22,    23,
    24,     0,    25,     0,    26,     0,     0,     0,    27,    28,
     0,     0,     0,    29,    30,    31,     0,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,     2,     0,
     0,    43,    44,     0,    45,     0,    46,    47,    48,    49,
   517,    50,     0,    51,    52,    53,    54,    55,     0,     0,
     0,     0,     0,     0,     0,     0,     3,     4,     0,     0,
     0,     0,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,     0,    15,    16,    17,     0,     0,     0,    18,
    19,    20,     0,    21,     0,     0,     0,    22,    23,    24,
     0,    25,     0,    26,     0,     0,     0,    27,    28,     0,
     0,     0,    29,    30,    31,     0,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,     2,     0,     0,
    43,    44,     0,    45,     0,    46,    47,    48,    49,     0,
    50,   573,    51,    52,    53,    54,    55,     0,     0,     0,
     0,     0,     0,     0,     0,     3,     4,     0,     0,     0,
     0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,     0,    15,    16,    17,     0,     0,     0,    18,    19,
    20,     0,    21,     0,     0,     0,    22,    23,    24,     0,
    25,     0,    26,     0,     0,     0,    27,    28,     0,     0,
     0,    29,    30,    31,     0,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,     0,     0,     0,    43,
    44,     0,    45,     2,    46,    47,    48,    49,     0,    50,
   576,    51,    52,    53,    54,    55,     0,   582,-32768,-32768,
-32768,-32768,   141,   142,   143,   144,   145,   146,   147,   148,
     0,     3,     4,     0,     0,     0,     0,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,     0,    15,    16,
    17,     0,     0,     0,    18,    19,    20,     0,    21,     0,
     0,     0,    22,    23,    24,     0,    25,     0,    26,     0,
     0,     0,    27,    28,     0,     0,     0,    29,    30,    31,
     0,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,     2,     0,     0,    43,    44,     0,    45,     0,
    46,    47,    48,    49,     0,    50,     0,    51,    52,    53,
    54,    55,     0,     0,     0,     0,     0,     0,     0,     0,
     3,     4,     0,     0,     0,     0,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,     0,    15,    16,    17,
     0,     0,     0,    18,    19,    20,     0,    21,     0,     0,
     0,    22,    23,    24,     0,    25,     0,    26,     0,     0,
     0,    27,    28,     0,     0,     0,    29,    30,    31,     0,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
    42,     2,     0,     0,    43,    44,     0,    45,     0,    46,
    47,    48,    49,   597,    50,     0,    51,    52,    53,    54,
    55,     0,     0,     0,     0,     0,     0,     0,     0,     3,
     4,     0,     0,     0,     0,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,     0,    15,    16,    17,     0,
     0,     0,    18,    19,    20,     0,    21,     0,     0,     0,
    22,    23,    24,     0,    25,     0,    26,     0,     0,     0,
    27,    28,     0,     0,     0,    29,    30,    31,     0,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
     2,     0,     0,    43,    44,     0,    45,     0,    46,    47,
    48,    49,   598,    50,     0,    51,    52,    53,    54,    55,
     0,     0,     0,     0,     0,     0,     0,     0,     3,     4,
     0,     0,     0,     0,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,     0,    15,    16,    17,     0,     0,
     0,    18,    19,    20,     0,    21,     0,     0,     0,    22,
    23,    24,     0,    25,   602,    26,     0,     0,     0,    27,
    28,     0,     0,     0,    29,    30,    31,     0,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,    42,     2,
     0,     0,    43,    44,     0,    45,     0,    46,    47,    48,
    49,     0,    50,     0,    51,    52,    53,    54,    55,     0,
     0,     0,     0,     0,     0,     0,     0,     3,     4,     0,
     0,     0,     0,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,     0,    15,    16,    17,     0,     0,     0,
    18,    19,    20,     0,    21,     0,     0,     0,    22,    23,
    24,     0,    25,     0,    26,     0,     0,     0,    27,    28,
     0,     0,     0,    29,    30,    31,     0,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,     2,     0,
     0,    43,    44,     0,    45,     0,    46,    47,    48,    49,
     0,    50,     0,    51,    52,    53,    54,    55,     0,     0,
     0,     0,     0,     0,     2,     0,     3,     4,     0,     0,
     0,     0,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,     0,    15,    16,     0,     0,     0,     0,    18,
    19,    20,     3,     4,     0,     0,     0,     0,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,     0,     0,     0,     0,    18,    19,    20,     0,     0,
     0,     0,     0,    39,    40,    41,     0,     0,     0,     0,
    43,    44,     0,     0,     0,    46,    47,    48,     0,     0,
    50,     0,    90,    52,    53,    54,    55,     2,     0,    39,
    40,    41,     0,     0,     0,     0,    43,    44,     0,     0,
     0,    46,    47,    48,     0,     0,    50,     0,    92,    52,
    53,    54,    55,     2,     0,     3,     4,     0,     0,     0,
     0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,     0,    15,    16,     0,     0,     0,     0,    18,    19,
    20,     3,     4,     0,     0,     0,     0,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,     0,    15,    16,
     0,     0,     0,     0,    18,    19,    20,     0,     0,     0,
     0,     0,    39,    40,    41,     0,     0,     0,     0,    43,
    44,     0,     0,     0,    46,    47,    48,     0,     0,    50,
     0,    97,    52,    53,    54,    55,     0,     0,    39,    40,
    41,     0,     0,     2,     0,    43,    44,     0,     0,     0,
    46,    47,    48,     0,     0,    50,   173,     0,    52,    53,
    54,    55,   307,     0,     0,     0,     0,     0,     0,     0,
     0,     3,     4,     2,     0,     0,     0,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,     0,    15,    16,
     0,     0,   406,     0,    18,    19,    20,     0,     0,     0,
     0,     3,     4,     0,     0,     0,     0,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,     0,    15,    16,
     0,     0,     0,     0,    18,    19,    20,     0,    39,    40,
    41,     0,     0,     0,     0,    43,    44,     0,     0,     0,
    46,    47,    48,     0,     0,    50,     0,     0,    52,    53,
    54,    55,     0,     0,     2,     0,     0,     0,    39,    40,
    41,     0,     0,     0,     0,    43,    44,     0,     0,     0,
    46,    47,    48,     0,     0,    50,     0,     0,    52,    53,
    54,    55,     3,     4,     0,     0,     0,     0,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,     0,    15,
    16,   278,   126,   127,   128,    18,    19,    20,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   129,     0,
   130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
   140,   141,   142,   143,   144,   145,   146,   147,   148,    39,
    40,    41,     0,     0,     0,     0,    43,    44,   126,   127,
   128,    46,    47,    48,     0,     0,    50,     0,     0,    52,
    53,    54,    55,     0,   129,   506,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,     0,     0,     0,     0,  -118,
     0,   126,   127,   128,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,   160,   161,     0,   129,   279,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,   147,   148,     0,   162,
   163,     0,     0,     0,     0,     0,     0,     0,   126,   127,
   128,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   129,   507,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,   213,     0,     0,     0,     0,
     0,   126,   127,   128,   214,   215,     0,   216,   217,   218,
     0,     0,     0,     0,     0,     0,  -118,   129,   149,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,   147,   148,     0,     0,
     0,   219,     0,     0,     0,     0,     0,     0,   126,   127,
   128,   220,   221,     0,     0,     0,   222,     0,   225,     0,
   224,     0,     0,     0,   129,   182,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,   213,     0,     0,     0,     0,
     0,   126,   127,   128,   214,   215,     0,   216,   217,   218,
     0,     0,     0,     0,     0,     0,     0,   129,   183,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,   147,   148,     0,     0,
     0,   219,     0,     0,     0,     0,     0,     0,   126,   127,
   128,   220,   221,     0,     0,     0,   222,     0,     0,   226,
   224,     0,     0,     0,   129,   187,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,     0,     0,     0,     0,     0,
     0,   126,   127,   128,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   129,   188,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,   147,   148,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,   126,   127,
   128,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   129,   190,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,     0,     0,     0,     0,     0,
     0,   126,   127,   128,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   129,   277,   130,
   131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
   141,   142,   143,   144,   145,   146,   147,   148,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   126,   127,   128,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   129,   265,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,     0,     0,     0,     0,     0,     0,
   126,   127,   128,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   129,   266,   130,   131,
   132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
   142,   143,   144,   145,   146,   147,   148,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   126,   127,   128,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   129,   269,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   147,   148,     0,     0,     0,     0,     0,     0,   126,
   127,   128,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   129,   272,   130,   131,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   145,   146,   147,   148,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   126,   127,   128,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   129,   293,   130,   131,   132,   133,   134,   135,   136,
   137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
   147,   148,     0,     0,     0,     0,     0,     0,   126,   127,
   128,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   129,   378,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   126,   127,   128,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   129,   432,   130,   131,   132,   133,   134,   135,   136,   137,
   138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
   148,     0,     0,     0,     0,     0,     0,   126,   127,   128,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   129,   463,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   126,   127,   128,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   129,
   569,   130,   131,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     0,     0,     0,     0,     0,     0,   126,   127,   128,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   129,   613,   130,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   147,   148,     0,     0,     0,     0,     0,     0,     0,
   126,   127,   128,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   129,   294,   130,   131,
   132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
   142,   143,   144,   145,   146,   147,   148,   126,   127,   128,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   290,     0,     0,   129,   355,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,     0,     0,     0,   126,   127,   128,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   129,   390,   130,   131,   132,   133,   134,
   135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
   145,   146,   147,   148,   127,   128,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   129,
     0,   130,   131,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
   128,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   129,     0,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,   129,     0,   130,   131,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   145,   146,   147,   148,   130,   131,   132,   133,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,   131,   132,   133,   134,   135,
   136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   147,   148,   132,   133,   134,   135,   136,   137,   138,
   139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
   134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
   144,   145,   146,   147,   148,   135,   136,   137,   138,   139,
   140,   141,   142,   143,   144,   145,   146,   147,   148,-32768,
-32768,   137,   138,   139,   140,   141,   142,   143,   144,   145,
   146,   147,   148
};

static const short yycheck[] = {     2,
     3,     4,     5,     6,    15,    49,     9,    10,    11,    12,
    13,   275,   276,   369,    52,    86,    21,     7,     8,    22,
   331,   332,   281,   367,    27,    28,   262,   458,    51,    32,
    33,    69,    35,   313,    21,    60,    60,    61,     3,   212,
    26,    52,    80,    60,    61,   112,    60,    50,    60,   109,
    60,   109,     3,   109,    97,    51,   109,    51,    69,    51,
     3,     3,   418,     3,   420,    60,   109,    74,    75,    80,
    74,    75,   331,   332,    97,    78,   109,   313,    69,    82,
    83,    60,   107,    60,    87,   108,    89,   112,   112,    80,
   263,   522,   103,   107,   105,   112,   109,   100,    84,   102,
   112,    97,   112,    97,   111,    97,   111,   111,   109,   112,
   113,    81,    82,   116,   425,   395,   109,   107,   108,   109,
   107,   111,   358,   382,   127,   109,   112,    55,    56,   132,
   133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
   143,   144,   145,   146,   147,   148,   111,   150,   151,   152,
   153,   154,   155,   156,   157,   158,   159,   160,   161,   395,
   111,   505,     4,     5,     6,    26,   425,   110,   110,    60,
   110,    74,    75,   176,   212,    55,    56,   180,    20,   112,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    54,
    55,   212,   112,    52,   515,    35,    36,    37,    38,    39,
    40,    35,    36,    81,    82,   571,   572,   109,   482,   109,
    69,    81,    82,    84,   227,   263,   229,   230,   231,   232,
   494,    80,   109,   101,    58,    59,    60,    38,    39,    40,
   108,   101,    35,    36,   315,   109,   317,   109,   108,     3,
   109,   112,   263,     3,    51,   519,   515,    96,   109,   262,
    97,   109,    97,   527,    97,    58,    59,    60,   612,   280,
    51,   282,   262,   617,   116,   278,   620,   321,   109,    68,
   104,   105,   106,   109,   109,   109,   324,   290,     8,   292,
   114,   115,    51,   112,   112,   110,   110,   110,   288,    60,
   107,    60,    61,     3,    63,    64,    65,   110,    21,     3,
   313,   104,   105,   106,   109,   318,   360,   307,   110,   363,
   111,   114,   115,   313,     8,   111,    51,    51,   116,   109,
     3,   334,   110,   109,   372,   373,   111,    60,    97,   342,
   112,   112,     3,   110,   110,   348,     8,   107,   107,   108,
    51,   116,   109,   112,   113,   358,    60,   116,    60,   362,
     8,   110,   107,   107,   367,   111,    73,    60,   358,    60,
   112,    60,    60,   116,   108,   116,   108,     1,   111,   116,
   109,   112,   112,     7,   110,   429,   430,   390,   391,   116,
   428,    60,   395,   108,   116,    60,    60,   109,    78,   437,
   111,   404,   111,   111,     8,   395,    73,     8,   111,   108,
   116,    35,    36,   416,   112,    21,   406,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,   108,    52,    53,
   501,   108,   111,   109,    58,    59,    60,     3,   476,   477,
    57,   109,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,   109,   109,   108,   110,   109,     8,
   110,    79,   111,   110,   508,   509,   107,   107,    92,    93,
    94,   110,    68,   111,   111,    99,   100,    43,    44,   110,
   104,   105,   106,   521,    21,   109,   111,    80,   112,   113,
   114,   115,   536,   537,   109,   109,     0,   110,   542,   570,
   111,   111,   505,   111,   548,   623,   581,   551,   382,   580,
   581,   383,    69,   394,    -1,    -1,    -1,   520,    -1,    -1,
    -1,   592,    -1,    -1,    -1,    -1,    -1,    -1,   531,    -1,
   574,   534,    -1,   577,    -1,    -1,    -1,    -1,   582,    -1,
    -1,    -1,    -1,    -1,   110,    -1,    -1,   550,    -1,    -1,
    -1,    -1,   623,    -1,    -1,    -1,    -1,    -1,   561,    -1,
   604,    -1,    -1,    -1,     1,    -1,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,    -1,
    -1,    -1,    -1,    20,     7,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    -1,    -1,   609,    -1,    -1,   612,
    -1,    -1,    35,    36,   617,    -1,    -1,   620,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
    53,    54,    -1,    -1,    -1,    58,    59,    60,    -1,    62,
    -1,    -1,    -1,    66,    67,    68,    -1,    70,    -1,    72,
    -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    81,    82,
    83,    -1,    85,    86,    87,    88,    89,    90,    91,    92,
    93,    94,    95,   110,    -1,    -1,    99,   100,    -1,   102,
    -1,   104,   105,   106,   107,    -1,   109,    -1,   111,   112,
   113,   114,   115,     4,     5,     6,    -1,    -1,    -1,    -1,
    -1,    -1,     7,    -1,    -1,    -1,    -1,    -1,    -1,    20,
    -1,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    -1,    52,    53,    -1,
    -1,    -1,    -1,    58,    59,    60,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,     7,    -1,    -1,    92,    93,    94,
    -1,    43,    44,    -1,    99,   100,    -1,    -1,    -1,   104,
   105,   106,    -1,    -1,   109,   116,    -1,   112,   113,   114,
   115,   116,    35,    36,    -1,    -1,    -1,    -1,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
    53,    54,    -1,    -1,    -1,    58,    59,    60,    -1,    62,
    -1,    -1,    -1,    66,    67,    68,    -1,    70,    -1,    72,
    -1,    -1,    -1,    76,    77,    -1,    -1,    -1,    81,    82,
    83,    -1,    85,    86,    87,    88,    89,    90,    91,    92,
    93,    94,    95,     7,    -1,    -1,    99,   100,    -1,   102,
    -1,   104,   105,   106,   107,   108,   109,    21,   111,   112,
   113,   114,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
    54,    -1,    -1,    -1,    58,    59,    60,    -1,    62,    -1,
    -1,    -1,    66,    67,    68,    -1,    70,    -1,    72,    -1,
    -1,    -1,    76,    77,    -1,    -1,    -1,    81,    82,    83,
    -1,    85,    86,    87,    88,    89,    90,    91,    92,    93,
    94,    95,     7,    -1,    -1,    99,   100,    -1,   102,    -1,
   104,   105,   106,   107,    -1,   109,    -1,   111,   112,   113,
   114,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    -1,    52,    53,    54,
    -1,    -1,    -1,    58,    59,    60,    -1,    62,    -1,    -1,
    -1,    66,    67,    68,    -1,    70,    -1,    72,    -1,    -1,
    -1,    76,    77,    -1,    -1,    -1,    81,    82,    83,    -1,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
    95,     7,    -1,    -1,    99,   100,    -1,   102,    -1,   104,
   105,   106,   107,    -1,   109,   110,   111,   112,   113,   114,
   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
    36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    -1,    52,    53,    54,    -1,
    -1,    -1,    58,    59,    60,    -1,    62,    -1,    -1,    -1,
    66,    67,    68,    69,    70,    -1,    72,    -1,    -1,    -1,
    76,    77,    -1,    -1,    -1,    81,    82,    83,    -1,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
     7,    -1,    -1,    99,   100,    -1,   102,    -1,   104,   105,
   106,   107,    -1,   109,    -1,   111,   112,   113,   114,   115,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,
    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    -1,    52,    53,    54,    -1,    -1,
    -1,    58,    59,    60,    -1,    62,    -1,    -1,    -1,    66,
    67,    68,    -1,    70,    -1,    72,    -1,    -1,    -1,    76,
    77,    -1,    -1,    -1,    81,    82,    83,    -1,    85,    86,
    87,    88,    89,    90,    91,    92,    93,    94,    95,     7,
    -1,    -1,    99,   100,    -1,   102,    -1,   104,   105,   106,
   107,   108,   109,    -1,   111,   112,   113,   114,   115,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,
    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    -1,    52,    53,    54,    -1,    -1,    -1,
    58,    59,    60,    -1,    62,    -1,    -1,    -1,    66,    67,
    68,    -1,    70,    -1,    72,    -1,    -1,    -1,    76,    77,
    -1,    -1,    -1,    81,    82,    83,    -1,    85,    86,    87,
    88,    89,    90,    91,    92,    93,    94,    95,     7,    -1,
    -1,    99,   100,    -1,   102,    -1,   104,   105,   106,   107,
   108,   109,    -1,   111,   112,   113,   114,   115,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,    -1,
    -1,    -1,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    -1,    52,    53,    54,    -1,    -1,    -1,    58,
    59,    60,    -1,    62,    -1,    -1,    -1,    66,    67,    68,
    -1,    70,    -1,    72,    -1,    -1,    -1,    76,    77,    -1,
    -1,    -1,    81,    82,    83,    -1,    85,    86,    87,    88,
    89,    90,    91,    92,    93,    94,    95,     7,    -1,    -1,
    99,   100,    -1,   102,    -1,   104,   105,   106,   107,    -1,
   109,   110,   111,   112,   113,   114,   115,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,    -1,    -1,
    -1,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    -1,    52,    53,    54,    -1,    -1,    -1,    58,    59,
    60,    -1,    62,    -1,    -1,    -1,    66,    67,    68,    -1,
    70,    -1,    72,    -1,    -1,    -1,    76,    77,    -1,    -1,
    -1,    81,    82,    83,    -1,    85,    86,    87,    88,    89,
    90,    91,    92,    93,    94,    95,    -1,    -1,    -1,    99,
   100,    -1,   102,     7,   104,   105,   106,   107,    -1,   109,
   110,   111,   112,   113,   114,   115,    -1,    21,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    -1,    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
    54,    -1,    -1,    -1,    58,    59,    60,    -1,    62,    -1,
    -1,    -1,    66,    67,    68,    -1,    70,    -1,    72,    -1,
    -1,    -1,    76,    77,    -1,    -1,    -1,    81,    82,    83,
    -1,    85,    86,    87,    88,    89,    90,    91,    92,    93,
    94,    95,     7,    -1,    -1,    99,   100,    -1,   102,    -1,
   104,   105,   106,   107,    -1,   109,    -1,   111,   112,   113,
   114,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    -1,    52,    53,    54,
    -1,    -1,    -1,    58,    59,    60,    -1,    62,    -1,    -1,
    -1,    66,    67,    68,    -1,    70,    -1,    72,    -1,    -1,
    -1,    76,    77,    -1,    -1,    -1,    81,    82,    83,    -1,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
    95,     7,    -1,    -1,    99,   100,    -1,   102,    -1,   104,
   105,   106,   107,   108,   109,    -1,   111,   112,   113,   114,
   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
    36,    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    -1,    52,    53,    54,    -1,
    -1,    -1,    58,    59,    60,    -1,    62,    -1,    -1,    -1,
    66,    67,    68,    -1,    70,    -1,    72,    -1,    -1,    -1,
    76,    77,    -1,    -1,    -1,    81,    82,    83,    -1,    85,
    86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
     7,    -1,    -1,    99,   100,    -1,   102,    -1,   104,   105,
   106,   107,   108,   109,    -1,   111,   112,   113,   114,   115,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,
    -1,    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    -1,    52,    53,    54,    -1,    -1,
    -1,    58,    59,    60,    -1,    62,    -1,    -1,    -1,    66,
    67,    68,    -1,    70,    71,    72,    -1,    -1,    -1,    76,
    77,    -1,    -1,    -1,    81,    82,    83,    -1,    85,    86,
    87,    88,    89,    90,    91,    92,    93,    94,    95,     7,
    -1,    -1,    99,   100,    -1,   102,    -1,   104,   105,   106,
   107,    -1,   109,    -1,   111,   112,   113,   114,   115,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    -1,
    -1,    -1,    -1,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    -1,    52,    53,    54,    -1,    -1,    -1,
    58,    59,    60,    -1,    62,    -1,    -1,    -1,    66,    67,
    68,    -1,    70,    -1,    72,    -1,    -1,    -1,    76,    77,
    -1,    -1,    -1,    81,    82,    83,    -1,    85,    86,    87,
    88,    89,    90,    91,    92,    93,    94,    95,     7,    -1,
    -1,    99,   100,    -1,   102,    -1,   104,   105,   106,   107,
    -1,   109,    -1,   111,   112,   113,   114,   115,    -1,    -1,
    -1,    -1,    -1,    -1,     7,    -1,    35,    36,    -1,    -1,
    -1,    -1,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    -1,    52,    53,    -1,    -1,    -1,    -1,    58,
    59,    60,    35,    36,    -1,    -1,    -1,    -1,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
    53,    -1,    -1,    -1,    -1,    58,    59,    60,    -1,    -1,
    -1,    -1,    -1,    92,    93,    94,    -1,    -1,    -1,    -1,
    99,   100,    -1,    -1,    -1,   104,   105,   106,    -1,    -1,
   109,    -1,   111,   112,   113,   114,   115,     7,    -1,    92,
    93,    94,    -1,    -1,    -1,    -1,    99,   100,    -1,    -1,
    -1,   104,   105,   106,    -1,    -1,   109,    -1,   111,   112,
   113,   114,   115,     7,    -1,    35,    36,    -1,    -1,    -1,
    -1,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    -1,    52,    53,    -1,    -1,    -1,    -1,    58,    59,
    60,    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
    -1,    -1,    -1,    -1,    58,    59,    60,    -1,    -1,    -1,
    -1,    -1,    92,    93,    94,    -1,    -1,    -1,    -1,    99,
   100,    -1,    -1,    -1,   104,   105,   106,    -1,    -1,   109,
    -1,   111,   112,   113,   114,   115,    -1,    -1,    92,    93,
    94,    -1,    -1,     7,    -1,    99,   100,    -1,    -1,    -1,
   104,   105,   106,    -1,    -1,   109,   110,    -1,   112,   113,
   114,   115,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    35,    36,     7,    -1,    -1,    -1,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
    -1,    -1,    26,    -1,    58,    59,    60,    -1,    -1,    -1,
    -1,    35,    36,    -1,    -1,    -1,    -1,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
    -1,    -1,    -1,    -1,    58,    59,    60,    -1,    92,    93,
    94,    -1,    -1,    -1,    -1,    99,   100,    -1,    -1,    -1,
   104,   105,   106,    -1,    -1,   109,    -1,    -1,   112,   113,
   114,   115,    -1,    -1,     7,    -1,    -1,    -1,    92,    93,
    94,    -1,    -1,    -1,    -1,    99,   100,    -1,    -1,    -1,
   104,   105,   106,    -1,    -1,   109,    -1,    -1,   112,   113,
   114,   115,    35,    36,    -1,    -1,    -1,    -1,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
    53,     3,     4,     5,     6,    58,    59,    60,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,    -1,
    22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    92,
    93,    94,    -1,    -1,    -1,    -1,    99,   100,     4,     5,
     6,   104,   105,   106,    -1,    -1,   109,    -1,    -1,   112,
   113,   114,   115,    -1,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,     3,
    -1,     4,     5,     6,     8,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    -1,    20,   110,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,    43,
    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,
     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    20,   111,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    51,    -1,    -1,    -1,    -1,
    -1,     4,     5,     6,    60,    61,    -1,    63,    64,    65,
    -1,    -1,    -1,    -1,    -1,    -1,   110,    20,   111,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
    -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,
     6,   107,   108,    -1,    -1,    -1,   112,    -1,   114,    -1,
   116,    -1,    -1,    -1,    20,   111,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    51,    -1,    -1,    -1,    -1,
    -1,     4,     5,     6,    60,    61,    -1,    63,    64,    65,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,   111,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
    -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,
     6,   107,   108,    -1,    -1,    -1,   112,    -1,    -1,   115,
   116,    -1,    -1,    -1,    20,   111,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
    -1,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,   111,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,
     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    20,   111,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
    -1,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,   111,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    20,   110,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    20,   110,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    20,   110,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,     4,
     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    20,   110,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,     4,     5,     6,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    20,   110,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,
     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    20,   110,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,     4,     5,     6,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    20,   110,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    20,   110,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,     4,     5,     6,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
   110,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    -1,    -1,    -1,    -1,    -1,    -1,     4,     5,     6,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    20,   110,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    20,   108,    22,    23,
    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    98,    -1,    -1,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    -1,    -1,    -1,     4,     5,     6,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    20,    98,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,     5,     6,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
    -1,    22,    23,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    20,    -1,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    20,    -1,    22,    23,    24,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    24,    25,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    27,    28,    29,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40
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
#line 281 "language-parser.y"
{ cs_start_if (&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 5:
#line 281 "language-parser.y"
{ cs_end_if ( _INLINE_TLS_VOID); ;
    break;}
case 6:
#line 282 "language-parser.y"
{ cs_start_if (&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 7:
#line 282 "language-parser.y"
{ cs_end_if ( _INLINE_TLS_VOID); ;
    break;}
case 9:
#line 283 "language-parser.y"
{ cs_start_while(&yyvsp[-3], &yyvsp[-1] _INLINE_TLS); ;
    break;}
case 10:
#line 283 "language-parser.y"
{ cs_end_while(&yyvsp[-5],&yychar _INLINE_TLS); ;
    break;}
case 12:
#line 284 "language-parser.y"
{ cs_start_do_while(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 13:
#line 284 "language-parser.y"
{ cs_force_eval_do_while( _INLINE_TLS_VOID); ;
    break;}
case 14:
#line 284 "language-parser.y"
{ cs_end_do_while(&yyvsp[-8],&yyvsp[-2],&yychar _INLINE_TLS); ;
    break;}
case 16:
#line 285 "language-parser.y"
{ for_pre_expr1(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 17:
#line 286 "language-parser.y"
{ if (GLOBAL(Execute)) pval_destructor(&yyvsp[0] _INLINE_TLS); for_pre_expr2(&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 18:
#line 287 "language-parser.y"
{ for_pre_expr3(&yyvsp[-6],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 19:
#line 288 "language-parser.y"
{ for_pre_statement(&yyvsp[-9],&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 20:
#line 289 "language-parser.y"
{ for_post_statement(&yyvsp[-12],&yyvsp[-7],&yyvsp[-4],&yyvsp[-1],&yychar _INLINE_TLS); ;
    break;}
case 22:
#line 290 "language-parser.y"
{ cs_switch_start(&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 23:
#line 290 "language-parser.y"
{ cs_switch_end(&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 24:
#line 291 "language-parser.y"
{ DO_OR_DIE(cs_break_continue(NULL,DO_BREAK _INLINE_TLS)) ;
    break;}
case 25:
#line 292 "language-parser.y"
{ DO_OR_DIE(cs_break_continue(&yyvsp[-1],DO_BREAK _INLINE_TLS)) ;
    break;}
case 26:
#line 293 "language-parser.y"
{ DO_OR_DIE(cs_break_continue(NULL,DO_CONTINUE _INLINE_TLS)) ;
    break;}
case 27:
#line 294 "language-parser.y"
{ DO_OR_DIE(cs_break_continue(&yyvsp[-1],DO_CONTINUE _INLINE_TLS)) ;
    break;}
case 28:
#line 295 "language-parser.y"
{ start_function_decleration(_INLINE_TLS_VOID); ;
    break;}
case 29:
#line 296 "language-parser.y"
{ end_function_decleration(&yyvsp[-7],&yyvsp[-6] _INLINE_TLS); ;
    break;}
case 30:
#line 297 "language-parser.y"
{ start_function_decleration(_INLINE_TLS_VOID); ;
    break;}
case 31:
#line 298 "language-parser.y"
{ end_function_decleration(&yyvsp[-8],&yyvsp[-7] _INLINE_TLS); ;
    break;}
case 32:
#line 299 "language-parser.y"
{ tc_set_token(&GLOBAL(token_cache_manager), yyvsp[-1].offset, FUNCTION); ;
    break;}
case 33:
#line 299 "language-parser.y"
{ if (!GLOBAL(shutdown_requested)) GLOBAL(shutdown_requested) = TERMINATE_CURRENT_PHPPARSE; ;
    break;}
case 34:
#line 300 "language-parser.y"
{ cs_return(NULL _INLINE_TLS); ;
    break;}
case 35:
#line 301 "language-parser.y"
{ cs_return(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 38:
#line 304 "language-parser.y"
{ cs_start_class_decleration(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 39:
#line 304 "language-parser.y"
{ cs_end_class_decleration( _INLINE_TLS_VOID); ;
    break;}
case 40:
#line 305 "language-parser.y"
{ cs_start_class_decleration(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 41:
#line 305 "language-parser.y"
{ cs_end_class_decleration( _INLINE_TLS_VOID); ;
    break;}
case 43:
#line 307 "language-parser.y"
{ if (GLOBAL(Execute)) { if (php3_header()) PUTS(yyvsp[0].value.str.val); } ;
    break;}
case 44:
#line 308 "language-parser.y"
{ if (GLOBAL(Execute)) pval_destructor(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 45:
#line 309 "language-parser.y"
{ php3cs_start_require(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 46:
#line 309 "language-parser.y"
{ php3cs_end_require(&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 47:
#line 310 "language-parser.y"
{ if (GLOBAL(Execute)) cs_show_source(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 48:
#line 311 "language-parser.y"
{ if (GLOBAL(Execute)) eval_string(&yyvsp[-2],NULL,1 _INLINE_TLS); ;
    break;}
case 49:
#line 312 "language-parser.y"
{ if (GLOBAL(Execute)) eval_string(&yyvsp[-4],&yyvsp[-2],2 _INLINE_TLS); ;
    break;}
case 50:
#line 313 "language-parser.y"
{ if (GLOBAL(Execute)) eval_string(&yyvsp[-2],&yyvsp[0],0 _INLINE_TLS); ;
    break;}
case 51:
#line 314 "language-parser.y"
{ if (GLOBAL(Execute)) conditional_include_file(&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 62:
#line 338 "language-parser.y"
{ cs_start_while(&yyvsp[-3], &yyvsp[-1] _INLINE_TLS); ;
    break;}
case 63:
#line 338 "language-parser.y"
{ cs_end_while(&yyvsp[-5],&yychar _INLINE_TLS); ;
    break;}
case 65:
#line 344 "language-parser.y"
{ cs_start_do_while(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 66:
#line 344 "language-parser.y"
{ cs_force_eval_do_while( _INLINE_TLS_VOID); ;
    break;}
case 67:
#line 344 "language-parser.y"
{ cs_end_do_while(&yyvsp[-8],&yyvsp[-2],&yychar _INLINE_TLS); ;
    break;}
case 69:
#line 350 "language-parser.y"
{ for_pre_expr1(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 70:
#line 351 "language-parser.y"
{ if (GLOBAL(Execute)) pval_destructor(&yyvsp[0] _INLINE_TLS); for_pre_expr2(&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 71:
#line 352 "language-parser.y"
{ for_pre_expr3(&yyvsp[-6],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 72:
#line 353 "language-parser.y"
{ for_pre_statement(&yyvsp[-9],&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 73:
#line 354 "language-parser.y"
{ for_post_statement(&yyvsp[-12],&yyvsp[-7],&yyvsp[-4],&yyvsp[-1],&yychar _INLINE_TLS); ;
    break;}
case 75:
#line 360 "language-parser.y"
{ cs_elseif_start_evaluate( _INLINE_TLS_VOID); ;
    break;}
case 76:
#line 361 "language-parser.y"
{ cs_elseif_end_evaluate( _INLINE_TLS_VOID); ;
    break;}
case 77:
#line 361 "language-parser.y"
{ cs_start_elseif (&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 80:
#line 367 "language-parser.y"
{ cs_elseif_start_evaluate( _INLINE_TLS_VOID); ;
    break;}
case 81:
#line 368 "language-parser.y"
{ cs_elseif_end_evaluate( _INLINE_TLS_VOID); ;
    break;}
case 82:
#line 368 "language-parser.y"
{ cs_start_elseif (&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 85:
#line 374 "language-parser.y"
{ cs_start_else( _INLINE_TLS_VOID); ;
    break;}
case 88:
#line 380 "language-parser.y"
{ cs_start_else( _INLINE_TLS_VOID); ;
    break;}
case 91:
#line 386 "language-parser.y"
{ cs_switch_case_pre(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 92:
#line 386 "language-parser.y"
{ cs_switch_case_post( _INLINE_TLS_VOID); ;
    break;}
case 94:
#line 387 "language-parser.y"
{ cs_switch_case_pre(NULL _INLINE_TLS); ;
    break;}
case 95:
#line 387 "language-parser.y"
{ cs_switch_case_post( _INLINE_TLS_VOID); ;
    break;}
case 96:
#line 388 "language-parser.y"
{ cs_switch_case_pre(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 97:
#line 388 "language-parser.y"
{ cs_switch_case_post( _INLINE_TLS_VOID); ;
    break;}
case 99:
#line 389 "language-parser.y"
{ cs_switch_case_pre(NULL _INLINE_TLS); ;
    break;}
case 100:
#line 389 "language-parser.y"
{ cs_switch_case_post( _INLINE_TLS_VOID); ;
    break;}
case 101:
#line 394 "language-parser.y"
{ GLOBAL(param_index)=0; ;
    break;}
case 104:
#line 400 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_NONE, NULL _INLINE_TLS); ;
    break;}
case 105:
#line 401 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_FORCE, NULL _INLINE_TLS); ;
    break;}
case 106:
#line 402 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_ALLOW, NULL _INLINE_TLS); ;
    break;}
case 107:
#line 403 "language-parser.y"
{ get_function_parameter(&yyvsp[-2], BYREF_NONE, &yyvsp[0] _INLINE_TLS); ;
    break;}
case 108:
#line 404 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_NONE, NULL _INLINE_TLS); ;
    break;}
case 109:
#line 405 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_FORCE, NULL  _INLINE_TLS); ;
    break;}
case 110:
#line 406 "language-parser.y"
{ get_function_parameter(&yyvsp[0], BYREF_ALLOW, NULL _INLINE_TLS); ;
    break;}
case 111:
#line 407 "language-parser.y"
{ get_function_parameter(&yyvsp[-2], BYREF_NONE, &yyvsp[0] _INLINE_TLS); ;
    break;}
case 114:
#line 418 "language-parser.y"
{ pass_parameter_by_value(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 115:
#line 419 "language-parser.y"
{ pass_parameter(&yyvsp[0],0 _INLINE_TLS); ;
    break;}
case 116:
#line 420 "language-parser.y"
{ pass_parameter(&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 117:
#line 421 "language-parser.y"
{ pass_parameter_by_value(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 118:
#line 422 "language-parser.y"
{ pass_parameter(&yyvsp[0],0 _INLINE_TLS); ;
    break;}
case 119:
#line 423 "language-parser.y"
{ pass_parameter(&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 120:
#line 427 "language-parser.y"
{ cs_global_variable(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 121:
#line 428 "language-parser.y"
{ cs_global_variable(&yyvsp[0] _INLINE_TLS); ;
    break;}
case 122:
#line 433 "language-parser.y"
{ cs_static_variable(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 123:
#line 434 "language-parser.y"
{ cs_static_variable(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 124:
#line 435 "language-parser.y"
{ cs_static_variable(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 125:
#line 436 "language-parser.y"
{ cs_static_variable(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 126:
#line 442 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 127:
#line 443 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 131:
#line 455 "language-parser.y"
{ start_function_decleration(_INLINE_TLS_VOID); ;
    break;}
case 132:
#line 456 "language-parser.y"
{ end_function_decleration(&yyvsp[-7],&yyvsp[-6] _INLINE_TLS); ;
    break;}
case 133:
#line 457 "language-parser.y"
{ start_function_decleration(_INLINE_TLS_VOID); ;
    break;}
case 134:
#line 458 "language-parser.y"
{ end_function_decleration(&yyvsp[-8],&yyvsp[-7] _INLINE_TLS); ;
    break;}
case 135:
#line 464 "language-parser.y"
{ declare_class_variable(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 136:
#line 465 "language-parser.y"
{ declare_class_variable(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 137:
#line 466 "language-parser.y"
{ declare_class_variable(&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 138:
#line 467 "language-parser.y"
{ declare_class_variable(&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 140:
#line 473 "language-parser.y"
{ if (GLOBAL(Execute)) { php3i_print_variable(&yyvsp[0] _INLINE_TLS); pval_destructor(&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 141:
#line 474 "language-parser.y"
{ if (GLOBAL(Execute)) { php3i_print_variable(&yyvsp[0] _INLINE_TLS); pval_destructor(&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 142:
#line 479 "language-parser.y"
{ yyval.value.lval=1;  yyval.type=IS_LONG; ;
    break;}
case 143:
#line 480 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; pval_destructor(&yyvsp[-2] _INLINE_TLS); } ;
    break;}
case 144:
#line 481 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 145:
#line 486 "language-parser.y"
{ assign_to_list(&yyval, &yyvsp[-3], &yyvsp[0] _INLINE_TLS);;
    break;}
case 146:
#line 487 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],NULL _INLINE_TLS); ;
    break;}
case 147:
#line 488 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],add_function _INLINE_TLS); ;
    break;}
case 148:
#line 489 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],sub_function _INLINE_TLS); ;
    break;}
case 149:
#line 490 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],mul_function _INLINE_TLS); ;
    break;}
case 150:
#line 491 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],div_function _INLINE_TLS); ;
    break;}
case 151:
#line 492 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],concat_function_with_free _INLINE_TLS); ;
    break;}
case 152:
#line 493 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],mod_function _INLINE_TLS); ;
    break;}
case 153:
#line 494 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],bitwise_and_function _INLINE_TLS); ;
    break;}
case 154:
#line 495 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],bitwise_or_function _INLINE_TLS); ;
    break;}
case 155:
#line 496 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],bitwise_xor_function _INLINE_TLS); ;
    break;}
case 156:
#line 497 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],shift_left_function _INLINE_TLS); ;
    break;}
case 157:
#line 498 "language-parser.y"
{ if (GLOBAL(Execute)) assign_to_variable(&yyval,&yyvsp[-2],&yyvsp[0],shift_right_function _INLINE_TLS); ;
    break;}
case 158:
#line 499 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[-1],increment_function,1 _INLINE_TLS); ;
    break;}
case 159:
#line 500 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[0],increment_function,0 _INLINE_TLS); ;
    break;}
case 160:
#line 501 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[-1],decrement_function,1 _INLINE_TLS); ;
    break;}
case 161:
#line 502 "language-parser.y"
{ if (GLOBAL(Execute)) incdec_variable(&yyval,&yyvsp[0],decrement_function,0 _INLINE_TLS); ;
    break;}
case 162:
#line 503 "language-parser.y"
{ cs_pre_boolean_or(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 163:
#line 503 "language-parser.y"
{ cs_post_boolean_or(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 164:
#line 504 "language-parser.y"
{ cs_pre_boolean_and(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 165:
#line 504 "language-parser.y"
{ cs_post_boolean_and(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 166:
#line 505 "language-parser.y"
{ cs_pre_boolean_or(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 167:
#line 505 "language-parser.y"
{ cs_post_boolean_or(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 168:
#line 506 "language-parser.y"
{ cs_pre_boolean_and(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 169:
#line 506 "language-parser.y"
{ cs_post_boolean_and(&yyval,&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 170:
#line 507 "language-parser.y"
{ if (GLOBAL(Execute)) { boolean_xor_function(&yyval,&yyvsp[-2],&yyvsp[0]); } ;
    break;}
case 171:
#line 508 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_or_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 172:
#line 509 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_xor_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 173:
#line 510 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_and_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 174:
#line 511 "language-parser.y"
{ if (GLOBAL(Execute)) concat_function(&yyval,&yyvsp[-2],&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 175:
#line 512 "language-parser.y"
{ if (GLOBAL(Execute)) add_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 176:
#line 513 "language-parser.y"
{ if (GLOBAL(Execute)) sub_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 177:
#line 514 "language-parser.y"
{ if (GLOBAL(Execute)) mul_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 178:
#line 515 "language-parser.y"
{ if (GLOBAL(Execute)) div_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 179:
#line 516 "language-parser.y"
{ if (GLOBAL(Execute)) mod_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 180:
#line 517 "language-parser.y"
{ if (GLOBAL(Execute)) shift_left_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 181:
#line 518 "language-parser.y"
{ if (GLOBAL(Execute)) shift_right_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 182:
#line 519 "language-parser.y"
{ if (GLOBAL(Execute)) { pval tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  add_function(&yyval,&tmp,&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 183:
#line 520 "language-parser.y"
{ if (GLOBAL(Execute)) { pval tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  sub_function(&yyval,&tmp,&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 184:
#line 521 "language-parser.y"
{ if (GLOBAL(Execute)) boolean_not_function(&yyval,&yyvsp[0]); ;
    break;}
case 185:
#line 522 "language-parser.y"
{ if (GLOBAL(Execute)) bitwise_not_function(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 186:
#line 523 "language-parser.y"
{ if (GLOBAL(Execute)) is_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 187:
#line 524 "language-parser.y"
{ if (GLOBAL(Execute)) is_not_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 188:
#line 525 "language-parser.y"
{ if (GLOBAL(Execute)) is_smaller_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 189:
#line 526 "language-parser.y"
{ if (GLOBAL(Execute)) is_smaller_or_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 190:
#line 527 "language-parser.y"
{ if (GLOBAL(Execute)) is_greater_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 191:
#line 528 "language-parser.y"
{ if (GLOBAL(Execute)) is_greater_or_equal_function(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 192:
#line 529 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 193:
#line 530 "language-parser.y"
{  php3_error(E_PARSE,"'(' unmatched",GLOBAL(current_lineno)); if (GLOBAL(Execute)) yyval = yyvsp[-1]; yyerrok; ;
    break;}
case 194:
#line 531 "language-parser.y"
{ cs_questionmark_op_pre_expr1(&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 195:
#line 532 "language-parser.y"
{ cs_questionmark_op_pre_expr2(&yyvsp[-4] _INLINE_TLS); ;
    break;}
case 196:
#line 533 "language-parser.y"
{ cs_questionmark_op_post_expr2(&yyval,&yyvsp[-6],&yyvsp[-3],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 197:
#line 534 "language-parser.y"
{ cs_functioncall_pre_variable_passing(&yyvsp[0],NULL,1 _INLINE_TLS); ;
    break;}
case 198:
#line 535 "language-parser.y"
{ cs_functioncall_post_variable_passing(&yyvsp[-4],&yychar _INLINE_TLS); ;
    break;}
case 199:
#line 536 "language-parser.y"
{ cs_functioncall_end(&yyval,&yyvsp[-6],&yyvsp[-2],&yychar,1 _INLINE_TLS);;
    break;}
case 201:
#line 538 "language-parser.y"
{ cs_functioncall_pre_variable_passing(&yyvsp[0],&yyvsp[-2],1 _INLINE_TLS); ;
    break;}
case 202:
#line 539 "language-parser.y"
{ cs_functioncall_post_variable_passing(&yyvsp[-4],&yychar _INLINE_TLS); ;
    break;}
case 203:
#line 540 "language-parser.y"
{ cs_functioncall_end(&yyval,&yyvsp[-6],&yyvsp[-2],&yychar,1 _INLINE_TLS); ;
    break;}
case 204:
#line 541 "language-parser.y"
{ assign_new_object(&yyval,&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 205:
#line 542 "language-parser.y"
{ assign_new_object(&yyval,&yyvsp[0],0 _INLINE_TLS); ;
    break;}
case 206:
#line 543 "language-parser.y"
{ if (!GLOBAL(shutdown_requested)) { pval object_pointer; object_pointer.value.varptr.pvalue = &yyvsp[-1]; cs_functioncall_pre_variable_passing(&yyvsp[-2], &object_pointer, 1 _INLINE_TLS); } ;
    break;}
case 207:
#line 544 "language-parser.y"
{ cs_functioncall_post_variable_passing(&yyvsp[-5], &yychar _INLINE_TLS); ;
    break;}
case 208:
#line 545 "language-parser.y"
{ cs_functioncall_end(&yyval, &yyvsp[-7], &yyvsp[-2], &yychar, 1 _INLINE_TLS);  yyval = yyvsp[-6]; ;
    break;}
case 209:
#line 546 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_long(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 210:
#line 547 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_double(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 211:
#line 548 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_string(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 212:
#line 549 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_array(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 213:
#line 550 "language-parser.y"
{ if (GLOBAL(Execute)) { convert_to_object(&yyvsp[0]); yyval = yyvsp[0]; } ;
    break;}
case 214:
#line 551 "language-parser.y"
{ if (GLOBAL(Execute)) { php3_header(); GLOBAL(shutdown_requested)=ABNORMAL_SHUTDOWN; yyval.type=IS_LONG; yyval.value.lval=1; } ;
    break;}
case 215:
#line 552 "language-parser.y"
{ if (GLOBAL(Execute)) { php3_header(); GLOBAL(shutdown_requested)=ABNORMAL_SHUTDOWN; yyval.type=IS_LONG; yyval.value.lval=1; } ;
    break;}
case 216:
#line 553 "language-parser.y"
{ if (GLOBAL(Execute)) { if (php3_header()) { convert_to_string(&yyvsp[-1]);  PUTS(yyvsp[-1].value.str.val); convert_to_long(&yyvsp[-1]); wanted_exit_status = yyvsp[-1].value.lval;  pval_destructor(&yyvsp[-1] _INLINE_TLS); } GLOBAL(shutdown_requested)=ABNORMAL_SHUTDOWN; yyval.type=IS_LONG; yyval.value.lval=1; } ;
    break;}
case 217:
#line 554 "language-parser.y"
{ yyvsp[0].cs_data.error_reporting=GLOBAL(error_reporting); GLOBAL(error_reporting)=0; ;
    break;}
case 218:
#line 554 "language-parser.y"
{ GLOBAL(error_reporting)=yyvsp[-2].cs_data.error_reporting; yyval = yyvsp[0]; ;
    break;}
case 219:
#line 555 "language-parser.y"
{ php3_error(E_ERROR,"@ operator may only be used on expressions"); ;
    break;}
case 220:
#line 556 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 221:
#line 557 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 222:
#line 558 "language-parser.y"
{ cs_system(&yyval,&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 223:
#line 559 "language-parser.y"
{ if (GLOBAL(Execute)) { php3i_print_variable(&yyvsp[0] _INLINE_TLS);  pval_destructor(&yyvsp[0] _INLINE_TLS);  yyval.value.lval=1; yyval.type=IS_LONG; } ;
    break;}
case 224:
#line 563 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 225:
#line 564 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 226:
#line 565 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 227:
#line 566 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 228:
#line 567 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; COPY_STRING(yyval); } ;
    break;}
case 229:
#line 568 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; COPY_STRING(yyval); php3_error(E_NOTICE,"'%s' is not a valid constant - assumed to be \"%s\"",yyvsp[0].value.str.val,yyvsp[0].value.str.val); } ;
    break;}
case 230:
#line 569 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; } ;
    break;}
case 231:
#line 570 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval = yyvsp[0]; COPY_STRING(yyval); } ;
    break;}
case 232:
#line 574 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 233:
#line 575 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 234:
#line 576 "language-parser.y"
{ if (GLOBAL(Execute)) { pval tmp;  tmp.value.lval=0;  tmp.type=IS_LONG;  sub_function(&yyval,&tmp,&yyvsp[0] _INLINE_TLS); } ;
    break;}
case 235:
#line 580 "language-parser.y"
{ if (GLOBAL(Execute)){ yyval = yyvsp[0]; };
    break;}
case 236:
#line 586 "language-parser.y"
{ if (GLOBAL(Execute)) read_pointer_value(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 237:
#line 587 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 238:
#line 592 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[0]; ;
    break;}
case 239:
#line 593 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 240:
#line 598 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval = yyvsp[0];COPY_STRING(yyval); };
    break;}
case 241:
#line 599 "language-parser.y"
{ if (GLOBAL(Execute)) yyval = yyvsp[-1]; ;
    break;}
case 242:
#line 605 "language-parser.y"
{ get_object_symtable(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 243:
#line 606 "language-parser.y"
{ if (GLOBAL(Execute)) { if (yyvsp[0].value.varptr.pvalue && ((pval *)yyvsp[0].value.varptr.pvalue)->type == IS_OBJECT) { yyval=yyvsp[0]; } else { yyval.value.varptr.pvalue=NULL; } } ;
    break;}
case 244:
#line 607 "language-parser.y"
{ get_object_symtable(&yyval,NULL,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 245:
#line 611 "language-parser.y"
{

	if (GLOBAL(Execute)) {
		yyval=yyvsp[-2];
		_php3_hash_next_index_insert(yyval.value.ht,&yyvsp[0],sizeof(pval),NULL);
	}
;
    break;}
case 246:
#line 618 "language-parser.y"
{

	if (GLOBAL(Execute)) {
		yyval.value.ht = (HashTable *) emalloc(sizeof(HashTable));
		_php3_hash_init(yyval.value.ht,0,NULL,NULL,0);
		yyval.type = IS_ARRAY;
		_php3_hash_next_index_insert(yyval.value.ht,&yyvsp[0],sizeof(pval),NULL);
	}
;
    break;}
case 247:
#line 627 "language-parser.y"
{
	if (GLOBAL(Execute)) {
		yyval=yyvsp[-1];
		yyvsp[0].value.varptr.pvalue = NULL; /* $2 is just used as temporary space */
		_php3_hash_next_index_insert(yyval.value.ht,&yyvsp[0],sizeof(pval),NULL);
	}
;
    break;}
case 248:
#line 634 "language-parser.y"
{
	if (GLOBAL(Execute)) {
		pval tmp;

		yyval.value.ht = (HashTable *) emalloc(sizeof(HashTable));
		_php3_hash_init(yyval.value.ht,0,NULL,NULL,0);
		yyval.type = IS_ARRAY;
		tmp.value.varptr.pvalue = NULL;
		_php3_hash_next_index_insert(yyval.value.ht,&tmp,sizeof(pval),NULL);
	}
;
    break;}
case 249:
#line 649 "language-parser.y"
{ get_regular_variable_pointer(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 250:
#line 650 "language-parser.y"
{ if (GLOBAL(Execute)) yyval=yyvsp[0]; ;
    break;}
case 251:
#line 651 "language-parser.y"
{ get_class_variable_pointer(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 252:
#line 652 "language-parser.y"
{ start_array_parsing(&yyvsp[-1],&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 253:
#line 652 "language-parser.y"
{ end_array_parsing(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 254:
#line 657 "language-parser.y"
{ if (GLOBAL(Execute)) get_regular_variable_contents(&yyval,&yyvsp[0],1 _INLINE_TLS); ;
    break;}
case 255:
#line 658 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval = yyvsp[0];COPY_STRING(yyval);} ;
    break;}
case 256:
#line 659 "language-parser.y"
{  if (GLOBAL(Execute)) read_pointer_value(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 257:
#line 663 "language-parser.y"
{ start_array_parsing(&yyvsp[-1],NULL _INLINE_TLS); ;
    break;}
case 258:
#line 663 "language-parser.y"
{ end_array_parsing(&yyval,&yyvsp[0] _INLINE_TLS); ;
    break;}
case 259:
#line 668 "language-parser.y"
{ fetch_array_index(&yyval,&yyvsp[-1],&yyvsp[-3] _INLINE_TLS); ;
    break;}
case 260:
#line 669 "language-parser.y"
{ fetch_array_index(&yyval,NULL,&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 261:
#line 670 "language-parser.y"
{ start_dimensions_parsing(&yyval _INLINE_TLS); ;
    break;}
case 262:
#line 670 "language-parser.y"
{ fetch_array_index(&yyval,NULL,&yyval _INLINE_TLS); ;
    break;}
case 263:
#line 671 "language-parser.y"
{ start_dimensions_parsing(&yyval _INLINE_TLS); ;
    break;}
case 264:
#line 671 "language-parser.y"
{ fetch_array_index(&yyval,&yyvsp[-1],&yyval _INLINE_TLS); ;
    break;}
case 265:
#line 676 "language-parser.y"
{ if (GLOBAL(Execute)) { yyval.value.ht = (HashTable *) emalloc(sizeof(HashTable));  _php3_hash_init(yyval.value.ht,0,NULL,PVAL_DESTRUCTOR,0); yyval.type = IS_ARRAY; } ;
    break;}
case 266:
#line 677 "language-parser.y"
{ yyval = yyvsp[0]; ;
    break;}
case 267:
#line 681 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval=yyvsp[-4]; add_array_pair_list(&yyval, &yyvsp[-2], &yyvsp[0], 0 _INLINE_TLS);} ;
    break;}
case 268:
#line 682 "language-parser.y"
{ if (GLOBAL(Execute)) {yyval=yyvsp[-2]; add_array_pair_list(&yyval, NULL, &yyvsp[0], 0 _INLINE_TLS);} ;
    break;}
case 269:
#line 683 "language-parser.y"
{ if (GLOBAL(Execute)) add_array_pair_list(&yyval, &yyvsp[-2], &yyvsp[0], 1 _INLINE_TLS); ;
    break;}
case 270:
#line 684 "language-parser.y"
{ if (GLOBAL(Execute)) add_array_pair_list(&yyval, NULL, &yyvsp[0], 1 _INLINE_TLS); ;
    break;}
case 271:
#line 689 "language-parser.y"
{ add_regular_encapsed_variable(&yyval,&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 272:
#line 690 "language-parser.y"
{ add_assoc_array_encapsed_variable(&yyval,&yyvsp[-5],&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 273:
#line 691 "language-parser.y"
{ add_regular_array_encapsed_variable(&yyval,&yyvsp[-5],&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 274:
#line 692 "language-parser.y"
{ add_variable_array_encapsed_variable(&yyval,&yyvsp[-6],&yyvsp[-4],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 275:
#line 693 "language-parser.y"
{ add_encapsed_object_property(&yyval,&yyvsp[-4],&yyvsp[-2],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 276:
#line 694 "language-parser.y"
{ add_regular_encapsed_variable(&yyval,&yyvsp[-4],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 277:
#line 695 "language-parser.y"
{ add_assoc_array_encapsed_variable(&yyval,&yyvsp[-7],&yyvsp[-4],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 278:
#line 696 "language-parser.y"
{ add_regular_array_encapsed_variable(&yyval,&yyvsp[-7],&yyvsp[-4],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 279:
#line 697 "language-parser.y"
{ add_variable_array_encapsed_variable(&yyval,&yyvsp[-8],&yyvsp[-5],&yyvsp[-2] _INLINE_TLS); ;
    break;}
case 280:
#line 698 "language-parser.y"
{ add_encapsed_object_property(&yyval,&yyvsp[-6],&yyvsp[-3],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 281:
#line 699 "language-parser.y"
{ add_indirect_encapsed_variable(&yyval,&yyvsp[-5],&yyvsp[-1] _INLINE_TLS); ;
    break;}
case 282:
#line 700 "language-parser.y"
{ if (GLOBAL(Execute)) { concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 283:
#line 701 "language-parser.y"
{ if (GLOBAL(Execute)) { concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 284:
#line 702 "language-parser.y"
{ if (GLOBAL(Execute)) { concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 285:
#line 703 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 286:
#line 704 "language-parser.y"
{ if (GLOBAL(Execute)) { php3_error(E_NOTICE,"Bad escape sequence:  %s",yyvsp[0].value.str.val); concat_function(&yyval,&yyvsp[-1],&yyvsp[0],0 _INLINE_TLS); } ;
    break;}
case 287:
#line 705 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 288:
#line 706 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 289:
#line 707 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 290:
#line 708 "language-parser.y"
{ if (GLOBAL(Execute)) add_char_to_string(&yyval,&yyvsp[-1],&yyvsp[0] _INLINE_TLS); ;
    break;}
case 291:
#line 709 "language-parser.y"
{  if (GLOBAL(Execute)) { pval tmp;  tmp.value.str.val="->"; tmp.value.str.len=2; tmp.type=IS_STRING; concat_function(&yyval,&yyvsp[-1],&tmp,0 _INLINE_TLS); } ;
    break;}
case 292:
#line 710 "language-parser.y"
{ if (GLOBAL(Execute)) var_reset(&yyval); ;
    break;}
case 293:
#line 715 "language-parser.y"
{ php3_unset(&yyval, &yyvsp[-1]); ;
    break;}
case 294:
#line 716 "language-parser.y"
{ php3_isset(&yyval, &yyvsp[-1]); ;
    break;}
case 295:
#line 717 "language-parser.y"
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
#line 727 "language-parser.y"


inline void clear_lookahead(int *yychar)
{
	if (yychar) {
		*yychar=YYEMPTY;
	}
}


/*** Manhattan project ***/
/* Be able to call user-levle functions from C */
/* "A beer and serious lack of sleep do wonders" -- Zeev */
PHPAPI int call_user_function(HashTable *function_table, pval *object, pval *function_name, pval *retval, int param_count, pval *params[])
{
	pval *func;
	pval return_offset;
	int i;
	pval class_ptr;
	int original_shutdown_requested=GLOBAL(shutdown_requested);
	int original_execute_flag = GLOBAL(ExecuteFlag);
	FunctionState original_function_state = GLOBAL(function_state);

	/* save the location to go back to */
	return_offset.offset = tc_get_current_offset(&GLOBAL(token_cache_manager))-1;	
		
	if (object) {
		function_table = object->value.ht;
	}
	php3_str_tolower(function_name->value.str.val, function_name->value.str.len);
	if (_php3_hash_find(function_table, function_name->value.str.val, function_name->value.str.len+1, (void **) &func)==FAILURE
		|| func->type != IS_USER_FUNCTION) {
		return FAILURE;
	}
	
	/* Do some magic... */
	GLOBAL(shutdown_requested) = 0;
	GLOBAL(function_state).loop_nest_level = GLOBAL(function_state).loop_change_level = GLOBAL(function_state).loop_change_type = 0;
	GLOBAL(function_state).returned = 0;
	GLOBAL(ExecuteFlag) = EXECUTE;
	GLOBAL(Execute) = SHOULD_EXECUTE;

	tc_set_token(&token_cache_manager, func->offset, IC_FUNCTION);
	if (object) {
		class_ptr.value.varptr.pvalue = object;
		cs_functioncall_pre_variable_passing(function_name, &class_ptr, 0 _INLINE_TLS);
	} else {
		cs_functioncall_pre_variable_passing(function_name,NULL, 0 _INLINE_TLS);
	}
	for (i=0; i<param_count; i++) {
		_php3_hash_next_index_pointer_insert(GLOBAL(function_state).function_symbol_table, params[i]);
	}
	cs_functioncall_post_variable_passing(function_name, NULL);
	phpparse();
	if (GLOBAL(shutdown_requested)) { /* we died during this function call */
		return FAILURE;
	}
	cs_functioncall_end(retval,function_name,&return_offset,NULL,0);
	GLOBAL(function_state) = original_function_state;
	GLOBAL(ExecuteFlag) = original_execute_flag;
	GLOBAL(shutdown_requested) = original_shutdown_requested;
	GLOBAL(Execute) = SHOULD_EXECUTE;

	return SUCCESS;
}
