/***[oracle.h]****************************************************[TAB=4]****\
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

/****************************************************************************\
*                                                                            *
* Oracle functions for PHP.                                                  *
*                                                                            *
* © Copyright (C) Guardian Networks AS 1997, Dmitry Povarov 1997             *
*                                                                            *
* Authors: Stig Sæther Bakken <ssb@guardian.no>                              *
*          Mitch Golden <mitch@spiralmedia.com>                              *
*          Dmitry "Dizzy" Povarov <dizzy@glas.net>                           *
*                                                                            *
\****************************************************************************/

/* $Id */

#if HAVE_LIBOCIC

/* The maximum number of Oracle connections in a single document */
#define MAX_CONNECTIONS 15

/* The maximum number of open Oracle cursors */
#define MAX_CURSORS 50

/* oparse flags */
#define  DEFER_PARSE        1
#define  NATIVE             1
#define  VERSION_7          2

/* Note that we have to be careful in the case of someone logging in 
   using SQLnet.  In that case, they can use things like
   username: scott@machine
   password: tiger
   or even
   username: scott/tiger@machine
   password: (nothing) */

#define ORAUIDLEN 100
#define ORAPWLEN  100

/* Maximum length of the ORACLE_SID */
#define ORASIDLEN 32

/* Longest allowed column name length */
#define ORANAMELEN 32

/* This is the maximum size of a varchar2.  When and if we add support for
   longer types, we'll have to increase this */
#define ORABUFLEN 2000

/* size to allocate for an oracle number */
#define ORANUMWIDTH 38

/* This should have been in ocidem.h! */
#define STRING_TRUNCATED		1406

/* ----------------------------------------------------------------------*/
/* This stuff is cloned straight out of ocidem.h.  Keep it up to date,
   please! */

/*
** Size of HDA area:
** 512 for 64 bit arquitectures
** 256 for 32 bit arquitectures
*/

#if (defined(__osf__) && defined(__alpha)) || defined(CRAY) || defined(KSR)
# define HDA_SIZE 512
#else
# define HDA_SIZE 256
#endif

/* Some Oracle error codes */
#define VAR_NOT_IN_LIST			1007
#define NO_DATA_FOUND			1403
#define NULL_VALUE_RETURNED		1405

/* Some Oracle types */
#define VARCHAR2_TYPE		1
#define NUMBER_TYPE			2
#define INT_TYPE			3
#define FLOAT_TYPE			4
#define STRING_TYPE			5
#define ROWID_TYPE			11
#define DATE_TYPE			12

/* Some SQL and OCI function codes */
#define FT_INSERT			3
#define FT_SELECT			4
#define FT_UPDATE			5
#define FT_DELETE			9

#define FC_OOPEN			14

static const text * ora_func_tab[] =  {(text *) "unused",
/*  1, 2  */       (text *) "unused", (text *) "OSQL",
/*  3, 4  */       (text *) "unused", (text *) "OEXEC/OEXN",
/*  5, 6  */       (text *) "unused", (text *) "OBIND",
/*  7, 8  */       (text *) "unused", (text *) "ODEFIN",
/*  9, 10 */       (text *) "unused", (text *) "ODSRBN",
/* 11, 12 */       (text *) "unused", (text *) "OFETCH/OFEN",
/* 13, 14 */       (text *) "unused", (text *) "OOPEN",
/* 15, 16 */       (text *) "unused", (text *) "OCLOSE",
/* 17, 18 */       (text *) "unused", (text *) "unused",
/* 19, 20 */       (text *) "unused", (text *) "unused",
/* 21, 22 */       (text *) "unused", (text *) "ODSC",
/* 23, 24 */       (text *) "unused", (text *) "ONAME",
/* 25, 26 */       (text *) "unused", (text *) "OSQL3",
/* 27, 28 */       (text *) "unused", (text *) "OBNDRV",
/* 29, 30 */       (text *) "unused", (text *) "OBNDRN",
/* 31, 32 */       (text *) "unused", (text *) "unused",
/* 33, 34 */       (text *) "unused", (text *) "OOPT",
/* 35, 36 */       (text *) "unused", (text *) "unused",
/* 37, 38 */       (text *) "unused", (text *) "unused",
/* 39, 40 */       (text *) "unused", (text *) "unused",
/* 41, 42 */       (text *) "unused", (text *) "unused",
/* 43, 44 */       (text *) "unused", (text *) "unused",
/* 45, 46 */       (text *) "unused", (text *) "unused",
/* 47, 48 */       (text *) "unused", (text *) "unused",
/* 49, 50 */       (text *) "unused", (text *) "unused",
/* 51, 52 */       (text *) "unused", (text *) "OCAN",
/* 53, 54 */       (text *) "unused", (text *) "OPARSE",
/* 55, 56 */       (text *) "unused", (text *) "OEXFET",
/* 57, 58 */       (text *) "unused", (text *) "OFLNG",
/* 59, 60 */       (text *) "unused", (text *) "ODESCR",
/* 61, 62 */       (text *) "unused", (text *) "OBNDRA"
};

/* ----------------------------------------------------------------------*/

/* Our structure definitions */

typedef struct oraConnection {
	Lda_Def lda;
	ub1 hda[HDA_SIZE];
	short ind;      /* index of connection in array, or -1 if not allocated */
	short inuse;    /* is this connection currently in use on this page? */
	short waserror; /* was the last thing done an error on this connection? */
	char userid[ORAUIDLEN+1];  /* login information */
	char password[ORAPWLEN+1];
	char sid[ORASIDLEN+1];
} oraConnection;

typedef struct oraColumn {
	sb4 dbsize;
	sb2 dbtype;
	text cbuf[ORANAMELEN+1];
	sb4 cbufl;
	sb4 dsize;
	sb2 prec;
	sb2 scale;
	sb2 nullok;
	float flt_buf;
	sword int_buf;
	ub1 *buf;
	sb2 indp;
	ub2 col_retlen, col_retcode;
	struct oraColumn *next;
} oraColumn;

typedef struct oraBindVar {     /*-== Variables Binding =--*/
        int     type;           /* Variable type           */
        int     len;            /* Allocated space length  */
        text*   sql_name;       /* Variable name in PL/SQL */
        char*   php_name;       /* Variable name in PHP    */
        void*   space;          /* Space to store variable */
        struct oraBindVar* next; /* Next list element or NULL */
} oraBindVar;

typedef struct oraCursor {
	Cda_Def cda;
	text *currentQuery;
	oraColumn *column_top;
	oraColumn *curr_column;
	oraColumn **columns;
	int ncols;
	short ind;
	short conn_ind;
	struct oraBindVar* ora_bind_top; /* Top of binding list */
} oraCursor;


static void	   ora_no_bindings(); /* Remove all bindings */
static oraBindVar* ora_new_bind(); /* Declare new binding descriptor     */
static oraBindVar* ora_get_bind(); /* Get binding descriptor             */
static int         ora_del_bind(); /* Erase binding descriptor           */
static int         ora_bind_in();  /* Copy data from PHP to bind holders */
static int         ora_bind_out(); /* Copy data from bind holders to PHP */
static oraConnection *ora_add_conn();
static oraConnection *ora_get_conn(int);
static void ora_del_conn(int);
static oraCursor *ora_add_cursor();
static oraCursor *ora_get_cursor(int);
static void ora_del_cursor(int);
static char *ora_error(Cda_Def *);
static int ora_describe_define(oraCursor *);
static void ora_closeall();

#endif

/*
 * Local Variables:
 * tab-width: 4
 * End:
 */
