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
   | Authors: Brian Schaffner <brian@tool.net>                            |
   |                                                                      |
   +----------------------------------------------------------------------+
 */
/* $Id: odbc.c,v 1.47 1998/01/19 10:56:39 ssb Exp $ */
#include "parser.h"
#include "internal_functions.h"
#include <stdio.h>
#include <stdlib.h>
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <ctype.h>

#if HAVE_IODBC && !HAVE_UODBC
#define HAVE_ODBC 1
#endif

#if HAVE_ODBC

#if MSVC5
#  include <windows.h>
#  include <sql.h>
#  include <sqlext.h>
#else
#  include <isql.h>
#  include <isqlext.h>
#endif

#if APACHE
#  ifndef DEBUG
#  undef palloc
#  endif
#endif

#define MAX_FIELD_LEN 256

#include "odbc.h"

function_entry odbc_functions[] = {
	{"sqlconnect",		ODBCconnect,		NULL},
	{"sqldisconnect",	ODBCdisconnect,		NULL},
	{"sqlfetch",		ODBCfetch,			NULL},
	{"sqlexecdirect",	ODBCexecdirect,		NULL},
	{"sqlgetdata",		ODBCgetdata,		NULL},
	{"sqlfree",			ODBCfree,			NULL},
	{"sqlrowcount",		ODBCrowcount,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry odbc_module_entry = {
	"ODBC", odbc_functions, php3_minit_odbc, php3_mshutdown_odbc, php3_rinit_odbc, NULL, NULL, 0, 0, 0, NULL
};

#if THREAD_SAFE
DWORD ODBCTls;
static int numthreads=0;
#endif

/*needed for blocking calls in windows*/
void *odbc_mutex;

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &odbc_module_entry; }

#if (WIN32|WINNT) && defined(THREAD_SAFE)

/*NOTE: You should have an odbc.def file where you
export DllMain*/
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    switch( ul_reason_for_call ) {
    case DLL_PROCESS_ATTACH:
		if((ODBCTls=TlsAlloc())==0xFFFFFFFF){
			return 0;
		}
		break;    
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break
	case DLL_PROCESS_DETACH:
		if(!TlsFree(ODBCTls)){
			return 0;
		}
		break;
    }
    return 1;
}
#endif
#endif


/* 
   Statement List 
   The statement list is a linked list
   of HSTMT's. It contains information about
   a query along with information about each column.
   No DATA is stored in the list.
 */

typedef struct odbcHstmtList {
	HDBC hdbc;					/* connection */
	HSTMT hstmt;				/* the statement */
	int index;					/* index in list */
	int gotnames;				/* did we get the field names? */
	int ncols;					/* number of fields */
#if MSVC5
	UCHAR  FAR 	**fname;		/* field name array */
  	SWORD  FAR 	*coltype;		/* field type array */
  	UDWORD FAR 	*colprecision;		/* field size array */
  	SWORD  FAR 	*colscale;		/* field scale array */
  	SWORD  FAR 	*colnullable;		/* is field nullable array */
#else
	char 	**fname;		/* field name array */
  	int 	*coltype;		/* field type array */
  	int 	*colprecision;		/* field size array */
  	int 	*colscale;		/* field scale array */
  	int 	*colnullable;		/* is field nullable array */
#endif
	struct odbcHstmtList *next;	/* next statement in list */
} odbcHstmtList;

/*
   Connection List
   The connection list is a linked list of
   HDBC's. It contains information about each
   connection.
 */

typedef struct odbcHdbcList {
	HDBC hdbc;					/* the connection */
	int index;					/* index in list */
	int connected;				/* is it connected? */
	struct odbcHdbcList *next;	/* next connection in list */
} odbcHdbcList;

/* 
   Field structure
   The odbcField struct contains information
   about a single column. No data is stored in
   the structure
 */

typedef struct odbcField {
	char name[MAX_FIELD_LEN];
#if MSVC5
	UWORD colnum;
#else
	int colnum;
#endif
	int coltype;
	int colprecision;
	int colscale;
	int colnullable;
} odbcField;


#ifdef THREAD_SAFE
typedef struct odbc_globals_struct{
odbcHstmtList *odbc_hstmt_top;	/* statement list head */
odbcHdbcList *odbc_hdbc_top;	/* connection list head */
int odbc_hstmt_ind;		/* initial statement index */
int odbc_hdbc_ind;		/* initial connection index */
HENV henv;		/* HENV variable */
} odbc_globals_struct;

