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


/* $Id: hash.c,v 1.75 1998/02/16 08:03:34 andi Exp $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

/* Generated on an Octa-ALPHA 300MHz CPU & 2.5GB RAM monster */
uint PrimeNumbers[] =
{5, 11, 19, 53, 107, 223, 463, 983, 1979, 3907, 7963, 16229, 32531, 65407, 130987, 262237, 524521, 1048793, 2097397, 4194103, 8388857, 16777447, 33554201, 67108961, 134217487, 268435697, 536870683, 1073741621, 2147483399};

uint nNumPrimeNumbers = sizeof(PrimeNumbers) / sizeof(uint);

static uint hashpjw(char *arKey, uint nKeyLength)
{
	uint h = 0, g;
	char *arEnd=arKey+nKeyLength;

	while (arKey < arEnd) {
		h = (h << 4) + *arKey++;
		if ((g = (h & 0xF0000000))) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h;
}

PHPAPI int hash_init(HashTable *ht, uint nSize, uint(*pHashFunction) (char *arKey, uint nKeyLength), void (*pDestructor) (void *pData),int persistent)
{
	uint i;

	for (i = 0; i < nNumPrimeNumbers; i++) {
		if (nSize < PrimeNumbers[i]) {
			nSize = PrimeNumbers[i];
			ht->nHashSizeIndex = i;
			break;
		}
	}
	if (i == nNumPrimeNumbers) {	/* This shouldn't really happen unless the ask for a ridiculous size */
		nSize = PrimeNumbers[i - 1];
		ht->nHashSizeIndex = i - 1;
	}
	
	/* Uses ecalloc() so that Bucket* == NULL */
	ht->arBuckets = (Bucket **) pecalloc(nSize, sizeof(Bucket *), persistent);
	
	if (!ht->arBuckets) {
		return FAILURE;
	}
	if (pHashFunction == NULL) {
		ht->pHashFunction = hashpjw;
	} else {
		ht->pHashFunction = pHashFunction;
	}
	ht->pDestructor = pDestructor;
	ht->nTableSize = nSize;
	ht->pListHead = NULL;
	ht->pListTail = NULL;
	ht->nNumOfElements = 0;
	ht->nNextFreeElement = 0;
	ht->pInternalPointer = NULL;
	ht->persistent = persistent;
	return SUCCESS;
}

PHPAPI int hash_add_or_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData, uint nDataSize, void **pDest, int flag)
{
	uint h, nIndex;
	Bucket *p;
#if DEBUG
	TLS_VARS;
#endif

	h = ht->pHashFunction(arKey, nKeyLength);
	nIndex = h % ht->nTableSize;

	if (nKeyLength <= 0) {
#if DEBUG
		PUTS("hash_update: Can't put in empty key\n");
#endif
		return FAILURE;
	}
	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->arKey != NULL) && (p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				if (flag == HASH_ADD) {
					return FAILURE;
				}
				BLOCK_INTERRUPTIONS;
#if DEBUG
				if (p->pData == pData) {
					PUTS("Fatal error in hash_update: p->pData == pData\n");
					UNBLOCK_INTERRUPTIONS;
					return FAILURE;
				}
#endif
				if (ht->pDestructor) {
					ht->pDestructor(p->pData);
				}
				memcpy(p->pData, pData, nDataSize);
				if (pDest) {
					*pDest = p->pData;
				}
				UNBLOCK_INTERRUPTIONS;
				return SUCCESS;
			}
		}
		p = p->pNext;
	}
	
	p = (Bucket *) pemalloc(sizeof(Bucket),ht->persistent);

	if (!p) {
		return FAILURE;
	}
	p->arKey = (char *) pemalloc(nKeyLength,ht->persistent);
	if (!p->arKey) {
		pefree(p,ht->persistent);
		return FAILURE;
	}
	p->pData = (void *) pemalloc(nDataSize,ht->persistent);
	if (!p->pData) {
		pefree(p,ht->persistent);
		pefree(p->arKey,ht->persistent);
		return FAILURE;
	}
	p->nKeyLength = nKeyLength;
	p->h = h;
	memcpy(p->arKey, arKey, nKeyLength);
	memcpy(p->pData, pData, nDataSize);
	p->bIsPointer = 0;
	p->pNext = ht->arBuckets[nIndex];
	if (pDest) {
		*pDest = p->pData;
	}

	BLOCK_INTERRUPTIONS;
	if (ht->pInternalPointer == NULL) {
		ht->pInternalPointer = p;
	}
	ht->arBuckets[nIndex] = p;

	/* Setup the double linked list */
	p->pListLast = ht->pListTail;
	ht->pListTail = p;
	p->pListNext = NULL;
	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p;
	}
	if (!ht->pListHead) {
		ht->pListHead = p;
	}
	UNBLOCK_INTERRUPTIONS;
	ht->nNumOfElements++;
	if_full_do_resize(ht);		/* If the Hash table is full, resize it */
	return SUCCESS;
}


