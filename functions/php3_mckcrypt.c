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
   | Authors: Timothy Whitfield <timothy@ametro.net>                      |
   | on behalf of American MetroComm Internet Services                    |
   +----------------------------------------------------------------------+
 */

/* 
 * php3_mckcrypt.c - Copyright 1998 Timothy Whitfield
 */
#include <stdio.h>
#include "php.h"
#include "internal_functions.h"

#include "php3_mckcrypt.h"

#if HAVE_MCK

#include "mckcrypt.h"

function_entry mckcrypt_functions[] = {
  {"mck_encr",php3_mckencr,NULL},
  {"mck_decr",php3_mckdecr,NULL},
  {"mck_base64_encode",php3_mckbase64_encode,NULL},
  {"mck_base64_decode",php3_mckbase64_decode,NULL},
  {NULL,NULL,NULL}
  };

php3_module_entry mckcrypt_module_entry = {
  "mckcrypt",mckcrypt_functions,NULL,NULL,NULL,NULL,NULL,
  STANDARD_MODULE_PROPERTIES
  };

#if COMPILE_DL
#include "dl/phpdl.h"
DLEXPORT php3_module_entry *get_module(void) { return &mckcrypt_module_entry; }
#endif

void php3_mckencr(INTERNAL_FUNCTION_PARAMETERS)
{
  pval *wmk,*sk,*inbuff;
  unsigned char *outbuff, *macbuff;
  unsigned int outAlloc, outLth;
  long errcode;
  
  if(ARG_COUNT(ht) != 3 ||
     getParameters(ht,3,&wmk,&sk,&inbuff)
       == FAILURE)
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string(wmk);
  convert_to_string(sk);
  convert_to_string(inbuff);

  outAlloc=inbuff->value.str.len+10;

  outbuff=(unsigned char *)emalloc(outAlloc);
  macbuff=(unsigned char *)emalloc(20);

  errcode=mck_encr(wmk->value.str.val,
                   sk->value.str.val,
                   inbuff->value.str.len+1,
                   inbuff->value.str.val,
                   outAlloc,
                   outbuff,
                   &outLth,
                   macbuff);

  if(array_init(return_value)==FAILURE)
  {
    php3_error(E_ERROR,"Unable to initialize array");
    RETURN_FALSE;
  }
  else
  {
    add_assoc_long(return_value,"errcode",errcode);
    if(!errcode)
    {
      add_assoc_stringl(return_value,"outbuff",outbuff,outLth,0);
      add_assoc_long(return_value,"outLth",outLth);
      add_assoc_stringl(return_value,"macbuff",macbuff,20,0);
    }
    else
    {
      efree(outbuff);
      efree(macbuff);
    }
  }
}

void php3_mckdecr(INTERNAL_FUNCTION_PARAMETERS)
{
  pval *wmk,*sk,*inbuff;
  unsigned char *outbuff, *macbuff;
  unsigned int outAlloc, outLth;
  long errcode;
  

  if(ARG_COUNT(ht) != 3 ||
     getParameters(ht,3,&wmk,&sk,&inbuff)
       == FAILURE)
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string(wmk);
  convert_to_string(sk);
  convert_to_string(inbuff);

  outAlloc=inbuff->value.str.len;

  outbuff=(unsigned char *)emalloc(outAlloc);
  macbuff=(unsigned char *)emalloc(20);

  errcode=mck_decr(wmk->value.str.val,
                   sk->value.str.val,
                   inbuff->value.str.len,
                   inbuff->value.str.val,
                   outAlloc,
                   outbuff,
                   &outLth,
                   macbuff);

  if(array_init(return_value)==FAILURE)
  {
    php3_error(E_ERROR,"Unable to initialize array");
    RETURN_FALSE;
  }
  else
  {
    add_assoc_long(return_value,"errcode",errcode);
    if(!errcode)
    {
      add_assoc_stringl(return_value,"outbuff",outbuff,outLth,0);
      add_assoc_long(return_value,"outLth",outLth);
      add_assoc_stringl(return_value,"macbuff",macbuff,20,0);
    }
    else
    {
      efree(outbuff);
      efree(macbuff);
    }
  }
}

void php3_mckbase64_encode(INTERNAL_FUNCTION_PARAMETERS)
{
  pval *inbuff;
  char *outbuff;
  long ret_length;

  if(ARG_COUNT(ht) != 1 ||
     getParameters(ht,1,&inbuff)
       == FAILURE)
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string(inbuff);

  outbuff=(char *)emalloc(
    base64_enc_size((unsigned int)inbuff->value.str.len));
  
  ret_length=base64_encode(outbuff,
     inbuff->value.str.val,inbuff->value.str.len);

  return_value->value.str.val=outbuff;
  return_value->value.str.len=ret_length;
  return_value->type=IS_STRING;
  
}

void php3_mckbase64_decode(INTERNAL_FUNCTION_PARAMETERS)
{
  pval *inbuff;
  char *outbuff;
  long ret_length;

  if(ARG_COUNT(ht) != 1 ||
     getParameters(ht,1,&inbuff)
       == FAILURE)
  {
    WRONG_PARAM_COUNT;
  }

  convert_to_string(inbuff);

  outbuff=(char *)emalloc(
    base64_dec_size((unsigned int)inbuff->value.str.len));
  
  ret_length=base64_decode(outbuff,
     inbuff->value.str.val,inbuff->value.str.len);

  return_value->value.str.val=outbuff;
  return_value->value.str.len=ret_length;
  return_value->type=IS_STRING;
  
}
#endif
