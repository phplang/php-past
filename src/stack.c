/***[stack.c]*****************************************************[TAB=4]****\
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
/* $Id: stack.c,v 1.21 1996/07/18 18:20:30 rasmus Exp $ */
/* Expression Stack */
#include <stdlib.h>
#include <string.h>
#include "php.h"
#include "parse.h"

static Stack *top=NULL;

void php_init_stack(void) {
	top = NULL;
}

#if DEBUG
void ShowStack(void) {
	Stack *s;

	Debug("ShowStack()\n");
	s=top;
	while(s) {
		Debug("(%s)%d",s->strval?s->strval:(char *)"NULL",s->type);
		s=s->next;
	}
	if(top) Debug("\n");
}
#endif

void ClearStack(void) {
	php_pool_clear(2);
	top=NULL;
}
	
void Push(char *value, int type) {
	Stack *new, *s=NULL;
	VarTree *t;
	int next=0;

	if(!value) return;
	if(type==ARRAY) {
		s = Pop();
		if(!s) {
			Error("Stack Error in Push");
			return;
		}
	} else if(type== -ARRAY) { type=ARRAY; next=1; }
	new = emalloc(2,sizeof(Stack));
	new->type   = type;
	new->next   = NULL;
	new->strval = NULL;
	new->var = NULL;
	if(type==DNUMBER || type==LNUMBER) {
		new->strval = estrdup(2,value);
		new->intval = atol(value);
		new->douval = atof(value);
	} else if(type==STRING) {
		new->strval = SubVar(estrdup(2,value));
		new->intval = atol(new->strval);
		new->douval = atof(new->strval);
	} else if(type==VAR) {
		t = GetVar(value,NULL,0);
		if(!t) {
#if DEBUG
			Debug("Undefined variable: %s\n",value);
#endif
			new->strval = estrdup(2,"");
			new->intval = 0;
			new->douval = 0;
			new->type = STRING;
		} else {
			new->strval = estrdup(2,t->strval);
			new->intval = t->intval;
			new->douval = t->douval;
			new->type = t->type;
			new->var = t;
			new->flag = 1;
		}
	} else if(type==ARRAY) {
		if(!next) t = GetVar(value,s->strval,0);
		else t = GetVar(value,NULL,1);
		if(!t) {
#if DEBUG
			Debug("Undefined array variable: %s\n",value);
#endif
			new->strval = estrdup(2,"");
			new->intval = 0;
			new->douval = 0;
			new->type = STRING;
		} else {
			new->strval = estrdup(2,t->strval);
			new->intval = t->intval;
			new->douval = t->douval;
			new->type = t->type;
			new->var = t;
			new->flag = 0;
		}
	}
	new->next = top;
	top = new;
}

Stack *Pop(void) {
	static Stack ret;
#ifndef APACHE
	static int FirstTime=1;
#endif

	if(!top) return(NULL);
#if APACHE
	ret.strval = NULL;
#else
	if(!FirstTime) {
		if(ret.strval) {
			ret.strval=NULL;
		}
	} else {
		ret.strval = NULL;
		FirstTime = 0;
	}
#endif
	ret.type = top->type;
	ret.intval = top->intval;
	ret.douval = top->douval;
	ret.next = top->next;
	ret.var = top->var;
	ret.flag = top->flag;
	if(top->strval) {
		ret.strval = emalloc(2,strlen(top->strval)+1);
		strcpy(ret.strval, top->strval);
	}
	top = ret.next;
	return(&ret);
}
