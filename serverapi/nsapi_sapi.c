/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997 PHP Development Team (See Credits file)           |
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
   | Authors: Shane Caraveo                                               |
   |                                                                      |
   +----------------------------------------------------------------------+
 */


/* FIXME BIG TIME.  This is by no means complete! */

#include "php.h"
#include "language-parser.tab.h"
#include "main.h"
#include "control_structures.h"
#include "php3_list.h"
#include "modules.h"
#include "functions/file.h"
#include "functions/head.h"
#include "functions/post.h"
#include "functions/head.h"
#include "functions/type.h"
#include "highlight.h"

/*netscape headers*/
#include "base/pblock.h"
#include "base/session.h"
#include "frame/req.h"
#include "base/util.h"       /* is_mozilla, getline */
#include "frame/protocol.h"  /* protocol_start_response */
#include "frame/http.h"
#include "base/file.h"       /* system_fopenRO */
#include "base/buffer.h"     /* filebuf */
#include "frame/log.h"       /* log_error */
#include "base/crit.h"
#include "base/daemon.h"


#ifndef XP_WIN32
#include <unistd.h>  /* sleep */
#define DLLEXPORT
#else /* XP_WIN32 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#define DLLEXPORT __declspec(dllexport)
#endif /* XP_WIN32 */

struct ns_request_info{
	pblock *pb;
	Session *sn;
	Request *rq;
};

int sapi_readclient(void *scid, char *buf, int size, int len){
	struct ns_request_info *ns_rqst=(struct ns_request_info *)scid;
	return net_read (ns_rqst->sn->csd, buf, size, 10);
}

char *sapi_getenv(void *scid, char *string){
	struct ns_request_info *ns_rqst=(struct ns_request_info *)scid;
}

/* FIXME */
void sapi_header(void *scid, char *header){
	struct ns_request_info *ns_rqst=(struct ns_request_info *)scid;
			pblock_remove("content-type",nsrq->srvhdrs);
			pblock_nvinsert("content-type","text/html",nsrq->srvhdrs);
			protocol_status(nssn,nsrq,PROTOCOL_OK,NULL);
			if (protocol_start_response(nssn,nsrq)==REQ_NOACTION)
				return REQ_PROCEED;
}

void sapi_flush(void *scid){
	struct ns_request_info *ns_rqst=(struct ns_request_info *)scid;
}

void sapi_puts(void *scid, char *string){
	struct ns_request_info *ns_rqst=(struct ns_request_info *)scid;
	puts(string);
}

void sapi_putc(void *scid, char c){
	struct ns_request_info *ns_rqst=(struct ns_request_info *)scid;
	net_write(ns_rqst->sn->csd, &c, 1);
}

int sapi_writeclient(void *scid, char *string, int len){
	struct ns_request_info *ns_rqst=(struct ns_request_info *)scid;
	return net_write(ns_rqst->sn->csd, s, size);
}

void sapi_block(void *scid){}
void sapi_unblock(void *scid){}


void sapi_init(struct ns_request_info *ns_rqst,struct sapi_request_info *sapi_info)
{
	char *buf;

	/* for cgi we dont use request id's */
	sapi_info->scid=ns_rqst;

	sapi_info->path_info = pblock_findval("path-info",rq->vars);
	sapi_info->path_translated = pblock_findval("path",rq->vars);
	sapi_info->filename = estrdup(sapi_info->path_translated);
	sapi_info->query_string = pblock_findval("query", srq->vars); 
	sapi_info->request_method = pblock_findval("method",rq->reqpb);
	sapi_info->script_name = pblock_findval("path",rq->vars);
	sapi_info->content_length = atoi(pblock_findval("content-length",rq->headers));
	sapi_info->content_type =  pblock_findval("content-type",rq->srvhdrs);
	request_header("cookie", &(sapi_info->cookies), sn,rq);
	sapi_info->puts=sapi_puts;
	sapi_info->putc=sapi_putc;
	sapi_info->getenv=sapi_getenv;
	sapi_info->writeclient=sapi_writeclient;
	sapi_info->flush=sapi_flush;
	sapi_info->header=sapi_header;
	sapi_info->readclient=sapi_readclient;
	sapi_info->block=sapi_block;
	sapi_info->unblock=sapi_unblock;
}


DLLEXPORT void close_nsapi_mod(void *vparam)
{
	//global shutdown for all php threads
	//for now this will do nothing until we
	//make php thread-safe
}

DLLEXPORT int init_nsapi_mod(pblock *pb, Session *sn, Request *rq)
{
	//global initialization for all php threads
	//for now this will do nothing until we
	//make php thread-safe

	daemon_atrestart(close_nsapi_mod, NULL);
	return REQ_PROCEED;
}

DLLEXPORT int nsapi_php_main(pblock *pb, Session *sn, Request *rq)
{
	struct ns_request_info *ns_rqst=emalloc(sizeof(struct ns_request_info));
	struct sapi_request_info sapi_info;
	
	ns_rqst->pb=pb;
	ns_rqst->sn=sn;
	ns_rqst->rq=rq;

	sapi_init(ns_rqst,&sapi_info);

	if (php3_sapi_main(&sapi_info))
		efree(ns_rqst);
		return REQ_PROCEED; /*SUCCESS*/
	} else {
		efree(ns_rqst);
		return REQ_ABORTED; /*FAILURE*/
	}
	return REQ_ABORTED; /*FAILURE*/
}


