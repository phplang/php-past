/***[php.h]*******************************************************[TAB=4]****\
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
/* $Id: php.h,v 1.196 1998/01/09 18:57:57 rasmus Exp $ */

/* MS Visual C++ V5 defines WIN32.  Use this to define WINDOWS
 * for any pre-existing modifications for windows.
 * I have used code from the apacheNT port, and from the Downhill
 * Project.
 *
 * Compilation of the win32 version has only been tested with VC5
 * and with the configuration as is in configure.h.w32 and php.h
 * 
 */
#if WINNT|WIN32
#define WINDOWS 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <malloc.h>
#include <direct.h>
#include <stdlib.h>
typedef int uid_t;
typedef int gid_t;
typedef int pid_t;
typedef int mode_t;
typedef char * caddr_t;
#define strcasecmp(s1, s2) stricmp(s1, s2)
#define strncasecmp(s1, s2, n) strnicmp(s1, s2, n)
#define lstat(x, y) stat(x, y)
#define		_IFIFO	0010000	/* fifo */
#define		_IFBLK	0060000	/* block special */
#define		_IFLNK	0120000	/* symbolic link */
#define S_IFIFO		_IFIFO
#define S_IFBLK		_IFBLK
#define	S_IFLNK		_IFLNK
#define pclose		_pclose
#define popen		_popen
#define chdir		_chdir
#define mkdir(a,b)	_mkdir(a)
#define rmdir		_rmdir
#define sleep		_sleep
#define getcwd		_getcwd
#endif


#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../config.h"
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#if HAVE_MEMORY_H
#include <memory.h>
#endif
#if HAVE_REGCOMP
#include <regex.h>
#else
#include "regex.h"
#endif
#if PHPFASTCGI
#include "fcgi_stdio.h"
#endif
#if APACHE
#if WINNT|WIN32
/* apacheNT wants to define these, so lets let it */
#if HAVE_MMAP
#undef HAVE_MMAP
#endif
#ifdef sleep
#undef sleep
#endif
#endif
#include "httpd.h"
#include "mod_php.h"

extern request_rec *php_rqst;
#endif


/* 
 * This should be set to the same as the UserDir httpd setting from
 * srm.conf on your system for proper ~username expansion 
 */
#define PHP_PUB_DIRNAME	"public_html"

/*
 * You can override this setting with the PHP_USERDIR env. var if you
 * change your config.
 *
 */
#define PHP_PUB_DIRNAME_ENV	"PHP_USERDIR"

/* 
 * Only define the following if you wish the ROOT_DIR for PHP loaded
 * scripts to be different from the server's default ROOT_DIR.  
 * Only really useful in non server-wide setups to help shorten the
 * URL's from something like:
 *
 *   http://www.io.org/~rasmus/php.cgi/~rasmus/file.html
 *
 * to:
 *
 *   http://www.io.org/~rasmus/php.cgi/file.html
 */ 
/* #define PHP_ROOT_DIR "/home/rasmus/public_html" */

/*
 * If you are compiling PHP in DEBUG mode, make sure the following
 * points to a valid path and that PHP is allowed to create/write to
 * this file.
 */
#ifdef WINDOWS
#define DEBUG_FILE	"/windows/php.err"
#else
#define DEBUG_FILE	"/tmp/php.err"
#endif

/*
 * If you are using a virtual domain on a braindead http server which
 * sets the SCRIPT_NAME environment variable to the absolute path as
 * opposed to the relative path to your virtual domain, then you need
 * to define the following to be the relative path to your php.cgi
 * program.  (include php.cgi in the path please)
 */
/* #define VIRTUAL_PATH	"/cgi/php.cgi" */

/*
 * If MAGIC_QUOTES is defined, then the single quote " ' " character
 * will be automatically escaped if it is found in a posted form variable.
 * This is useful when mSQL or Postgres95 support is enabled since the 
 * single quote has to be escaped when it is part of an mSQL  or Postgres95
 * query.  In the case of Postgres95, the double quote character will also
 * be escaped.
 */
#define MAGIC_QUOTES 1

/*
 * If UPLOAD_TMPDIR is defined, then this directory will be used for storing
 * uploaded files temporarily.  If undefined, the system's default temporary
 * directory will be used.  You can check which directory is the default
 * temporary directory by searching through your /usr/include directory and
 * looking for the P_tmpdir variable.
 *
 * NOTE: For Apache module, set this via phpUploadTmpDir configuration
 *       directive.
 */
