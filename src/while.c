/***[while.c]*****************************************************[TAB=4]****\
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
/* $Id: while.c,v 1.18 1997/10/25 20:54:51 mitch Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "php.h"
#include "parse.h"

static WhileStack *top=NULL;
static WhileMark *mark=NULL;
static long WhileCount;

void php_init_while(void) {
	top = NULL;
	mark=NULL;
	WhileCount=0;
}

void ShowWhileStack(void) {
	WhileStack *s;
	
	s = top;
	while(s) {
		s = s->next;
	}
}

void WhilePush(long seekpos, int offset, int lineno) {
	WhileStack *new;

	new=emalloc(0,sizeof(WhileStack));
	new->seekpos = seekpos;
	new->offset = offset;
	new->next = top;
	new->state = 1;
	new->lineno = lineno;
	top = new;
}

WhileStack *WhilePop(void) {
	static WhileStack ret;
	WhileStack *s;

	s = top;
	ret.seekpos = top->seekpos;
	ret.offset = top->offset;
	ret.state = top->state;
	ret.lineno = top->lineno;
	top = top->next;
	return(&ret);
}

WhileStack *WhilePeek(void) {
	static WhileStack ret;

	if(!top) return NULL;
	ret.seekpos = top->seekpos;
	ret.offset = top->offset;
	ret.state = top->state;
	ret.lineno = top->lineno;
	return(&ret);
}

void PushWhileMark(void) {
	WhileMark *new;

	new = emalloc(0,sizeof(struct WhileMark));
	new->mark = top;
	new->next = mark;
	mark=new;
}

void PopWhileMark(void) {
	WhileMark *temp;

	while(mark && top && mark->mark != top) {
		WhilePop();
	}
	if(mark) {
		temp=mark->next;
		mark=temp;
	}
}

void While(long sp) {
	Stack *s;
	int active, state, c;
	
	state = GetCurrentState(&active);
	if(state) {
		s = Pop();
		if(!s) {
			Error("Stack Error in while");
			return;
		}
		c = CheckCond(s);
#ifdef PHP_LOOPLIMIT
		WhileCount++;
		if(c!=0 && WhileCount>PHP_LOOPLIMIT) {
			Error("Program in infinite loop (exceeded PHP_LOOPLIMIT), aborting");
			c = 0;
		}
#endif
		top->state = c;
		if(NewWhileIteration(sp)) {
			CondPush(c,-4);
			BracePush(ENDWHILE);
		} else {
			CondChange(c,-4);  /* update condition on condition stack */
		}
		if(!c) {
			WhilePop();
		}
	} else {
		CondPush(state,-4);
		BracePush(ENDWHILE);
	}
}

void EndWhile(void) {
	WhileStack *t=NULL;
	int active, state;

	state = CondPeek(&active);
	if(active != -4) {
		Error("endwhile misplaced");
		return;
	}	
	if(state && (t=WhilePeek())) {
		WhileAgain(t->seekpos,t->offset,t->lineno);
	} else {
		CondPop(&active);
		BracePop();
		WhileFinish();
	}
}