int hash_index_update_or_next_insert(HashTable *ht, uint h, void *pData, uint nDataSize, void **pDest, int flag)
{
	uint nIndex;
	Bucket *p;
#if DEBUG
	TLS_VARS;
#endif

	if (flag == HASH_NEXT_INDEX_INSERT) {
		h = ht->nNextFreeElement;
	}
	nIndex = h % ht->nTableSize;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->arKey == NULL) && (p->h == h)) {
			if (flag == HASH_NEXT_INDEX_INSERT) {
				return FAILURE;
			}
			BLOCK_INTERRUPTIONS;
#if DEBUG
			if (p->pData == pData) {
				PUTS("Fatal error in hash_index_update: p->pData == pData\n");
				UNBLOCK_INTERRUPTIONS;
				return FAILURE;
			}
#endif
			if (ht->pDestructor) {
				ht->pDestructor(p->pData);
			}
			memcpy(p->pData, pData, nDataSize);
			UNBLOCK_INTERRUPTIONS;
			if (h >= ht->nNextFreeElement) {
				ht->nNextFreeElement = h + 1;
			}
			if (pDest) {
				*pDest = p->pData;
			}
			return SUCCESS;
		}
		p = p->pNext;
	}
	p = (Bucket *) pemalloc(sizeof(Bucket),ht->persistent);
	if (!p) {
		return FAILURE;
	}
	p->arKey = NULL;			/* Numeric indices are marked by making the arKey == NULL */
	p->nKeyLength = 0;
	p->h = h;
	p->pData = (void *) pemalloc(nDataSize,ht->persistent);
	if (!p->pData) {
		pefree(p,ht->persistent);
		return FAILURE;
	}
	memcpy(p->pData, pData, nDataSize);
	p->bIsPointer = 0;
	if (pDest) {
		*pDest = p->pData;
	}

	p->pNext = ht->arBuckets[nIndex];
	BLOCK_INTERRUPTIONS;
	if (ht->pInternalPointer == NULL) {
		ht->pInternalPointer = p;
	}
	ht->arBuckets[nIndex] = p;

	/* Setup the double linked list */
	p->pListLast = ht->pListTail;
	ht->pListTail = p;
	p->pListNext = NULL;
	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p;
	}
	if (!ht->pListHead) {
		ht->pListHead = p;
	}
	UNBLOCK_INTERRUPTIONS;
	if (h >= ht->nNextFreeElement) {
		ht->nNextFreeElement = h + 1;
	}
	ht->nNumOfElements++;
	if_full_do_resize(ht);
	return SUCCESS;
}

int hash_pointer_update(HashTable *ht, char *arKey, uint nKeyLength, void *pData)
{
	uint h, nIndex;
	Bucket *p;
#if DEBUG
	TLS_VARS;
#endif

	h = ht->pHashFunction(arKey, nKeyLength);
	nIndex = h % ht->nTableSize;

	if (nKeyLength <= 0) {
#if DEBUG
		PUTS("hash_update: Can't put in empty key\n");
#endif
		return FAILURE;
	}
	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->arKey != NULL) && (p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
#if DEBUG
				if (p->pData == pData) {
					PUTS("Fatal error in hash_pointer_update: p->pData == pData\n");
					return FAILURE;
				}
#endif
				BLOCK_INTERRUPTIONS;
				if (!p->bIsPointer && ht->pDestructor) {
					ht->pDestructor(p->pData);
					pefree(p->pData,ht->persistent);
				}			
				p->pData = pData;
				p->bIsPointer = 1;
				UNBLOCK_INTERRUPTIONS;
				return SUCCESS;
			}
		}
		p = p->pNext;
	}
	p = (Bucket *) pemalloc(sizeof(Bucket),ht->persistent);
	if (!p) {
		return FAILURE;
	}
	p->arKey = (char *) pemalloc(nKeyLength,ht->persistent);
	if (!p->arKey) {
		pefree(p,ht->persistent);
		return FAILURE;
	}
	p->nKeyLength = nKeyLength;
	p->pData = pData;
	p->h = h;
	p->bIsPointer = 1;
	memcpy(p->arKey, arKey, nKeyLength);

	p->pNext = ht->arBuckets[nIndex];

	BLOCK_INTERRUPTIONS;
	if (ht->pInternalPointer == NULL) {
		ht->pInternalPointer = p;
	}
	ht->arBuckets[nIndex] = p;

	/* Setup the double linked list */
	p->pListLast = ht->pListTail;
	ht->pListTail = p;
	p->pListNext = NULL;
	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p;
	}
	if (!ht->pListHead) {
		ht->pListHead = p;
	}
	UNBLOCK_INTERRUPTIONS;

	ht->nNumOfElements++;
	if_full_do_resize(ht);		/* If the Hash table is full, resize it */
	return SUCCESS;
}


