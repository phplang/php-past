/***[odbc.c]******************************************************[TAB=4]****\
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
* ODBC Extensions                                                            *
*                                                                            *
* Written by Brian Schaffner (brian@tool.net)                                *
*                                                                            *
\****************************************************************************/

#include "php.h"
#include <stdlib.h>

#include "parse.h"
#include <ctype.h>

#if HAVE_ODBC
#if WINNT|WIN32
#include <sql.h>
#include <sqlext.h>
#else
  #include <isql.h>
  #include <isqlext.h>
#endif

#if APACHE
#ifndef DEBUG
#undef palloc
#endif
#endif

#define MAX_FIELD_LEN 256

/* 
	Statement List 
	The statement list is a linked list
	of HSTMT's. It contains information about
	a query along with information about each column.
	No DATA is stored in the list.
*/

typedef struct odbcHstmtList {
	HDBC	hdbc;			/* connection */
	HSTMT	hstmt;			/* the statement */
	int	index;			/* index in list */
	int	gotnames;		/* did we get the field names? */
	int 	ncols;			/* number of fields */
#if WINNT|WIN32
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
	struct 	odbcHstmtList *next;	/* next statement in list */
} odbcHstmtList;

/*
	Connection List
	The connection list is a linked list of
	HDBC's. It contains information about each
	connection.
*/

typedef struct odbcHdbcList {
	HDBC	hdbc;			/* the connection */
	int	index;			/* index in list */
	int 	connected;		/* is it connected? */
	struct	odbcHdbcList *next;	/* next connection in list */
} odbcHdbcList;

/* 
	Field structure
	The odbcField struct contains information
	about a single column. No data is stored in
	the structure
*/

typedef struct odbcField {
	char name[MAX_FIELD_LEN];
#if WINNT|WIN32
	UWORD colnum;
#else
  	int colnum;
#endif
	int coltype;
	int colprecision;
	int colscale;
	int colnullable;
} odbcField;	

static odbcHstmtList	*odbc_hstmt_top=NULL;  	/* statement list head */
static odbcHdbcList	*odbc_hdbc_top=NULL;	/* connection list head */	
static odbc_hstmt_ind = 2;			/* initial statement index */
static odbc_hdbc_ind = 1;			/* initial connection index */
static HENV henv = NULL;			/* HENV variable */


/* odbc initialization routine */
/* called in main.c */
void php_init_odbc(void) {
	/* simply initialize some variables */
	odbc_hstmt_top=NULL;
	odbc_hdbc_top=NULL;
	odbc_hstmt_ind = 2;
	odbc_hdbc_ind = 1;
}