#define ODBC_GLOBAL(a) odbc_globals->a
#undef GLOBAL
#define GLOBAL(a) a
#define ODBC_TLS_VARS \
	odbc_globals_struct *odbc_globals; \
	odbc_globals = TlsGetValue(ODBCTls); 
#else
static odbcHstmtList *odbc_hstmt_top = NULL;	/* statement list head */
static odbcHdbcList *odbc_hdbc_top = NULL;	/* connection list head */
static odbc_hstmt_ind = 2;		/* initial statement index */
static odbc_hdbc_ind = 1;		/* initial connection index */
static HENV henv = NULL;		/* HENV variable */
#define ODBC_GLOBAL(a) a
#define ODBC_TLS_VARS
#endif

/* odbc module initialization routine */
/* called in main.c */
int php3_minit_odbc(INITFUNCARG)
{
#ifdef THREAD_SAFE
	odbc_globals_struct *odbc_globals;
#if !COMPILE_DL
#if WIN32|WINNT
	odbc_mutex = CreateMutex (NULL, FALSE, "ODBC_TLS");
#endif
#endif
#if !COMPILE_DL
	BLOCK_INTERRUPTIONS(odbc_mutex);
	numthreads++;
	if(numthreads==1){
	if((ODBCTls=TlsAlloc())==0xFFFFFFFF){
		UNBLOCK_INTERRUPTIONS(odbc_mutex);
		return 0;
	}}
	UNBLOCK_INTERRUPTIONS(odbc_mutex);
#endif
	odbc_globals = (odbc_globals_struct *) LocalAlloc(LPTR, sizeof(odbc_globals_struct)); 
	TlsSetValue(ODBCTls, (void *) odbc_globals);
#endif
	return SUCCESS;
}

int php3_mshutdown_odbc(void)
{
#ifdef THREAD_SAFE
	odbc_globals_struct *odbc_globals;
	odbc_globals = TlsGetValue(ODBCTls); 
	if (odbc_globals != 0) 
		LocalFree((HLOCAL) odbc_globals); 
#if !COMPILE_DL
	BLOCK_INTERRUPTIONS(odbc_mutex);
	numthreads--;
	if(!numthreads){
	if(!TlsFree(ODBCTls)){
		UNBLOCK_INTERRUPTIONS(odbc_mutex);
		return 0;
	}}
	UNBLOCK_INTERRUPTIONS(odbc_mutex);
#endif
#endif
	return SUCCESS;
}

int php3_rinit_odbc(INITFUNCARG)
{
	ODBC_TLS_VARS;

	/* simply initialize some variables */
	ODBC_GLOBAL(odbc_hstmt_top) = NULL;
	ODBC_GLOBAL(odbc_hdbc_top) = NULL;
	ODBC_GLOBAL(odbc_hstmt_ind) = 2;
	ODBC_GLOBAL(odbc_hdbc_ind) = 1;

	return SUCCESS;
}

/*
   odbc_get_list()
   Finds a list item based on a statement
 */
odbcHstmtList *odbc_get_list(HSTMT hstmt)
{
	odbcHstmtList *new;
	ODBC_TLS_VARS;

	/* find head */
	new = ODBC_GLOBAL(odbc_hstmt_top);
	while (new) {
		/* find statement and return item */
		if (new->hstmt == hstmt) {
			return (new);
		}
		new = new->next;
	}
	return NULL;
}


/* odbc_error() reports errors from odbc calls */
void odbc_error(HDBC h, HSTMT hstmt, char *func)
{
	char msg[1025];
	char state[255];
	SDWORD native;
	SWORD bytes;
#if !(MSVC5)
	int status;
#endif
	int sethenv = 0;
	odbcHstmtList *list;
	HDBC hdbc = SQL_NULL_HDBC;
	ODBC_TLS_VARS;

	list = odbc_get_list(hstmt);
	if (list) {
		hdbc = list->hdbc;
	}
	while (SQLError(ODBC_GLOBAL(henv), hdbc, hstmt, state, &native, msg, 1024, &bytes) == SQL_SUCCESS) {
		if (func) {
			php3_error(E_WARNING, "%s(%s): %s\n", func, state, msg);
		} else {
			php3_error(E_WARNING, "SQLError(%s): %s\n", state, msg);
		}
	}
	while (SQLError(ODBC_GLOBAL(henv), hdbc, SQL_NULL_HSTMT, state, &native, msg, 1024, &bytes) == SQL_SUCCESS) {
		if (func) {
			php3_error(E_WARNING, "%s(%s): %s\n", func, state, msg);
		} else {
			php3_error(E_WARNING, "SQLError(%s): %s\n", state, msg);
		}
	}
	while (SQLError(ODBC_GLOBAL(henv), SQL_NULL_HDBC, SQL_NULL_HSTMT, state, &native, msg, 1024, &bytes) == SQL_SUCCESS) {
		if (func) {
			php3_error(E_WARNING, "%s(%s): %s\n", func, state, msg);
		} else {
			php3_error(E_WARNING, "SQLError(%s): %s\n", state, msg);
		}
	}

}