int hash_pointer_index_update_or_next_insert(HashTable *ht, uint h, void *pData, int flag)
{
	uint nIndex;
	Bucket *p;
#if DEBUG
	TLS_VARS;
#endif

	if (flag == HASH_NEXT_INDEX_POINTER_INSERT) {
		h = ht->nNextFreeElement;
	}
	nIndex = h % ht->nTableSize;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->arKey == NULL) && (p->h == h)) {
			if (flag == HASH_NEXT_INDEX_POINTER_INSERT) {
				return FAILURE;
			}
#if DEBUG
			if (p->pData == pData) {
				PUTS("Fatal error in hash_pointer_update: p->pData == pData\n");
				return FAILURE;
			}
#endif
			BLOCK_INTERRUPTIONS;
			if (!p->bIsPointer && ht->pDestructor) {
				ht->pDestructor(p->pData);
				pefree(p->pData,ht->persistent);
			}
			p->pData = pData;
			p->bIsPointer = 1;
            if (h >= ht->nNextFreeElement) {
                ht->nNextFreeElement = h + 1;
            }
			UNBLOCK_INTERRUPTIONS;
			return SUCCESS;
		}
		p = p->pNext;
	}
	p = (Bucket *) pemalloc(sizeof(Bucket),ht->persistent);
	if (!p) {
		return FAILURE;
	}
	p->nKeyLength = 0;
	p->pData = pData;
	p->h = h;
	p->bIsPointer = 1;
	p->arKey = (char *)NULL;

	p->pNext = ht->arBuckets[nIndex];

	BLOCK_INTERRUPTIONS;
	if (ht->pInternalPointer == NULL) {
		ht->pInternalPointer = p;
	}
	ht->arBuckets[nIndex] = p;

	/* Setup the double linked list */
	p->pListLast = ht->pListTail;
	ht->pListTail = p;
	p->pListNext = NULL;
	if (p->pListLast != NULL) {
		p->pListLast->pListNext = p;
	}
	if (!ht->pListHead) {
		ht->pListHead = p;
	}
	UNBLOCK_INTERRUPTIONS;

	ht->nNumOfElements++;
    if (h >= ht->nNextFreeElement) {
	    ht->nNextFreeElement = h + 1;
    }
	if_full_do_resize(ht);		/* If the Hash table is full, resize it */
	return SUCCESS;
}


int hash_is_pointer(HashTable *ht, char *arKey, uint nKeyLength)
{

	uint h, nIndex;
	Bucket *p;
#if DEBUG
	TLS_VARS;
#endif

	h = ht->pHashFunction(arKey, nKeyLength);
	nIndex = h % ht->nTableSize;

	if (nKeyLength <= 0) {
#if DEBUG
		PUTS("hash_update: Can't check for empty key\n");
#endif
		return FAILURE;
	}
	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->arKey != NULL) && (p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				return (p->bIsPointer);
			}
		}
		p = p->pNext;
	}
	return 0;
}

int hash_index_is_pointer(HashTable *ht, uint h)
{

	uint nIndex;
	Bucket *p;

	nIndex = h % ht->nTableSize;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->arKey == NULL) && (p->h == h)) {
			return (p->bIsPointer);
		}
		p = p->pNext;
	}
	return 0;
}


