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


/* $Id: php3_hash.h,v 1.11 2000/04/10 19:29:36 andi Exp $ */


#ifndef _HASH_
#define _HASH_

#include <sys/types.h>

#define HASH_KEY_IS_STRING 1
#define HASH_KEY_IS_LONG 2
#define HASH_KEY_NON_EXISTANT 3

#define HASH_UPDATE 0
#define HASH_ADD 1
#define HASH_NEXT_INSERT 2

#define HASH_DEL_KEY 0
#define HASH_DEL_INDEX 1

struct hashtable;

typedef struct bucket {
	ulong h;						/* Used for numeric indexing */
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
	ulong nNextFreeElement;
	ulong(*pHashFunction) (char *arKey, uint nKeyLength);
	Bucket *pInternalPointer;	/* Used for element traversal */
	Bucket *pListHead;
	Bucket *pListTail;
	Bucket **arBuckets;
	void (*pDestructor) (void *pData);
	unsigned char persistent;
#if DEBUG
	unsigned char indestroy;
#endif
} HashTable;


/* startup/shutdown */
extern PHPAPI int _php3_hash_init(HashTable *ht, uint nSize, ulong(*pHashFunction) (char *arKey, uint nKeyLength), void (*pDestructor) (void *pData), int persistent);
extern PHPAPI void _php3_hash_destroy(HashTable *ht);

/* additions/updates/changes */
extern PHPAPI int _php3_hash_add_or_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest,int flag);
#define _php3_hash_update(ht,arKey,nKeyLength,pData,nDataSize,pDest) \
		_php3_hash_add_or_update(ht,arKey,nKeyLength,pData,nDataSize,pDest,HASH_UPDATE)
#define _php3_hash_add(ht,arKey,nKeyLength,pData,nDataSize,pDest) \
		_php3_hash_add_or_update(ht,arKey,nKeyLength,pData,nDataSize,pDest,HASH_ADD)

extern PHPAPI int _php3_hash_index_update_or_next_insert(HashTable *ht, ulong h, void *pData, uint nDataSize, void **pDest, int flag);
#define _php3_hash_index_update(ht,h,pData,nDataSize,pDest) \
		_php3_hash_index_update_or_next_insert(ht,h,pData,nDataSize,pDest,HASH_UPDATE)
#define _php3_hash_next_index_insert(ht,pData,nDataSize,pDest) \
		_php3_hash_index_update_or_next_insert(ht,0,pData,nDataSize,pDest,HASH_NEXT_INSERT)

extern PHPAPI int _php3_hash_pointer_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData);

extern PHPAPI int _php3_hash_pointer_index_update_or_next_insert(HashTable *ht, ulong h, void *pData, int flag);
#define _php3_hash_pointer_index_update(ht,h,pData) \
		_php3_hash_pointer_index_update_or_next_insert(ht,h,pData,HASH_UPDATE)
#define _php3_hash_next_index_pointer_insert(ht,pData) \
        _php3_hash_pointer_index_update_or_next_insert(ht,0,pData,HASH_NEXT_INSERT)
extern PHPAPI void _php3_hash_apply(HashTable *ht,int (*destruct) (void *));
extern PHPAPI void _php3_hash_apply_with_argument(HashTable *ht,int (*destruct) (void *, void *), void *);



/* Deletes */
extern PHPAPI int _php3_hash_del_key_or_index(HashTable *ht, char *arKey, uint nKeyLength, ulong h, int flag);
#define _php3_hash_del(ht,arKey,nKeyLength) \
		_php3_hash_del_key_or_index(ht,arKey,nKeyLength,0,HASH_DEL_KEY)
#define _php3_hash_index_del(ht,h) \
		_php3_hash_del_key_or_index(ht,NULL,0,h,HASH_DEL_INDEX)

/* Data retreival */
extern PHPAPI int _php3_hash_find(HashTable *ht, char *arKey, uint nKeyLength, void **pData);
extern PHPAPI int _php3_hash_index_find(HashTable *ht, ulong h, void **pData);

/* Misc */
extern PHPAPI int _php3_hash_exists(HashTable *ht, char *arKey, uint nKeyLength);
extern PHPAPI int _php3_hash_index_exists(HashTable *ht, ulong h);
extern PHPAPI int _php3_hash_is_pointer(HashTable *ht, char *arKey, uint nKeyLength);
extern PHPAPI int _php3_hash_index_is_pointer(HashTable *ht, ulong h);
extern PHPAPI ulong _php3_hash_next_free_element(HashTable *ht);

/* traversing */
extern PHPAPI void _php3_hash_move_forward(HashTable *ht);
extern PHPAPI void _php3_hash_move_backwards(HashTable *ht);
extern PHPAPI int _php3_hash_get_current_key(HashTable *ht, char **str_index, ulong *num_index);
extern PHPAPI int _php3_hash_get_current_data(HashTable *ht, void **pData);
extern PHPAPI void _php3_hash_internal_pointer_reset(HashTable *ht);
extern PHPAPI void _php3_hash_internal_pointer_end(HashTable *ht);

/* internal functions */
extern int if_full_do_resize(HashTable *ht);
extern int _php3_hash_rehash(HashTable *ht);

/* Copying, merging and sorting */
extern PHPAPI void _php3_hash_copy(HashTable **target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size);
extern PHPAPI void _php3_hash_merge(HashTable *target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size);
extern PHPAPI int _php3_hash_sort(HashTable *ht, int (*compar) (const void *, const void *), int renumber);
extern PHPAPI int _php3_hash_minmax(HashTable *ht, int (*compar) (const void *, const void *), int flag, void **pData);

extern PHPAPI int _php3_hash_num_elements(HashTable *ht);


#if DEBUG
/* debug */
extern PHPAPI void _php3_hash_display_pListTail(HashTable *ht);
extern PHPAPI void _php3_hash_display(HashTable *ht);
#endif

#endif							/* _HASH_ */
