/***[var.c]*******************************************************[TAB=4]****\
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
/* $Id: var.c,v 1.97 1997/11/17 00:47:21 lr Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "php.h"
#include "parse.h"

static VarTree *var_main=NULL;
static VarTree *var_super=NULL;
static VarTree *var_top=NULL;
static VarTree *var_supertop=NULL;
static PtrStack *ptr_top=NULL;
static FrameStack *fs_top=NULL;

void php_init_symbol_tree(void) {
	var_main=NULL;
	var_top=NULL;
	var_super=NULL;
	var_supertop=NULL;
	ptr_top=NULL;
	fs_top=NULL;
}

/*
 * Creates a new node in binary variable tree, or sets value of existing nodes.
 * New nodes are automatically assigned a scope local to the current frame.
 * 
 * The value should be pushed onto the expression stack before this function
 * is called.  For indexed arrays, push the index value onto the stack first,
 * then the content value.
 *
 * The mode parameter is a bitfield.  
 * mode:  0 = create/set simple variable
 *        1 = append to end of array
 *        2 = set absolute array position
 *        4 = set scope to global
 *        8 = set scope to static
 *       16 = set read-only bit
 *       32 = set to super-global
 *   
 * inc:  -2 = define variable placeholder
 *       -1 = set variable from a GET method argument
 *        0 = set the variable to value on expression stack 
 *        1 = increment by value on expression stack
 *        2 = decrement by value on expression stack
 *        3 = & with value on expression stack
 *        4 = | with value on expression stack
 *        5 = ^ with value on expression stack
 *
 */
