/***[cond.c]******************************************************[TAB=4]****\
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
/* $Id: cond.c,v 1.17 1997/07/14 19:12:51 rasmus Exp $ */
#include <stdlib.h>
#include <string.h>
#include "php.h"
#include "parse.h"

int iflevel=0;
int ifstate=1;

static CondStack *top=NULL;
static CondStack *ru_top=NULL;

static MatchStack *mtop=NULL;
static MatchStack *ru_mtop=NULL;

static CondMark *mark=NULL;
static CondMark *ru_mark=NULL;

static MatchMark *mmark=NULL;
static MatchMark *ru_mmark=NULL;

static EndBraceStack *btop=NULL;
static EndBraceStack *ru_btop=NULL;

static EndBraceMark *bmark=NULL;
static EndBraceMark *ru_bmark=NULL;

void php_init_cond(void) {
	top=NULL;
	ru_top=NULL;
	mtop=NULL;
	ru_mtop=NULL;
	mark=NULL;
	ru_mark=NULL;
	mmark=NULL;
	ru_mmark=NULL;
	btop=NULL;
	ru_btop=NULL;
	bmark=NULL;
	ru_bmark=NULL;
}

void MatchPush(int val) {
	MatchStack *new;

	if(ru_mtop) {
		new = ru_mtop;
		ru_mtop = ru_mtop->next;
	} else {
		new = emalloc(0,sizeof(MatchStack));
	}
	new->val = val;
	new->next = mtop;
	mtop = new;	
}

int MatchPop(void) {
	MatchStack *s;
	int ret;

	if(!mtop) {
		Error("if/else/endif error");
		return(1);
	}
	ret = mtop->val;
	s = mtop;
	mtop = mtop->next;
	s->next = ru_mtop; /* stick node onto re-use stack */
	ru_mtop=s;
	return(ret);
}	

void BracePush(int token) {
	EndBraceStack *new;

	if(ru_btop) {
		new = ru_btop;
		ru_btop = ru_btop->next;
	} else {
		new = emalloc(0,sizeof(EndBraceStack));
	}
	new->token = token;
	new->next = btop;
	btop=new;
}

int BracePop(void) {
	EndBraceStack *s;
	int ret;

	if(!btop) {
		Error("Misplaced }");
		return(0);
	}
	ret = btop->token;
	s = btop;
	btop=btop->next;
	s->next=ru_btop;
	ru_btop=s;
	return(ret);
}

int BraceCheck(void) {
	if(btop) {
		return(btop->token);
	}
#if DEBUG
	Debug("BraceCheck: no btop\n");
#endif
	return(0);
}

void ShowCondStack(void) {
	CondStack *s;

	s = top;
	while(s) {
#if DEBUG
		Debug("(%d,%d) ",s->state, s->active);
#endif
		s = s->next;
	}
#if DEBUG
	Debug("\n");
#endif
}

void CondPush(int state, int active) {
	CondStack *new;

	if(!ru_top) { /* If no nodes on the re-use stack */
		new = emalloc(0,sizeof(CondStack));
	} else { 
		new = ru_top;  /* grab node from re-use stack */
		ru_top=ru_top->next;	
	}
	new->state = state;
	new->active = active;
	new->next=top;
	top=new;
}

void CondChange(int state, int active) {
	if(!top) return;

	top->state = state;
	top->active = active;
}

int CondPop(int *active) {
	int ret=0;
	CondStack *s;

	if(top) ret = top->state;
	else return(ret);
	if(active) *active = top->active;
	s = top;
	top=top->next;
	s->next = ru_top; /* stick node onto re-use stack */
	ru_top=s;
	return(ret);
}

int CondPeek(int *active) {
	int ret=0;

	if(top) ret = top->state;
	else return(ret);
	if(active) *active = top->active;
	return(ret);
}

int GetCurrentState(int *active) {
	int ret;

	if(!top) { 
		ret=1; 
		if(active) *active=-1; 
	} else { 
		ret=top->state; 
		if(active) *active=top->active; 
	}
	return(ret);
}