int if_full_do_resize(HashTable *ht)
{
	Bucket **t;

	if ((ht->nNumOfElements > ht->nTableSize) && (ht->nHashSizeIndex < nNumPrimeNumbers - 1)) {		/* Let's double the table
																									   size */
		t = (Bucket **) perealloc(ht->arBuckets, PrimeNumbers[ht->nHashSizeIndex + 1] * sizeof(Bucket *),ht->persistent);
		if (t) {
			BLOCK_INTERRUPTIONS;
			ht->arBuckets = t;
			ht->nTableSize = PrimeNumbers[ht->nHashSizeIndex + 1];
			ht->nHashSizeIndex++;
			hash_rehash(ht);
			UNBLOCK_INTERRUPTIONS;
			return SUCCESS;
		}
		return FAILURE;
	}
	return SUCCESS;
}

int hash_rehash(HashTable *ht)
{
	Bucket *p;
	uint nIndex;

	memset(ht->arBuckets, 0, PrimeNumbers[ht->nHashSizeIndex] * sizeof(Bucket *));
	p = ht->pListHead;
	while (p != NULL) {
		nIndex = p->h % ht->nTableSize;
		p->pNext = ht->arBuckets[nIndex];
		ht->arBuckets[nIndex] = p;
		p = p->pListNext;
	}
	return SUCCESS;
}

PHPAPI int hash_del_key_or_index(HashTable *ht, char *arKey, uint nKeyLength, uint h, int flag)
{
	uint nIndex;
	Bucket *p, *t = NULL;		/* initialize just to shut gcc up with -Wall */

	if (flag == HASH_DEL_KEY) {
		h = ht->pHashFunction(arKey, nKeyLength);
	}
	nIndex = h % ht->nTableSize;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && ((p->arKey == NULL) || /* Numeric index */
			((p->arKey != NULL) && (p->nKeyLength == nKeyLength) && (!memcmp(p->arKey, arKey, nKeyLength))))) {
			BLOCK_INTERRUPTIONS;
			if (p == ht->arBuckets[nIndex]) {
				ht->arBuckets[nIndex] = p->pNext;
			} else {
				t->pNext = p->pNext;
			}
			if (p->pListLast != NULL) {
				p->pListLast->pListNext = p->pListNext;
			} else { 
				/* Deleting the head of the list */
				ht->pListHead = p->pListNext;
			}
			if (p->pListNext != NULL) {
				p->pListNext->pListLast = p->pListLast;
			} else {
				ht->pListTail = p->pListLast;
			}
			if (flag == HASH_DEL_KEY) {
				pefree(p->arKey,ht->persistent);
			}
			if (!p->bIsPointer) {
				if (ht->pDestructor) {
					ht->pDestructor(p->pData);
				}
				pefree(p->pData,ht->persistent);
			}
			if (ht->pInternalPointer == p) {
				ht->pInternalPointer = ht->pListTail;
			}
			pefree(p,ht->persistent);
			UNBLOCK_INTERRUPTIONS;
			ht->nNumOfElements--;
			return SUCCESS;
		}
		t = p;
		p = p->pNext;
	}
	return FAILURE;
}


PHPAPI void hash_destroy(HashTable *ht)
{
	Bucket *p, *q;

	p = ht->pListHead;
	while (p != NULL) {
		q = p;
		p = p->pListNext;
		if (!q->bIsPointer) {
			if (ht->pDestructor) {
				ht->pDestructor(q->pData);
			}
			if (q->pData) {
				pefree(q->pData,ht->persistent);
			}
		}
		if (q->arKey) {
			pefree(q->arKey,ht->persistent);
		}
		pefree(q,ht->persistent);
	}
	pefree(ht->arBuckets,ht->persistent);
}


/* This is used to selectively delete certain entries from a hashtable.
 * destruct() receives the data and decides if the entry should be deleted 
 * or not
 */
void hash_apply(HashTable *ht,int (*destruct) (void *))
{
	Bucket *p, *q;

	p = ht->pListHead;
	while (p != NULL) {
		q = p;
		p = p->pListNext;
		if (destruct(q->pData)) {
			if (q->arKey == NULL) {
				hash_index_del(ht, q->h);
			} else {
				hash_del(ht,q->arKey,q->nKeyLength);
			}
		}
	}
}


