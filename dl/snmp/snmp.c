/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
/* $Id: snmp.c,v 1.21 1999/06/16 11:34:16 ssb Exp $ */

#ifndef __P
#define __P(p) p
#endif

#include "php.h"
#if COMPILE_DL
#include "phpdl.h"
#include "functions/dl.h"
#endif
#include "php3_snmp.h"
#include <sys/types.h>
#if MSVC5
#include <winsock.h>
#include <errno.h>
#include <process.h>
#include "win32/time.h"
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <netdb.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "asn1.h"
#include "snmp_api.h"
#include "snmp_client.h"
#include "snmp_impl.h"
#include "snmp.h"
#include "parse.h"

extern int  errno;

/* constant - can be shared among threads */
static oid objid_mib[] = {1, 3, 6, 1, 2, 1};

/* Add missing prototype */
void sprint_variable(char *, oid *, int, struct variable_list *);

function_entry snmp_functions[] = {
    {"snmpget", php3_snmpget, NULL},
    {"snmpwalk", php3_snmpwalk, NULL},
    {NULL,NULL,NULL}
};

php3_module_entry snmp_module_entry = {
	"SNMP",snmp_functions,NULL,NULL,NULL,NULL,NULL,0,0,0,NULL
};

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module() { return &snmp_module_entry; };
#endif

/* THREAD_LS snmp_module php3_snmp_module; - may need one of these at some point */

/*
 * Generic SNMP object fetcher
 *
 * st=1 GET
 * st=2 WALK
 */
void _php3_snmp(INTERNAL_FUNCTION_PARAMETERS, int st) {
	pval *a1, *a2, *a3;
	struct snmp_session session, *ss;
	struct snmp_pdu *pdu=NULL, *response;
	struct variable_list *vars;
	char *community, *hostname;
    char *objid;
    oid name[MAX_NAME_LEN];
    int name_length;
    int status, count,rootlen=0,gotroot=0;
	oid root[MAX_NAME_LEN];
	char buf[2048];
	int keepwalking=1;
	static int mib_init=0;

	if (getParameters(ht, 3, &a1, &a2, &a3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(a1);
	convert_to_string(a2);
	convert_to_string(a3);

	hostname=a1->value.str.val;
	community=a2->value.str.val;
	objid=a3->value.str.val;

	if (!mib_init) {
		init_mib();
		mib_init=1;
	}

	if (st==2) { /* walk */
		rootlen = MAX_NAME_LEN;
		if (strlen(objid)) { /* on a walk, an empty string means top of tree - no error */
			if (read_objid(objid, root, &rootlen)) {
				gotroot = 1;
			} else {
				php3_error(E_WARNING,"Invalid object identifier: %s\n", objid);
			}
		}
    	if (gotroot == 0) {
			memmove((char *)root, (char *)objid_mib, sizeof(objid_mib));
        	rootlen = sizeof(objid_mib) / sizeof(oid);
        	gotroot = 1;
    	}
	}

	memset(&session, 0, sizeof(struct snmp_session));
	session.peername = hostname;

	session.version = SNMP_VERSION_1;
	session.community = (u_char *)community;
	session.community_len = strlen((char *)community);
	session.retries = SNMP_DEFAULT_RETRIES;
	session.timeout = SNMP_DEFAULT_TIMEOUT;

	session.authenticator = NULL;
	snmp_synch_setup(&session);
	ss = snmp_open(&session);
	if (ss == NULL){
		php3_error(E_WARNING,"Couldn't open snmp\n");
		RETURN_FALSE;
	}
	if (st==2) {
		memmove((char *)name, (char *)root, rootlen * sizeof(oid));
		name_length = rootlen;
		/* prepare result array */
		array_init(return_value);	
	}

	while(keepwalking) {
		keepwalking=0;
		if (st==1) pdu = snmp_pdu_create(GET_REQ_MSG);
		else if (st==2) pdu = snmp_pdu_create(GETNEXT_REQ_MSG);

		if (st==1) {
			name_length = MAX_NAME_LEN;
			if (!read_objid(objid, name, &name_length)) {
				php3_error(E_WARNING,"Invalid object identifier: %s\n", objid);
				RETURN_FALSE;
			}
		}
		snmp_add_null_var(pdu, name, name_length);

retry:
		status = snmp_synch_response(ss, pdu, &response);
		if (status == STAT_SUCCESS) {
			if (response->errstat == SNMP_ERR_NOERROR) {
				for(vars = response->variables; vars; vars = vars->next_variable) {
					if (st==2 && (vars->name_length < rootlen || memcmp(root, vars->name, rootlen * sizeof(oid))))
						continue;       /* not part of this subtree */

					sprint_value(buf,vars->name, vars->name_length, vars);
#if 0
					Debug("snmp response is: %s\n",buf);
#endif
					if (st==1) {
						RETVAL_STRING(buf,1);
					} else if (st==2) {
						/* Add to returned array */
						add_next_index_string(return_value,buf,1);
						if (vars->type != SNMP_ENDOFMIBVIEW && vars->type != SNMP_NOSUCHOBJECT && vars->type != SNMP_NOSUCHINSTANCE) {
							memmove((char *)name, (char *)vars->name,vars->name_length * sizeof(oid));
							name_length = vars->name_length;
							keepwalking = 1;
						}
					}
				}	
			} else {
				if (st!=2 || response->errstat != SNMP_ERR_NOSUCHNAME) {
					php3_error(E_WARNING,"Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
					if (response->errstat == SNMP_ERR_NOSUCHNAME) {
						for(count=1, vars = response->variables; vars && count != response->errindex;
							vars = vars->next_variable, count++);
						if (vars) sprint_objid(buf,vars->name, vars->name_length);
						php3_error(E_WARNING,"This name does not exist: %s\n",buf);
					}
					if (st==1) {
						if ((pdu = snmp_fix_pdu(response, GET_REQ_MSG)) != NULL) goto retry;
					} else if (st==2) {
						if ((pdu = snmp_fix_pdu(response, GETNEXT_REQ_MSG)) != NULL) goto retry;
					}
					RETURN_FALSE;
				}
			}
		} else if (status == STAT_TIMEOUT) {
			php3_error(E_WARNING,"No Response from %s\n", hostname);
			RETURN_FALSE;
		} else {    /* status == STAT_ERROR */
			php3_error(E_WARNING,"An error occurred, Quitting\n");
			RETURN_FALSE;
		}
		if (response) snmp_free_pdu(response);
	} /* keepwalking */
	snmp_close(ss);
}

DLEXPORT void php3_snmpget(INTERNAL_FUNCTION_PARAMETERS) {
	_php3_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}

DLEXPORT void php3_snmpwalk(INTERNAL_FUNCTION_PARAMETERS) {
	return _php3_snmp(INTERNAL_FUNCTION_PARAM_PASSTHRU,2);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
