/* This is the default configuration file to read */
#define CONFIGURATION_FILE_PATH "php3.ini"
#define USE_CONFIG_FILE 1

/* Define if you have the gd library (-lgd). */
#define HAVE_LIBGD 0

/* Define if you want safe mode enabled by default. */
#define PHP_SAFE_MODE 0

/* Set to the path to the dir containing safe mode executables */
#define PHP_SAFE_MODE_EXEC_DIR /usr/local/bin

/* Define if you want POST/GET/Cookie track variables by default */
#define PHP_TRACK_VARS 0

/* Undefine if you want stricter XML/SGML compliance by default */
/* (this disables "<?expression?>" by default) */
#define DEFAULT_SHORT_OPEN_TAG 1

/* Undefine if you do not want PHP by default to escape "'" */
/* in GET/POST/Cookie data */
#define MAGIC_QUOTES 1

/* Define if you want the logging to go to ndbm/gdbm/flatfile */
#define LOG_DBM 0
#define LOG_DBM_DIR "."

/* Define if you want the logging to go to a mysql database */
#define LOG_MYSQL 0

/* Define if you want the logging to go to a mysql database */
#define LOG_MSQL 0

/* Define these if you are using an SQL database for logging */
#define LOG_SQL_HOST ""
#define LOG_SQL_DB ""

/* Define if you an ndbm compatible library (-ldbm).  */
#define NDBM 0

/* Define if you have the gdbm library (-lgdbm).  */
#define GDBM 0

/* Define both of these if you want the bundled REGEX library */
#define REGEX 0
#define HSREGEX 0

/* Define if you want Solid database support */
#define HAVE_SOLID 0

/* Define if you want to use the supplied dbase library */
#define DBASE 0

/* Define if you have the crypt() function */
#define HAVE_CRYPT 1

/* Define if you have the Oracle database client libraries */
#define HAVE_ORACLE 0

/* Define if you want to use the iODBC database driver */
#define HAVE_IODBC 0

/* Define if you have the AdabasD client libraries */
#define HAVE_ADABAS 0

/* Define if you want the LDAP directory interface */
#define HAVE_LDAP 0

/* Define to use the unified ODBC interface */
#define HAVE_UODBC 0

/* Define if you have libdl (used for dynamic linking) */
#define HAVE_LIBDL 0

/* Define if you have libdnet_stub (used for Sybase support) */
#define HAVE_LIBDNET_STUB 0

/* Define if you have and want to use libcrypt */
#define HAVE_LIBCRYPT 0

/* Define if you have and want to use libnsl */
#define HAVE_LIBNSL 0

/* Define if you have and want to use libsocket */
#define HAVE_LIBSOCKET 0

/* Define if you have the sendmail program available */
#define HAVE_SENDMAIL 0

/* Define if you are compiling PHP as an Apache module */
#define APACHE 0

#define HAVE_SYBASE 0
#define HAVE_SYBASE_CT 0

#ifndef HAVE_MYSQL
#define HAVE_MYSQL 0
#endif

#ifndef HAVE_MSQL
#define HAVE_MSQL 0
#endif

#ifndef HAVE_PGSQL
#define HAVE_PGSQL 0
#endif

#define MSQL1 0
#define HAVE_FILEPRO 0
#define HAVE_SOLID 0
#define DEBUG 0

/* Define if your system has the gettimeofday() call */
#define HAVE_GETTIMEOFDAY 0

/* Define if your system has the putenv() library call */
#define HAVE_PUTENV 0

/* Define if you want to enable the PHP TCP/IP debugger (experimental) */
#define PHP_DEBUGGER 0

/* Define if you want to enable bc style precision math support */
#define WITH_BCMATH 0

/* Define if you want to prevent the CGI from working unless REDIRECT_STATUS is defined in the environment */
#define FORCE_CGI_REDIRECT 0

/* Define if you want to enable memory limit support */
#define MEMORY_LIMIT 0

/* Define if you want include() and other functions to be able to open
 * http and ftp URLs as files.
 */
#define PHP3_URL_FOPEN 0

/* Define if you have broken header files like SunOS 4 */
#define MISSING_FCLOSE_DECL 0
