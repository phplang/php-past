/***[sybsql.c]****************************************************[TAB=4]****\
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
/*
 *  Sybase SQL - WWW interface
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      This file contains the routine to talk to a Sybase server via PHP/FI
 *      cgi program or php/fi Apache server module. This routines uses
 *      Sybase Db library API calls.
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-15-96  first cut
 */

#include "php.h"
#include <stdlib.h>

#ifdef HAVE_SYBASE
#include <sybfront.h>
#include <sybdb.h>
#include <syberror.h>
#endif
#include "parse.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h> /* getlogin() */
#endif
#include <ctype.h>
#include <stdarg.h> /* var args */


#ifdef  iswhite
# undef iswhite
#endif
#define iswhite(c)  (c == ' ' || c == '\t' || c == '\n')

#ifdef HAVE_SYBASE  /* { */
/*
** macros from symfmt
*/
#define coltype(j) dbcoltype(dbproc,j)
#define putchr(c) putc(c, stdout)
#define putstr(s) fputs(s, stdout)
#define intcol(i) ((int) *(DBINT *) dbdata(dbproc,i))
#define smallintcol(i) ((int) *(DBSMALLINT *) dbdata(dbproc,i))
#define tinyintcol(i) ((int) *(DBTINYINT *) dbdata(dbproc,i))
#define anyintcol(j) (coltype(j)==SYBINT4?intcol(j):(coltype(j)==SYBINT2?smallintcol(j):tinyintcol(j)))
#define charcol(i) ((DBCHAR *) dbdata(dbproc,i))
#define floatcol(i) ((float) *(DBFLT8 *) dbdata(dbproc,i))

/*
** globals in this file
*/
static DBPROCESS
    *dbproc;
static FILE
    *bitbucket;

/*
** function prototypes for private functons
*/
#ifdef HAVE_SYBASE
static DBPROCESS
    *MdbInit(char *,char *,char *,char *,char *);

static int
    MdbmsgHandler (DBPROCESS *,DBINT,int,int,char *,char *,char *,
    DBUSMALLINT);

static int
    MdberrHandler (DBPROCESS *,int,int,int,char *,char *);
#endif

static char
    *col2char(int);
    
static char
    *aprintf(char *,...);

static int
    submit(char *,...);

static void
    db_flush(void);

static void
    CheckConnection (void);

static int
    SubmitQuery (char *);

static char
    *SubstSybCol(char *);
static char
    *col2char(int);
#endif /* } */
/*
** initialize sybase related stuff
*/
void php_init_sybsql(void)
{
#ifdef HAVE_SYBASE
    dbproc=(DBPROCESS *) NULL;
    bitbucket=(FILE *) NULL;
#endif
}


/*
 *  SybsqlConnect() -   connect to the sybase server
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      This function opens a network connection to a sybase server by
 *      calling the local MdbnInit() function. It calls the function with
 *      NULL arguments. When MdbIntit() function is called with NULL 
 *      arguments, it will try to collect the necessary information from
 *      some environment variable. 
 *
 *      All necessary parameters are obtained from the environment.
 *      The following environemnt variables must be set before calling this
 *      function.
 *
 *      DSQUERY - the alias of sybase server as defined in sybase interface
 *                file.
 *      DBUSER  - connect to server as this user
 *      DBPW    - password of the user
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      char    *hostname   name of the sybase server. Note, this is not the
 *                          FQDN of the host, it's how it's defined in sybase
 *                          interface file.
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      None
 *
 *  Return Values:
 *      value   description
 *
 *      None
 *      pushes on stack, 1 on success, 0 on failure
 *
 *  Side Effects:
 *
 *      modies the stack
 *
 *  Limitations and Comments:
 *
 *      DSQUERY environment variable must be set before calling the
 *      function, otherwise sybase API call will fail.
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-15-96  first cut
 */

