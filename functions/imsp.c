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
   | Authors: Charles Hagenbuch   <chuck@horde.org>                       |
   +----------------------------------------------------------------------+
 */
/* $Id: imsp.c,v 1.5 2000/09/30 17:32:44 sas Exp $ */

#ifdef ERROR
#undef ERROR
#endif

#if !(WIN32|WINNT)
#include "config.h"
#endif
#include "php.h"
#include "internal_functions.h"

#if COMPILE_DL
#include "dl/phpdl.h"
#endif

#if HAVE_IMSP

#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include "php3_list.h"
#include "imsp.h"
#include "mail.h"
#include "imap4r1.h"
#include "imutil.h"
#include "support.h"
#include "modules.h"
#if (WIN32|WINNT)
#include "winsock.h"
#endif

extern void rfc822_date(char *date);
extern char *cpystr(const char *string);
extern unsigned long find_rightmost_bit (unsigned long *valptr);
extern long imsp_open(SUPPORTSTREAM *s, char *hostname);
extern void fs_give (void **block);
extern void *fs_get (size_t size);
#ifdef HAVE_IMAP
extern int add_assoc_object(pval *arg, char *key, pval tmp);
extern int add_next_index_object(pval *arg, pval tmp);
#else
int add_next_index_object(pval *arg, pval tmp);
int add_assoc_object(pval *arg, char *key, pval tmp);
#endif

typedef struct php3_imsp_le_struct {
	SUPPORTSTREAM *imsp_stream;
	long flags;
} pimls;

typedef struct php3_imsp_option_struct {
	SIZEDTEXT key;
	SIZEDTEXT val;
	struct php3_imsp_option_struct *next;
} OPTIONLIST;

typedef struct php3_imsp_abook_struct {
	SIZEDTEXT name;
	char seperator;
	struct php3_imsp_abook_struct *next;
} ABOOKLIST;

typedef struct php3_imsp_address_struct {
	SIZEDTEXT name;
	keyvalue *kv;
	int count;
	struct php3_imsp_address_struct *next;
} ADDRESSLIST;

SUPPORTSTREAM *imsp_close_it (pimls *imsp_le_struct);

#ifndef HAVE_IMAP
inline int add_assoc_object(pval *arg, char *key, pval tmp)
{
	return _php3_hash_update(arg->value.ht, key, strlen(key)+1, (void *) &tmp, sizeof(pval), NULL);
}

inline int add_next_index_object(pval *arg, pval tmp)
{
	return _php3_hash_next_index_insert(arg->value.ht, (void *) &tmp, sizeof(pval), NULL); 
}
#endif


/* 
 * this array should be set up as:
 * {"PHPScriptFunctionName", dllFunctionName, 1} 
 */

function_entry imsp_functions[] = {
	{"imsp_open", php3_imsp_open, NULL},
	{"imsp_close", php3_imsp_close, NULL},
	{"imsp_option_get", php3_imsp_option_get, NULL},
	{"imsp_option_set", php3_imsp_option_set, NULL},
	{"imsp_option_unset", php3_imsp_option_unset, NULL},
	{"imsp_abook_create", php3_imsp_abook_create, NULL},
	{"imsp_abook_delete", php3_imsp_abook_delete, NULL},
	{"imsp_abook_rename", php3_imsp_abook_rename, NULL},
	{"imsp_abook_find", php3_imsp_abook_find, NULL},
	{"imsp_abook_getlist", php3_imsp_abook_getlist, NULL},
	{"imsp_abook_search", php3_imsp_abook_search, NULL},
	{"imsp_abook_fetch", php3_imsp_abook_fetch, NULL},
	{"imsp_abook_lock", php3_imsp_abook_lock, NULL},
	{"imsp_abook_unlock", php3_imsp_abook_unlock, NULL},
	{"imsp_abook_store", php3_imsp_abook_store, NULL},
	{"imsp_abook_deleteent", php3_imsp_abook_deleteent, NULL},
	{"imsp_abook_expand", php3_imsp_abook_expand, NULL},
	{NULL, NULL, NULL}
};

#define	IMSP_IS_STREAM(ind_type) ((ind_type) == le_imsp)