/* #define UPLOAD_TMPDIR "/tmp" */

/*
 * If no matter what you do, you cannot get it to accept your ?config
 * password, define the following variable.  Delete any files in your
 * configuration directory, recompile PHP and try using the password: php
 */
/* #define BROKEN_CONFIG_PW 1 */

/*
 * SETLOCALE Support can be enabled by setting this to 1.  You will probably
 * want to do this if you are designing non-English pages.  When SETLOCALE is
 * enabled, PHP/FI will check your environment variables for the correct
 * locale settings.  See your system man page for setlocale() for more details.
 */
/* #define SETLOCALE 1 */

/*
 * SQLLOGDB should be set to the database name you wish to use to store
 * log data if you are using PHP/FI with mSQL-based logging.  Remember
 * to create this database before trying to run PHP/FI in mSQL logging mode.
 *
 * NOTE: For Apache module, set this via phpSQLLogDB configuration directive
 */
#define SQLLOGDB "phpfi"

/*
 * SQLLOGHOST should be set to the database host you wish to use to store
 * log data if you are using PHP/FI with SQL-based logging.  
 *
 * NOTE: For Apache module, set this via phpSQLLogHost configuration directive
 */
#define SQLLOGHOST "localhost"

/*
 * SQLLOGTMP is set to the directory where you want temporary lock
 * files created when SQL LOGGING is used.  Since mSQL 1 does not support
 * atomic updates of counters, an external lock file unfortunately needs
 * to be used.  Atomic counter updates are used when the SQL engine supports
 * them.
 */
#define SQLLOGTMP "/tmp"

/*
 * NOLOGSUCCESSIVE should be set to 1 if you want multiple successive hits
 * from the same remote host to be ignored (e.g. someone repeatedly 
 * reloading the same page).  If undefined or set to 0, successive hits 
 * from the same remote host will be logged (and counted on the page counter).
 * The default is set to 0.
 */
#define NOLOGSUCCESSIVE 0

/*
 * INCLUDEPATH is a colon-separated list of directories where php will
 * look for files in when running include().  The default is to look
 * only in the current directory.
 */
#define INCLUDEPATH "."

/*
 * PATTERN_RESTRICT can be enabled for security reasons to restrict php/fi
 * to only load files with certain patterns in their filenames.  If unset,
 * php/fi will by default be allowed to open all files regardless of the
 * presence of any .htaccess or other external security mechanisms
 * NOTE: the pattern must be a valid regular expression.  The example,
 * ".*\\.phtml$" causes php/fi to only be allowed to load files ending with
 * the .phtml extension.
 */
/* #define PATTERN_RESTRICT ".*\\.phtml$" */

/*
 * PHP_SAFE_MODE should be set if you are running PHP on a shared server
 * and you want to restrict users from being able to access each others
 * files and do other nasty things related to the fact that everyones'
 * scripts all run as the same user id.  See the documentation for a
 * full description of PHP's Safe Mode.
 *
 * The EXEC_DIR is the directory where Exec(), System(), PassThru() and 
 * Popen() calls are allowed to execute binaries from in SAFE MODE.
 */
/*
#define PHP_SAFE_MODE 1
#define PHP_SAFE_MODE_EXEC_DIR "/usr/local/bin"
*/

/*
 * Set PHP_LOOPLIMIT to ensure that your programs don't go into an
 * infinite loop.  Unfortunately, if your PHP script goes into a loop
 * the apache server will simply hang there gobbling up CPU time, until
 * the machine reboots or someone kills the process.
 * If PHP_LOOPLIMIT is defined, only PHP_LOOPLIMIT while statements can
 * evaluate true.  After that number is reached, each while generates an
 * error message and behaves as though the expression was false.
 * The figure 100000 was chosen because it took about 15 CPU seconds to run
 * on a Pentium 166 with an empty loop, and this is already too long for
 * most production web environments.
 */
#define PHP_LOOPLIMIT 100000

/*
 * Max size of a single line of input in the HTML files
 */
#define LINEBUFSIZE	4096

/*
 * Input buffer size for Exec() commands
 */
#define EXEC_INPUT_BUF 4096

/*
 * ECHO_BUF sets the size of the echo overflow buffer
 * (only used in Apache module version)
 */
#define ECHO_BUF 64

/*
 * DEFAULT_MAX_DATA_SPACE
 *
 * Set this to the maximum size a memory sub-pool is allowed to grow
 * to.  (number of kilo bytes)  
 * In the Apache module version, this can be overwritten by the
 * phpMaxDataSpace Apache configuration directive.
 */
