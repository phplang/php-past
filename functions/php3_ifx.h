/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Danny Heijl <Danny.Heijl@cevi.be>                           |
   | based on mysql code by: Zeev Suraski <bourbon@netvision.net.il>      |
   +----------------------------------------------------------------------+
 */



#ifndef _PHP3_IFX_H
#define _PHP3_IFX_H

#if COMPILE_DL
#undef HAVE_IFX
#define HAVE_IFX 1
#endif

#if HAVE_IFX
#ifndef DLEXPORT
#define DLEXPORT
#endif

#include "locator.h"

extern php3_module_entry ifx_module_entry;
#define ifx_module_ptr &ifx_module_entry

extern int php3_minit_ifx(INIT_FUNC_ARGS);
extern int php3_rinit_ifx(INIT_FUNC_ARGS);
extern int php3_mshutdown_ifx(void);
extern void php3_info_ifx(void);
extern void php3_ifx_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_pconnect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_create_db(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_drop_db(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_query(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_error(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_affected_rows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_num_rows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_num_fields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_fetch_row(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_fetch_object(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_free_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_htmltbl_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_fieldtypes(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_fieldproperties(INTERNAL_FUNCTION_PARAMETERS);


typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	int le_result,le_link,le_plink;
	char *default_host, *default_user, *default_password;
	int connectionid;
	int cursorid;
} ifx_module;

#define MAXBLOBS          64
#define INITIAL_BLOBSIZE  4096

typedef struct ifx_res {
	char connecid[16];
	char cursorid[16];
	char descrpid[16];
	char statemid[16];
	int  numcols;
	int  rowid;
	loc_t *blob_data[MAXBLOBS];
} IFX_RES;

#ifndef THREAD_SAFE
extern ifx_module php3_ifx_module;
#endif

#else

#define ifx_module_ptr NULL

#endif

#endif /* _PHP3_IFX_H */