void SybsqlConnect(void)
{
#ifdef HAVE_SYBASE

    RETCODE
        rc;
    char
        rbuf[10];

    *rbuf='\0';


    dbproc=MdbInit(
            NULL,   /* app name */
            NULL,   /* user */
            NULL,   /* password */
            NULL,   /* server */
            NULL    /* database */
    );

    if (dbproc == (DBPROCESS *) NULL)
    {
        Error("Failed to open connection to sybase server");
        Push("0",LNUMBER);
        return;
    }
    /*
    ** turn on row buffering
    ** buffer maximum 10,000 rows, I think it's enough
    */
    (void) sprintf(rbuf,"%i",10000);
    rc=dbsetopt(dbproc,DBBUFFER,rbuf,-1);
    if (rc == FAIL)
    {
        Error("Failed to turn on row buffering in SybsqlConnect");
        Push("0",LNUMBER);
        return;
    }
    /*
    ** succeeded, push 1 to stack
    */
    Push("1",LNUMBER);
#else
    Push("0",LNUMBER);
    Error("no Sybase support");
#endif
}


/*
 *  MdbInit - actually connects to the sybase server
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      This function actually makes connection to the sybase 
 *      server by calling sybase DB library API calls. It's nothing but a
 *      nice wrapper around Syabase API calls.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      char    *appname    app name to register with server
 *      char    *user       connect as this user
 *      char    *pass       password of the user
 *      char    *server     alias name of the server
 *      char    *database   database to use
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value               description
 *      DBPROCESS *dbproc   a pointer to DBPROCESS struct if succeeds
 *      NULL                if fails
 *
 *  Side Effects:
 *
 *      none
 *
 *  Limitations and Comments:
 *
 *     if the  the function is called with NULL arguments, it will try
 *     to collect the necessary information from some environment variables.
 *     The following environemnt variables must be set before calling this
 *     function without arguments.
 *
 *     APPNAME - the name of the app to register with sybase server.
 *               (good to set, not necessary)
 *  
 *     DSQUERY - the alias of sybase server as defined in sybase interface
 *               file. (must be set)
 *     DBUSER  - connect to server as this user (first try),
 *               if DBUSER is not set, it will look for USER, if it fails,
 *               it will use getlogin(). if getlogin() fails, outta here.
 *               (must be set)
 *
 *     DBPW    - password of the user
 *               (must be set)
 *
 *     DB      - database to use (not needed unless you want to set to the
 *               database right away).
 *
 *     NOTE: the string length of each of the arguments can be maximum 80
 *           character long. If length > 80, it will be truncated.
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-15-96  first cut
 */


#ifdef HAVE_SYBASE

static DBPROCESS *MdbInit(
    char *appname,      /* name of the app to register with sybase server */
    char *user,         /* connect as user */
    char *pass,         /* password of user */
    char *server,       /* alias of sybase server as in interface file */
    char *database      /* database to use */
    )     
{
    RETCODE
        rc=0;

    LOGINREC
        *login;

    DBPROCESS
        *dp;

    char
        loc_appname[80],
        loc_user[80],
        loc_pass[80],
        loc_server[80],
        loc_database[80];

    /*
    ** initialize automatics
    */

    *loc_appname='\0';
    *loc_user='\0';
    *loc_pass='\0';
    *loc_server='\0';
    *loc_database='\0';
    dp=(DBPROCESS *) NULL;
    login=(LOGINREC *) NULL;

    /*
    ** check if appname is set
    */
    if (appname == (char *) NULL)
    {
        appname=getenv("APPNAME");
        if (appname == (char *) NULL)
        {
            (void) strcpy(loc_appname,"Mdbinit)");
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
                Error(" Unable to get user name..");
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
            Error("MdbInit: unable to get server name..");
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
            Error("MdbInit: unable to get password!");
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
    dbmsghandle (MdbmsgHandler);
    dberrhandle (MdberrHandler);

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
        Error("Failed in dbopen()..");
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
            Error("Failed to open database:%s",loc_database);
            dbclose(dp);
            return ((DBPROCESS *) NULL);
        }
    }

return (dp);
}