void SetVar(char *name, int mode, int inc) {
	VarTree *t, *ot=NULL, *tt, *ptt, *ftt, *ntt, *var, *o, *rt;
	VarTree a, b;
	Stack *st;
	char *s, *buf=NULL;
	int done=0;
	int i=0, oi=0;
	int count=0, in_num=0;
	int whole_array=0, array_cnt=0;
	char temp[256];
	int data_len=0;
	VarTree *new_name=NULL;

   	a.strval = NULL;
	a.allocated = 0;
    b.strval = NULL;
	b.allocated = 0;
    st = Pop();
   	if(!st) {
		/* Don't print an error message in this case */
       	return;
    }
	b.type = st->type;
	b.intval = st->intval;
	b.douval = st->douval;
	var = st->var;
	whole_array = st->flag&1;	/* indicates that src data could be an entire array */
	if(whole_array) {
		array_cnt=0;
		t=var;
		while(t && !done) {
			if(!t->deleted) {
				array_cnt=1;
				done=1;
			} else {
				t=t->next;
			}
		}
		done=0;
	} else {
		array_cnt=1;
	}
   	if(st->strval) {
		b.strval = estrdup(1,st->strval);
		if(inc==-2) b.allocated = 0;
		else b.allocated = strlen(st->strval) + 1;
		data_len = strlen(b.strval);
	}

	if(mode&2) { 
    	st = Pop();
    	if(!st) {
        	Error("Stack Error in SetVar for %s",name);
        	return;
    	}
		a.type = st->type;
		a.intval = st->intval;
		a.douval = st->douval;
    	if(st->strval) {
			a.strval = estrdup(1,st->strval);
		}
	}

	if(*name==VAR_INIT_CHAR) {
		if(mode&3) {
			Error("Sorry, variable array names not supported");
			return;
		}
		new_name = GetVar(name+1,NULL,0);
		if(!new_name) return;
		s = new_name->strval;
	} else s = name;
	if(!(mode&32)) t=var_top;	
	else t=var_supertop;
	while(t && !done) {
		ot=t;
		oi=i;
		if( *(s+i) < *(t->name+i)	) { 
			t=t->left; i=0; 
		}
		else if( *(s+i) > *(t->name+i) ) { 
			t=t->right; i=0; 
		}
		else if( *(s+i) == *(t->name+i) ) {
			if(i==0 && !strcmp(s,t->name)) {
				if((t->scope)&4 && !(mode&4) && fs_top) { /* if setting a global variable */
					var_top = var_main; /* switch to global frame */
					if(mode&2) {
						Push(a.strval,a.type);
					}
					Push(b.strval,b.type);
					SetVar(name,mode,inc);
					var_top = fs_top->frame;
					return;
				}
				if((t->scope)&8 && !(mode&8) && fs_top) { /* setting static var */
					var_top = GetFuncFrame();
					if(mode&2) {
						Push(a.strval,a.type);
					}
					Push(b.strval,b.type);
					SetVar(name,mode,inc);
					var_top = fs_top->frame;
					return;
				}
				if((t->scope)&32 && !(mode&32)) { /* if setting a super-global variable */
					var_top = var_super; /* switch to super-global frame */
					if(mode&2) {
						Push(a.strval,a.type);
					}
					Push(b.strval,b.type);
					SetVar(name,mode,inc); /* recursive call */
					var_top = fs_top->frame; /* switch back */
					return;
				}
				if(!(t->scope&16)) switch(mode&3) { /* set variable if not read-only */
				case 0: /* simple set */
					/* var exists - set new value */
					done=1;
					switch(inc) {
					case 0: /* absolute set */
					case -2:
						t->douval = b.douval;
						t->intval = b.intval;
						/* check if we need a new bit of memory to store value */
#if DEBUG
						Debug("inc=%d, data_len=%d\n",inc,data_len);
#endif
						if(data_len) {
							if(t->strval) {
								if(data_len >= t->allocated) {
									t->strval = estrdup(0,b.strval);
									t->allocated = b.allocated;
								} else {
									strcpy(t->strval,b.strval);	
								}
							} else {
								t->strval = estrdup(0,b.strval);
								t->allocated = b.allocated;
							}							
						} 
						else {
							if(t->strval) *(t->strval)='\0';
							else {
								t->strval = estrdup(0,"");
								t->allocated=1;
							}
						}
						t->type = b.type;
						if(inc!=-2) {
							if(t->deleted) t->count++;
						}
						t->deleted = 0; 
						if (var && whole_array && var->iname) {
							if(t->iname) {
								if(strlen(var->iname) > strlen(t->iname)) {
									t->iname=estrdup(0,var->iname);
								} else {
									strcpy(t->iname,var->iname);
								}
							} else {
								t->iname=estrdup(0,var->iname);
							}
						}
						if(var && whole_array && var->next && array_cnt) {
							o=t->next;
							while(o) {
								o->deleted=1;
								o = o->next;	
							}
							t->count = 1;
							t->lastnode=t->next;
							t->prev=(VarTree *)-1;
							t->lacc = (VarTree *)-1;
							copyarray(t,var->next,t,0);
						}
						break;
					case 1: /* increment */
						switch(t->type) {
						case LNUMBER:
							t->intval+=b.intval;
							t->douval=(double)t->intval;
							sprintf(temp,"%ld",t->intval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									t->allocated = 1;
								}
							}
							break;	
						case DNUMBER:
							t->douval+=b.douval;
							t->intval=(long)t->douval;
							sprintf(temp,"%.10f",t->douval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									t->allocated = 1;
								}
							}
							break;
						case STRING:
							data_len = strlen(t->strval) + strlen(b.strval);
							if(data_len + 1 > t->allocated) {
								buf = emalloc(0,data_len + 1024);	
								t->allocated = data_len+1024;
								*buf='\0';
								if(t->strval) strcpy(buf,t->strval);
							} else {
								buf = t->strval;
							}
							strcat(buf,b.strval);
							t->strval = buf;
							t->douval = atof(t->strval);
							t->intval = atol(t->strval);	
							break;
						}	
						break;
					case 2: /* decrement */
						switch(t->type) {
						case LNUMBER:
							t->intval-=b.intval;
							t->douval=(double)t->intval;
							sprintf(temp,"%ld",t->intval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									t->allocated = 1;
								}
							}
							break;	
						case DNUMBER:
							t->douval-=b.douval;
							t->intval=(long)t->douval;
							sprintf(temp,"%.10f",t->douval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									t->allocated = 1;
								}
							}
							break;
						case STRING:
							if(*(t->strval) && *(b.strval)) {
								Push(b.strval,STRING);
								Push("",STRING);
								Push(t->strval,STRING);
								RegReplace();
								st = Pop();
								if(st) {
									data_len = strlen(st->strval);
									if(data_len) {
										if(t->strval) {
											if(data_len >= t->allocated) {
												t->strval = estrdup(0,st->strval);
												t->allocated = strlen(st->strval) + 1;
											} else {
												strcpy(t->strval,st->strval);	
											}
										} else {
											t->strval = estrdup(0,st->strval);
											t->allocated = strlen(st->strval) + 1;
										}							
									}
									else {
										if(t->strval) *(t->strval)='\0';
										else {
											t->strval = estrdup(0,"");
											t->allocated = 1;
										}
									}
									t->douval = atof(st->strval);
									t->intval = atol(st->strval);
								}
							} else {
								t->strval = buf;
								t->allocated = strlen(buf) + 1;
								t->douval = atof(t->strval);
								t->intval = atol(t->strval);	
							}
							break;
						}	
						break;
					case 3: /* ANDEQ */
						switch(t->type) {
						case LNUMBER:
						case STRING:
							t->intval&=b.intval;
							t->douval=(double)t->intval;
							sprintf(temp,"%ld",t->intval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									t->allocated = 1;
								}
							}
							break;	
						case DNUMBER:
							t->douval = (long)t->douval & (long)b.douval;
							t->intval=(long)t->douval;
							sprintf(temp,"%.10f",t->douval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									t->allocated = 1;
								}
							}
							break;
						}	
						break;
					case 4: /* OREQ */
						switch(t->type) {
						case LNUMBER:
						case STRING:
							t->intval|=b.intval;
							t->douval=(double)t->intval;
							sprintf(temp,"%ld",t->intval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									t->allocated = 1;
								}
							}
							break;	
						case DNUMBER:
							t->douval = (long)t->douval | (long)b.douval;
							t->intval=(long)t->douval;
							sprintf(temp,"%.10f",t->douval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									t->allocated = 1;
								}
							}
							break;
						}	
						break;
					case 5: /* XOREQ */
						switch(t->type) {
						case LNUMBER:
						case STRING:
							t->intval^=b.intval;
							t->douval=(double)t->intval;
							sprintf(temp,"%ld",t->intval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									t->allocated = 1;
								}
							}
							break;	
						case DNUMBER:
							t->douval = (long)t->douval ^ (long)b.douval;
							t->intval=(long)t->douval;
							sprintf(temp,"%.10f",t->douval);
							data_len = strlen(temp);
							if(data_len) {
								if(t->strval) {
									if(data_len >= t->allocated) {
										t->strval = estrdup(0,temp);
										t->allocated = strlen(temp) + 1;
									} else {
										strcpy(t->strval,temp);	
									}
								} else {
									t->strval = estrdup(0,temp);
									t->allocated = strlen(temp) + 1;
								}							
							}
							else {
								if(t->strval) *(t->strval)='\0';
								else {
									t->strval = estrdup(0,"");
									if(inc==-2) t->allocated = 0;
									else t->allocated=1;
								}
							}
							break;
						}	
					}
					t->flag = (inc==-2)?0:inc;
					break;
				case 1: /* append to end of array */
					ptt=t;
					tt=t;
					rt=t;
					ftt=t;
					ntt=NULL;
					count=0;
					while(tt) {
						if(!tt->deleted && tt->allocated) {
#if DEBUG
							Debug("inc counter from %d\n", count);
#endif
							count++;
							if(count==1) {
								rt=tt;
							}
							ftt=tt;
						}
						ptt=tt;
						tt=tt->next;
					}
					if(var && whole_array && var->next && ((ptt->deleted||!ptt->allocated) || strcmp(rt->iname,"0")) && array_cnt) {
						if(ptt->deleted || !ptt->allocated) tt=ptt;
						else tt = GetVar(name,var->iname,0);
						if(tt) {
							if(strlen(var->strval)>(unsigned int)tt->allocated) {
								tt->strval = estrdup(0,var->strval);
								tt->allocated = strlen(var->strval) + 1;
							} else {
								strcpy(tt->strval,var->strval);
							}
							tt->intval = var->intval;
							tt->douval = var->douval;
							tt->type = var->type;
							tt->flag = (inc==-2)?0:inc;
							tt->scope = mode&60;	
							tt->deleted=0;
						}
					}

					if(!tt) {
						if(ftt) {
							in_num = atoi(ftt->iname);
						} else {
							in_num=count;
						}
						if(!ftt || (ftt && ftt->allocated)) {
							sprintf(temp,"%d",in_num);
							tt = GetVar(name,temp,0);
							while(tt) {
								sprintf(temp,"%d",++in_num);
								tt = GetVar(name,temp,0);
							}
						}
						if(ftt==t && (t->deleted || !t->allocated)) {
							tt=t;
							tt->count=0;
						} else if(!ftt->next) {
							tt = emalloc(0,sizeof(VarTree));
							ntt = tt;
							tt->name = estrdup(0,name);
							tt->next=NULL;
							tt->left = t->left;
							tt->right = t->right;
						} else {
							tt = ftt->next;
						}
						tt->intval = b.intval;
						tt->douval = b.douval;
						if(ntt) {
							tt->strval = estrdup(0,b.strval);
							tt->allocated = b.allocated;
						} else {
							if(strlen(b.strval)+1 > (unsigned int)tt->allocated) {
								tt->strval = estrdup(0,b.strval);
								tt->allocated = b.allocated;
							} else {
								strcpy(tt->strval,b.strval);
							}
						}
						tt->type = b.type;
						tt->flag = (inc==-2)?0:inc;
						tt->scope = mode&60;	
						tt->deleted=0;
						if(var && whole_array && var->next && strcmp(t->iname,"0") && array_cnt) {
							tt->iname = estrdup(0,var->iname);
						} else {
							sprintf(temp,"%d",in_num);
							tt->iname=estrdup(0,temp);
						}
						if(ntt) {
							ptt->next = tt;
							if(tt!=ptt) tt->prev = ptt;
							else tt->prev = (VarTree *)-1;
						}
						t->count++;
#if DEBUG
						Debug("Count is now %d\n",t->count);
#endif
						if(t!=tt)
							tt->count = t->count-1;
						t->lastnode = tt;
					} else {
						tt=ptt;
					}
					done = 1;
					if(var && whole_array && var->next && array_cnt) {
						/* Here we will guess that if the first
						   element of the destination array does 
						   not have an index of "0", then we are 
						   dealing with an associative array and
						   we will merge indeces instead of
						   renumbering */
						if(!strcmp(rt->iname,"0")) {
							copyarray(tt,var->next,t,1);
						} else {
							copyarray(tt,var->next,t,2);
						}
					}
					break;
				
				case 2: /* absolute array set */
					ptt=t;
					tt=t;
					while(tt) {
						if(tt->iname && !strcmp(tt->iname,a.strval)) break;
						ptt=tt;
						tt = tt->next;
					}
					if(tt) { /* Found absolute index, re-assign */
						switch(inc) {	
						case 0:
						case -2:
							tt->douval = b.douval;
							tt->intval = b.intval;
							data_len = strlen(b.strval);
							if(data_len) {
								if(tt->strval) {
									if(data_len >= tt->allocated) {
										tt->strval = estrdup(0,b.strval);
										tt->allocated = b.allocated;
									} else {
										strcpy(tt->strval,b.strval);	
									}
								} else {
									tt->strval = estrdup(0,b.strval);
									tt->allocated = b.allocated;
								}							
							}
							else {
								if(tt->strval) *(tt->strval)='\0';
								else {
									tt->strval = estrdup(0,"");
									tt->allocated = 1;
								}
							}
							tt->type = b.type;
							if(tt->deleted) t->count++;
							tt->deleted = 0;
							break;
						case 1: /* increment */
							switch(tt->type) {
							case LNUMBER:
								tt->intval+=b.intval;
								tt->douval=(double)tt->intval;
								sprintf(temp,"%ld",tt->intval);
								data_len = strlen(temp);
								if(data_len) {
									if(tt->strval) {
										if(data_len >= tt->allocated) {
											tt->strval = estrdup(0,temp);
											tt->allocated = strlen(temp) + 1;
										} else {
											strcpy(tt->strval,temp);	
										}
									} else {
										tt->strval = estrdup(0,temp);
										tt->allocated = strlen(temp) + 1;
									}							
								}
								else {
									if(tt->strval) *(tt->strval)='\0';
									else {
										tt->strval = estrdup(0,"");
										tt->allocated = 1;
									}
								}
								break;	
							case DNUMBER:
								tt->douval+=b.douval;
								tt->intval=(long)tt->douval;
								sprintf(temp,"%.10f",tt->douval);
								data_len = strlen(temp);
								if(data_len) {
									if(tt->strval) {
										if(data_len >= tt->allocated) {
											tt->strval = estrdup(0,temp);
											tt->allocated = strlen(temp) + 1;
										} else {
											strcpy(tt->strval,temp);	
										}
									} else {
										tt->strval = estrdup(0,temp);
										tt->allocated = strlen(temp) + 1;
									}							
								}
								else {
									if(tt->strval) *(tt->strval)='\0';
									else {
										tt->strval = estrdup(0,"");
										tt->allocated = 1;
									}
								}
								break;
							case STRING:
								buf = emalloc(0,strlen(tt->strval) + strlen(b.strval) + 1);	
								*buf='\0';
								if(tt->strval) {
									strcpy(buf,tt->strval);
								}
								strcat(buf,b.strval);
								tt->strval = buf;
								tt->allocated = strlen(buf)+1;
								tt->douval = atof(tt->strval);
								tt->intval = atol(tt->strval);	
								break;
							}	
							break;
						case 2: /* decrement */
							switch(tt->type) {
							case LNUMBER:
								tt->intval-=b.intval;
								tt->douval=(double)tt->intval;
								sprintf(temp,"%ld",tt->intval);
								data_len = strlen(temp);
								if(data_len) {
									if(tt->strval) {
										if(data_len >= tt->allocated) {
											tt->strval = estrdup(0,temp);
											tt->allocated = strlen(temp) + 1;
										} else {
											strcpy(tt->strval,temp);	
										}
									} else {
										tt->strval = estrdup(0,temp);
										tt->allocated = strlen(temp) + 1;
									}							
								}
								else {
									if(tt->strval) *(tt->strval)='\0';
									else {
										tt->strval = estrdup(0,"");
										tt->allocated = 1;
									}
								}
								break;	
							case DNUMBER:
								tt->douval-=b.douval;
								tt->intval=(long)tt->douval;
								sprintf(temp,"%.10f",tt->douval);
								data_len = strlen(temp);
								if(data_len) {
									if(tt->strval) {
										if(data_len >= tt->allocated) {
											tt->strval = estrdup(0,temp);
											tt->allocated = strlen(temp) + 1;
										} else {
											strcpy(tt->strval,temp);	
										}
									} else {
										tt->strval = estrdup(0,temp);
										tt->allocated = strlen(temp) + 1;
									}							
								}
								else {
									if(tt->strval) *(tt->strval)='\0';
									else {
										tt->strval = estrdup(0,"");
										tt->allocated = 1;
									}
								}
								break;
							case STRING:
								if(*(tt->strval) && *(b.strval)) {
									Push(b.strval,STRING);
									Push("",STRING);
									Push(tt->strval,STRING);
									RegReplace();
									st = Pop();
									if(st) {
										data_len = strlen(st->strval);
										if(data_len) {
											if(tt->strval) {
												if(data_len >= tt->allocated) {
													tt->strval = estrdup(0,st->strval);
													tt->allocated = strlen(st->strval) + 1;
												} else {
													strcpy(tt->strval,st->strval);	
												}
											} else {
												tt->strval = estrdup(0,st->strval);
												tt->allocated = strlen(st->strval) + 1;
											}							
										}
										else {
											if(tt->strval) *(tt->strval)='\0';
											else {
												tt->strval = estrdup(0,"");
												tt->allocated = 1;
											}
										}
										tt->douval = atof(st->strval);
										tt->intval = atol(st->strval);
									}
								} else {
									tt->strval = buf;
									tt->allocated = strlen(buf) + 1;
									tt->douval = atof(tt->strval);
									tt->intval = atol(tt->strval);	
								}
								break;
							}	
							break;
						}
						tt->flag = (inc==-2)?0:inc;
					} else { /* Wasn't found, create it */
						tt = emalloc(0,sizeof(VarTree));
						tt->name = estrdup(0,name);
						tt->strval = estrdup(0,b.strval);
						tt->allocated = b.allocated;
						tt->left = t->left;
						tt->right = t->right;
						tt->next=NULL;
						tt->count=t->count;
						tt->intval = b.intval;
						tt->douval = b.douval;
						tt->type = b.type;
						tt->flag = (inc==-2)?0:inc;
						tt->scope = mode&60;
						tt->deleted=0;
						tt->iname=estrdup(0,a.strval);
						ptt->next = tt;
						if(tt!=ptt) tt->prev = ptt;
						else tt->prev = (VarTree *)-1;
						t->count++;
						t->lastnode = tt;
						if(var && whole_array && var->next && array_cnt) {
							copyarray(tt,var->next,t,0);
						}
					}	
					done=1;
					break;
				} else done=1; 
			} else {
				i++;
				continue;	
			}
		}
	}
	if(!done && !(mode&32)) {
		if(!var_top) {
			var_top=emalloc(0,sizeof(VarTree));
			t = var_top;
			if(fs_top && !fs_top->frame) {
				fs_top->frame = t;
			}
		} else if(*(s+oi) < *(ot->name+oi)) {
			ot->left = emalloc(0,sizeof(VarTree));
			t=ot->left;
		} else {
			ot->right = emalloc(0,sizeof(VarTree));
			t=ot->right;
		}
		t->left=NULL;
		t->right=NULL;
		t->next=NULL;
		t->prev=(VarTree *)-1;
		t->lacc = (VarTree *)-1;
		t->count=1;
		t->deleted = 0;
		t->lastnode = t;
		t->intval = b.intval;
		t->douval = b.douval;
		t->strval = estrdup(0,b.strval);
		t->allocated = b.allocated;
		t->name = estrdup(0,s);
		t->iname=NULL;
		t->type = b.type;
		t->flag = (inc==-2)?0:inc;
		t->scope = mode&28;
		if(!t->iname && var && whole_array && (mode==0 || mode==1)) {
			if(var->iname) t->iname=estrdup(0,var->iname);
		} 
		if(!t->iname && (mode==0 || mode==1)) t->iname=estrdup(0,"0");
		else if(!t->iname && mode&2) {
			t->iname=estrdup(0,a.strval);
		}
		if(var && whole_array && var->next && array_cnt) {
			copyarray(t,var->next,t,0);
		}
		if(mode&8 && fs_top) { /* Creating static var in function frame */
			var_top = GetFuncFrame();
			if(!GetVar(name,0,0)) {
				if(mode&2) Push(a.strval,a.type);
				Push(b.strval,b.type);
				SetVar(name,mode-8,inc);
			}
			var_top = fs_top->frame;
		}
	} else if(mode&32) { /* create var in super-global frame */
		if(!var_supertop) {
			var_supertop=emalloc(0,sizeof(VarTree));
			t = var_supertop;
		} else if(*(s+oi) < *(ot->name+oi)) {
			ot->left = emalloc(0,sizeof(VarTree));
			t=ot->left;
		} else {
			ot->right = emalloc(0,sizeof(VarTree));
			t=ot->right;
		}
		t->left=NULL;
		t->right=NULL;
		t->next=NULL;
		t->prev=(VarTree *)-1;
		t->lacc = (VarTree *)-1;
		t->count=1;
		t->lastnode = t;
		t->intval = b.intval;
		t->douval = b.douval;
		t->strval = estrdup(0,b.strval);
		t->allocated = b.allocated;
		t->name = estrdup(0,s);
		t->iname=NULL;
		t->type = b.type;
		t->flag = (inc==-2)?0:inc;
		t->scope = mode&60;
		t->deleted = 0;
		if(!t->iname && var && whole_array && (mode==0 || mode==1)) {
			if(var->iname) t->iname=estrdup(0,var->iname);
		} 
		if(!t->iname && (mode==0 || mode==1)) t->iname=estrdup(0,"0");
		else if(!t->iname && mode&2) {
			t->iname=estrdup(0,a.strval);
		}
		if(var && whole_array && var->next && array_cnt) {
			copyarray(t,var->next,t,0);
		}
	}
}

