/* $Id: php3_COM.h,v 1.3 1998/12/28 09:43:58 sas Exp $ */

#ifndef _PHP3_COM_H
#define _PHP3_COM_H

#if WIN32|WINNT

extern int php3_minit_COM(INIT_FUNC_ARGS);
extern int php3_mshutdown_COM(void);
extern void php3_COM_load(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_COM_invoke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_COM_propget(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_COM_propput(INTERNAL_FUNCTION_PARAMETERS);

extern php3_module_entry COM_module_entry;
#define COM_module_ptr &COM_module_entry

#else

#define COM_module_ptr NULL

#endif  /* Win32|WINNT */

#endif  /* _PHP3_COM_H */
