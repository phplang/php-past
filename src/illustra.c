/***[illustra.c]**************************************************[TAB=8]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996 Rasmus Lerdorf                                         *
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
* Illustra Extensions                                                        *
*                                                                            *
* Written by Simon Gornall (simon@oyster.co.uk)                              *
*                                                                            *
*  More information about Illustra can be found at this URL:                 *
*                                                                            *
*        http://www.illustra.com                                             *
*                                                                            *
*  The Illustra database system is Copyright (c) Informix Ltd                *
*                                                                            *
*                                                                            *
\****************************************************************************/

#include <stdlib.h>
#include <ctype.h>
#include "php.h"
#include "parse.h"

#ifdef ILLUSTRA
#include <mi.h>
#endif

/******************************************************************************
 * Static global variables to store state in between calls in one session
 *****************************************************************************/
#ifdef ILLUSTRA

typedef struct miRow
    {
    int numC;			/* The number of columns in this row	    */
    char **row;			/* The column data itself		    */
    struct miRow *next;     	/* The next row element     	    	    */
    } miRow;

typedef struct miResult
    {
    int numC;	    	    	/* Number of columns	    	    	    */
    int numR;			/* Number of rows for this result	    */
    int maxR;			/* Maximum number of rows for this result   */
    char **names;		/* The titles of the rows		    */
    miRow *rows;		/* The rows of data themselves		    */
    } miResult;

typedef struct miConn
    {
    int numR;			/* Number of results for this connection    */
    int maxR;			/* Maximum num results we can use *now*	    */
    miResult **result;		/* List of result structures		    */
    MI_CONNECTION *this;	/* The connection to the database	    */
    char *dbName;		/* Name of the database for this conn	    */
    } miConn;
    
typedef struct miList
    {
    int numC;			/* Number of current connections	    */
    int maxC;			/* Maximum nun of conns we can use *now*    */
    miConn **conn;		/* The connection structure		    */
    } miList;

/* Static variables to preserve state					    */
static miList _list = {0, 0, NULL};	/* Database model		    */

/* Prototypes								    */
int	    _miAddConn(MI_CONNECTION *newConn, char *dbase);
int 	    _miDelConn(int cIndex);
mi_integer  _miAddResult(int cIndex);
mi_integer  _miGetRow(int cIndex, int resIndex);
int 	    _miFieldNumber(int cIndex, int rIndex, char *name);
char *      _miFieldName(int cIndex, int rIndex, int num);

#endif /* ILLUSTRA							    */

/******************************************************************************
 * Status   : Public
 * Function : Connect to an Illustra database
 *****************************************************************************/
void MIconnect(void)
    {
#ifdef ILLUSTRA

    Stack		    *s;
    MI_CONNECTION	    *conn;
    char		    *dbname=NULL;
    char		    *uname=NULL;
    char		    *passwd=NULL;
    char		    *temp;
    int			    j;
    
    /* Check for the password to be on the stack			*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_connect - no <b>password</b>");
	Push("-1", LNUMBER);
	return;
	}
    /* Recover the password						*/
    if (s->strval)
	if (strlen(s->strval) > 0)
	    passwd = estrdup(1,s->strval);
    else
	{
    	passwd = "";
    	}
 
    /* Check for the username to be on the stack			*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_connect - no <b>username</b>");
	Push("-1", LNUMBER);
	return;
	}
    /* Recover the username						*/
    if (s->strval)
	uname = estrdup(1,s->strval);
    else
	{
	Error("(Illustra) No username supplied in mi_connect");
	Push("-1", LNUMBER);
	return;
	}

    /* Check for the database name to be on the stack			*/
    /*==================================================================*/
     s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_connect - no <b>database</b>");
	Push("-1", LNUMBER);
	return;
	}
    /* Recover the database name					*/
    if (s->strval)
	dbname = estrdup(0,s->strval);
    else
	{
	Error("(Illustra) No database name supplied in mi_connect");
	Push("-1", LNUMBER);
	return;
	}  

    /* Attempt to connect						*/
    /*==================================================================*/
    conn = mi_open(dbname, uname, passwd);
    if (conn == NULL)
	{
	Error("(Illustra) Could not connect to server");
	temp = (char*) emalloc(1,2);
	sprintf(temp, "-1");
	}
    else
	{
	j = _miAddConn(conn, dbname);
	temp = (char*) emalloc(1,(j%10)+3);
	sprintf(temp, "%d", j);			
	}
	
	
    /* Return success (or otherwise)					*/
    /*==================================================================*/
    Push(temp, LNUMBER);
    
