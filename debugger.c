/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
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
   | Authors: Stig Sæther Bakken <ssb@fast.no>                            |
   +----------------------------------------------------------------------+
 */

/* $Id: debugger.c,v 1.72 2000/01/01 04:31:12 sas Exp $ */
#include "php.h"
#include "internal_functions.h"
#include "modules.h"
#include "main.h"

#if PHP_DEBUGGER

#define DEBUGGER_FAIL_SILENTLY

#include <sys/types.h>
#include <errno.h>

#if MSVC5
# include <winsock.h>
# include <process.h>
# include <direct.h>
# include "win32/time.h"
#define PATH_MAX MAX_PATH
#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#include <time.h>
#include "php3_debugger.h"
#include "functions/fsock.h"

extern int lookup_hostname(const char *addr, struct in_addr *in);
int _php3_send_error(char *message, char *address);

static int debug_socket = 0;
static char *myhostname = NULL;
static char *currenttime = NULL;
extern Stack function_state_stack;
char *debugger_host = NULL;
long debugger_port = 0;
long debugger_default = 0;
long debugger_on = 0;
#if WIN32|WINNT
static int mypid=0;
#else
static pid_t mypid = 0;
#endif

#if WIN32|WINNT
#define SCLOSE(a) closesocket(a)
#define SSEND(a,b,c) send(a,b,c,0)
#else
#define SCLOSE(a) close(a)
#define SSEND(a,b,c) write(a,b,c)
#endif


function_entry debugger_functions[] = {
	{"debugger_on", php3_debugger_on, NULL},
	{"debugger_off", php3_debugger_off, NULL},
	{NULL,NULL,NULL}
};

/* We'll call the shutdown just in case someone
   doesn't do it in their script.  Doesn't hurt either way. */
php3_module_entry debugger_module_entry = {
	"Debugger",
	debugger_functions,
	php3_minit_debugger,
	php3_mshutdown_debugger,
	php3_rinit_debugger,
	php3_rshutdown_debugger,
	NULL, 0, 0, 0, NULL
};

/*
 * Connects to a specified host and port, and returns a file
 * descriptor for the connection.
 */
static int create_debugger_socket(const char *hostname, int dport)
{

	struct sockaddr_in address;
	int err = -1;
	int sockfd;

	memset(&address, 0, sizeof(address));
	lookup_hostname(hostname, &address.sin_addr);
	address.sin_family = AF_INET;
	address.sin_port = htons((unsigned short)dport);

	sockfd = socket(address.sin_family, SOCK_STREAM, 0);
	if (sockfd == SOCK_ERR) {
#ifndef DEBUGGER_FAIL_SILENTLY
#if WIN32|WINNT
		php3_printf("create_debugger_socket(\"%s\", %d) socket: %d\n",
					hostname, dport, WSAGetLastError());
#else
		php3_printf("create_debugger_socket(\"%s\", %d) socket: %s\n",
					hostname, dport, strerror(errno));
#endif
#endif
		return -1;
	}
	while ((err = connect(sockfd, (struct sockaddr *) &address,
						  sizeof(address))) == SOCK_ERR && errno == EAGAIN);

	if (err < 0) {
#ifndef DEBUGGER_FAIL_SILENTLY
#if WIN32|WINNT
		php3_printf("create_debugger_socket(\"%s\", %d) connect: %d\n",
				   hostname, dport, WSAGetLastError());
#else
		php3_printf("create_debugger_socket(\"%s\", %d) connect: %s\n",
				   hostname, dport, strerror(errno));
#endif
#endif
		SCLOSE(sockfd);
		return -1;
	}
	return sockfd;
}


static char *
find_hostname(void)
{
	char tmpname[33];
	int err;
	
	memset(tmpname, 0, sizeof(tmpname));
	err = gethostname(tmpname, sizeof(tmpname) - 1);
	if (err == -1) {
		return NULL;
	}
	return (char *) strdup(tmpname);
}


static char *
get_current_time(void)
{
	static char debug_timebuf[50];
	char microbuf[10];
#if HAVE_GETTIMEOFDAY
	struct timeval tv;
	struct timezone tz;
#endif
	const struct tm *tm;
	size_t len;
	time_t t;
	TLS_VARS;

	memset(STATIC(debug_timebuf), 0, sizeof(STATIC(debug_timebuf)));
	t = time(NULL);
	tm = localtime((const time_t *) &t);
	len = strftime(STATIC(debug_timebuf), (sizeof(STATIC(debug_timebuf)) - sizeof(microbuf) - 1),
				   "%Y-%m-%d %H:%M", tm);

#if HAVE_GETTIMEOFDAY
	gettimeofday(&tv, &tz);
	snprintf(microbuf, sizeof(microbuf) - 1, ":%06d", tv.tv_usec);
	strcat(STATIC(debug_timebuf), microbuf);
#endif
	return STATIC(debug_timebuf);
}


