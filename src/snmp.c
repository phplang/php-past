/***[snmp.c]******************************************************[TAB=4]****\
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
/* $Id: snmp.c,v 1.6 1997/08/12 15:37:13 ssb Exp $ */
/*
 * Portions of this file contain code which was originally part of the
 * CMU SNMP project.  The following copyright notice applies
 */
/***********************************************************************
	Copyright 1988, 1989, 1991, 1992 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
#include "php.h"
#if PHP_SNMP_SUPPORT
#include "parse.h"
/* conflict - work around */
#define PHPSTRING 260
#undef STRING
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#ifdef TM_IN_SYS_TIME
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <netdb.h>

#include "snmp.h"
#include "mib.h"
#include "asn1.h"
#include "snmp_impl.h"
#include "snmp_api.h"
#include "snmp_client.h"
#include "party.h"
#include "context.h"
#include "view.h"
#include "acl.h"

extern int  errno;
int	snmp_dump_packet = 0;
oid objid_mib[] = {1, 3, 6, 1, 2, 1};
/* Add missing prototype */
void sprint_variable(char *, oid *, int, struct  variable_list *);

/*
 * Genetic SNMP object fetcher
 *
 * st=1 GET
 * st=2 WALK
 */
void phpsnmp(int st) {
	Stack *s;
	VarTree *var;
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

	s = Pop();
	if(!s) {
		Error("Stack error in snmpget()\n");
		return;
	}
	objid=estrdup(1,s->strval);
	s = Pop();
	if(!s) {
		Error("Stack error in snmpget()\n");
		return;
	}
	community=estrdup(1,s->strval);
	s = Pop();
	if(!s) {
		Error("Stack error in snmpget()\n");
		return;
	}
	hostname=estrdup(1,s->strval);

	if(!mib_init) {
		init_mib();
		mib_init=1;
	}
	if(st==2) {
		rootlen = MAX_NAME_LEN;
		if(read_objid(objid, root, &rootlen)) {
			gotroot = 1;
		} else {
			Error("Invalid object identifier: %s\n", objid);
		}
    	if(gotroot == 0) {
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
		Error("Couldn't open snmp\n");
		return;
	}
	if(st==2) {
		memmove((char *)name, (char *)root, rootlen * sizeof(oid));
		name_length = rootlen;
		/* prepare temporary result array */
		var = GetVar("__snmptmp__",NULL,0);
		if(var) deletearray(var);
	}

	while(keepwalking) {
	keepwalking=0;
	if(st==1) pdu = snmp_pdu_create(GET_REQ_MSG);
	else if(st==2) pdu = snmp_pdu_create(GETNEXT_REQ_MSG);

	if(st==1) {
		name_length = MAX_NAME_LEN;
		if(!read_objid(objid, name, &name_length)) {
			Error("Invalid object identifier: %s\n", objid);
			return;
		}
	}
	snmp_add_null_var(pdu, name, name_length);

retry:
	status = snmp_synch_response(ss, pdu, &response);
	if(status == STAT_SUCCESS) {
		if(response->errstat == SNMP_ERR_NOERROR) {
			for(vars = response->variables; vars; vars = vars->next_variable) {
				if(st==2 && (vars->name_length < rootlen || memcmp(root, vars->name, rootlen * sizeof(oid))))
					continue;       /* not part of this subtree */

				sprint_value(buf,vars->name, vars->name_length, vars);
#if DEBUG
				Debug("snmp response is: %s\n",buf);
#endif
				Push(buf,PHPSTRING);
				if(st==2) SetVar("__snmptmp__",1,0);
				if(st==2) {
				if (vars->type != SNMP_ENDOFMIBVIEW && vars->type != SNMP_NOSUCHOBJECT && vars->type != SNMP_NOSUCHINSTANCE) {
					memmove((char *)name, (char *)vars->name,vars->name_length * sizeof(oid));
					name_length = vars->name_length;
					keepwalking = 1;
				}
				}
			}	
		} else {
			if(st!=2 || response->errstat != SNMP_ERR_NOSUCHNAME) {
				Error("Error in packet.\nReason: %s\n", snmp_errstring(response->errstat));
				if(response->errstat == SNMP_ERR_NOSUCHNAME) {
					for(count=1, vars = response->variables; vars && count != response->errindex;
						vars = vars->next_variable, count++);
					if(vars) sprint_objid(buf,vars->name, vars->name_length);
					Error("This name does not exist: %s\n",buf);
				}
				if(st==1) {
					if ((pdu = snmp_fix_pdu(response, GET_REQ_MSG)) != NULL) goto retry;
				} else if(st==2) {
					if ((pdu = snmp_fix_pdu(response, GETNEXT_REQ_MSG)) != NULL) goto retry;
				}
				Push("",PHPSTRING);
			}
		}
	} else if(status == STAT_TIMEOUT) {
		Error("No Response from %s\n", hostname);
		Push("",PHPSTRING);
	} else {    /* status == STAT_ERROR */
		Error("An error occurred, Quitting\n");
		Push("",PHPSTRING);
	}
	if(response) snmp_free_pdu(response);
	} /* keepwalking */
	snmp_close(ss);
	if(st==2) Push("__snmptmp__",VAR);
}

void phpsnmpget(void) {
	phpsnmp(1);
}

void phpsnmpwalk(void) {
	phpsnmp(2);
}

#endif