php3_module_entry php3_imsp_module_entry = {
	"IMSP", imsp_functions, imsp_init, NULL, NULL, NULL, imsp_info, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &php3_imsp_module_entry; }
#endif

/* 
   I believe since this global is used ONLY within this module,
   and nothing will link to this module, we can use the simple 
   thread local storage
*/
int le_imsp;
int le_abook;
char imsp_user[80] = "";
char imsp_password[80] = "";
void kv_free (keyvalue *kv, int count);
#ifdef HAVE_IMAP
extern char imap_user[80];
extern char imap_password[80];
#endif
OPTIONLIST *imsp_options = NIL;
ABOOKLIST *imsp_abooks = NIL;
ADDRESSLIST *imsp_addresses = NIL;


/* IMSP instantiate OPTIONLIST
 * Returns: new OPTIONLIST list
 */
OPTIONLIST *imsp_new_optionlist (void) {
	return (OPTIONLIST *) memset(fs_get(sizeof(OPTIONLIST)), 0, sizeof(OPTIONLIST));
}

/* IMSP garbage collect OPTIONLIST
 * Accepts: pointer to OPTIONLIST pointer
 */
void imsp_free_optionlist (OPTIONLIST **list)
{
	if (*list) {
		/* only free if exists */
		if ((*list)->key.data) fs_give ((void **) &(*list)->key.data);
		if ((*list)->val.data) fs_give ((void **) &(*list)->val.data);
		imsp_free_optionlist (&(*list)->next);
		/* return string to free storage */
		fs_give ((void **) list);
	}
}

/* IMSP instantiate ABOOKLIST
 * Returns: new ABOOKLIST list
 */
ABOOKLIST *imsp_new_abooklist (void) {
	return (ABOOKLIST *) memset(fs_get(sizeof(ABOOKLIST)), 0, sizeof(ABOOKLIST));
}

/* IMSP garbage collect ABOOKLIST
 * Accepts: pointer to ABOOKLIST pointer
 */
void imsp_free_abooklist (ABOOKLIST **list)
{
	if (*list) {
		/* only free if exists */
		if ((*list)->name.data) fs_give ((void **) &(*list)->name.data);
		imsp_free_abooklist (&(*list)->next);
		/* return string to free storage */
		fs_give ((void **) list);
	}
}

/* IMSP instantiate ADDRESSLIST
 * Returns: new ADDRESSLIST list
 */
ADDRESSLIST *imsp_new_addresslist (void) {
	return (ADDRESSLIST *) memset(fs_get(sizeof(ADDRESSLIST)), 0, sizeof(ADDRESSLIST));
}

/* IMSP garbage collect ADDRESSLIST
 * Accepts: pointer to ADDRESSLIST pointer
 */
void imsp_free_addresslist (ADDRESSLIST **list)
{
	if (*list) {
		/* only free if exists */
		if ((*list)->name.data) fs_give ((void **) &(*list)->name.data);
		if ((*list)->kv) {
			kv_free((*list)->kv, (*list)->count);
		}
		imsp_free_addresslist (&(*list)->next);
		/* return string to free storage */
		fs_give ((void **) list);
	}
}

SUPPORTSTREAM *imsp_close_it (pimls *imsp_le_struct)
{
	support_done(imsp_le_struct->imsp_stream);
	efree(imsp_le_struct);
	return SUCCESS;
}

keyvalue *kv_copy (keyvalue *kv, int count) {
	int i;
	keyvalue *new;
	
	new = emalloc(sizeof(keyvalue) * count);
	for (i = 0; i < count; ++i) {
		new[i].key = estrdup(kv[i].key);
		new[i].value = estrdup(kv[i].value);
	}
	return new;
}

void kv_free (keyvalue *kv, int count) {
	int i;
	
	for (i = 0; i < count; ++i) {
		efree(kv[i].key);
		efree(kv[i].value);
	}
	efree(kv);
}

void imsp_info (void)
{
	php3_printf("IMSP Support enabled<br>");
}


int imsp_init (INIT_FUNC_ARGS)
{
	/* lets allow NIL */
	REGISTER_MAIN_LONG_CONSTANT("NIL", NIL, CONST_PERSISTENT | CONST_CS);
	
	le_imsp = register_list_destructors(imsp_close_it, NULL);
	return SUCCESS;
}

