#include "imap.h"
#include "modules.h"
#include "php3_list.h"

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#ifdef ERROR
#undef ERROR
#endif
#include "mail.h"
#include "rfc822.h"
MAILSTREAM *mail_close_it (MAILSTREAM *stream);
#if (WIN32|WINNT)
#include "winsock.h"
#define IMAP41
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
        {NULL, NULL, NULL}
};


php3_module_entry imap_module_entry = {
	IMAPVER, imap_functions, imap_init, NULL, NULL, NULL, NULL, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &imap_module_entry; }
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

    le_imap = register_list_destructors(mail_close_it,NULL);
	return SUCCESS;
}

void php3_imap_open(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *mailbox;
	pval *user;
	pval *passwd;
	MAILSTREAM *imap_stream;
	int ind;

	if (getParameters(ht, 3, &mailbox,&user,&passwd) == SUCCESS) {
		convert_to_string(mailbox);
		convert_to_string(user);
		convert_to_string(passwd);
		strcpy(imap_user,user->value.str.val);
		strcpy(imap_password,passwd->value.str.val);

		imap_stream = NIL;
		imap_stream = mail_open(imap_stream,mailbox->value.str.val,NIL);
		if (imap_stream == NIL){
			php3_error(E_WARNING,"Couldn't open stream %s\n",mailbox->value.str.val);
			RETURN_FALSE;
		}
		ind = php3_list_insert(imap_stream, le_imap);

		RETURN_LONG(ind);
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


void php3_imap_reopen(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	pval *mailbox;
	pval *user;
	pval *passwd;
	MAILSTREAM *imap_stream;
	int ind, ind_type;

	if (getParameters(ht, 4,&streamind, &mailbox,&user,&passwd) == FAILURE) {
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
	strcpy(imap_user,user->value.str.val);
	strcpy(imap_password,passwd->value.str.val);

	imap_stream = mail_open(imap_stream,mailbox->value.str.val,NIL);
	if (imap_stream == NIL){
		php3_error(E_WARNING,"Couldn't re-open stream\n");
		RETURN_FALSE;
	}
	RETURN_TRUE;
}

void php3_imap_append(INTERNAL_FUNCTION_PARAMETERS)
{
 pval *streamind,*folder, *message;
 int ind, ind_type;
 MAILSTREAM *imap_stream;

 if ( ARG_COUNT(ht) != 3 || getParameters( ht, 3, &streamind, &folder, &message) == FAILURE) {
    WRONG_PARAM_COUNT;
 }

 convert_to_long(streamind);
 convert_to_string(folder);
 convert_to_string(message);

 ind = streamind->value.lval;

 imap_stream = ( MAILSTREAM *)php3_list_find( ind, &ind_type );

 if ( !imap_stream || ind_type != le_imap ) {
    php3_error(E_WARNING, "Unable to find stream pointer");
    RETURN_FALSE;
 }
 mail_append( imap_stream, folder->value.str.val, (STRING *)message->value.str.val );
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

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &ind) == FAILURE) {
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
	unsigned int msgno;
	MAILSTREAM *imap_stream;
	char tmp[MAILTMPLEN];
        char tmp2[MAILTMPLEN];
        char tmp3[MAILTMPLEN];
        char tmp4[MAILTMPLEN];
        char mysubject[MAILTMPLEN];
        char myfrom[MAILTMPLEN];
/*	unused vars, left here just in case needed for some reason
	char *t;
	unsigned long i;
*/

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
                mail_fetchfrom (tmp2,imap_stream,msgno,(long) 30);
                sprintf(myfrom,"%-30s",tmp2);
                strcat(tmp,myfrom);
                strcat(tmp," ");

		/* Don't want user flags.
		if (i = cache->user_flags) {
			strcat (tmp,"{");
			while (i) {
				strcat (tmp,imap_stream->user_flags[find_rightmost_bit (&i)]);
				if (i) strcat (tmp," ");
			}
			strcat (tmp,"} ");
		}
		*/

                mail_fetchsubject(tmp3,imap_stream,msgno,(long)25);
                sprintf(mysubject,"%-25s",tmp3);
                strcat(tmp,mysubject);
                sprintf (tmp4," %ld",cache->rfc822_size);
                strcat(tmp,tmp4);
		add_next_index_string(return_value,tmp,1);
	}

}

void php3_imap_body(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, * msgno;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
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
	RETVAL_STRING(mail_fetchtext (imap_stream,msgno->value.lval),1);

}

void php3_imap_mail_copy(INTERNAL_FUNCTION_PARAMETERS)
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
	if ( mail_copy(imap_stream,seq->value.str.val,folder->value.str.val)==T ) {
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
	/* int i; unused */
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
		/* int i; unused */
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
	/*char tmp[150]; unused */

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
	pval *streamind, * msgno;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	STRINGLIST *lines=NULL;
	MESSAGECACHE * cache;
	char dummy[2000];
	if (ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno)==FAILURE) {
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

	mail_fetchstructure (imap_stream,msgno->value.lval,NIL);
	cache = mail_elt (imap_stream,msgno->value.lval);
	

	object_init(return_value);
	lines = mail_newstringlist ();
	lines->LSIZE = strlen (lines->LTEXT = (char *)cpystr ("Date"));
	add_property_string(return_value,"Date"
	    ,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
	lines = mail_newstringlist ();
	lines->LSIZE = strlen (lines->LTEXT = (char *)cpystr ("From"));
    add_property_string(return_value,"From"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->LSIZE = strlen (lines->LTEXT = (char *)cpystr (">From"));
    add_property_string(return_value,"From2"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->LSIZE = strlen (lines->LTEXT = (char *)cpystr ("Subject"));
    add_property_string(return_value,"Subject"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->LSIZE = strlen (lines->LTEXT = (char *)cpystr ("To"));
    add_property_string(return_value,"To"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->LSIZE = strlen (lines->LTEXT = (char *)cpystr ("cc"));
    add_property_string(return_value,"cc"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->LSIZE = strlen (lines->LTEXT = (char *)cpystr ("Reply-To"));
    add_property_string(return_value,"ReplyTo"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);

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
}

/* KMLANG */
void php3_imap_lsub(INTERNAL_FUNCTION_PARAMETERS)
{
        pval *streamind, *ref, *pat;
		/* int i; unused */
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

inline int add_assoc_object(pval *arg, char *key, pval tmp)
{
 return hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), NULL);
}

inline int add_next_index_object(pval *arg, pval tmp)
{
 return hash_next_index_insert( arg->value.ht, (void *) &tmp, sizeof(pval), NULL); 
}

void imap_add_body( pval *arg, BODY *body )
{
 pval parametres, param, dparametres, dparam;
 PARAMETER *par, *dpar;
 PART *part;

 add_property_long( arg, "type", body->type );
 add_property_long( arg, "encoding", body->encoding );

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
   add_property_string( arg, "id",  body->description, 1 );
 } else {
   add_property_long( arg, "ifid", 0 );
 }

 add_property_long( arg, "lines", body->size.lines );
 add_property_long( arg, "bytes", body->size.bytes );
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
        } while ( dpar = dpar->next );
	add_assoc_object( arg, "dparametres", dparametres );
}
else
{
	add_property_long( arg, "ifdparametres", 0);
}
#endif
 
 if ( par = body->parameter ) {
    add_property_long( arg, "ifparametres", 1 );

    array_init( &parametres );
    do{
       object_init( &param );
       add_property_string( &param, "attribute", par->attribute, 1 );
       add_property_string( &param, "value", par->value, 1 );

       add_next_index_object( &parametres, param );
    } while ( par = par->next );
    add_assoc_object( arg, "parametres", parametres );
 } else {
   add_property_long( arg, "ifparametres", 0 );
 }

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

 if ( ( body->type == TYPEMESSAGE ) && ( body = body->CONTENT_MSG_BODY )){
    object_init( &param );
    imap_add_body( &param, body );
    add_assoc_object( arg, "parts", param );
 }
}

void php3_imap_fetchstructure(INTERNAL_FUNCTION_PARAMETERS)
{
 pval *streamind, *msgno;
 int ind, ind_type;
 MAILSTREAM *imap_stream;
 BODY *body;
 /* char dummy[100]; not used */

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

// mail_free_body_data( body );
}

void php3_imap_fetchbody(INTERNAL_FUNCTION_PARAMETERS)
{
 pval *streamind, *msgno, *sec;
 int ind, ind_type;
 MAILSTREAM *imap_stream;
 char *body;
 unsigned long len;

 if ( ARG_COUNT(ht) != 3 || getParameters( ht, 3, &streamind, &msgno, &sec ) == FAILURE ) {
    WRONG_PARAM_COUNT;
 }

 convert_to_long( streamind );
 convert_to_long( msgno );
 convert_to_string( sec );

 ind = streamind->value.lval;

 imap_stream = ( MAILSTREAM *)php3_list_find( ind, &ind_type );

 if ( !imap_stream || ind_type != le_imap ) {
    php3_error( E_WARNING, "Unable to find stream pointer" );
    RETURN_FALSE;
 }
 
 body = mail_fetchbody( imap_stream, msgno->value.lval, sec->value.str.val, &len );

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