/* 
   odbc_add_hstmt() 
   Creates a new hstmt in the list and
   initializes it.  
 */
int odbc_add_hstmt(HDBC hdbc, HSTMT hstmt)
{
	/* result index 1 is reserved to indicate successful
	 * non-tuple returning commands
	 */

	odbcHstmtList *new;
	ODBC_TLS_VARS;

	/* get list head */
	new = ODBC_GLOBAL(odbc_hstmt_top);

	/* if no head, allocate one */
	if (!new) {
		new = emalloc(sizeof(odbcHstmtList));
		if (!new) {
			php3_error(E_WARNING, "Out of memory");
		}
		ODBC_GLOBAL(odbc_hstmt_top) = new;
	} else {
		/* otherwise, go to end of list and allocate */
		while (new->next) {
			new = new->next;
		}
		new->next = emalloc(sizeof(odbcHstmtList));
		if (!(new->next)) {
			php3_error(E_WARNING, "Out of memory");
		} else {
			new = new->next;
		}
	}

	/* initialize statement pointer */
	new->hdbc = hdbc;
	new->hstmt = hstmt;
	new->gotnames = 0;
	new->fname = NULL;
	new->coltype = NULL;
	new->colprecision = NULL;
	new->colscale = NULL;
	new->colnullable = NULL;
	new->ncols = 0;
	new->index = ODBC_GLOBAL(odbc_hstmt_ind)++;
	new->next = NULL;

	/* return index */

	return (ODBC_GLOBAL(odbc_hstmt_ind) - 1);
}

/* 
   odbc_get_hstmt()
   Finds a statement using integer
   index.
 */

HSTMT odbc_get_hstmt(int count)
{
	odbcHstmtList *new;
	ODBC_TLS_VARS;

	/* 1st index is reserved for no tuples */
	if (count == 1) {
		return NULL;
	}
	/* find head */
	new = ODBC_GLOBAL(odbc_hstmt_top);
	while (new) {
		/* find index and return item */
		if (new->index == count) {
			return (new->hstmt);
		}
		new = new->next;
	}
	/* not found - return null */
	return (NULL);
}


/* 
   odbc_del_hstmt()
   Removes a statement from list
   and clears resources 
 */

void odbc_del_hstmt(int count)
{
	odbcHstmtList *new, *prev, *next;
	int status;
	ODBC_TLS_VARS;

	if (count == 1) {
		return;
	}
	prev = NULL;
	/* find head */
	new = ODBC_GLOBAL(odbc_hstmt_top);
	while (new) {
		next = new->next;
		/* find item */
		if (new->index == count) {
			/* free the statement */
			status = SQLFreeStmt(new->hstmt, SQL_DROP);
			/* check status */
			if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
				odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLFreeStmt");
				return;
			}
			if (prev) {
				prev->next = next;
			} else {
				ODBC_GLOBAL(odbc_hstmt_top) = next;
			}
			break;
		}
		prev = new;
		new = next;
	}
}

/*
   odbc_add_hdbc()
   Adds a connection to the connection
   list.
 */

int odbc_add_hdbc(HDBC hdbc)
{
	odbcHdbcList *new;
	ODBC_TLS_VARS;
	/* find head */
	new = ODBC_GLOBAL(odbc_hdbc_top);

	/* if no head - alloc one */
	if (!new) {
		new = emalloc(sizeof(odbcHdbcList));
		if (!new) {
			php3_error(E_WARNING, "Out of memory");
		}
		ODBC_GLOBAL(odbc_hdbc_top) = new;
	} else {
		/* otherwise go to end and alloc */
		while (new->next)
			new = new->next;
		new->next = emalloc(sizeof(odbcHdbcList));
		if (!(new->next)) {
			php3_error(E_WARNING, "Out of memory");
		} else {
			new = new->next;
		}
	}

	/* initialize connection */

	new->hdbc = hdbc;
	new->index = ODBC_GLOBAL(odbc_hdbc_ind)++;
	new->next = NULL;
	return (ODBC_GLOBAL(odbc_hdbc_ind) - 1);
}