/* {{{ proto int imsp_open(string user, string password, string hostname)
   Open an IMSP stream */
void php3_imsp_open (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *hostname;
	pval *user;
	pval *passwd;
	SUPPORTSTREAM *imsp_stream = NIL;
	pimls *imsp_le_struct;

	long cl_flags = NIL;
	int ind;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &hostname, &user, &passwd) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string(hostname);
	convert_to_string(user);
	convert_to_string(passwd);
	strcpy(imsp_user, user->value.str.val);
	strcpy(imsp_password, passwd->value.str.val);
	
	imsp_stream = support_new();
	if (!imsp_open(imsp_stream, hostname->value.str.val)) {
		php3_error(E_WARNING, "Couldn't open IMSP stream %s\n", hostname->value.str.val);
		RETURN_FALSE;
	}
	
	imsp_le_struct = emalloc(sizeof(pimls));
	imsp_le_struct->imsp_stream = imsp_stream;
	imsp_le_struct->flags = cl_flags;	
	ind = php3_list_insert(imsp_le_struct, le_imsp);
	
	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int imsp_close(int stream_id)
   Close an IMSP stream */
void php3_imsp_close (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind;
	pimls *imsp_le_struct = NULL; 
	int ind, ind_type;
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &streamind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	php3_list_delete(ind);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array imsp_option_get(int stream_id, string pattern)
   Retrieve options matching the pattern from the IMSP server and returns an associative array mapping keys => values */
void php3_imsp_option_get (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *pattern;
	pimls *imsp_le_struct = NIL;
	int ind, ind_type;
	OPTIONLIST *cur = NIL;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &streamind, &pattern) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(pattern);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	imsp_options = NIL;
	option_get(imsp_le_struct->imsp_stream, pattern->value.str.val);
	if (imsp_options == NIL) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	cur = imsp_options;
	while (cur != NIL) {
		add_assoc_string(return_value, cur->key.data, cur->val.data, 1);
		cur = cur->next;
	}
	imsp_free_optionlist(&imsp_options);
}
/* }}} */

/* {{{ proto array imsp_option_set(int stream_id, string option, string value)
   Sets <option> to <value> */
