/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2000 PHP Development Team (See Credits file)      |
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
   | Authors: Kristian Koehntopp (kris@koehntopp.de)                      |
   +----------------------------------------------------------------------+
 */
 
/* $Id: recode.c,v 1.6 2000/01/01 04:31:16 sas Exp $ */


#include "php.h"
#include "internal_functions.h"
#include "php3_string.h"

#if HAVE_RECODE
#include "php3_recode.h"
#include "php3_list.h"

#ifdef HAVE_BROKEN_RECODE
extern char *program_name;
char *program_name = "php";
#endif

#define SAFE_STRING(s) ((s)?(s):"")

#define RECODE_GLOBAL(a) php3_recode_module.a
recode_module php3_recode_module;
extern int le_fp,le_pp;

function_entry recode_functions[] = {
	{"recode_string",	php3_recode_string,	NULL},
	{"recode_file",		php3_recode_file,	NULL},
	{NULL, NULL, NULL}
};

php3_module_entry recode_module_entry = {
	"Recode", 
	recode_functions, 
	php3_minit_recode, 
	php3_mshutdown_recode, 
	NULL,
	NULL, 
	php3_info_recode, 
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &recode__module_entry; }
#endif

#if APACHE
extern void timeout(int sig);
#endif

int php3_minit_recode(INIT_FUNC_ARGS)
{
	RECODE_GLOBAL(outer)   = recode_new_outer(true);
	if (RECODE_GLOBAL(outer) == NULL)
		return FAILURE;
	
	return SUCCESS;
}


int php3_mshutdown_recode(void){

	if (RECODE_GLOBAL(outer))
		recode_delete_outer(RECODE_GLOBAL(outer));

	return SUCCESS;
}


void php3_info_recode(void)
{
    php3_printf("$Revision: 1.6 $\n");
}

/* {{{ proto string recode_string(string request, string str)
   Recode string str according to request string */

void php3_recode_string(INTERNAL_FUNCTION_PARAMETERS)
{
	RECODE_REQUEST request = NULL;
	pval	*str;
	pval	*req;
	char	*r;
	bool	 success;
	
	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &req, &str) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(str);
	convert_to_string(req);

	request = recode_new_request(RECODE_GLOBAL(outer));
	if (request == NULL) {
		php3_error(E_WARNING, "Cannot allocate request structure");
		RETURN_FALSE;
	}

	success = recode_scan_request(request, req->value.str.val);
	if (!success) {
		php3_error(E_WARNING, "Illegal recode request.");
		goto error_exit;
	}
	
	r = recode_string(request, str->value.str.val);
	if (!r) {
		php3_error(E_WARNING, "Recoding failed.");
		goto error_exit;
	}

	RETVAL_STRING(r, 1);
	free(r);
	return;

error_exit:
	if (request)
		recode_delete_request(request);
	
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool recode_file(string request, int input, int output)
   Recode file input into file output according to request */
void php3_recode_file(INTERNAL_FUNCTION_PARAMETERS)
{
	RECODE_REQUEST request = NULL;
	int success;
	pval *req, *input, *output;
	int   in_id,  in_type;
	FILE *in_fp;
	int  out_id, out_type;
	FILE *out_fp;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &req, &input, &output) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(input);
	convert_to_long(output);
	in_id  = input->value.lval;
	out_id = output->value.lval;

	in_fp = php3_list_find(in_id,&in_type);
	if (!in_fp || (in_type!=GLOBAL(le_fp) && in_type!=GLOBAL(le_pp))) {
		php3_error(E_WARNING,"Unable to find input file identifier %d",in_id);
		RETURN_FALSE;
	}

	out_fp = php3_list_find(out_id,&out_type);
	if (!out_fp || (out_type!=GLOBAL(le_fp) && out_type!=GLOBAL(le_pp))) {
		php3_error(E_WARNING,"Unable to find output file identifier %d", out_id);
		RETURN_FALSE;
	}

	convert_to_string(req);

	request = recode_new_request(RECODE_GLOBAL(outer));
	if (request == NULL) {
		php3_error(E_WARNING, "Cannot allocate request structure");
		RETURN_FALSE;
	}

	success = recode_scan_request(request, req->value.str.val);
	if (!success) {
		php3_error(E_WARNING, "Illegal recode request.");
		goto error_exit;
	}
	
	success = recode_file_to_file(request, in_fp, out_fp);
	if (!success) {
		php3_error(E_WARNING, "Recoding failed.");
		goto error_exit;
	}

	if (request)
		recode_delete_request(request);
	RETURN_TRUE;

error_exit:
	if (request)
		recode_delete_request(request);
	
	RETURN_FALSE;
}
/* }}} */


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
