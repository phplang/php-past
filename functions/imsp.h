/* $Id: imsp.h,v 1.2 1999/12/26 18:44:46 rasmus Exp $ */

#ifndef _INCLUDED_IMSP_H
#define _INCLUDED_IMSP_H

#if COMPILE_DL
#undef HAVE_IMSP
#define HAVE_IMSP 1
#endif

#if HAVE_IMSP

#ifndef MSVC5
#include "build-defs.h"
#endif

/* Functions accessable to PHP */
extern php3_module_entry php3_imsp_module_entry;
#define php3_imsp_module_ptr &php3_imsp_module_entry

extern int imsp_init (INIT_FUNC_ARGS);
extern void imsp_info (void);
void php3_imsp_open (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_close (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_option_get (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_option_set (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_option_unset (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_create (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_delete (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_rename (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_find (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_getlist (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_search (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_fetch (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_lock (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_unlock (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_store (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_deleteent (INTERNAL_FUNCTION_PARAMETERS);
void php3_imsp_abook_expand (INTERNAL_FUNCTION_PARAMETERS);
#else
#define php3_imsp_module_ptr NULL
#endif /* HAVE_IMSP */

#endif
