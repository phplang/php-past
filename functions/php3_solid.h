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
   | Authors: Jeroen van der Most <jvdmost@digiface.nl>                   |
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */

#ifndef _PHP3_SOLID_H
#define _PHP3_SOLID_H

#if COMPILE_DL
#undef HAVE_SOLID
#define HAVE_SOLID 1
#endif

#if HAVE_SOLID && !HAVE_UODBC

#if MSVC5 /* Windows */
# include <windows.h>
# include <sql.h>
# include <sqlext.h>
#else /* UNIX */
# if HAVE_IODBC /* iODBC */
#  include <isql.h>
#  include <isqlext.h>
#  include <odbc_types.h>
#  include <odbc_funcs.h>
# else /* Solid */
#  include <cli0core.h>
#  include <cli0ext1.h>
# endif
#endif

#ifndef MSVC5
# define FAR
#endif

typedef struct SQLConn {
	HDBC            hdbc;
	int             index;
} SQLConn;

typedef struct {
	char            name[32];
	char           *value;
	long int        vallen;
} SQLResultValue;

typedef struct SQLResult {
	HSTMT           stmt;
	SQLResultValue *values;
	SQLConn        *conn;
	int             numcols;
	int             fetched;
	int             index;
} SQLResult;

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	int le_result,le_link,le_plink;
} solid_module;

extern php3_module_entry solid_module_entry;
#define solid_module_ptr &solid_module_entry

/* solid.c functions */
extern int php3_minit_solid(INITFUNCARG);
extern int php3_shutdown_solid(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_closeall(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_exec(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_fetchrow(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_freeresult(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_numrows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_numfields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_fieldname(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_solid_fieldnum(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_info_solid(void);

extern solid_module php3_solid_module;

#else

#define solid_module_ptr NULL

#endif /* HAVE_SOLID */
#endif /* _PHP3_SOLID_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
