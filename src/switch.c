/***[switch.c]****************************************************[TAB=4]****\
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
/* $Id: switch.c,v 1.11 1997/01/04 15:17:08 rasmus Exp $ */
#include <stdlib.h>
#include <string.h>
#include "php.h"
#include "parse.h"

static SwitchStack *top=NULL;
static SwitchMark *mark=NULL;

void php_init_switch(void) {
	top = NULL;
	mark=NULL;
}

void SwitchPush(Stack *s) {
	struct SwitchStack *new;

	new = emalloc(0,sizeof(SwitchStack));
	new->type = s->type;
	new->matched = 0;
	new->intval = s->intval;
	new->douval = s->douval;
	if(s->strval) new->strval = estrdup(0,s->strval);
	new->next = top;
	top = new;
}

SwitchStack *SwitchPop(void) {
#ifndef APACHE
	static int FirstTime=1;
#endif
	static SwitchStack ret;
	SwitchStack *s;

	if(!top) return(NULL);

#if APACHE
	ret.strval = NULL;
#else
	if(!FirstTime) {
		if(ret.strval) ret.strval = NULL;
	} else {
		ret.strval=NULL;
		FirstTime=0;
	}
#endif
	ret.type = top->type;
	ret.matched = top->matched;
	ret.intval = top->intval;
	ret.douval = top->douval;
	if(top->strval) {
		ret.strval=estrdup(0,top->strval);
	}
	s = top->next;
	top = s;
	return(&ret);
}	

void PushSwitchMark(void) {
	SwitchMark *new;

	new=emalloc(0,sizeof(struct SwitchMark));
	new->mark=top;
	new->next=mark;
	mark=new;
}

void PopSwitchMark(void) {
	SwitchMark *temp;

	while(mark && top && mark->mark != top) {
		SwitchPop();		
	}
	if(mark) {
		temp = mark->next;
		mark=temp;
	}
}

void Switch(void) {
	Stack *s;
	int active, state;

	state = GetCurrentState(&active);
	if(state) {
		s = Pop();
		if(!s) {
			Error("Stack error in switch");
			return;
		}
		SwitchPush(s);
		CondPush(1,1);
		BracePush(ENDSWITCH);
	} else {
		CondPush(0,-3);
		BracePush(ENDSWITCH);
	}
}

void EndSwitch(void) {
	int active, state;

	state = GetCurrentState(&active);
	if(active==-2) {
		CondPop(NULL);
		state = GetCurrentState(&active);
	}
	if(active==-3) {
		CondPop(NULL);
		BracePop();
	} else {
		SwitchPop();
		CondPop(NULL);
		BracePop();
	}
}

void Case(void) {
	Stack *s;
	int ret;
	int active;
	int state;

	s = Pop();
	if(!s) {
		Error("Stack Error in case statement");
		return;
	}
	if(!top) {
		Error("Case statement found outside of switch");
		return;
	}
	state = GetCurrentState(&active);
	if(state && active==-2) {
		return;
	} else if(!state && active==-2) {
		CondPop(NULL);
	}

	switch(s->type) {
		case LNUMBER:
			ret = (s->intval == top->intval);
			CondPush(ret,-2);
			break;
		case DNUMBER:
			ret = (s->douval == top->douval);
			CondPush(ret,-2);
			break;
		case STRING:
			ret = (strcmp(s->strval,top->strval)==0);
			CondPush(ret,-2);
			break;
	}
}	

void Default(void) {
	int state,active;

	state = GetCurrentState(&active);
	if(state && active==-2) {
		return;
	}
	if(!top->matched) CondPush(1,-2);
	else CondPush(0,-2);
}
	
void Break(void) {
	int active;
	int state;

	active = GetCurrentActive();
	if(active>-2) {
		Error("Misplaced break");
		return;
	}
	state = CondPop(NULL);
	if(!top->matched) {
		top->matched = state;
	}
}