/* Looks for a variable in the current frame.  If located variable is
 * a global placeholder, then the global frame is checked.  If the
 * located variable is a static variable placeholder, then the current
 * function's static variable frame is checked.
 */
VarTree *GetVar(char *name, char *index, int mode) {
	VarTree *t=NULL, *tt;
	VarTree *env=NULL;
	static char temp[128];
	char *s=NULL, *ss, *sss, *ma=NULL;
	int i=0, ind=0, frames=0;
	char o='\0';
	VarTree *new_name;
#if APACHE
	array_header *env_arr;
	table_entry *tenv;
#endif

	if(*name==VAR_INIT_CHAR) {
		new_name = GetVar(name+1,NULL,0);
		if(new_name) {
			s=new_name->strval;
		} else {
			return(NULL);
		}
	} else s=name;

	/* First check super-global stack frame */

	for(frames=0; frames<2; frames++) {	
		if(frames==0) t=var_supertop;
		else if(frames==1) t=var_top;
		while(t) {
			if( *(s+i) < *(t->name+i)) {
				t=t->left; i=0;
			} else if( *(s+i) > *(t->name+i)) {
				t=t->right; i=0;
			} else if( *(s+i) == *(t->name+i)) {
				if(i==0 && !strcmp(s,t->name)) {
					if(t->flag==-2) return(NULL);  /* GET method var accessed in secure mode */
					if(t->deleted && t->count==0) return(NULL);
					if(t->scope&4 && fs_top && var_top != var_main) { /* Global variable placeholder */
						var_top = var_main; /* switch to global frame */
						t = GetVar(name,index,mode);
						var_top = fs_top->frame; /* switch back to stack frame */
						return(t);
					} else if(t->scope&8 && fs_top && var_top != var_main) {
						var_top = GetFuncFrame(); /* switch to global frame */
						t = GetVar(name,index,mode);
						var_top = fs_top->frame; /* switch back to stack frame */
						return(t);
					}
					if(!index && !mode) {
						while(t && t->deleted) t=t->next;
						return(t);
					}
					else if(index) {
						tt = t;
						while(tt) {
							if(!strcmp(tt->iname,index)) {
/*								tt->count = t->count;  */
								if(tt->deleted) return(NULL);
								return(tt);
							}
							tt=tt->next;
						}
						return(NULL);
					} else {
						if(t->lacc == (VarTree *)-1) { 
							t->lacc=t;
							while(t && t->deleted) t=t->next;
							return(t);
						} else if(t->lacc==NULL) return(NULL);
						tt = t->lacc->next;
						t->lacc = tt;
						while(tt && tt->deleted) tt=tt->next;
						return(tt);
					}						
				} else {
					i++;
					continue;
				}
			} /* else if */
		} /* while */
	} /* for */

	/* If not a local variable, perhaps it is an environment variable? */
	/* For the Apache module we need to check the sub-process environment
	   space first */
#if APACHE
	if(strcasecmp(name,"SCRIPT_NAME")) {
		if(!strcasecmp(name,"PATH_TRANSLATED"))
			s = table_get(php_rqst->subprocess_env,"SCRIPT_FILENAME");
		else s = table_get(php_rqst->subprocess_env,name);
		if(s) {
			env = emalloc(2,sizeof(VarTree));
			env->strval = NULL;
			env->allocated = 0;
			env->name = NULL;
			env->iname = NULL;
			env->intval = atol(s);
			env->douval = atof(s);
			env->strval = estrdup(2,s);
			env->allocated = strlen(s)+1;
			env->name = estrdup(2,name);
			env->type = STRING;
			env->next = NULL;
			env->prev = (VarTree *)-1;
			env->left = NULL;
			env->right = NULL;
			env->lacc = NULL;
			env->lastnode = env;
			return(env);		
		}
	} else s=NULL;
	if(!strcasecmp(name,"PATH_INFO") || !strcasecmp(name,"PHP_SELF")) s=php_rqst->uri;
	else s = getenv(name);
#ifdef VIRTUAL_PATH
	if(!strcasecmp(name,"SCRIPT_NAME")) s=VIRTUAL_PATH;
	else if(!s)  s = getenv(name);
#else
	if(!s) s = getenv(name);
#endif
#else
#ifdef VIRTUAL_PATH
	if(!strcasecmp(name,"SCRIPT_NAME")) s=VIRTUAL_PATH;
	else s = getenv(name);
#else
	s = getenv(name);
#endif
	if(!strcasecmp(name,"PHP_SELF")) {
		{
			char *sn, *pi;
			sn = getenv("SCRIPT_NAME");
			pi = getenv("PATH_INFO");
			if(sn && pi && !strcmp(sn,pi)) {
				pi = NULL;
			}
			sprintf(temp,"%s%s",sn?sn:"",pi?pi:"");
			s = temp;
		}
	}
#endif
	if(s) {
		env = emalloc(2,sizeof(VarTree));
		env->strval = NULL;
		env->iname = NULL;
		env->next = NULL;
		env->prev = (VarTree *)-1;
		env->left = NULL;
		env->right = NULL;
		env->lacc = NULL;
		env->lastnode = env;
		env->intval = atol(s);
		env->douval = atof(s);
		env->strval = estrdup(2,s);
		env->allocated = strlen(s) + 1;
		env->name = estrdup(2,name);
		env->type = STRING;
		return(env);		
	}
	/* then perhaps it is a command line (GET method) argument? */
	if(!strcasecmp(name,"argc")) {
#if APACHE
		s = php_rqst->args;
#else
		s = getenv("QUERY_STRING");
#endif
		i = 0;
		ss=s;
		while(ss) {
			sss = strchr(ss,'+');
			if(sss || *ss) i++;
			if(sss) ss = sss+1;
			else ss=sss;
		}
		env = emalloc(2,sizeof(VarTree));
		env->strval = NULL;
		env->iname = NULL;
		env->next = NULL;
		env->prev = (VarTree *)-1;
		env->left = NULL;
		env->right = NULL;
		env->lacc = NULL;
		env->lastnode = env;
		env->intval = i;
		env->douval = i;
		sprintf(temp,"%d",i);
		env->strval = estrdup(2,temp);;
		env->allocated = strlen(temp) + 1;
		env->name = estrdup(2,name);
		env->type = LNUMBER;
		return(env);		
	} else if(!strcasecmp(name,"argv")) {
#if APACHE
		s = php_rqst->args;
#else
		s = getenv("QUERY_STRING");
#endif
		i = 0;
		if(index) ind = atoi(index);
		sss=s;
		ss=s;
		ma=ss;
		while(ss) {
			ma = ss;
			sss = strchr(ss,'+');
			if(sss) ss = sss+1;
			else ss=sss;
			if(ind==i) break;
			i++;	
		}	
		if(sss) {
			o=*sss;
			*sss='\0';
		}
		env = emalloc(2,sizeof(VarTree));
		env->iname = NULL;
		env->next = NULL;
		env->prev = (VarTree *)-1;
		env->left = NULL;
		env->right = NULL;
		env->lacc = NULL;
		env->lastnode = env;
		if(ma) env->intval = atol(ma);
		else env->intval=0;
		if(ma) env->douval = atof(ma);
		else env->douval=0;
		if(ma) {
			env->strval = estrdup(2,ma);
			env->allocated = strlen(ma) + 1;
		} else {
			env->strval=emalloc(2,sizeof(char));
			env->allocated = 2;
			*(env->strval) = '\0';
		}
		parse_url(env->strval);
		env->name = estrdup(2,name);
		env->type = STRING;
		if(sss) *sss=o;
		return(env);		
	}
#if APACHE
	/* Ok, it could be a request header */
#if PHP_SAFE_MODE
	if(!strncasecmp(name,"req_authorization", 17)) {
		Error("The PHP module is running in SAFE MODE.  You may not access $req_authorization in this mode.");
		return(NULL);
	}
#endif
	if(!strncasecmp(name,"req_",4)) {
		env_arr = table_elts(php_rqst->headers_in);
		tenv = (table_entry *)env_arr->elts;
		for(i = 0; i < env_arr->nelts; ++i) {
			if(!tenv[i].key) continue;
			strncpy(temp,tenv[i].key,127);
			s = temp;
			while(*s) {  /* Map - to _ */
				if(*s=='-') *s='_';
				s++;
			}	
			if(!strcasecmp(name+4,temp)) {
				env = emalloc(2,sizeof(VarTree));
				env->strval = NULL;
				env->iname = NULL;
				env->next = NULL;
				env->prev = (VarTree *)-1;
				env->left = NULL;
				env->right = NULL;
				env->lacc = NULL;
				env->lastnode = env;
				env->intval = atol(tenv[i].val);
				env->douval = atof(tenv[i].val);
				env->strval = estrdup(2,tenv[i].val);
				env->allocated = strlen(tenv[i].val) + 1;
				env->name = estrdup(2,name);
				env->type = STRING;
				return(env);		
			}
		}
	}
#endif
	return(NULL);
}