#define DEFAULT_MAX_DATA_SPACE 8192

/*
 * PHP_TRACK_VARS
 *
 * If you define this, 3 arrays will be created in your scripts.  They are:
 * 
 *  $PHP_GETVARS
 *  $PHP_POSTVARS
 *  $PHP_COOKIEVARS
 *
 * These arrays will contain the GET/POST/Cookie variables respectively.
 */
/* #define PHP_TRACK_VARS 1 */

/*
 * PHP_AUTH_VARS
 *
 * If this is defined, the the Apache module version will be allowed to
 * set the PHP_AUTH_USER, PHP_AUTH_PW and PHP_AUTH_TYPE variables.  
 * This is a potential security concern.  See the HTTP Authentication
 * section in the documentation for more details.
 */
#define PHP_AUTH_VARS 1

/*
 * SNMP Support
 *
 * The SNMP support in PHP is very rough.  It was written for ucd-snmp-3.1.3
 * available from ftp.ece.ucdavis.edu:/pub/snmp/ucd-snmp.tar.gz or from
 * sunsite.cnlab-switch.ch:/mirror/ucd-snmp/ucd-snmp.tar.gz
 *
 * To enable it, set the following #define to 1 and edit your Makefile's
 * LIBS line adding something like, "-lsnmp" assuming you have put
 * libsnmp.a somewhere on your linker path
 *
 * (Please do not send me SNMP related questions.  I know just enough
 *  about SNMP to hack support into PHP for it, but nothing more.)
 */
/* #define PHP_SNMP_SUPPORT 1 */

/*
 * PHP_PG_LOWER
 *
 * Define the PHP_PG_LOWER variable if you want to force PHP to lower-case
 * Postgres field names.
 *
 */
/* #define PHP_PG_LOWER 1 */

/*
 * PHP_MYSQL_GETLOGIN
 *
 * If you want the default mysql user to be fetched via the getlogin()
 * call and then if that fails by cuserid(), then uncomment the following
 * line.  Otherwise the default mysql user will be set to the current user
 * that the process is running as.
 */
/* #define PHP_MYSQL_GETLOGIN 1 */

/* 
 * GDBM_FIX
 *
 * Some people have reported problems getting gdbm to work correctly.  If
 * you are seeing a gdbm compatibility problem, try defining this and
 * let me know if it fixes your problem.  If it does, please tell me
 * which version of gdbm you are using
 */
/* #define GDBM_FIX 1 */

/*
 * Redefine PHP's Error() function so that it doesn't conflict with mod_perl
 */
#define Error PHPError

/*-- Do not touch anything after this point unless you are very brave --*/

#define PHP_VERSION "2.0.1"

#define VAR_INIT_CHAR	'$'

#if APACHE
#if APACHE_NEWAPI
#define PUTS(a) rputs((a),php_rqst)
#define PUTC(a) rputc((a),php_rqst)
#else
#define PUTS(a) rprintf(php_rqst,"%s",(a))
#define PUTC(a) rprintf(php_rqst,"%c",(a))
#endif
#else
#define PUTS(a) fputs((a),stdout)
#define PUTC(a) fputc((a),stdout)
#endif

#if HAVE_FLOCK
#ifndef LOCK_SH
#define LOCK_SH 1
#endif
#ifndef LOCK_EX
#define LOCK_EX 2
#endif
#ifndef LOCK_NB
#define LOCK_NB 4
#endif
#ifndef LOCK_UN
#define LOCK_UN 8
#endif
#endif

#if DEBUG
#define emalloc(a,b)		php_pool_alloc(__FILE__,__LINE__,(a),(b))
#define estrdup(a,b)		php_pool_strdup(__FILE__,__LINE__,(a),(b))
#else
#define emalloc(a,b)		php_pool_alloc((a),(b))
#define estrdup(a,b)		php_pool_strdup((a),(b))
#endif

#if STDC_HEADERS
#include <string.h>
#else
#ifndef HAVE_MEMCPY
#define memcpy(d, s, n)		bcopy((s), (d), (n))
#endif
#endif

#define YYSTYPE long

#if GDBM
#define DBM_TYPE GDBM_FILE;
#else
#if NDBM
#define DBM_TYPE DBM*;
#else
#define DBM_TYPE FILE*;
typedef struct {
	char *dptr;
	int   dsize;
} datum;
#endif
#endif