/*
   odbc_get_hdbc()
   Gets a connection based on the index
 */

HDBC odbc_get_hdbc(int count)
{
	odbcHdbcList *new;
	ODBC_TLS_VARS;
	/* find head */
	new = ODBC_GLOBAL(odbc_hdbc_top);

	while (new) {
		/* find connection and return */
		if (new->index == count) {
			return (new->hdbc);
		}
		new = new->next;
	}
	return (NULL);
}

/*
   odbc_del_hdbc()
   Remove a connection and free its
   resources
 */

void odbc_del_hdbc(HDBC hdbc)
{
	odbcHdbcList *new, *prev, *next;
	int status;
	ODBC_TLS_VARS;

	prev = NULL;

	/* find head */
	new = ODBC_GLOBAL(odbc_hdbc_top);
	while (new) {
		next = new->next;
		/* find connection */
		if (new->hdbc == hdbc) {
			/* if we're connected - disconnect */
			if (new->connected) {
				status = SQLDisconnect(new->hdbc);
				if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
					odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLDisconnect");
					return;
				}
				new->connected = 0;
			}
			/* clear resources */
			status = SQLFreeConnect(new->hdbc);
			if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
				odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLFreeConnect");
				return;
			}
			if (prev) {
				prev->next = next;
			} else {
				ODBC_GLOBAL(odbc_hdbc_top) = next;
			}
			break;
		}
		prev = new;
		new = next;
	}
}

/*
   ODBCcloseAll()
   Closes all connections and frees all 
   resources on exit
 */

void ODBCcloseAll(void)
{
	odbcHstmtList *lnew, *lnext;
	odbcHdbcList *cnew, *cnext;
	int status;
	ODBC_TLS_VARS;

	/* find head */
	lnew = ODBC_GLOBAL(odbc_hstmt_top);
	while (lnew) {
		lnext = lnew->next;
		/* if the statement is good, free it */
		if (lnew->hstmt) {
#if MSVC5
   			status = SQLFreeStmt(lnew->hstmt,SQL_DROP);
#else
   			status = SQLFreeStmt(lnew->hstmt);
#endif
			if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
				odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLFreeStmt");
				return;
			}
		}
		lnew = lnext;
	}
	ODBC_GLOBAL(odbc_hstmt_top) = NULL;

	/* find head */
	cnew = ODBC_GLOBAL(odbc_hdbc_top);
	while (cnew) {
		cnext = cnew->next;
		/* make sure we have a connection */
		if (cnew->hdbc) {
			/* if connected, disconnect */
			if (cnew->connected) {
				status = SQLDisconnect(cnew->hdbc);
				if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
					odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLDisconnect");
					return;
				}
				cnew->connected = 0;
			}
			/* free resources */
			status = SQLFreeConnect(cnew->hdbc);
			if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
				odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLFreeConnect");
				return;
			}
		}
		cnew = cnext;
	}
	ODBC_GLOBAL(odbc_hdbc_top) = NULL;
}

/*
   odbc_getFieldByName()
   Finds a field using a case-insensitive
   string comparison.   
 */

