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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
 */
/* $Id $ */
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if MSVC5
#include <windows.h>
#include <winsock.h>
#define O_RDONLY _O_RDONLY
#include "win32/param.h"
#else
#include <sys/param.h>
#endif
#include "head.h"
#include "internal_functions.h"
#include "safe_mode.h"
#include "list.h"
#include "php3_string.h"
#include "pack.h"
#if HAVE_PWD_H
#if MSVC5
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#include "snprintf.h"
#include "fsock.h"
#if HAVE_NETINET_IN_H
#include "netinet/in.h"
#endif

function_entry pack_functions[] = {
	{"pack",		php3_pack,	NULL},
	{"unpack",		php3_unpack,		NULL},
	{NULL, NULL, NULL}
};

php3_module_entry pack_module_entry = {
	"PHP_pack", pack_functions, php3_minit_pack, NULL, NULL, NULL, NULL, 0, 0, 0, NULL
};

/* pack() idea stolen from Perl. Implemented formats are a,a,c,C,s,S,i,I,l,L,n,N,f,d,x,X,@.
 * 'h' is also implemented, but I'm not sure if it works the same way as Perl
 */
void php3_pack(INTERNAL_FUNCTION_PARAMETERS) {
	YYSTYPE **args;
	int argc, currarg = 1, size = 240;
	const char *format;
	char thisformat = '\0';
	char *out, *outstart;
	int repeat = 0;
	signed char c;
	unsigned char C;
	signed short s;
	unsigned short S;
	signed int i;
	unsigned int I;
	signed long l;
	unsigned long L;
	float f;
	double d;

	argc = ARG_COUNT(ht);

	if (argc < 1) {
		WRONG_PARAM_COUNT;
	}
	args = emalloc(argc * sizeof(YYSTYPE *));

	if (getParametersArray(ht, argc, args) == FAILURE) {
		efree(args);
		WRONG_PARAM_COUNT;
	}
	convert_to_string(args[0]);
	out = outstart = emalloc(size);

	/* I got tired of seeing warnings under msvc so
	is some places where this macro was used, I replaced
	it with actual code */
#define CASEFOR(vartype,var) \
	case vartype:\
		convert_to_long(args[currarg]);\
		var = args[currarg]->value.lval;\
		memcpy(out, &var, sizeof(var));\
		out += sizeof(var);\
		currarg++;\
		break;

	format = args[0]->value.strval;
	while (*format || repeat) {
		char repeatbuf[32];
		int inpos;

		/* make sure we didn't go past the end */
		if (currarg >= argc && *format != 'x' && *format != 'X' && *format != '@') {
			php3_error(E_WARNING,"pack: too few arguments");
			break;
		}
		if (repeat == 0) {
			thisformat = *(format++);
			/* grab any repeater args */
			if (*format == '*') {
				repeat = 32000;
			}
			else {
				for (repeat = 0; *format && *format >= '0' && *format <= '9'; format++, repeat++)
					repeatbuf[repeat] = *format;
				repeatbuf[repeat] = '\0';
				repeat = atoi(repeatbuf);
				if (repeat == 0)
					repeat = 1;
			}
		}
		
		/* handle the format char */
		switch (thisformat) {
			case 'A':
			case 'a':
				convert_to_string(args[currarg]);
				for (inpos = 0; inpos < repeat; inpos++)
					if (inpos >= args[currarg]->strlen)
						*(out++) = (thisformat == 'A') ? ' ' : '\0';
					else
						*(out++) = args[currarg]->value.strval[inpos];
				repeat = 0;
				currarg++;
				break;
			case 'h':
				convert_to_string(args[currarg]);
				for (inpos = 0; inpos < args[currarg]->strlen; inpos++) {
					sprintf(out, "%02x", (int)args[currarg]->value.strval[inpos]);
					out += 2;
				}
				currarg++;
				break;
			case 'c':
				convert_to_long(args[currarg]);
				c = (signed char)args[currarg]->value.lval;
				memcpy(out, &c, sizeof(c));
				out += sizeof(c);
				currarg++;
				break;
			case 'C':
				convert_to_long(args[currarg]);
				C = (unsigned char)args[currarg]->value.lval;
				memcpy(out, &C, sizeof(C));
				out += sizeof(C);
				currarg++;
				break;
			case 's':
				convert_to_long(args[currarg]);
				s = (signed short)args[currarg]->value.lval;
				memcpy(out, &s, sizeof(s));
				out += sizeof(s);
				currarg++;
				break;
			case 'S':
				convert_to_long(args[currarg]);
				S = (unsigned short)args[currarg]->value.lval;
				memcpy(out, &S, sizeof(S));
				out += sizeof(S);
				currarg++;
				break;
			CASEFOR('i', i);
			CASEFOR('I', I);
			CASEFOR('l', l);
			CASEFOR('L', L);
			case 'n':
				convert_to_long(args[currarg]);
				S = htons((unsigned short)args[currarg]->value.lval);
				memcpy(out, &S, sizeof(S));
				out += sizeof(S);
				currarg++;
				break;
			case 'N':
				convert_to_long(args[currarg]);
				L = htonl(args[currarg]->value.lval);
				memcpy(out, &L, sizeof(L));
				out += sizeof(L);
				currarg++;
				break;
			case 'f':
				convert_to_long(args[currarg]);
				f = (float)args[currarg]->value.lval;
				memcpy(out, &f, sizeof(f));
				out += sizeof(f);
				currarg++;
				break;
			CASEFOR('d', d);
			case 'x':
				*(out++) = '\0';
				break;
			case 'X':
				out--;
				break;
			case '@':
				/* repeat == the length to which we should pad */
				while (out - outstart < repeat)
					*(out++) = '\0';
				repeat = 0;
				break;
			default:
				php3_error(E_WARNING,"pack: unknown format type '%c'", *format);
				break;
		}
		if (repeat)
			repeat--;
	}
#undef CASEFOR
	
	efree(args);
	RETVAL_STRINGL(outstart, out - outstart);
	efree(outstart);

	return;
}


/* unpack() is based on Perl's unpack(), but is modified quite a bit from there.
 * Rather than depending on error-prone ordered lists or syntactically unpleasant
 * pass-by-reference, we return an object with named paramters 
 * (like *_fetch_object()). Syntax is "f[rpt]name/...", where "f" is the
 * formatter char (like pack()), "[rpt]" is the optional repeater argument,
 * and "name" is the name of the variable to use.
 * Example: "c2chars/nints" will return an object with fields chars1, chars2, and ints.
 * Numeric pack types will return numbers, a and A will return strings,
 * f and d will return
 */
void php3_unpack(INTERNAL_FUNCTION_PARAMETERS) {
	return;
}


int php3_minit_pack(INITFUNCARG)
{
	TLS_VARS;
	
	return SUCCESS;
}



/*
 * Local variables:
 * tab-width: 4
 * End:
 */
