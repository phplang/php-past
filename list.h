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
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: list.h,v 1.43 1998/01/17 21:23:29 andi Exp $ */


#ifndef _LIST_H
#define _LIST_H

#ifndef THREAD_SAFE
extern HashTable list_destructors;
#endif

typedef struct {
	void *ptr;
	int type;
} list_entry;

typedef struct {
	void (*list_destructor)(void *);
	void (*plist_destructor)(void *);
} list_destructors_entry;

#define register_list_destructors(ld,pld) _register_list_destructors((void (*)(void *))ld, (void (*)(void *))pld);
extern PHPAPI int _register_list_destructors(void (*ld)(void *), void (*pld)(void *));

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

#endif
