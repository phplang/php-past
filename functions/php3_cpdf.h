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
   | Authors: Uwe Steinmann                                               |
   +----------------------------------------------------------------------+
 */
/* $Id: php3_cpdf.h,v 1.11 2000/02/14 10:09:20 kk Exp $ */

#ifndef _PHP3_CPDF_H
#define _PHP3_CPDF_H

#if HAVE_CPDFLIB

#include <cpdflib.h>

extern php3_module_entry cpdf_module_entry;
#define cpdf_module_ptr &cpdf_module_entry

extern void php3_info_cpdf(void);
extern int php3_minit_cpdf(INIT_FUNC_ARGS);
extern int php3_rinit_cpdf(INIT_FUNC_ARGS);
extern int php3_mend_cpdf(void);
extern void php3_cpdf_set_creator(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_title(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_subject(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_keywords(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_viewer_preferences(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_open(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_close(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_page_init(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_finalize_page(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_current_page(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_begin_text(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_end_text(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_show(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_show_xy(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_continue_text(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_text(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_font(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_leading(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_text_rendering(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_horiz_scaling(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_text_rise(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_text_matrix(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_text_pos(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_rotate_text(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_char_spacing(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_word_spacing(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_continue_text(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_stringwidth(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_save(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_restore(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_translate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_scale(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_rotate(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setflat(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setlinejoin(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setlinecap(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setmiterlimit(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setlinewidth(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setdash(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_moveto(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_rmoveto(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_curveto(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_lineto(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_rlineto(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_circle(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_arc(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_rect(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_newpath(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_closepath(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_closepath_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_fill(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_fill_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_closepath_fill_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_endpath(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_clip(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setgray_fill(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setgray_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setgray(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setrgbcolor_fill(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setrgbcolor_stroke(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_setrgbcolor(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_add_outline(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_set_page_animation(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_finalize(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_output_buffer(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_save_to_file(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_add_annotation(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_cpdf_import_jpeg(INTERNAL_FUNCTION_PARAMETERS);
#if HAVE_LIBGD
extern void php3_cpdf_place_inline_image(INTERNAL_FUNCTION_PARAMETERS);
#endif
extern void php3_cpdf_set_action_url(INTERNAL_FUNCTION_PARAMETERS);
#else
#define cpdf_module_ptr NULL
#endif
#endif /* _PHP3_PDF_H */
