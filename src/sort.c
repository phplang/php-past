/***[sort.c]******************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996 Rasmus Lerdorf                                         *
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
/* $Id: sort.c,v 1.2 1996/05/08 21:32:40 rasmus Exp $ */
#include <php.h>
#include <stdlib.h>
#include <parse.h>

int comp_string(char **a, char **b) {
	return(strcmp(*a,*b));
}

int comp_real(char **a, char **b) {
	double f, s;
	
	f = atof(*a);
	s = atof(*b);
	if(f==s) return(0);
	return(f>s?1:-1);	
}

int comp_long(char **a, char **b) {
	long f, s;

	f = atol(*a);
	s = atof(*b);
	if(f==s) return(0);
	return(f>s?1:-1);
}

void Sort(void) {
	Stack *s;
	VarTree *var;
	static char **array;
	int num=1024,count=0;
	int type;

	s = Pop();
	if(!s) {
		Error("Stack error in sort");
		return;
	}
	var = s->var;
	if(!var) return;
	type = var->type;
	array = (char **)emalloc(1,num*sizeof(char *));	
	while(var) {
		array[count] = var->strval;
		count++;
		var = var->next;
		if(count>=num) {
			/* FIXME - no realloc's allowed!! */
			array = realloc(array, num*2*sizeof(char *));
			num = num*2;
		}
	}
#if DEBUG
	Debug("count = %d\n",count);
#endif

	switch(type) {
	case STRING:
		(void)qsort(array,count,sizeof(char *),(int (*)(const void *, const void *))comp_string);
		break;
	case LNUMBER:
		(void)qsort(array,count,sizeof(char *),(int (*)(const void *, const void *))comp_long);
		break;
	case DNUMBER:
		(void)qsort(array,count,sizeof(char *),(int (*)(const void *, const void *))comp_real);
		break;
	}
	count=0;
	var = s->var;
	while(var) {
		var->strval = array[count];	
		var->intval = atol(var->strval);
		var->douval = atof(var->strval);
		var = var->next;
		count++;
	}
}