static void debugger_message(char *msg)
{
	TLS_VARS;
	
	if (GLOBAL(debug_socket) > 0) {
		SSEND(GLOBAL(debug_socket), msg, strlen(msg));
	}
}

static void debugger_send_string(char *field, char *data)
{
	char buf[1025];
	TLS_VARS;
	
	fflush(stdout);
	snprintf(buf, 1024, "%s %s(%d) %s: %s\n\0", GLOBAL(currenttime),
			 GLOBAL(myhostname), GLOBAL(mypid), field, data);
	debugger_message(buf);
}

static void debugger_send_int(char *field, int data)
{
	char buf[1025];
	TLS_VARS;
	
	snprintf(buf, 1024, "%s %s(%d) %s: %d\n\0", GLOBAL(currenttime),
			 GLOBAL(myhostname), GLOBAL(mypid), field, data);
	debugger_message(buf);
}


/*
 * Functions accessible from other object files:
 */

int php3_minit_debugger(INIT_FUNC_ARGS)
{
	TLS_VARS;

	if (cfg_get_string("debugger.host", &GLOBAL(debugger_host)) == FAILURE) {
		GLOBAL(debugger_host) = "localhost";
	}
	if (cfg_get_long("debugger.port", &GLOBAL(debugger_port)) == FAILURE) {
		GLOBAL(debugger_port) = 7869;
	}
	if (cfg_get_long("debugger.enabled", &GLOBAL(debugger_default)) == FAILURE) {
		GLOBAL(debugger_default) = 0;
	}

	GLOBAL(myhostname) = find_hostname();
	GLOBAL(mypid) = getpid();

	return SUCCESS;
}


int php3_mshutdown_debugger(void)
{
	TLS_VARS;

	if (GLOBAL(myhostname)) {
		free(GLOBAL(myhostname));
	}
	return SUCCESS;
}

int php3_rinit_debugger(INIT_FUNC_ARGS)
{
	TLS_VARS;

	if (GLOBAL(debugger_default)) {
		php3_start_debugger(NULL);
	}

	return SUCCESS;
}


int php3_rshutdown_debugger(void)
{
	TLS_VARS;
	
	php3_stop_debugger();
	return SUCCESS;
}


int php3_start_debugger(char *ip)
{
	TLS_VARS;
	
	if (!GLOBAL(debugger_on)) {
		if (!ip) {
			GLOBAL(debug_socket) = create_debugger_socket(GLOBAL(debugger_host),
														  GLOBAL(debugger_port));
		} else {
			GLOBAL(debug_socket) = create_debugger_socket(ip, GLOBAL(debugger_port));
		}
		
		if (GLOBAL(debug_socket) < 0) {
			GLOBAL(debugger_on) = 0;
			return FAILURE;
		} else {
			GLOBAL(debugger_on) = 1;
		}
	}

	return SUCCESS;
}


int php3_stop_debugger(void)
{
	TLS_VARS;
	
	if (GLOBAL(debug_socket) > 0) {
		SCLOSE(GLOBAL(debug_socket));
	}
	return SUCCESS;
}

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

void php3_debugger_frame_location(FunctionState *fs, int level)
{
	int line_number;
	char *file_name;
	char location[PATH_MAX+20];
	char cwd[PATH_MAX];

	if (!fs->lineno) {
		return;
	}

	line_number = php3_get_lineno(fs->lineno);
	file_name = php3_get_filename(fs->lineno);

	if (strchr(file_name, '/') == NULL && getcwd(cwd, PATH_MAX) != NULL) {
		snprintf(location, sizeof(location),
				 "%s/%s:%d", cwd, file_name, line_number);
	} else {
		snprintf(location, sizeof(location),
				 "%s:%d", file_name, line_number);
	}

	debugger_send_string("function", fs->function_name);
	debugger_send_string("location", location);
}