/* Variable Tree */
typedef struct VarTree {
	short type;
	int count;
	char *name;
	char *strval;
	char *iname;
	long intval;
	double douval;
	int flag;
	int scope;				/* 0=local to frame, 4=global, 8=static to frame */
	struct VarTree *left;
	struct VarTree *right;
	struct VarTree *next;
	struct VarTree *prev;
	struct VarTree *lacc;
	struct VarTree *lastnode;
	int deleted;
	int allocated;
} VarTree;

/* Expression Stack */
typedef struct Stack {
	short type;
	char *strval;
	long intval;
	double douval;
	VarTree *var;	
	struct Stack *next;
	int flag;
} Stack;

/* Conditional State Stack */
typedef struct CondStack {
	int state;
	int active;
	struct CondStack *next;
} CondStack;

/* Entry point marker stack */
typedef struct CondMark {
	struct CondStack *mark;
	struct CondMark *next;
} CondMark;

/* Switch state stack */
typedef struct SwitchStack {
	int type;
	int matched;
	long intval;
	double douval;
	char *strval;
	struct SwitchStack *next;
} SwitchStack;

/* Entry point marker stack */
typedef struct SwitchMark {
	struct SwitchStack *mark;
	struct SwitchMark *next;
} SwitchMark;

/* Match Stack */
typedef struct MatchStack {
	int val;
	struct MatchStack *next;
} MatchStack;

/* Entry point marker stack */
typedef struct MatchMark {
	struct MatchStack *mark;
	struct MatchMark *next;
} MatchMark;

/* While loop stack */
typedef struct WhileStack {
	long seekpos;
	int offset;
	int state;
	int lineno;
	struct WhileStack *next;
} WhileStack;

/* Entry point marker stack */
typedef struct WhileMark {
	struct WhileStack *mark;
	struct WhileMark *next;
} WhileMark;

typedef struct EndBraceStack {
	int token;
	struct EndBraceStack *next;
} EndBraceStack;

typedef struct EndBraceMark {
	struct EndBraceStack *mark;
	struct EndBraceMark *next;
} EndBraceMark;

typedef struct dbmStack {
	char *filename;
	char *lockfn;
	int lockfd;
	void *dbf;
	struct dbmStack *next;
} dbmStack;

typedef struct FileStack {
#if PHP_HAVE_MMAP
	caddr_t pa;
#else
	FILE *fp;
	char *pa;
#endif
	int fd;
	long size;
	long pos;
	int state;
	int lstate;
	int lineno;
	char *filename;
	struct FileStack *next;
} FileStack;

typedef struct FrameStack {
	VarTree *frame;
	struct FrameStack *next;
} FrameStack;

typedef struct FuncArgList {
	char *arg;
	struct FuncArgList *next;
	struct FuncArgList *prev;
} FuncArgList;

typedef struct FuncStack {
	char *name;
	long size;
	char *buf;
	VarTree *frame;
	FuncArgList *params;	
	struct FuncStack *next;
} FuncStack;

typedef struct CounterStack {
	int inif;
	int inwhile;
	struct CounterStack *next;
} CounterStack;

typedef struct FpStack {
	FILE *fp;
	char *filename;
	int id;
	int type;
	struct FpStack *next;
} FpStack;

typedef struct AccessInfo {
	int def;
	int mode;
	char *password;
	int type;
	char *patt;
	struct AccessInfo *next;
} AccessInfo;

typedef struct PtrStack {
	void *ptr;
	struct PtrStack *next;
} PtrStack;

typedef struct CookieList {
	char *name;
	char *value;
	time_t expires;
	char *path;
	char *domain;
	int secure;		
	struct CookieList *next;
} CookieList;

#ifndef APACHE
typedef struct pool {
  union php_block_hdr *first;
  union php_block_hdr *last;
  struct cleanup *cleanups;
  struct process_chain *subprocesses;
  struct pool *sub_pools;
  struct pool *sub_next;
  struct pool *sub_prev;
  struct pool *parent;
  char *free_first_avail;
} pool;
#endif
/* Prototypes */

/* error.c */
void Debug(char *, ...);
void Error(char *, ...);
void CloseDebug(void);
int  ErrorPrintState(int);
void SetErrorReporting(void);
void php_init_error(void);