/* odbc_error() reports errors from odbc calls */
void odbc_error(HDBC hdbc, HSTMT hstmt, char *func) {
  	char 	msg[1025];
  	char 	state[255];
  	SDWORD	native;
  	SWORD	bytes;
  	int 	status;
  	int 	sethenv = 0;
  	
  	/* check henv - use SQL_NULL_HENV if not good */
  	if (!henv) {
  		henv = SQL_NULL_HENV;
  		sethenv=1;
  	}
  
  	/* call SQLError to get error */
   	status = SQLError(henv, hdbc, hstmt, state, &native, msg, 1024, &bytes);
   	
   	/* check status */
   	if (status == SQL_NO_DATA_FOUND) {
   		/* if no errors, return */
   	  	return;
   	}
   	
   	if (status == SQL_ERROR || status == SQL_INVALID_HANDLE) {
   		Error("SQLError: Unknown error\n");
   		return;
   	}
   	
   	/* while there are errors in the stack */
  	while ( status == SQL_SUCCESS || status == SQL_SUCCESS_WITH_INFO) {
  		/* report error */
        	if (func) {
        		Error("%s(%s): %s\n", func, state, msg);
        	} else {
        		Error("SQLError(%s): %s\n", state, msg);
        	}
        	/* get next error */
        	status = SQLError(henv, hdbc, hstmt, state, &native, msg, 1024, &bytes);
        	/* if we're done, return */
        	if (status == SQL_NO_DATA_FOUND) {
   	  		return;
	  	}
   		if (status == SQL_ERROR || status == SQL_INVALID_HANDLE) {
   			Error("SQLError: Unknown error\n");
	   		return;
   		}
  	}

  	/* restore henv if changed */
  	if (sethenv) {
   		henv = NULL;
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

	odbcHstmtList	*new;
	
	/* get list head */
	new = odbc_hstmt_top;
	
	/* if no head, allocate one */
	if (!new) {
		new = emalloc(0,sizeof(odbcHstmtList));
		if (!new) {
			Error("Out of memory");
		}
		odbc_hstmt_top = new;
	} else {
		/* otherwise, go to end of list and allocate */
		while (new->next) {
			new=new->next;
		}
		new->next = emalloc(0,sizeof(odbcHstmtList));
		if (!(new->next)) {
			Error("Out of memory");
		} else {
			new = new->next;
		}
	}

	/* initialize statement pointer */ 
	new->hdbc	= hdbc;
	new->hstmt	= hstmt;
	new->gotnames	= 0;
	new->fname	= NULL;
	new->coltype	= NULL;
	new->colprecision = NULL;
	new->colscale 	= NULL;
	new->colnullable = NULL;
	new->ncols 	= 0;
	new->index	= odbc_hstmt_ind++;
	new->next	= NULL;
	
	/* return index */
	
	return (odbc_hstmt_ind-1);
}

/* 
	odbc_get_hstmt()
	Finds a statement using integer
	index.
*/

HSTMT odbc_get_hstmt(int count)
{
	odbcHstmtList	*new;

	/* 1st index is reserved for no tuples */
	if (count == 1) {
		return NULL;
	}

	/* find head */
	new = odbc_hstmt_top;
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
	odbc_get_list()
	Finds a list item based on a statement
*/
odbcHstmtList *odbc_get_list(HSTMT hstmt) {
  	odbcHstmtList	*new;
  	
  	/* find head */
	new = odbc_hstmt_top;
	while (new) {
		/* find statement and return item */
	  	if(new->hstmt == hstmt) {
	  		return (new);
	  	}
	  	new=new->next;
	}
	return NULL;
} 	

/* 
	odbc_del_hstmt()
	Removes a statement from list
	and clears resources 
*/

void odbc_del_hstmt(int count)
{
	odbcHstmtList	*new, *prev, *next;
	int		status;

	if (count == 1) {
		return;
	}
	
	prev = NULL;
	/* find head */
	new = odbc_hstmt_top;
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
				odbc_hstmt_top = next;
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
	odbcHdbcList	*new;

	/* find head */
	new = odbc_hdbc_top;
	
	/* if no head - alloc one */
	if (!new) {
		new = emalloc(0,sizeof(odbcHdbcList));
		if (!new) {
			Error("Out of memory");
		}
		odbc_hdbc_top = new;
	} else {
		/* otherwise go to end and alloc */
		while (new->next) new=new->next;
		new->next = emalloc(0,sizeof(odbcHdbcList));
		if (!(new->next)) {
			Error("Out of memory");
		} else {
			new = new->next;
		}
	}

	/* initialize connection */
	
	new->hdbc	= hdbc;
	new->index	= odbc_hdbc_ind++;
	new->next	= NULL;
	return (odbc_hdbc_ind-1);
}

/*
	odbc_get_hdbc()
	Gets a connection based on the index
*/

HDBC odbc_get_hdbc(int count)
{
	odbcHdbcList	*new;

	/* find head */
	new = odbc_hdbc_top;
	
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
	odbcHdbcList	*new, *prev, *next;
	int		status;

	prev = NULL;
	
	/* find head */
	new = odbc_hdbc_top;
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
				odbc_hdbc_top = next;
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

void ODBCcloseAll(void) {
	odbcHstmtList	*lnew, *lnext;
	odbcHdbcList	*cnew, *cnext;
	int		status;

	/* find head */
	lnew = odbc_hstmt_top; 
	while (lnew) {
		lnext = lnew->next;
		/* if the statement is good, free it */
		if (lnew->hstmt) {
#if WINNT|WIN32
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
	odbc_hstmt_top = NULL;

	/* find head */
	cnew = odbc_hdbc_top;
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
		cnew=cnext;
	}
	odbc_hdbc_top = NULL;
}

/*
	odbc_getFieldByName()
	Finds a field using a case-insensitive
	string comparison.	
*/

odbcField *odbc_getFieldByName(HSTMT hstmt, char *field) {
	int 		gotnames = 0;
#if WINNT|WIN32
	UWORD		i = 0;
	SWORD FAR   n;
#else
	int 		i = 0;
	int 		n;
#endif
	odbcHstmtList	*list;
	int 		fieldnum = -1;
	odbcField 	*odbc_field = NULL;
	int		status;
  
  	/* get list */
  	list = odbc_get_list(hstmt);
  
  	if (!list) {
    		return NULL;
  	}

 	/* did we already get field names? */
  	switch(list->gotnames) {
    	case 0:
    		/* get the number of fields */
    		status = SQLNumResultCols(hstmt, (SWORD *) &(list->ncols));
    		if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
        	 	odbc_error(SQL_NULL_HDBC, hstmt, "SQLNumResultCols");
        		return NULL;
        	}

		/* alloc space for fields */
        	if (list->ncols > 0) {
#if WINNT|WIN32
				list->fname = (UCHAR FAR **) emalloc(0, sizeof(char *) * (list->ncols + 1));
          		list->coltype = (SWORD FAR *) emalloc(0, sizeof(int) * (list->ncols + 1));
          		list->colprecision = (UDWORD FAR *) emalloc(0, sizeof(int) * (list->ncols + 1));
          		list->colscale = (SWORD FAR *) emalloc(0, sizeof(int) * (list->ncols + 1));
          		list->colnullable = (SWORD FAR *) emalloc(0, sizeof(int) * (list->ncols + 1));          
#else
				list->fname = (char **) emalloc(0, sizeof(char *) * (list->ncols + 1));
          		list->coltype = (int *) emalloc(0, sizeof(int) * (list->ncols + 1));
          		list->colprecision = (int *) emalloc(0, sizeof(int) * (list->ncols + 1));
          		list->colscale = (int *) emalloc(0, sizeof(int) * (list->ncols + 1));
          		list->colnullable = (int *) emalloc(0, sizeof(int) * (list->ncols + 1));          
#endif
			}
        	
        	/* put fields into arrays */
       	 	i = 0;
        	while (i < list->ncols) {
          		list->fname[i] = emalloc(0, 255);
#if WINNT|WIN32
				status = SQLDescribeCol(hstmt, (UWORD)(i+1), list->fname[i], (SWORD)MAX_FIELD_LEN, &n, &(list->coltype[i]), &(list->colprecision[i]), &(list->colscale[i]), &(list->colnullable[i]));
#else
				status = SQLDescribeCol(hstmt, i+1, list->fname[i], MAX_FIELD_LEN, &n, &(list->coltype[i]), &(list->colprecision[i]), &(list->colscale[i]), &(list->colnullable[i]));
#endif          		
				if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
          			odbc_error(SQL_NULL_HDBC, hstmt, "SQLDescribeCol");
            			return NULL;
          		}

 	  		list->fname[i+1] = NULL;
          		list->coltype[i+1] = 0;
          		list->colprecision[i+1] = 0;
          		list->colscale[i+1] = 0;
          		list->colnullable[i+1] = 0;
          
          		/* if it's the field we're looking for, alloc
          		   field and assign values
          		*/
          		if (!strcasecmp(list->fname[i], field)) {
            			odbc_field = emalloc(0, sizeof(odbcField));
            			if (odbc_field) {
              				odbc_field->colnum = i+1;
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
            			odbc_field = emalloc(0, sizeof(odbcField));
            			if (odbc_field) {
              				odbc_field->colnum = i+1;
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

odbcField *odbc_getFieldByNumber(HSTMT hstmt, int field) {
	int 		gotnames = 0;
#if WINNT|WIN32
	UWORD		i = 0;
	SWORD FAR	n;
#else
	int 		i = 0;
	int 		n;
#endif
	odbcHstmtList	*list;
	int 		fieldnum = -1;
	odbcField 	*odbc_field = NULL;
	int		status;  
	
	/* get list */
	list = odbc_get_list(hstmt);
	if (!list) {
    		return NULL;
  	}
	
	/* got the field names already? */
	switch(list->gotnames) {
    	case 0:
    		/* get number of fields */
    		status = SQLNumResultCols(hstmt, (SWORD *) &(list->ncols));
    		if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
        		odbc_error(SQL_NULL_HDBC, hstmt, "SQLNumResultCols");
          		return NULL;
        	}
        
        	/* alloc space */
        	if (list->ncols > 0) {
#if WINNT|WIN32
				list->fname = (UCHAR FAR  **) emalloc(0, sizeof(char) * MAX_FIELD_LEN * list->ncols);
          		list->coltype = (SWORD FAR  *) emalloc(0, sizeof(int) * list->ncols);
          		list->colprecision = (UDWORD FAR *) emalloc(0, sizeof(int) * list->ncols);
          		list->colscale = (SWORD FAR  *) emalloc(0, sizeof(int) * list->ncols);
          		list->colnullable = (SWORD FAR  *) emalloc(0, sizeof(int) * list->ncols);          
#else
				list->fname = (char **) emalloc(0, sizeof(char) * MAX_FIELD_LEN * list->ncols);
          		list->coltype = (int *) emalloc(0, sizeof(int) * list->ncols);
          		list->colprecision = (int *) emalloc(0, sizeof(int) * list->ncols);
          		list->colscale = (int *) emalloc(0, sizeof(int) * list->ncols);
          		list->colnullable = (int *) emalloc(0, sizeof(int) * list->ncols);          
#endif
			}
        
        	/* loop through and assign fields */
        	while (i < list->ncols) {
#if WINNT|WIN32
				status = SQLDescribeCol(hstmt, (UWORD)(i+1), list->fname[i], MAX_FIELD_LEN, &n, &(list->coltype[i]), (list->colprecision), (list->colscale), (list->colnullable));
#else
				status = SQLDescribeCol(hstmt, i+1, list->fname[i], MAX_FIELD_LEN, &n, &(list->coltype[i]), &(list->colprecision), &(list->colscale), &(list->colnullable));
#endif
				if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
          			odbc_error(SQL_NULL_HDBC, hstmt, "SQLDescribeCol");
            			return NULL;
          		}
	          	/* if it's the field we're looking for,
	          	   alloc a field and assign values 
	          	*/
	          	
          		if (i ==  field) {
            			odbc_field = emalloc(0, sizeof(odbcField));
            			if (odbc_field) {
              				odbc_field->colnum = i+1;
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
            			odbc_field =  emalloc(0, sizeof(odbcField));
            			if (odbc_field) {
              				odbc_field->colnum = i+1;
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
#endif

/* Main User Functions */

/* 
	ODBCfetch()
	Fetches a row from an executed statement
*/

void ODBCfetch(void) {
#if HAVE_ODBC
	Stack 	*s;
	HSTMT	hstmt;
	int	res_ind, status;
	
		
	/* get statement index from stack */		
	s = Pop(); 
	if (!s) {
	  	Error("Stack error in odbc_fetch");
	  	Push("0", LNUMBER);
	  	return;
	}
	
	if (s->strval) {
	 	res_ind = s->intval;
	} else {
		res_ind = 0;
	}
	
	/* statement index of 1 means no tuples */
	if (res_ind == 1) {
	  	Push("0", LNUMBER);
	  	return;
	}
	
	/* convert index to statement */
	hstmt = odbc_get_hstmt(res_ind);
	if (!hstmt) {
		Error("Bad statement index in odbc_fetch");
		Push("0", LNUMBER);
		return;
	}
	
	/* fetch a row */
	status = SQLFetch(hstmt);
	if (status == SQL_SUCCESS || status == SQL_SUCCESS_WITH_INFO) {
	  	Push("1", LNUMBER);
 		return;
	} else {
		if (status == SQL_ERROR || status == SQL_INVALID_HANDLE) {
			odbc_error(SQL_NULL_HDBC, hstmt, "SQLfetch");
		}	
	  	Push("0", LNUMBER);
	  	return;
	}
#else
	Pop();
	Push("0", LNUMBER);
	Error("No odbc support");
#endif
}

/*
	ODBCexecdirect()
	Does an SQLExecDirect on an SQL query
*/

void ODBCexecdirect(void) {
#if HAVE_ODBC
	Stack	*s;
	int	conn;
	char	*query;
	int	j;
	char	temp[16];
#if !(WINNT|WIN32)
	const 	char *tmpoid;
#endif
	HDBC 	hdbc=NULL;
	HSTMT 	hstmt=NULL;
	int	status;

	/* get args */
	
	/* get query string */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_exec ");
		Push("0", LNUMBER);
		return;
	}
	
	if (s->strval) {
		query = (char*) estrdup(1,s->strval);
	} else {
		Error("No query string in odbc_exec");
		Push("0", LNUMBER);
		return;
	}

	/* get connection index */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_exec");
		Push("0", LNUMBER);
		return;
	}

	if (s->strval) {
		conn = s->intval;
	} else {
		conn = 0;
	}

	/* check db connection */
	hdbc = odbc_get_hdbc(conn);
	if (hdbc == NULL) {
		Error("Bad odbc connection number");
		Push("0", LNUMBER);
		return;
	}

	/* alloc a statement */
	status = SQLAllocStmt(hdbc, &hstmt);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		odbc_error(hdbc, SQL_NULL_HSTMT, "SQLAllocStmt");
	  	Push("0", LNUMBER);
	  	return;
	}
	
	/* execute query */
	status = SQLExecDirect(hstmt, (UCHAR *) query, SQL_NTS);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		odbc_error(hdbc, hstmt, "SQLExecDirect"); 
	  	Push("0", LNUMBER);
	  	return;
	}

	/* check results */
	if (hstmt == NULL) {
		Error("Bad response to odbc query");
#if WINNT|WIN32
		status = SQLFreeStmt(hstmt,SQL_DROP);
#else
		status = SQLFreeStmt(hstmt);
#endif
		if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
			odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLFreeStmt");
			return;
		}
		j = 0;
	}  
	
	/* add statement to list */
	j = odbc_add_hstmt(hdbc, hstmt);
	
	/* return statement index */
	sprintf(temp, "%d", j);
	Push(temp, LNUMBER);
#else
	Pop();
	Pop();
	Push("0", LNUMBER);
	Error("No odbc support");
#endif
}

/*
	ODBCgetdata()
	Retrieves data from a query set
*/

void ODBCgetdata(void) {
#if HAVE_ODBC
	Stack		*s;
	char		*fieldname;
	int		res_ind, tuple_ind;
	int		field_ind = -1;
	SDWORD		nchars;
	HSTMT		hstmt;
	char		value[8193];
	odbcField	*odbc_field = NULL;
	int		status;
	static int	lasttuple = -1;
#if !(WINNT|WIN32)
	HDBC		hdbc;
	char 		state[256], msg[1025];
	int 		n, nmsg;
#endif
	
	/* get field name/number */	
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_getdata");
		Push("", STRING);
		return;
	}
	if (s->strval) {
		if (s->type == STRING) {
			fieldname = estrdup(1,s->strval); 
		} else {
			field_ind = s->intval;
		}
	} else {
		Error("No field argument in odbc_getdata");
		Push("", STRING);
		return;
	}


	/* get tuple index */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_getdata");
		Push("", STRING);
		return;
	}
	if (s->strval) {
	  	tuple_ind = s->intval;
	} else {
		tuple_ind = 0;
	}

	/* get statement index */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_getdata");
		Push("", STRING);
		return;
	}
	if (s->strval) {
		res_ind = s->intval;
	} else {
		res_ind = 0;
	}


	/* index 1 means no tuples */
	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("", STRING);
		return;
	}

	
	/* get statement from index */
	hstmt = odbc_get_hstmt(res_ind);
	if (hstmt == NULL) {
		Error("Unable to find result index %d", res_ind);
		Push("", STRING);
		return;
	}

	/* get field index if the field parameter was a string */
	if (fieldname != NULL)
	{
		odbc_field = odbc_getFieldByName(hstmt, fieldname);
		if (!odbc_field) {
			Error("Field %s not found", fieldname);
			Push("", STRING);
			return;
		}
	} else {
		odbc_field = odbc_getFieldByNumber(hstmt, field_ind);
		if (!odbc_field) {
			Error("Field %d not found", field_ind);
			Push("", STRING);
			return;
		}
	}

	/* get field type */

	if (!odbc_field->coltype) {
		Error ("Unable to type field %s", fieldname);
		Push("", STRING);
		return;
	}

	/* retrieve the field value */
	status = SQLGetData(hstmt, odbc_field->colnum, SQL_CHAR, value, 8192, &nchars);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		odbc_error(SQL_NULL_HDBC, hstmt, "SQLGetData");
	  	Push("", STRING);
	  	return;
	}
	
	/* check value */
	if (!value)
	{
		Error("Error getting data value");
		Push("", STRING);
		return;
	}

	/* check type and return appropriate PHP type */	
	switch(odbc_field->coltype) {
	case SQL_BIT:
	case SQL_BIGINT:
	case SQL_TINYINT:
	case SQL_SMALLINT:
	case SQL_INTEGER:
		Push(value, LNUMBER);
		break;
	case SQL_DOUBLE:
	case SQL_DECIMAL:
	case SQL_NUMERIC:
	case SQL_FLOAT:
	case SQL_REAL:
		Push(value, DNUMBER);
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
		Push(value, STRING);
 	  	break;
	}
	return;

#else
	Pop();
	Pop();
	Pop();
	Push("", STRING);
	Error("No odbc support");
#endif
}

/*
	ODBCfree()
	Frees a statement (result)
*/

void ODBCfree(void) {
#if HAVE_ODBC
	Stack		*s;

	/* get index */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_free");
		return;
	}
	if (s->strval) {
		/* remove statement */
		odbc_del_hstmt(s->intval);
	} else {
		Error("Invalid result index in odbc_free");
		return;
	}
#else
	Pop();
	Error("No odbc support");
#endif
}

/*
	ODBCconnect()
	Connect to an ODBC data source
*/

void ODBCconnect(void) {
#if HAVE_ODBC
	Stack		*s;
#if WINNT|WIN32
	HDBC		hdbc;
	UCHAR		*dsn=NULL;
	UCHAR 		*uid=NULL;
	UCHAR 		*pwd=NULL;
	RETCODE		status;
#else
	HDBC		*hdbc;
	char		*dsn=NULL;
	char 		*uid=NULL;
	char 		*pwd=NULL;
	int		status;
#endif
	char		*temp;
	int		j;

	/* get password */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) pwd = estrdup(1,s->strval);
	else {
		Error("No password supplied in odbc_connect");
		Push("0", LNUMBER);
		return;
	}
	
	/* get userid */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) uid = estrdup(1,s->strval);
	else {
		Error("No userid supplied in odbc_connect");
		Push("0", LNUMBER);
		return;
	}

	/* get dsn */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_connect");
		Push("0", LNUMBER);
		return;
	}
	if (s->strval) dsn = estrdup(1,s->strval);
	else {
		Error("No Data Source Name supplied in odbc_connect");
		Push("0", LNUMBER);
		return;
	}
		
	/* if no henv, alloc one */	
	
	if (!henv) {
		status = SQLAllocEnv(&henv);
		if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
	  		odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLAllocEnv");
	    		Push("0", LNUMBER);
	    		return;
	  	}
	}
	
	/* alloc a connection */
	status = SQLAllocConnect(henv, &hdbc);
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		odbc_error(SQL_NULL_HDBC, SQL_NULL_HSTMT, "SQLAllocConnect");
	    	Push("0", LNUMBER);
	    	return;
	}
	
	/* connect... */
	status = SQLConnect(hdbc, (UCHAR *) dsn, SQL_NTS, (UCHAR *) uid, SQL_NTS, (UCHAR *) pwd, SQL_NTS);
	
	/* check status */
	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
		Error("Could not connect to database (%s)\n", dsn);
		odbc_error(hdbc, SQL_NULL_HSTMT, "SQLConnect");
	    	Push("0", LNUMBER);
	    	return;
	} else {
		j = odbc_add_hdbc(hdbc);
		temp = (char*) emalloc(1,(j%10)+3);
		sprintf(temp, "%d", j);
	}
	Push(temp, LNUMBER);
