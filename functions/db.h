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
   |          Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id: db.h,v 1.15 1998/01/15 22:01:07 jim Exp $ */

#ifndef _PHP3_DB_H
#define _PHP3_DB_H

#ifndef DLEXPORT
#define DLEXPORT
#endif


typedef struct dbm_info {
        char *filename;
        char *lockfn;
        int lockfd;
        void *dbf;
} dbm_info;

extern php3_module_entry dbm_module_entry;
#define dbm_module_ptr &dbm_module_entry

/*
  we're not going to bother with flatfile on win32
  because the dbm module will be external, and we
  do not want flatfile compiled staticly
*/
#if defined(MSVC5) && !defined(COMPILE_DL)
#undef dbm_module_ptr
#define dbm_module_ptr NULL
#endif

DLEXPORT dbm_info *_php3_finddbm(YYSTYPE *id,HashTable *list);
DLEXPORT int _php3_dbmclose(dbm_info *info);
DLEXPORT dbm_info *_php3_dbmopen(char *filename, char *mode);
DLEXPORT int _php3_dbminsert(dbm_info *info, char *key, char *value);
DLEXPORT char *_php3_dbmfetch(dbm_info *info, char *key);
DLEXPORT int _php3_dbmreplace(dbm_info *info, char *key, char *value);
DLEXPORT int _php3_dbmexists(dbm_info *info, char *key);
DLEXPORT int _php3_dbmdelete(dbm_info *info, char *key);
DLEXPORT char *_php3_dbmfirstkey(dbm_info *info);
DLEXPORT char *_php3_dbmnextkey(dbm_info *info, char *key);

/* db file functions */
extern DLEXPORT int php3_minit_db(INITFUNCARG);
extern DLEXPORT int php3_rinit_db(INITFUNCARG);
extern DLEXPORT void php3_info_db(void);
extern DLEXPORT void php3_dblist(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_dbmopen(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_dbmclose(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_dbminsert(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_dbmfetch(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_dbmreplace(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_dbmexists(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_dbmdelete(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_dbmfirstkey(INTERNAL_FUNCTION_PARAMETERS);
extern DLEXPORT void php3_dbmnextkey(INTERNAL_FUNCTION_PARAMETERS);

#endif /* _PHP3_DB_H */