/* Returns true (1) if variable is defined */
void IsSet(char *name, int index) {
	VarTree *t;
	Stack *s=NULL;

	if(index) {
		s = Pop();
		if(!s) {
			Error("Stack Error in IsSet");
			return;
		}
#if DEBUG
		Debug("Looking up %s[%s]\n",name,s->strval);
#endif
		t = GetVar(name,s->strval,0);
	} else {
		t = GetVar(name,NULL,0);
	}
	if(!t || (t && !t->allocated)) {
		Push("0",LNUMBER);
	} else {
		Push("1",LNUMBER);
	}
}

/* Move array pointer to the beginning of array */
void Reset(char *name) {
	VarTree *t, *tt;

	t = GetVar(name,NULL,0);
	if(t && t->allocated) {
		t->lacc = (VarTree *)-1;
		tt=t;
		while(tt && tt->deleted) tt=tt->next;
		if(tt) Push(t->strval,t->type);	
		else Push("",STRING);
	} else Push("",STRING);
}

void End(char *name) {
	VarTree *t,*tt;

	t = GetVar(name,NULL,0);
	if(t && t->allocated) {
		t->lacc = t->lastnode->prev;
		tt=t->lastnode;
		while(tt && tt->deleted) tt=tt->prev;
		if(tt) Push(tt->strval,t->lastnode->type);
		else Push("",STRING);
	} else Push("",STRING);
}