#else
    Pop();
    Pop();
    Pop();
    Error("No illustra support in this PHP module");
#endif
    }


/******************************************************************************
 * Status   : Public
 * Function : Return the database name for a passed connection
 *****************************************************************************/
void MIdbname(void)
    {
#ifdef ILLUSTRA
    Stack		*s;		    /* PHP Stack system		*/
    int			cIndex;		    /* Passed connection	*/
    char		*tmp;		    /* Return buffer		*/

    /* Recover the passed connection index				*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_dbname - no <b>Connection index</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	cIndex = s->intval;
    else cIndex = 0;

    /* Recover the connection from the index				*/
    /*==================================================================*/
    if (cIndex < 0 || cIndex >= _list.maxC)
	{
	Error("(Illustra) Bad connection index in mi_dbName");
	Push("-1", LNUMBER);
	return;
	}

    /* Return the connection name					*/
    /*==================================================================*/
    tmp = estrdup(1,_list.conn[cIndex]->dbName);
    if (tmp == NULL)
	{
	Error("(Illustra) Out of memory in mi_dbName");
	Push("-1", LNUMBER);
	return;
	}

    Push(tmp, STRING);

#else
    Pop();
    Error("No illustra support in this PHP module");
#endif
    }

/******************************************************************************
 * Status   : Public
 * Function : Close an active connection
 *****************************************************************************/
void MIclose(void)
    {
#ifdef ILLUSTRA
    Stack		*s;		    /* PHP Stack system		*/
    int			cIndex;		    /* Passed connection	*/
    MI_CONNECTION	*conn;		    /* The connection to close	*/

    /* Recover the passed connection index				*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_close - no <b>Connection Index</b>");
	return;
	}
    if (s->strval)
	cIndex = s->intval;
    else
	cIndex = 0;

    /* Delete the connection						*/
    /*==================================================================*/
    _miDelConn(cIndex);
    
#else
	Pop();
	Error("No illustra support in this PHP module");
#endif
}

 
/******************************************************************************
 * Status   : Public
 * Function : Execute SQL statements in the database
 *****************************************************************************/
void MIexec(void)
    {
#ifdef ILLUSTRA
    Stack		*s;		    /* PHP Stack system		*/
    int			cIndex;		    /* Connection id		*/
    MI_CONNECTION	*currConn=NULL;	    /* Connection itself	*/
    char		*query;		    /* The SQL to send		*/
    char		temp[16];	    /* Return string		*/
    int			j;		    /* Return value		*/
    int			result;		    /* Result of SQL query	*/
    
    /* Recover the passed query string					*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in illustra expression");
	Push("0", LNUMBER);
	return;
	}
    if (s->strval)
    	{
    	if (s->strval[strlen(s->strval)-1] != ';')
    	    {
    	    query = emalloc(0,strlen(s->strval) +1);
    	    strcpy(query,s->strval);
    	    strcat(query, ";");
    	    }
    	else
	    query = (char*) estrdup(1,s->strval);
	}
    else
	{
	Error("(Illustra) No query string in illustra expression");
	Push("0", LNUMBER);
	return;
	}

    /* Recover the passed connection index				*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in illustra expression");
	Push("0", LNUMBER);
	return;
	}
    if (s->strval)
	{
	cIndex = s->intval;
	}
    else
	{
	cIndex = 0;
	}

    /* Recover the connection from the index				*/
    /*==================================================================*/
    if (cIndex < 0 || cIndex >= _list.maxC)
	{
	Error("(Illustra) Bad connection index in mi_exec");
	Push("-1", LNUMBER);
	return;
	}

    /* Check the connection is ok					*/
    /*==================================================================*/
    result = mi_exec(_list.conn[cIndex]->this, query, 0);
    if (result == MI_ERROR) 
	Error("(Illustra) SQL returned ERROR status");
		
    else
	{
	/* Retrieve the results from the database query			*/
	/*==============================================================*/
	j = _miAddResult(cIndex);
	}

    sprintf(temp, "%d", j);
    Push(temp,LNUMBER);
#else
    Pop();
    Pop();
    Push("0", LNUMBER);
    Error("No illustra support in this PHP module");
#endif
}

 
/******************************************************************************
 * Status   : Public
 * Function : Retrieve a result inside PHP
 *****************************************************************************/