/*
** display messages from sql server
*/
static int MdbmsgHandler(DBPROCESS *dbproc,DBINT msgno,int msgstate,
    int severity,char *msgtext,char *srvname,char *procname,
    DBUSMALLINT line)
{
    /*
    (void) fprintf (stderr,
        "Message from SQL server %s: (Msg %ld, Level %d, State %d\n",
            srvname,msgno,severity,msgstate);
    (void) fprintf(stderr,"%s\n",msgtext);
    */
      
    return (0);
}

/*
** display Dblib error messages
*/
static int MdberrHandler (DBPROCESS *dbproc,int severity,int dberr,
    int oserr,char *dberrstr,char *oserrstr)
{
    Error("DBlib error (Severity %d):\n\t%s\n", severity,dberrstr);
    Exit(0); 
}
      

#endif /* HAVE_SYBASE */


/*
 *  SybsqlDbuse - use a requested database
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      This function calls sybase API dbuse() to switch to a database
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value   description
 *
 *      none
 *      pushes on stack, 1 on success, 0 on failure
 *
 *  Side Effects:
 *
 *      none
 *
 *  Limitations and Comments:
 *
 *      All necessary things are obtained from php's internal stack. 
 *      return code is pushed back to stack as well.
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-16-96  first cut
 */


void SybsqlDbuse(void)
{
#ifdef HAVE_SYBASE
    Stack
        *s;

    char
        *database=(char *) NULL;
        
    RETCODE
        rc;

    s=Pop();
    if (!s)
    {
        Error("Stack error in sybsql_dbuse");
        return;
    }
    
    if (s->strval)
        database=(char *) estrdup(1,s->strval);
    else
    {
        Error("No database name in sybsql_dbuse expression");
        Push("0",LNUMBER);
        return;
    }
    if (dbproc == (DBPROCESS *) NULL)
    {
        Error("No connection to sybase server in sybsql_dbuse");
        Push("0",LNUMBER);
        return;
    }

    /*
    ** call sybase API
    */
    rc=dbuse(dbproc,database);

    if (rc == FAIL)
    {
        Error("Failed to use database: %s",database);
        Push("0",LNUMBER);
        return;
    }
    Push("1",LNUMBER);

#else
    Push("0",LNUMBER);
    Error("No Sybase support");
#endif
}

/*
 *  SybsqlQuery - submits query to sybase server
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      This function sends the SQL query to sybase server.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      NONE
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      NONE
 *
 *  Return Values:
 *      value   description
 *
 *      NONE
 *      pushes on stack, 1 on success, 0 on failure
 *
 *  Side Effects:
 *
 *      Internal stack is modified.
 *
 *  Limitations and Comments:
 *
 *      All necessary things are obtained from php's internal stack. 
 *      return code is pushed back to stack as well.
 *
 *      this funcion will work if a connection to sybase server is 
 *      already established.
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-16-96  first cut
 */

void SybsqlQuery(void)
{
#ifdef HAVE_SYBASE
    Stack
        *s;

    char
        *query;

    int
        rc;

    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */
    CheckConnection();

    s=Pop();
    if (!s)
    {
        Error("Stack error in sybsql_query");
        return;
    }
    if (s->strval)
        query=(char *) estrdup(0,s->strval);
    else
    {
        Error("No query string in sybsql_query");
        Push("0",LNUMBER);
        return;
    }

    /*
    ** submit the query
    */
    rc=SubmitQuery(query);
    if (rc == -1)
    {
        Push("0",LNUMBER);
        return;
    }
    Push("1",LNUMBER);
#else
    Push("0",LNUMBER);
    Error("No sybase support");
#endif
}

/*
** checks if a connecton to sybase server is establised. 
*/
#ifdef HAVE_SYBASE
static void CheckConnection(void)
{
    if (dbproc == (DBPROCESS *) NULL)
    {
        Error("No connecton to sybase server");
		Exit(1);
    }
return;
}
#endif