/* Return index of current array node */
void Key(char *name) {
	VarTree *t, *tt;

	t = GetVar(name,NULL,0);
	if(t && t->allocated) {
		if(t->lacc != (VarTree *)-1 && t->lacc != NULL) tt=t->lacc->next;
		else tt=t;
		while(tt && tt->deleted) tt=tt->next;
		if(tt) Push(tt->iname,STRING);
		else Push("",STRING);
	} else Push("",STRING);
}

/* Move array pointer forward one step and return the value */
void Next(char *name) {
	VarTree *t, *tt;

	t = GetVar(name,NULL,0);
	if(t && t->allocated) {
		if(t->lacc == (VarTree *)-1) tt=t;	
		else if(t->lacc == NULL) tt=NULL;
		else tt=t->lacc->next;
		while(tt && tt->deleted) tt=tt->next;	
		if(!tt) t->lacc = t->lastnode;
		else t->lacc = tt;
		if(tt) Push(tt->strval,tt->type);
		else Push("",STRING);
	} else Push("",STRING);
}

/* Move array pointer back one step and return the value */
void Prev(char *name) {
	VarTree *t, *tt;

	t = GetVar(name,NULL,0);
	if(t && t->allocated) {
		if(t->lacc == (VarTree *)-1) tt=(VarTree *)-1;	
		else if(t->lacc == NULL) tt=t->lastnode;
		else tt=t->lacc;
		while(tt!=(VarTree *)-1 && tt && tt->deleted) tt=tt->prev;	
		if(!tt || tt==(VarTree *)-1) {
			t->lacc = (VarTree *)-1;
		} else {
			t->lacc = tt->prev;
		}
		if(tt!=NULL && tt!=(VarTree *)-1) Push(tt->strval,tt->type);
		else Push("",STRING);
	} else Push("",STRING);
}

