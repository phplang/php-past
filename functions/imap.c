#define IMAP41

#ifdef ERROR
#undef ERROR
#endif

#if !(WIN32|WINNT)
#include "config.h"
#endif
#include "php.h"
#include "internal_functions.h"

#if COMPILE_DL
#include "dl/phpdl.h"
#endif

#if HAVE_IMAP

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include "php3_list.h"
#include "imap.h"
#include "mail.h"
#include "rfc822.h"
#include "modules.h"
MAILSTREAM *mail_close_it (MAILSTREAM *stream);
#if (WIN32|WINNT)
#include "winsock.h"
#endif

#ifdef IMAP41
#define LSIZE text.size
#define LTEXT text.data
#define DTYPE int
#define CONTENT_PART nested.part
#define CONTENT_MSG_BODY nested.msg->body
#define IMAPVER "Imap 4R1"
#else
#define LSIZE size
#define LTEXT text
#define DTYPE char
#define CONTENT_PART contents.part
#define CONTENT_MSG_BODY contents.msg.body
#define IMAPVER "Imap 4"
#endif
/* 
 * this array should be set up as:
 * {"PHPScriptFunctionName",dllFunctionName,1} 
 */

/* type casts left out, put here to remove warnings in
   msvc
*/
extern void rfc822_date(char *date);
extern char *cpystr(const char *string);
extern unsigned long find_rightmost_bit (unsigned long *valptr);
extern void fs_give (void **block);
int add_assoc_object(pval *arg, char *key, pval tmp);
int add_next_index_object(pval *arg, pval tmp);
void imap_add_body( pval *arg, BODY *body );

function_entry imap_functions[] =
{
	{"imap_open", php3_imap_open, NULL},
	{"imap_reopen", php3_imap_reopen, NULL},
	{"imap_num_msg", php3_imap_num_msg, NULL},
	{"imap_num_recent", php3_imap_num_recent, NULL},
	{"imap_headers", php3_imap_headers, NULL},
	{"imap_header", php3_imap_headerinfo, NULL},
	{"imap_headerinfo", php3_imap_headerinfo, NULL},
	{"imap_body", php3_imap_body, NULL},
	{"imap_fetchstructure", php3_imap_fetchstructure, NULL},
	{"imap_fetchbody", php3_imap_fetchbody, NULL},
	{"imap_expunge", php3_imap_expunge, NULL},
	{"imap_delete", php3_imap_delete, NULL},
	{"imap_undelete", php3_imap_undelete, NULL},
	{"imap_check", php3_imap_check, NULL},
	{"imap_close", php3_imap_close, NULL},
	{"imap_mail_copy", php3_imap_mail_copy, NULL},
	{"imap_mail_move", php3_imap_mail_move, NULL},
	{"imap_createmailbox", php3_imap_createmailbox, NULL},
	{"imap_renamemailbox", php3_imap_renamemailbox, NULL},
	{"imap_deletemailbox", php3_imap_deletemailbox, NULL},
	{"imap_listmailbox", php3_imap_list, NULL},
	{"imap_scanmailbox", php3_imap_listscan, NULL},
	{"imap_listsubscribed", php3_imap_lsub, NULL},
	{"imap_subscribe", php3_imap_subscribe, NULL},
	{"imap_unsubscribe", php3_imap_unsubscribe, NULL},
	{"imap_append", php3_imap_append, NULL},
	{"imap_ping", php3_imap_ping, NULL},
	{"imap_base64", php3_imap_base64, NULL},
	{"imap_qprint", php3_imap_qprint, NULL},
	{"imap_8bit", php3_imap_8bit, NULL},
	{"imap_binary", php3_imap_binary, NULL},
	{"imap_mailboxmsginfo", php3_imap_mailboxmsginfo, NULL},
	{"imap_rfc822_write_address", php3_imap_rfc822_write_address, NULL},
	{"imap_rfc822_parse_adrlist", php3_imap_rfc822_parse_adrlist, NULL},
	{"imap_setflag_full", php3_imap_setflag_full, NULL},
	{"imap_clearflag_full", php3_imap_clearflag_full, NULL},
	{"imap_sort", php3_imap_sort, NULL},
	{"imap_fetchheader", php3_imap_fetchheader, NULL},
	{"imap_fetchtext", php3_imap_body, NULL},
	{"imap_uid", php3_imap_uid, NULL},
	{"imap_msgno",php3_imap_msgno, NULL},
	{NULL, NULL, NULL}
};


php3_module_entry php3_imap_module_entry = {
	IMAPVER, imap_functions, imap_init, NULL, NULL, NULL,imap_info, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &php3_imap_module_entry; }
#endif

/* 
   I beleive since this global is used ONLY within this module,
   and nothing will link to this module, we can use the simple 
   thread local storage
*/
int le_imap;
char imap_user[80]="";
char imap_password[80]="";
STRINGLIST *imap_folders=NIL;
STRINGLIST *imap_sfolders=NIL;

MAILSTREAM *mail_close_it (MAILSTREAM *stream)
{
	return mail_close_full ( stream,NIL);
}

inline int add_assoc_object(pval *arg, char *key, pval tmp)
{
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), NULL);
}

inline int add_next_index_object(pval *arg, pval tmp)
{
	return _php3_hash_next_index_insert( arg->value.ht, (void *) &tmp, sizeof(pval), NULL); 
}


void imap_info(void)
{
	php3_printf("Imap Support enabled<br>");
	php3_printf("<table>");
	php3_printf("<tr><td>Imap c-client Version:</td>");
#ifdef IMAP41
	php3_printf("<td>Imap 4.1</td>");
#else
	php3_printf("<td>Imap 4.0</td>");
#endif
	php3_printf("</tr></table>");
}