/* lex.c */
void ParserInit(int, long, int, char *);
int yylex(YYSTYPE *);
void yyerror(char *);
void WhileAgain(long, int, int);
void Include(void);
void Exit(int);
char *GetCurrentLexLine(int *, int *);
void InitFunc(void);
void DefineFunc(char *);
FuncStack *FindFunc(char *, long *, VarTree **);
void RunFunc(char *);
VarTree *GetFuncFrame(void);
void AddToArgList(char *);
FuncArgList *GetFuncArgList(void);
void ClearFuncArgList(void);
void Return(void);
void php_init_lex(void);
void IntFunc(char *);
int NewWhileIteration(long);
void Eval(void);
void PushCounters(void);
void PopCounters(void);
void SetHeaderCalled(void);
long GetSeekPos(void);
void PHPFlush(void);
void PreParseFile(void);
void PostParseFile(void);

/* date.c */
void Date(int, int);
void UnixTime(void);
void MkTime(int);
char *std_date(time_t);
void CheckDate(void);

/* uniqid.c */
void UniqId(void);

/* soundex.c */
void Soundex(void);

/* syslog.c */
void OpenLog(void);
void CloseLog(void);
void Syslog(void);
void php_init_syslog();

/* parse.c */
int yyparse(void);
void php_init_yacc(void);

/* calc.c */
int Calc(int);
int CalcInc(int);
void Neg(void);
void BitNot(void);
void BinDec(void);
void DecBin(void);
void DecHex(void);
long _HexDec(char *);
void HexDec(void);
long _OctDec(char *);
void OctDec(void);
void DecOct(void);
void Sin(void);
void Cos(void);
void Tan(void);
void Sqrt(void);
void Exp(void);
void mathLog(void);
void mathLog10(void);
void Abs(void);
void Pow(void);
void shl(void);
void shr(void);
void Ceil(void);
void Floor(void);

/* stack.c */
void Push(char *, int);
Stack *Pop(void);
void ClearStack(void);
void php_init_stack(void);

/* var.c */
void php_init_symbol_tree(void);
void SetVar(char *, int, int);
VarTree *GetVar(char *, char *, int);
void IsSet(char *, int);
char *SubVar(char *);
void Count(void);
void ArrayMax(void);
void ArrayMin(void);
void PutEnv(void);
void GetEnv(void);
void PtrPush(void *);
void *PtrPop(void);
void SecureVar(void);
void Reset(char *);
void Key(char *);
void Next(char *);
void Prev(char *);
void End(char *);
void PushStackFrame(void);
void PopStackFrame(void);
void Global(void);
void copyarray(VarTree *, VarTree *, VarTree *, int);
void deletearray(VarTree *);
void UnSet(char *, int);
void Pos(void);
void GetAllHeaders(void);

/* echo.c */
void Echo(char *, int);
void StripSlashes(char *);
void StripDollarSlashes(char *);
char *AddSlashes(char *, int);
void ParseEscapes(char *);
void HtmlSpecialChars(void);
int FormatCheck(char **, char **, char **);
void _AddSlashes(void);
void _StripSlashes(void);

/* cond.c */
int Compare(int);
void If(void);
void Else(void);
void ElseIf(void);
void EndIf(void);
void Not(void);
int Logic(int);
int GetCurrentState(int *);
int GetCurrentActive(void);
void ClearCondStack(void);
void CondPush(int, int);
void CondChange(int, int);
int CondPop(int *);
int CondPeek(int *);
int CheckCond(Stack *);
void PushCondMatchMarks(void);
void PopCondMatchMarks(void);
void php_init_cond(void);
void BracePush(int);
int BracePop(void);
int BraceCheck(void);

/* switch.c */
void Switch(void);
void EndSwitch(void);
void Case(void);
void Break(void);
void Default(void);
void PushSwitchMark(void);
void PopSwitchMark(void);
void php_init_switch(void);

/* alloc.c */
void *Emalloc(char *, int, int);
void Efree(char *, int, void *);
char *Estrdup(char *, int, char *);

/* db.c */
void ListSupportedDBs(void);
void dbmOpen(void);
int _dbmOpen(char *, char *, int);
void dbmClose(void);
int _dbmClose(char *);
void dbmCloseAll(void);
void dbmInsert(void);
int _dbmInsert(char *, char *, char *);
void dbmReplace(void);
int _dbmReplace(char *, char *, char *);
void dbmFetch(void);
char *_dbmFetch(char *, char *);
void dbmExists(void);
int _dbmExists(char *, char *);
void dbmDelete(void);
int _dbmDelete(char *, char *);
void dbmFirstKey(void);
char *_dbmFirstKey(char *);
void dbmNextKey(void);
char *_dbmNextKey(char *, char *);
void php_init_db(void);