void php3_debugger_error(char *message, int type, char *filename, int lineno)
{
	char location[PATH_MAX+20], cwd[PATH_MAX], errtype[20];
	int depth = 0;
	TLS_VARS;

	GLOBAL(currenttime) = get_current_time();

	if (GLOBAL(debug_socket) <= 0) {
		return;
	}
	switch (type) {
		case E_WARNING:
			strcpy(errtype, "warning");
			break;
		case E_ERROR:
			strcpy(errtype, "error");
			break;
		case E_PARSE:
			strcpy(errtype, "parse");
			break;
		case E_NOTICE:
			strcpy(errtype, "notice");
			break;
		case E_CORE_ERROR:
			strcpy(errtype, "core-error");
			break;
		case E_CORE_WARNING:
			strcpy(errtype, "core-warning");
			break;
		default:
			strcpy(errtype, "unknown");
			break;
	}

	depth = GLOBAL(function_state_stack).top;
	if (filename && strchr(filename, '/') == NULL &&
		getcwd(cwd, PATH_MAX) != NULL) {
		snprintf(location, sizeof(location), "%s/%s:%d", cwd, filename, lineno);
	} else {
		snprintf(location, sizeof(location), "%s:%d", filename, lineno);
	}

	debugger_send_string("start", errtype);
	debugger_send_string("message", message);
	debugger_send_string("location", location);

	depth = GLOBAL(function_state_stack).top;

	if (depth > 0) {
		FunctionState *funcstate;
		debugger_send_int("frames", depth);
		php3_debugger_frame_location(&GLOBAL(function_state), depth);
		while (--depth >= 0) {
			funcstate = GLOBAL(function_state_stack).elements[depth];
			php3_debugger_frame_location(funcstate, depth);
		}
	}
	debugger_send_string("end", errtype);
}

/* {{{ proto int debugger_on(string ip_address)
   Enable internal PHP debugger */
/*FIXME
  we also need a way to dissable the cgi timer so we dont
  time out while debugging.
*/
void php3_debugger_on(INTERNAL_FUNCTION_PARAMETERS){
	pval *ip;
	TLS_VARS;
	
	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&ip) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(ip);

	if (php3_start_debugger(ip->value.str.val) == SUCCESS) {
		RETURN_TRUE;
	}
	RETVAL_FALSE;
}
/* }}} */

/* {{{ proto int debugger_off(void)
   Disable internal PHP debugger */
void php3_debugger_off(INTERNAL_FUNCTION_PARAMETERS){
	TLS_VARS;
	
	if (GLOBAL(debugger_on)) {
		GLOBAL(debugger_on) = 0;
		php3_stop_debugger();
	}
}
/* }}} */


/*FIXME
  this function gets called after the execution of each
  line, if the debugger flag is set

  we want to send various info that can be used by a full
  debugger like line number and variable values.

  we will pause and wait for acknowledgement from the
  debugger, so the debugger can do stepping.

*/
void send_debug_info(void) {
	
}


/*  
	sends a log message to an ip address 
	This is seperate from the rest of the debugger
	stuff, but I felt this was a more apropriate
	place to put it, as basicly its related to
	that.  However, this will be used by error_log
	in basic_functions.c as a way for the user-level
	script to send a message to a debugger.

	It is seperated out to avoid interference with
	anything we might do with the regular debugger.
*/    
int _php3_send_error(char *message, char *hostaddr){
	struct sockaddr_in address;
	int err = -1;
	int sockfd;
	int dport;
	char *hostname,*delim;

	/*hostaddr is expected in this form: name:port*/
	if (!(delim=strchr(hostaddr,(int)(":")))){
		return 0;
	}
	
	hostname=estrndup(hostaddr,delim-hostaddr-1);
	dport=atoi(delim+1);

	memset(&address, 0, sizeof(address));
	lookup_hostname(hostname, &address.sin_addr);
	address.sin_family = AF_INET;
	address.sin_port = htons((unsigned short)dport);

	sockfd = socket(address.sin_family, SOCK_STREAM, 0);
	if (sockfd == SOCK_ERR) {
		php3_error(E_WARNING,"Couln't create socket!");
		return 0;
	}

	while ((err = connect(sockfd, (struct sockaddr *) &address,
						  sizeof(address))) == SOCK_ERR && errno == EAGAIN);
	
	if (err < 0) {
		SCLOSE(sockfd);
		return 0;
	}

	if (! SSEND(sockfd, message, strlen(message))){
		return 0;
	}

	SCLOSE(sockfd);

	efree(hostname);
	return 0;
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
