/***[sort.c]******************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996,1997 Rasmus Lerdorf                                    *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License as published by      *
*  the Free Software Foundation; either version 2 of the License, or         *
*  (at your option) any later version.                                       *
*                                                                            *
*  This program is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU General Public License for more details.                              *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with this program; if not, write to the Free Software               *
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
*                                                                            *
\****************************************************************************/
/* $Id: sort.c,v 1.9 1997/06/18 04:57:31 rasmus Exp $ */
#include "php.h"
#include <stdlib.h>
#include "parse.h"

typedef struct _array {
	char *ptr;
	char *iname;
} Array;

int comp_string(Array *a, Array *b) {
	return(strcmp(a->ptr,b->ptr));
}

int rcomp_string(Array *a, Array *b) {
	return(-1*strcmp(a->ptr,b->ptr));
}

int comp_real(Array *a, Array *b) {
	double f, s;
	
	f = atof(a->ptr);
	s = atof(b->ptr);
	if(f==s) return(0);
	return(f>s?1:-1);	
}

int rcomp_real(Array *a, Array *b) {
	double f, s;
	
	f = atof(a->ptr);
	s = atof(b->ptr);
	if(f==s) return(0);
	return(f>s?-1:1);	
}

int comp_long(Array *a, Array *b) {
	long f, s;

	f = atol(a->ptr);
	s = atol(b->ptr);
	if(f==s) return(0);
	return(f>s?1:-1);
}

int rcomp_long(Array *a, Array *b) {
	long f, s;

	f = atol(a->ptr);
	s = atol(b->ptr);
	if(f==s) return(0);
	return(f>s?-1:1);
}

/*
 * Sort has two modes.  Associative (mode=1) and non-associative (mode=0)
 * In associative mode, the index-value pairings are maintained and the array
 * is sorted such that it can be stepped through using the next()/prev() functions.
 * In non-associated mode (mode=0) the indices stay as in the original order and 
 * the values are shuffled around.
 *
 * dir specifies ascending/descending sort order
 */
void Sort(int mode, int dir) {
	Stack *s;
	VarTree *var;
	Array *array;
	Array *old_array;
	Array *new_array;
	int num=1024,count=0;
	int type, size=1;

	s = Pop();
	if(!s) {
		Error("Stack error in sort");
		return;
	}
	var = s->var;
	if(!var) return;
	type = var->type;
	array = (Array *)emalloc(1,num*sizeof(Array));	
	while(var) {
		if(!var->deleted) {
			array[count].ptr = var->strval;
			array[count].iname = var->iname;
			count++;
		}
		var = var->next;
		if(count>=num) {
			new_array = emalloc(1, ++size * num * sizeof(Array));
			memcpy(new_array,array,(size-1)*num*sizeof(Array));
			array = new_array;
			num = size*num;
		}
	}
	old_array = emalloc(1,count*sizeof(Array));
	memcpy(old_array,array,count*sizeof(Array));
#if DEBUG
	Debug("count = %d\n",count);
#endif

	switch(type) {
	case STRING:
		if(dir)
			(void)qsort(array,count,sizeof(Array),(int (*)(const void *, const void *))rcomp_string);
		else
			(void)qsort(array,count,sizeof(Array),(int (*)(const void *, const void *))comp_string);
		break;
	case LNUMBER:
		if(dir)
			(void)qsort(array,count,sizeof(Array),(int (*)(const void *, const void *))rcomp_long);
		else
			(void)qsort(array,count,sizeof(Array),(int (*)(const void *, const void *))comp_long);
		break;
	case DNUMBER:
		if(dir)
			(void)qsort(array,count,sizeof(Array),(int (*)(const void *, const void *))rcomp_real);
		else
			(void)qsort(array,count,sizeof(Array),(int (*)(const void *, const void *))comp_real);
		break;
	}
	var = s->var;
	count=0;
	while(var) {
		if(!var->deleted) {
			var->strval = array[count].ptr;	
			var->intval = atol(var->strval);
			var->douval = atof(var->strval);
			if(mode) {
				var->iname = array[count].iname;
			}
			count++;
		}
		var = var->next;
	}
}