int imap_init(INIT_FUNC_ARGS)
{
#if WIN32|WINNT
	mail_link (&mbxdriver);      /* link in the mbox driver */
#else
	mail_link (&mboxdriver);      /* link in the mbox driver */
#endif
	mail_link (&imapdriver);      /* link in the imap driver */
	mail_link (&nntpdriver);      /* link in the nntp driver */
	mail_link (&pop3driver);      /* link in the pop3 driver */
#if !(WIN32|WINNT)
	mail_link (&mhdriver);        /* link in the mh driver */
	mail_link (&mxdriver);        /* link in the mx driver */
#endif
	mail_link (&mbxdriver);       /* link in the mbx driver */
	mail_link (&tenexdriver);     /* link in the tenex driver */
	mail_link (&mtxdriver);       /* link in the mtx driver */
#if !(WIN32|WINNT)
	mail_link (&mmdfdriver);      /* link in the mmdf driver */
	mail_link (&newsdriver);      /* link in the news driver */
	mail_link (&philedriver);     /* link in the phile driver */
#endif
	mail_link (&dummydriver);     /* link in the dummy driver */
	auth_link (&auth_log);        /* link in the log authenticator */
	/* Close options */

        REGISTER_MAIN_LONG_CONSTANT("CL_EXPUNGE", CL_EXPUNGE, CONST_PERSISTENT | CONST_CS);
        /* expunge silently */


	/* Fetch options */

        REGISTER_MAIN_LONG_CONSTANT("FT_UID", FT_UID, CONST_PERSISTENT | CONST_CS);
         /* argument is a UID */
        REGISTER_MAIN_LONG_CONSTANT("FT_PEEK", FT_PEEK, CONST_PERSISTENT | CONST_CS);
       /* peek at data */
        REGISTER_MAIN_LONG_CONSTANT("FT_NOT", FT_NOT, CONST_PERSISTENT | CONST_CS);
        /* NOT flag for header lines fetch */
        REGISTER_MAIN_LONG_CONSTANT("FT_INTERNAL", FT_INTERNAL, CONST_PERSISTENT | CONST_CS);
    /* text can be internal strings */
        REGISTER_MAIN_LONG_CONSTANT("FT_PREFETCHTEXT", FT_PREFETCHTEXT, CONST_PERSISTENT | CONST_CS);
    /* IMAP prefetch text when fetching header */


	/* Flagging options */

        REGISTER_MAIN_LONG_CONSTANT("ST_UID", ST_UID, CONST_PERSISTENT | CONST_CS);
         /* argument is a UID sequence */
        REGISTER_MAIN_LONG_CONSTANT("ST_SILENT", ST_SILENT, CONST_PERSISTENT | CONST_CS);
     /* don't return results */
        REGISTER_MAIN_LONG_CONSTANT("ST_SET", ST_SET, CONST_PERSISTENT | CONST_CS);
     /* set vs. clear */

   /* Copy options */

        REGISTER_MAIN_LONG_CONSTANT("CP_UID", CP_UID, CONST_PERSISTENT | CONST_CS);
         /* argument is a UID sequence */
        REGISTER_MAIN_LONG_CONSTANT("CP_MOVE", CP_MOVE, CONST_PERSISTENT | CONST_CS);
        /* delete from source after copying */


   /* Search/sort options */

        REGISTER_MAIN_LONG_CONSTANT("SE_UID", SE_UID, CONST_PERSISTENT | CONST_CS);
         /* return UID */
        REGISTER_MAIN_LONG_CONSTANT("SE_FREE", SE_FREE, CONST_PERSISTENT | CONST_CS);
        /* free search program after finished */
        REGISTER_MAIN_LONG_CONSTANT("SE_NOPREFETCH", SE_NOPREFETCH, CONST_PERSISTENT | CONST_CS);
  /* no search prefetching */
        REGISTER_MAIN_LONG_CONSTANT("SO_FREE", SO_FREE, CONST_PERSISTENT | CONST_CS);
        /* free sort program after finished */
        REGISTER_MAIN_LONG_CONSTANT("SO_NOSERVER", SO_NOSERVER, CONST_PERSISTENT | CONST_CS);
   /* don't do server-based sort */

   /* Status options */

        REGISTER_MAIN_LONG_CONSTANT("SA_MESSAGES",SA_MESSAGES , CONST_PERSISTENT | CONST_CS);
    /* number of messages */
        REGISTER_MAIN_LONG_CONSTANT("SA_RECENT", SA_RECENT, CONST_PERSISTENT | CONST_CS);
      /* number of recent messages */
        REGISTER_MAIN_LONG_CONSTANT("SA_UNSEEN",SA_UNSEEN , CONST_PERSISTENT | CONST_CS);
      /* number of unseen messages */
        REGISTER_MAIN_LONG_CONSTANT("SA_UIDNEXT", SA_UIDNEXT, CONST_PERSISTENT | CONST_CS);
     /* next UID to be assigned */
        REGISTER_MAIN_LONG_CONSTANT("SA_UIDVALIDITY",SA_UIDVALIDITY , CONST_PERSISTENT | CONST_CS);
     /* UID validity value */


   /* Bits for mm_list() and mm_lsub() */

        REGISTER_MAIN_LONG_CONSTANT("LATT_NOINFERIORS",LATT_NOINFERIORS , CONST_PERSISTENT | CONST_CS);
        REGISTER_MAIN_LONG_CONSTANT("LATT_NOSELECT", LATT_NOSELECT, CONST_PERSISTENT | CONST_CS);
        REGISTER_MAIN_LONG_CONSTANT("LATT_MARKED", LATT_MARKED, CONST_PERSISTENT | CONST_CS);
        REGISTER_MAIN_LONG_CONSTANT("LATT_UNMARKED",LATT_UNMARKED , CONST_PERSISTENT | CONST_CS);


   /* Sort functions */

        REGISTER_MAIN_LONG_CONSTANT("SORTDATE",SORTDATE , CONST_PERSISTENT | CONST_CS);
             /* date */
        REGISTER_MAIN_LONG_CONSTANT("SORTARRIVAL",SORTARRIVAL , CONST_PERSISTENT | CONST_CS);
          /* arrival date */
        REGISTER_MAIN_LONG_CONSTANT("SORTFROM",SORTFROM , CONST_PERSISTENT | CONST_CS);
            /* from */
        REGISTER_MAIN_LONG_CONSTANT("SORTSUBJECT",SORTSUBJECT , CONST_PERSISTENT | CONST_CS);
           /* subject */
        REGISTER_MAIN_LONG_CONSTANT("SORTTO",SORTTO , CONST_PERSISTENT | CONST_CS);
                /* to */
        REGISTER_MAIN_LONG_CONSTANT("SORTCC",SORTCC , CONST_PERSISTENT | CONST_CS);
              /* cc */
        REGISTER_MAIN_LONG_CONSTANT("SORTSIZE",SORTSIZE , CONST_PERSISTENT | CONST_CS);
              /* size */


    le_imap = register_list_destructors(mail_close_it,NULL);
	return SUCCESS;
}

void php3_imap_open(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mailbox;
	pval *user;
	pval *passwd;
	pval *options;
	MAILSTREAM *imap_stream;
	int ind;
        int myargc=ARG_COUNT(ht);
	
	if (myargc <3 || myargc >4 || getParameters(ht, myargc, &mailbox,&user,&passwd,&options) == FAILURE) {
		WRONG_PARAM_COUNT;
	} else {
		convert_to_string(mailbox);
		convert_to_string(user);
		convert_to_string(passwd);
		if(myargc ==4) convert_to_long(options);
		strcpy(imap_user,user->value.str.val);
		strcpy(imap_password,passwd->value.str.val);
		imap_stream = NIL;
		imap_stream = mail_open(imap_stream,mailbox->value.str.val,myargc == 4 ? options->value.lval : NIL);

		if (imap_stream == NIL){
			php3_error(E_WARNING,"Couldn't open stream %s\n",mailbox->value.str.val);
			RETURN_FALSE;
		}
		ind = php3_list_insert(imap_stream, le_imap);

		RETURN_LONG(ind);
	}
	return;
}


