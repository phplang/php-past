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
   | Authors: Paul Panotzki - Bunyip Information Systems                  |
   +----------------------------------------------------------------------+
 */

/* $Id: microtime.c,v 1.38 2000/02/26 09:03:55 bjh Exp $ */

#include "php.h"
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#include <string.h>
#include <errno.h>

#include "tls.h"
#include "internal_functions.h"
#include "microtime.h"
#include "snprintf.h"

#include <stdio.h>
#if HAVE_GETTIMEOFDAY
#if WIN32
#include "win32/time.h"
#else
#include <sys/time.h>
#endif
#endif

#define NUL  '\0'
#define MICRO_IN_SEC 1000000.00

#if OS2
#define INCL_DOSPROFILE
#include <os2.h>
#endif

/* {{{ proto string microtime(void)
   Returns a string containing the current time in seconds and microseconds */
#ifdef __cplusplus
void php3_microtime(HashTable *)
#else
void php3_microtime(INTERNAL_FUNCTION_PARAMETERS)
#endif
{
#if HAVE_GETTIMEOFDAY
	struct timeval tp;
	char ret[100];

#if OS2
    static ULONG hrtimerHz = 0;
    /* Number of hrtimerHz between 1 Jan 1970 & base of DosTmrQueryTime() */
    static unsigned long long hradjust; 
    QWORD qwNow;
    unsigned long long llNow;

    if (hrtimerHz == 0) {
        DosTmrQueryFreq(&hrtimerHz);
        gettimeofday(&tp, NULL);
        DosTmrQueryTime(&qwNow);
        llNow = ((unsigned long long)qwNow.ulHi << 32) | qwNow.ulLo;
        hradjust = (unsigned long long)tp.tv_sec * hrtimerHz + (tp.tv_usec * hrtimerHz / MICRO_IN_SEC) - llNow;
    }

    DosTmrQueryTime(&qwNow);
    llNow = ((unsigned long long)qwNow.ulHi << 32) | qwNow.ulLo;
    llNow += hradjust;
    snprintf(ret, 100, "%.8f %ld", (double)(llNow % hrtimerHz)/hrtimerHz, llNow / hrtimerHz);
    RETVAL_STRING(ret,1);
#else
	long sec = 0L;
	double msec = 0.0;
	TLS_VARS;
	
	if (gettimeofday((struct timeval *) &tp, (NUL)) == 0) {
		msec = (double) (tp.tv_usec / MICRO_IN_SEC);
		sec = tp.tv_sec;
	}
	if (msec >= 1.0) msec -= (long) msec;
	snprintf(ret, 100, "%.8f %ld", msec, sec);
	RETVAL_STRING(ret,1);
#endif
#endif
}
/* }}} */

/* {{{ proto array gettimeofday(void)
   Returns the current time as array */
void php3_gettimeofday(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_gettimeofday(return_value);
}

void _php3_gettimeofday(pval *return_value) {
#if HAVE_GETTIMEOFDAY
	struct timeval tp;
	struct timezone tz;
	
	memset(&tp, 0, sizeof(tp));
	memset(&tz, 0, sizeof(tz));
	if(gettimeofday(&tp, &tz) == 0) {
		array_init(return_value);
		add_assoc_long(return_value, "sec", tp.tv_sec);
		add_assoc_long(return_value, "usec", tp.tv_usec);
		add_assoc_long(return_value, "minuteswest", tz.tz_minuteswest);
		add_assoc_long(return_value, "dsttime", tz.tz_dsttime);
	} else {
		RETURN_FALSE;
	}
#endif
}
/* }}} */

/* {{{ proto array getrusage([int who])
   Returns an array of usage statistics */
void php3_getrusage(INTERNAL_FUNCTION_PARAMETERS)
{
#if HAVE_GETRUSAGE
	struct rusage usg;
	int ac = ARG_COUNT(ht);
	pval *pwho;
	int who = RUSAGE_SELF;

	if(ac == 1 &&
		getParameters(ht, ac, &pwho) != FAILURE) {
		convert_to_long(pwho);
		if(pwho->value.lval == 1)
			who = RUSAGE_CHILDREN;
	}

	memset(&usg, 0, sizeof(usg));
	if(getrusage(who, &usg) == -1) {
		RETURN_FALSE;
	}

	array_init(return_value);
#define PHP3_RUSAGE_PARA(a) \
		add_assoc_long(return_value, #a, usg.a)
#ifndef _OSD_POSIX /* BS2000 has only a few fields in the rusage struct */
	PHP3_RUSAGE_PARA(ru_oublock);
	PHP3_RUSAGE_PARA(ru_inblock);
	PHP3_RUSAGE_PARA(ru_msgsnd);
	PHP3_RUSAGE_PARA(ru_msgrcv);
	PHP3_RUSAGE_PARA(ru_maxrss);
	PHP3_RUSAGE_PARA(ru_ixrss);
	PHP3_RUSAGE_PARA(ru_idrss);
	PHP3_RUSAGE_PARA(ru_minflt);
	PHP3_RUSAGE_PARA(ru_majflt);
	PHP3_RUSAGE_PARA(ru_nsignals);
	PHP3_RUSAGE_PARA(ru_nvcsw);
	PHP3_RUSAGE_PARA(ru_nivcsw);
#endif /*_OSD_POSIX*/
	PHP3_RUSAGE_PARA(ru_utime.tv_usec);
	PHP3_RUSAGE_PARA(ru_utime.tv_sec);
	PHP3_RUSAGE_PARA(ru_stime.tv_usec);
	PHP3_RUSAGE_PARA(ru_stime.tv_sec);
#undef PHP3_RUSAGE_PARA
#endif /* HAVE_GETRUSAGE */
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
