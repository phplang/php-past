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
   | Author: Danny Heijl  <Danny.Heijl@cevi.be>                           |
   | Based on the MySQL code by:  Zeev Suraski <bourbon@netvision.net.il> |
   +----------------------------------------------------------------------+
 */
/*
 * I started with the mysql-driver, removed all stuff I did not need,
 * and changed all mysql-specific stuff to Informix-ESQL/C.
 * I used the X-open way of using ESQL/C (using an SQL descriptor and
 * not the Informix-specific way). It is perhaps a little bit slower,
 * but more verbose and less prone to coding errors.
 * This is the first time in my life I coded ESQL/C, so do not look too
 * closely and do not hesitate to point out errors/omissions etc...
 * Aug. 8, 1998 
 * Danny Heijl, Danny.Heijl@cevi.be
 */

/* TODO:
 *
 * ? Safe mode implementation
 *
 * cursory stored procedures ?
 * blob updates ( does someone need that ? )
 * lots of other stuff ??
 * What first ?
 *
 * I hope UDS 9.X support will be added in cooperation with Christian Cartus
 * with lots of other good stuff he had already done before in his own 
 * driver version (Christian Cartus <chc@idgruppe.de>).
 * Jouni Ahto also has promised help and already did the configuration stuff
 * (Jouni Ahto <jah@guru.cultnet.fi>).
 *
 */

#if defined(COMPILE_DL)
#include "dl/phpdl.h"
#endif
#if defined(THREAD_SAFE)
#include "tls.h"
DWORD InformixTls;
static int numthreads=0;
void *ifx_mutex;
#endif

#if WIN32|WINNT
#include <winsock.h>
#else
#include "config.h"
#include "build-defs.h"

#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <netdb.h>
#include <netinet/in.h>
#endif

#include "php.h"
#include "internal_functions.h"
#include "php3_string.h"
#include "build-defs.h"
#include "php3_ifx.h"

#if HAVE_IFX

#include <errno.h>

EXEC SQL include sqltypes;
EXEC SQL include locator;


typedef char IFX[128];

#include "php3_list.h"

#define SAFE_STRING(s) ((s)?(s):"")

function_entry ifx_functions[] = {
    {"ifx_connect",        php3_ifx_connect,            NULL},
    {"ifx_pconnect",       php3_ifx_pconnect,           NULL},
    {"ifx_close",          php3_ifx_close,              NULL},
    {"ifx_create_db",      php3_ifx_create_db,          NULL},
    {"ifx_drop_db",        php3_ifx_drop_db,            NULL},
    {"ifx_query",          php3_ifx_query,              NULL},
    {"ifx_error",          php3_ifx_error,              NULL},
    {"ifx_affected_rows",  php3_ifx_affected_rows,      NULL},
    {"ifx_result",         php3_ifx_result,             NULL},
    {"ifx_num_rows",       php3_ifx_num_rows,           NULL},
    {"ifx_num_fields",     php3_ifx_num_fields,         NULL},
    {"ifx_fetch_row",      php3_ifx_fetch_row,          NULL},
    {"ifx_fetch_object",   php3_ifx_fetch_object,       NULL},
    {"ifx_free_result",    php3_ifx_free_result,        NULL},
    {"ifx_htmltbl_result", php3_ifx_htmltbl_result,     NULL},
    {"ifx_fieldtypes",     php3_ifx_fieldtypes,         NULL},
    {"ifx_fieldproperties",php3_ifx_fieldproperties,    NULL},
    {NULL,                 NULL,                        NULL}
};

php3_module_entry ifx_module_entry = {
    "Informix", 
    ifx_functions, 
    php3_minit_ifx, 
    php3_mshutdown_ifx, 
    php3_rinit_ifx, 
    NULL, 
    php3_info_ifx, 
    STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &ifx_module_entry; }
#if 0
BOOL WINAPI DllMain(HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved)
{
    return 1;
}
#endif
#endif


#if defined(THREAD_SAFE)
typedef struct ifx_global_struct{
    ifx_module php3_ifx_module;
}ifx_global_struct;

#define Informix_GLOBAL(a) ifx_globals->a

#define Informix_TLS_VARS \
    ifx_global_struct *ifx_globals; \
    ifx_globals=TlsGetValue(InformixTls); 

#else
#define Informix_GLOBAL(a) a
#define Informix_TLS_VARS
ifx_module php3_ifx_module;
#endif

#define CHECK_LINK(link) {     \
    if (link==0) {             \
        php3_error(E_WARNING,  \
                "Informix:  A link to the server could not be established"); \
        RETURN_FALSE;          \
        }                      \
    }

#define DUP    1

EXEC SQL DEFINE IFX_SUCCESS 0;
EXEC SQL DEFINE IFX_WARNING 1;
EXEC SQL DEFINE IFX_ERROR  -1;
EXEC SQL DEFINE IFX_NODATA 100;

static int ifx_check()
{
    int _ifx_check;

    _ifx_check = IFX_ERROR;
    
    if (SQLSTATE[0] == '0') {
        switch (SQLSTATE[1]) {
            case '0':
                _ifx_check = IFX_SUCCESS;
                break;
            case '1':
                _ifx_check = IFX_WARNING;
                break;
            case '2':
                _ifx_check = IFX_NODATA;
                break;
            default :
                _ifx_check = IFX_ERROR;
                break;
        }  
    }

  return _ifx_check;
}

static char *ifx_error(ifx)
    EXEC SQL BEGIN DECLARE SECTION;
        PARAMETER char *ifx;
    EXEC SQL END DECLARE SECTION;
{
   static char ifx_err_msg[81];
   char   c;
   
   switch (ifx_check()) {
       case IFX_SUCCESS:
         c = ' ';
         break;       
       case IFX_WARNING:
         c = 'W';
         break;       
       case IFX_ERROR:
         c = 'E';
         break;       
       case IFX_NODATA:
         c = 'N';
         break;       
       default:
         c = '?';
         break;       
   }    
   sprintf(ifx_err_msg,"%c [SQLSTATE=%c%c %c%c%c  SQLCODE=%d]",
                        c,
                        SQLSTATE[0],
                        SQLSTATE[1],
                        SQLSTATE[2],
                        SQLSTATE[3],
                        SQLSTATE[4],
                        SQLCODE);
   return(ifx_err_msg);
}



static void _close_ifx_link(link)
    EXEC SQL BEGIN DECLARE SECTION;
        PARAMETER char *link;
    EXEC SQL END DECLARE SECTION;
{

    Informix_TLS_VARS;

    EXEC SQL SET CONNECTION :link;
    EXEC SQL DISCONNECT CURRENT;
    efree(link);
    Informix_GLOBAL(php3_ifx_module).num_links--;
}

static void _close_ifx_plink(link)
EXEC SQL BEGIN DECLARE SECTION;
  PARAMETER char *link;
EXEC SQL END DECLARE SECTION;
{

    Informix_TLS_VARS;

    EXEC SQL SET CONNECTION :link;
    EXEC SQL DISCONNECT CURRENT;

    free(link);
    Informix_GLOBAL(php3_ifx_module).num_persistent--;
    Informix_GLOBAL(php3_ifx_module).num_links--;
}

static void ifx_free_result(a_result_id)
char *a_result_id;
{
  return;
}

int php3_minit_ifx(INIT_FUNC_ARGS)
{
#if defined(THREAD_SAFE)
    ifx_global_struct *ifx_globals;
    CREATE_MUTEX(ifx_mutex,"Informix_TLS");
    SET_MUTEX(ifx_mutex);
    numthreads++;
    if (numthreads==1){
        if ((InformixTls=TlsAlloc())==0xFFFFFFFF){
            FREE_MUTEX(ifx_mutex);
            return 0;
        }
    }
    FREE_MUTEX(ifx_mutex);
    ifx_globals = (ifx_global_struct *) 
                           LocalAlloc(LPTR, sizeof(ifx_global_struct)); 
    TlsSetValue(InformixTls, (void *) ifx_globals);
#endif

    if (cfg_get_long("ifx.allow_persistent",
                      &Informix_GLOBAL(php3_ifx_module).allow_persistent)==FAILURE) {
        Informix_GLOBAL(php3_ifx_module).allow_persistent=1;
    }
    if (cfg_get_long("ifx.max_persistent",
                      &Informix_GLOBAL(php3_ifx_module).max_persistent)==FAILURE) {
        Informix_GLOBAL(php3_ifx_module).max_persistent=-1;
    }
    if (cfg_get_long("ifx.max_links",
                     &Informix_GLOBAL(php3_ifx_module).max_links)==FAILURE) {
        Informix_GLOBAL(php3_ifx_module).max_links=-1;
    }
    if (cfg_get_string("ifx.default_host",
                      &Informix_GLOBAL(php3_ifx_module).default_host)==FAILURE
              || Informix_GLOBAL(php3_ifx_module).default_host[0]==0) {
        Informix_GLOBAL(php3_ifx_module).default_host=NULL;
    }
    if (cfg_get_string("ifx.default_user",
                       &Informix_GLOBAL(php3_ifx_module).default_user)==FAILURE
              || Informix_GLOBAL(php3_ifx_module).default_user[0]==0) {
        Informix_GLOBAL(php3_ifx_module).default_user=NULL;
    }
    if (cfg_get_string("ifx.default_password",
                        &Informix_GLOBAL(php3_ifx_module).default_password)==FAILURE
              || Informix_GLOBAL(php3_ifx_module).default_password[0]==0) {    
        Informix_GLOBAL(php3_ifx_module).default_password=NULL;
    }
    Informix_GLOBAL(php3_ifx_module).num_persistent=0;
    Informix_GLOBAL(php3_ifx_module).le_result = 
            register_list_destructors(ifx_free_result,NULL);
    Informix_GLOBAL(php3_ifx_module).le_link =  
            register_list_destructors(_close_ifx_link,NULL);
    Informix_GLOBAL(php3_ifx_module).le_plink = 
            register_list_destructors(NULL,_close_ifx_plink);

#if 0
    printf("Registered:  %d,%d,%d\n",
         Informix_GLOBAL(php3_ifx_module).le_result,
         Informix_GLOBAL(php3_ifx_module).le_link,
         Informix_GLOBAL(php3_ifx_module).le_plink);
#endif
    ifx_module_entry.type = type;

    return SUCCESS;
}


