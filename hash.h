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


/* $Id: hash.h,v 1.39 1998/01/17 21:23:27 andi Exp $ */


#ifndef _HASH_
#define _HASH_

#include <sys/types.h>

#define HASH_KEY_IS_STRING 1
#define HASH_KEY_IS_INT 2
#define HASH_KEY_NON_EXISTANT 3

#define HASH_UPDATE 0
#define HASH_ADD 1

#define HASH_DEL_KEY 0
#define HASH_DEL_INDEX 1

#define HASH_INDEX_POINTER_UPDATE 0
#define HASH_NEXT_INDEX_POINTER_INSERT 1

#define HASH_INDEX_UPDATE 0
#define HASH_NEXT_INDEX_INSERT 1

typedef struct bucket {
	uint h;						/* Used for numeric indexing */
	char *arKey;
	uint nKeyLength;
	void *pData;
	char bIsPointer;
	struct bucket *pListNext;
	struct bucket *pListLast;
	struct bucket *pNext;
} Bucket;

typedef struct hashtable {
	uint nTableSize;
	uint nHashSizeIndex;
	uint nNumOfElements;
	uint nNextFreeElement;
	 uint(*pHashFunction) (char *arKey, uint nKeyLength);
	Bucket *pInternalPointer;	/* Used for element traversal */
	Bucket *pListHead;
	Bucket *pListTail;
	Bucket **arBuckets;
	void (*pDestructor) (void *pData);
	unsigned char persistent;
} HashTable;


/* startup/shutdown */
extern PHPAPI int hash_init(HashTable *ht, uint nSize, uint(*pHashFunction) (char *arKey, uint nKeyLength), void (*pDestructor) (void *pData), int persistent);
extern PHPAPI void hash_destroy(HashTable *ht);

/* additions/updates/changes */
extern PHPAPI int hash_add_or_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest,int flag);
#define hash_update(ht,arKey,nKeyLength,pData,nDataSize,pDest) \
		hash_add_or_update(ht,arKey,nKeyLength,pData,nDataSize,pDest,HASH_UPDATE)
#define hash_add(ht,arKey,nKeyLength,pData,nDataSize,pDest) \
		hash_add_or_update(ht,arKey,nKeyLength,pData,nDataSize,pDest,HASH_ADD)

extern PHPAPI int hash_index_update_or_next_insert(HashTable *ht, uint h, void *pData, uint nDataSize, void **pDest, int flag);
#define hash_index_update(ht,h,pData,nDataSize,pDest) \
		hash_index_update_or_next_insert(ht,h,pData,nDataSize,pDest,HASH_INDEX_UPDATE)
#define hash_next_index_insert(ht,pData,nDataSize,pDest) \
		hash_index_update_or_next_insert(ht,0,pData,nDataSize,pDest,HASH_NEXT_INDEX_INSERT)

extern PHPAPI int hash_pointer_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData);

extern PHPAPI int hash_pointer_index_update_or_next_insert(HashTable *ht, uint h, void *pData, int flag);
#define hash_pointer_index_update(ht,h,pData) \
		hash_pointer_index_update_or_next_insert(ht,h,pData,HASH_INDEX_POINTER_UPDATE)
#define hash_next_index_pointer_insert(ht,pData) \
        hash_pointer_index_update_or_next_insert(ht,0,pData,HASH_NEXT_INDEX_POINTER_INSERT)
extern PHPAPI void hash_apply(HashTable *ht,int (*destruct) (void *));


/* Deletes */
extern PHPAPI int hash_del_key_or_index(HashTable *ht, char *arKey, uint nKeyLength, uint h, int flag);
#define hash_del(ht,arKey,nKeyLength) \
		hash_del_key_or_index(ht,arKey,nKeyLength,0,HASH_DEL_KEY)
#define hash_index_del(ht,h) \
		hash_del_key_or_index(ht,NULL,0,h,HASH_DEL_INDEX)

/* Data retreival */
extern PHPAPI int hash_find(HashTable *ht, char *arKey, uint nKeyLength, void **pData);
extern PHPAPI int hash_index_find(HashTable *ht, uint h, void **pData);

/* Misc */
extern PHPAPI int hash_exists(HashTable *ht, char *arKey, uint nKeyLength);
extern PHPAPI int hash_index_exists(HashTable *ht, uint h);
extern int hash_is_pointer(HashTable *ht, char *arKey, uint nKeyLength);
extern int hash_index_is_pointer(HashTable *ht, uint h);
extern PHPAPI uint hash_next_free_element(HashTable *ht);

/* traversing */
extern void hash_move_forward(HashTable *ht);
extern void hash_move_backwards(HashTable *ht);
extern int hash_get_current_key(HashTable *ht, char **str_index, int *int_index);
extern int hash_get_current_data(HashTable *ht, void **pData);
extern void hash_internal_pointer_reset(HashTable *ht);
extern void hash_internal_pointer_end(HashTable *ht);

/* internal functions */
extern int if_full_do_resize(HashTable *ht);
extern int hash_rehash(HashTable *ht);

/* Copying, merging and sorting */
extern void hash_copy(HashTable **target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size);
extern void hash_merge(HashTable *target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size);
extern int hash_sort(HashTable *ht, int (*compar) (const void *, const void *), int renumber);

extern PHPAPI int hash_num_elements(HashTable *ht);


#if DEBUG
/* debug */
extern void hash_display_pListTail(HashTable *ht);
extern void hash_display(HashTable *ht);
#endif

#endif							/* _HASH_ */