void MIresult(void)
    {
#ifdef ILLUSTRA
    Stack		*s;		    /* PHP stack structure	*/ 
    char		*colName =NULL;	    /* The name of the column	*/
    int			colIdx =-1;	    /* The column index		*/
    int			rIndex =0;	    /* The index of the result	*/
    int			cIndex =0;	    /* The connection index	*/
    int			rowIdx =0;	    /* The index of the row	*/
    miRow		*row;	    	    /* The result store area	*/
    char		*retVal =NULL;	    /* What to return as result	*/
    int     	    	i;  	    	    /* Loop variable	    	*/
    
    /* Retrieve the column argument					*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_result - no <b>column</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	{
	if (s->type == STRING)
	    colName = estrdup(1,s->strval);
	else
	    colIdx  = s->intval;
	}
    else
	{
	Error("(Illustra) Argument error in mi_result - <b>malformed column</b>");
	Push("-1", LNUMBER);
	return;
	}

    /* Retrieve the row number						*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_result - no <b>row</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	rowIdx = s->intval;

    /* Retrieve the result index					*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_result - no <b>result index</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	rIndex = s->intval;

    /* Retrieve the connection index					*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_result - no <b>connection index</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	cIndex = s->intval;

    /* Check the connection index   					    */
    /*======================================================================*/
    if (cIndex < 0 || cIndex >= _list.maxC)
	{
	Error("(illustra) Bad connection index in mi_result");
	Push("-1", LNUMBER);
	return;
	}

    /* Check the result index   					    */
    /*======================================================================*/
    if (rIndex < 0 || rIndex >= _list.conn[cIndex]->numR)
	{
	Error("(Illustra) Bad result index in mi_result");
	Push("-1", LNUMBER);
	return;
	}

    /* get field index if the field parameter was a string		*/
    /*==================================================================*/
    if (colName != NULL)
	{
	colIdx = _miFieldNumber(cIndex, rIndex, colName);
	if (colIdx < 0)
	    {
	    Error("(Illustra) Field %s not found", colName);
	    Push("-1", LNUMBER);
	    return;
	    }
	}

    /* Get the row to return						*/
    /*==================================================================*/
    row = _list.conn[cIndex]->result[rIndex]->rows;
    for (i=0; i<rowIdx; i++)
    	{    	
    	if (row == NULL)
    	    {
    	    Error("(Illustra) Bad row index in mi_result");
    	    Push("-1", LNUMBER);
    	    return;
    	    }
    	else
    	    row=row->next;
    	}
    	
    /* Get the string to return						*/
    /*==================================================================*/
    if (row->row[colIdx] == NULL)
	retVal = estrdup(1, "[NULL]");
    else
        retVal = estrdup(1, row->row[colIdx]);
    
    Push(retVal, STRING);

#else
	Pop();
	Pop();
	Pop();
	Push("-1", LNUMBER);
	Error("No Illustra support in this PHP module");
#endif
    }
 
/******************************************************************************
 * Status   : Public
 * Function : Return the number of rows in a query
 *****************************************************************************/
void MInumRows(void)
    {
#ifdef ILLUSTRA
    Stack		*s;		    /* PHP manipulation stack	    */
    int     	    	cIndex;    	    /* passed-in index of connection*/
    int			rIndex;	    	    /* Passed-in index of result    */
    char		temp[16];	    /* Passed-out number	    */
    miResult		*result;	    /* Result structure	    	    */
    
    /* Retrieve the result argument					    */
    /*======================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_numrows - no <b>result</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	rIndex = s->intval;
    else rIndex = 0;

    /* Retrieve the connection argument					    */
    /*======================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_numrows - no <b>connection</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	cIndex = s->intval;
    else cIndex = 0;

    /* Check the connection index   					    */
    /*======================================================================*/
    if (cIndex < 0 || cIndex >= _list.maxC)
	{
	Error("(Illustra) Bad connection index in mi_numRows");
	Push("-1", LNUMBER);
	return;
	}

    /* Check the result index   					    */
    /*======================================================================*/
    if (rIndex < 0 || rIndex >= _list.conn[cIndex]->numR)
	{
	Error("(Illustra) Bad result index in mi_numRows");
	Push("-1", LNUMBER);
	return;
	}

    sprintf(temp, "%d", _list.conn[cIndex]->result[rIndex]->numR);
    Push(temp, LNUMBER);
#else
    Pop();
    Error("No illustra support in this PHP module");
#endif
    }
 
