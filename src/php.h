/***[php.h]*******************************************************[TAB=4]****\
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
/* $Id: php.h,v 1.73 1996/06/01 02:21:43 rasmus Exp $ */
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
#if APACHE
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
#define DEBUG_FILE	"/tmp/php.err"

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
 * query.  
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
 * MSQLLOGDB should be set to the database name you wish to use to store
 * log data if you are using PHP/FI with mSQL-based logging.  Remember
 * to create this database before trying to run PHP/FI in mSQL logging mode.
 *
 * NOTE: For Apache module, set this via phpMsqlLogDB configuration directive
 */
#define MSQLLOGDB "phpfi"

/*
 * MSQLLOGTMP is set to the directory where you want temporary lock
 * files created when MSQL LOGGING is used.  Since mSQL does not support
 * atomic updates of counters, an external lock file unfortunately needs
 * to be used.  With mSQL-2, these lock files can be eliminated, but at
 * the time of this writing, mSQL-2 is not available as of yet.
 */
#define MSQLLOGTMP "/tmp"

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
 * Max size of a single line of input in the HTML files
 */
#define LINEBUFSIZE	4096

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
 */
#define DEFAULT_MAX_DATA_SPACE 8192

/*-- Do not touch anything after this point unless you are very brave --*/

#define PHP_VERSION "1.99s"

#define VAR_INIT_CHAR	'$'

#if APACHE
#if APACHE_NEWAPI
#define PUTS(a) rputs((a),php_rqst)
#else
#define PUTS(a) rprintf(php_rqst,"%s",(a))
#endif
#else
#define PUTS(a) fputs((a),stdout)
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

#define MAX_CMD_LEN 21

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
	unsigned char *name;
	unsigned char *strval;
	unsigned char *iname;
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
	unsigned char *strval;
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

typedef struct FpStack {
	FILE *fp;
	char *filename;
	int id;
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
void DefineFunc(unsigned char *);
FuncStack *FindFunc(char *, long *, VarTree **);
void RunFunc(unsigned char *);
VarTree *GetFuncFrame(void);
void AddToArgList(unsigned char *);
FuncArgList *GetFuncArgList(void);
void ClearFuncArgList(void);
void Return(void);
void php_init_lex(void);
void IntFunc(unsigned char *);
int NewWhileIteration(void);
void Eval(void);
void set_text_magic(int);

/* date.c */
void Date(int, int);
void UnixTime(void);
void MkTime(int);

/* parse.c */
int yyparse(void);
void php_init_yacc(void);

/* calc.c */
int Calc(int);
int CalcInc(int);
void Neg(void);
void BinDec(void);
void DecBin(void);
void DecHex(void);
void HexDec(void);
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

/* stack.c */
void Push(unsigned char *, int);
Stack *Pop(void);
void ClearStack(void);
void php_init_stack(void);

/* var.c */
void php_init_symbol_tree(void);
void SetVar(unsigned char *, int, int);
VarTree *GetVar(unsigned char *, unsigned char *, int);
void IsSet(unsigned char *);
char *SubVar(char *);
void Count(void);
void ArrayMax(void);
void ArrayMin(void);
void PutEnv(void);
void GetEnv(void);
void PtrPush(void *);
void *PtrPop(void);
void SecureVar(void);
void Reset(unsigned char *);
void Key(unsigned char *);
void Next(unsigned char *);
void Prev(unsigned char *);
void End(unsigned char *);
void PushStackFrame(void);
void PopStackFrame(void);
void Global(void);
void copyarray(VarTree *, VarTree *);
void deletearray(VarTree *);
void UnSet(unsigned char *);

/* echo.c */
void Echo(unsigned char *, int);
void StripSlashes(char *);
char *AddSlashes(char *, int);
void ParseEscapes(char *);
void HtmlSpecialChars(void);
int FormatCheck(char **, char **, char **);

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
int _dbmOpen(char *, char *);
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
void While(void);
void EndWhile(void);
void PushWhileMark(void);
void PopWhileMark(void);
void php_init_while(void);

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
void Ord(void);
void QuoteMeta(void);
void UcFirst(void);
void Sprintf(void);
void Chr(void);

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
void MsqlTableName(void);
void MsqlListDBs(void);
void MsqlDBName(void);
void MsqlDropDB(void);
void MsqlCreateDB(void);
void php_init_msql(void);

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
void php_init_pg95(void);

/* reg.c */
void RegMatch(unsigned char *);
void RegSearch(unsigned char *);
void RegReplace(void);
char *_RegReplace(char *, char *, char *);

/* exec.c */
void Exec(unsigned char *, unsigned char *, int);
void EscapeShellCmd(void);

/* file.c */
int OpenFile(char *, int, long *);
char *GetCurrentFilename(void);
void SetCurrentFilename(char *);
long GetCurrentFileSize(void);
void SetCurrentFileSize(long);
char *FixFilename(char *, int, int *);
char *getfilename(char *, int);
void FileFunc(int);
void TempNam(void);
void Unlink(void);
void Rename(void);
void Sleep(void);
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
void ChMod(void);
void ChOwn(void);
void ChGrp(void);
void MkDir(void);
int  FpPush(FILE *, char *);
void File(void);
void php_init_file(void);
void set_path_dir(char *);
void Popen(void);
void Pclose(void);
void Feof(void);

/* crypt.c */
void Crypt(int);

/* head.c */
void Header(void);
void php_header(int, char *);
void php_init_head(void);

/* info.c */
void Info(void);
void ShowPageInfo(void);
void PHPVersion(void);

/* post.c */
void TreatData(int);
void parse_url(char *);

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
void GetLastMod(void);
void GetTotal(void);
void GetToday(void);
void GetLogDir(void);
char *getlogdir(void);
void GetMyUid(void);
void GetMyInode(void);
void SetStatInfo(struct stat *);
void GetMyPid(void);
long getmyuid(void);
void MsqlLog(char *);
void msqlloadlastinfo(char *);
#if APACHE
void php_init_log(php_module_conf *);
#else
void php_init_log(void);
#endif

/* sort.c */
void Sort(void);

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