odbcField *odbc_getFieldByName(HSTMT hstmt, char *field)
{
	int gotnames = 0;
#if MSVC5
	UWORD		i = 0;
	SWORD FAR	n;
#else
	int 		i = 0;
	int 		n;
#endif
	odbcHstmtList *list;
	int fieldnum = -1;
	odbcField *odbc_field = NULL;
	int status;

	/* get list */
	list = odbc_get_list(hstmt);

	if (!list) {
		return NULL;
	}
	/* did we already get field names? */
	switch (list->gotnames) {
		case 0:
			/* get the number of fields */
			status = SQLNumResultCols(hstmt, (SWORD *) & (list->ncols));
			if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
				odbc_error(SQL_NULL_HDBC, hstmt, "SQLNumResultCols");
				return NULL;
			}
			/* alloc space for fields */
			if (list->ncols > 0) {
#if MSVC5
				list->fname = (UCHAR FAR  **) emalloc(sizeof(char) * MAX_FIELD_LEN * list->ncols);
          		list->coltype = (SWORD FAR  *) emalloc(sizeof(int) * list->ncols);
          		list->colprecision = (UDWORD FAR *) emalloc(sizeof(int) * list->ncols);
          		list->colscale = (SWORD FAR  *) emalloc(sizeof(int) * list->ncols);
          		list->colnullable = (SWORD FAR  *) emalloc(sizeof(int) * list->ncols);          
#else
				list->fname = (char **) emalloc(0, sizeof(char) * MAX_FIELD_LEN * list->ncols);
          		list->coltype = (int *) emalloc(0, sizeof(int) * list->ncols);
          		list->colprecision = (int *) emalloc(0, sizeof(int) * list->ncols);
          		list->colscale = (int *) emalloc(0, sizeof(int) * list->ncols);
          		list->colnullable = (int *) emalloc(0, sizeof(int) * list->ncols);          
#endif
			}
			/* put fields into arrays */
			i = 0;
			while (i < list->ncols) {
				list->fname[i] = emalloc(255);
#if MSVC5
				status = SQLDescribeCol(hstmt, (UWORD)(i+1), list->fname[i], MAX_FIELD_LEN, &n, &(list->coltype[i]), list->colprecision, list->colscale, list->colnullable);
#else
				status = SQLDescribeCol(hstmt, i+1, list->fname[i], MAX_FIELD_LEN, &n, &(list->coltype[i]), &(list->colprecision), &(list->colscale), &(list->colnullable));
#endif
				if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
					odbc_error(SQL_NULL_HDBC, hstmt, "SQLDescribeCol");
					return NULL;
				}
				list->fname[i + 1] = NULL;
				list->coltype[i + 1] = 0;
				list->colprecision[i + 1] = 0;
				list->colscale[i + 1] = 0;
				list->colnullable[i + 1] = 0;

				/* if it's the field we're looking for, alloc
				   field and assign values
				 */
				if (!strcasecmp(list->fname[i], field)) {
					odbc_field = emalloc(sizeof(odbcField));
					if (odbc_field) {
						odbc_field->colnum = i + 1;
						strcpy(odbc_field->name, list->fname[i]);
						odbc_field->coltype = list->coltype[i];
						odbc_field->colprecision = list->colprecision[i];
						odbc_field->colscale = list->colscale[i];
						odbc_field->colnullable = list->colnullable[i];
					}
				}
				i++;
			}

			list->gotnames = 1;
			break;
		case 1:

			/* already got names... */
			i = 0;
			while (i < list->ncols) {
				/* find field and alloc and assign */
				if (!strcasecmp(list->fname[i], field)) {
					odbc_field = emalloc(sizeof(odbcField));
					if (odbc_field) {
						odbc_field->colnum = i + 1;
						strcpy(odbc_field->name, list->fname[i]);
						odbc_field->coltype = list->coltype[i];
						odbc_field->colprecision = list->colprecision[i];
						odbc_field->colscale = list->colscale[i];
						odbc_field->colnullable = list->colnullable[i];
					}
				}
				i++;
			}
			break;
	}

	return odbc_field;
}

/*
   odbc_getFieldByNumber()
   Find a field using the field index 
 */