/* public version */
void SybsqlCheckConnect(void) {
#ifdef HAVE_SYBASE
    if (dbproc == (DBPROCESS *) NULL)
		Push("0",LNUMBER);
	else
		Push("1",LNUMBER);
#endif
}

/*
 *  SubmitQuery - submits sql query to sybase server
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function sends the SQL request to server.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      char    *query      sql query
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      NONE
 *
 *  Return Values:
 *      value   description
 *
 *      >=0     on success
 *      -1      on failure      
 *
 *  Side Effects:
 *
 *      internal stack is modified.
 *
 *  Limitations and Comments:
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-16-96  first cut
 */

#ifdef HAVE_SYBASE
static int SubmitQuery(char *query)
{
    RETCODE
        rc;
    int
        n;
    rc=dbcmd(dbproc,query);
    if (rc == FAIL)
    {
        Error("Failed to add command %s to DBPROCESS command buffer",query);
        return (-1);
    }

    /*
    ** dispatch command
    */
    if ((dbsqlexec(dbproc) != SUCCEED) ||
        (dbresults(dbproc) != SUCCEED))
    {
        Error("Failed in dbqlexec() or dbresults()");
        return (-1);
    }
    
    n=dbcount(dbproc);
    if (n < 0)
        n=0;
return (n);
    return (0);
}
#endif

/*
 *  SybsqlIsRow - indicates if currect sql command returned any rows
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function calls sybase API routine to determine if the current
 *      SQL command returned any rows.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      NONE
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      NONE
 *
 *  Return Values:
 *      value   description
 *
 *      pushes   1      on stack if the command can return rows
 *      pushes   0      on stack if the command can't return rows
 *
 *  Side Effects:
 *
 *      internal stack is modified
 *
 *  Limitations and Comments:
 *
 *      necessary arguments are extracted and taked from stack.
 *      NOTE: this functin must be called after: 
 *          . a successful connection is made to sybase server. 
 *          . a sql request is dispatched successfully
 *          . dbresult is called
 *      Should never be called after dbnextrow()
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-16-96  first cut
 */

void SybsqlIsRow(void)
{
#ifdef HAVE_SYBASE
    
    RETCODE
        nrows;

    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */
    CheckConnection();

    nrows=DBROWS(dbproc);
    if (nrows != FAIL)
    {
        Push("1",LNUMBER);
        return;
    }
    else
    {
        Push("0",LNUMBER);
        return;
    }
#else
    Push("0",LNUMBER);
    Error("No sybase support");
#endif
}


/*
 *  SybsqlPrintArow - print the requested fields of the current row
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      This function prints the requestd fields of the current row 
 *      in the current row buffer.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      none 
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value   description
 *
 *      none
 *
 *  Side Effects:
 *
 *      none
 *
 *  Limitations and Comments:
 *
 *      the print format is popped from stack
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-16-96  first cut
 */

void SybsqlPrintArow(void)
{
#ifdef HAVE_SYBASE
    Stack
        *s;

    char
        *format,
        *val;

    s=Pop();
    if (!s)
    {
        Error("Stack error in sybsql_print_rows");
        return;
    }
    if (s->strval)
    {
        format=(char *) estrdup(1,s->strval);
    }
    else
    {
        Error("No print format found in sysql_print_rows");
        return;
    }
    
    val=SubstSybCol(format);
    Echo(val,0);
#else
    Error("no sybase support");
#endif
}

/*
 *  SybsqlNextRow - reads the next result row
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function reads the next result row by calling sybase API routine
 *      dbnextrow(). 
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *
 *
 *  Return Values:
 *      value   description
 *
 *
 *
 *  Side Effects:
 *
 *
 *
 *  Limitations and Comments:
 *
 *
 *      Orininarily, the next row is read directly from server.  If DBBUFFER
 *      option is turned on (as we do), and rows have been read out of order
 *      by calling dbgetrow(), the next row is read instead of a linked list
 *      of buffer rows.
 *
 *      NOTE: dbresults() must return SUCCED before calling dbnext row
 *
 *      It pushes 1 on the stack as long as there'r rows left to read. If
 *      there'r no more rows left to read or in case of error it will push
 *      0 on the stack.
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-16-96  first cut
 */

