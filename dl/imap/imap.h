#ifndef _INCLUDED_IMAP_H
#define _INCLUDED_IMAP_H

#include "../phpdl.h"

/* Functions accessable to PHP */

extern int imap_init(INIT_FUNC_ARGS);
void php3_imap_open(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_reopen(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_close(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_num_msg(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_headers(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_body(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_expunge(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_delete(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_undelete(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_check(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_headerinfo(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_mail_copy(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_mail_move(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_createmailbox(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_renamemailbox(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_deletemailbox(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_list(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_listscan(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_lsub(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_subscribe(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_unsubscribe(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_num_recent(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_fetchstructure(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_fetchbody(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_append(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_ping(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_base64(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_qprint(INTERNAL_FUNCTION_PARAMETERS);
void php3_imap_8bit(INTERNAL_FUNCTION_PARAMETERS);
#endif