odbcField *odbc_getFieldByNumber(HSTMT hstmt, int field)
{
	int gotnames = 0;
#if MSVC5
	UWORD   	i = 0;
	SWORD FAR   n;
#else
	int 		i = 0;
	int 		n;
#endif
	odbcHstmtList *list;
	int fieldnum = -1;
	odbcField *odbc_field = NULL;
	int status;

	/* get list */
	list = odbc_get_list(hstmt);
	if (!list) {
		return NULL;
	}
	/* got the field names already? */
	switch (list->gotnames) {
		case 0:
			/* get number of fields */
			status = SQLNumResultCols(hstmt, (SWORD *) & (list->ncols));
			if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
				odbc_error(SQL_NULL_HDBC, hstmt, "SQLNumResultCols");
				return NULL;
			}
			/* alloc space */
			if (list->ncols > 0) {
#if MSVC5
				list->fname = (UCHAR FAR **) emalloc(sizeof(char *) * (list->ncols + 1));
          		list->coltype = (SWORD FAR *) emalloc(sizeof(int) * (list->ncols + 1));
          		list->colprecision = (UDWORD FAR *) emalloc(sizeof(int) * (list->ncols + 1));
          		list->colscale = (SWORD FAR *) emalloc(sizeof(int) * (list->ncols + 1));
          		list->colnullable = (SWORD FAR *) emalloc(sizeof(int) * (list->ncols + 1));          
#else
				list->fname = (char **) emalloc(0, sizeof(char *) * (list->ncols + 1));
          		list->coltype = (int *) emalloc(0, sizeof(int) * (list->ncols + 1));
          		list->colprecision = (int *) emalloc(0, sizeof(int) * (list->ncols + 1));
          		list->colscale = (int *) emalloc(0, sizeof(int) * (list->ncols + 1));
          		list->colnullable = (int *) emalloc(0, sizeof(int) * (list->ncols + 1));          
#endif
			}
			/* loop through and assign fields */
			while (i < list->ncols) {
				status = SQLDescribeCol(hstmt, (UWORD)(i+1), list->fname[i], MAX_FIELD_LEN, &n, &(list->coltype[i]), &(list->colprecision[i]), &(list->colscale[i]), &(list->colnullable[i]));
				if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
					odbc_error(SQL_NULL_HDBC, hstmt, "SQLDescribeCol");
					return NULL;
				}
				/* if it's the field we're looking for,
				   alloc a field and assign values 
				 */

				if (i == field) {
					odbc_field = emalloc(sizeof(odbcField));
					if (odbc_field) {
						odbc_field->colnum = i + 1;
						strcpy(odbc_field->name, list->fname[i]);
						odbc_field->coltype = list->coltype[i];
						odbc_field->colprecision = list->colprecision[i];
						odbc_field->colscale = list->colscale[i];
						odbc_field->colnullable = list->colnullable[i];
					}
				}
				i++;
			}
			list->gotnames = 1;
			break;
		case 1:
			/* already got field names */
			i = 0;
			while (i < list->ncols) {
				/* find field and alloc and return */
				if (i == field) {
					odbc_field = emalloc(sizeof(odbcField));
					if (odbc_field) {
						odbc_field->colnum = i + 1;
						strcpy(odbc_field->name, list->fname[i]);
						odbc_field->coltype = list->coltype[i];
						odbc_field->colprecision = list->colprecision[i];
						odbc_field->colscale = list->colscale[i];
						odbc_field->colnullable = list->colnullable[i];
					}
				}
				i++;
			}
			break;
	}

	return odbc_field;
}

/* Main User Functions */

/* 
   ODBCfetch()
   Fetches a row from an executed statement
 */

void ODBCfetch(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *y_res_ind;
	HSTMT hstmt;
	int res_ind, status;

	/* get statement index  */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &y_res_ind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(y_res_ind);

	res_ind = y_res_ind->value.lval;

	/* statement index of 1 means no tuples */
	if (res_ind == 1) {
		RETURN_ZERO;
		return;
	}
	/* convert index to statement */
	hstmt = odbc_get_hstmt(res_ind);
	if (!hstmt) {
		php3_error(E_WARNING, "Bad statement index in odbc_fetch\n");
		RETURN_ZERO;
	}
	/* fetch a row */
	status = SQLFetch(hstmt);
	if (status == SQL_SUCCESS || status == SQL_SUCCESS_WITH_INFO) {
		RETURN_ONE;
	} else {
		if (status == SQL_ERROR || status == SQL_INVALID_HANDLE) {
			odbc_error(SQL_NULL_HDBC, hstmt, "SQLfetch");
		}
		RETURN_ZERO;
	}
}

/*
   ODBCexecdirect()
   Does an SQLExecDirect on an SQL query
 */

void ODBCexecdirect(INTERNAL_FUNCTION_PARAMETERS)
{
	int conn;
	char *query;
	int j;
#ifndef MSVC5
	char temp[16];
	const char *tmpoid;
#endif
	HSTMT hstmt = NULL;
	HDBC hdbc = NULL;
	int status;
	YYSTYPE *y_conn_ind, *y_query;

	/* get args */

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &y_conn_ind, &y_query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(y_conn_ind);
	convert_to_string(y_query);

	conn = y_conn_ind->value.lval;
	query = y_query->value.strval;

	/* check db connection */
	hdbc = odbc_get_hdbc(conn);
	if (hdbc == NULL) {
		php3_error(E_WARNING, "Bad odbc connection number");
		RETURN_ZERO;
	}
	/* alloc a statement */
	status = SQLAllocStmt(hdbc, &hstmt);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		odbc_error(hdbc, SQL_NULL_HSTMT, "SQLAllocStmt");
		php3_error(E_WARNING, "Could not allocate statement handle\n");
		RETURN_ZERO;
	}
	/* execute query */
	status = SQLExecDirect(hstmt, (UCHAR *) query, SQL_NTS);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		odbc_error(hdbc, hstmt, "SQLExecDirect");
		php3_error(E_WARNING, "Bad results from database\n");
		RETURN_ZERO;
	}
	/* check results */
	if (hstmt == NULL) {
		php3_error(E_WARNING, "Bad response to odbc query\n");
#if MSVC5
		status = SQLFreeStmt(hstmt,SQL_DROP);
#else
		status = SQLFreeStmt(hstmt);
#endif
		if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
			odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLFreeStmt");
			php3_error(E_WARNING, "Could not free statement handle\n");
			RETURN_ZERO;
		}
		j = 0;
	}
	/* add statement to list */
	j = odbc_add_hstmt(hdbc, hstmt);

	/* return statement index */
	return_value->type = IS_LONG;
	return_value->value.lval = j;
	return;
}