void php3_imap_reopen(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	pval *mailbox;
	pval *user;
	pval *passwd;
	pval *options;
	MAILSTREAM *imap_stream;
	int ind, ind_type;
	int myargc=ARG_COUNT(ht);
	if (getParameters(ht,myargc,&streamind, &mailbox,&user,&passwd,&options) == FAILURE) {
        WRONG_PARAM_COUNT;
    }

	convert_to_long(streamind);
	ind = streamind->value.lval;
	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	convert_to_string(mailbox);
	convert_to_string(user);
	convert_to_string(passwd);
	if(myargc == 4) convert_to_long(options);
	strcpy(imap_user,user->value.str.val);
	strcpy(imap_password,passwd->value.str.val);

	imap_stream = mail_open(imap_stream,mailbox->value.str.val,myargc == 4 ? options->value.lval : NIL );
	if (imap_stream == NIL){
		php3_error(E_WARNING,"Couldn't re-open stream\n");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

void php3_imap_append(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*folder, *message,*flags;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	STRING st;
	int myargc=ARG_COUNT(ht);

	if ( myargc < 3 || myargc > 4 || getParameters( ht, myargc, &streamind, &folder, &message,&flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(folder);
	convert_to_string(message);
	if (myargc == 4) convert_to_long(flags);
	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find( ind, &ind_type );

	if ( !imap_stream || ind_type != le_imap ) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	INIT (&st,mail_string,(void *) message->value.str.val,message->value.str.len);
	mail_append_full( imap_stream, folder->value.str.val,flags->value.str.val,NIL,&st);
	RETURN_TRUE;
}

void php3_imap_num_msg(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);

	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	RETURN_LONG(imap_stream->nmsgs);
}

void php3_imap_ping(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long( streamind );
	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find( ind, &ind_type );
	if ( !imap_stream || ind_type != le_imap ) {
		php3_error( E_WARNING, "Unable to find stream pointer" );
		RETURN_FALSE;
	}
	RETURN_LONG( mail_ping( imap_stream ) );
}


void php3_imap_num_recent(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	ind = streamind->value.lval;
	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETURN_LONG(imap_stream->recent);
}

void php3_imap_expunge(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);

	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_expunge (imap_stream);

	RETURN_TRUE;
}


void php3_imap_close(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *ind;
	int myargcount=ARG_COUNT(ht);

	if (myargcount != 1 || getParameters(ht, myargcount, &ind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(ind);
	php3_list_delete(ind->value.lval);
	RETURN_TRUE;
}


void php3_imap_headers(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	unsigned long i;
	char *t;
	unsigned int msgno;
	MAILSTREAM *imap_stream;
	char tmp[MAILTMPLEN];

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);

	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	/* Initialize return array */
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	for (msgno = 1; msgno <= imap_stream->nmsgs; msgno++) {
		MESSAGECACHE * cache = mail_elt (imap_stream,msgno);
		mail_fetchstructure (imap_stream,msgno,NIL);
		tmp[0] = cache->recent ? (cache->seen ? 'R': 'N') : ' ';
		tmp[1] = (cache->recent | cache->seen) ? ' ' : 'U';
		tmp[2] = cache->flagged ? 'F' : ' ';
		tmp[3] = cache->answered ? 'A' : ' ';
		tmp[4] = cache->deleted ? 'D' : ' ';
		sprintf (tmp+5,"%4ld) ",cache->msgno);
		mail_date (tmp+11,cache);
		tmp[17] = ' ';
		tmp[18] = '\0';
		mail_fetchfrom (tmp+18,imap_stream,msgno,(long) 20);
		strcat (tmp," ");
		if ((i = cache->user_flags)) {
			strcat (tmp,"{");
			while (i) {
				strcat (tmp,imap_stream->user_flags[find_rightmost_bit (&i)]);
				if (i) strcat (tmp," ");
			}
			strcat (tmp,"} ");
		}
		mail_fetchsubject(t=tmp+strlen(tmp),imap_stream,msgno,(long)25);
		sprintf (t+=strlen(t)," (%ld chars)",cache->rfc822_size);
		add_next_index_string(return_value,tmp,1);
	}

}

void php3_imap_body(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, * msgno, *flags;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	int myargc=ARG_COUNT(ht);
	if (myargc <2 || myargc > 3 || getParameters(ht,myargc,&streamind,&msgno,&flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(msgno);
	if(myargc == 3) convert_to_long(flags);
	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETVAL_STRING(mail_fetchtext_full (imap_stream,msgno->value.lval,NIL,myargc == 3 ? flags->value.lval : NIL),1);

}

void php3_imap_fetchtext_full(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, * msgno, *flags;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	int myargcount = ARG_COUNT(ht);
	if (myargcount >3 || myargcount <2 || getParameters(ht,myargcount,&streamind,&msgno,&flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(msgno);
	if (myargcount == 3) convert_to_long(flags);
	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETVAL_STRING(mail_fetchtext_full (imap_stream,msgno->value.lval,NIL,flags->value.lval),1);

}

void php3_imap_mail_copy(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*seq, *folder, *options;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	int myargcount = ARG_COUNT(ht);
	if (myargcount > 4 || myargcount < 3 
		|| getParameters(ht,myargcount,&streamind,&seq,&folder,&options) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(seq);
	convert_to_string(folder);
	if(myargcount == 4) convert_to_long(options);
	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_copy_full(imap_stream,seq->value.str.val,folder->value.str.val,myargcount == 4 ? options->value.lval : NIL)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

void php3_imap_mail_move(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind,*seq, *folder;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht)!=3 
		|| getParameters(ht,3,&streamind,&seq,&folder) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(seq);
	convert_to_string(folder);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_move(imap_stream,seq->value.str.val,folder->value.str.val)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

void php3_imap_createmailbox(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *folder;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&folder) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(folder);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_create(imap_stream,folder->value.str.val)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

void php3_imap_renamemailbox(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *old, *new;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht)!=3 || getParameters(ht,3,&streamind,&old,&new)==FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(old);
	convert_to_string(new);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_rename(imap_stream,old->value.str.val,new->value.str.val)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

void php3_imap_deletemailbox(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *folder;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&folder) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(folder);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	if ( mail_delete(imap_stream,folder->value.str.val)==T ) {
        RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

void php3_imap_list(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *ref, *pat;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	STRINGLIST *cur=NIL;

	if (ARG_COUNT(ht)!=3 
		|| getParameters(ht,3,&streamind,&ref,&pat) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(ref);
	convert_to_string(pat);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
    imap_folders = NIL;
	mail_list(imap_stream,ref->value.str.val,pat->value.str.val);
	if (imap_folders == NIL) {
		RETURN_FALSE;
	}
	array_init(return_value);
    cur=imap_folders;
    while (cur != NIL ) {
		add_next_index_string(return_value,cur->LTEXT,1);
        cur=cur->next;
    }
	mail_free_stringlist (&imap_folders);
}
void php3_imap_listscan(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *streamind, *ref, *pat, *content;
        int ind, ind_type;
        MAILSTREAM *imap_stream;
        STRINGLIST *cur=NIL;

        if (ARG_COUNT(ht)!=3 
                || getParameters(ht,4,&streamind,&ref,&pat,&content) == FAILURE) {
                WRONG_PARAM_COUNT;
        }

        convert_to_long(streamind);
        convert_to_string(ref);
        convert_to_string(pat);
        convert_to_string(content);

        ind = streamind->value.lval;

        imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
        if (!imap_stream || ind_type != le_imap) {
                php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
        }
    imap_folders = NIL;
        mail_scan(imap_stream,ref->value.str.val,pat->value.str.val,content->value.str.val);
        if (imap_folders == NIL) {
                RETURN_FALSE;
        }
        array_init(return_value);
    cur=imap_folders;
    while (cur != NIL ) {
                add_next_index_string(return_value,cur->LTEXT,1);
        cur=cur->next;
    }
        mail_free_stringlist (&imap_folders);
}

void php3_imap_check(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	char date[50];

	if (ARG_COUNT(ht)!=1 || getParameters(ht,1,&streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	if (mail_ping (imap_stream) == NIL ) {
		RETURN_FALSE;
    }
	if (imap_stream && imap_stream->mailbox) {
		rfc822_date (date);
		object_init(return_value);
		add_property_string(return_value,"Date",date,1);
		add_property_string(return_value,"Driver",imap_stream->dtb->name,1);
		add_property_string(return_value,"Mailbox",imap_stream->mailbox,1);
		add_property_long(return_value,"Nmsgs",imap_stream->nmsgs);
		add_property_long(return_value,"Recent",imap_stream->recent);
	} else {
		RETURN_FALSE;
	}

}

void php3_imap_delete(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, * msgno;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(msgno);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_setflag(imap_stream,msgno->value.str.val,"\\DELETED");
	RETVAL_TRUE;

}

void php3_imap_undelete(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, * msgno;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(msgno);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_clearflag (imap_stream,msgno->value.str.val,"\\DELETED");
	RETVAL_TRUE;

}


void php3_imap_headerinfo(INTERNAL_FUNCTION_PARAMETERS)
{
  pval *streamind, * msgno,to,tovals,from,fromvals,reply_to,reply_tovals,sender;
  pval *fromlength;
  pval *subjectlength;
  pval sendervals,return_path,return_pathvals;
  pval cc,ccvals,bcc,bccvals;
  pval *defaulthost;
  int ind, ind_type;
  unsigned long length;
  MAILSTREAM *imap_stream;
	MESSAGECACHE * cache;
	char *mystring;
	char dummy[2000];
	char fulladdress[MAILTMPLEN];
	ENVELOPE *en;
/*	BODY *body; */
	ADDRESS *addresstmp;
	int myargc;
	STRINGLIST *lines=mail_newstringlist ();
	STRINGLIST *cur=lines;
	myargc=ARG_COUNT(ht);
	if (myargc<2 || myargc>5 || getParameters(ht,myargc,&streamind,&msgno,&fromlength,&subjectlength,&defaulthost)==FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(streamind);
	convert_to_long(msgno);
       if(myargc >= 3) convert_to_long(fromlength); else fromlength=0x00;;
       if(myargc >= 4) convert_to_long(subjectlength); else subjectlength=0x00;
       if(myargc == 5) convert_to_string(defaulthost);
	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_fetchstructure (imap_stream,msgno->value.lval,NIL);
	cache = mail_elt (imap_stream,msgno->value.lval);
	
	object_init(return_value);

 	cur->LSIZE = strlen (cur->LTEXT = (char *)cpystr ("From"));
        cur = cur->next = mail_newstringlist ();
 	cur->LSIZE = strlen (cur->LTEXT = (char *)cpystr ("Date"));
        cur = cur->next = mail_newstringlist ();
 	cur->LSIZE = strlen (cur->LTEXT = (char *)cpystr ("Subject"));
        cur = cur->next = mail_newstringlist ();
 	cur->LSIZE = strlen (cur->LTEXT = (char *)cpystr ("To"));
        cur = cur->next = mail_newstringlist ();
 	cur->LSIZE = strlen (cur->LTEXT = (char *)cpystr ("cc"));
        cur = cur->next = mail_newstringlist ();
 	cur->LSIZE = strlen (cur->LTEXT = (char *)cpystr ("Reply-To"));
    mystring=mail_fetchheader_full(imap_stream,msgno->value.lval,lines,&length,NIL);
mail_free_stringlist (&lines);
if(myargc ==5)
rfc822_parse_msg (&en,NULL,mystring,length,NULL,defaulthost->value.str.val,NIL);
else
rfc822_parse_msg (&en,NULL,mystring,length,NULL,"UNKNOWN",NIL);

    if(en->remail) add_property_string(return_value,"remail",en->remail,1);
    if(en->date) add_property_string(return_value,"date",en->date,1);
    if(en->date) add_property_string(return_value,"Date",en->date,1);
    if(en->subject)add_property_string(return_value,"subject",en->subject,1);
    if(en->subject)add_property_string(return_value,"Subject",en->subject,1);
    if(en->in_reply_to)add_property_string(return_value,"in_reply_to",en->in_reply_to,1);
    if(en->message_id)add_property_string(return_value,"message_id",en->message_id,1);
    if(en->newsgroups)add_property_string(return_value,"newsgroups",en->newsgroups,1);
    if(en->followup_to)add_property_string(return_value,"followup_to",en->followup_to,1);
    if(en->references)add_property_string(return_value,"references",en->references,1);

if(en->to)
{
    addresstmp=en->to;
    fulladdress[0]=0x00;
    rfc822_write_address(fulladdress,addresstmp);
      if(fulladdress) add_property_string( return_value, "toaddress", fulladdress, 1);
    array_init( &to );
    do {
      object_init( &tovals);
      if(addresstmp->personal) add_property_string( &tovals, "personal", addresstmp->personal, 1);
      if(addresstmp->adl) add_property_string( &tovals, "adl", addresstmp->adl, 1);
      if(addresstmp->mailbox) add_property_string( &tovals, "mailbox", addresstmp->mailbox, 1);
      if(addresstmp->host) add_property_string( &tovals, "host", addresstmp->host, 1);
      add_next_index_object( &to, tovals );
    } while ( (addresstmp = addresstmp->next) );
    add_assoc_object( return_value, "to", to );
}

if(en->from)
{
    addresstmp=en->from;
    fulladdress[0]=0x00;
    rfc822_write_address(fulladdress,addresstmp);
      if(fulladdress) add_property_string( return_value, "fromaddress", fulladdress, 1);
    array_init( &from );
    do {
      object_init( &fromvals);
      if(addresstmp->personal) add_property_string( &fromvals, "personal", addresstmp->personal, 1);
      if(addresstmp->adl) add_property_string( &fromvals, "adl", addresstmp->adl, 1);
      if(addresstmp->mailbox) add_property_string( &fromvals, "mailbox", addresstmp->mailbox, 1);
      if(addresstmp->host) add_property_string( &fromvals, "host", addresstmp->host, 1);
      add_next_index_object( &from, fromvals );
    } while ( (addresstmp = addresstmp->next) );
    add_assoc_object( return_value, "from", from );
}

if(en->cc)
{
    addresstmp=en->cc;
      fulladdress[0]=0x00;
      rfc822_write_address(fulladdress,addresstmp);
      if(fulladdress) add_property_string( return_value, "ccaddress", fulladdress, 1);
    array_init( &cc );
    do {
      object_init( &ccvals);
      if(addresstmp->personal) add_property_string( &ccvals, "personal", addresstmp->personal, 1);
      if(addresstmp->adl) add_property_string( &ccvals, "adl", addresstmp->adl, 1);
      if(addresstmp->mailbox) add_property_string( &ccvals, "mailbox", addresstmp->mailbox, 1);
      if(addresstmp->host) add_property_string( &ccvals, "host", addresstmp->host, 1);
      add_next_index_object( &cc, ccvals );
    } while ( (addresstmp = addresstmp->next) );
    add_assoc_object( return_value, "cc", cc );
}

if(en->bcc)
{
    addresstmp=en->bcc;
      fulladdress[0]=0x00;
      rfc822_write_address(fulladdress,addresstmp);
      if(fulladdress) add_property_string( return_value, "bccaddress", fulladdress, 1);
    array_init( &bcc );
    do {
      object_init( &bccvals);
      if(addresstmp->personal) add_property_string( &bccvals, "personal", addresstmp->personal, 1);
      if(addresstmp->adl) add_property_string( &bccvals, "adl", addresstmp->adl, 1);
      if(addresstmp->mailbox) add_property_string( &bccvals, "mailbox", addresstmp->mailbox, 1);
      if(addresstmp->host) add_property_string( &bccvals, "host", addresstmp->host, 1);
      add_next_index_object( &bcc, bccvals );
    } while ( (addresstmp = addresstmp->next) );
    add_assoc_object( return_value, "bcc", bcc );
}

if(en->reply_to)
{
    addresstmp=en->reply_to;
      fulladdress[0]=0x00;
      rfc822_write_address(fulladdress,addresstmp);
      if(fulladdress) add_property_string( return_value, "reply_toaddress", fulladdress, 1);
    array_init( &reply_to );
    do {
      object_init( &reply_tovals);
      if(addresstmp->personal) add_property_string( &reply_tovals, "personal", addresstmp->personal, 1);
      if(addresstmp->adl) add_property_string( &reply_tovals, "adl", addresstmp->adl, 1);
      if(addresstmp->mailbox) add_property_string( &reply_tovals, "mailbox", addresstmp->mailbox, 1);
      if(addresstmp->host) add_property_string( &reply_tovals, "host", addresstmp->host, 1);
      add_next_index_object( &reply_to, reply_tovals );
    } while ( (addresstmp = addresstmp->next) );
    add_assoc_object( return_value, "reply_to", reply_to );
}

if(en->sender)
{
    addresstmp=en->sender;
      fulladdress[0]=0x00;
      rfc822_write_address(fulladdress,addresstmp);
      if(fulladdress) add_property_string( return_value, "senderaddress", fulladdress, 1);
    array_init( &sender );
    do {
      object_init( &sendervals);
      if(addresstmp->personal) add_property_string( &sendervals, "personal", addresstmp->personal, 1);
      if(addresstmp->adl) add_property_string( &sendervals, "adl", addresstmp->adl, 1);
      if(addresstmp->mailbox) add_property_string( &sendervals, "mailbox", addresstmp->mailbox, 1);
      if(addresstmp->host) add_property_string( &sendervals, "host", addresstmp->host, 1);
      add_next_index_object( &sender, sendervals );
    } while ( (addresstmp = addresstmp->next) );
    add_assoc_object( return_value, "sender", sender );
}

if(en->return_path)
{
    addresstmp=en->return_path;
      fulladdress[0]=0x00;
      rfc822_write_address(fulladdress,addresstmp);
      if(fulladdress) add_property_string( return_value, "return_pathaddress", fulladdress, 1);
    array_init( &return_path );
    do {
      object_init( &return_pathvals);
      if(addresstmp->personal) add_property_string( &return_pathvals, "personal", addresstmp->personal, 1);
      if(addresstmp->adl) add_property_string( &return_pathvals, "adl", addresstmp->adl, 1);
      if(addresstmp->mailbox) add_property_string( &return_pathvals, "mailbox", addresstmp->mailbox, 1);
      if(addresstmp->host) add_property_string( &return_pathvals, "host", addresstmp->host, 1);
      add_next_index_object( &return_path, return_pathvals );
    } while ( (addresstmp = addresstmp->next) );
    add_assoc_object( return_value, "return_path", return_path );
}
	add_property_string(return_value,"Recent",cache->recent ? (cache->seen ? "R": "N") : " ",1);
	add_property_string(return_value,"Unseen",(cache->recent | cache->seen) ? " " : "U",1);
	add_property_string(return_value,"Flagged",cache->flagged ? "F" : " ",1);
	add_property_string(return_value,"Answered",cache->answered ? "A" : " ",1);
	add_property_string(return_value,"Deleted",cache->deleted ? "D" : " ",1);
	sprintf (dummy,"%4ld",cache->msgno);
	add_property_string(return_value,"Msgno",dummy,1);
	mail_date (dummy,cache);
	add_property_string(return_value,"MailDate",dummy,1);
	sprintf (dummy,"%ld",cache->rfc822_size); 
    add_property_string(return_value,"Size",dummy,1);
    add_property_long(return_value,"udate",mail_longdate(cache)-(60*60*4));
 
if(en->from && fromlength)
{
fulladdress[0]=0x00;
mail_fetchfrom(fulladdress,imap_stream,msgno->value.lval,fromlength->value.lval);
add_property_string(return_value,"fetchfrom",fulladdress,1);
}
if(en->subject && subjectlength)
{
fulladdress[0]=0x00;
mail_fetchsubject(fulladdress,imap_stream,msgno->value.lval,subjectlength->value.lval);
add_property_string(return_value,"fetchsubject",fulladdress,1);
}
}

/* KMLANG */
void php3_imap_lsub(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *streamind, *ref, *pat;
        int ind, ind_type;
        MAILSTREAM *imap_stream;
        STRINGLIST *cur=NIL;

        if (ARG_COUNT(ht)!=3 
                || getParameters(ht,3,&streamind,&ref,&pat) == FAILURE) {
                WRONG_PARAM_COUNT;
        }

        convert_to_long(streamind);
        convert_to_string(ref);
        convert_to_string(pat);

        ind = streamind->value.lval;

        imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
        if (!imap_stream || ind_type != le_imap) {
                php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
        }
    imap_sfolders = NIL;
        mail_lsub(imap_stream,ref->value.str.val,pat->value.str.val);
        if (imap_sfolders == NIL) {
                RETURN_FALSE;
        }
        array_init(return_value);
    cur=imap_sfolders;
    while (cur != NIL ) {
                add_next_index_string(return_value,cur->LTEXT,1);
        cur=cur->next;
    }
        mail_free_stringlist (&imap_sfolders);
}

void php3_imap_subscribe(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *streamind, *folder;
        int ind, ind_type;
        MAILSTREAM *imap_stream;

        if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&folder) == FAILURE) {
                WRONG_PARAM_COUNT;
        }

        convert_to_long(streamind);
        convert_to_string(folder);

        ind = streamind->value.lval;

        imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
        if (!imap_stream || ind_type != le_imap) {
                php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
        }
        if ( mail_subscribe(imap_stream,folder->value.str.val)==T ) {
        RETURN_TRUE;
        } else {
                RETURN_FALSE;
        }
}
void php3_imap_unsubscribe(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *streamind, *folder;
        int ind, ind_type;
        MAILSTREAM *imap_stream;

        if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&folder) == FAILURE) {
                WRONG_PARAM_COUNT;
        }

        convert_to_long(streamind);
        convert_to_string(folder);

        ind = streamind->value.lval;

        imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
        if (!imap_stream || ind_type != le_imap) {
                php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
        }
        if ( mail_unsubscribe(imap_stream,folder->value.str.val)==T ) {
        RETURN_TRUE;
        } else {
                RETURN_FALSE;
        }
}


void imap_add_body( pval *arg, BODY *body )
{
 pval parametres, param, dparametres, dparam;
 PARAMETER *par, *dpar;
 PART *part;

if(body->type) add_property_long( arg, "type", body->type );
if(body->encoding) add_property_long( arg, "encoding", body->encoding );

 if ( body->subtype ){
   add_property_long( arg, "ifsubtype", 1 );
   add_property_string( arg, "subtype",  body->subtype, 1 );
 } else {
   add_property_long( arg, "ifsubtype", 0 );
 }

 if ( body->description ){
   add_property_long( arg, "ifdescription", 1 );
   add_property_string( arg, "description",  body->description, 1 );
 } else {
   add_property_long( arg, "ifdescription", 0 );
 }
 if ( body->id ){
   add_property_long( arg, "ifid", 1 );
   if(body->description) add_property_string( arg, "id",  body->description, 1 );
 } else {
   add_property_long( arg, "ifid", 0 );
 }

if(body->size.lines) add_property_long( arg, "lines", body->size.lines );
if(body->size.bytes) add_property_long( arg, "bytes", body->size.bytes );
#ifdef IMAP41
if ( body->disposition.type ){
   add_property_long( arg, "ifdisposition", 1);
   add_property_string( arg, "disposition", body->disposition.type, 1);
 } else {
   add_property_long( arg, "ifdisposition", 0);
}

if ( body->disposition.parameter )
{
	dpar = body->disposition.parameter;
	add_property_long( arg, "ifdparametres", 1);
	array_init( &dparametres );
	do {
		object_init( &dparam );
		add_property_string( &dparam, "attribute", dpar->attribute, 1);
		add_property_string( &dparam, "value", dpar->value, 1);
		add_next_index_object( &dparametres, dparam );
        } while ( (dpar = dpar->next) );
	add_assoc_object( arg, "dparametres", dparametres );
}
else
{
	add_property_long( arg, "ifdparametres", 0);
}
#endif
 
 if ( (par = body->parameter) ) {
    add_property_long( arg, "ifparametres", 1 );

    array_init( &parametres );
    do{
       object_init( &param );
       if(par->attribute) add_property_string( &param, "attribute", par->attribute, 1 );
       if(par->value) add_property_string( &param, "value", par->value, 1 );

       add_next_index_object( &parametres, param );
    } while ( (par = par->next) );
 } else {
   object_init(&parametres);
   add_property_long( arg, "ifparametres", 0 );
 }
    add_assoc_object( arg, "parametres", parametres );

 /* multipart message ? */

 if ( body->type == TYPEMULTIPART ){
    array_init( &parametres );
    for ( part = body->CONTENT_PART; part; part = part->next ){
        object_init( &param );
        imap_add_body( &param, &part->body );
        add_next_index_object( &parametres, param );
    }
    add_assoc_object( arg, "parts", parametres );
 }

 /* encapsulated message ? */

 if ( ( body->type == TYPEMESSAGE ) && (!strncasecmp(body->subtype, "rfc822", 6))){
   body=body->CONTENT_MSG_BODY;
   array_init(&parametres);
    object_init( &param );
    imap_add_body( &param, body );
    add_next_index_object(&parametres, param );
    add_assoc_object( arg, "parts", parametres );
 }
}

void php3_imap_fetchstructure(INTERNAL_FUNCTION_PARAMETERS)
{
 pval *streamind, *msgno;
 int ind, ind_type;
 MAILSTREAM *imap_stream;
 BODY *body;

 if ( ARG_COUNT(ht) != 2 || getParameters( ht, 2, &streamind, &msgno ) == FAILURE ) {
    WRONG_PARAM_COUNT;
 }

 convert_to_long( streamind );
 convert_to_long( msgno );
 object_init(return_value);

 ind = streamind->value.lval;

 imap_stream = ( MAILSTREAM *)php3_list_find( ind, &ind_type );

 if ( !imap_stream || ind_type != le_imap ) {
    php3_error( E_WARNING, "Unable to find stream pointer" );
    RETURN_FALSE;
 }

 mail_fetchstructure( imap_stream, msgno->value.lval, &body );

 if ( !body ) {
    php3_error( E_WARNING, "No body information available" );
    RETURN_FALSE;
 }

 imap_add_body( return_value, body );

}

void php3_imap_fetchbody(INTERNAL_FUNCTION_PARAMETERS)
{
 pval *streamind, *msgno, *sec,*flags;
 int ind, ind_type;
 MAILSTREAM *imap_stream;
 char *body;
 unsigned long len;
 int myargc=ARG_COUNT(ht);
 if(myargc < 3 || myargc >4 || getParameters( ht, myargc, &streamind, &msgno, &sec,&flags ) == FAILURE ) {
    WRONG_PARAM_COUNT;
 }

 convert_to_long( streamind );
 convert_to_long( msgno );
 convert_to_string( sec );
 if(myargc == 4) convert_to_long(flags);
 ind = streamind->value.lval;

 imap_stream = ( MAILSTREAM *)php3_list_find( ind, &ind_type );

 if ( !imap_stream || ind_type != le_imap ) {
    php3_error( E_WARNING, "Unable to find stream pointer" );
    RETURN_FALSE;
 }
 
 body = mail_fetchbody_full( imap_stream, msgno->value.lval, sec->value.str.val, &len,myargc == 4 ? flags->value.lval : NIL );

 if ( !body ) {
    php3_error( E_WARNING, "No body information available" );
    RETURN_FALSE;
 }
 RETVAL_STRING( body ,1);
}

void php3_imap_base64(INTERNAL_FUNCTION_PARAMETERS)
{
 pval *text;
 char *decode;
 unsigned long newlength;
 if ( ARG_COUNT(ht) != 1 || getParameters( ht, 1, &text) == FAILURE ) {
    WRONG_PARAM_COUNT;
 }

 convert_to_string( text );
 object_init(return_value);

    decode = (char *) rfc822_base64((unsigned char *) text->value.str.val,
    text->value.str.len,&newlength);
RETVAL_STRINGL(decode,newlength,1);
}

void php3_imap_qprint(INTERNAL_FUNCTION_PARAMETERS)
{
 pval *text;
 char *decode;
 unsigned long newlength;
 if ( ARG_COUNT(ht) != 1 || getParameters( ht, 1, &text) == FAILURE ) {
    WRONG_PARAM_COUNT;
 }

 convert_to_string( text );
 object_init(return_value);

    decode = (char *) rfc822_qprint((unsigned char *) text->value.str.val,
    text->value.str.len,&newlength);
RETVAL_STRINGL(decode,newlength,1);
}

void php3_imap_8bit(INTERNAL_FUNCTION_PARAMETERS)
{
 pval *text;
 char *decode;
 unsigned long newlength;
 if ( ARG_COUNT(ht) != 1 || getParameters( ht, 1, &text) == FAILURE ) {
    WRONG_PARAM_COUNT;
 }

 convert_to_string( text );
 object_init(return_value);

    decode = (char *) rfc822_8bit((unsigned char *) text->value.str.val,
    text->value.str.len,&newlength);
RETVAL_STRINGL(decode,newlength,1);
}

void php3_imap_binary(INTERNAL_FUNCTION_PARAMETERS)
{
 pval *text,*length;
#if 0
 char *decode;
 FILE *fp;
 int x;
 char *dest;
 unsigned long newlength;
#endif

 if ( ARG_COUNT(ht) != 2 || getParameters( ht, 2, &text,&length) == FAILURE ) {
    WRONG_PARAM_COUNT;
 }
}


void php3_imap_mailboxmsginfo(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *streamind;
	char date[50];
        int ind, ind_type;
        unsigned int msgno;
        MAILSTREAM *imap_stream;
        unsigned unreadmsg,msize;

        if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) ==
FAILURE) {
                WRONG_PARAM_COUNT;
        }

        convert_to_long(streamind);

        ind = streamind->value.lval;

        imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);

        if(!imap_stream || ind_type != le_imap) {
                php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
        }

        /* Initialize return array */
        if(object_init(return_value) == FAILURE) {
                RETURN_FALSE;
        }

        unreadmsg = 0;
        msize = 0;
        for (msgno = 1; msgno <= imap_stream->nmsgs; msgno++) {
                MESSAGECACHE * cache = mail_elt (imap_stream,msgno);
                mail_fetchstructure (imap_stream,msgno,NIL);
                unreadmsg = cache->recent ? (cache->seen ? unreadmsg :
unreadmsg++) : unreadmsg;
                unreadmsg = (cache->recent | cache->seen) ? unreadmsg :
unreadmsg++;
                msize = msize + cache->rfc822_size;
        }
        add_property_long(return_value,"Unread",unreadmsg);
        add_property_long(return_value,"Nmsgs",imap_stream->nmsgs);
        add_property_long(return_value,"Size",msize);
	rfc822_date (date);
	add_property_string(return_value,"Date",date,1);
	add_property_string(return_value,"Driver",imap_stream->dtb->name,1);
	add_property_string(return_value,"Mailbox",imap_stream->mailbox,1);
	add_property_long(return_value,"Recent",imap_stream->recent);
}

void php3_imap_rfc822_write_address(INTERNAL_FUNCTION_PARAMETERS)
{
  pval *mailbox,*host,*personal;
  ADDRESS *addr;
  char string[MAILTMPLEN];
  int argc;
  argc=ARG_COUNT(ht);
  if ( argc <2 || argc>3 || getParameters( ht, argc, &mailbox,&host,&personal) == FAILURE ) {
    WRONG_PARAM_COUNT;
  }
  convert_to_string(mailbox);
  convert_to_string(host);
  convert_to_string(personal);
  addr=mail_newaddr();
  if(mailbox) addr->mailbox=cpystr(mailbox->value.str.val);
  if(host) addr->host=cpystr(host->value.str.val);
  if(personal) addr->personal=cpystr(personal->value.str.val);
  addr->next=NIL;
  addr->error=NIL;
  addr->adl=NIL;
  string[0]=0x00;
  
  rfc822_write_address(string,addr);
  RETVAL_STRING(string,1);
}

void php3_imap_rfc822_parse_adrlist(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *string,*defaulthost;
	ENVELOPE *env;
	int argc;

	env=mail_newenvelope();
	argc=ARG_COUNT(ht);
	if ( argc != 2 || getParameters( ht, argc, &string,&defaulthost) == FAILURE ) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(string);
	convert_to_string(defaulthost);
	rfc822_parse_adrlist(&env->to,string->value.str.val,defaulthost->value.str.val);
	if(object_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	if(env->to->mailbox) add_property_string(return_value,"mailbox",env->to->mailbox,1);
	if(env->to->host) add_property_string(return_value,"host",env->to->host,1);
	if(env->to->personal) add_property_string(return_value,"personal",env->to->personal,1);
}


void php3_imap_setflag_full(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *streamind;
	pval *sequence;
	pval *flag;
	pval *flags;
	int ind,ind_type;
        MAILSTREAM *imap_stream;
	int myargc=ARG_COUNT(ht);
        if (myargc < 3 || myargc > 4 || getParameters(ht, myargc, &streamind,&sequence,&flag,&flags) ==FAILURE) {
                WRONG_PARAM_COUNT;
        }
        convert_to_long(streamind);
	convert_to_string(sequence);
	convert_to_string(flag);
	if(myargc==4) convert_to_long(flags);

        ind = streamind->value.lval;
        imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
        if(!imap_stream || ind_type != le_imap) {
                php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
        }
	  mail_setflag_full(imap_stream,sequence->value.str.val,flag->value.str.val,myargc == 4 ? flags->value.lval : NIL);

}

void php3_imap_clearflag_full(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *streamind;
	pval *sequence;
	pval *flag;
	pval *flags;
	int ind,ind_type;
        MAILSTREAM *imap_stream;
	int myargc=ARG_COUNT(ht);
        if (myargc < 3 || myargc > 4 || getParameters(ht, myargc, &streamind,&sequence,&flag,&flags) ==FAILURE) {
                WRONG_PARAM_COUNT;
        }
        convert_to_long(streamind);
	convert_to_string(sequence);
	convert_to_string(flag);
	if(myargc==4) convert_to_long(flags);

        ind = streamind->value.lval;
        imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
        if(!imap_stream || ind_type != le_imap) {
                php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
        }
	mail_clearflag_full(imap_stream,sequence->value.str.val,flag->value.str.val,myargc == 4 ? flags->value.lval : NIL);
}

void php3_imap_sort(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *streamind;
	pval *pgm;
	pval *rev;
	pval *flags;
	int ind,ind_type;
	unsigned long *slst,*sl;
	char mystring[100];
	SORTPGM *mypgm;
        MAILSTREAM *imap_stream;
	int myargc=ARG_COUNT(ht);
        if (myargc < 3 || myargc > 4 || getParameters(ht, myargc, &streamind,&pgm,&rev,&flags) ==FAILURE) {
                WRONG_PARAM_COUNT;
        }
        convert_to_long(streamind);
        convert_to_long(rev);
	convert_to_long(pgm);
	if(myargc==4) convert_to_long(flags);

        ind = streamind->value.lval;
        imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
        if(!imap_stream || ind_type != le_imap) {
                php3_error(E_WARNING, "Unable to find stream pointer");
                RETURN_FALSE;
        }
	mypgm=mail_newsortpgm();
	mypgm->reverse=rev->value.lval;
	mypgm->function=pgm->value.lval;
	mypgm->next=NIL;

        array_init(return_value);
	slst=mail_sort(imap_stream,NIL,NIL,mypgm,myargc == 4 ? flags->value.lval:NIL);
	for (sl = slst; *sl; sl++) 
	  { 
	    sprintf (mystring,"%lu",*sl);
	    add_next_index_string(return_value,mystring,1);
	  }
	fs_give ((void **) &slst);
}


void php3_imap_fetchheader(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, * msgno, * flags;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	int myargc = ARG_COUNT(ht);
	if (myargc < 2 || myargc > 3 || getParameters(ht,myargc,&streamind,&msgno,&flags) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(msgno);
	if(myargc == 3) convert_to_long(flags);
	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if (!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETVAL_STRING(mail_fetchheader_full (imap_stream,msgno->value.lval,NIL,NIL,myargc == 3 ? flags->value.lval : NIL),1);

}

 void php3_imap_uid(INTERNAL_FUNCTION_PARAMETERS)
 {
 	pval *streamind, *msgno;
 	int ind, ind_type;
 	MAILSTREAM *imap_stream;
 
 	if (ARG_COUNT(ht) != 2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
 		WRONG_PARAM_COUNT;
 	}
 	
 	convert_to_long(streamind);
 	convert_to_long(msgno);
 
 	ind = streamind->value.lval;
 
         imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
 
         if (!imap_stream || ind_type != le_imap) {
 		php3_error(E_WARNING, "Unable to find stream pointer");
 	        RETURN_FALSE;
 	}
 
 	RETURN_LONG(mail_uid(imap_stream, msgno->value.lval));
 }
  
  
 
 void php3_imap_msgno(INTERNAL_FUNCTION_PARAMETERS)
 {
 	pval *streamind, *msgno;
 	int ind, ind_type;
 	MAILSTREAM *imap_stream;
 
 	if (ARG_COUNT(ht) != 2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
 		WRONG_PARAM_COUNT;
 	}
 	
 	convert_to_long(streamind);
 	convert_to_long(msgno);
 
 	ind = streamind->value.lval;
 
         imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
 
         if (!imap_stream || ind_type != le_imap) {
 		php3_error(E_WARNING, "Unable to find stream pointer");
 	        RETURN_FALSE;
 	}
 
 	RETURN_LONG(mail_msgno(imap_stream, msgno->value.lval));
 }
 

/* Interfaces to C-client */


void mm_searched (MAILSTREAM *stream,unsigned long number)
{
}


void mm_exists (MAILSTREAM *stream,unsigned long number)
{
}


void mm_expunged (MAILSTREAM *stream,unsigned long number)
{
}


void mm_flags (MAILSTREAM *stream,unsigned long number)
{
}


void mm_notify (MAILSTREAM *stream,char *string,long errflg)
{
}

void mm_list (MAILSTREAM *stream,DTYPE delimiter,char *mailbox,long attributes)
{
	STRINGLIST *cur=NIL;
if(!(attributes & LATT_NOSELECT))
{
	if (imap_folders == NIL) {
		imap_folders=mail_newstringlist();
		imap_folders->LSIZE=strlen(imap_folders->LTEXT=cpystr(mailbox));
		imap_folders->next=NIL; 
	} else {
		cur=imap_folders;
		while (cur->next != NIL ) {
			cur=cur->next;
		}
		cur->next=mail_newstringlist ();
		cur=cur->next;
		cur->LSIZE = strlen (cur->LTEXT = cpystr (mailbox));
		cur->next = NIL;
	}
}
}

void mm_lsub (MAILSTREAM *stream,DTYPE delimiter,char *mailbox,long attributes)
{
        STRINGLIST *cur=NIL;
        if (imap_sfolders == NIL) {
                imap_sfolders=mail_newstringlist();
        imap_sfolders->LSIZE=strlen(imap_sfolders->LTEXT=cpystr(mailbox));
                imap_sfolders->next=NIL; 
        } else {
                cur=imap_sfolders;
                while (cur->next != NIL ) {
                        cur=cur->next;
                }
                cur->next=mail_newstringlist ();
                cur=cur->next;
                cur->LSIZE = strlen (cur->LTEXT = cpystr (mailbox));
                cur->next = NIL;
        }
}


void mm_status (MAILSTREAM *stream,char *mailbox,MAILSTATUS *status)
{
}


void mm_log (char *string,long errflg)
{
   switch ((short) errflg) {
   case NIL:
     php3_error(E_NOTICE,string); /* messages */
     break;
   case PARSE:
   case WARN:
     php3_error(E_WARNING,string); /* warnings */
     break;
   case ERROR:
     php3_error(E_NOTICE,string);   /* errors */
     break;
   }
}


void mm_dlog (char *string)
{
   php3_error(E_NOTICE,string);
}

void mm_login (NETMBX *mb,char *user,char *pwd,long trial)
{
	if (*mb->user) {
		strcpy (user,mb->user);
	} else {
		strcpy (user,imap_user);
	}
	strcpy (pwd,imap_password);
}


void mm_critical (MAILSTREAM *stream)
{
}


void mm_nocritical (MAILSTREAM *stream)
{
}


long mm_diskerror (MAILSTREAM *stream,long errcode,long serious)
{
	return 1;
}


void mm_fatal (char *string)
{
}

#endif
