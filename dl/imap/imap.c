#include "imap.h"
#include "modules.h"
#include "list.h"

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#ifdef ERROR
#undef ERROR
#endif
#include "mail.h"
#include "rfc822.h"
MAILSTREAM *mail_close_it (MAILSTREAM *stream);

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
	{"imap_num_msg", php3_imap_num_msg, NULL},
	{"imap_headers", php3_imap_headers, NULL},
	{"imap_header", php3_imap_header, NULL},
	{"imap_body", php3_imap_body, NULL},
	{"imap_expunge", php3_imap_expunge, NULL},
	{"imap_delete", php3_imap_delete, NULL},
	{"imap_undelete", php3_imap_undelete, NULL},
	{"imap_check", php3_imap_check, NULL},
	{"imap_close", php3_imap_close, NULL},
	{NULL, NULL, NULL}
};


php3_module_entry imap_module_entry = {
	"Imap", imap_functions, imap_init, NULL, NULL, NULL, NULL, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &imap_module_entry; }
#endif

/* 
   I beleive since this global is used ONLY within this module,
   and nothing will link to this module, we can use the simple 
   thread local storage
*/
#if WIN32|WINNT
#define THREAD __declspec(thread)
#else
#define THREAD
#endif
THREAD int le_imap;
THREAD char imap_user[80]="";
THREAD char imap_password[80]="";

MAILSTREAM *mail_close_it (MAILSTREAM *stream)
{
	return mail_close_full ( stream,NIL);
}

int imap_init(INITFUNCARG)
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


DLEXPORT void php3_imap_open(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *mailbox;
	YYSTYPE *user;
	YYSTYPE *passwd;
	MAILSTREAM *imap_stream;
	int ind;

	if (getParameters(ht, 3, &mailbox,&user,&passwd) == SUCCESS) {
		convert_to_string(mailbox);
		convert_to_string(user);
		convert_to_string(passwd);
		strcpy(imap_user,user->value.strval);
		strcpy(imap_password,passwd->value.strval);

		imap_stream = NIL;
		imap_stream = mail_open(imap_stream,mailbox->value.strval,NIL);
		if (imap_stream == NIL){
			php3_error(E_WARNING,"Couldn't open stream\n");
			RETURN_FALSE;
		}
		ind = php3_list_insert(imap_stream, le_imap);

		RETURN_LONG(ind);
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}

DLEXPORT void php3_imap_num_msg(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *streamind;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);

	if(!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	RETURN_LONG(imap_stream->nmsgs);
}

DLEXPORT void php3_imap_expunge(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *streamind;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);

	if(!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_expunge (imap_stream);

	RETURN_TRUE;
}


DLEXPORT void php3_imap_close(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *ind;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &ind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(ind);
	php3_list_delete(ind->value.lval);
	RETURN_TRUE;
}


DLEXPORT void php3_imap_headers(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *streamind;
	int ind, ind_type;
	unsigned int msgno;
	MAILSTREAM *imap_stream;
	unsigned long i;
	char tmp[MAILTMPLEN];
	char *t;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
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
	if(array_init(return_value) == FAILURE) {
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
		if (i = cache->user_flags) {
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

DLEXPORT void php3_imap_body(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *streamind, * msgno;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if(ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(msgno);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if(!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}
	RETVAL_STRING(mail_fetchtext (imap_stream,msgno->value.lval));

}

DLEXPORT void php3_imap_check(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *streamind;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	char date[50];
	char tmp[150];

	if(ARG_COUNT(ht)!=1 || getParameters(ht,1,&streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if(!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_check (imap_stream);
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

DLEXPORT void php3_imap_delete(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *streamind, * msgno;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if(ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(msgno);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if(!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_setflag (imap_stream,msgno->value.strval,"\\DELETED");
	RETVAL_TRUE;

}

DLEXPORT void php3_imap_undelete(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *streamind, * msgno;
	int ind, ind_type;
	MAILSTREAM *imap_stream;

	if(ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_string(msgno);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if(!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}

	mail_clearflag (imap_stream,msgno->value.strval,"\\DELETED");
	RETVAL_TRUE;

}

DLEXPORT void php3_imap_header(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *streamind, * msgno;
	int ind, ind_type;
	MAILSTREAM *imap_stream;
	STRINGLIST *lines=NULL;
	STRINGLIST *cur=NULL;

	if(ARG_COUNT(ht)!=2 || getParameters(ht,2,&streamind,&msgno)==FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(streamind);
	convert_to_long(msgno);

	ind = streamind->value.lval;

	imap_stream = ( MAILSTREAM *)php3_list_find(ind, &ind_type);
	if(!imap_stream || ind_type != le_imap) {
		php3_error(E_WARNING, "Unable to find stream pointer");
		RETURN_FALSE;
	}


    object_init(return_value);
    lines = mail_newstringlist ();
	lines->size = strlen (lines->text = (char *)cpystr ("Date"));
    add_property_string(return_value,"Date"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->size = strlen (lines->text = (char *)cpystr ("From"));
    add_property_string(return_value,"From"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->size = strlen (lines->text = (char *)cpystr (">From"));
    add_property_string(return_value,"From2"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->size = strlen (lines->text = (char *)cpystr ("Subject"));
    add_property_string(return_value,"Subject"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->size = strlen (lines->text = (char *)cpystr ("To"));
    add_property_string(return_value,"To"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->size = strlen (lines->text = (char *)cpystr ("cc"));
    add_property_string(return_value,"cc"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
    lines = mail_newstringlist ();
	lines->size = strlen (lines->text = (char *)cpystr ("Reply-To"));
    add_property_string(return_value,"ReplyTo"
		,mail_fetchheader_full(imap_stream,msgno->value.lval,lines,NIL,NIL),1);
	mail_free_stringlist (&lines);
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


void mm_list (MAILSTREAM *stream,char delimiter,char *mailbox,long attributes)
{
}


void mm_lsub (MAILSTREAM *stream,char delimiter,char *mailbox,long attributes)
{
}


void mm_status (MAILSTREAM *stream,char *mailbox,MAILSTATUS *status)
{
}


void mm_log (char *string,long errflg)
{
}


void mm_dlog (char *string)
{
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