/******************************************************************************
 * Status   : Public
 * Function : Return the number of the passed field name
 *****************************************************************************/
void MIfieldNum(void)
    {
#ifdef ILLUSTRA
    Stack		*s;		    /* PHP Stack system		*/
    int			cIndex;		    /* Passed connection	*/
    int			rIndex;		    /* Passed result	    	*/
    int			fNum;		    /* The field number		*/
    char		*fName;		    /* The field name		*/
    char		number[128];	    /* Temporary result ptr	*/
    
    /* Recover the field name						*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_fieldNum - no <b>field name</b>");
	return;
	}
    if (s->strval)
	fName = estrdup(1, s->strval);
    else
	{
	Error("(Illustra) Can't find a valid field name in mi_fieldNum");
	Push("-1", LNUMBER);
	return;
	}
    
    /* Retrieve the result argument					    */
    /*======================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_fieldNum - no <b>result</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	rIndex = s->intval;
    else rIndex = 0;
	
    /* Recover the passed connection index				*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_fieldNum - no <b>connection index</b>");
	return;
	}
    if (s->strval)
	cIndex = s->intval;
    else
	cIndex = 0;

    /* Check the connection index   					    */
    /*======================================================================*/
    if (cIndex < 0 || cIndex >= _list.maxC)
	{
	Error("(Illustra) Bad connection index in mi_numRows");
	Push("-1", LNUMBER);
	return;
	}

    /* Check the result index   					    */
    /*======================================================================*/
    if (rIndex < 0 || rIndex >= _list.conn[cIndex]->numR)
	{
	Error("(Illustra) Bad result index in mi_numRows");
	Push("-1", LNUMBER);
	return;
	}
   
    fNum = _miFieldNumber(cIndex, rIndex, fName);
    if (fNum < 0)
    	{
    	Error("(Illustra) Can't find field-name %s", fName);
    	Push("-1", LNUMBER);
    	}
    else
    	{
    	sprintf(number, "%d", fNum);
    	Push(number, LNUMBER);
    	}
    	
#else
    Pop();
    Pop();
    Error("No illustra support in this PHP module");
#endif
    }
 
/******************************************************************************
 * Status   : Public
 * Function : Return the name of the passed field number
 *****************************************************************************/
void MIfieldName(void)
    {
#ifdef ILLUSTRA
    Stack		*s;		    /* PHP Stack system		*/
    int			cIndex;		    /* Passed connection	*/
    int			fNum;		    /* The field number		*/
    int			rIndex;		    /* The result number	*/
    char		*fName;		    /* The field name		*/
    char    	    	*tmp;	    	    /* Temporary pointer    	*/
    
    /* Recover the field number						*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_fieldName - no <b>field</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	fNum = s->intval;
    else
	{
	Error("(Illustra) Can't find a valid field number in mi_fieldName");
	Push("-1", LNUMBER);
	return;
	}
    
    /* Recover the result number					*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_fieldName - no <b>result</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	rIndex = s->intval;
    else
	{
	Error("(Illustra) Can't find a valid result index in mi_fieldName");
	Push("-1", LNUMBER);
	return;
	}
	
    /* Recover the passed connection index				*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_fieldName");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	cIndex = s->intval;
    else
	cIndex = 0;

    /* Check the connection index   					    */
    /*======================================================================*/
    if (cIndex < 0 || cIndex >= _list.maxC)
	{
	Error("(Illustra) Bad connection index in mi_numRows");
	Push("-1", LNUMBER);
	return;
	}

    /* Check the result index   					    */
    /*======================================================================*/
    if (rIndex < 0 || rIndex >= _list.conn[cIndex]->numR)
	{
	Error("(Illustra) Bad result index in mi_numRows");
	Push("-1", LNUMBER);
	return;
	}
       
    /* Check the result is valid in the context of this connection	*/
    /*==================================================================*/
    tmp = _miFieldName(cIndex, rIndex, fNum);
    if (tmp == NULL)
	{
	Error("(Illustra) Can't find field %d in mi_fieldName", fNum);
	Push("-1", LNUMBER);
	return;
	}

    /* Return the field name						*/
    /*==================================================================*/
    fName = estrdup(0, tmp);
    if (fName == NULL)
	{
	Error("(Illustra) Out of memory in mi_fieldName");
	Push("-1", LNUMBER);
	return;
	}

    Push(fName, STRING);

#else
    Pop();
    Pop();
    Error("No illustra support in this PHP module");
#endif
    }    

 
