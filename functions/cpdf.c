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
   | Authors: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>              |
   +----------------------------------------------------------------------+
 */

/* $Id: cpdf.c,v 1.23 2000/02/14 10:09:20 kk Exp $ */
/* cpdflib.h -- C language API definitions for ClibPDF library
 * Copyright (C) 1998 FastIO Systems, All Rights Reserved.
*/

/* Note that there is no code from the cpdflib package in this file */

#include "php.h"
#include "internal_functions.h"
#include "php3_list.h"
#include "head.h"
#include <math.h>
#include "php3_cpdf.h"
#if HAVE_LIBGD
#include <gd.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#if WIN32|WINNT
# include <io.h>
# include <fcntl.h>
#endif

#if HAVE_CPDFLIB

#  define CPDF_GLOBAL(a) a
#  define CPDF_TLS_VARS
static int le_cpdf;
static int le_outline;

function_entry cpdf_functions[] = {
	{"cpdf_set_creator",		php3_cpdf_set_creator,		NULL},
	{"cpdf_set_title",			php3_cpdf_set_title,		NULL},
	{"cpdf_set_subject",		php3_cpdf_set_subject,		NULL},
	{"cpdf_set_keywords",		php3_cpdf_set_keywords,		NULL},
	{"cpdf_open",				php3_cpdf_open,				NULL},
	{"cpdf_close",				php3_cpdf_close,			NULL},
	{"cpdf_set_viewer_preferences",	php3_cpdf_set_viewer_preferences,NULL},
	{"cpdf_page_init",			php3_cpdf_page_init,		NULL},
	{"cpdf_finalize_page",			php3_cpdf_finalize_page,			NULL},
	{"cpdf_set_current_page",	php3_cpdf_set_current_page,	NULL},
	{"cpdf_begin_text",			php3_cpdf_begin_text,		NULL},
	{"cpdf_end_text",			php3_cpdf_end_text,			NULL},
	{"cpdf_show",				php3_cpdf_show,				NULL},
	{"cpdf_show_xy",			php3_cpdf_show_xy,			NULL},
	{"cpdf_text",				php3_cpdf_text,				NULL},
	{"cpdf_continue_text",		php3_cpdf_continue_text,	NULL},
	{"cpdf_set_font",			php3_cpdf_set_font,			NULL},
	{"cpdf_set_leading",		php3_cpdf_set_leading,		NULL},
	{"cpdf_set_text_rendering",	php3_cpdf_set_text_rendering,NULL},
	{"cpdf_set_horiz_scaling",	php3_cpdf_set_horiz_scaling,NULL},
	{"cpdf_set_text_rise",		php3_cpdf_set_text_rise,	NULL},
	{"cpdf_set_text_matrix",	php3_cpdf_set_text_matrix,	NULL},
	{"cpdf_set_text_pos",		php3_cpdf_set_text_pos,		NULL},
	{"cpdf_rotate_text",		php3_cpdf_rotate_text,		NULL},
	{"cpdf_set_char_spacing",	php3_cpdf_set_char_spacing,	NULL},
	{"cpdf_set_word_spacing",	php3_cpdf_set_word_spacing,	NULL},
	{"cpdf_stringwidth",		php3_cpdf_stringwidth,		NULL},
	{"cpdf_save",				php3_cpdf_save,				NULL},
	{"cpdf_restore",			php3_cpdf_restore,			NULL},
	{"cpdf_translate",			php3_cpdf_translate,		NULL},
	{"cpdf_scale",				php3_cpdf_scale,			NULL},
	{"cpdf_rotate",				php3_cpdf_rotate,			NULL},
	{"cpdf_setflat",			php3_cpdf_setflat,			NULL},
	{"cpdf_setlinejoin",		php3_cpdf_setlinejoin,		NULL},
	{"cpdf_setlinecap",			php3_cpdf_setlinecap,		NULL},
	{"cpdf_setmiterlimit",		php3_cpdf_setmiterlimit,	NULL},
	{"cpdf_setlinewidth",		php3_cpdf_setlinewidth,		NULL},
	{"cpdf_setdash",			php3_cpdf_setdash,			NULL},
	{"cpdf_moveto",				php3_cpdf_moveto,			NULL},
	{"cpdf_rmoveto",			php3_cpdf_rmoveto,			NULL},
	{"cpdf_lineto",				php3_cpdf_lineto,			NULL},
	{"cpdf_rlineto",			php3_cpdf_rlineto,			NULL},
	{"cpdf_curveto",			php3_cpdf_curveto,			NULL},
	{"cpdf_circle",				php3_cpdf_circle,			NULL},
	{"cpdf_arc",				php3_cpdf_arc,				NULL},
	{"cpdf_rect",				php3_cpdf_rect,				NULL},
	{"cpdf_newpath",			php3_cpdf_newpath,			NULL},
	{"cpdf_closepath",			php3_cpdf_closepath,		NULL},
	{"cpdf_stroke",				php3_cpdf_stroke,			NULL},
	{"cpdf_closepath_stroke",	php3_cpdf_closepath_stroke,	NULL},
	{"cpdf_fill",				php3_cpdf_fill,  			NULL},
	{"cpdf_fill_stroke",		php3_cpdf_fill_stroke,  	NULL},
	{"cpdf_closepath_fill_stroke",	php3_cpdf_closepath_fill_stroke, NULL},
	{"cpdf_clip",				php3_cpdf_clip,				NULL},
	{"cpdf_setgray_fill",		php3_cpdf_setgray_fill,		NULL},
	{"cpdf_setgray_stroke",		php3_cpdf_setgray_stroke,	NULL},
	{"cpdf_setgray",			php3_cpdf_setgray,			NULL},
	{"cpdf_setrgbcolor_fill",	php3_cpdf_setrgbcolor_fill,	NULL},
	{"cpdf_setrgbcolor_stroke",	php3_cpdf_setrgbcolor_stroke,NULL},
	{"cpdf_setrgbcolor",		php3_cpdf_setrgbcolor,		NULL},
	{"cpdf_set_page_animation",	php3_cpdf_set_page_animation,	NULL},
	{"cpdf_finalize",			php3_cpdf_finalize,			NULL},
	{"cpdf_output_buffer",		php3_cpdf_output_buffer,	NULL},
	{"cpdf_save_to_file",		php3_cpdf_save_to_file,		NULL},
	{"cpdf_import_jpeg",		php3_cpdf_import_jpeg,		NULL},
#if HAVE_LIBGD
	{"cpdf_place_inline_image",	php3_cpdf_place_inline_image,	NULL},
#endif
	{"cpdf_add_annotation",		php3_cpdf_add_annotation,	NULL},
	{"cpdf_add_outline",		php3_cpdf_add_outline,	NULL},
	{"cpdf_set_action_url",		php3_cpdf_set_action_url,	NULL},
	{NULL, NULL, NULL}
};

