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
   | Authors: MA_Muquit@fccc.edu                                          |
   |          Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
*/
/* $Id: sybsql.c,v 1.29 1997/12/31 15:56:51 rasmus Exp $ */
#ifndef MSVC5
#include "config.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "sybsql.h"

#if HAVE_SYBASE

function_entry sybase_old_functions[] = {
	{"sybsql_seek",			php3_sybsql_seek,			NULL},
	{"sybsql_exit",			php3_sybsql_exit,			NULL},
	{"sybsql_dbuse",		php3_sybsql_dbuse,			NULL},
	{"sybsql_query",		php3_sybsql_query,			NULL},
	{"sybsql_isrow",		php3_sybsql_isrow,			NULL},
	{"sybsql_result",		php3_sybsql_result,			NULL},
	{"sybsql_connect",		php3_sybsql_connect,		NULL},
	{"sybsql_nextrow",		php3_sybsql_nextrow,		NULL},
	{"sybsql_numrows",		php3_sybsql_numrows,		NULL},
	{"sybsql_getfield",		php3_sybsql_getfield,		NULL},
	{"sybsql_numfields",	php3_sybsql_numfields,		NULL},
	{"sybsql_fieldname",	php3_sybsql_fieldname,		NULL},
	{"sybsql_result_all",	php3_sybsql_result_all,		NULL},
	{"sybsql_checkconnect",	php3_sybsql_checkconnect,	NULL},
	{NULL, NULL}
};

php3_module_entry sybase_old_module_entry = {
	"Sybase SQL - old", sybase_old_functions, NULL, NULL, php3_rinit_sybsql, NULL, php3_info_sybsql, 0, 0, 0, NULL
};

#include <sybfront.h>
#include <sybdb.h>
#include <syberror.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h> /* getlogin() */
#endif
#include <ctype.h>
#include <stdarg.h> /* var args */
#include "list.h"
#include "main.h"
#include "head.h"

#ifdef  iswhite
# undef iswhite
#endif
#define iswhite(c)  (c == ' ' || c == '\t' || c == '\n')

#if HAVE_SYBASE  /* { */
/*
** macros from symfmt
*/
#define coltype(j) dbcoltype(dbproc,j)
/* #define putchr(c) putc(c, stdout) */
/* #define putstr(s) fputs(s, stdout) */
#define intcol(i) ((int) *(DBINT *) dbdata(dbproc,i))
#define smallintcol(i) ((int) *(DBSMALLINT *) dbdata(dbproc,i))
#define tinyintcol(i) ((int) *(DBTINYINT *) dbdata(dbproc,i))
#define anyintcol(j) (coltype(j)==SYBINT4?intcol(j):(coltype(j)==SYBINT2?smallintcol(j):tinyintcol(j)))
#define charcol(i) ((DBCHAR *) dbdata(dbproc,i))
#define floatcol(i) ((float) *(DBFLT8 *) dbdata(dbproc,i))

/*
** globals in this file
*/
THREAD_LS static DBPROCESS *dbproc;
THREAD_LS static FILE *bitbucket;

/*
** function prototypes for private functons
*/
static DBPROCESS *_php3_mdb_init(char *,char *,char *,char *,char *);
static int _php3_mdb_msg_handler(DBPROCESS *,DBINT,int,int,char *,char *,char *, DBUSMALLINT);
static int _php3_mdb_err_handler(DBPROCESS *,int,int,int,char *,char *);
static char *_php3_col2char(int);
static void _php3_sybsql_check_connection(void);
static int _php3_submit_query(char *);
static char *_php3_subst_syb_col(char *);

/*
** initialize sybase related stuff
*/
int php3_rinit_sybsql(INITFUNCARG)
{
	dbproc=(DBPROCESS *) NULL;
	bitbucket=(FILE *) NULL;
	return SUCCESS;
}