/******************************************************************************
 * Status   : Public
 * Function : Return the number of fields
 *****************************************************************************/
void MInumFields(void)
    {
#ifdef ILLUSTRA
    Stack		*s;		    /* PHP manipulation stack	    */
    int			rIndex;	    	    /* Passed-in index of result    */
    int			cIndex;		    /* Passed connection	    */
    int     	    	numFields;  	    /* Number of fields     	    */
    char		temp[16];	    /* Passed-out number	    */
    
    /* Retrieve the result argument					    */
    /*======================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_numrows - no <b>result index</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	rIndex = s->intval;
    else rIndex = 0;
	
    /* Recover the passed connection index				*/
    /*==================================================================*/
    s = Pop();
    if (!s)
	{
	Error("(Illustra) Stack error in mi_fieldName - no <b>connection index</b>");
	Push("-1", LNUMBER);
	return;
	}
    if (s->strval)
	cIndex = s->intval;
    else
	cIndex = 0;

    /* Check the connection index   					    */
    /*======================================================================*/
    if (cIndex < 0 || cIndex >= _list.maxC)
	{
	Error("(Illustra) Bad connection index in mi_numRows");
	Push("-1", LNUMBER);
	return;
	}

    /* Check the result index   					    */
    /*======================================================================*/
    if (rIndex < 0 || rIndex >= _list.conn[cIndex]->numR)
	{
	Error("(Illustra) Bad result index in mi_numRows");
	Push("-1", LNUMBER);
	return;
	}
    
    /* Get the number of columns   					    */
    /*======================================================================*/
    numFields = _list.conn[cIndex]->result[rIndex]->numC;
 
    sprintf(temp, "%d", numFields);
	Push(temp, LNUMBER);
#else
	Pop();
	Push("-1", LNUMBER);
	Error("No illustra support in this PHP module");
#endif    
    }

/******************************************************************************
 * Status   : Private 
 * Function : Maintain a list of active connections
 *****************************************************************************/
#ifdef ILLUSTRA
int _miAddConn(MI_CONNECTION *newConn, char *dbase)
    {
    miConn *conn = NULL;
    
    /* Check to see if there is space for more connections		*/
    if (_list.numC == _list.maxC)
	{
	if (_list.maxC == 0)
	    {
	    /* Do an initial reserve operation				*/
	    if ((_list.conn = (miConn **) emalloc(0,5*sizeof(miConn*))) == NULL)
		{
		Error("(Illustra) Can't allocate connection space");
		return -1;
		}
	    _list.maxC = 5;
	    _list.numC = 0;
	    }
	else
	    {
	    int i;
	    /* Increase the memory allocation				*/
	    miConn **new;
	    int quant = 5 + _list.maxC;
	    if ((new = (miConn **) emalloc(0,quant*sizeof(miConn*))) == NULL)
		{
		Error("(Illustra) Can't re-allocate connection space");
		return -1;
		}
	    for (i=0; i<_list.maxC; i++)
		new[i] = _list.conn[i];
	    _list.conn = new;		    /* MEMORY LEAK		*/
	    _list.maxC += 5;
	    }
	}

    /* Add a new connection structure					*/
    if ((conn = emalloc(0,sizeof(miConn))) == NULL)
	Error("(Illustra) Out of memory while alloc()ing new connection");
    conn->numR = conn->maxR = 0;
    conn->result = NULL;
    conn->this = newConn;
    conn->dbName = estrdup(0, dbase);
    
    _list.conn[_list.numC] = conn;
    _list.numC ++;

    return _list.numC -1;
    }


/******************************************************************************
 * Status   : Private 
 * Function : Delete an active connection
 *****************************************************************************/
int _miDelConn(int cIndex)
    {
    int i;
    
    if (cIndex < 0 || cIndex >= _list.numC)
	{
	Error("(Illustra) Attempt to close non-existant connection");
	return -1;
	}

    if (cIndex < _list.numC -1)
    	{
    	/* We have to move the other connections around     	    	    */
    	for (i=cIndex; i< _list.numC-1; i++)
    	    _list.conn[i] = _list.conn[i+1];
   	}
   
    /* Subtract 1 from the list of connections	    	    	    	    */
    _list.numC --;	    	/* MEMORY LEAK	    	    	    	    */
    }