int GetCurrentActive(void) {
	if(!top) return(0);
	else return(top->active);
}

void ClearCondStack(void) {
	CondStack *s;

	s=top;
	while(s) {
		top=s->next;
		s=top;
	}
}

void PushCondMatchMarks(void) {
	CondMark *new;
	MatchMark *mnew;
	EndBraceMark *bnew;

	if(ru_mark) {
		new = ru_mark;
		ru_mark = ru_mark->next;
	} else {
		new = emalloc(0,sizeof(struct CondMark));
	}
	if(ru_mmark) {
		mnew = ru_mmark;
		ru_mmark = ru_mmark->next;
	} else {
		mnew = emalloc(0,sizeof(struct MatchMark));
	}
	if(ru_bmark) {
		bnew = ru_bmark;
		ru_bmark = ru_bmark->next;
	} else {
		bnew = emalloc(0,sizeof(struct EndBraceMark));
	}
	new->mark = top;
	mnew->mark = mtop;
	bnew->mark = btop;
	new->next=mark;
	mnew->next=mmark;
	bnew->next=bmark;
	mark=new;
	mmark=mnew;
	bmark=bnew;
}

void PopCondMatchMarks(void) {
	CondMark *temp;
	MatchMark *mtemp;

	while(mark && top && mark->mark!=top) {
		CondPop(NULL);	
	}	
	if(mark) {
		temp=mark->next;
		mark->next = ru_mark;
		ru_mark = mark;
		mark=temp;
	}	
	while(mmark && mtop && mmark->mark!=mtop) {
		MatchPop();
	}	
	if(mmark) {
		mtemp=mmark->next;
		mmark->next = ru_mmark;
		ru_mmark = mmark;
		mmark=mtemp;
	}	
}

int Logic(int lop) {
	Stack *s, a, b;
	int ret=0;
	char temp[32];

	a.strval=NULL;
	b.strval=NULL;
	s=Pop();		
	if(!s) {
		Error("Stack Error");
		return(0);
	}
	b.type = s->type;
	b.intval = s->intval;
	b.douval = s->douval;
	if(s->strval) b.strval = estrdup(0,s->strval);
 
	s = Pop();
	if(!s) {
		Error("Stack Error");
		return(0);
	}
	a.type = s->type;
	a.intval = s->intval;
	a.douval = s->douval;
	if(s->strval) a.strval = estrdup(0,s->strval);
	
	switch(a.type) {
		case LNUMBER:
			if(lop==LOG_OR) ret = (a.intval || b.intval);
			else ret = (a.intval && b.intval);
			break;

		case DNUMBER:
			if(lop==LOG_OR) ret = (a.douval || b.douval);
			else ret = (a.douval && b.douval);
			break;

		case STRING:
			if(lop==LOG_OR) ret = (strlen(a.strval) || strlen(b.strval));
			else ret = (strlen(a.strval) && strlen(b.strval));
			break;
	}
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
	return(ret);
}
			