/* while.c */
void WhilePush(long, int, int);
WhileStack *WhilePop(void);
void While(long);
void EndWhile(void);
void PushWhileMark(void);
void PopWhileMark(void);
void php_init_while(void);
void WhileFinish(void);

/* string.c */
void StrLen(void);
void StrVal(void);
void IntVal(void);
void DoubleVal(void);
void StrTok(int);
char *_strtoupper(char *);
void StrToUpper(void);
char *_strtolower(char *);
void StrToLower(void);
void StrStr(void);
void StrrChr(void);
void SetType(void);
void GetType(void);
void SubStr(void);
void UrlEncode(void);
void UrlDecode(void);
char *php_urlencode(char *);
void Ord(void);
void QuoteMeta(void);
void UcFirst(void);
void Sprintf(int);
void Chr(void);
void Chop(void);
char *_StrTr(char *,char *,char *);
void StrTr(void);

/* msql.c */
void Msql(void);
void MsqlResult(void);
void MsqlClose(void);
void MsqlConnect(void);
void MsqlFreeResult(void);
void MsqlNumRows(void);
void MsqlNumFields(void);
void MsqlField(int);
void MsqlRegCase(void);
int  msqlGetDbSock(void);
void msqlSetCurrent(int, char *);
void MsqlListTables(void);
void MsqlListFields(void);
void MsqlTableName(void);
void MsqlListDBs(void);
void MsqlDBName(void);
void MsqlDropDB(void);
void MsqlCreateDB(void);
void MsqlListIndex(void);
void php_init_msql(char *);

/* sybsql.c */  /*muquit, # ma_muquit@fccc.edu, Sep-15-96 */
void SybsqlConnect(void);
void SybsqlDbuse(void);
void SybsqlQuery(void);
void SybsqlIsRow(void);
void SybsqlNumRows(void);
void SybsqlNextRow(void);
void SybsqlPrintRows(void);
void SybsqlResult(void);
void SybsqlSeek(void);
void SybsqlNumFields(void);
void SybsqlFieldName(void);
void SybsqlResultAll(void);
void SybsqlGetField(void);
void SybsqlExit(void);
void SybsqlCheckConnect(void);

/* pg95.c */
void PGcloseAll(void);
void PGexec(void);
void PG_result(void);
void PGconnect(void);
void PGclose(void);
void PGnumRows(void);
void PGnumFields(void);
void PGfieldName(void);
void PgfieldNum(void);
void PGfieldType(void);
void PGfieldLen(void);
void PGhost(void);
void PGdbName(void);
void PGoptions(void);
void PGport(void);
void PGtty(void);
void PGfreeResult(void);
void PGfieldNum(void);
void PGfieldPrtLen(void);
void PGfieldSize(void);
void PGgetlastoid(void);
void PGerrorMessage(void);
void php_init_pg95(void);

/* reg.c */
void RegMatch(char *, int);
void RegReplace(void);
char *_RegReplace(char *, char *, char *);
void EReg(char *, int);
void ERegReplace(void);
void ERegiReplace(void);
char *_ERegReplace(char *, char *, char *, int);
char *reg_eprint(int);

/* exec.c */
void Exec(char *, char *, int);
void EscapeShellCmd(void);

/* file.c */
/* Note: WIN32 Defines for OpenFile, Sleep and ReadFile
 * are required to avoid conflict with vc5 libraries
 */
#ifdef WINDOWS
int _OpenFile(char *, int, long *);
#ifdef WIN32
#define OpenFile _OpenFile
#endif
#else
int OpenFile(char *, int, long *);
#endif
char *GetCurrentFilename(void);
void SetCurrentFilename(char *);
long GetCurrentFileSize(void);
void SetCurrentFileSize(long);
char *GetIncludePath(void);
void SetIncludePath(char *);
char *GetAutoPrependFile();
char *GetAutoAppendFile();
char *FixFilename(char *, int, int *, int);
char *getfilename(char *, int);
void ClearStatCache(void);
void FileFunc(int);
void TempNam(void);
void Link(void);
void SymLink(void);
void ReadLink(void);
void LinkInfo(void);
void Unlink(void);
void Rename(void);
#ifdef WINDOWS
void _Sleep(void);
#ifdef WIN32
#define Sleep _Sleep
#endif
#else
void Sleep(void);
#endif
void USleep(void);
void Fopen(void);
void Fclose(void);
void Fputs(void);
void Fgets(void);
void Fgetss(void);
void Rewind(void);
void Fseek(void);
void Ftell(void);
char *GetCurrentPI(void);
void SetCurrentPI(char *);
void SetCurrentPD(char *);
void ChMod(void);
void ChOwn(void);
void ChGrp(void);
void MkDir(void);
void RmDir(void);
int  FpPush(FILE *, char *, int);
void PHPFile(void);
#if APACHE
void php_init_file(php_module_conf *);
#else
void php_init_file(void);
#endif
void set_path_dir(char *);
void Popen(void);
void Pclose(void);
void Feof(void);
void FpCloseAll(void);
#if APACHE
void Virtual(void);
#endif
#ifdef WIN32
void _ReadFile(void);
#define ReadFile _ReadFile
#else
void ReadFile(void);
#endif
void FileUmask(int);
int CheckUid(char *, int);
char *GetAutoPrependFile(void);
char *GetAutoAppendFile(void);
void FPassThru(void);