/*
   ODBCgetdata()
   Retrieves data from a query set
 */

void ODBCgetdata(INTERNAL_FUNCTION_PARAMETERS)
{
	char *fieldname = NULL;
	int res_ind, tuple_ind;
	int field_ind = -1;
	HSTMT hstmt;
	char value[8193];
	odbcField *odbc_field = NULL;
	int status;
#if MSVC5
	SDWORD FAR nchars;
#else
	SDWORD nchars;
	/* unused ? */
	HDBC hdbc;
	char state[256], msg[1025];
	int n, nmsg;
#endif
	YYSTYPE *y_res_ind, *y_tuple_ind, *y_field;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &y_res_ind, &y_tuple_ind, &y_field) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(y_res_ind);
	convert_to_long(y_tuple_ind);
	convert_to_string(y_field);

	res_ind = y_res_ind->value.lval;
	tuple_ind = y_tuple_ind->value.lval;
	fieldname = y_field->value.strval;

	/* index 1 means no tuples */
	if (res_ind == 1) {
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
	}
	/* get statement from index */
	hstmt = odbc_get_hstmt(res_ind);
	if (hstmt == NULL) {
		php3_error(E_WARNING, "Unable to find result index %d", res_ind);
		RETURN_FALSE;
	}
	/* get field index if the field parameter was a string */
	if (fieldname != NULL) {
		odbc_field = odbc_getFieldByName(hstmt, fieldname);
		if (!odbc_field) {
			php3_error(E_WARNING, "Field %s not found", fieldname);
			RETURN_FALSE;
		}
php3_error(E_WARNING, "Fieldname [%s] Fieldnum [%d]", fieldname, odbc_field->colnum);
	}
	/*
	   else {
	   odbc_field = odbc_getFieldByNumber(hstmt, field_ind);
	   if (!odbc_field) {
	   php3_error(E_WARNING,"Field %d not found", field_ind);
	   RETURN_FALSE;
	   }
	   }
	 */

	/* get field type */

	if (!odbc_field->coltype) {
		php3_error(E_WARNING, "Unable to type field %s", fieldname);
		RETURN_FALSE;
	}
	/* retrieve the field value */
	status = SQLGetData(hstmt, odbc_field->colnum, SQL_CHAR, value, 8192, &nchars);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		odbc_error(SQL_NULL_HDBC, hstmt, "SQLGetData");
php3_error(E_WARNING, "Fieldnum [%d] status [%d]", odbc_field->colnum, status);
		RETURN_FALSE;
	}
	/* check value */
	if (!value) {
		php3_error(E_WARNING, "Error getting data value");
		RETURN_FALSE;
	}
	/* check type and return appropriate PHP type */
	switch (odbc_field->coltype) {
		case SQL_BIT:
		case SQL_BIGINT:
		case SQL_TINYINT:
		case SQL_SMALLINT:
		case SQL_INTEGER:
			return_value->type = IS_LONG;
			return_value->value.lval = atoi(value);
			break;
		case SQL_DOUBLE:
		case SQL_DECIMAL:
		case SQL_NUMERIC:
		case SQL_FLOAT:
		case SQL_REAL:
			return_value->type = IS_DOUBLE;
			return_value->value.dval = atof(value);
			break;
		case SQL_TIME:
		case SQL_TIMESTAMP:
		case SQL_LONGVARCHAR:
		case SQL_LONGVARBINARY:
		case SQL_VARBINARY:
		case SQL_BINARY:
		case SQL_DATE:
		case SQL_VARCHAR:
		case SQL_CHAR:
		default:
			return_value->type = IS_STRING;
			return_value->value.strval = estrdup(value);
			return_value->strlen = strlen(value);
			break;
	}
	return;
}