/****************************************************************************
 * Status   : Private 
 * Function : Entry point for fetching results
 ****************************************************************************/
mi_integer _miAddResult(int cIndex)
    {
    mi_integer count;			/* Number of rows in total	*/
    mi_integer result;			/* Answer to 'any more rows ?'	*/
    char *cmd;				/* The SQL command name		*/
    int rowIdx =0;			/* The row index		*/
    int i;  	    	    	    	/* Loop variable    	    	*/
    
    /* Create a shorthand to the connection structure	    	    	*/
    miConn *conn = _list.conn[cIndex];
    
    /* Create a new result structure	    	    	    	    	*/
    /* First check to see if we need to reserve more space  	    	*/
    if (conn->numR == conn->maxR)
    	{
    	/* Create a list with more space for results in it	    	*/
    	miResult **results;
    	int numR = conn->maxR +5;
	if ((results = (miResult **) emalloc(0,numR*sizeof(miResult*))) == NULL)
	    {
	    Error("(Illustra) Can't allocate result space");
	    return -1;
	    }
    	
    	/* Reset the number of structures we can use	    	    	*/
    	conn->maxR = numR;
    	
    	/* Cope with any previous connections	    	    	    	*/
    	if (conn->numR != 0)
    	    {
    	    for (i=0; i<numR; i++)
    	    	{
    	    	if (i< conn->numR)
    	    	    results[i] = conn->result[i];
    	    	else
    	    	    {
    	    	    results[i] = emalloc(0,sizeof(miResult));
    	    	    if (results[i] == NULL)
    	    	    	{
    	    	    	Error("(Illustra) can't allocate result struct");
    	    	    	return (-1);
    	    	    	}
    	    	    else
    	    	    	memset(results[i], 0, sizeof(miResult));
    	    	    }
    	    	}
    	    }
    	else
    	    {
    	    for (i=0; i<numR; i++)
    	    	{
    	    	results[i] = emalloc(0,sizeof(miResult));
    	    	if (results[i] == NULL)
    	    	    {
    	    	    Error("(Illustra) can't allocate result struct");
    	    	    return (-1);
    	    	    }
    	    	else
    	    	    memset(results[i], 0, sizeof(miResult));
    	    	}
    	    }
    	    
    	conn->result = results;     /* MEMORY LEAK  	    	    	*/
	}
    	
    while ((result = mi_get_result(conn->this)) != MI_NO_MORE_RESULTS)
	{
	switch (result)
	    {
	    case MI_ERROR:
		Error("(Illustra): Could not get results");
		return -1;
	    case MI_DDL:
		break;
	    case MI_DML:
		/* What kind of command was it ?			*/
		cmd = mi_result_command_name(conn->this);
		
		/* Get the number of rows affected by the command	*/
		if ((count = mi_result_row_count(conn->this)) == MI_ERROR)
		    {
		    Error("(Illustra): Cannot get row count\n");
		    return -1;
		    }
		else
    	    	    {
#if DEBUG
		    Error("(Illustra): %d %s affected by %s command\n", 
			    count, (count >1)? "rows": "row", cmd);
#endif
		    }
		break;
		
	    case MI_ROWS:
		_miGetRow(cIndex, conn->numR);
		rowIdx ++;
		break;
		
	    default:
		Error("(Illustra): unknown results");
	    }
	}

    /* Store the number of returned rows    	    	    	    	*/
    conn->result[conn->numR]->numR = count;

    /* Now add the new connection structure in (increment num Results)	*/
    conn->numR ++;

    return conn->numR-1;
    }

/************************************************************************
 * Status   : Private
 * Function : Row retrieval function
 ************************************************************************/
