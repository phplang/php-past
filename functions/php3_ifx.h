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
   | Authors: Danny Heijl <Danny.Heijl@cevi.be>, initial cut (ODS 7)      |
   |          Christian Cartus <chc@idgruppe.de>, blobs, and IUS 9        |
   |          Jouni Ahto <jah@guru.cultnet.fi>   : configuration stuff    |
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
#include "sqltypes.h"

extern php3_module_entry ifx_module_entry;
#define ifx_module_ptr &ifx_module_entry

extern int php3_minit_ifx(INIT_FUNC_ARGS);
extern int php3_rinit_ifx(INIT_FUNC_ARGS);
extern int php3_mshutdown_ifx(void);
extern void php3_info_ifx(void);
extern void php3_ifx_connect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_pconnect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_query(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_prepare(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_do(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_error(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_errormsg(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_affected_rows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_num_rows(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_num_fields(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_fetch_row(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_free_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_htmltbl_result(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_fieldtypes(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_fieldproperties(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_ifx_create_blob(INTERNAL_FUNCTION_PARAMETERS);
long php3_intifx_create_blob(long type, long mode, char* param, long len, HashTable *list);
extern void php3_ifx_free_blob(INTERNAL_FUNCTION_PARAMETERS) ;
long php3_intifx_free_blob(long id, HashTable *list);

long php3_intifx2_free_blob(long id, HashTable *list);

extern void php3_ifx_get_blob(INTERNAL_FUNCTION_PARAMETERS);
long php3_intifx_get_blob(long bid, HashTable *list, char** content);
extern void php3_ifx_update_blob(INTERNAL_FUNCTION_PARAMETERS);
long php3_intifx_update_blob(long bid, char* param, long len, HashTable *list);
loc_t *php3_intifx_get_blobloc(long bid, HashTable *list);
char* php3_intifx_create_tmpfile(long bid);
extern void php3_ifx_blobinfile_mode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_copy_blob(INTERNAL_FUNCTION_PARAMETERS);
long php3_intifx_copy_blob(long bid, HashTable *list);
extern void php3_ifx_textasvarchar(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_byteasvarchar(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifx_nullformat(INTERNAL_FUNCTION_PARAMETERS);
char* php3_intifx_null();

#if HAVE_IFX_IUS
extern void php3_ifxus_create_slob(INTERNAL_FUNCTION_PARAMETERS);
long php3_intifxus_create_slob(long create_mode, HashTable *list);
extern void php3_ifxus_free_slob(INTERNAL_FUNCTION_PARAMETERS) ;
long php3_intifxus_free_slob(long bid, HashTable *list);
extern void php3_ifxus_close_slob(INTERNAL_FUNCTION_PARAMETERS) ;
long php3_intifxus_close_slob(long bid, HashTable *list);
extern void php3_ifxus_open_slob(INTERNAL_FUNCTION_PARAMETERS) ;
long php3_intifxus_open_slob(long bid, long create_mode, HashTable *list);
long php3_intifxus_new_slob(HashTable *list);
ifx_lo_t *php3_intifx_get_slobloc(long bid, HashTable *list);
extern void php3_ifxus_read_slob(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifxus_write_slob(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifxus_seek_slob(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ifxus_tell_slob(INTERNAL_FUNCTION_PARAMETERS);
#endif

typedef struct {
	long default_link;
	long num_links,num_persistent;
	long max_links,max_persistent;
	long allow_persistent;
	int le_result,le_link,le_plink,le_blobresult,le_slobresult;
	char *default_host, *default_user, *default_password;
	int connectionid;
	int cursorid;
        int sv_sqlcode;      /* saved informix SQLCODE value */
        long blobinfile;     /* 0=save in memory, 1=save in file */
        long textasvarchar;  /* 0=as id, 1=as varchar */
        long byteasvarchar;  /* 0=as id, 1=as varchar */
        long charasvarchar;  /* 0=don't strip trailing blanks, 1=strip */
        long nullformat;     /* 0=NULL as "", 1= NULL as "NULL" */
} ifx_module;

#define MAXBLOBS          64
#define MAXSLOBS          64
#define BLOBINFILE 0      /* 0=in memory, 1=in file */

typedef struct ifx_res {
	char connecid[16];
	char cursorid[16];
	char descrpid[16];
	char statemid[16];
        int  isscroll;
        int  ishold;
        int  iscursory;
        int  paramquery;
	int  numcols;
	int  rowid;
        int  affected_rows;
        int blob_id[MAXBLOBS];
      #if HAVE_IFX_IUS
        int slob_id[MAXSLOBS];
      #endif
} IFX_RES;





typedef struct ifx_blobres {
  	  int mode;
	  loc_t blob_data;
 	  int type;
} IFX_BLOBRES;


#if HAVE_IFX_IUS
typedef struct ifx_slobres {
          ifx_lo_t slobdata;
          ifx_lo_create_spec_t *createspec;
          int lofd;
} IFX_SLOBRES;
#endif


#ifndef THREAD_SAFE
extern ifx_module php3_ifx_module;
#endif

#else

#define ifx_module_ptr NULL

#endif

#endif /* _PHP3_IFX_H */