/* Perform variable substitution in the passed string */
char *SubVar(char *string) {
	char *allocated, *s, *t, o='\0', *ind, *inde, *rind=NULL;
	VarTree *var, *var2=NULL;
	char *ret, *sret;
	int l, ll, mode=0, cz=0;
	char buf[64];

	if(!strchr(string,VAR_INIT_CHAR)) return(estrdup(2,string));
	l = strlen(string) + 1024;	
	allocated = emalloc(2,l);
	ret = allocated;
	*ret = '\0';
	sret = ret;
	s = string;
	while(*s) {
		if(ret-sret > l-1) {
			sret = emalloc(2,2*l);
			l = 2*l;
			strcpy(sret,allocated);
			allocated=sret;
			ret = sret + strlen(sret);
		}
		if(*s==VAR_INIT_CHAR) {
			if(s>string && *(s-1)=='\\') {
				*ret++ = VAR_INIT_CHAR;
				*ret = '\0';
				s++;
				continue;
			}
			t = s++;
			if(!*s) {
				*ret++ = VAR_INIT_CHAR;
				*ret = '\0';
				continue;
			}
			while(isalnum(*s) || *s == '_' || (*s==VAR_INIT_CHAR&&s==t+1)) s++;
			ind = s;
			while(*ind == ' ' || *ind == '\t' || *ind == '\n') ind++;
			if(*ind == '[') {
				inde=ind+1;
				while(isalnum(*inde) || *inde=='_' || *inde=='[' || *inde==VAR_INIT_CHAR) inde++;
				if(*inde==']') {
					s = inde+1;					
					*ind='\0';
					*inde='\0';
					o='\0';
					if(strchr(ind+1,'[')) {
						*inde=']';
						o = *(inde+1);
						*(inde+1)='\0';	
						s = inde+2;					
					}
					ind = ind+1;
					if(strchr(ind,VAR_INIT_CHAR)) {
						rind = SubVar(ind);	
						ind = rind;
					}
					if(o) *(inde+1) = o;
					ll = strlen(ind);
					if(ll==0) {
						mode=1;
						if(rind) rind=NULL;
						ind=NULL;
					} else {
						if(ind[ll-1]==']') {
							ind[ll-1]='\0';
						}
					}
				} else ind=NULL;
			} else ind=NULL;
			o = *s;
			*s = '\0';
			if(*(t+1) == VAR_INIT_CHAR) {
				if(ind) while(*(ind+cz)=='0') cz++;	
				if(ind && cz && CheckType(ind+cz)==LNUMBER) {
					sprintf(buf,"%ld",atol(ind));
					var2 = GetVar(t+2,buf,mode);
				} else var2 = GetVar(t+2,ind,mode);
				if(var2 && var2->strval) {
					var = GetVar(var2->strval,ind,mode);
				} else var=NULL;
			} else {
				if(ind) while(*(ind+cz)=='0') cz++;	
				if(ind && cz && CheckType(ind+cz)==LNUMBER) {
					sprintf(buf,"%ld",atol(ind));
					var = GetVar(t+1,buf,mode);
				} else var = GetVar(t+1,ind,mode);
			}
			if(rind) rind=NULL;
			if(var && var->allocated) {
				ll = strlen(var->strval);
				if(ret-sret > l-1-ll) {
					sret = emalloc(2,2*l+ll);
					l = 2*l+ll;
					strcpy(sret,allocated);
					allocated=sret;
					ret = sret + strlen(sret);
				}
				strcat(ret,var->strval);
				ret+=ll;
			}
			*s = o;
			continue;
		}
		*ret++ = *s++;
		*ret = '\0';
	}
	return(sret);
}