#else
	Pop();
	Pop();
	Pop();
	Error("No odbc support");
#endif
}

/*
	ODBCdisconnect()
	Shuts down an ODBC connection
*/

void ODBCdisconnect(void) {
#if HAVE_ODBC
	Stack		*s;
	int		conn_ind;
	HDBC		hdbc;
	
	/* get connection index */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_close");
		return;
	}
	
	if (s->strval) {
		conn_ind = s->intval;
	} else {
		conn_ind = 0;
	}

	/* get connection */
	hdbc = odbc_get_hdbc(conn_ind);
	
	/* close connection */
	odbc_del_hdbc(hdbc);
#else
	Pop();
	Error("No odbc support");
#endif
}

/*
	ODBCrowcount()
	Returns number of rows in 
	query
*/

void ODBCrowcount(void) {
#if HAVE_ODBC
	Stack		*s;
	int		res_ind;
	HSTMT		hstmt;
	SDWORD		nrows;
	char		temp[16];
	int		status;

	/* get statement index */
	s = Pop();
	if (!s) {
		Error("Stack error in odbc_rowcount");
		Push("-1", LNUMBER);
		return;
	}
	if (s->strval) {
		res_ind = s->intval;
	} else {
		res_ind = 0;
	}

	/* error returned for non-tuple returning queries */
	/* maybe this should be zero? */
	if (res_ind == 1)
	{
		Error("No tuples available at this result index");
		Push("-1", LNUMBER);
		return;
	}
	
	/* get statement from index */
	hstmt = odbc_get_hstmt(res_ind);
	
	if (hstmt == NULL) {
		Error("Bad result index in odbc_rowcount");
		Push("-1", LNUMBER);
		return;
	}
 
   	/* get rowcount */
 	status = SQLRowCount(hstmt, &nrows);
 	if (status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO) {
        	odbc_error(SQL_NULL_HDBC, hstmt, "SQLRowCount");
          	Push("-1", LNUMBER);
          	return;
        }
	
	/* return number of rows */
	sprintf(temp, "%ld", nrows);
	Push(temp, LNUMBER);
#else
	Pop();
	Error("No odbc support");
#endif
}

