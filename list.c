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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
 */


/* $Id: list.c,v 1.91 2000/04/10 19:29:36 andi Exp $ */

#include "php.h"
#include "php3_list.h"
#include "modules.h"

#include "functions/db.h"

#ifdef HAVE_DIRENT_H
# include <dirent.h>
#endif

#if MSVC5
#if !(APACHE)
#define NEEDRDH 1
#endif
#include "win32/readdir.h"
#endif
/* HashTable list,plist; */
PHPAPI int le_index_ptr;

PHPAPI int php3_list_do_insert(HashTable *list,void *ptr, int type)
{
	int index;
	list_entry le;

	index = _php3_hash_next_free_element(list);

	if (index==0) index++;

	le.ptr=ptr;
	le.type=type;
	if (_php3_hash_index_update(list, index, (void *) &le, sizeof(list_entry), NULL)==FAILURE) {
		php3_log_err("Failed inserting resource");
	}
	return index;
}

PHPAPI int php3_list_do_delete(HashTable *list,int id)
{
	return _php3_hash_index_del(list, id);
}

PHPAPI void *php3_list_do_find(HashTable *list,int id, int *type)
{
	list_entry *le;

	if (_php3_hash_index_find(list, id, (void **) &le)==SUCCESS) {
		*type = le->type;
		return le->ptr;
	} else {
		*type = -1;
		return NULL;
	}
}

PHPAPI void list_entry_destructor(void *ptr)
{
	list_entry *le = (list_entry *) ptr;
	list_destructors_entry *ld;
	TLS_VARS;
	
	if (_php3_hash_index_find(&GLOBAL(list_destructors),le->type,(void **) &ld)==SUCCESS) {
		if (ld->list_destructor) {
			(ld->list_destructor)(le->ptr);
		}
	} else {
		php3_error(E_WARNING,"Unknown list entry type in request shutdown (%d)",le->type);
	}
}


PHPAPI void plist_entry_destructor(void *ptr)
{
	list_entry *le = (list_entry *) ptr;
	list_destructors_entry *ld;
	TLS_VARS;

	if (_php3_hash_index_find(&GLOBAL(list_destructors),le->type,(void **) &ld)==SUCCESS) {
		if (ld->plist_destructor) {
			(ld->plist_destructor)(le->ptr);
		}
	} else {
		php3_error(E_WARNING,"Unknown persistent list entry type in module shutdown (%d)",le->type);
	}
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