#if 1
void hash_copy(HashTable **target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size)
{
	HashTable *t;
	Bucket *p;

	t = (HashTable *) pemalloc(sizeof(HashTable),0);
	if (!t) {
		*target = NULL;
		return;
	}
    if (hash_init(t, source->nTableSize, source->pHashFunction,source->pDestructor, 0) == FAILURE) {
        *target = NULL;
        return;
    }

    p = source->pListHead;
	while (p) {
		memcpy(tmp, p->pData, size);
		pCopyConstructor(tmp);
		if (p->arKey) {
			hash_update(t, p->arKey, p->nKeyLength, tmp, size, NULL);
		} else {
			hash_index_update(t, p->h, tmp, size, NULL);
		}
		p = p->pListNext;
	}
	t->pInternalPointer = t->pListHead;
	*target = t;
}


void hash_merge(HashTable *target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size)
{
	Bucket *p;
	void *t;

    p = source->pListHead;
	while (p) {
		memcpy(tmp, p->pData, size);
		if (p->arKey) {
			if (hash_add(target, p->arKey, p->nKeyLength, tmp, size, &t)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		} else {
			if (hash_index_update(target, p->h, tmp, size, &t)==SUCCESS && pCopyConstructor) {
				pCopyConstructor(t);
			}
		}
		p = p->pListNext;
	}
	target->pInternalPointer = target->pListHead;
}

#else

void hash_copy(HashTable **target, HashTable *source, void (*pCopyConstructor) (void *pData), void *tmp, uint size)
{
	HashTable *t;
	Bucket *p,*nb;
	uint nIndex;

	t = (HashTable *) pemalloc(sizeof(HashTable),0);
	if (!t) {
		*target = NULL;
		return;
	}
	
	memcpy(t,source,sizeof(HashTable));
	t->arBuckets = (Bucket **) pecalloc(source->nTableSize, sizeof(Bucket *), 0);
	if (!t) {
		*target = NULL;
		return;
	}

	t->pListHead = NULL;
	t->pListTail = NULL;

	p = source->pListHead;
	while (p) {
		/* Create a duplicate of the bucket */
		nb = (Bucket *) pemalloc(sizeof(Bucket),0);
		if (!nb) {
			*target = NULL;
			return;
		}
		memcpy(nb,p,sizeof(Bucket));
		if (p->nKeyLength) {
			nb->arKey = (char *) pemalloc(p->nKeyLength,0);
			if (!nb->arKey) {
				*target = NULL;
				return;
			}
			memcpy(nb->arKey, p->arKey, p->nKeyLength);
		}

		if (!p->bIsPointer) {
			nb->pData = pemalloc(size,0);
			if (!nb->pData) {
				*target = NULL;
				return;
			}
			memcpy(nb->pData,p->pData,size);
			pCopyConstructor(nb->pData);
		}
			
		/* Setup the double linked list */
		nb->pListLast = t->pListTail;
		t->pListTail = nb;
		nb->pListNext = NULL;
		if (nb->pListLast != NULL) {
			nb->pListLast->pListNext = nb;
		}
		if (!t->pListHead) {
			t->pListHead = nb;
		}
	
    	nIndex = nb->h % t->nTableSize;
		nb->pNext = t->arBuckets[nIndex];
		t->arBuckets[nIndex] = nb;

		p = p->pListNext;
	}
	t->pInternalPointer = t->pListHead;
	*target = t;
}

#endif


/* Returns SUCCESS if found and FAILURE if not. The pointer to the
 * data is returned in pData. The reason is that there's no reason
 * someone using the hash table might not want to have NULL data
 */
PHPAPI int hash_find(HashTable *ht, char *arKey, uint nKeyLength, void **pData)
{
	uint h, nIndex;
	Bucket *p;

	h = ht->pHashFunction(arKey, nKeyLength);
	nIndex = h % ht->nTableSize;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->arKey != NULL) && (p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				*pData = p->pData;
				return SUCCESS;
			}
		}
		p = p->pNext;
	}
	return FAILURE;
}

int hash_exists(HashTable *ht, char *arKey, uint nKeyLength)
{
	uint h, nIndex;
	Bucket *p;

	h = ht->pHashFunction(arKey, nKeyLength);
	nIndex = h % ht->nTableSize;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->arKey != NULL) && (p->h == h) && (p->nKeyLength == nKeyLength)) {
			if (!memcmp(p->arKey, arKey, nKeyLength)) {
				return 1;
			}
		}
		p = p->pNext;
	}
	return 0;
}


PHPAPI int hash_index_find(HashTable *ht, uint h, void **pData)
{
	uint nIndex;
	Bucket *p;

	nIndex = h % ht->nTableSize;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->arKey == NULL)) {
			*pData = p->pData;
			return SUCCESS;
		}
		p = p->pNext;
	}
	return FAILURE;
}