mi_integer _miGetRow(int cIndex, int resIndex)
    {
    MI_ROW *row;		    /* A row of returned data		*/
    MI_ROW_DESC *rowDesc;	    /* A description of the row		*/
    mi_integer error;		    /* Type of error encountered	*/
    mi_integer numCols;		    /* Number of columns in this row	*/
    char *colName;		    /* Column name			*/
    mi_integer colLen;		    /* Column length			*/
    char *colVal;		    /* Column value			*/
    int i;			    /* Loop variable			*/
    miRow *rPtr;    	    	    /* Pointer to miRow structure   	*/
    miRow *pPtr;    	    	    /* Pointer to previous miRow    	*/
        
    /* Get the connection object    	    	    	    	    	*/
    miConn *conn = _list.conn[cIndex];
    
    /* Get the row descriptor                                           */
    rowDesc = mi_get_row_desc_without_row(conn->this);
    
    /* Get the number of columns in the row                             */
    numCols = mi_column_count(rowDesc);
    
    /* Check to see if we need to create a new result structure     	*/
    if (conn->result[resIndex] == NULL)
    	{
    	if ((conn->result[resIndex] = emalloc(0, sizeof(miResult*))) == NULL)
    	    {
    	    Error("(Illustra) Can't allocate new result structure");
    	    return -1;
    	    }
    	else
    	    memset(conn->result[resIndex], 0, sizeof(miResult*));
    	}
    
    /* Create a row pointer to act as a cursor	    	    	    	*/
    rPtr = conn->result[resIndex]->rows;
    pPtr = NULL;
    
    /* 'curse' through until we get to the end of the list  	    	*/
    if (rPtr != NULL)
        {
        while (rPtr->next != NULL)
    	    rPtr = rPtr->next;

    	pPtr = rPtr;
   	rPtr = rPtr->next;
    	}
    	
    /* Preserve the column names					*/
    if ( conn->result[resIndex]->names == NULL)
	{
	conn->result[resIndex]->numC = numCols;
	conn->result[resIndex]->names = emalloc(0, numCols* sizeof(char*));
	if (conn->result[resIndex]->names == NULL)
	    {
	    Error("(Illustra) can't alloc names table");
	    return 0;
	    }    
	for (i=0; i<numCols; i++)
	    {
	    colName = mi_column_name(rowDesc, i);
	    conn->result[resIndex]->names[i] = estrdup(0, colName);
	    }
	}
    
    /* For each row...							*/
    while ((row=mi_next_row(conn->this, &error)) != NULL)
	{
	/* Create a new row object					*/
	rPtr = (miRow*) emalloc(0, sizeof(miResult));
	if (rPtr == NULL)
	    Error("(Illustra) Can't reserve new row storage object");
	    
	/* Populate the new object with sensible values			*/
	rPtr->numC = numCols;
	rPtr->next = NULL;
	if (pPtr != NULL)
	    pPtr->next = rPtr;
	else
	    conn->result[resIndex]->rows = rPtr;
	    
	/* Create storage space for the actual data			*/
	rPtr->row = emalloc(0, numCols * sizeof(char*));
	if (rPtr->row == NULL)
	    Error("(Illustra) can't alloc() row storage");
		    
	/* For each column ...						*/
	for (i=0; i<numCols; i++)
	    {
	    /* Put the column value into colVal				*/
	    switch (mi_value(row, i, &colVal, &colLen))
		{
		case MI_ERROR:
		    Error("(Illustra) Can't get cell value");
		    return -1;
		case MI_NULL_VALUE:
		    colVal = "Null";
		    break;
		case MI_NORMAL_VALUE:
		    break;
		default:
		    Error("(Illustra) Unknown cell value type");
		    return -1;
		}
	    
	    /* Store the column value					*/
	    rPtr->row[i] = estrdup(0, colVal);
	    }
	
	/* Advance the rPtr cursor  	    	    	    	    	*/
	pPtr = rPtr;
	rPtr = rPtr->next;
	}
	
    if (error == MI_ERROR)
	Error("(Illustra): Cannot get row from DB");

    return 0;
    }

 
/******************************************************************************
 * Status   : Private
 * Function : Return the number of the passed field name
 *****************************************************************************/
int _miFieldNumber(int cIndex, int rIndex, char *name)
    {
    int i;
    
    /* Get the names	    		    	    	    	    	*/
    char **names = _list.conn[cIndex]->result[rIndex]->names;
    int numNames = _list.conn[cIndex]->result[rIndex]->numC;
    
    for (i=0; i<numNames; i++)
    	if (strcasecmp(name, names[i]) == 0)
    	    return i;
    	    
    return -1;
    }

/******************************************************************************
 * Status   : Private
 * Function : Return the name of the passed field number
 *****************************************************************************/
char * _miFieldName(int cIndex, int rIndex, int num)
    {
    /* Get the names	    		    	    	    	    	*/
    char **names = _list.conn[cIndex]->result[rIndex]->names;
    int numNames = _list.conn[cIndex]->result[rIndex]->numC;
    
    if (num<0 || num>=numNames)
    	return NULL;

    return names[num];
    }

#endif /* ILLUSTRA							*/