/* Return the number of elements in the array name on the stack */
void Count(void) {
	Stack *s;
	VarTree *t;	
	char temp[8];

	s = Pop();
	if(!s) {
		Error("Stack error in count");
		return;
	}

 	t = s->var;	
	if(t) {
		while(t->prev!=(VarTree *)-1) {
			t = t->prev;
		}
	}
	if(t && t->allocated && !t->deleted) sprintf(temp,"%d",t->count);	
	else strcpy(temp,"0");
	Push(temp,LNUMBER);
}

/* Return the position of an array element */
void Pos(void) {
	Stack *s;
	VarTree *t;	
	char temp[16];

	s = Pop();
	if(!s) {
		Error("Stack error in count");
		return;
	}

 	t = s->var;	
	if(t && t->allocated) {
		if(t->prev!=(VarTree *)-1) {
			sprintf(temp,"%d",t->count);	
		} else {
			sprintf(temp,"%d",0);	
		}
	} else strcpy(temp,"-1");
	Push(temp,LNUMBER);
}

/* Return the largest element from a array */
void ArrayMax(void) {
	Stack *s;
	VarTree *t,*tt, *max;

	s = Pop();
	if(!s) {
		Error("Stack error in max");
		return;
	}
	t = s->var;
	if(t && t->allocated) {
		max=t;
		tt=t->next;
		while(tt) {
			if(!tt->deleted) {
				switch(tt->type) {
				case STRING:
					if(strlen(tt->strval) > strlen(max->strval)) max=tt;
					break;
				case DNUMBER:
					if(tt->douval > max->douval) max=tt;
					break;
				case LNUMBER:
					if(tt->intval > max->intval) max=tt;
					break;
				}
			}
			tt=tt->next;
		}
		switch(max->type) {
			case STRING:
				Push(max->strval,STRING);
				break;
			case DNUMBER:
				Push(max->strval,DNUMBER);
				break;
			case LNUMBER:
				Push(max->strval,LNUMBER);
				break;	
		}
	} else {
		switch(s->type) {
		case STRING:
			Push(s->strval,STRING);
			break;
		case LNUMBER:
			Push(s->strval,LNUMBER);
			break;
		case DNUMBER:
			Push(s->strval,LNUMBER);
			break;
		}
	}
}

/* Return the smallest element from an array */
void ArrayMin(void) {
	Stack *s;
	VarTree *t,*tt, *min;

	s = Pop();
	if(!s) {
		Error("Stack error in min");
		return;
	}
	t = s->var;
	if(t && t->allocated) {
		min=t;
		tt=t->next;
		while(tt) {
			if(!tt->deleted) {
				switch(tt->type) {
				case STRING:
					if(strlen(tt->strval) < strlen(min->strval)) min=tt;
					break;
				case DNUMBER:
					if(tt->douval < min->douval) min=tt;
					break;
				case LNUMBER:
					if(tt->intval < min->intval) min=tt;
					break;
				}
			}
			tt=tt->next;
		}
		switch(min->type) {
			case STRING:
				Push(min->strval,STRING);
				break;
			case DNUMBER:
				Push(min->strval,DNUMBER);
				break;
			case LNUMBER:
				Push(min->strval,LNUMBER);
				break;	
		}
	} else {
		switch(s->type) {
		case STRING:
			Push(s->strval,STRING);
			break;
		case LNUMBER:
			Push(s->strval,LNUMBER);
			break;
		case DNUMBER:
			Push(s->strval,LNUMBER);
			break;
		}
	}
}

/* 
 * Put a string into the environment space
 */
void PutEnv(void) {
	Stack *s;
	int ret;
	char *env;

	s = Pop();
	if(!s) {
		Error("Stack error in putenv");
		return;
	}
	if(s->strval && *(s->strval)) {
		/* You can't do putenv on memory that is going to go away! */
		/* The memory must be allocated as long as the process is around! */
		/* (Yes this is a memory leak, but really, that's how PutEnv works.) */
		env = malloc(strlen(s->strval)+1);
		strcpy(env, s->strval);
		ret = putenv(env);
		if(ret) {
			Error("putenv failed");
		}
	}
}

/* 
 * Get a string from the environment space
 * If running as an Apache module, check subprocess
 * environment space first
 */
void GetEnv(void) {
	Stack *s;
	char *ret;

	s = Pop();
	if(!s) {
		Error("Stack error in getenv");
		return;
	}
#if APACHE
	ret = table_get(php_rqst->subprocess_env,s->strval);
	if(ret) {
		Push(ret,STRING);
		return;
	}
#endif
	if(s->strval && *(s->strval)) {
		ret = getenv(s->strval);
		if(!ret) {
			Push("",STRING);
			Error("getenv failed");
		} else {
			Push(ret,STRING);
		}
	}
}

void PtrPush(void *ptr) {
	PtrStack *new;

	new = emalloc(0,sizeof(PtrStack));
	new->ptr = ptr;
	new->next = ptr_top;
	ptr_top = new;
}

void *PtrPop(void) {
	void *ptr;
	PtrStack *tmp;

	if(ptr_top) {
		ptr = ptr_top->ptr;
		tmp = ptr_top->next;
		ptr_top = tmp;	
		return(ptr);
	} else return(NULL);
}
	
void SecureVar(void) {
	Stack *s;
	VarTree *v;
    regex_t re;
    regmatch_t subs[1];
    char erbuf[150];
    int err, len;
 
	s = Pop();
	if(!s) {
		Error("Stack error in securevar");
		return;
	}
	if(!*(s->strval)) return;

	err = regcomp(&re, s->strval, REG_EXTENDED | REG_NOSUB);
	if(err) {
		len = regerror(err, &re, erbuf, sizeof(erbuf));
		Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
		return;
	}
	
	/* inorder traversal of binary symbol tree */
	v = var_top;
	while(v) {
		if(v->left) {
			PtrPush(v);
			v = v->left;
			continue;
		} else {
			err = regexec(&re, v->name, (size_t)1, subs, 0);
			if(err && err!=REG_NOMATCH) {
				len = regerror(err, &re, erbuf, sizeof(erbuf));
				Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
				regfree(&re);
				return;
			}
			if(err==0) {
				if(v->flag == -1) {
					v->flag = -2;
#if DEBUG
					Debug("Variable %s marked inaccessible\n",v->name);
#endif
				}
			}
right:
			if(v->right) {
				v = v->right;	
				continue;
			}
			v = PtrPop();
			if(!v) break;
			err = regexec(&re, v->name, (size_t)1, subs, 0);
			if(err && err!=REG_NOMATCH) {
				len = regerror(err, &re, erbuf, sizeof(erbuf));
				Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
				regfree(&re);
				return;
			}
			if(err==0) {
				if(v->flag == -1) {
					v->flag = -2;
#if DEBUG
					Debug("Variable %s marked inaccessible\n",v->name);
#endif
				}
			}
			while(!v->right) {
				v=PtrPop();	
				if(!v) goto end;	
				err = regexec(&re, v->name, (size_t)1, subs, 0);
				if(err && err!=REG_NOMATCH) {
					len = regerror(err, &re, erbuf, sizeof(erbuf));
					Error("Regex error %s, %d/%d `%s'\n", reg_eprint(err), len, sizeof(erbuf), erbuf);
					regfree(&re);
					return;
				}
				if(err==0) {
					if(v->flag == -1) {
						v->flag = -2;
#if DEBUG
						Debug("Variable %s marked inaccessible\n",v->name);
#endif
					}
				}
			}
			goto right;
		}	
	}
end: ;
	regfree(&re);
}