int hash_index_exists(HashTable *ht, uint h)
{
	uint nIndex;
	Bucket *p;

	nIndex = h % ht->nTableSize;

	p = ht->arBuckets[nIndex];
	while (p != NULL) {
		if ((p->h == h) && (p->arKey == NULL)) {
			return 1;
		}
		p = p->pNext;
	}
	return 0;
}


PHPAPI inline int hash_num_elements(HashTable *ht)
{
	return ht->nNumOfElements;
}


void hash_internal_pointer_reset(HashTable *ht)
{
	ht->pInternalPointer = ht->pListHead;
}


/* This function will be extremely optimized by remembering 
 * the end of the list
 */
void hash_internal_pointer_end(HashTable *ht)
{
	ht->pInternalPointer = ht->pListTail;
}


void hash_move_forward(HashTable *ht)
{
	if (ht->pInternalPointer) {
		ht->pInternalPointer = ht->pInternalPointer->pListNext;
	}
}

void hash_move_backwards(HashTable *ht)
{
	if (ht->pInternalPointer) {
		ht->pInternalPointer = ht->pInternalPointer->pListLast;
	}
}


int hash_get_current_key(HashTable *ht, char **str_index, int *int_index)
{
	Bucket *p = ht->pInternalPointer;

	if (p) {
		if (p->arKey) {
			*str_index = (char *) pemalloc(p->nKeyLength,ht->persistent);
			memcpy(*str_index, p->arKey, p->nKeyLength);
			return HASH_KEY_IS_STRING;
		} else {
			*int_index = p->h;
			return HASH_KEY_IS_INT;
		}
	}
	return HASH_KEY_NON_EXISTANT;
}


int hash_get_current_data(HashTable *ht, void **pData)
{
	Bucket *p = ht->pInternalPointer;

	if (p) {
		*pData = p->pData;
		return SUCCESS;
	} else {
		return FAILURE;
	}
}


int hash_sort(HashTable *ht, int (*compar) (const void *, const void *), int renumber)
{
	Bucket **arTmp;
	Bucket *p;
	int i, j;

	if (ht->nNumOfElements <= 1) {	/* Doesn't require sorting */
		return SUCCESS;
	}
	arTmp = (Bucket **) pemalloc(ht->nNumOfElements * sizeof(Bucket *),ht->persistent);
	if (!arTmp) {
		return FAILURE;
	}
	p = ht->pListHead;
	i = 0;
	while (p) {
		arTmp[i] = p;
		p = p->pListNext;
		i++;
	}

	qsort((void *) arTmp, i, sizeof(Bucket *), compar);

	BLOCK_INTERRUPTIONS;
	ht->pListHead = arTmp[0];
	ht->pListTail = NULL;
	ht->pInternalPointer = ht->pListHead;

	for (j = 0; j < i; j++) {
		if (ht->pListTail) {
			ht->pListTail->pListNext = arTmp[j];
		}
		arTmp[j]->pListLast = ht->pListTail;
		arTmp[j]->pListNext = NULL;
		ht->pListTail = arTmp[j];
	}
	pefree(arTmp,ht->persistent);
	UNBLOCK_INTERRUPTIONS;

	if (renumber) {
		p = ht->pListHead;
		i=0;
		while (p != NULL) {
			if (p->arKey) {
				pefree(p->arKey,ht->persistent);
			}
			p->arKey = NULL;
			p->nKeyLength = 0;
			p->h = i++;
			p = p->pListNext;
		}
		ht->nNextFreeElement = i;
		hash_rehash(ht);
	}
	return SUCCESS;
}


uint hash_next_free_element(HashTable *ht)
{
	return ht->nNextFreeElement;

}

#if DEBUG
void hash_display_pListTail(HashTable *ht)
{
	Bucket *p;

	p = ht->pListTail;
	while (p != NULL) {
		php3_printf("pListTail has key %s\n", p->arKey);
		p = p->pListLast;
	}
}

void hash_display(HashTable *ht)
{
	Bucket *p;
	uint i;

	for (i = 0; i < ht->nTableSize; i++) {
		p = ht->arBuckets[i];
		while (p != NULL) {
			php3_printf("%s <==> 0x%X\n", p->arKey, p->h);
			p = p->pNext;
		}

	}

	p = ht->pListTail;
	while (p != NULL) {
		php3_printf("%s <==> 0x%X\n", p->arKey, p->h);
		p = p->pListLast;
	}
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