int php3_mshutdown_ifx(void){
#if defined(THREAD_SAFE)
    Informix_TLS_VARS;
    if (ifx_globals != 0) 
        LocalFree((HLOCAL) ifx_globals); 
    SET_MUTEX(ifx_mutex);
    numthreads--;
    if (!numthreads){
        if (!TlsFree(InformixTls)){
            FREE_MUTEX(ifx_mutex);
            return 0;
        }
    }
    FREE_MUTEX(ifx_mutex);
#endif
    return SUCCESS;
}

int php3_rinit_ifx(INIT_FUNC_ARGS)
{
    Informix_TLS_VARS;

    Informix_GLOBAL(php3_ifx_module).default_link=-1;
    Informix_GLOBAL(php3_ifx_module).num_links = 
                 Informix_GLOBAL(php3_ifx_module).num_persistent;
    return SUCCESS;
}


void php3_info_ifx(void)
{
    char maxp[16],maxl[16];
    Informix_TLS_VARS;

    
    if (Informix_GLOBAL(php3_ifx_module).max_persistent==-1) {
        strcpy(maxp,"Unlimited");
    } else {
        snprintf(maxp,15,"%ld",Informix_GLOBAL(php3_ifx_module).max_persistent);
        maxp[15]=0;
    }
    if (Informix_GLOBAL(php3_ifx_module).max_links==-1) {
        strcpy(maxl,"Unlimited");
    } else {
        snprintf(maxl,15,"%ld",Informix_GLOBAL(php3_ifx_module).max_links);
        maxl[15]=0;
    }
    php3_printf("<table cellpadding=5>"
                "<tr><td>Allow persistent links:</td><td>%s</td></tr>\n"
                "<tr><td>Persistent links:</td><td>%d/%s</td></tr>\n"
                "<tr><td>Total links:</td><td>%d/%s</td></tr>\n"
                "<tr><td>Client API version:</td><td>%02.2f</td></tr>\n"
#if !(WIN32|WINNT)
                "<tr><td valign=\"top\">Compilation definitions:</td><td>"
                "<tt>IFX_INCLUDE=%s<br>\n"
                "IFX_LFLAGS=%s<br>\n"
                "IFX_LIBS=%s<br></tt></td></tr>"
#endif
                "</table>\n",
                (Informix_GLOBAL(php3_ifx_module).allow_persistent?"Yes":"No"),
                Informix_GLOBAL(php3_ifx_module).num_persistent,maxp,
                Informix_GLOBAL(php3_ifx_module).num_links,maxl,
                (double)(CLIENT_SQLI_VER/100.0)
#if !(WIN32|WINNT)
                ,PHP_IFX_INCLUDE,
                PHP_IFX_LFLAGS,
                PHP_IFX_LIBS
#endif
                );
}


static void php3_ifx_do_connect(INTERNAL_FUNCTION_PARAMETERS,int persistent)
{

    EXEC SQL BEGIN DECLARE SECTION;
      char *user,*passwd,*host;
      char *ifx;
    EXEC SQL END DECLARE SECTION;    

    char *tmp;
    char *hashed_details;
    int hashed_details_length;
    Informix_TLS_VARS;

    if (php3_ini.sql_safe_mode) {
        if (ARG_COUNT(ht)>0) {
            php3_error(E_NOTICE,
            "SQL safe mode in effect - ignoring host/user/password information");
        }
        host = passwd = NULL;
        user = _php3_get_current_user();
        hashed_details_length = strlen(user)+3+3;
        hashed_details = (char *) emalloc(hashed_details_length+1);
        sprintf(hashed_details,"ifx__%s_",user);
    } else {
        host = Informix_GLOBAL(php3_ifx_module).default_host;
        user = Informix_GLOBAL(php3_ifx_module).default_user;
        passwd = Informix_GLOBAL(php3_ifx_module).default_password;
        
        switch(ARG_COUNT(ht)) {
            case 0: /* defaults */
                break;
            case 1: {
                    pval *yyhost;
                    
                    if (getParameters(ht, 1, &yyhost)==FAILURE) {
                        RETURN_FALSE;
                    }
                    convert_to_string(yyhost);
                    host = yyhost->value.str.val;
                }
                break;
            case 2: {
                    pval *yyhost,*yyuser;
                    
                    if (getParameters(ht, 2, &yyhost, &yyuser)==FAILURE) {
                        RETURN_FALSE;
                    }
                    convert_to_string(yyhost);
                    convert_to_string(yyuser);
                    host = yyhost->value.str.val;
                    user = yyuser->value.str.val;
                }
                break;
            case 3: {
                    pval *yyhost,*yyuser,*yypasswd;
                
                    if (getParameters(ht, 3, &yyhost, &yyuser, &yypasswd) == FAILURE) {
                        RETURN_FALSE;
                    }
                    convert_to_string(yyhost);
                    convert_to_string(yyuser);
                    convert_to_string(yypasswd);
                    host = yyhost->value.str.val;
                    user = yyuser->value.str.val;
                    passwd = yypasswd->value.str.val;
                }
                break;
            default:
                WRONG_PARAM_COUNT;
                break;
        }
        hashed_details_length = sizeof("ifx___")-1 + 
                          strlen(SAFE_STRING(host))+
                          strlen(SAFE_STRING(user))+
                          strlen(SAFE_STRING(passwd));
        hashed_details = (char *) emalloc(hashed_details_length+1);
        sprintf(hashed_details,"ifx_%s_%s_%s",
                               SAFE_STRING(host), 
                               SAFE_STRING(user), 
                               SAFE_STRING(passwd));
    }



    
    if (!Informix_GLOBAL(php3_ifx_module).allow_persistent) {
        persistent=0;
    }
    if (persistent) {
        list_entry *le;
        
        /* try to find if we already have this link in our persistent list */
        if (_php3_hash_find(plist, hashed_details, hashed_details_length+1, 
                            (void **) &le)==FAILURE) {  /* we don't */
            list_entry new_le;

            if (Informix_GLOBAL(php3_ifx_module).max_links!=-1 &&
                    Informix_GLOBAL(php3_ifx_module).num_links >=
                    Informix_GLOBAL(php3_ifx_module).max_links) {
                php3_error(E_WARNING,
                           "Informix:  Too many open links (%d)",
                           Informix_GLOBAL(php3_ifx_module).num_links);
                efree(hashed_details);
                RETURN_FALSE;
            }
            if (Informix_GLOBAL(php3_ifx_module).max_persistent!=-1 && 
                    Informix_GLOBAL(php3_ifx_module).num_persistent >=
                    Informix_GLOBAL(php3_ifx_module).max_persistent) {
                php3_error(E_WARNING,
                           "Informix:  Too many open persistent links (%d)",
                           Informix_GLOBAL(php3_ifx_module).num_persistent);
                efree(hashed_details);
                RETURN_FALSE;
            }
            /* create the link */
            ifx = (char *)malloc(sizeof(IFX));
            Informix_GLOBAL(php3_ifx_module).connectionid++;
            sprintf(ifx,"%s%x", 
                    user, 
                    Informix_GLOBAL(php3_ifx_module).connectionid);

            EXEC SQL CONNECT TO :host AS :ifx 
                     USER :user USING :passwd 
                     WITH CONCURRENT TRANSACTION;  

            if (ifx_check() == IFX_ERROR) {
                php3_error(E_WARNING,ifx_error(ifx));
                free(ifx);
                efree(hashed_details);
                RETURN_FALSE;
            }
            
            /* hash it up */
            new_le.type = Informix_GLOBAL(php3_ifx_module).le_plink;
            new_le.ptr = ifx;
            if (_php3_hash_update(plist, hashed_details, 
                   hashed_details_length+1, 
                   (void *) &new_le, sizeof(list_entry), NULL)==FAILURE) {
                free(ifx);
                efree(hashed_details);
                RETURN_FALSE;
            }
            Informix_GLOBAL(php3_ifx_module).num_persistent++;
            Informix_GLOBAL(php3_ifx_module).num_links++;
        } else {  /* we do */
            if (le->type != Informix_GLOBAL(php3_ifx_module).le_plink) {
                RETURN_FALSE;
            }
            /* ensure that the link did not die */
            ifx = le->ptr;
            EXEC SQL SET CONNECTION :ifx;
            if (ifx_check() == IFX_ERROR) {
                          /* the link died      */
                ifx = le->ptr;        /* reconnect silently */
                EXEC SQL CONNECT TO :host AS :ifx 
                         USER :user USING :passwd 
                         WITH CONCURRENT TRANSACTION;  

                if (ifx_check() == IFX_ERROR) {
                    php3_error(E_WARNING,
                               "Informix:  Link to server lost, unable to reconnect (%s)",
                               ifx_error(ifx));
                    _php3_hash_del(plist, hashed_details, 
                                   hashed_details_length+1);
                    efree(hashed_details);
                    RETURN_FALSE;
                }
            }
            ifx = le->ptr;
        }
        return_value->value.lval = php3_list_insert(ifx,
                               Informix_GLOBAL(php3_ifx_module).le_plink);
        return_value->type = IS_LONG;
    } else { /* non persistent */
        list_entry *index_ptr,new_index_ptr;
        
        /* first we check the hash for the hashed_details key.  if it exists,
         * it should point us to the right offset where the actual ifx link sits.
         * if it doesn't, open a new ifx link, add it to the resource list,
         * and add a pointer to it with hashed_details as the key.
         */
        if (_php3_hash_find(list,hashed_details,hashed_details_length+1,
                           (void **) &index_ptr) == SUCCESS) {
            int type,link;
            void *ptr;

            if (index_ptr->type != le_index_ptr) {
                RETURN_FALSE;
            }
            link = (int) index_ptr->ptr;
            ptr = php3_list_find(link,&type);   /* check if the link is still there */
            if (ptr && (type==Informix_GLOBAL(php3_ifx_module).le_link ||
                        type==Informix_GLOBAL(php3_ifx_module).le_plink)) {
                return_value->value.lval = 
                          Informix_GLOBAL(php3_ifx_module).default_link = 
                          link;
                return_value->type = IS_LONG;
                efree(hashed_details);
                return;
            } else {
                _php3_hash_del(list,hashed_details,hashed_details_length+1);
            }
        }
        if (Informix_GLOBAL(php3_ifx_module).max_links != -1 && 
            Informix_GLOBAL(php3_ifx_module).num_links >= 
                             Informix_GLOBAL(php3_ifx_module).max_links) {
            php3_error(E_WARNING,
                       "Informix:  Too many open links (%d)",
                       Informix_GLOBAL(php3_ifx_module).num_links);
            efree(hashed_details);
            RETURN_FALSE;
        }
        ifx = (char *) emalloc(sizeof(IFX));
        Informix_GLOBAL(php3_ifx_module).connectionid++;        
        sprintf(ifx,"connec%x", 
                Informix_GLOBAL(php3_ifx_module).connectionid);

        EXEC SQL CONNECT TO :host AS :ifx 
             USER :user USING :passwd 
             WITH CONCURRENT TRANSACTION;  
        if (ifx_check() == IFX_ERROR) {
            php3_error(E_WARNING,"ifx_pconnect : %s", ifx_error(ifx));
            efree(hashed_details);
            efree(ifx);
            RETURN_FALSE;
        }

        /* add it to the list */
        return_value->value.lval = 
             php3_list_insert(ifx,Informix_GLOBAL(php3_ifx_module).le_link);
        return_value->type = IS_LONG;
        
        /* add it to the hash */
        new_index_ptr.ptr = (void *) return_value->value.lval;
        new_index_ptr.type = le_index_ptr;
        if (_php3_hash_update(list,
                              hashed_details,
                              hashed_details_length+1,
                              (void *) &new_index_ptr, 
                              sizeof(list_entry), NULL) == FAILURE) {
            efree(hashed_details);
            RETURN_FALSE;
        }
        Informix_GLOBAL(php3_ifx_module).num_links++;
    }
    efree(hashed_details);
    Informix_GLOBAL(php3_ifx_module).default_link=return_value->value.lval;
}