/* Create a new stack frame and make it the current one */
void PushStackFrame(void) {
	FrameStack *new;

	new = emalloc(0,sizeof(FrameStack));
	if(fs_top == NULL) var_main = var_top;
	new->next = fs_top;
	new->frame = NULL;	
	fs_top = new;
	var_top = NULL;
}

/* Remove current stack frame and set new current frame */
void PopStackFrame(void) {
	FrameStack *old;

	if(fs_top) {
		old = fs_top;
		fs_top = fs_top->next;
/* ??	if(fs_top == NULL || (fs_top && fs_top->frame==NULL)) var_top = var_main; ?? */
		if(fs_top == NULL) var_top = var_main;
		else var_top = fs_top->frame;
	}
}

/* Create global stub in current stack frame for each argument in the arg list */
void Global(void) {
	VarTree *var;
	FuncArgList *args;
	char def_val[2] = {'0','\0'};

	/* Only valid for frames on the stack (ie. inside functions) */
	if(fs_top) {
		args = GetFuncArgList();
		while(args) {
			var = GetVar(args->arg,NULL,0);
			if(!var || (var && !var->allocated)) { /* if it doesn't exist, create it */
				Push(def_val,LNUMBER);
				SetVar(args->arg,4,0); 
			}
			args=args->next;
		}
		ClearFuncArgList();
	}
}

/* 
 * Copies an array from svar to dvar starting at the second element
 */
void copyarray(VarTree *dvar, VarTree *svar, VarTree *dtop, int renum) {
	VarTree *new=NULL,*s,*d;
	int count;
	int lname,lstrval,liname;
	char newnum[32];

	s = svar;
	d = dvar;
	count = dtop->count;
	while(s && d) {
		if(!s->deleted) {
		if(renum==2) {
			new = GetVar(d->name,s->iname,0);
			if(new) {
				if(strlen(s->strval)>(unsigned int)new->allocated) {
					new->strval = estrdup(0,s->strval);
					new->allocated = strlen(s->strval) + 1;
				} else {
					strcpy(new->strval,s->strval);
				}
				new->intval = s->intval;
				new->douval = s->douval;
				new->type = s->type;
				new->deleted = s->deleted;
				d = d->next;
				s = s->next;	
				new=d;
				continue;
			}
		}
		count++;
		if(!d->next) {
			new = emalloc(0,sizeof(VarTree));
			lname=0;
			lstrval=0;
			liname=0;
			memcpy(new,s,sizeof(VarTree));
			new->next = NULL;
		} else {
			new = d->next;
			lname = strlen(new->name);
			lstrval = new->allocated-1;
			liname = strlen(new->iname);
		}
		if(lname==0 || strlen(s->name)>(unsigned int)lname) {
			new->name = estrdup(0,s->name);
		} else {
			strcpy(new->name,s->name);
		}
		if(lstrval==0 || strlen(s->strval)>(unsigned int)lstrval) {
			new->strval = estrdup(0,s->strval);
			new->allocated = strlen(s->strval) + 1;
		} else {
			strcpy(new->strval,s->strval);
			new->allocated = lstrval+1;
		}
		new->intval = s->intval;
		new->douval = s->douval;
		new->count = s->count;
		if(renum==0) {
			if(liname==0 || strlen(s->iname)>(unsigned int)liname) {
				new->iname = estrdup(0,s->iname);
			} else {
				strcpy(new->iname,s->iname);
			}
		} else if(renum==1) {
			sprintf(newnum,"%d",count-1);
			if(liname==0 || strlen(newnum)>(unsigned int)liname) {
				new->iname = estrdup(0,newnum);
			} else {
				strcpy(new->iname,newnum);
			}
		}	
		new->deleted=s->deleted;
		new->prev = d;
		new->left = NULL;
		new->right = NULL;
		new->lacc = (VarTree *)-1;
		new->lastnode = new;
		new->type = s->type;
		new->flag = s->flag;
		d->next = new;
		d = d->next;
		}
		s = s->next;	
	}
	dtop->count = count;
	if(new) dtop->lastnode = new;
}

/* deletes an array
 *
 * It would be nice if this function would return these
 * VarTree pointers to some sort of buffer where they can
 * be re-used.  Or, if we had some sort of garbage collection,
 * the fact that we dereference them here would be enough.
 *
 * At the moment the array elements get marked as deleted, so
 * if the script decides to re-use this same array, at least the
 * memory will be re-used.
 */
void deletearray(VarTree *old) {
	VarTree *o;
#if DEBUG
	Debug("deletearray called for %s\n",old->name);
#endif
	if(!old) return;
	o = old->next;
	while(o) {
		o->deleted=1;
		o = o->next;	
	}
	old->count = 0;
	old->prev=(VarTree *)-1;
	old->lacc = (VarTree *)-1;
	old->deleted=1;
	old->lastnode=old;
}

void UnSet(char *name, int index) {
	VarTree *var, *firstvar, vartemp;
	Stack *s=NULL;

	if(index) {
		s = Pop();
		if(!s) {
			Error("Stack Error in UnSet");
			return;
		}
		var = GetVar(name,s->strval,0);
		if(var) {
			firstvar = GetVar(name,NULL,0);
			if(firstvar) {
				firstvar->count--;
				if(firstvar==var) {	/* check if it is the first item in an array */
					var = var->next;
					while(var) { /* find the first undeleted item */
						if(!(var->deleted)) break;
						var = var->next;
					}
					if(var) {
						memcpy(&vartemp,var,sizeof(VarTree));	/* grab a copy of other var */
						/* switch the two */
						memcpy(var,firstvar,sizeof(VarTree));
						memcpy(firstvar,&vartemp,sizeof(VarTree));	

						/* now fix the links */
						firstvar->lacc   = (VarTree *)-1;
						var->prev = firstvar->prev;
						firstvar->prev = (VarTree *)-1;
						var->next   = firstvar->next;
						firstvar->next = vartemp.next;

						/* And we want the count to be accurate on the new first node */
						firstvar->count = var->count;

						/* lastnode needs to be set accurately as well */
						firstvar->lastnode = var->lastnode;

						/* Delete the switched item */
					}
				}
			}
			var->deleted = 1;	
		}
	} else {
		var = GetVar(name,NULL,0);
		if(var && var->allocated) {
			deletearray(var);
		}
	}
}
