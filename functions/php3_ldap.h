/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors: Amitay Isaacs <amitay@cse.iitb.ernet.in>                    |
   |          Eric Warnke   <ericw@albany.edu>                            |
   +----------------------------------------------------------------------+
 */


/* $Id: php3_ldap.h,v 1.10 1998/01/14 23:52:51 amitay Exp $ */

#ifndef _PHP3_LDAP_H
#define _PHP3_LDAP_H

#if COMPILE_DL
#undef HAVE_LDAP
#define HAVE_LDAP 1
#endif

#if HAVE_LDAP

extern php3_module_entry ldap_module_entry;
#define ldap_module_ptr &ldap_module_entry

/* LDAP functions */
extern int php3_minit_ldap(INITFUNCARG);
extern int php3_rinit_ldap(INITFUNCARG);

extern void php3_info_ldap(void);

extern void php3_ldap_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_pconnect(INTERNAL_FUNCTION_PARAMETERS);

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
extern void php3_ldap_free_entry(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ldap_first_attribute(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_next_attribute(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_get_attributes(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ldap_get_attribute_values(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_get_values(INTERNAL_FUNCTION_PARAMETERS);

/*extern void php3_ber_free(INTERNAL_FUNCTION_PARAMETERS);*/
extern void php3_ldap_get_dn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_explode_rdn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_explode_dn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_dn2ufn(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ldap_add(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_delete(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ldap_modify(INTERNAL_FUNCTION_PARAMETERS);

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	char *base_dn;
	int le_result, le_result_entry, le_ber_entry;
	int le_link;
	int le_plink;
} ldap_module;

extern ldap_module php3_ldap_module;

#else

#define ldap_module_ptr NULL

#endif

#endif /* _PHP3_LDAP_H */