/*
   ODBCfree()
   Frees a statement (result)
 */

void ODBCfree(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *y_res_ind;
	int res_ind;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &y_res_ind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(y_res_ind);
	res_ind = y_res_ind->value.lval;

	/* remove statement */
	odbc_del_hstmt(res_ind);
	RETURN_ONE;
}

/*
   ODBCconnect()
   Connect to an ODBC data source
 */

void ODBCconnect(INTERNAL_FUNCTION_PARAMETERS)
{
#if MSVC5
	HDBC		hdbc;
	UCHAR FAR	*dsn=NULL;
	UCHAR FAR	*uid=NULL;
	UCHAR FAR	*pwd=NULL;
	RETCODE		status;
#else
	HDBC		*hdbc;
	char		*dsn=NULL;
	char 		*uid=NULL;
	char 		*pwd=NULL;
	int			status;
	char		*temp;
#endif
	int j;
	YYSTYPE *y_dsn, *y_uid, *y_pwd;
	ODBC_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &y_dsn, &y_uid, &y_pwd) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(y_dsn);
	convert_to_string(y_uid);
	convert_to_string(y_pwd);

	dsn = y_dsn->value.strval;
	uid = y_uid->value.strval;
	pwd = y_pwd->value.strval;

	/* if no henv, alloc one */

	if (!ODBC_GLOBAL(henv)) {
		status = SQLAllocEnv(&ODBC_GLOBAL(henv));
		if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
			odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLAllocEnv");
			RETURN_ZERO;
		}
	}
	/* alloc a connection */
	status = SQLAllocConnect(ODBC_GLOBAL(henv), &hdbc);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLAllocConnect");
		RETURN_ZERO;
	}
	/* connect... */
#if MSVC5
	status = SQLConnect(hdbc, dsn, SQL_NTS, uid, SQL_NTS, pwd, SQL_NTS);
#else
	status = SQLConnect(hdbc, (UCHAR *) dsn, SQL_NTS, (UCHAR *) uid, SQL_NTS, (UCHAR *) pwd, SQL_NTS);
#endif

	/* check status */
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		php3_error(E_WARNING, "Could not connect to database (%s)\n", dsn);
		odbc_error(hdbc, SQL_NULL_HSTMT, "SQLConnect");
		RETURN_ZERO;
	} else {
		j = odbc_add_hdbc(hdbc);
	}
	return_value->type = IS_LONG;
	return_value->value.lval = j;
	return;
}

/*
   ODBCdisconnect()
   Shuts down an ODBC connection
 */

void ODBCdisconnect(INTERNAL_FUNCTION_PARAMETERS)
{
	int conn_ind;
	HDBC hdbc;
	YYSTYPE *y_conn_ind;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &y_conn_ind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(y_conn_ind);
	conn_ind = y_conn_ind->value.lval;

	/* get connection */
	hdbc = odbc_get_hdbc(conn_ind);

	/* close connection */
	odbc_del_hdbc(hdbc);
	RETURN_ONE;
}

/*
   ODBCrowcount()
   Returns number of rows in 
   query
 */

void ODBCrowcount(INTERNAL_FUNCTION_PARAMETERS)
{
	int res_ind;
	HSTMT hstmt;
	SDWORD nrows;
#ifndef MSVC5
	char temp[16];
#endif
	int status;
	YYSTYPE *y_res_ind;

	/* get statement index */
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &y_res_ind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(y_res_ind);
	res_ind = y_res_ind->value.lval;

	/* error returned for non-tuple returning queries */
	/* maybe this should be zero? */
	if (res_ind == 1) {
		php3_error(E_WARNING, "No tuples available at this result index");
		RETURN_FALSE;
		return;
	}
	/* get statement from index */
	hstmt = odbc_get_hstmt(res_ind);

	if (hstmt == NULL) {
		php3_error(E_WARNING, "Bad result index in odbc_rowcount");
		RETURN_FALSE;
		return;
	}
	/* get rowcount */
	status = SQLRowCount(hstmt, &nrows);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		odbc_error(SQL_NULL_HDBC, hstmt, "SQLRowCount");
		RETURN_FALSE;
	}
	/* return number of rows */
	return_value->type = IS_LONG;
	return_value->value.lval = nrows;
	return;
}


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
