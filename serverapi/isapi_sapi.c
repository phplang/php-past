#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <httpext.h>
#include <stdarg.h>
#include "parser.h"
#define SAPI_INTERFACE 1
#include "sapi.h"

#if DEBUG
void logmessage(void *scid,char *message, char *funcname){
	char logmessage[81];
	LPEXTENSION_CONTROL_BLOCK lpEcb=scid;
	snprintf(logmessage,79,"%d:%s:%s\n",lpEcb->ConnID,funcname,message);
#if 0
	_php3_error_log(4,logmessage,"/phplog.txt",NULL);
#else
	/* this will output right to the msvc debugger window,
	   or to DBMON which comes with msvc */
	OutputDebugString(logmessage);
#endif
}
#endif

int sapi_readclient(void * scid, char *buf, int size, int len){
	LPEXTENSION_CONTROL_BLOCK lpEcb=scid;
	if(!lpEcb->ReadClient(lpEcb->ConnID,(LPVOID)buf,&len)){
		OutputDebugString("ReadClient Failed\n");
		return 0;
	}
	return 1;
}

char *sapi_getenv(void *scid, char *string){
	char var[255],dummy;
	DWORD dwLen, dummylen = 1;
	LPEXTENSION_CONTROL_BLOCK lpEcb=scid;

	if (!string)
		return "";
	
	dwLen =lpEcb->GetServerVariable(lpEcb->ConnID,string,&dummy,&dummylen);

	if (dwLen == 0)
		return "";
	
	(void)lpEcb->GetServerVariable(lpEcb->ConnID,string,var,&dwLen);

	return estrdup(var);
}

void sapi_header(void *scid, char *header){
	LPEXTENSION_CONTROL_BLOCK lpEcb=scid;
	int len=strlen(header);
#if DEBUG
	logmessage(scid,header,"header");
#endif
	if(!lpEcb->ServerSupportFunction(lpEcb->ConnID,
		HSE_REQ_SEND_URL_REDIRECT_RESP,
		NULL,&len,(LPDWORD)header))
			OutputDebugString("Header Failed\n");

}

/*FIXME*/
void sapi_flush(void *scid){ 
	LPEXTENSION_CONTROL_BLOCK lpEcb=scid;
	/*fflush(stdout);*/
}

void sapi_puts(void *scid, char *string){
	LPEXTENSION_CONTROL_BLOCK lpEcb=scid;
	int n;
#if DEBUG
	logmessage(scid,string,"puts");
#endif
	n = strlen(string);
	if(!lpEcb->WriteClient(lpEcb->ConnID,string,&n,0))
		OutputDebugString("PUTS Failed\n");
	return;
}

void sapi_putc(void *scid, char c){
	LPEXTENSION_CONTROL_BLOCK lpEcb=scid;
	int n = 1;
#if DEBUG
	logmessage(scid,&c,"putc");
#endif
	if(!lpEcb->WriteClient(lpEcb->ConnID,&c,&n,0)){
		OutputDebugString("PUTC Failed\n");
	}
	return;
}

int sapi_writeclient(void *scid, char *string, int len){
	LPEXTENSION_CONTROL_BLOCK lpEcb=scid;
#if DEBUG
	logmessage(scid,string,"writeclient");
#endif
	if(!lpEcb->WriteClient(lpEcb->ConnID,string,&len,0)){
		OutputDebugString("WriteClient Failed\n");
		return 0;
	}
	return 1;
}

void sapi_log(void *scid, char *message){}
void sapi_block(void *scid){}
void sapi_unblock(void *scid){}

void sapi_init(LPEXTENSION_CONTROL_BLOCK lpEcb, struct sapi_request_info *sapi_info)
{
	char *buf;
	sapi_info->scid=lpEcb;
	sapi_info->path_info = lpEcb->lpszPathInfo;
	sapi_info->path_translated = lpEcb->lpszPathTranslated;
	sapi_info->filename = strdup(sapi_info->path_translated);
	sapi_info->query_string = lpEcb->lpszQueryString;
	sapi_info->current_user = NULL;
	sapi_info->current_user_length=0;
	sapi_info->request_method = lpEcb->lpszMethod;
	sapi_info->script_name = sapi_getenv(lpEcb,"SCRIPT_NAME");

	buf = sapi_getenv(lpEcb,"CONTENT_LENGTH");
	sapi_info->content_length = (buf ? atoi(buf) : 0);

	sapi_info->content_type = lpEcb->lpszContentType;
	sapi_info->cookies = sapi_getenv(lpEcb,"HTTP_COOKIE");
	sapi_info->puts=sapi_puts;
	sapi_info->putc=sapi_putc;
	sapi_info->getenv=sapi_getenv;
	sapi_info->writeclient=sapi_writeclient;
	sapi_info->flush=sapi_flush;
	sapi_info->header=sapi_header;
	sapi_info->readclient=sapi_readclient;
	sapi_info->block=sapi_block;
	sapi_info->unblock=sapi_unblock;
	sapi_info->log=sapi_log;

	/*these are used in cgi, what do we do with them now*/
	sapi_info->cgi=0;
	sapi_info->display_source_mode=0;
	sapi_info->preprocess=0;
	sapi_info->info_only=0;
	sapi_info->quiet_mode=0;
}


BOOL WINAPI GetExtensionVersion (HSE_VERSION_INFO  *version)
{
    version->dwExtensionVersion = MAKELONG( HSE_VERSION_MINOR,
                                            HSE_VERSION_MAJOR );
    strncpy( version->lpszExtensionDesc,
            "PHP v3.0 Beta 1 ISAPI Ext v1",
			HSE_MAX_EXT_DLL_NAME_LEN);

    return TRUE;
}

/* this is php3_isapi_main()!!! */
DWORD WINAPI HttpExtensionProc (LPEXTENSION_CONTROL_BLOCK lpEcb)
{
	int ret=0;
	struct sapi_request_info sapi_info;
#if DEBUG
	char logmessage[80];
	snprintf(logmessage,79,"%d:isapi_main:connID\n",lpEcb->ConnID);
	OutputDebugString(logmessage);
#endif

	sapi_init(lpEcb,&sapi_info);
	ret=php3_sapi_main(&sapi_info);

	/*make sure we free any loose allocations*/
	free(sapi_info.filename);
	return ret;
}