/* crypt.c */
void Crypt(int);

/* head.c */
void Header(void);
void php_header(int, char *);
void php_init_head(void);
void SetCookie(int);
void PushCookieList(char *, char *, time_t, char *, char *, int);
CookieList *PopCookieList(void);
void NoHeader(void);
void GetAllHeaders(void);

/* info.c */
void Info(void);
void ShowPageInfo(void);
void PHPVersion(void);

/* post.c */
void TreatData(int);
void parse_url(char *);
void TreatHeaders(void);

/* type.c */
int CheckType(char *);
int CheckIdentType(char *);
char *GetIdentIndex(char *);

/* conf.c */
#if APACHE
void Configuration(void);
#else
void Configuration(int, char **);
#endif

/* acc.c */
AccessInfo *StrtoAccess(char *);
void PostToAccessStr(char *);
void AddRule(char *);
void AddFile(char *, char *);
void ChkPostVars(char *);
int CheckAccess(char *, long);
void ShowBanPage(char *);
void ShowEmailPage(char *);
void ShowPasswordPage(char *);
char *getremotehostname(void);
char *getemailaddr(void);
char *getrefdoc(void);
char *getbrowser(void);
int getlogging(void);
void setlogging(int);
void SetLogging(void);
int getshowinfo(void);
void setshowinfo(int);
void SetShowInfo(void);
void GetAccDir(void);
char *getaccdir(void);
#if APACHE
void php_init_acc(php_module_conf *);
#else
void php_init_acc(void);
#endif

/* dns.c */
void GetHostByAddr(void);
const char *_GetHostByAddr(char *);
void GetHostByName(void);
char *_GetHostByName(char *);

/* log.c */
void Log(char *);
char *getlastemailaddr(void);
char *getlasthost(void);
char *getlastbrowser(void);
char *getlastref(void);
char *getlogfile(void);
time_t getlastaccess(void);
time_t getstartlogging(void);
time_t getlastmod(void);
int gettotal(void);
int gettoday(void);
void loadlastinfo(char *, char *);
void GetLastEmail(void);
void GetLastBrowser(void);
void GetLastHost(void);
void GetLastAccess(void);
void GetStartLogging(void);
void GetLastRef(void);
void GetLogFile(void);
void LogAs(void);
void GetLastMod(void);
void GetTotal(void);
void GetToday(void);
void GetLogDir(void);
void GetLogHost(void);
char *getlogdir(void);
char *getloghost(void);
void GetMyUid(void);
void GetMyInode(void);
void SetStatInfo(struct stat *);
void GetMyPid(void);
long getmyuid(void);
void SQLLog(char *);
void sqlloadlastinfo(char *);
#if APACHE
void php_init_log(php_module_conf *);
#else
void php_init_log(void);
#endif

/* sort.c */
void Sort(int,int);

/* dir.c */
void OpenDir(void);
void CloseDir(void);
void RewindDir(void);
void ReadDir(void);
void ChDir(void);
void php_init_dir(void);

/* rand.c */
void Srand(void);
void Rand(void);
void GetRandMax(void);

/* gd.c */
void ImageCreate(void);
void ImageCreateFromGif(void);
void ImageDestroy(void);
void ImageGif(int);
void ImageSetPixel(void);
void ImageLine(void);
void ImageRectangle(void);
void ImageFilledRectangle(void);
void ImageArc(void);
void ImageFillToBorder(void);
void ImageFill(void);
void ImageColorAllocate(void);
void ImageColorTransparent(void);
void ImageInterlace(void);
void ImagePolygon(int);
void ImageChar(int);
void ImageCopyResized(void);
void php_init_gd(void);
void ImageSXFN(void);
void ImageSYFN(void);