int Compare(int cop) {
	Stack *s, a, b;
	int ret=0;
	char temp[32];
	
	a.strval = NULL;
	b.strval = NULL;
	s=Pop();
	if(!s) {
		Error("Stack Error");
		return(0);
	}
	b.type = s->type;
	b.intval = s->intval;
	b.douval = s->douval;
	if(s->strval) b.strval = estrdup(1,s->strval);
 
	s = Pop();
	if(!s) {
		Error("Stack Error");
		return(0);
	}
	a.type = s->type;
	a.intval = s->intval;
	a.douval = s->douval;
	if(s->strval) a.strval = estrdup(1,s->strval);

	switch(a.type) {
		case LNUMBER:
			switch(cop) {
			case COND_EQ:
				ret = (a.intval == b.intval);
				break;

			case COND_NE:
				ret = (a.intval != b.intval);
				break;
			
			case COND_GT:
				ret = (a.intval > b.intval);
				break;

			case COND_GE:
				ret = (a.intval >= b.intval);
				break;

			case COND_LT:
				ret = (a.intval < b.intval);
				break;

			case COND_LE:
				ret = (a.intval <= b.intval);
				break;
			}
			break;

		case DNUMBER:
			switch(cop) {
			case COND_EQ:
				ret = (a.douval == b.douval);
				break;

			case COND_NE:
				ret = (a.douval != b.douval);
				break;
			
			case COND_GT:
				ret = (a.douval > b.douval);
				break;

			case COND_GE:
				ret = (a.douval >= b.douval);
				break;

			case COND_LT:
				ret = (a.douval < b.douval);
				break;

			case COND_LE:
				ret = (a.douval <= b.douval);
				break;
			}
			break;

		case STRING:
			switch(cop) {
			case COND_EQ:
				ret = (strcmp(a.strval,b.strval) == 0);
				break;

			case COND_NE:
				ret = (strcmp(a.strval,b.strval) != 0);
				break;
			
			case COND_GT:
				ret = (strcmp(a.strval,b.strval) > 0);
				break;

			case COND_GE:
				ret = (strcmp(a.strval,b.strval) >= 0);
				break;

			case COND_LT:
				ret = (strcmp(a.strval,b.strval) < 0);
				break;

			case COND_LE:
				ret = (strcmp(a.strval,b.strval) <= 0);
				break;
			}
			break;
	}	
	sprintf(temp,"%d",ret);
	Push(temp,LNUMBER);
	return(ret);
}

void Not(void) {
	Stack *s;
	char temp[128];

	s = Pop();
	if(!s) {
		Error("Stack Error");
		return;
	}
	switch(s->type) {
		case LNUMBER:
			sprintf(temp,"%d",!s->intval);
			break;
		case DNUMBER:
			sprintf(temp,"%d",!s->douval);
			break;
		case STRING:
			sprintf(temp,"%d",!strlen(s->strval));
			break;
	}
	Push(temp,LNUMBER);
}

/* 
 * This returns the conditional value to be pushed onto the
 * stack by conditional expressions
 */
int CheckCond(Stack *s) {
	if(s->var) {
		switch(s->var->type) {
		case LNUMBER:
		case DNUMBER:
			return(s->intval);
			break;
		case STRING:
			return(strlen(s->strval));
			break;
		}
	} else {
		switch(s->type) {
		case LNUMBER:
		case DNUMBER:
			return(s->intval);
			break;
		case STRING:
			return(strlen(s->strval));
			break;
		}
	}
	return(s->intval);
}

void If(void) {
	Stack *s;
	int c;

	if(!GetCurrentState(NULL)) {
		CondPush(0,0);
		MatchPush(0);
		BracePush(ENDIF);
	} else {
		s = Pop();
		if(!s) {
			Error("Stack Error in if statement");
			return;
		}
		c = CheckCond(s);
		CondPush(c,1);
		MatchPush(c);
		BracePush(ENDIF);
	}
}

void Else(void) {
	int state;
	int active;
	int match;

	state = GetCurrentState(&active);

	if(active<0) {
		Error("Misplaced else");
		return;
	}
	if(!active) return;
	CondPop(NULL);
	match = MatchPop();
	CondPush(match?0:1,-1);		
	MatchPush(!match);
}

void ElseIf(void) {
	Stack *s;
	int state, active, match, c;

	state = GetCurrentState(&active);
	
	if(active<0) {
		Error("Misplaced elseif");
		return;
	}
	if(!active) return;
	match = MatchPop();
	CondPop(NULL);
	if(!match) {	
		s = Pop();
		if(!s) {
			Error("Stack Error in elseif statement");
			return;
		}
		c = CheckCond(s);
		CondPush(c,1);
		MatchPush(c);
	} else {
		MatchPush(match);
		CondPush(0,1);
	}
}

void EndIf(void) {
	CondPop(NULL);
	MatchPop();
	BracePop();
}
