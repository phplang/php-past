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


/* $Id: php3_list.h,v 1.11 2000/04/10 19:29:36 andi Exp $ */


#ifndef _LIST_H
#define _LIST_H

extern HashTable list_destructors;

typedef struct {
	void *ptr;
	int type;
} list_entry;

typedef struct {
	void (*list_destructor)(void *);
	void (*plist_destructor)(void *);
	int module_number;
	int resource_id;
} list_destructors_entry;

#define register_list_destructors(ld,pld) _register_list_destructors((void (*)(void *))ld, (void (*)(void *))pld, module_number);
extern PHPAPI int _register_list_destructors(void (*ld)(void *), void (*pld)(void *), int module_number);

enum list_entry_type {
	LE_DB=1000
};

extern PHPAPI int php3_list_do_insert(HashTable *list,void *ptr, int type);
extern PHPAPI int php3_list_do_delete(HashTable *list,int id);
extern PHPAPI void *php3_list_do_find(HashTable *list,int id, int *type);
extern PHPAPI void list_entry_destructor(void *ptr);
extern PHPAPI void plist_entry_destructor(void *ptr);

#define php3_list_insert(ptr,type) php3_list_do_insert(list,(ptr),(type))
#define php3_list_delete(id) php3_list_do_delete(list,id)
#define php3_list_find(id,type) php3_list_do_find(list, (id),(type))

#define php3_plist_insert(ptr,type) php3_list_do_insert(plist,(ptr),(type))
#define php3_plist_delete(id) php3_list_do_delete(plist,id)
#define php3_plist_find(id,type) php3_list_do_find(plist,(id),(type))

extern int clean_module_resource_destructors(list_destructors_entry *ld, int *module_number);

#endif