void php3_imsp_option_set (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *option, *value;
	pimls *imsp_le_struct = NIL;
	int ind, ind_type;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &streamind, &option, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(option);
	convert_to_string(value);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (option_lock(imsp_le_struct->imsp_stream, option->value.str.val)) {
		if (option_set(imsp_le_struct->imsp_stream, option->value.str.val, value->value.str.val)) {
			option_unlock(imsp_le_struct->imsp_stream, option->value.str.val);
			RETURN_TRUE;
		} else {
			option_unlock(imsp_le_struct->imsp_stream, option->value.str.val);
			RETURN_FALSE;
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imsp_option_unset(int stream_id, string option)
   Unsets <option> */
void php3_imsp_option_unset (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *option;
	pimls *imsp_le_struct = NIL;
	int ind, ind_type;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &streamind, &option) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(option);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (option_lock(imsp_le_struct->imsp_stream, option->value.str.val)) {
		if (option_set(imsp_le_struct->imsp_stream, option->value.str.val, NULL)) {
			option_unlock(imsp_le_struct->imsp_stream, option->value.str.val);
			RETURN_TRUE;
		} else {
			option_unlock(imsp_le_struct->imsp_stream, option->value.str.val);
			RETURN_FALSE;
		}
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imsp_abook_create(int stream_id, string addressbook_name)
   Create an IMSP addressbook */
void php3_imsp_abook_create (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *abook_name;
	pimls *imsp_le_struct = NIL;
	int ind, ind_type;
	long status;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &streamind, &abook_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(abook_name);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	status = abook_create(imsp_le_struct->imsp_stream, abook_name->value.str.val);
	if (status != NIL) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imsp_abook_delete(int stream_id, string addressbook_name)
   Delete an IMSP addressbook */
void php3_imsp_abook_delete (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *abook_name;
	pimls *imsp_le_struct = NIL;
	int ind, ind_type;
	long status;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &streamind, &abook_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(abook_name);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	status = abook_delete(imsp_le_struct->imsp_stream, abook_name->value.str.val);
	if (status != NIL) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imsp_abook_find(int stream_id, string pattern)
   List all accessible IMSP addressbooks matching "pattern" */
void php3_imsp_abook_find (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *pattern, abookob;
	pimls *imsp_le_struct = NIL;
	int ind, ind_type;
	long status;
	ABOOKLIST *cur = NIL;
	char *sep = NIL;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &streamind, &pattern) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(pattern);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	imsp_abooks = NIL;
	status = abook_find(imsp_le_struct->imsp_stream, pattern->value.str.val);
	if (status == NIL) {
		RETURN_FALSE;
	}
	
	if (imsp_abooks == NIL) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	sep = emalloc(2 * sizeof(char));
	cur = imsp_abooks;
	while (cur != NIL ) {
		sep[0] = cur->seperator;
		sep[1] = 0;
		object_init(&abookob);
		add_property_string(&abookob, "name", cur->name.data, 1);
		add_property_string(&abookob, "seperator", sep, 1);
		add_next_index_object(return_value, abookob);
		cur = cur->next;
	}
	efree(sep);
	imsp_free_abooklist(&imsp_abooks);
}
/* }}} */

/* {{{ proto int imsp_abook_rename(int stream_id, string old_name, string new_name)
   Rename an IMSP addressbook */
void php3_imsp_abook_rename (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *old_name, *new_name;
	pimls *imsp_le_struct = NIL;
	int ind, ind_type;
	long status;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &streamind, &old_name, &new_name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(old_name);
	convert_to_string(new_name);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	status = abook_rename(imsp_le_struct->imsp_stream, old_name->value.str.val, new_name->value.str.val);
	if (status != NIL) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imsp_abook_getlist(int stream_id, string addressbook)
   List all addresses in an IMSP addressbook */
void php3_imsp_abook_getlist (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *addressbook;
	int ind, ind_type;
	long status;
	ADDRESSLIST *cur = NIL;
	pimls *imsp_le_struct = NIL;
	abook *ab;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &streamind, &addressbook) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(addressbook);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (!(ab = abook_open(imsp_le_struct->imsp_stream, addressbook->value.str.val, NIL))) {
		php3_error(E_WARNING, "Couldn't open ADDRESSBOOK %s\n", addressbook->value.str.val);
		RETURN_FALSE;
	}
	
	imsp_addresses = NIL;
	status = abook_getlist(ab);
	abook_close(ab);
	if (status == NIL) {
		imsp_free_addresslist(&imsp_addresses);
		RETURN_FALSE;
	}
	
	if (imsp_addresses == NIL) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	cur = imsp_addresses;
	while (cur != NIL ) {
		add_next_index_string(return_value, cur->name.data, 1);
		cur = cur->next;
	}
	imsp_free_addresslist(&imsp_addresses);
}
/* }}} */

/* {{{ proto int imsp_abook_search(int stream_id, string addressbook, string pattern, array criteria)
   List all addresses in an IMSP addressbook */
void php3_imsp_abook_search (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *addressbook, *pattern, *criteria, *tmp;
	pimls *imsp_le_struct = NIL;
	abook *ab = NIL;
	int ind, ind_type, num_criteria, i;
	long status;
	ADDRESSLIST *cur = NIL;
	Bucket *p;
	keyvalue *kv;
	
	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &streamind, &addressbook, &pattern, &criteria) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (!(criteria->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in imsp_abook_search() call");
		return;
	}
	
	convert_to_long(streamind);
	convert_to_string(addressbook);
	convert_to_string(pattern);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (!(ab = abook_open(imsp_le_struct->imsp_stream, addressbook->value.str.val, NIL))) {
		php3_error(E_WARNING, "Couldn't open ADDRESSBOOK %s\n", addressbook->value.str.val);
		RETURN_FALSE;
	}
	
	/* first loop through and find the number of elements in the criteria array */
	num_criteria = 0;
	for (p = criteria->value.ht->pListHead, num_criteria = 0; p; p = p->pListNext) {
		tmp = (pval *) p->pData;
		if (tmp->type == IS_STRING && tmp->value.str.val == undefined_variable_string) {
			continue;
		}
		num_criteria++;
	}
	
	/* now loop through once more now that we can allocate the keyvalue */
	kv = emalloc(sizeof(keyvalue) * num_criteria);
	i = 0;
	for (p = criteria->value.ht->pListHead; i < num_criteria; p = p->pListNext, i++) {
		tmp = (pval *) p->pData;
		if (tmp->type == IS_STRING && tmp->value.str.val == undefined_variable_string) {
			continue;
		}
		kv[i].key = estrdup(p->arKey);
		kv[i].value = estrdup(tmp->value.str.val);
	}
	
	imsp_addresses = NIL;
	status = abook_search(ab, pattern->value.str.val, kv, num_criteria);
	abook_close(ab);
	kv_free(kv, num_criteria);
	if (status == NIL) {
		imsp_free_addresslist(&imsp_addresses);
		RETURN_FALSE;
	}
	
	if (imsp_addresses == NIL) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	cur = imsp_addresses;
	while (cur != NIL ) {
		add_next_index_string(return_value, cur->name.data, 1);
		cur = cur->next;
	}
	imsp_free_addresslist(&imsp_addresses);
}
/* }}} */

/* {{{ proto int imsp_abook_fetch(int stream_id, string addressbook, string name)
   Fetch the information for "name" from the specified IMSP addressbook */
void php3_imsp_abook_fetch (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *addressbook, *name;
	pimls *imsp_le_struct = NIL;
	abook *ab;
	int ind, ind_type, i;
	long status;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &streamind, &addressbook, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(addressbook);
	convert_to_string(name);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (!(ab = abook_open(imsp_le_struct->imsp_stream, addressbook->value.str.val, NIL))) {
		php3_error(E_WARNING, "Couldn't open ADDRESSBOOK %s\n", addressbook->value.str.val);
		RETURN_FALSE;
	}
	
	imsp_addresses = NIL;
	status = abook_fetch(ab, name->value.str.val);
	abook_close(ab);
	if (status == NIL) {
		imsp_free_addresslist(&imsp_addresses);
		RETURN_FALSE;
	}
	
	if (imsp_addresses == NIL) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	for (i = 0; i < imsp_addresses->count; i++) {
		add_assoc_string(return_value, imsp_addresses->kv[i].key, imsp_addresses->kv[i].value, 1);
	}
	imsp_free_addresslist(&imsp_addresses);
}
/* }}} */

/* {{{ proto int imsp_abook_lock(int stream_id, string addressbook, string name)
   Lock the specified IMSP addressbook entry */
void php3_imsp_abook_lock (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *addressbook, *name;
	pimls *imsp_le_struct = NIL;
	abook *ab;
	int ind, ind_type;
	long status;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &streamind, &addressbook, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(addressbook);
	convert_to_string(name);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (!(ab = abook_open(imsp_le_struct->imsp_stream, addressbook->value.str.val, NIL))) {
		php3_error(E_WARNING, "Couldn't open ADDRESSBOOK %s\n", addressbook->value.str.val);
		RETURN_FALSE;
	}
	
	status = abook_lock(ab, name->value.str.val);
	abook_close(ab);
	if (status == NIL) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imsp_abook_unlock(int stream_id, string addressbook, string name)
   Unlock an address in an IMSP addressbook */
void php3_imsp_abook_unlock (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *addressbook, *name;
	pimls *imsp_le_struct = NIL;
	abook *ab;
	int ind, ind_type;
	long status;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &streamind, &addressbook, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(addressbook);
	convert_to_string(name);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (!(ab = abook_open(imsp_le_struct->imsp_stream, addressbook->value.str.val, NIL))) {
		php3_error(E_WARNING, "Couldn't open ADDRESSBOOK %s\n", addressbook->value.str.val);
		RETURN_FALSE;
	}
	
	abook_unlock(ab, name->value.str.val);
	abook_close(ab);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imsp_abook_store(int stream_id, string abook, string name, array field_data)
   Stores the address "name" with fields specified by the field_data array in the specified IMSP addressbook */
void php3_imsp_abook_store (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *addressbook, *name, *field_data, *tmp;
	pimls *imsp_le_struct = NIL;
	abook *ab;
	int ind, ind_type, num_fields, i;
	long status;
	Bucket *p;
	keyvalue *fields;
	
	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &streamind, &addressbook, &name, &field_data) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	if (!(field_data->type & IS_HASH)) {
		php3_error(E_WARNING, "Wrong datatype in imsp_abook_store() call");
		return;
	}
	
	convert_to_long(streamind);
	convert_to_string(addressbook);
	convert_to_string(name);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (!(ab = abook_open(imsp_le_struct->imsp_stream, addressbook->value.str.val, NIL))) {
		php3_error(E_WARNING, "Couldn't open ADDRESSBOOK %s\n", addressbook->value.str.val);
		RETURN_FALSE;
	}
	
	/* first loop through and find the number of elements in the criteria array */
	num_fields = 0;
	for (p = field_data->value.ht->pListHead, num_fields = 0; p; p = p->pListNext) {
		tmp = (pval *)p->pData;
		if (tmp->type == IS_STRING && tmp->value.str.val == undefined_variable_string) {
			continue;
		}
		num_fields++;
	}
	
	/* now loop through once more now that we can allocate the keyvalue */
	fields = emalloc(sizeof(keyvalue) * num_fields);
	i = 0;
	for (p = field_data->value.ht->pListHead; i < num_fields; p = p->pListNext, i++) {
		tmp = (pval *)p->pData;
		if (tmp->type == IS_STRING && tmp->value.str.val == undefined_variable_string) {
			continue;
		}
		fields[i].key = estrdup(p->arKey);
		fields[i].value = estrdup(tmp->value.str.val);
	}
	
	status = abook_store(ab, name->value.str.val, fields, num_fields);
	abook_close(ab);
	kv_free(fields, num_fields);
	if (status == NIL) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imsp_abook_delete_entry(int stream_id, string addressbook, string name)
   Delete specified IMSP addressbook entry */
void php3_imsp_abook_deleteent (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *addressbook, *name;
	pimls *imsp_le_struct = NIL;
	abook *ab;
	int ind, ind_type;
	long status;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &streamind, &addressbook, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(addressbook);
	convert_to_string(name);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (!(ab = abook_open(imsp_le_struct->imsp_stream, addressbook->value.str.val, NIL))) {
		php3_error(E_WARNING, "Couldn't open ADDRESSBOOK %s\n", addressbook->value.str.val);
		RETURN_FALSE;
	}
	
	status = abook_deleteent(ab, name->value.str.val);
	abook_close(ab);
	if (status == NIL) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto int imsp_abook_expand(int stream_id, string addressbook, string name)
   Finds the definitive list of email addresses for an IMSP addressbook entry */
void php3_imsp_abook_expand (INTERNAL_FUNCTION_PARAMETERS)
{
	pval *streamind, *addressbook, *name;
	pimls *imsp_le_struct = NIL;
	abook *ab;
	int ind, ind_type;
	char *expanded;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &streamind, &addressbook, &name) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(streamind);
	convert_to_string(addressbook);
	convert_to_string(name);
	
	ind = streamind->value.lval;
	imsp_le_struct = (pimls *)php3_list_find(ind, &ind_type);
	if (!imsp_le_struct || !IMSP_IS_STREAM(ind_type)) {
		php3_error(E_WARNING, "Unable to find IMSP stream pointer");
		RETURN_FALSE;
	}
	
	if (!(ab = abook_open(imsp_le_struct->imsp_stream, addressbook->value.str.val, NIL))) {
		php3_error(E_WARNING, "Couldn't open ADDRESSBOOK %s\n", addressbook->value.str.val);
		RETURN_FALSE;
	}
	
	expanded = abook_expand(ab, name->value.str.val);
	abook_close(ab);
	RETURN_STRING(expanded, 1);
}
/* }}} */

/* IMSP Interfaces */
void mm_mailbox (char *string)
{
}

void mm_bboard (char *string)
{
}

void mm_addressbook(SUPPORTSTREAM *stream, char *abook_name, char sep_char)
{
	ABOOKLIST *cur = NIL;
	
	if (imsp_abooks == NIL) {
		imsp_abooks = imsp_new_abooklist();
		imsp_abooks->name.size = strlen(imsp_abooks->name.data = cpystr(abook_name));
		imsp_abooks->seperator = sep_char;
		imsp_abooks->next = NIL;
	} else {
		cur = imsp_abooks;
		while (cur->next != NIL) {
			cur = cur->next;
		}
		cur->next = imsp_new_abooklist();
		cur = cur->next;
		cur->name.size = strlen(cur->name.data = cpystr(abook_name));
		cur->seperator = sep_char;
		cur->next = NIL;
	}
}

void mm_address(abook *ab, char *address_name, keyvalue *kv, int count)
{
	ADDRESSLIST *cur = NIL;
	
	if (imsp_addresses == NIL) {
		imsp_addresses = imsp_new_addresslist();
		imsp_addresses->name.size = strlen(imsp_addresses->name.data = cpystr(address_name));
		imsp_addresses->kv = kv_copy(kv, count);
		imsp_addresses->count = count;
		imsp_addresses->next = NIL;
	} else {
		cur = imsp_addresses;
		while (cur->next != NIL) {
			cur = cur->next;
		}
		cur->next = imsp_new_addresslist();
		cur = cur->next;
		cur->name.size = strlen(cur->name.data = cpystr(address_name));
		cur->kv = kv_copy(kv, count);
		cur->count = count;
		cur->next = NIL;
	}
}

void mm_abookacl (abook *ab, char *identifier, char *rights)
{
    if (identifier) {
		php3_printf("ACL: %s %s\n", identifier, rights);
    } else {
		php3_printf("MYRIGHTS: %s\n", rights);
    }
}

void mm_option (SUPPORTSTREAM *s, char *option, char *value, int read_write_flag)
{
	OPTIONLIST *cur = NIL;
	
	if (imsp_options == NIL) {
		imsp_options = imsp_new_optionlist();
		imsp_options->key.size = strlen(imsp_options->key.data = cpystr(option));
		imsp_options->val.size = strlen(imsp_options->val.data = cpystr(value));
		imsp_options->next = NIL;
	} else {
		cur = imsp_options;
		while (cur->next != NIL) {
			cur = cur->next;
		}
		cur->next = imsp_new_optionlist();
		cur = cur->next;
		cur->key.size = strlen(cur->key.data = cpystr(option));
		cur->val.size = strlen(cur->val.data = cpystr(value));
		cur->next = NIL;
	}
}

/* Interfaces to C-client */
/* Only include these if we don't have IMAP support. Otherwise the IMAP module will cover them. */
#ifndef HAVE_IMAP

/* CJH: check the service name in order to allow different IMSP
   usernames & passwords */
void mm_login (NETMBX *mb,char *user,char *pwd,long trial)
{
	if (*mb->service && strcmp(mb->service, "imsp") == 0) {
		if (*mb->user) {
			strcpy(user, mb->user);
		} else {
			strcpy(user, imsp_user);
		}
		strcpy (pwd,imsp_password);
	} else {
		if (*mb->user) {
			strcpy(user, mb->user);
		} else {
			strcpy(user, imap_user);
		}
		strcpy(pwd, imap_password);
	}
}

void mm_dlog (char *string)
{
}

void mm_searched (MAILSTREAM *stream, unsigned long number)
{
}

void mm_exists (MAILSTREAM *stream, unsigned long number)
{
}

void mm_expunged (MAILSTREAM *stream, unsigned long number)
{
}

void mm_flags (MAILSTREAM *stream, unsigned long number)
{
}

void mm_list (MAILSTREAM *stream, int delimiter, char *name, long attributes)
{
}

void mm_lsub (MAILSTREAM *stream, int delimiter, char *name, long attributes)
{
}

void mm_fatal (char *string)
{
}

void mm_critical (MAILSTREAM *stream)
{
}

void mm_log (char *string, long errflg)
{
	switch ((short) errflg) {
	case NIL:
		/* php3_error(E_NOTICE, "%s", string); */
		break;
	case PARSE:
	case WARN:
		php3_error(E_WARN, "%s", string);
		break;
	case ERROR:
		php3_error(E_ERROR, "%s", string);
		break;
	}
}

void mm_notify (MAILSTREAM *stream, char *string, long errflg)
{
	mm_log(string, errflg);
}

void mm_nocritical (MAILSTREAM *stream)
{
}

long mm_diskerror (MAILSTREAM *stream,long errcode,long serious) {
	return 1;
}

void mm_status (MAILSTREAM *stream, char* mailbox, MAILSTATUS *status)
{
}
#endif /* HAVE_IMAP */

#endif /* HAVE_IMSP */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