/*
 *  sybsql_connect() -   connect to the sybase server
 *
 *  Description:
 *
 *	  This function opens a network connection to a sybase server by
 *	  calling the local _php3_mdb_Init() function. It calls the function with
 *	  NULL arguments. When mdb_init() function is called with NULL 
 *	  arguments, it will try to collect the necessary information from
 *	  some environment variable. 
 *
 *	  All necessary parameters are obtained from the environment.
 *	  The following environemnt variables must be set before calling this
 *	  function.
 *
 *	  DSQUERY - the alias of sybase server as defined in sybase interface
 *				file.
 *	  DBUSER  - connect to server as this user
 *	  DBPW	- password of the user
 *
 *  Input Parameters:
 *	  type	identifier  description
 *
 *	  char	*hostname   name of the sybase server. Note, this is not the
 *						  FQDN of the host, it's how it's defined in sybase
 *						  interface file.
 *
 *  Limitations and Comments:
 *
 *	  DSQUERY environment variable must be set before calling the
 *	  function, otherwise sybase API call will fail.
 *
 */

static void _php3_sybsql_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{
	RETCODE rc;
	char rbuf[10];

	*rbuf='\0';

	dbproc=_php3_mdb_init(
			NULL,   /* app name */
			NULL,   /* user */
			NULL,   /* password */
			NULL,   /* server */
			NULL	/* database */
	);

	if (dbproc == (DBPROCESS *) NULL) {
		php3_error(E_WARNING,"Failed to open connection to sybase server");
		RETURN_FALSE;
	}
	/*
	** turn on row buffering
	** buffer maximum 10,000 rows, I think it's enough
	*/
	(void) sprintf(rbuf,"%i",10000);
	rc=dbsetopt(dbproc,DBBUFFER,rbuf,-1);
	if (rc == FAIL)
	{
		php3_error(E_WARNING,"Failed to turn on row buffering in SybsqlConnect");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

void php3_sybsql_connect(INTERNAL_FUNCTION_PARAMETERS)
{
    _php3_sybsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}
 
void php3_sybsql_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
    _php3_sybsql_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}

/*
 *  mdb_init - actually connects to the sybase server
 *
 *  Description:
 *
 *	  This function actually makes connection to the sybase 
 *	  server by calling sybase DB library API calls. It's nothing but a
 *	  nice wrapper around Syabase API calls.
 *
 *  Input Parameters:
 *	  type	identifier  description
 *
 *	  char	*appname	app name to register with server
 *	  char	*user	   connect as this user
 *	  char	*pass	   password of the user
 *	  char	*server	 alias name of the server
 *	  char	*database   database to use
 *
 *  Return Values:
 *	  value			   description
 *	  DBPROCESS *dbproc   a pointer to DBPROCESS struct if succeeds
 *	  NULL				if fails
 *
 *  Limitations and Comments:
 *
 *	 if the  the function is called with NULL arguments, it will try
 *	 to collect the necessary information from some environment variables.
 *	 The following environemnt variables must be set before calling this
 *	 function without arguments.
 *
 *	 APPNAME - the name of the app to register with sybase server.
 *			   (good to set, not necessary)
 *  
 *	 DSQUERY - the alias of sybase server as defined in sybase interface
 *			   file. (must be set)
 *	 DBUSER  - connect to server as this user (first try),
 *			   if DBUSER is not set, it will look for USER, if it fails,
 *			   it will use getlogin(). if getlogin() fails, outta here.
 *			   (must be set)
 *
 *	 DBPW	- password of the user
 *			   (must be set)
 *
 *	 DB	  - database to use (not needed unless you want to set to the
 *			   database right away).
 *
 *	 NOTE: the string length of each of the arguments can be maximum 80
 *		   character long. If length > 80, it will be truncated.
 *
 */


static DBPROCESS *_php3_mdb_init(
	char *appname,	  /* name of the app to register with sybase server */
	char *user,		 /* connect as user */
	char *pass,		 /* password of user */
	char *server,	   /* alias of sybase server as in interface file */
	char *database	  /* database to use */
	)	 
{
	LOGINREC *login;
	DBPROCESS *dp;
	char loc_appname[80], loc_user[80], loc_pass[80], loc_server[80], loc_database[80];

	*loc_appname='\0';
	*loc_user='\0';
	*loc_pass='\0';
	*loc_server='\0';
	*loc_database='\0';
	dp=(DBPROCESS *) NULL;
	login=(LOGINREC *) NULL;

	if (appname == (char *) NULL)
	{
		appname=getenv("APPNAME");
		if (appname == (char *) NULL)
		{
			(void) strcpy(loc_appname,"mdb_init)");
		}
	}
	/*
	** if user name is not passed, we'll try to get it from the env
	** DBUSER.
	*/
	if (user == (char *) NULL)
	{
		user=getenv("DBUSER");
		/*
		** check if USER env variable is set
		*/
		if (user == (char *) NULL)
			user = getenv("USER");
		if (user == (char *) NULL)
		{
			/*
			** try getlogin()
			*/
			user=getlogin();
			if (user == (char *) NULL)
			{
				php3_error(E_WARNING," Unable to get user name.");
				return ((DBPROCESS *) NULL);
			}
		}

	}
	(void) strncpy(loc_user,user,79);

	/*
	** get the sybase server name
	*/
	if (server == (char *) NULL)
	{
		server = getenv ("DSQUERY");
		if (server == (char *) NULL)
		{
			php3_error(E_WARNING,"mdb_init: unable to get server name.");
			return ((DBPROCESS *) NULL);
		}
	}
	(void) strncpy(loc_server,server,79);
	
	/*
	** check for password
	*/
	if (pass == (char *) NULL)
	{
		pass=getenv("DBPW");
		if (pass == (char *) NULL)
		{
			php3_error(E_WARNING,"mdb_init: unable to get password!");
			return ((DBPROCESS *) NULL);
		}
	}
	(void) strncpy(loc_pass,pass,79);

	/*
	** copy the database name to the local space if supplied
	** if not we'll continue anyway to get into default database
	*/
	if (database == (char *) NULL)
		database=getenv("DB");

	if (database != (char *) NULL)
	{
		(void) strncpy (loc_database,database,79);
	}

	/*
	** initialize
	*/
	dbinit();
	login=dblogin();

	/*
	** install the callback function for handling server messages
	*/
	dbmsghandle((MHANDLEFUNC)_php3_mdb_msg_handler);
	dberrhandle((EHANDLEFUNC)_php3_mdb_err_handler);

	/*
	** TODO: specify the name/location of interface file
	*/

	/*
	** Set the username in the LOGINREC structure
	*/
	DBSETLUSER (login,loc_user);

	/*
	** set the server password to the LOGINREC structure
	*/
	DBSETLPWD (login,loc_pass);
	DBSETLAPP (login,loc_appname);

	/*
	** register if appname is set. the appname is used by the sybase
	** server in syprocesses table to identify the process. it's good to
	** register the appname, but not needed
	*/
	if (*loc_appname != '\0')
		DBSETLAPP(login,appname);

	/*
	** ready to open the database
	*/
	dp=dbopen(login,loc_server);
	if (dp == (DBPROCESS *) NULL)
	{
		php3_error(E_WARNING,"Failed in dbopen()..");
 		dbloginfree(login);
		return ((DBPROCESS *) NULL);
	}

	/*
	** use the database if supplied
	*/

	/*
	**  SCB  7/2/1997
	**  I think this will do the trick
	**  to get rid of login stuff after the dbopen succeeded..
	*/
	dbloginfree(login);

	if (database != (char *) NULL)
	{
		if (dbuse(dp,loc_database) == FAIL)
		{
			php3_error(E_WARNING,"Failed to open database:%s",loc_database);
			dbclose(dp);
			return ((DBPROCESS *) NULL);
		}
	}

	return (dp);
}

/*
** display messages from sql server
*/
static int _php3_mdb_msg_handler(DBPROCESS *dbproc,DBINT msgno,int msgstate,
	int severity,char *msgtext,char *srvname,char *procname,
	DBUSMALLINT line)
{
	/*
	php3_error(E_WARNING, "Message from SQL server %s: (Msg %ld, Level %d, State %d\n",
		srvname,msgno,severity,msgstate);
	*/
	return (0);
}

/*
** display Dblib error messages
*/
static int _php3_mdb_err_handler (DBPROCESS *dbproc,int severity,int dberr,
	int oserr,char *dberrstr,char *oserrstr)
{
	php3_error(E_WARNING,"DBlib error (Severity %d):\n\t%s\n", severity,dberrstr);
	return(0);
}
	  

/*
 *  sybsql_dbuse - use a requested database
 *
 *  Description:
 *
 *	  This function calls sybase API dbuse() to switch to a database
 *
 *  Limitations and Comments:
 *
 *	  All necessary things are obtained from php's internal stack. 
 *	  return code is pushed back to stack as well.
 *
 */

void php3_sybsql_dbuse(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *db;
	RETCODE rc;

	if (getParameters(ht, 1, &db)==FAILURE) {
		RETURN_FALSE;
  	}
	if (dbproc == (DBPROCESS *) NULL)
	{
		php3_error(E_WARNING,"No connection to sybase server in sybsql_dbuse");
		RETURN_FALSE;
	}
	convert_to_string(db);

	/*
	** call sybase API
	*/
	rc=dbuse(dbproc,db->value.strval);

	if (rc == FAIL)
	{
		php3_error(E_WARNING,"Failed to use database: %s",db->value.strval);
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

/*
 *  sybsql_query - submits query to sybase server
 *
 *  Description:
 *
 *	  This function sends the SQL query to sybase server.
 *
 *  Limitations and Comments:
 *
 *	  All necessary things are obtained from php's internal stack. 
 *	  return code is pushed back to stack as well.
 *
 *	  this funcion will work if a connection to sybase server is 
 *	  already established.
 */

void php3_sybsql_query(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *q;
	int rc;

	_php3_sybsql_check_connection();
	if(getParameters(ht, 1, &q)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_string(q);
	rc=_php3_submit_query(q->value.strval);
	if (rc == -1) RETURN_FALSE;
	RETURN_TRUE;
}

/*
** checks if a connecton to sybase server is establised. 
*/
static void _php3_sybsql_check_connection(void)
{
	if (dbproc == (DBPROCESS *) NULL)
	{
		php3_error(E_WARNING,"No connecton to sybase server");
	}
	return;
}

/* public version */
void php3_sybsql_checkconnect(INTERNAL_FUNCTION_PARAMETERS) {
	if (dbproc == (DBPROCESS *) NULL) RETURN_FALSE;
	RETURN_TRUE;
}

/*
 *  submit_query - submits sql query to sybase server
 *
 *  Description:
 *
 *	  this function sends the SQL request to server.
 *
 */

static int _php3_submit_query(char *query)
{
	RETCODE rc;
	int n;

	rc=dbcmd(dbproc,query);
	if (rc == FAIL)
	{
		php3_error(E_WARNING,"Failed to add command %s to DBPROCESS command buffer",query);
		return(-1);
	}

	/*
	** dispatch command
	*/
	if ((dbsqlexec(dbproc) != SUCCEED) || (dbresults(dbproc) != SUCCEED))
	{
		php3_error(E_WARNING,"Failed in dbqlexec() or dbresults()");
		return (-1);
	}
	
	n=dbcount(dbproc);
	if (n < 0) n=0;
	return (n);
}

/*
 *  sybsql_isrow - indicates if currect sql command returned any rows
 *
 *  Description:
 *
 *	  this function calls sybase API routine to determine if the current
 *	  SQL command returned any rows.
 *
 *  Limitations and Comments:
 *
 *	  NOTE: this functin must be called after: 
 *		  . a successful connection is made to sybase server. 
 *		  . a sql request is dispatched successfully
 *		  . dbresult is called
 *	  Should never be called after dbnextrow()
 *
 */

void php3_sybsql_isrow(INTERNAL_FUNCTION_PARAMETERS)
{
	RETCODE nrows;

	_php3_sybsql_check_connection();
	nrows=DBROWS(dbproc);
	if (nrows != FAIL) RETURN_TRUE;
	RETURN_FALSE;
}


/*
 *  sybsql_print_a_row - print the requested fields of the current row
 *
 *  Description:
 *
 *	  This function prints the requestd fields of the current row 
 *	  in the current row buffer.
 */
void php3_sybsql_print_a_row(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *format;
	char *val;

	if (getParameters(ht, 1, &format)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_string(format);
	val=_php3_subst_syb_col(format->value.strval);
	if(php3_header(0,NULL))
		PUTS(val); 
	efree(val);
}

/*
 *  sybsql_nextrow - reads the next result row
 *
 *  Limitations and Comments:
 *
 *	  Orininarily, the next row is read directly from server.  If DBBUFFER
 *	  option is turned on (as we do), and rows have been read out of order
 *	  by calling dbgetrow(), the next row is read instead of a linked list
 *	  of buffer rows.
 *
 *	  NOTE: dbresults() must return SUCCED before calling dbnext row
 *
 *	  It pushes 1 on the stack as long as there'r rows left to read. If
 *	  there'r no more rows left to read or in case of error it will push
 *	  0 on the stack.
 *
 */

void php3_sybsql_nextrow(INTERNAL_FUNCTION_PARAMETERS)
{
	STATUS st;

	_php3_sybsql_check_connection();
	st=dbnextrow(dbproc);
	if(st!=NO_MORE_ROWS && st!=FAIL) RETURN_TRUE;
	RETURN_FALSE;
}


/*
 *  subst_syb_col - substitutes a field with value at this field
 *
 *  Description:
 *
 *	  this function substitues field/s of the current row with the value
 *	  at the field/s.
 *
 *  Input Parameters:
 *	  type	identifier  description
 *
 *	  char	*format	 column/s to print
 *	  
 *	  example of format:  "this is col1 @1,<td>@2</td> col10 @10"
 *
 *  Return Values:
 *	  value	   description
 *
 *	  char *buf   pointer to formatted output, ready to print
 *
 *  Limitations and Comments:
 *
 *	  the maximum number of characters in the result can be 4096
 *	  characters long
 *
 */
static char *_php3_subst_syb_col(char *str)
{
	register char *cp1, *cp2, *var1, *var2;
	char *val, *buf, varBuf[80];
	int offset, count;

	/*
	** malloc for buf
	*/
	buf=emalloc(4096);
	count=offset=0;
	cp1=str;
	cp2=buf;
	memset(buf,'\0',4096);

	while (*cp1)
	{
		switch (*cp1)
		{
			case '@':
			{
				/*
				** skip past @
				*/
				cp1++;
				/*
				** spaces are allowed between @ and digits, someone 
				** might do this mistakenly
				*/
				while (iswhite(*cp1)) cp1++;

				/*
				** now this thing must be a digit or exit
				*/
				if (!isdigit(*cp1))
				{
					php3_error(E_WARNING,"Bad Column number: %c",*cp1);
				}

				var1=cp1;
				var2=cp1;
				/*
				** copy the column number/s
				*/
				while (isdigit(*var2)) var2++;

				memset(varBuf,'\0',sizeof(varBuf));
				(void) strncpy(varBuf,var1,var2-var1);

				/*
				** check if the column offset is more than the number of
				** columns in the row buffer
				*/
				count=dbnumcols(dbproc);
				/*
				** sybase col starts with 1, but user can start from 0,
				** so we internally increment it by 1
				*/
				offset=atoi(varBuf)+1;

				if (offset > count)
				{
					php3_error(E_WARNING,"Column no %d is bigger than max no of cols:%d", offset,count);
				}

				/*
				** now get the value from that offset
				*/
				val=_php3_col2char(offset);

				if (val)
				{
					(void) strcpy(cp2,val);
					cp2 += strlen(val);
					efree(val);
				}
				cp1=var2;
				break;
			}

			default:
			{
				*cp2++ = *cp1++;
				break;
			}
		}
	}
	return (buf);
}


/*
 *  _php3_col2char - converts any column data to an array of char
 *
 *  Description:
 *
 *	  this function converts any column data in the current row buffer
 *	  to an array of characters and returns a pointer to it.
 *
 */

static char *_php3_col2char(int j)
{
	char buf[256];

	if (dbdatlen(dbproc,j) == 0) return ((char *) NULL);

	switch (coltype(j))
	{
		case SYBINT2:
		case SYBINT4:
		{	
			sprintf(buf,"%i",anyintcol(j));
			return(estrdup(buf));
		}
 
		case SYBCHAR:
		case SYBTEXT:
		{
			int len;
		
			len=dbdatlen(dbproc,j);
			while (len && charcol(j)[len-1] == ' ') len--;
			if(len+1>255) {
			  {
				char *b;

				b=emalloc(len+1);
				sprintf(b,"%.*s",len,charcol(j));
				return(b);
			  }
			}
			sprintf(buf,"%.*s",len,charcol(j));
			return(estrdup(buf));
		}
		 
		case SYBFLT8:
		case SYBREAL:
		{
			sprintf(buf,"%-.*f",2,floatcol(j));
			return(estrdup(buf));
		}

		default:
		{
			if (dbwillconvert(coltype(j),SYBCHAR))
			{
				dbconvert(NULL,coltype(j),dbdata(dbproc,j), dbdatlen(dbproc,j),SYBCHAR,buf,-1);
				return (estrdup(buf));
			}
			else
			{
				php3_error(E_WARNING,"datatype conversion problem: can't convert Sybase datatype %i to char.\n", coltype(j));
				return ((char *) NULL);
			}
		}
	}
}

/*
 *  SybsqlNumRows - number of rows in a result
 *
 *  Description:
 *
 *	  this function returns the number of rows in the current result
 *	  buffer.
 *
 *  Side Effects:
 *
 *	  after this call the row counter always points to the very first
 *	  row (by calling dbsetrow()).
 *
 *  Limitations and Comments:
 *
 *	  when this function is called, it will seek to the first row right
 *	  away, then it will call dbnextrow() until there'r no more rows and
 *	  incrment a internal counter to find out the number of rows in the
 *	  result buffer. Then it points back to the very first row. Therefore,
 *	  after calling this function row counter always points to the very
 *	  first row. It's ugly but I don't know any other way at this time.
 *
 */

void php3_sybsql_numrows(INTERNAL_FUNCTION_PARAMETERS)
{
	int nrows=0;
	char tmpbuf[20];

	*tmpbuf='\0';
	_php3_sybsql_check_connection();
	(void) dbsetrow(dbproc,(DBINT) 1);  /* 1 means first row */
	while (dbnextrow(dbproc) != NO_MORE_ROWS) { nrows++; }
	RETVAL_LONG(nrows);
	dbsetrow(dbproc,1);
	(void) dbresults(dbproc);
}


/*
 *  SybsqlPrintAllRows - print specified fields of all rows
 *
 */


void php3_sybsql_print_all_rows(INTERNAL_FUNCTION_PARAMETERS)
{
	if(php3_header(0,NULL))
		PUTS("Not implementd yet");
	RETURN_FALSE;
} 



/*
 *  sybsql_result - prints specific fields of the current row
 *
 *  Description:
 *
 *	  this function prints the specific field/s from the current row
 *	  buffer.
 *
 *  Limitations and Comments:
 *
 *	  never call sybsql_nextrow() while calling this function, this 
 *	  function will call dbnext().
 *
 */

void php3_sybsql_result(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *format;
	char *val;

	_php3_sybsql_check_connection();
	if (getParameters(ht, 1, &format)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_string(format);	
	/*
	** get the result and put in a variable
	*/
	if (dbnextrow(dbproc) != NO_MORE_ROWS)
	{
		val=_php3_subst_syb_col(format->value.strval);
		if(php3_header(0,NULL))
			PUTS(val);
		efree(val);
	}
	RETURN_TRUE;
}


/*
 *  sybsql_seek - sets a row to the current row
 *
 *  Description:
 *
 *	  this function sets the requested row number as the current row in
 *	  the row buffer. dbnextrow() will read this row.
 */

void php3_sybsql_seek(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *s;
	DBINT row;
	STATUS st;

	row=0;
	_php3_sybsql_check_connection();
	if (getParameters(ht, 1, &s)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_long(s);
	row=(DBINT)s->value.lval;
	if (row >=0 )
	{
		row++;
		st=dbsetrow(dbproc,row);
		switch(st)
		{
			case MORE_ROWS:
				RETURN_TRUE;	
				break;

			case NO_MORE_ROWS:
			case FAIL:
				RETURN_FALSE;
				break;
		}
		
	}
	else
	{
		php3_error(E_WARNING,"Row number can not be negative");
		RETURN_FALSE;
	}
}


/*
 *  sybsql_numfields - number of fields in a result
 *
 *  Description:
 *
 *	  this function returns the number of fields in a row in the current
 *	  row buffer.
 *
 */

void php3_sybsql_numfields(INTERNAL_FUNCTION_PARAMETERS)
{
	int ncols;

	_php3_sybsql_check_connection();
	ncols=dbnumcols(dbproc);	
	RETURN_LONG(ncols);
}


/*
 *  SybsqlFieldName - name of a column
 *
 *  Description:
 *
 *	  this function returns the name of a regular result column
 */

void php3_sybsql_fieldname(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *s;
	int col;
	char *colname;

	_php3_sybsql_check_connection();
    if (getParameters(ht, 1, &s)==FAILURE) {
        RETURN_FALSE;
    }
	convert_to_long(s);
	col=s->value.lval;
	if (col >=0 )
	{
		col++;  /* sybase column start at 1 */

		colname=dbcolname(dbproc,col);
		RETURN_STRING(colname);
	}
	else
	{
		php3_error(E_WARNING,"Column number can not be negative");
		RETURN_FALSE;
	}
}


/*
 *  SybsqlResultAll - prints all rows and columns
 *
 */

void php3_sybsql_result_all(INTERNAL_FUNCTION_PARAMETERS)
{
	int i, ncols;
	RETCODE rc;
	char *result;

	if(!php3_header(0,NULL)) return;
	_php3_sybsql_check_connection();
	PUTS("<table border cellspacing=0>\n");
	ncols=dbnumcols(dbproc);
	i=0;
	if (ncols > 0)
	{
		PUTS("<tr>\n");
		for (i=1; i <= ncols; i++)
		{
			PUTS("<td align=center bgcolor=\"cyan\"><b>");
			PUTS(dbcolname(dbproc,i));
			PUTS("</b></td>\n");
		}
		PUTS("</tr>\n");
	}

	while ((rc=dbnextrow(dbproc)) != NO_MORE_ROWS)
	{
		if (rc == BUF_FULL) 
		{
			php3_error(E_WARNING,"buffer full, discarding row 1\n");
			dbclrbuf(dbproc,1);
		}
		PUTS("<tr>\n");

		for (i=1; i <= ncols; i++)
		{
			result=_php3_col2char(i);
			if (result != (char *) NULL)
			{
				PUTS("<td><font size=-1>");
				PUTS(result);
				PUTS("</font></td>\n");
				efree(result);
			}
		}
		PUTS("</tr>\n");
	}
	PUTS("</table>\n");
}


/*
 *  SybsqlGetField() -   gets the value of a specific field
 *
 *  Description:
 *
 *	  this function gets the value of a specific field in the current row
 *
 *  Limitations and Comments:
 *
 *	  sybsql_nextrow() must be callied before calling this function.
 *	  sybsql_nextrow() must be called if row pointer needs to be 
 *	  increment, because this function only reads the current row in the
 *	  row buffer.
 */

void php3_sybsql_getfield(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *s;
	char *val;

	_php3_sybsql_check_connection();
    if (getParameters(ht, 1, &s)==FAILURE) {
        RETURN_FALSE;
    }
	val=_php3_subst_syb_col(s->value.strval);
	RETVAL_STRING(val);
	efree(val);
}

void php3_sybsql_exit(INTERNAL_FUNCTION_PARAMETERS)
{
	if(dbproc) {
		dbclose(dbproc);
		dbproc=NULL;
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

void php3_info_sybsql(void)
{
	PUTS("<pre>");
	PUTS(dbversion());
	PUTS("</pre>");
}

#endif