void SybsqlNextRow(void)
{
#ifdef HAVE_SYBASE
    STATUS
        st;
    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */
    CheckConnection();
    
    st=dbnextrow(dbproc);
    switch(st)
    {
        case NO_MORE_ROWS:
        {
            Push("0",LNUMBER);
            break;
        }
        case FAIL:
        {
            Push("0",LNUMBER);
            break;
        }
        default:
        {
            Push("1",LNUMBER);
            break;
        }
    }
#else
    Push("0",LNUMBER);
    Error("No sybase support");
#endif
}


/*
 *  SubstSybCol - substitutes a field with value at this field
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function substitues field/s of the current row with the value
 *      at the field/s.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      char    *format     column/s to print
 *      
 *      example of format:  "this is col1 @1,<td>@2</td> col10 @10"
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value       description
 *
 *      char *buf   pointer to formatted output, ready to print
 *
 *  Side Effects:
 *
 *      none
 *
 *  Limitations and Comments:
 *
 *      the maximum number of characters in the result can be 4096
 *      characters long
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-18-96  first cut
 */




#ifdef HAVE_SYBASE
static char *SubstSybCol(char *str)
{
    register char
        *cp1,
        *cp2,
        *a1,
        *a2,
        *var1,
        *var2;

    char
        *val,
        *buf,
        varBuf[80];

    int
        offset,
        count;
    /*
    ** malloc for buf
    */
    buf=emalloc(1,4096);
    count=offset=0;
    cp1=str;
    cp2=buf;
    bzero(buf,2048);

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
                    Error("Bad Column number: %c",*cp1);
                    Exit(0);
                }

                var1=cp1;
                var2=cp1;
                /*
                ** copy the column number/s
                */
                while (isdigit(*var2))
                {
                    var2++;
                }

                bzero(varBuf,sizeof(varBuf));
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
                    Error("Column no %d is bigger than max no of cols:%d",
                        offset,count);
                    Exit(0);
                }

                /*
                ** now get the value from that offset
                */
                val=col2char(offset);

                if (val)
                {
                    (void) strcpy(cp2,val);
                    cp2 += strlen(val);
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
#endif



/*
 *  col2char - converts any column data to an array of char
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function converts any column data in the current row buffer
 *      to an array of characters and returns a pointer to it.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      int     col         column number
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value           description
 *
 *      char    *val    a pointer to converted to string    
 *
 *  Side Effects:
 *
 *      none
 *
 *  Limitations and Comments:
 *
 *      this routine is adapted from sybfmt
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-18-96  first cut
 */

#ifdef HAVE_SYBASE
static char *col2char(int j)
{
    if (dbdatlen(dbproc,j) == 0)    /* data has null value */
        return ((char *) NULL);

    switch (coltype(j))
    {
        case SYBINT2:
        case SYBINT4:
        {    
            return (aprintf("%i",anyintcol(j)));
        }
 
        case SYBCHAR:
        case SYBTEXT:
        {
            int
                len;
        
             len=dbdatlen(dbproc,j);
             while (len && charcol(j)[len-1] == ' ')
                len--;
    
             return (aprintf("%.*s",len,charcol(j)));
        }
         
        case SYBFLT8:
        case SYBREAL:
        {
            return (aprintf("%-.*f",2,floatcol(j)));
        }

        default:
        {
            if (dbwillconvert(coltype(j),SYBCHAR))
            {
                char
                    buf[200];

                dbconvert(NULL,coltype(j),dbdata(dbproc,j),
                    dbdatlen(dbproc,j),SYBCHAR,buf,-1);
                return (estrdup(1,(buf)));
            }
            else
            {
                Error("datatype conversion problem: can't convert Sybase datatype %i to char.\n", coltype(j));
                return ((char *) NULL);
            }
        }
    }
}
#endif


/*
 *  aprintf - formatted print to newly allocated space
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this funciton prints formatted string to newly allocated large 
 *      enough space.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      char    *format
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value   description
 *
 *      char *
 *
 *  Side Effects:
 *
 *      none
 *
 *  Limitations and Comments:
 *
 *      adapted from sybfmt
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-18-96  first cut
 */

#ifdef HAVE_SYBASE
static char *aprintf(char *format,...)
{
    va_list
        args;
 
    int  
        len;   

    char
        *buf;
             
    if (bitbucket == (FILE *) NULL)
        bitbucket=fopen("/dev/null","w");
             
    va_start(args,format);
    len=vfprintf(bitbucket,format,args)+1;
    if (len == 0)
        return ((char *) NULL);
         
    buf=emalloc(1,len*sizeof(char));
    vsprintf(buf,format,args);
    return (buf);   
}
#endif


/*
 *  SybsqlNumRows - number of rows in a result
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function returns the number of rows in the current result
 *      buffer.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value   description
 *
 *      number of rows is pushed on the stack.      
 *
 *  Side Effects:
 *
 *      after this call the row counter always points to the very first
 *      row (by calling dbsetrow()).
 *
 *  Limitations and Comments:
 *
 *      when this function is called, it will seek to the first row right
 *      away, then it will call dbnextrow() until there'r no more rows and
 *      incrment a internal counter to find out the number of rows in the
 *      result buffer. Then it points back to the very first row. Therefore,
 *      after calling this function row counter always points to the very
 *      first row. It's ugly but I don't know any other way at this time.
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-20-96  first cut
 */

void SybsqlNumRows(void)
{
#ifdef HAVE_SYBASE
    Stack
        *s;

    int
        nrows=0;
    
    char
        tmpbuf[20];

    DBINT
        n;

    *tmpbuf='\0';
    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */
    CheckConnection();

    /*
    ** jump to the first row, row counter might not be at the first
    ** row.
    */
    (void) dbsetrow(dbproc,(DBINT) 1);  /* 1 means first row */
    while (dbnextrow(dbproc) != NO_MORE_ROWS)
    {
        nrows++;
    }

    (void) sprintf(tmpbuf,"%d",nrows);
    Push(tmpbuf,LNUMBER);
    dbsetrow(dbproc,1);
    (void) dbresults(dbproc);
    /*
    ** point back to the first row
    */
#else
    Push("0",LNUMBER);
    Error("No sybase support");
#endif
}


/*
 *  SybsqlPrintAllRows - print specified fields of all rows
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *
 *
 *  Return Values:
 *      value   description
 *
 *
 *
 *  Side Effects:
 *
 *
 *
 *  Limitations and Comments:
 *
 *
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-20-96  first cut
 */


void SybsqlPrintAllRows(void)
{
#ifdef HAVE_SYBASE
    Echo("Not implementd yet",0);
#else
    Error("No sybase support");
#endif
} 



/*
 *  SybsqlResult - prints specific fields of the current row
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function prints the specific field/s from the current row
 *      buffer.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      none
 *      first argument is popped from the stack.
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value   description
 *
 *      none
 *
 *  Side Effects:
 *
 *      internal stack is modified
 *
 *  Limitations and Comments:
 *
 *      never call sybsql_nextrow() while calling this function, this 
 *      function will call dbnext().
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-20-96  first cut
 */

void SybsqlResult(void)
{
#ifdef HAVE_SYBASE
    Stack
        *s;

    char
        *format,
        *val;

    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */
    CheckConnection();

    s=Pop();
    if (!s)
    {
        Error("Stack error in sybsql_result");
        return;
    }
    if (s->strval)
    {
        format=(char *) estrdup(1,s->strval);
    }
    else
    {
        Error("No field specification found in sybsql_result");
        return;
    }
    /*
    ** get the result and put in a variable
    */
    if (dbnextrow(dbproc) != NO_MORE_ROWS)
    {
        val=SubstSybCol(format);
        Echo(val,0);
    }

#else
    Error("No sybase suport");
#endif
}


/*
 *  SybsqlSeek - sets a row to the current row
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function sets the requested row number as the current row in
 *      the row buffer. dbnextrow() will read this row.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      none
 *      row number is pooped from stack
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value   description
 *
 *      pushes 1 on stack if the row number is found in the buffer
 *      pushes 0 if there's no such row number in the buffer or if it fails.
 *
 *  Side Effects:
 *
 *      stack is modified       
 *
 *  Limitations and Comments:
 *
 *      none
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-21-96  first cut
 */

void SybsqlSeek(void)
{
#ifdef HAVE_SYBASE
    Stack
        *s;
    DBINT
        row;

    STATUS
        st;

    char
        buf[50];
    row=0;
    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */
    CheckConnection();

    s=Pop();
    if (!s)
    {
        Error("Stack error in sybsql_seek");
        return;
    }
    if (s->strval)
    {
        row=(DBINT)s->intval;
    }
    else
    {
        Error("No row number in sybsql_seek expression");
        Push("0",LNUMBER);
        return;
    }
    if (row >=0 )
    {
        row++;
        st=dbsetrow(dbproc,row);
        switch(st)
        {
            case MORE_ROWS:
            {
                Push("1",LNUMBER);
                break;
            }

            case NO_MORE_ROWS:
            case FAIL:
            {
                Push("0",LNUMBER);
                break;
            }
        }
        
    }
    else
    {
        Push("0",LNUMBER);
        Error("Row number can not be negative");
        return;
    }
    
    
#else
    Error("No sybase support");
#endif
}


/*
 *  SybsqlNumFields - number of fields in a result
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function returns the number of fields in a row in the current
 *      row buffer.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Output Parameters:
 *      type    identifier  description
 *      none
 *
 *  Return Values:
 *      value   description
 *
 *      none
 *      number of fields is pushed in the stack
 *
 *  Side Effects:
 *
 *      stack is modified
 *
 *  Limitations and Comments:
 *
 *
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-22-96  first cut
 */

void SybsqlNumFields(void)
{
#ifdef HAVE_SYBASE
    int
        ncols;
    char
        buf[20];
    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */  
    CheckConnection();
    
    ncols=dbnumcols(dbproc);    
    (void) sprintf(buf,"%d",ncols);
    Push(buf,LNUMBER);
#else
    Push("0",LNUMBER);
    Error("No sybase support");
#endif
}


/*
 *  SybsqlFieldName - name of a column
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function returns the name of a regular result column
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      none
 *      one argument popped from stack
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value   description
 *
 *      none
 *      pushes the name of the column in the stack
 *
 *  Side Effects:
 *
 *      stack is modified
 *
 *  Limitations and Comments:
 *
 *      none
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-22-96  first cut
 */

void SybsqlFieldName(void)
{
#ifdef HAVE_SYBASE
    Stack
        *s;

    int
        col;

    char
        *colname;
    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */
    CheckConnection();

    s=Pop();
    if (!s)
    {
         Error("Stack error in sybsql_fieldname");
         return;
    }
    if (s->strval)
    {
        col=s->intval;
    }
    else
    {
        Error("No such columns number:%d",col);
        Push("",STRING);
        return;
    }
    if (col >=0 )
    {
        col++;  /* sybase column start at 1 */

        colname=dbcolname(dbproc,col);
        Push(colname,STRING);
    }
    else
    {
        Push("",STRING);
        Error("Column number can not be negative");
        return;
    }
#else
    Push("",STRING);
    Error("no sybase support");
#endif
}


/*
 *  SybsqlResultAll - prints all rows and columns
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function prints all the rows and columns in the current
 *      row buffer.
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *
 *
 *  Return Values:
 *      value   description
 *
 *
 *
 *  Side Effects:
 *
 *
 *
 *  Limitations and Comments:
 *
 *
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-22-96  first cut
 */

void SybsqlResultAll(void)
{
#ifdef HAVE_SYBASE
    int
        i,
        ncols;

     RETCODE
        rc;

    char
        *result;
    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */
    CheckConnection();

    php_header(0,NULL);
    /*
    (void) fprintf (stdout,"<table border cellspacing=0>\n");
    */
    Echo("<table border cellspacing=0>\n",0);
    
    ncols=dbnumcols(dbproc);
    i=0;
    if (ncols > 0)
    {
        /*
        (void) fprintf (stdout,"<tr>\n");
        */
        Echo("<tr>\n",0);
        for (i=1; i <= ncols; i++)
        {
            /*
            (void) fprintf (stdout,"<td align=center bgcolor=\"cyan\"><b>%s</b></td>\n",dbcolname(dbproc,i));
            */
            Echo("<td align=center bgcolor=\"cyan\"><b>",0);
            Echo(dbcolname(dbproc,i),0);
            Echo("</b></td>\n",0);
        }
        /*
        (void) fprintf (stdout,"</tr>\n");
        */
        Echo("</tr>\n",0);
    }

    /*
    ** now print the output
    */
    while ((rc=dbnextrow(dbproc)) != NO_MORE_ROWS)
    {
        if (rc == BUF_FULL) 
        {
            /*
            (void) fprintf (stderr,"Warning: buffer full, discarding row 1\n");
            */
            Error("buffer full, discarding row 1\n",0);
            dbclrbuf(dbproc,1);
        }
        /*
        (void) fprintf (stdout,"<tr>\n");
        */
        Echo("<tr>\n",0);

        for (i=1; i <= ncols; i++)
        {
            result=col2char(i);
            if (result != (char *) NULL)
            {
                /*
                (void) fprintf (stdout,"<td><font size=-1>%s</font></td>\n",
                    result);
                */
                Echo("<td><font size=-1>",0);
                Echo(result,0);
                Echo("</font></td>\n",0);
            }
        }
        /*
        (void) fprintf (stdout,"</tr>\n");
        */
        Echo("</tr>\n",0);
    }
    /*
    (void) fprintf (stdout,"</table>\n");
    */
    Echo("</table>\n",0);
#else
    Error("No sybase support");
#endif
}


/*
 *  SybsqlGetField() -   gets the value of a specific field
 *
 *  RCS:
 *      $Revision: 1.12 $
 *      $Date: 1997/10/25 16:17:30 $
 *
 *  Description:
 *
 *      this function gets the value of a specific field in the current row
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      none
 *      first argument is popped from the stack
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      none
 *
 *  Return Values:
 *      value   description
 *
 *      none
 *      pushes the string value to the stack. empty string if there's no value
 *      for the field.
 *
 *  Side Effects:
 *
 *      stack is modified
 *
 *  Limitations and Comments:
 *
 *      sybsql_nextrow() must be callied before calling this function.
 *      sybsql_nextrow() must be called if row pointer needs to be 
 *      increment, because this function only reads the current row in the
 *      row buffer.
 *
 *  Development History:
 *      who                  when       why
 *      MA_Muquit@fccc.edu   Sep-26-96  first cut
 */

void SybsqlGetField(void)
{
#ifdef HAVE_SYBASE
    Stack
        *s;

    char
        *format,
        *val;

    /*
    ** if no connection to sybase, we will exit from CheckConnection()
    */
    CheckConnection();

    s=Pop();
    if (!s)
    {
        Error("Stack error in sybsql_getfield");
        return;
    }
    if (s->strval)
    {
        format=(char *) estrdup(1,s->strval);
    }
    else
    {
        Error("No field specification found in sybsql_getfield");
        return;
    }
    val=SubstSybCol(format);
    Push(val,STRING);

#else
    Error("No sybase support");
#endif
}

void SybsqlExit (void)
{
#ifdef HAVE_SYBASE
	if(dbproc) {
		dbclose(dbproc);
		dbproc=NULL;
	}
#endif
}