/* mime.c */
void mime_split(char *, int, char *);
#if APACHE
void php_init_mime(php_module_conf *);
#endif

/* fsock.c */
void FSockOpen(void);

/* microtime.c */
void MicroTime(void);

/* pool.c */
void *php_pool_alloc(
#if DEBUG
char *, int,
#endif
int,int);
char *php_pool_strdup(
#if DEBUG
char *, int,
#endif
int,char *);
#ifndef APACHE
void php_pool_free(int);
#endif
void php_pool_clear(int);
#if DEBUG
void php_pool_show(void);
#endif
#if APACHE
void php_init_pool(php_module_conf *);
#else
void php_init_pool(void);
void *php_palloc(struct pool *, int nbytes);
void *php_pcalloc(struct pool *, int nbytes);
char *php_pstrdup(struct pool *, const char *s);
struct pool *php_make_sub_pool(struct pool *);
void php_clear_pool(struct pool *);
void php_destroy_pool(struct pool *);
#endif
void ShowPool(void);

/* local.c */
#ifndef HAVE_STRCASECMP
int strcasecmp(char *, char *);
int strncasecmp(char *, char *, int);
#endif
#ifndef HAVE_STRDUP
char *strdup(char *);
#endif
#ifndef HAVE_STRERROR
char *strerror(int);
#endif

/* mysql.c */
void php_init_mysql(char *);
void MYsql(void);
void MYsqlResult(void);
void MYsqlClose(void);
void MYsqlConnect(int);
void MYsqlFreeResult(void);
void MYsqlNumRows(void);
void MYsqlNumFields(void);
void MYsqlField(int);
void MYsqlListTables(void);
void MYsqlListFields(void);
void MYsqlTableName(void);
void MYsqlListDBs(void);
void MYsqlDBName(void);
void MYsqlDropDB(void);
void MYsqlCreateDB(void);
void MYsqlInsertId(void);
void MYsqlAffectedRows(void);
void mysqlSetCurrent();

/* adabasd.c */
void Ada_exec(void);
void Ada_close(void);
void Ada_result(void);
void Ada_resultAll(int);
void Ada_numRows(void);
void Ada_connect(void);
void Ada_fieldNum(void);
void Ada_fetchRow(int);
void Ada_numFields(void);
void Ada_Field(int);
void Ada_freeResult(void);
void Ada_closeAll(void);
void php_init_adabas(char *,char *,char *);

/* solid.c */
void Solid_exec(void);
void Solid_close(void);
void Solid_result(void);
void Solid_numRows(void);
void Solid_connect(void);
void Solid_fieldNum(void);
void Solid_fetchRow(void);
void Solid_numFields(void);
void Solid_fieldName(void);
void Solid_freeResult(void);
void Solid_closeAll(void);
void php_init_solid(void);

/* mail.c */
void Mail(int);

/* md5.c */
void Md5(void);

/* image.c */
void GetImageSize(void);

#if PHP_SNMP_SUPPORT
/* snmp.c */
void phpsnmpget(void);
void phpsnmpwalk(void);
void phpsnmp(int);
#endif

/* oracle.c */
void Ora_Close(void);
void Ora_Commit(void);
void Ora_CommitOff(void);
void Ora_CommitOn(void);
void Ora_Exec(void);
void Ora_Fetch(void);
void Ora_GetColumn(void);
void Ora_Logoff(void);
void Ora_Logon(void);
void Ora_Open(void);
void Ora_Parse(int);
void Ora_Rollback(void);
void Ora_Bind(void);
void php_init_oracle(void);
void OraCloseAll(void);

/* filepro.c */
void filePro(void);
void filePro_rowcount(void);
void filePro_fieldcount(void);
void filePro_fieldtype(void);
void filePro_fieldname(void);
void filePro_fieldwidth(void);
void filePro_retrieve(void);

/* illustra.c */
void MIconnect(void);
void MIdbname(void);
void MIclose(void);
void MIexec(void);
void MIresult(void);
void MInumRows(void);
void MIfieldName(void);
void MIfieldNum(void);
void MInumFields(void);

/* odbc.c */
void ODBCfetch(void);
void ODBCexecdirect(void);
void ODBCgetdata(void);
void ODBCfree(void);
void ODBCconnect(void);
void ODBCdisconnect(void);
void ODBCrowcount(void);
void php_init_odbc(void);