static int php3_ifx_get_default_link(INTERNAL_FUNCTION_PARAMETERS)
{
    Informix_TLS_VARS;

    if (Informix_GLOBAL(php3_ifx_module).default_link==-1) { /* no link opened yet, implicitly open one */
        HashTable tmp;
        
        _php3_hash_init(&tmp,0,NULL,NULL,0);
        php3_ifx_do_connect(&tmp,return_value,list,plist,0);
        _php3_hash_destroy(&tmp);
    }
    return Informix_GLOBAL(php3_ifx_module).default_link;
}


void php3_ifx_connect(INTERNAL_FUNCTION_PARAMETERS)
{
    php3_ifx_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,0);
}

void php3_ifx_pconnect(INTERNAL_FUNCTION_PARAMETERS)
{
    php3_ifx_do_connect(INTERNAL_FUNCTION_PARAM_PASSTHRU,1);
}

void php3_ifx_close(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *ifx_link;
    int id,type;

EXEC SQL BEGIN DECLARE SECTION;
    char *ifx;
EXEC SQL END DECLARE SECTION;

    Informix_TLS_VARS;

    
    switch (ARG_COUNT(ht)) {
        case 0:
            id = Informix_GLOBAL(php3_ifx_module).default_link;
            break;
        case 1:
            if (getParameters(ht, 1, &ifx_link)==FAILURE) {
                RETURN_FALSE;
            }
            convert_to_long(ifx_link);
            id = ifx_link->value.lval;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    ifx = (char *) php3_list_find(id,&type);
    if (type!=Informix_GLOBAL(php3_ifx_module).le_link && 
            type!=Informix_GLOBAL(php3_ifx_module).le_plink) {
        php3_error(E_WARNING,
                   "ifx_close : %d (type %d) is not an Informix link index",
                   id, 
                   type);
        RETURN_FALSE;
    }
    
    EXEC SQL SET CONNECTION :ifx;
    EXEC SQL close database;
    EXEC SQL DISCONNECT CURRENT;    

    php3_list_delete(id);
    RETURN_TRUE;
}
    
            

void php3_ifx_create_db(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *db,*ifx_link;
    int id,type;

EXEC SQL BEGIN DECLARE SECTION;
    char *ifx;
    char statement[512];
EXEC SQL END DECLARE SECTION;

    Informix_TLS_VARS;

    
    switch(ARG_COUNT(ht)) {
        case 1:
            if (getParameters(ht, 1, &db)==FAILURE) {
                RETURN_FALSE;
            }
            id = php3_ifx_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
            break;
        case 2:
            if (getParameters(ht, 2, &db, &ifx_link)==FAILURE) {
                RETURN_FALSE;
            }
            convert_to_long(ifx_link);
            id = ifx_link->value.lval;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    CHECK_LINK(id);
    
    ifx = (char *) php3_list_find(id,&type);
    if (type!=Informix_GLOBAL(php3_ifx_module).le_link && type!=Informix_GLOBAL(php3_ifx_module).le_plink) {
        php3_error(E_WARNING,
                   "ifx_create_db : %d (type %d) is not a Informix link index",
                   id,
                   type);
        RETURN_FALSE;
    }
    
    convert_to_string(db);
    sprintf(statement, "create database %s", db->value.str.val); 
    EXEC SQL SET CONNECTION :ifx;
    EXEC SQL EXECUTE IMMEDIATE :statement;
    if (ifx_check() != IFX_SUCCESS) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}


void php3_ifx_drop_db(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *db,*ifx_link;
    int id,type;

EXEC SQL BEGIN DECLARE SECTION;
    char *ifx;
    char statement[512];
EXEC SQL END DECLARE SECTION;

    Informix_TLS_VARS;

    
    switch(ARG_COUNT(ht)) {
        case 1:
            if (getParameters(ht, 1, &db)==FAILURE) {
                RETURN_FALSE;
            }
            id = php3_ifx_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
            break;
        case 2:
            if (getParameters(ht, 2, &db, &ifx_link)==FAILURE) {
                RETURN_FALSE;
            }
            convert_to_long(ifx_link);
            id = ifx_link->value.lval;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    CHECK_LINK(id);
    
    ifx = (char *) php3_list_find(id,&type);
    if (type!=Informix_GLOBAL(php3_ifx_module).le_link && type!=Informix_GLOBAL(php3_ifx_module).le_plink) {
        php3_error(E_WARNING,
                   "ifx_drop_db : %d (type %d) is not a Informix link index",
                   id,
                   type);
        RETURN_FALSE;
    }
    
    convert_to_string(db);
    sprintf(statement, "drop database %s", db->value.str.val); 
    EXEC SQL SET CONNECTION :ifx;
    EXEC SQL EXECUTE IMMEDIATE :statement;
    if (ifx_check() != IFX_SUCCESS) {
        RETURN_TRUE;
    } else {
        RETURN_FALSE;
    }
}


void php3_ifx_query(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *query,*ifx_link;
    int id,type;

    IFX_RES *Ifx_Result;
    
EXEC SQL BEGIN DECLARE SECTION;
    char *ifx;                /* connection ID     */
    char cursorid[32];        /* query cursor id   */
    char statemid[32];        /* statement id      */
    char descrpid[32];        /* descriptor id     */
    char *statement;          /* query text        */
    int  fieldcount;          /* field count       */
    int  i;                   /* field index       */
    short fieldtype;
    loc_t *locator;
EXEC SQL END DECLARE SECTION;

    int locind;
    
    Informix_TLS_VARS;

    
    switch(ARG_COUNT(ht)) {
        case 1:
            if (getParameters(ht, 1, &query)==FAILURE) {
                RETURN_FALSE;
            }
            id = php3_ifx_get_default_link(INTERNAL_FUNCTION_PARAM_PASSTHRU);
            break;
        case 2:
            if (getParameters(ht, 2, &query, &ifx_link)==FAILURE) {
                RETURN_FALSE;
            }
            convert_to_long(ifx_link);
            id = ifx_link->value.lval;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    ifx = (char *) php3_list_find(id,&type);
    if (type != Informix_GLOBAL(php3_ifx_module).le_link && 
            type!=Informix_GLOBAL(php3_ifx_module).le_plink) {
        php3_error(E_WARNING,
                   "ifx_query : %d (type %d) is not a Informix link index",
                   id,
                   type);
        RETURN_FALSE;
    }
    

    convert_to_string(query);

    statement = query->value.str.val;
    Informix_GLOBAL(php3_ifx_module).cursorid++;    
    sprintf(statemid, "statem%x", Informix_GLOBAL(php3_ifx_module).cursorid); 
    sprintf(cursorid, "cursor%x", Informix_GLOBAL(php3_ifx_module).cursorid); 
    sprintf(descrpid, "descrp%x", Informix_GLOBAL(php3_ifx_module).cursorid); 
    EXEC SQL set connection :ifx;
    if (ifx_check() < 0) {
        php3_error(E_WARNING,"Set connection %s fails (%s)",
                              ifx,
                              ifx_error(ifx));
        RETURN_FALSE;
    }
    EXEC SQL PREPARE :statemid FROM :statement;
    if (ifx_check() < 0) {
        php3_error(E_WARNING,"Prepare <%s> fails (%s)",
                              statement,
                              ifx_error(ifx));
        RETURN_FALSE;
    }
    EXEC SQL ALLOCATE DESCRIPTOR :descrpid WITH MAX 256;
    if (ifx_check() < 0) {
        php3_error(E_WARNING,"Allocate desciptor <%s> fails (%s)",
                              descrpid,
                              ifx_error(ifx));
        RETURN_FALSE;
    }
    EXEC SQL DESCRIBE :statemid USING SQL DESCRIPTOR :descrpid;
    if (ifx_check() < 0) {
        php3_error(E_WARNING,"Describe fails (%s)",
                              ifx_error(ifx));
        RETURN_FALSE;
    }

    Ifx_Result = (IFX_RES *)emalloc(sizeof(IFX_RES));
    if (Ifx_Result == NULL) { 
        php3_error(E_WARNING,"Out of memory allocating IFX_RES");
        RETURN_FALSE;
    }

    /* initialize result data structure */
    
    Ifx_Result->rowid = 0;
    strcpy(Ifx_Result->connecid, ifx); 
    strcpy(Ifx_Result->descrpid, descrpid);
    for (i = 0; i < MAXBLOBS; ++i)
        Ifx_Result->blob_data[i] = (loc_t *)NULL;

    if (sqlca.sqlcode != 0) {    /* not a select, execute immediate */
    strcpy(Ifx_Result->cursorid, "");
        strcpy(Ifx_Result->descrpid, descrpid);
        strcpy(Ifx_Result->statemid, statemid);
        EXEC SQL EXECUTE :statemid;
        if (ifx_check() < 0) {
            php3_error(E_WARNING,"Execute immediate fails : %s (%s)",
                                 statement,
                                 ifx_error(ifx));
            efree(Ifx_Result);
            RETURN_FALSE;
        }
    } else {
        EXEC SQL GET DESCRIPTOR :descrpid :fieldcount = COUNT;
        if (ifx_check() < 0) {
            php3_error(E_WARNING,"Can not get descriptor %s (%s)",
                                  descrpid,
                                  ifx_error(ifx));
            RETURN_FALSE;
        }
        Ifx_Result->numcols = fieldcount;
        
        EXEC SQL DECLARE :cursorid CURSOR FOR :statemid;
        if (ifx_check() < 0) {
            php3_error(E_WARNING,"Declare cursor fails (%s)", ifx_error(ifx));
            efree(Ifx_Result);
            RETURN_FALSE;
        }
        EXEC SQL OPEN :cursorid;
        if (ifx_check() < 0) {
            php3_error(E_WARNING,"Open cursor fails (%s)", ifx_error(ifx));
            efree(Ifx_Result);
            RETURN_FALSE;
        }
        strcpy(Ifx_Result->cursorid, cursorid);
        strcpy(Ifx_Result->descrpid, descrpid);
        strcpy(Ifx_Result->statemid, statemid);
        
        /* check for text/blob columns */
        
        locind = 0;
        for (i = 1; i <= fieldcount; ++i) {
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i  :fieldtype = TYPE;
            if ((fieldtype == SQLTEXT) || (fieldtype == SQLBYTES)) {
                locator = (loc_t *)malloc(sizeof(loc_t));
                Ifx_Result->blob_data[locind] = locator;
                ++locind;
                locator->loc_loctype = LOCMEMORY;
                locator->loc_bufsize = INITIAL_BLOBSIZE - 4;
                locator->loc_buffer = (char *)malloc(INITIAL_BLOBSIZE);
                locator->loc_mflags = 0;
                locator->loc_oflags = 0;
                EXEC SQL SET DESCRIPTOR :descrpid VALUE :i DATA = :*locator;
            }
        }
                
    }

    return_value->value.lval = 
                 php3_list_insert(Ifx_Result,
                                  Informix_GLOBAL(php3_ifx_module).le_result);
    return_value->type = IS_LONG;
}

void php3_ifx_error(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *ifx_link;
    int id,type;
    IFX *ifx;
    Informix_TLS_VARS;

    
    switch(ARG_COUNT(ht)) {
        case 0:
            id = Informix_GLOBAL(php3_ifx_module).default_link;
            break;
        case 1:
            if (getParameters(ht, 1, &ifx_link)==FAILURE) {
                RETURN_FALSE;
            }
            convert_to_long(ifx_link);
            id = ifx_link->value.lval;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    if (id==-1) {
        RETURN_FALSE;
    }
    ifx = (IFX *) php3_list_find(id,&type);
    if (type!=Informix_GLOBAL(php3_ifx_module).le_link && type!=Informix_GLOBAL(php3_ifx_module).le_plink) {
        php3_error(E_WARNING,
                   "ifx_error : %d (type %d) is not a Informix link index",
                   id,
                   type);
        RETURN_FALSE;
    }
    
    RETURN_STRING(ifx_error(ifx),1);
}




void php3_ifx_affected_rows(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *ifx_link;
    int id,type;
    IFX *ifx;
    Informix_TLS_VARS;

    
    switch(ARG_COUNT(ht)) {
        case 0:
            id = Informix_GLOBAL(php3_ifx_module).default_link;
            break;
        case 1:
            if (getParameters(ht, 1, &ifx_link)==FAILURE) {
                RETURN_FALSE;
            }
            convert_to_long(ifx_link);
            id = ifx_link->value.lval;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    ifx = (IFX *) php3_list_find(id,&type);
    if (type!=Informix_GLOBAL(php3_ifx_module).le_link && type!=Informix_GLOBAL(php3_ifx_module).le_plink) {
        php3_error(E_WARNING,
                   "ifx_affected_rows : %d (type %d) is not a Informix link index",
                   id,
                   type);
        RETURN_FALSE;
    }
    
    return_value->value.lval = -1; /* not supported */
    return_value->type = IS_LONG;
}



void php3_ifx_fetch_row(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *result;
    IFX_RES *Ifx_Result;

EXEC SQL BEGIN DECLARE SECTION;
    char *ifx;                /* connection ID    */
    char *cursorid;           /* query cursor id  */
    char *statemid;           /* statement id     */
    char *descrpid;           /* descriptor id    */
    char *statement;          /* query text       */
    int  fieldcount;          /* field count      */
    int   i;                  /* an index         */ 
    char  fieldname[64];      /* fieldname        */
    char  ifx_bug;            /* pad for ifx bug  */
    short fieldtype;          /* field type       */
    int   fieldleng;          /* field length     */
    int   field_ind;          /* the field index  */
    char  *field;             /* the field data   */

    short      indicator;
    int        int_data;
    char       *char_data;
    long       date_data;
    interval   intvl_data = {0};
    datetime   dt_data = {0};
    decimal    dec_data = {0};
    short      short_data;
    loc_t      *locator;
    float      float_data;
    double     double_data;
EXEC SQL END DECLARE SECTION;

    int type;
    int num_fields;
    int locind;
    
    char string_data[256];
    long long_data;
    char *p;
    FILE *fp;
    char *blobfilename;            
    char *blobdir;

    Informix_TLS_VARS;

    
    if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
        WRONG_PARAM_COUNT;
    }
    
    convert_to_long(result);
    Ifx_Result = (IFX_RES *) php3_list_find(result->value.lval,&type);
    
    if (type!=Informix_GLOBAL(php3_ifx_module).le_result) {
        php3_error(E_WARNING,"%d is not a Informix result index",
                   result->value.lval);
        RETURN_FALSE;
    }
    if (strcmp(Ifx_Result->cursorid,"") == 0) {
        php3_error(E_WARNING,"Not a select cursor !");
        RETURN_FALSE;
    }
    
    ifx        = Ifx_Result->connecid;
    cursorid   = Ifx_Result->cursorid;
    statemid   = Ifx_Result->statemid;
    descrpid   = Ifx_Result->descrpid;
    fieldcount = Ifx_Result->numcols;
        
    EXEC SQL FETCH :cursorid USING SQL DESCRIPTOR :descrpid;
    switch (ifx_check()) {
        case IFX_ERROR:
            php3_error(E_WARNING,
                       "Can not fetch next row on cursor %s (%s)",
                       ifx_error(ifx),
                       cursorid);
            RETURN_FALSE;
            break;
        case IFX_NODATA:
            RETURN_FALSE;
            break;
        default:
            break;
    }

    Ifx_Result->rowid++;

    if (array_init(return_value)==FAILURE) {
        RETURN_FALSE;
    }
    num_fields = fieldcount;
    
    if ((blobdir = getenv("php3_blobdir")) == NULL)
        blobdir = ".";

    locind = 0;
    for (i = 1; i <= num_fields; i++) {
        EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :fieldtype = TYPE,
                                                   :fieldname = NAME,
                                                   :fieldleng = LENGTH,
                                                   :indicator = INDICATOR;
        if (ifx_check() < 0) {
            php3_error(E_WARNING,"Get descriptor (field # %d) fails (%s)",
                                 i,
                                 ifx_error(ifx));
            RETURN_FALSE;
        }
        
        p = fieldname;         /* rtrim fieldname */
        while ((*p != ' ') && (p < &fieldname[sizeof(fieldname) - 1])) ++p;
        *p = 0;

        if (indicator == -1) {        /* NULL */
            add_assoc_string(return_value, fieldname, "NULL", DUP);
            continue;
        }
        switch (fieldtype) {
            case SQLSERIAL  : 
            case SQLINT     :
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :int_data = DATA;
                long_data = int_data;
                sprintf(string_data,"%ld", long_data); 
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
            case SQLSMINT   :
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :short_data = DATA;
                long_data = short_data;
                sprintf(string_data,"%ld", long_data); 
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
            case SQLDECIMAL :
            case SQLMONEY   :
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :dec_data = DATA;
                memset(string_data, 0x20, 64);
                dectoasc(&dec_data, string_data, 63, -1);
                for (p =string_data; *p != ' '; ++p) ;
                *p = 0;                
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
            case SQLSMFLOAT :
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :float_data = DATA;
                double_data = float_data;
                sprintf(string_data,"%17.17g", double_data);
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
            case SQLFLOAT   :
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :double_data = DATA;
                sprintf(string_data,"%17.17g", double_data);
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
            case SQLDATE    :
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :date_data = DATA;
                rdatestr(date_data, string_data); 
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
            case SQLDTIME   :
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :dt_data = DATA;
                dttoasc(&dt_data, string_data); 
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
            case SQLINTERVAL:
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :intvl_data = DATA;
                intoasc(&intvl_data, string_data); 
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
            case SQLCHAR    :
            case SQLVCHAR   :
                if ((char_data = (char *)emalloc(fieldleng + 1)) == NULL) {
                    php3_error(E_WARNING, "Out of memory");
                    RETURN_FALSE;
                }
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :char_data = DATA;
                add_assoc_string(return_value, fieldname, char_data, DUP);
                efree(char_data);
                char_data = NULL;
                break;
            case SQLTEXT    :    /* is returned as if a long varchar */
                locator = Ifx_Result->blob_data[locind];
                ++locind;
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
                if (locator->loc_status < 0) {  /* blob too large */   
                    free(locator->loc_buffer);
                    locator->loc_bufsize = locator->loc_size + 4;
                    locator->loc_buffer = (char *)malloc(locator->loc_bufsize + 4);
                    if (locator->loc_buffer == NULL) {
                        php3_error(E_WARNING,"no memory (%d bytes) for blob",
                                   locator->loc_bufsize);
                        RETURN_FALSE;
                    }
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
                }
                add_assoc_stringl(return_value, 
                                  fieldname, 
                                  locator->loc_buffer,
                                  locator->loc_size,
                                  DUP);
                break;
            case SQLBYTES   :    /* is dumped into a file, returns filename */
                locator = Ifx_Result->blob_data[locind];
                ++locind;
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
                if (locator->loc_status < 0) {  /* blob too large */   
                    free(locator->loc_buffer);
                    locator->loc_bufsize = locator->loc_size + 4;
                    locator->loc_buffer = (char *)malloc(locator->loc_bufsize + 4);
                    if (locator->loc_buffer == NULL) {
                        php3_error(E_WARNING,"no memory (%d bytes) for blob",
                                   locator->loc_bufsize);
                        RETURN_FALSE;
                    }
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
                }
                /* dump blob in temp file */
                sprintf(string_data,"blb%d", locind);
                blobfilename = tempnam(blobdir,string_data);
                fp = fopen(blobfilename, "wb");
                if (fp == NULL) {
                    php3_error(E_WARNING, "can't create blobfile %s (%d)",
                                blobfilename,
                                errno);
                }
                if (fwrite(locator->loc_buffer,
                           locator->loc_size, 1, fp) != 1) {
                    php3_error(E_WARNING, "can't write blobfile %s (%d)",
                                blobfilename,
                                errno);
                }
                fclose(fp);
                /* return filename and filesize */
                sprintf(string_data,"@BlobFile@%s@%d@", blobfilename, locator->loc_size);
                free(blobfilename);
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
            default         :
                sprintf(string_data,"ESQL/C : %s : unsupported field type[%d]",
                        fieldname,
                        fieldleng);
                add_assoc_string(return_value, fieldname, string_data, DUP);
                break;
        }
            
        continue;
    }    
}

void php3_ifx_result(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *result, *arg2;
    IFX_RES *Ifx_Result;

EXEC SQL BEGIN DECLARE SECTION;
    char  *ifx;                 /* connection ID    */
    char  *cursorid;            /* query cursor id  */
    char  *statemid;            /* statement id     */
    char  *descrpid;            /* descriptor id    */
    char  *statement;           /* query text       */
    int   fieldcount;           /* field count      */
    int   i;                    /* an index         */ 
    char  fieldname[64];        /* fieldname        */
    short fieldtype;            /* field type       */
    int   field_ind;            /* the field index  */
    int   fieldleng;            /* field length     */
    char  *field;               /* the field        */

    short     indicator;
    int       int_data;
    char      *char_data = NULL;
    long      date_data;
    interval  intvl_data = {0};
    datetime  dt_data = {0};
    decimal   dec_data = {0};
    short     short_data;
    float     float_data;
    double    double_data;
    loc_t     *locator;
EXEC SQL END DECLARE SECTION;

    int type;
    int num_fields;
    
    char string_data[256];
    long long_data;
    char *p;
    int  locind;
    FILE *fp;
    char *blobfilename;            
    char *blobdir;
        
    Informix_TLS_VARS;

    switch (ARG_COUNT(ht)) {
        case 2:
            if (getParameters(ht, 2, &result, &arg2)==FAILURE) {
                RETURN_FALSE;
            }
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    convert_to_long(result);
    Ifx_Result = (IFX_RES *) php3_list_find(result->value.lval,&type);
    
    if (type!=Informix_GLOBAL(php3_ifx_module).le_result) {
        php3_error(E_WARNING,
                   "%d is not a Informix result index",result->value.lval);
        RETURN_FALSE;
    }

    if (Ifx_Result->rowid == 0) {
        php3_error(E_WARNING,"No rows fetched yet !");
        RETURN_FALSE;
    }
    if (strcmp(Ifx_Result->cursorid,"") == 0) {
        php3_error(E_WARNING,"Not a select cursor !");
        RETURN_FALSE;
    }
          
    ifx        = Ifx_Result->connecid;
    cursorid   = Ifx_Result->cursorid;
    statemid   = Ifx_Result->statemid;
    descrpid   = Ifx_Result->descrpid;
    fieldcount = Ifx_Result->numcols;
    
    if (arg2->type == IS_STRING) {
        field     = arg2->value.str.val;
        field_ind = -1;    
    } else {
        field_ind = arg2->value.lval;
        field     = NULL;
    }    
    
    if ((blobdir = getenv("php3_blobdir")) == NULL)
        blobdir = ".";
   
    /* if a fieldindex is given, we need to count the blobs to    */
    /* get at the right blobindex                                 */
    /* if a fieldname is given, we have to check all fields       */
    /* for the right name and count the blobs too                 */
        
    locind = 0;

    for (i = 1; i <= fieldcount; i++) {
        EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :fieldname = NAME,
                                                   :fieldtype = TYPE;
        if (ifx_check() < 0) {
            php3_error(E_WARNING,"ifx_result : can not describe field %d",i);
            RETURN_FALSE;
        }
        if ((field == NULL) && (field_ind == i)) {
           break;        /* we got to the field with the index */
        }
                         /* rtrim the informix fieldname */           
        p = fieldname;
        while ((*p != ' ') && (p < &fieldname[sizeof(fieldname) - 1])) ++p;
        *p = 0;
                        
        if (strcasecmp(fieldname, field) == 0) {
            field_ind = i;    /* we got to the field with the name */    
            break;
        }
        if ((fieldtype == SQLBYTES) || (fieldtype == SQLTEXT)) {
            ++locind;        /* bump locind if this one was a blob */
        }                    /* and try the next field             */
    }
    
    if (field_ind < 0) {
        php3_error(E_WARNING,"Unkown field <%s>", field);
        RETURN_FALSE;
    }

    if (field_ind > fieldcount || field_ind <= 0) {
        php3_error(E_WARNING,"ifx_result : invalid field index");
        RETURN_FALSE;
    }
    
    EXEC SQL GET DESCRIPTOR :descrpid VALUE :field_ind :fieldtype = TYPE,
                                                       :fieldleng = LENGTH,
                                                       :indicator = INDICATOR;
    if (ifx_check() < 0) {
        php3_error(E_WARNING,"ifx_result : get descriptor for field %d fails (%s)",
                              field_ind,
                              ifx_error(ifx));
        RETURN_FALSE;
    }
    if (indicator == -1) {        /* NULL */
        return_value->type = IS_STRING;
        return_value->value.str.len = strlen("NULL");
        return_value->value.str.val = 
            (char *)safe_estrndup("NULL",return_value->value.str.len);
        return;
    }
    
    i = field_ind;    /* field_index */
        
    char_data = NULL;
    switch (fieldtype) {
        case SQLSERIAL  : 
        case SQLINT     :
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :int_data = DATA;
            long_data = int_data;
            sprintf(string_data,"%ld", long_data); 
            break;
        case SQLSMINT   :
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :short_data = DATA;
            long_data = short_data;
            sprintf(string_data,"%ld", long_data); 
            break;
        case SQLSMFLOAT :
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :float_data = DATA;
            double_data = float_data;
            sprintf(string_data,"%17.17g", double_data);
            break;
        case SQLFLOAT   :
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :double_data = DATA;
            sprintf(string_data,"%17.17g", double_data);
            break;
        case SQLDECIMAL :
        case SQLMONEY   :
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :dec_data = DATA;
            memset(string_data, 0x20, 64);
            dectoasc(&dec_data, string_data, 63, -1);
            for (p =string_data; *p != ' '; ++p) ;
            *p = 0;                
            break;
        case SQLDATE    :
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :date_data = DATA;
            rdatestr(date_data, string_data); 
            break;
        case SQLDTIME   :
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :dt_data = DATA;
            dttoasc(&dt_data, string_data); 
            break;
        case SQLINTERVAL:
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :intvl_data = DATA;
            intoasc(&intvl_data, string_data); 
            break;
        case SQLCHAR    :
        case SQLVCHAR   :
            if ((char_data = (char *)emalloc(fieldleng + 1)) == NULL) {
                php3_error(E_WARNING, "Out of memory");
                RETURN_FALSE;
            }
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :char_data = DATA;
            break;
        case SQLTEXT    :    /* is returned as if a long varchar */
            locator = Ifx_Result->blob_data[locind];
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
            if (locator->loc_status < 0) {  /* blob too large */   
                free(locator->loc_buffer);
                locator->loc_bufsize = locator->loc_size + 4;
                locator->loc_buffer = (char *)malloc(locator->loc_bufsize + 4);
                if (locator->loc_buffer == NULL) {
                    php3_error(E_WARNING,"no memory (%d bytes) for blob",
                               locator->loc_bufsize);
                    RETURN_FALSE;
                }
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
            }
            /* return immediately here to prevent the free after the case */
            p = locator->loc_buffer;            
            return_value->value.str.len = locator->loc_size;
            return_value->value.str.val = 
                    (char *) safe_estrndup(p, return_value->value.str.len);
            return_value->type = IS_STRING;
            return;
            break;
        case SQLBYTES   :    /* is dumped into a file, returns filename */
            locator = Ifx_Result->blob_data[locind];
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
            if (locator->loc_status < 0) {  /* blob too large */   
                free(locator->loc_buffer);
                locator->loc_bufsize = locator->loc_size + 4;
                locator->loc_buffer = (char *)malloc(locator->loc_bufsize + 4);
                if (locator->loc_buffer == NULL) {
                    php3_error(E_WARNING,"no memory (%d bytes) for blob",
                               locator->loc_bufsize);
                    RETURN_FALSE;
            }
                EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
            }
            /* dump blob in temp file */
            sprintf(string_data,"blb%d", locind);
            blobfilename = tempnam(blobdir,string_data);
            fp = fopen(blobfilename, "wb");
            if (fp == NULL) {
                php3_error(E_WARNING, "can't create blobfile %s (%d)",
                            blobfilename,
                            errno);
            }
            if (fwrite(locator->loc_buffer,
                       locator->loc_size, 1, fp) != 1) {
                php3_error(E_WARNING, "can't write blobfile %s (%d)",
                            blobfilename,
                            errno);
            }
            fclose(fp);
            /* return filename and filesize */
            sprintf(string_data,"@BlobFile@%s@%d@", blobfilename, locator->loc_size);
            free(blobfilename);
            break;
        default         :
            sprintf(string_data,"ESQL/C : %s : unsupported field type[%d]",
                    fieldname,
                    fieldleng);
            break;
    }

    if (char_data != NULL)
        p = char_data;
    else
        p = string_data;
        
    if (php3_ini.magic_quotes_runtime) {
        return_value->value.str.val = 
            _php3_addslashes(p, strlen(p), &return_value->value.str.len, 0);
    } else {    
        return_value->value.str.len = strlen(p);
        return_value->value.str.val = 
            (char *) safe_estrndup(p, return_value->value.str.len);
    }

    if (char_data != NULL) efree(char_data);
    char_data = NULL;
    
    return_value->type = IS_STRING;
}



void php3_ifx_htmltbl_result(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *result, *arg2;
    IFX_RES *Ifx_Result;

EXEC SQL BEGIN DECLARE SECTION;
    char  *ifx;                 /* connection ID    */
    char  *cursorid;            /* query cursor id  */
    char  *statemid;            /* statement id     */
    char  *descrpid;            /* descriptor id    */
    char  *statement;           /* query text       */
    int   fieldcount;           /* field count      */
    int   i;                    /* an index         */ 
    char  fieldname[64];        /* fieldname        */
    short fieldtype;            /* field type       */
    int   field_ind;            /* the field index  */
    int   fieldleng;            /* field length     */
    char  *field;               /* the field        */

    short     indicator;
    int       int_data;
    char      *char_data = NULL;
    long      date_data;
    interval  intvl_data = {0};
    datetime  dt_data = {0};
    decimal   dec_data = {0};
    short     short_data;
    float     float_data;
    double    double_data;
    loc_t     *locator;
EXEC SQL END DECLARE SECTION;

    int type;
    int num_fields;
    
    char string_data[256];
    long long_data;
    char *p;
    int  locind;
    FILE *fp;
    char *blobfilename;            
    char *blobdir;
    char *table_options;
    int  moredata;
            
    Informix_TLS_VARS;

    
    switch (ARG_COUNT(ht)) {
    	case 1:
            if (getParameters(ht, 1, &result)==FAILURE) {
                RETURN_FALSE;
            }
            table_options = NULL;
            break;
        case 2:
            if (getParameters(ht, 2, &result, &arg2)==FAILURE) {
                RETURN_FALSE;
            }
            table_options = arg2->value.str.val;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    convert_to_long(result);
    Ifx_Result = (IFX_RES *) php3_list_find(result->value.lval,&type);
    
    if (type!=Informix_GLOBAL(php3_ifx_module).le_result) {
        php3_error(E_WARNING,
                   "%d is not a Informix result index",result->value.lval);
        RETURN_FALSE;
    }

    if (strcmp(Ifx_Result->cursorid,"") == 0) {
        php3_error(E_WARNING,"Not a select cursor !");
        RETURN_FALSE;
    }

    ifx        = Ifx_Result->connecid;
    cursorid   = Ifx_Result->cursorid;
    statemid   = Ifx_Result->statemid;
    descrpid   = Ifx_Result->descrpid;
    fieldcount = Ifx_Result->numcols;

    /* try to fetch the first row */            
    EXEC SQL FETCH :cursorid USING SQL DESCRIPTOR :descrpid;
    switch (ifx_check()) {
        case IFX_ERROR:
            php3_error(E_WARNING,
                       "Can not fetch next row on cursor %s (%s)",
                       ifx_error(ifx),
                       cursorid);
            RETURN_FALSE;
            break;
        case IFX_NODATA:
            moredata = 0;
            break;
        default:
            Ifx_Result->rowid = moredata = 1;
            break;
    }
    
    if(! moredata) {
        php3_printf("<h2>No rows found</h2>\n");
        RETURN_LONG(0);
    }
    num_fields = fieldcount;

    /* start table tag */
    if (table_options == NULL)
        php3_printf("<table><tr>");
    else
        php3_printf("<table %s><tr>", table_options);

    /* table headings */
    for (i = 1; i <= num_fields; i++) {
        EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :fieldname = NAME;
        if (ifx_check() < 0) {
            php3_error(E_WARNING,"Get descriptor (field # %d) fails (%s)",
                                 i,
                                 ifx_error(ifx));
            RETURN_FALSE;
        }
            
        p = fieldname;         /* Capitalize and rtrim fieldname */
        *p = toupper(*p);
        while ((*p != ' ') && (p < &fieldname[sizeof(fieldname) - 1])) ++p;
        *p = 0;
        
        php3_printf("<th>%s</th>", fieldname);
    }    
    php3_printf("</tr>\n");
    
    /* start spitting out rows untill none left */    
    while (moredata) { 
        printf("<tr>");
        locind = 0;
        for (i = 1; i <= num_fields; i++) {
            EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :fieldtype = TYPE,
                                                       :fieldname = NAME,
                                                       :fieldleng = LENGTH,
                                                       :indicator = INDICATOR;
            if (ifx_check() < 0) {
                php3_error(E_WARNING,"Get descriptor (field # %d) fails (%s)",
                                     i,
                                     ifx_error(ifx));
                RETURN_FALSE;
            }
            
            p = fieldname;         /* rtrim fieldname */
            while ((*p != ' ') && (p < &fieldname[sizeof(fieldname) - 1])) ++p;
            *p = 0;
    
            if (indicator == -1) {        /* NULL */
                php3_printf("<td>NULL</td>");
                continue;
            }
            switch (fieldtype) {
                case SQLSERIAL  : 
                case SQLINT     :
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :int_data = DATA;
                    long_data = int_data;
                    sprintf(string_data,"%ld", long_data); 
                    php3_printf("<td>%s</td>", string_data);
                    break;
                case SQLSMINT   :
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :short_data = DATA;
                    long_data = short_data;
                    sprintf(string_data,"%ld", long_data); 
                    php3_printf("<td>%s</td>", string_data);
                    break;
                case SQLDECIMAL :
                case SQLMONEY   :
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :dec_data = DATA;
                    memset(string_data, 0x20, 64);
                    dectoasc(&dec_data, string_data, 63, -1);
                    for (p =string_data; *p != ' '; ++p) ;
                    *p = 0;                
                    php3_printf("<td>%s</td>", string_data);
                    break;
                case SQLSMFLOAT :
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :float_data = DATA;
                    double_data = float_data;
                    sprintf(string_data,"%17.17g", double_data);
                    php3_printf("<td>%s</td>", string_data);
                    break;
                case SQLFLOAT   :
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :double_data = DATA;
                    sprintf(string_data,"%17.17g", double_data);
                    php3_printf("<td>%s</td>", string_data);
                    break;
                case SQLDATE    :
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :date_data = DATA;
                    rdatestr(date_data, string_data); 
                    php3_printf("<td>%s</td>", string_data);
                    break;
                case SQLDTIME   :
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :dt_data = DATA;
                    dttoasc(&dt_data, string_data); 
                    php3_printf("<td>%s</td>", string_data);
                    break;
                case SQLINTERVAL:
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :intvl_data = DATA;
                    intoasc(&intvl_data, string_data); 
                    php3_printf("<td>%s</td>", string_data);
                    break;
                case SQLCHAR    :
                case SQLVCHAR   :
                    if ((char_data = (char *)emalloc(fieldleng + 1)) == NULL) {
                        php3_error(E_WARNING, "Out of memory");
                        RETURN_FALSE;
                    }
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :char_data = DATA;
                    php3_printf("<td>%s</td>", char_data);
                    efree(char_data);
                    char_data = NULL;
                    break;
                case SQLTEXT    :    /* is returned as if a long varchar */
                    locator = Ifx_Result->blob_data[locind];
                    ++locind;
                    EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
                    if (locator->loc_status < 0) {  /* blob too large */   
                        free(locator->loc_buffer);
                        locator->loc_bufsize = locator->loc_size + 4;
                        locator->loc_buffer = (char *)malloc(locator->loc_bufsize + 4);
                        if (locator->loc_buffer == NULL) {
                            php3_error(E_WARNING,"no memory (%d bytes) for blob",
                                       locator->loc_bufsize);
                            RETURN_FALSE;
                        }
                        EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :*locator = DATA;
                    }
                    php3_printf("<td>%s</td>", locator->loc_buffer);
                    break;
                case SQLBYTES   :   
                    ++locind;
                    php3_printf("<td>(BLOB)</td>");
                    break;
                default         :
                    sprintf(string_data,"ESQL/C : %s : unsupported field type[%d]",
                            fieldname,
                            fieldleng);
                    php3_printf("<td>%s</td>", string_data);
                    break;
            }
                
            continue;
        }
        php3_printf("</tr>\n");    
        /* fetch next row */ 
        EXEC SQL FETCH :cursorid USING SQL DESCRIPTOR :descrpid;
        switch (ifx_check()) {
            case IFX_ERROR:
                php3_error(E_WARNING,
                           "Can not fetch next row on cursor %s (%s)",
                           ifx_error(ifx),
                           cursorid);
                RETURN_FALSE;
                break;
            case IFX_NODATA:
                moredata = 0;
                break;
            default:
                break;
        }
        Ifx_Result->rowid++;
          
    } /* endwhile (moredata); */    
    php3_printf("</table>\n");
    
    RETURN_LONG(Ifx_Result->rowid);

}


void php3_ifx_fieldtypes(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *result, *arg2;
    IFX_RES *Ifx_Result;

EXEC SQL BEGIN DECLARE SECTION;
    char  *ifx;                 /* connection ID    */
    char  *cursorid;            /* query cursor id  */
    char  *statemid;            /* statement id     */
    char  *descrpid;            /* descriptor id    */
    char  *statement;           /* query text       */
    int   fieldcount;           /* field count      */
    int   i;                    /* an index         */ 
    char  fieldname[64];        /* fieldname        */
    short fieldtype;            /* field type       */
    int   field_ind;            /* the field index  */
    int   fieldleng;            /* field length     */
    char  *field;               /* the field        */

    short     indicator;
    int       int_data;
    char      *char_data = NULL;
    long      date_data;
    interval  intvl_data = {0};
    datetime  dt_data = {0};
    decimal   dec_data = {0};
    short     short_data;
    float     float_data;
    double    double_data;
    loc_t     *locator;
EXEC SQL END DECLARE SECTION;

    int type;
    int num_fields;
    
    char string_data[256];
    long long_data;
    char *p;
    int  locind;
    FILE *fp;
    char *blobfilename;            
    char *blobdir;
    char *table_options;
    int  moredata;
            
    Informix_TLS_VARS;

    
    switch (ARG_COUNT(ht)) {
    	case 1:
            if (getParameters(ht, 1, &result)==FAILURE) {
                RETURN_FALSE;
            }
            table_options = NULL;
            break;
        case 2:
            if (getParameters(ht, 2, &result, &arg2)==FAILURE) {
                RETURN_FALSE;
            }
            table_options = arg2->value.str.val;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    convert_to_long(result);
    Ifx_Result = (IFX_RES *) php3_list_find(result->value.lval,&type);
    
    if (type!=Informix_GLOBAL(php3_ifx_module).le_result) {
        php3_error(E_WARNING,
                   "%d is not a Informix result index",result->value.lval);
        RETURN_FALSE;
    }

    if (strcmp(Ifx_Result->cursorid,"") == 0) {
        php3_error(E_WARNING,"Not a select cursor !");
        RETURN_FALSE;
    }

    ifx        = Ifx_Result->connecid;
    cursorid   = Ifx_Result->cursorid;
    statemid   = Ifx_Result->statemid;
    descrpid   = Ifx_Result->descrpid;
    fieldcount = Ifx_Result->numcols;

    if (array_init(return_value)==FAILURE) {
        RETURN_FALSE;
    }
    num_fields = fieldcount;
    for (i = 1; i <= num_fields; i++) {
        EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :fieldname = NAME,
                                                   :fieldtype = TYPE;
        if (ifx_check() < 0) {
            php3_error(E_WARNING,"Get descriptor (field # %d) fails (%s)",
                                 i,
                                 ifx_error(ifx));
            RETURN_FALSE;
        }
            
        p = fieldname;         /* rtrim fieldname */
        while ((*p != ' ') && (p < &fieldname[sizeof(fieldname) - 1])) ++p;
        *p = 0;
        
        switch (fieldtype) {
            case SQLSERIAL  : 
                char_data = "SQLSERIAL";
                break;
            case SQLINT     :
                char_data = "SQLINT";
                break;
            case SQLSMINT   :
                char_data = "SQLSMINT";
                break;
            case SQLDECIMAL :
                char_data = "SQLDECIMAL";
                break;
            case SQLMONEY   :
                char_data = "SQLMONEY";
                break;
            case SQLSMFLOAT :
                char_data = "SQLSMFLOAT";
                break;
            case SQLFLOAT   :
                char_data = "SQLFLOAT";
                break;
            case SQLDATE    :
                char_data = "SQLDATE";
                break;
            case SQLDTIME   :
                char_data = "SQLDTIME";
                break;
            case SQLINTERVAL:
                char_data = "SQLINTERVAL";
                break;
            case SQLCHAR    :
                char_data = "SQLCHAR";
                break;
            case SQLVCHAR   :
                char_data = "SQLVCHAR";
                break;
            case SQLTEXT    :
                char_data = "SQLTEXT";
                break;
            case SQLBYTES   :   
                char_data = "SQLBYTES";
                break;
            default         :
                char_data = "UNKNOWN";
                break;
        } /* switch (fieldtype) */
                
        add_assoc_string(return_value, fieldname, char_data, DUP);

    }   /* for() */ 
    
}

void php3_ifx_fieldproperties(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *result, *arg2;
    IFX_RES *Ifx_Result;

EXEC SQL BEGIN DECLARE SECTION;
    char  *ifx;                 /* connection ID    */
    char  *cursorid;            /* query cursor id  */
    char  *statemid;            /* statement id     */
    char  *descrpid;            /* descriptor id    */
    char  *statement;           /* query text       */
    int   fieldcount;           /* field count      */
    int   i;                    /* an index         */ 
    char  fieldname[64];        /* fieldname        */
    short fieldtype;            /* field type       */
    int   field_ind;            /* the field index  */
    int   fieldleng;            /* field length     */
    char  *field;               /* the field        */

    short     indicator;
    int       int_data;
    char      *char_data = NULL;
    long      date_data;
    interval  intvl_data = {0};
    datetime  dt_data = {0};
    decimal   dec_data = {0};
    short     short_data;
    float     float_data;
    double    double_data;
    loc_t     *locator;
    int       size;
    int       precision;
    int       scale;
EXEC SQL END DECLARE SECTION;

    int type;
    int num_fields;
    
    char string_data[256];
    long long_data;
    char *p;
    int  locind;
    FILE *fp;
    char *blobfilename;            
    char *blobdir;
    char *table_options;
    int  moredata;
            
    Informix_TLS_VARS;

    
    switch (ARG_COUNT(ht)) {
    	case 1:
            if (getParameters(ht, 1, &result)==FAILURE) {
                RETURN_FALSE;
            }
            table_options = NULL;
            break;
        case 2:
            if (getParameters(ht, 2, &result, &arg2)==FAILURE) {
                RETURN_FALSE;
            }
            table_options = arg2->value.str.val;
            break;
        default:
            WRONG_PARAM_COUNT;
            break;
    }
    
    convert_to_long(result);
    Ifx_Result = (IFX_RES *) php3_list_find(result->value.lval,&type);
    
    if (type!=Informix_GLOBAL(php3_ifx_module).le_result) {
        php3_error(E_WARNING,
                   "%d is not a Informix result index",result->value.lval);
        RETURN_FALSE;
    }

    if (strcmp(Ifx_Result->cursorid,"") == 0) {
        php3_error(E_WARNING,"Not a select cursor !");
        RETURN_FALSE;
    }

    ifx        = Ifx_Result->connecid;
    cursorid   = Ifx_Result->cursorid;
    statemid   = Ifx_Result->statemid;
    descrpid   = Ifx_Result->descrpid;
    fieldcount = Ifx_Result->numcols;

    if (array_init(return_value)==FAILURE) {
        RETURN_FALSE;
    }
    num_fields = fieldcount;
    for (i = 1; i <= num_fields; i++) {
        EXEC SQL GET DESCRIPTOR :descrpid VALUE :i :fieldname = NAME,
                                                   :fieldtype = TYPE,
                                                   :size = LENGTH,
                                                   :precision = PRECISION,
                                                   :scale = SCALE;
        if (ifx_check() < 0) {
            php3_error(E_WARNING,"Get descriptor (field # %d) fails (%s)",
                                 i,
                                 ifx_error(ifx));
            RETURN_FALSE;
        }
            
        p = fieldname;         /* rtrim fieldname */
        while ((*p != ' ') && (p < &fieldname[sizeof(fieldname) - 1])) ++p;
        *p = 0;
        switch (fieldtype) {
            case SQLSERIAL  : 
                char_data = "SQLSERIAL";
                break;
            case SQLINT     :
                char_data = "SQLINT";
                break;
            case SQLSMINT   :
                char_data = "SQLSMINT";
                break;
            case SQLDECIMAL :
                char_data = "SQLDECIMAL";
                break;
            case SQLMONEY   :
                char_data = "SQLMONEY";
                break;
            case SQLSMFLOAT :
                char_data = "SQLSMFLOAT";
                break;
            case SQLFLOAT   :
                char_data = "SQLFLOAT";
                break;
            case SQLDATE    :
                char_data = "SQLDATE";
                break;
            case SQLDTIME   :
                char_data = "SQLDTIME";
                break;
            case SQLINTERVAL:
                char_data = "SQLINTERVAL";
                break;
            case SQLCHAR    :
                char_data = "SQLCHAR";
                break;
            case SQLVCHAR   :
                char_data = "SQLVCHAR";
                break;
            case SQLTEXT    :
                char_data = "SQLTEXT";
                break;
            case SQLBYTES   :   
                char_data = "SQLBYTES";
                break;
            default         :
                char_data = "UNKNOWN";
                break;
        } /* switch (fieldtype) */

        sprintf(string_data,"%s;%d;%d;%d",char_data, size, precision, scale);       
        add_assoc_string(return_value, fieldname, string_data, DUP);

    }   /* for() */ 
    
}


void php3_ifx_num_rows(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *result;
    IFX_RES *Ifx_Result;
    int type;
    Informix_TLS_VARS;

    
    if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
        WRONG_PARAM_COUNT;
    }
    
    convert_to_long(result);
    Ifx_Result = (IFX_RES *) php3_list_find(result->value.lval,&type);
    
    if (type!=Informix_GLOBAL(php3_ifx_module).le_result) {
        php3_error(E_WARNING,"%d is not a Informix result index",
                   result->value.lval);
        RETURN_FALSE;
    }
    
    return_value->value.lval = -1;    /* not supported */
    return_value->type = IS_LONG;
}


void php3_ifx_num_fields(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *result;
    IFX_RES *Ifx_Result;
    int type;
    Informix_TLS_VARS;

    
    if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
        WRONG_PARAM_COUNT;
    }
    
    convert_to_long(result);
    Ifx_Result = (IFX_RES *) php3_list_find(result->value.lval,&type);
    
    if (type!=Informix_GLOBAL(php3_ifx_module).le_result) {
        php3_error(E_WARNING,"%d is not a Informix result index",
                   result->value.lval);
        RETURN_FALSE;
    }
    
    return_value->value.lval = Ifx_Result->numcols;
    return_value->type = IS_LONG;
}




void php3_ifx_fetch_object(INTERNAL_FUNCTION_PARAMETERS)
{

#if 0    /* not yet supported (blobs ?) */

    if (return_value->type==IS_ARRAY) {
        return_value->type=IS_OBJECT;
    }

#endif
}

void php3_ifx_free_result(INTERNAL_FUNCTION_PARAMETERS)
{
    pval *result;
    IFX_RES *Ifx_Result;
    
EXEC SQL BEGIN DECLARE SECTION;
    char *ifx;                /* connection ID    */
    char *cursorid;            /* query cursor id    */
    char *statemid;            /* statement id        */
    char *descrpid;            /* descriptor id    */
EXEC SQL END DECLARE SECTION;

    int type;

    int i, locind;
    
    Informix_TLS_VARS;

    if (ARG_COUNT(ht)!=1 || getParameters(ht, 1, &result)==FAILURE) {
        WRONG_PARAM_COUNT;
    }
    
    convert_to_long(result);
    if (result->value.lval==0) {
        RETURN_FALSE;
    }
    
    Ifx_Result = (IFX_RES *) php3_list_find(result->value.lval,&type);
    
    if (type!=Informix_GLOBAL(php3_ifx_module).le_result) {
        php3_error(E_WARNING,"%d is not a Informix result index",
                   result->value.lval);
        RETURN_FALSE;
    }
    
    cursorid   = Ifx_Result->cursorid;
    statemid   = Ifx_Result->statemid;
    descrpid   = Ifx_Result->descrpid;
    
    EXEC SQL free :statemid;
    if (strlen(cursorid) != 0)
        EXEC SQL free :cursorid;
    
    for (i = 0; i < MAXBLOBS; ++i) {
        if (Ifx_Result->blob_data[i] != NULL) {
            free(Ifx_Result->blob_data[i]->loc_buffer);
            free(Ifx_Result->blob_data[i]);
        }
    }
        
    EXEC SQL DEALLOCATE DESCRIPTOR :descrpid;
        
    php3_list_delete(result->value.lval);
    RETURN_TRUE;
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
