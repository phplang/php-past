/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
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
   | Authors: Amitay Isaacs <amitay@w-o-i.com>                            |
   |          Eric Warnke   <ericw@albany.edu>                            |
   +----------------------------------------------------------------------+
 */


/* $Id: php3_ldap.h,v 1.30 2000/01/01 04:44:09 sas Exp $ */

#ifndef _PHP3_LDAP_H
#define _PHP3_LDAP_H

#if COMPILE_DL
#undef HAVE_LDAP
#define HAVE_LDAP 1
#endif

#if HAVE_LDAP
#include <lber.h>
#include <ldap.h>

extern php3_module_entry ldap_module_entry;
#define ldap_module_ptr &ldap_module_entry

/* LDAP functions */
extern int php3_minit_ldap(INIT_FUNC_ARGS);
extern int php3_mshutdown_ldap(void);

extern void php3_info_ldap(void);

extern void php3_ldap_connect(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ldap_bind(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_unbind(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ldap_read(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_list(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_search(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ldap_free_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_count_entries(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ldap_first_entry(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_next_entry(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_get_entries(INTERNAL_FUNCTION_PARAMETERS);
#if 0
extern void php3_ldap_free_entry(INTERNAL_FUNCTION_PARAMETERS);
#endif
extern void php3_ldap_first_attribute(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_next_attribute(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_get_attributes(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ldap_get_values(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_get_values_len(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ber_free(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_get_dn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_explode_dn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_dn2ufn(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ldap_add(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_delete(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_modify(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_mod_add(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_mod_del(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_mod_replace(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_errno(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_error(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_err2str(INTERNAL_FUNCTION_PARAMETERS);

typedef struct {
	long default_link;
	long num_links, max_links;
	char *base_dn;
	int le_result, le_result_entry, le_ber_entry;
	int le_link;
} ldap_module;

extern ldap_module php3_ldap_module;

#else

#define ldap_module_ptr NULL

#endif

#endif /* _PHP3_LDAP_H */