php3_module_entry cpdf_module_entry = {
	"cpdf", cpdf_functions, php3_minit_cpdf, php3_mend_cpdf, php3_rinit_cpdf, NULL, php3_info_cpdf, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
#include "dl/phpdl.h"
DLEXPORT php3_module_entry *get_module(void) { return &cpdf_module_entry; }
#endif

static void _free_outline(CPDFoutlineEntry *outline)
{
}

static void _free_doc(int *pdf)
{
	cpdf_close();
}

int php3_minit_cpdf(INIT_FUNC_ARGS)
{
	CPDF_GLOBAL(le_outline) = register_list_destructors(_free_outline, NULL);
	CPDF_GLOBAL(le_cpdf) = register_list_destructors(_free_doc, NULL);
	return SUCCESS;
}

int php3_rinit_cpdf(INIT_FUNC_ARGS)
{
/*	CPDF_GLOBAL(le_outline) = NULL; */
	return SUCCESS;
}

void php3_info_cpdf(void) {
	/* need to use a PHPAPI function here because it is external module in windows */
	php3_printf("ClibPDF");
}

int php3_mend_cpdf(void){
	return SUCCESS;
}

/* {{{ proto bool cpdf_set_creator(int pdfdoc, string creator)
   Sets the creator field */
void php3_cpdf_set_creator(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if (!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	cpdf_setCreator(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool cpdf_set_title(int pdfptr, string title)
   Fills the title field of the info structure */
void php3_cpdf_set_title(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if (!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	cpdf_setTitle(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool cpdf_set_subject(int pdfptr, string subject)
   Fills the subject field of the info structure */
void php3_cpdf_set_subject(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if (!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	cpdf_setSubject(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool cpdf_set_keywords(int pdfptr, string keywords)
   Fills the keywords field of the info structure */
void php3_cpdf_set_keywords(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;


	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if (!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d (type=%d)",id, type);
		RETURN_FALSE;
	}

	cpdf_setKeywords(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_viewer_preferences(int pdfdoc, int pagemode)
   How to show the document by the viewer */
void php3_cpdf_set_viewer_preferences(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[6];
	int id, type, pagemode;
	int argc;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if(argc < 1 || argc > 2)
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_long(argv[1]);
	id=argv[0]->value.lval;
	pagemode=argv[1]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setViewerPreferences(pagemode, 0, 0, 0, 0, 0, 0, pagemode);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int cpdf_open(int compression [, string filename])
   Opens a new pdf document */
void php3_cpdf_open(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type, argc;
	FILE *fp;
	int *cpdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	switch(argc) {
		case 1:
			if (getParameters(ht, 1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if (getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);

	cpdf_open(0);
//	*cpdf = 1;
	if(!cpdf)
		RETURN_FALSE;
	if(arg1->value.lval == 1)
		cpdf_enableCompression(YES);
	else
		cpdf_enableCompression(NO);

	if(argc > 1) {
		convert_to_string(arg2);
#if APACHE
		if(strcmp(arg2->value.str.val, "-") == 0)
			php3_error(E_WARNING,"Writing to stdout as described in the ClibPDF manual is not possible if php3 is used as an Apache module. Write to a memory stream and use cpdf_output_buffer() instead.");
#endif
		cpdf_setOutputFilename(arg2->value.str.val);
	}
	cpdf_init();

	id = php3_list_insert(cpdf,CPDF_GLOBAL(le_cpdf));
	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto void cpdf_close(int pdfdoc)
   Closes the pdf document */
void php3_cpdf_close(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}
	php3_list_delete(id);

	RETURN_TRUE;
}
/* }}} */

#define BUFFERLEN 40
/* {{{ proto void cpdf_page_init(int pdfdoc, int pagenr, int orientation, int height, int width [, double unit])
   Starts page */
void php3_cpdf_page_init(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[6];
	int id, type, pagenr, orientation;
	int height, width, argc;
	char buffer[BUFFERLEN];
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if(argc < 5 || argc > 6)
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_long(argv[1]);
	convert_to_long(argv[2]);
	convert_to_long(argv[3]);
	convert_to_long(argv[4]);
	id=argv[0]->value.lval;
	pagenr=argv[1]->value.lval;
	orientation=argv[2]->value.lval;
	height = argv[3]->value.lval;
	width = argv[4]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 5) {
		convert_to_double(argv[5]);
		if(argv[5]->value.dval > 0.0)
			cpdf_setDefaultDomainUnit(argv[5]->value.dval);
	}
	snprintf(buffer, BUFFERLEN, "0 0 %d %d", width, height);
	cpdf_pageInit(pagenr, orientation, buffer, buffer);

	RETURN_TRUE;
}
/* }}} */
#undef BUFFERLEN

/* {{{ proto void cpdf_finalize_page(int pdfdoc, int pagenr)
   Ends the page to save memory */
void php3_cpdf_finalize_page(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type, pagenr;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pagenr=arg2->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_finalizePage(pagenr);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_current_page(int pdfdoc, int pagenr)
   Sets page for output */
void php3_cpdf_set_current_page(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type, pagenr;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pagenr=arg2->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setCurrentPage(pagenr);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_begin_text(int pdfdoc)
   Starts text section */
void php3_cpdf_begin_text(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_beginText(0);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_end_text(int pdfdoc)
   Ends text section */
void php3_cpdf_end_text(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_endText();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_show(int pdfdoc, string text)
   Output text at current position */
void php3_cpdf_show(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_textShow(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_show_xy(int pdfdoc, string text, double x-koor, double y-koor [, int mode])
   Output text at position */
void php3_cpdf_show_xy(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[5];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 4) || (argc > 5))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_string(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc == 5) {
		convert_to_long(argv[4]);
		mode = argv[4]->value.lval;
	}
	if(mode == 1)
		cpdf_rawText((float) argv[2]->value.dval, (float) argv[3]->value.dval, 0.0, argv[1]->value.str.val);
	else
		cpdf_text((float) argv[2]->value.dval, (float) argv[3]->value.dval, 0.0, argv[1]->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_continue_text(int pdfdoc, string text)
   Output text in next line */
void php3_cpdf_continue_text(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_textCRLFshow(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_text(int pdfdoc, string text [, double x-koor, double y-koor [, int mode [, double orientation [, int alignmode]]]])
   Output text */
void php3_cpdf_text(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[7];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 2) || (argc == 3) || (argc > 7) || getParametersArray(ht, argc, argv) == FAILURE)
			WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_string(argv[1]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 4) {
		convert_to_long(argv[4]);
		mode = argv[2]->value.lval;
	}
	switch(argc) {
		case 2:
			cpdf_textShow(argv[1]->value.str.val);
			break;
		case 4:
			convert_to_double(argv[2]);
			convert_to_double(argv[3]);
			cpdf_text((float) argv[2]->value.dval,
			          (float) argv[3]->value.dval,
			           0.0,
			           argv[1]->value.str.val);
			break;
		case 5:
			convert_to_double(argv[2]);
			convert_to_double(argv[3]);
			if(mode == 1)
				cpdf_rawText((float) argv[2]->value.dval,
			             	(float) argv[3]->value.dval,
			             	0.0,
			              	argv[1]->value.str.val);
			else
				cpdf_text((float) argv[2]->value.dval,
				          (float) argv[3]->value.dval,
				          0.0,
				          argv[1]->value.str.val);
			break;
		case 6:
			convert_to_double(argv[2]);
			convert_to_double(argv[3]);
			convert_to_double(argv[5]);
			if(mode == 1)
				cpdf_rawText((float) argv[2]->value.dval,
			             	(float) argv[3]->value.dval,
			             	(float) argv[5]->value.dval,
			              	argv[1]->value.str.val);
			else
				cpdf_text((float) argv[2]->value.dval,
			             	(float) argv[3]->value.dval,
			             	(float) argv[5]->value.dval,
			              	argv[1]->value.str.val);
			break;
		case 7:
			convert_to_double(argv[2]);
			convert_to_double(argv[3]);
			convert_to_double(argv[5]);
			convert_to_long(argv[6]);
			if(mode == 1)
				cpdf_rawTextAligned((float) argv[2]->value.dval,
			             	(float) argv[3]->value.dval,
			             	(float) argv[5]->value.dval,
			             	argv[6]->value.lval,
			              	argv[1]->value.str.val);
			else
				cpdf_textAligned((float) argv[2]->value.dval,
			             	(float) argv[3]->value.dval,
			             	(float) argv[5]->value.dval,
			             	argv[6]->value.lval,
			              	argv[1]->value.str.val);
			break;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_font(int pdfdoc, string font, double size, int encoding)
   Select the current font face, size and encoding */
void php3_cpdf_set_font(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	convert_to_double(arg3);
	convert_to_long(arg4);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}
	
	if(arg4->value.lval > 6) {
		php3_error(E_WARNING,"Font encoding set to 5");
		arg4->value.lval = 5;
	}
	switch(arg4->value.lval) {
		case 2:
			cpdf_setFont(arg2->value.str.val, "MacRomanEncoding", (float) arg3->value.dval);
			break;
		case 3:
			cpdf_setFont(arg2->value.str.val, "MacExpertEncoding", (float) arg3->value.dval);
			break;
		case 4:
			cpdf_setFont(arg2->value.str.val, "WinAnsiEncoding", (float) arg3->value.dval);
			break;
		default:
			cpdf_setFont(arg2->value.str.val, NULL, (float) arg3->value.dval);
			break;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_leading(int pdfdoc, double distance)
   Sets distance between text lines */
void php3_cpdf_set_leading(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}
	
	cpdf_setTextLeading((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_text_rendering(int pdfdoc, int rendermode)
   Determines how text is rendered */
void php3_cpdf_set_text_rendering(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}
	
	cpdf_setTextRenderingMode(arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_horiz_scaling(int pdfdoc, double scale)
   Sets horizontal scaling of text */
void php3_cpdf_set_horiz_scaling(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}
	
	cpdf_setHorizontalScaling((float) arg2->value.dval * 100.0);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_text_rise(int pdfdoc, double value)
   Sets the text rise */
void php3_cpdf_set_text_rise(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}
	
	cpdf_setTextRise((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_text_matrix(int pdfdoc, arry matrix)
   Sets the text matrix */
void php3_cpdf_set_text_matrix(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *data;
	int id, type, i;
	HashTable *matrix;
	int *pdf;
	float *pdfmatrixptr;
	float pdfmatrix[6];
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_array(arg2);
	id=arg1->value.lval;
	matrix=arg2->value.ht;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}
	
	if(_php3_hash_num_elements(matrix) != 6) {
		 php3_error(E_WARNING,"Text matrix must have 6 elements");
		RETURN_FALSE;
	}

	pdfmatrixptr = pdfmatrix;
	_php3_hash_internal_pointer_reset(matrix);
	for(i=0; i<_php3_hash_num_elements(matrix); i++) {
		_php3_hash_get_current_data(matrix, (void *) &data);
		switch(data->type) {
			case IS_DOUBLE:
				*pdfmatrixptr++ = (float) data->value.dval;
				break;
			default:
				*pdfmatrixptr++ = 0.0;
				break;
		}
		_php3_hash_move_forward(matrix);
	}

	cpdf_setTextMatrix(pdfmatrix[0], pdfmatrix[1],
                       pdfmatrix[2], pdfmatrix[3],
                       pdfmatrix[4], pdfmatrix[5]);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_text_pos(int pdfdoc, double x, double y [, int mode])
   Set the position of text for the next cpdf_show call */
void php3_cpdf_set_text_pos(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[4];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 3) || (argc > 4))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}
	
	if(argc > 3) {
		convert_to_long(argv[3]);
		mode = argv[3]->value.lval;
	}
	if(mode == 1)
		cpdf_rawSetTextPosition((float) argv[1]->value.dval, (float) argv[2]->value.dval);
	else
		cpdf_setTextPosition((float) argv[1]->value.dval, (float) argv[2]->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_rotate_text(int pdfdoc, double angle)
   Sets character spacing */
void php3_cpdf_rotate_text(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_rotateText((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_char_spacing(int pdfdoc, double space)
   Sets character spacing */
void php3_cpdf_set_char_spacing(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setCharacterSpacing((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_word_spacing(int pdfdoc, double space)
   Sets spacing between words */
void php3_cpdf_set_word_spacing(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setWordSpacing((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto double cpdf_stringwidth(int pdfdoc, string text)
   Returns width of text in current font */
void php3_cpdf_stringwidth(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	double width;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	width = (double) cpdf_stringWidth(arg2->value.str.val);

	RETURN_DOUBLE((double)width);
}
/* }}} */

/* {{{ proto void cpdf_save(int pdfdoc)
   Saves current enviroment */
void php3_cpdf_save(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_gsave();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_restore(int pdfdoc)
   Restores formerly saved enviroment */
void php3_cpdf_restore(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_grestore();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_translate(int pdfdoc, double x, double y)
   Sets origin of coordinate system */
void php3_cpdf_translate(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_rawTranslate((float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_scale(int pdfdoc, double x-scale, double y-scale)
   Sets scaling */
void php3_cpdf_scale(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_scale((float) arg2->value.dval, (float) arg3->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_rotate(int pdfdoc, double angle)
   Sets rotation */
void php3_cpdf_rotate(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_rotate((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setflat(int pdfdoc, double value)
   Sets flatness */
void php3_cpdf_setflat(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	int flatness;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if((arg2->value.lval > 100) && (arg2->value.lval < 0)) {
		php3_error(E_WARNING,"Parameter of pdf_setflat() has to between 0 and 100");
		RETURN_FALSE;
	}

	cpdf_setflat((int) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setlinejoin(int pdfdoc, int value)
   Sets linejoin parameter */
void php3_cpdf_setlinejoin(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if((arg2->value.lval > 2) && (arg2->value.lval < 0)) {
		php3_error(E_WARNING,"Parameter of pdf_setlinejoin() has to between 0 and 2");
		RETURN_FALSE;
	}

	cpdf_setlinejoin(arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setlinecap(int pdfdoc, int value)
   Sets linecap parameter */
void php3_cpdf_setlinecap(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if((arg2->value.lval > 2) && (arg2->value.lval < 0)) {
		php3_error(E_WARNING,"Parameter of pdf_setlinecap() has to be > 0 and =< 2");
		RETURN_FALSE;
	}

	cpdf_setlinecap(arg2->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setmiterlimit(int pdfdoc, double value)
   Sets miter limit */
void php3_cpdf_setmiterlimit(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(arg2->value.dval < 1) {
		php3_error(E_WARNING,"Parameter of pdf_setmiterlimit() has to be >= 1");
		RETURN_FALSE;
	}

	cpdf_setmiterlimit((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setlinewidth(int pdfdoc, double width)
   Sets line width */
void php3_cpdf_setlinewidth(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setlinewidth((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

#define BUFFERLEN 20
/* {{{ proto void cpdf_setdash(int pdfdoc, long white, long black)
   Sets dash pattern */
void php3_cpdf_setdash(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3;
	int id, type;
	char buffer[BUFFERLEN];
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	convert_to_long(arg3);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	snprintf(buffer, BUFFERLEN, "[%d %d] 0", arg2->value.lval, arg3->value.lval);
	cpdf_setdash(buffer);

	RETURN_TRUE;
}
/* }}} */
#undef BUFFERLEN

/* {{{ proto void cpdf_moveto(int pdfdoc, double x, double y [, int mode])
   Sets current point */
void php3_cpdf_moveto(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[4];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 3) || (argc > 4))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 3) {
		convert_to_long(argv[3]);
		mode = argv[3]->value.lval;
	}
	if(mode == 1)
		cpdf_rawMoveto((float) argv[1]->value.dval, (float) argv[2]->value.dval);
	else
		cpdf_moveto((float) argv[1]->value.dval, (float) argv[2]->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_rmoveto(int pdfdoc, double x, double y [, int mode])
   Sets current point */
void php3_cpdf_rmoveto(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[4];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 3) || (argc > 4))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 3) {
		convert_to_long(argv[3]);
		mode = argv[3]->value.lval;
	}
	if(mode == 1)
		cpdf_rawRmoveto((float) argv[1]->value.dval, (float) argv[2]->value.dval);
	else
		cpdf_rmoveto((float) argv[1]->value.dval, (float) argv[2]->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_curveto(int pdfdoc, double x1, double y1, double x2, double y2, double x3, double y3 [, int mode])
   Draws a curve */
void php3_cpdf_curveto(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[8];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 7) || (argc > 8))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	convert_to_double(argv[4]);
	convert_to_double(argv[5]);
	convert_to_double(argv[6]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 7) {
		convert_to_long(argv[7]);
		mode = argv[7]->value.lval;
	}
	if(mode == 1)
		cpdf_rawCurveto((float) argv[1]->value.dval,
	                	(float) argv[2]->value.dval,
	                	(float) argv[3]->value.dval,
	                	(float) argv[4]->value.dval,
	                	(float) argv[5]->value.dval,
	                	(float) argv[6]->value.dval);
	else
		cpdf_curveto((float) argv[1]->value.dval,
	                	(float) argv[2]->value.dval,
	                	(float) argv[3]->value.dval,
	                	(float) argv[4]->value.dval,
	                	(float) argv[5]->value.dval,
	                	(float) argv[6]->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_lineto(int pdfdoc, double x, double y [, int mode])
   Draws a line */
void php3_cpdf_lineto(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[4];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 3) || (argc > 4))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 3) {
		convert_to_long(argv[3]);
		mode = argv[3]->value.lval;
	}
	if(mode == 1)
		cpdf_rawLineto((float) argv[1]->value.dval, (float) argv[2]->value.dval);
	else
		cpdf_lineto((float) argv[1]->value.dval, (float) argv[2]->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_rlineto(int pdfdoc, double x, double y [, int mode])
   Draws a line relative to current point */
void php3_cpdf_rlineto(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[4];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 3) || (argc > 4))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 3) {
		convert_to_long(argv[3]);
		mode = argv[3]->value.lval;
	}
	if(mode == 1)
		cpdf_rawRlineto((float) argv[1]->value.dval, (float) argv[2]->value.dval);
	else
		cpdf_rlineto((float) argv[1]->value.dval, (float) argv[2]->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_circle(int pdfdoc, double x, double y, double radius [, int mode])
   Draws a circle */
void php3_cpdf_circle(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[5];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 4) || (argc > 5))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 4) {
		convert_to_long(argv[4]);
		mode = argv[4]->value.lval;
	}
	if(mode == 1)
		cpdf_rawCircle((float) argv[1]->value.dval, (float) argv[2]->value.dval, (float) argv[3]->value.dval);
	else
		cpdf_circle((float) argv[1]->value.dval, (float) argv[2]->value.dval, (float) argv[3]->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_arc(int pdfdoc, double x, double y, double radius, double start, double end [, int mode])
   Draws an arc */
void php3_cpdf_arc(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[7];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 6) || (argc > 7))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	convert_to_double(argv[4]);
	convert_to_double(argv[5]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 6) {
		convert_to_long(argv[6]);
		mode = argv[6]->value.lval;
	}
	if(mode == 1)
		cpdf_rawArc((float) argv[1]->value.dval, (float) argv[2]->value.dval, (float) argv[3]->value.dval, (float) argv[4]->value.dval, (float) argv[5]->value.dval, 1);
	else
		cpdf_arc((float) argv[1]->value.dval, (float) argv[2]->value.dval, (float) argv[3]->value.dval, (float) argv[4]->value.dval, (float) argv[5]->value.dval, 1);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_rect(int pdfdoc, double x, double y, double width, double height [, int mode])
   Draws a rectangle */
void php3_cpdf_rect(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[6];
	int id, type, argc, mode=0;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 5) || (argc > 6))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	convert_to_double(argv[4]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 5) {
		convert_to_long(argv[5]);
		mode = argv[5]->value.lval;
	}
	if(mode == 1)
		cpdf_rawRect((float) argv[1]->value.dval,
	                	(float) argv[2]->value.dval,
	                	(float) argv[3]->value.dval,
	                	(float) argv[4]->value.dval);
	else
		cpdf_rect((float) argv[1]->value.dval,
	                	(float) argv[2]->value.dval,
	                	(float) argv[3]->value.dval,
	                	(float) argv[4]->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_newpath(int pdfdoc)
   Starts new path */
void php3_cpdf_newpath(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_newpath();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_closepath(int pdfdoc)
   Close path */
void php3_cpdf_closepath(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_closepath();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_closepath_stroke(int pdfdoc)
   Close path and draw line along path */
void php3_cpdf_closepath_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_closepath();
	cpdf_stroke();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_stroke(int pdfdoc)
   Draw line along path path */
void php3_cpdf_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_stroke();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_fill(int pdfdoc)
   Fill current path */
void php3_cpdf_fill(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_fill();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_fill_stroke(int pdfdoc)
   Fill and stroke current path */
void php3_cpdf_fill_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_fill();
	cpdf_stroke();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_closepath_fill_stroke(int pdfdoc)
   Close, fill and stroke current path */
void php3_cpdf_closepath_fill_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_closepath();
	cpdf_fill();
	cpdf_stroke();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_clip(int pdfdoc)
   Clips to current path */
void php3_cpdf_clip(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_clip();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setgray_fill(int pdfdoc, double value)
   Sets filling color to gray value */
void php3_cpdf_setgray_fill(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setgrayFill((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setgray_stroke(int pdfdoc, double value)
   Sets drawing color to gray value */
void php3_cpdf_setgray_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setgrayStroke((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setgray(int pdfdoc, double value)
   Sets drawing and filling color to gray value */
void php3_cpdf_setgray(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setgray((float) arg2->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setrgbcolor_fill(int pdfdoc, double red, double green, double blue)
   Sets filling color to rgb color value */
void php3_cpdf_setrgbcolor_fill(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setrgbcolorFill((float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setrgbcolor_stroke(int pdfdoc, double red, double green, double blue)
   Sets drawing color to rgb color value */
void php3_cpdf_setrgbcolor_stroke(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setrgbcolorStroke((float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_setrgbcolor(int pdfdoc, double red, double green, double blue)
   Sets drawing and filling color to rgb color value */
void php3_cpdf_setrgbcolor(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_double(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setrgbcolor((float) arg2->value.dval, (float) arg3->value.dval, (float) arg4->value.dval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_page_animation(int pdfdoc, int transition, double duration, double direction, int orientation, int inout)
   Sets transition between pages */
void php3_cpdf_set_page_animation(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 6 || getParameters(ht, 6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_long(arg2);
	convert_to_double(arg3);
	convert_to_double(arg4);
	convert_to_long(arg5);
	convert_to_long(arg6);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_setPageTransition(arg2->value.lval, arg3->value.dval, arg4->value.dval,
	                       arg5->value.lval, arg6->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array cpdf_finalize(int pdfdoc)
   Creates pdf doc in memory */
void php3_cpdf_finalize(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	cpdf_finalizeAll();

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array cpdf_output_buffer(int pdfdoc)
   Returns the internal memory stream as string */
void php3_cpdf_output_buffer(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1;
	pval out_arr;
	int id, type, lenght;
	int *pdf;
	char *buffer;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	buffer = cpdf_getBufferForPDF(&lenght);
	php3_header();
	php3_write(buffer, lenght);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array cpdf_save_to_file(int pdfdoc, string filename)
   Saves the internal memory stream to a file */
void php3_cpdf_save_to_file(INTERNAL_FUNCTION_PARAMETERS) {
	pval *arg1, *arg2;
	int id, type;
	int *pdf;
	CPDF_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(arg1);
	convert_to_string(arg2);
	id=arg1->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

#if APACHE
	if(strcmp(arg2->value.str.val, "-") == 0)
		php3_error(E_WARNING,"Writing to stdout as described in the ClibPDF manual is not possible if php3 is used as an Apache module. Use cpdf_output_buffer() instead.");
#endif

	cpdf_savePDFmemoryStreamToFile(arg2->value.str.val);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_import_jpeg(int pdfdoc, string filename, double x, double y, double angle, double width, double height, double x-scale, double y-scale, int gsave [, int mode])
   Includes jpeg image */
void php3_cpdf_import_jpeg(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[11];
	int id, type, argc, mode=0;
	float width, height, xscale, yscale;
	int *pdf;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 10) || (argc > 11))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_string(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	convert_to_double(argv[4]);
	convert_to_double(argv[5]);
	width = (float) argv[5]->value.dval;
	convert_to_double(argv[6]);
	height = (float) argv[6]->value.dval;
	convert_to_double(argv[7]);
	xscale = (float) argv[7]->value.dval;
	convert_to_double(argv[8]);
	yscale = (float) argv[8]->value.dval;
	convert_to_long(argv[9]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	if(argc > 10) {
		convert_to_long(argv[10]);
		mode = argv[10]->value.lval;
	}
	if(mode == 1)
		cpdf_rawImportImage(argv[1]->value.str.val,
	                	    JPEG_IMG,
	                	    (float) argv[2]->value.dval,
	                	    (float) argv[3]->value.dval,
	                	    (float) argv[4]->value.dval,
	                	    &width,
	                	    &height,
	                	    &xscale,
	                	    &yscale,
	                	    argv[9]->value.lval);
	else
		cpdf_rawImportImage(argv[1]->value.str.val,
	                	    JPEG_IMG,
	                	    (float) argv[2]->value.dval,
	                	    (float) argv[3]->value.dval,
	                	    (float) argv[4]->value.dval,
	                	    &width,
	                	    &height,
	                	    &xscale,
	                	    &yscale,
	                	    argv[9]->value.lval);

	RETURN_TRUE;
}
/* }}} */

#if HAVE_LIBGD
/* {{{ proto void cpdf_place_inline_image(int pdfdoc, int gdimage, double x, double y, double angle, fload width, float height, int gsave [, int mode])
   Includes image */
void php3_cpdf_place_inline_image(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[11];
	int id, gid, type, argc, mode=0;
	int count, i, j, color;
	float width, height;
	int *pdf;
	unsigned char *buffer, *ptr;
	gdImagePtr im;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 8) || (argc > 9))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_long(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	convert_to_double(argv[4]);
	convert_to_double(argv[5]);
	convert_to_double(argv[6]);
	convert_to_long(argv[7]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	gid=argv[1]->value.lval;
	im = php3_list_find(gid, &type);
	if (!im || type != php3i_get_le_gd()) {
		php3_error(E_WARNING, "cpdf: Unable to find image pointer");
		RETURN_FALSE;
	}

	if(argc > 8) {
		convert_to_long(argv[8]);
		mode = argv[8]->value.lval;
	}

	count = 3 * im->sx * im->sy;
	if(NULL == (buffer = (unsigned char *) emalloc(count)))
		RETURN_FALSE;

	ptr = buffer;
	for(i=0; i<im->sy; i++) {
		for(j=0; j<im->sx; j++) {
			color = im->pixels[i][j];
			*ptr++ = im->red[color];
			*ptr++ = im->green[color];
			*ptr++ = im->blue[color];
		}
	}

	if(mode == 1)
		cpdf_placeInLineImage(buffer, count,
	                	    (float) argv[2]->value.dval,
	                	    (float) argv[3]->value.dval,
	                	    (float) argv[4]->value.dval,
	                	    (float) argv[5]->value.dval,
	                	    (float) argv[6]->value.dval,
	                	    im->sx,
	                	    im->sy,
	                	    8, 2, argv[7]->value.lval);
	else
		cpdf_rawPlaceInLineImage(buffer, count,
	                	    (float) argv[2]->value.dval,
	                	    (float) argv[3]->value.dval,
	                	    (float) argv[4]->value.dval,
	                	    (float) argv[5]->value.dval,
	                	    (float) argv[6]->value.dval,
	                	    im->sx,
	                	    im->sy,
	                	    8, 2, argv[7]->value.lval);

	efree(buffer);
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void cpdf_add_annotation(int pdfdoc, double xll, double yll, double xur, double xur, string title, string text [, int mode])
   Sets annotation */
void php3_cpdf_add_annotation(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[11];
	int id, type, argc, mode=0;
	int *pdf;
	CPDFannotAttrib attrib;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 7) || (argc > 8))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	convert_to_double(argv[4]);
	convert_to_string(argv[5]);
	convert_to_string(argv[6]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	attrib.flags = AF_NOZOOM | AF_NOROTATE | AF_READONLY;
	attrib.border_array = "[0 0 1 [4 2]]";
	attrib.BS = NULL;
	attrib.r = 0.00;
	attrib.g = 1.00;
	attrib.b = 1.00;
	if(argc > 7) {
		convert_to_long(argv[7]);
		mode = argv[7]->value.lval;
	}
	if(mode == 1)
		cpdf_rawSetAnnotation((float) argv[1]->value.dval,
	                	    (float) argv[2]->value.dval,
	                	    (float) argv[3]->value.dval,
	                	    (float) argv[4]->value.dval,
	                	    argv[5]->value.str.val,
	                	    argv[6]->value.str.val,
		                    &attrib);
	else
		cpdf_setAnnotation((float) argv[1]->value.dval,
	                	    (float) argv[2]->value.dval,
	                	    (float) argv[3]->value.dval,
	                	    (float) argv[4]->value.dval,
	                	    argv[5]->value.str.val,
	                	    argv[6]->value.str.val,
		                    &attrib);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void cpdf_set_action_url(int pdfdoc, double xll, double yll, double xur, double xur, string url [, int mode])
   Sets Hyperlink */
void php3_cpdf_set_action_url(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[11];
	int id, type, argc, mode=0;
	int *pdf;
	CPDFannotAttrib attrib;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if((argc < 6) || (argc > 7))
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_double(argv[1]);
	convert_to_double(argv[2]);
	convert_to_double(argv[3]);
	convert_to_double(argv[4]);
	convert_to_string(argv[5]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	attrib.flags = AF_NOZOOM | AF_NOROTATE | AF_READONLY;
	attrib.border_array = "[0 0 1 [4 0]]";
	attrib.BS = NULL;
	attrib.r = 0.00;
	attrib.g = 0.00;
	attrib.b = 1.00;
	if(argc > 6) {
		convert_to_long(argv[6]);
		mode = argv[6]->value.lval;
	}
	if(mode == 1)
		cpdf_rawSetActionURL((float) argv[1]->value.dval,
	                	    (float) argv[2]->value.dval,
	                	    (float) argv[3]->value.dval,
	                	    (float) argv[4]->value.dval,
	                	    argv[5]->value.str.val,
		                    &attrib);
	else
		cpdf_setActionURL((float) argv[1]->value.dval,
	                	    (float) argv[2]->value.dval,
	                	    (float) argv[3]->value.dval,
	                	    (float) argv[4]->value.dval,
	                	    argv[5]->value.str.val,
		                    &attrib);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int cpdf_add_outline(int pdfdoc, int lastoutline, int sublevel, int open, int pagenr, string title)
   Add outline */
void php3_cpdf_add_outline(INTERNAL_FUNCTION_PARAMETERS) {
	pval *argv[11];
	int id, oid, type, argc, mode=0;
	int *pdf;
	CPDFoutlineEntry *lastoutline;
	CPDF_TLS_VARS;

	argc = ARG_COUNT(ht);
	if(argc != 6)
		WRONG_PARAM_COUNT;
	if (getParametersArray(ht, argc, argv) == FAILURE)
		WRONG_PARAM_COUNT;

	convert_to_long(argv[0]);
	convert_to_long(argv[1]);
	convert_to_long(argv[2]);
	convert_to_long(argv[3]);
	convert_to_long(argv[4]);
	convert_to_string(argv[5]);
	id=argv[0]->value.lval;
	pdf = php3_list_find(id,&type);
	if(!pdf || type!=CPDF_GLOBAL(le_cpdf)) {
		php3_error(E_WARNING,"Unable to find identifier %d",id);
		RETURN_FALSE;
	}

	oid=argv[1]->value.lval;
	lastoutline = php3_list_find(oid,&type);
	if(!lastoutline || type!=CPDF_GLOBAL(le_outline)) {
		lastoutline = NULL;
/*		php3_error(E_WARNING,"Unable to find last outline entry %d",id);
		RETURN_FALSE; */
	}

	lastoutline = cpdf_addOutlineEntry(lastoutline,
	              	     argv[2]->value.lval,
	              	     argv[3]->value.lval,
	               	     argv[4]->value.lval,
	               	     argv[5]->value.str.val,
		             1, 0.0, 0.0, 0.0, 0.0);

	id = php3_list_insert(lastoutline,CPDF_GLOBAL(le_outline));
	RETURN_LONG(id);
}
/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
