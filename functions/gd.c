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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Stig Bakken <ssb@fast.no>                                   |
   |          Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
 */

/* $Id: gd.c,v 1.155 2000/09/30 17:32:44 sas Exp $ */

/* gd 1.2 is copyright 1994, 1995, Quest Protein Database Center, 
   Cold Spring Harbor Labs. */

/* Note that there is no code from the gd package in this file */
#define IS_EXT_MODULE
#include "php.h"
#include "internal_functions.h"
#include "php3_list.h"
#include "head.h"
#include <math.h>
#include "php3_gd.h"

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#if WIN32|WINNT
# include <io.h>
# include <fcntl.h>
#endif

#if HAVE_LIBGD
#include <gd.h>
#include <gdfontt.h>  /* 1 Tiny font */
#include <gdfonts.h>  /* 2 Small font */
#include <gdfontmb.h> /* 3 Medium bold font */
#include <gdfontl.h>  /* 4 Large font */
#include <gdfontg.h>  /* 5 Giant font */
#if HAVE_LIBTTF|HAVE_LIBFREETYPE
# include "functions/gdttf.h"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#if HAVE_LIBTTF|HAVE_LIBFREETYPE
static void php3_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int);
#endif

#define GD_GLOBAL(a) a
#define GD_TLS_VARS
int le_gd;
int le_gd_font;
#if HAVE_LIBT1
int le_ps_enc;
int le_ps_font;
gd_module php3_gd_module;
#endif

function_entry gd_functions[] = {
	{"imagearc",				php3_imagearc,				NULL},
	{"imagechar",				php3_imagechar,				NULL},
	{"imagecharup",				php3_imagecharup,			NULL},
	{"imagecolorallocate",		php3_imagecolorallocate,	NULL},
	{"imagecolorat",		php3_imagecolorat,		NULL},
	{"imagecolorclosest",		php3_imagecolorclosest,		NULL},
	{"imagecolordeallocate",	php3_imagecolordeallocate,	NULL},
	{"imagecolorresolve",		php3_imagecolorresolve,		NULL},
	{"imagecolorexact",			php3_imagecolorexact,		NULL},
	{"imagecolorset",		php3_imagecolorset,		NULL},
	{"imagecolortransparent",	php3_imagecolortransparent,	NULL},
	{"imagecolorstotal",		php3_imagecolorstotal,		NULL},
	{"imagecolorsforindex",		php3_imagecolorsforindex,	NULL},
	{"imagecopy",			php3_imagecopy,			NULL},
	{"imagecopyresized",		php3_imagecopyresized,		NULL},
	{"imagecreate",				php3_imagecreate,			NULL},
	{"imagecreatefromgif",		php3_imagecreatefromgif,	NULL},
	{"imagegif",				php3_imagegif,				NULL},
    {"imagecreatefrompng",          php3_imagecreatefrompng,        NULL},
    {"imagepng",                            php3_imagepng,                          NULL},
    {"imagecreatefromjpeg",          php3_imagecreatefromjpeg,        NULL},
    {"imagejpeg",                            php3_imagejpeg,                          NULL},
	{"imagewbmp",				php3_imagewbmp,				NULL},
	{"imagedestroy",			php3_imagedestroy,			NULL},
	{"imagefill",				php3_imagefill,				NULL},
	{"imagefilledpolygon",		php3_imagefilledpolygon,	NULL},
	{"imagefilledrectangle",	php3_imagefilledrectangle,	NULL},
	{"imagefilltoborder",		php3_imagefilltoborder,		NULL},
	{"imagefontwidth",			php3_imagefontwidth,		NULL},
	{"imagefontheight",			php3_imagefontheight,		NULL},
	{"imageinterlace",			php3_imageinterlace,		NULL},
	{"imageline",				php3_imageline,				NULL},
	{"imageloadfont",			php3_imageloadfont,			NULL},
	{"imagepolygon",			php3_imagepolygon,			NULL},
	{"imagerectangle",			php3_imagerectangle,		NULL},
	{"imagesetpixel",			php3_imagesetpixel,			NULL},
	{"imagestring",				php3_imagestring,			NULL},
	{"imagestringup",			php3_imagestringup,			NULL},
	{"imagesx",					php3_imagesxfn,				NULL},
	{"imagesy",					php3_imagesyfn,				NULL},
	{"imagedashedline",			php3_imagedashedline,  		NULL},
	{"imagegammacorrect",		php3_imagegammacorrect,		NULL},
	{"imagettfbbox",			php3_imagettfbbox,			NULL},
	{"imagettftext",			php3_imagettftext,			NULL},
	{"imagepsloadfont",			php3_imagepsloadfont,		NULL},
	{"imagepsfreefont",			php3_imagepsfreefont,		NULL},
	/* The function in t1lib which this function uses seem to be buggy...
	{"imagepscopyfont",			php3_imagepscopyfont,		NULL},
	*/
	{"imagepsencodefont",		php3_imagepsencodefont,		NULL},
	{"imagepsextendfont",		php3_imagepsextendfont,		NULL},
	{"imagepsslantfont",		php3_imagepsslantfont,		NULL},
	{"imagepsbbox",				php3_imagepsbbox,			NULL},
	{"imagepstext",				php3_imagepstext,			NULL},
	{NULL, NULL, NULL}
};

php3_module_entry gd_module_entry = {
	"gd", gd_functions, php3_minit_gd, php3_mend_gd, NULL, NULL, php3_info_gd, STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL
#include "dl/phpdl.h"
DLEXPORT php3_module_entry *get_module(void) { return &gd_module_entry; }
#endif


static void php3i_destructor_gdImageDestroy(gdImagePtr img) {
	(void)gdImageDestroy(img);
}

int php3_minit_gd(INIT_FUNC_ARGS)
{
	GD_GLOBAL(le_gd) = register_list_destructors(php3i_destructor_gdImageDestroy, NULL);
	GD_GLOBAL(le_gd_font) = register_list_destructors(php3_free_gd_font, NULL);
#if HAVE_LIBT1
	T1_SetBitmapPad(8);
	GD_GLOBAL(le_ps_font) = register_list_destructors(_php3_free_ps_font, NULL);
	GD_GLOBAL(le_ps_enc) = register_list_destructors(_php3_free_ps_enc, NULL);
	T1_InitLib(NO_LOGFILE|IGNORE_CONFIGFILE|IGNORE_FONTDATABASE);
	T1_SetLogLevel(T1LOG_DEBUG);
	if (cfg_get_string("ps.default_encoding", &GD_GLOBAL(php3_gd_module).default_encoding) != FAILURE
		&& GD_GLOBAL(php3_gd_module).default_encoding[0] != 0) {
		T1_SetDefaultEncoding(T1_LoadEncoding(GD_GLOBAL(php3_gd_module).default_encoding));
	}

#endif
	return SUCCESS;
}

void php3_info_gd(void) {
	/* need to use a PHPAPI function here because it is external module in Windows */
#if HAVE_GD_PNG
        php3_printf("Version 1.6 (PNG) or later");
#endif
#if HAVE_GD_GIF
        php3_printf("Version 1.5 (GIF) or earlier");
#endif
#if HAVE_LIBTTF
	php3_printf(", FreeType support");
#endif
#if HAVE_LIBFREETYPE
	php3_printf(", FreeType 2 support");
#endif
#if HAVE_LIBT1
	php3_printf(", t1lib support");
#endif
}

int php3_mend_gd(void){
	GD_TLS_VARS;
#if HAVE_LIBT1
	T1_CloseLib();
#endif
	return SUCCESS;
}

/* Need this for cpdf. See also comment in file.c php3i_get_le_fp() */
PHPAPI_EXPORT int php3i_get_le_gd(void){
	TLS_VARS;
	return GLOBAL(le_gd);
}

/********************************************************************/
/* gdImageColorResolve is a replacement for the old fragment:       */
/*                                                                  */
/*      if ((color=gdImageColorExact(im,R,G,B)) < 0)                */
/*        if ((color=gdImageColorAllocate(im,R,G,B)) < 0)           */
/*          color=gdImageColorClosest(im,R,G,B);                    */
/*                                                                  */
/* in a single function                                             */

#ifndef HAVE_GD_COLORRESOLVE
int
gdImageColorResolve(gdImagePtr im, int r, int g, int b)
{
	int c;
	int ct = -1;
	int op = -1;
	long rd, gd, bd, dist;
	long mindist = 3*255*255;  /* init to max poss dist */

	for (c = 0; c < im->colorsTotal; c++) {
		if (im->open[c]) {
			op = c;             /* Save open slot */
			continue;           /* Color not in use */
		}
		rd = (long)(im->red  [c] - r);
		gd = (long)(im->green[c] - g);
		bd = (long)(im->blue [c] - b);
		dist = rd * rd + gd * gd + bd * bd;
		if (dist < mindist) {
			if (dist == 0) {
				return c;       /* Return exact match color */
			}
			mindist = dist;
			ct = c;
		}
	}
	/* no exact match.  We now know closest, but first try to allocate exact */
	if (op == -1) {
		op = im->colorsTotal;
		if (op == gdMaxColors) {    /* No room for more colors */
			return ct;          /* Return closest available color */
		}
		im->colorsTotal++;
	}
	im->red  [op] = r;
	im->green[op] = g;
	im->blue [op] = b;
	im->open [op] = 0;
	return op;                  /* Return newly allocated color */
}
#endif

void php3_free_gd_font(gdFontPtr fp)
{
	if (fp->data) {
		efree(fp->data);
	}
	efree(fp);
}

/* {{{ proto int imageloadfont(string filename)
   Load a new font */
void php3_imageloadfont(INTERNAL_FUNCTION_PARAMETERS) {
	pval *file;
	int hdr_size = sizeof(gdFont) - sizeof(char *);
	int ind, body_size, n=0, b;
	gdFontPtr font;
	FILE *fp;
	int issock=0, socketd=0;
	GD_TLS_VARS;


	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(file);

#if WIN32|WINNT
	fp = fopen(file->value.str.val, "rb");
#else
	fp = php3_fopen_wrapper(file->value.str.val, "r", IGNORE_PATH|IGNORE_URL_WIN, &issock, &socketd);
#endif
	if (fp == NULL) {
		php3_error(E_WARNING, "ImageFontLoad: unable to open file");
		RETURN_FALSE;
	}

	/* Only supports a architecture-dependent binary dump format
	 * at the moment.
	 * The file format is like this on machines with 32-byte integers:
	 *
	 * byte 0-3:   (int) number of characters in the font
	 * byte 4-7:   (int) value of first character in the font (often 32, space)
	 * byte 8-11:  (int) pixel width of each character
	 * byte 12-15: (int) pixel height of each character
	 * bytes 16-:  (char) array with character data, one byte per pixel
	 *                    in each character, for a total of 
	 *                    (nchars*width*height) bytes.
	 */
	font = (gdFontPtr)emalloc(sizeof(gdFont));
	b = 0;
	while (b < hdr_size && (n = fread(&font[b], 1, hdr_size - b, fp)))
		b += n;
	if (!n) {
		fclose(fp);
		efree(font);
		if (feof(fp)) {
			php3_error(E_WARNING, "ImageFontLoad: end of file while reading header");
		} else {
			php3_error(E_WARNING, "ImageFontLoad: error while reading header");
		}
		RETURN_FALSE;
	}
	body_size = font->w * font->h * font->nchars;
	font->data = emalloc(body_size);
	b = 0;
	while (b < body_size && (n = fread(&font->data[b], 1, body_size - b, fp)))
		b += n;
	if (!n) {
		fclose(fp);
		efree(font->data);
		efree(font);
		if (feof(fp)) {
			php3_error(E_WARNING, "ImageFontLoad: end of file while reading body");
		} else {
			php3_error(E_WARNING, "ImageFontLoad: error while reading body");
		}
		RETURN_FALSE;
	}
	fclose(fp);

	/* Adding 5 to the font index so we will never have font indices
	 * that overlap with the old fonts (with indices 1-5).  The first
	 * list index given out is always 1.
	 */
	ind = 5 + php3_list_insert(font, GD_GLOBAL(le_gd_font));

	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int imagecreate(int x_size, int y_size)
   Create a new image */
void php3_imagecreate(INTERNAL_FUNCTION_PARAMETERS) {
	pval *x_size, *y_size;
	int ind;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &x_size, &y_size) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(x_size);
	convert_to_long(y_size);

	im = gdImageCreate(x_size->value.lval, y_size->value.lval);
	ind = php3_list_insert(im, GD_GLOBAL(le_gd));		

	RETURN_LONG(ind);
}
/* }}} */

/* {{{ proto int imagecreatefromgif(string filename)
   Create a new image from GIF file or URL */
void php3_imagecreatefromgif (INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_GD_GIF
	pval *file;
	int ind;
	gdImagePtr im;
	char *fn=NULL;
	FILE *fp;
	int issock=0, socketd=0;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(file);

	fn = file->value.str.val;

#if WIN32|WINNT
	fp = fopen(file->value.str.val, "rb");
#else
	fp = php3_fopen_wrapper(file->value.str.val, "r", IGNORE_PATH|IGNORE_URL_WIN, &issock, &socketd);
#endif
	if (!fp) {
		php3_strip_url_passwd(fn);
		php3_error(E_WARNING,
					"ImageCreateFromGif: Unable to open %s for reading", fn);
		RETURN_FALSE;
	}

	im = gdImageCreateFromGif (fp);

	fflush(fp);
	fclose(fp);

	if (!im) {
		php3_error(E_WARNING,"ImageCreateFromGif: %s is not a valid GIF file", fn);
		RETURN_FALSE;
	}
	ind = php3_list_insert(im, GD_GLOBAL(le_gd));

	RETURN_LONG(ind);
#else /* HAVE_GD_GIF */
	php3_error(E_WARNING, "ImageCreateFromGif: No GIF support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_GIF */
}
/* }}} */


/* {{{ proto int imagecreatefrompng(string filename)
   Create a new image from PNG file or URL */
void php3_imagecreatefrompng (INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_GD_PNG
      pval *file;
      int ind;
      gdImagePtr im;
      char *fn=NULL;
      FILE *fp;
      int issock=0, socketd=0;
      GD_TLS_VARS;

      if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &file) == FAILURE) {
              WRONG_PARAM_COUNT;
      }

      convert_to_string(file);

      fn = file->value.str.val;

#if WIN32|WINNT
      fp = fopen(file->value.str.val, "rb");
#else
      fp = php3_fopen_wrapper(file->value.str.val, "r", IGNORE_PATH|IGNORE_URL_WIN, &issock, &socketd);
#endif
      if (!fp) {
              php3_strip_url_passwd(fn);
              php3_error(E_WARNING,
                                      "ImageCreateFromPng: Unable to open %s for reading", fn);
              RETURN_FALSE;
      }

      im = gdImageCreateFromPng (fp);

      fflush(fp);
      fclose(fp);
      if (!im) {
             php3_error(E_WARNING,"ImageCreateFromPng: %s is not a valid PNG file", fn);
             RETURN_FALSE;
      }
      ind = php3_list_insert(im, GD_GLOBAL(le_gd));

      RETURN_LONG(ind);
#else /* HAVE_GD_PNG */
	php3_error(E_WARNING, "ImageCreateFromPng: No PNG support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_PNG */
}
/* }}} */

/* {{{ proto int imagecreatefromjpeg(string filename)
   Create a new image from JPEG file or URL */
void php3_imagecreatefromjpeg (INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_GD_JPG
	pval *file;
	int ind;
	gdImagePtr im;
	char *fn=NULL;
	FILE *fp;
	int issock=0, socketd=0;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(file);

	fn = file->value.str.val;

#if WIN32|WINNT
	fp = fopen(file->value.str.val, "rb");
#else
	fp = php3_fopen_wrapper(file->value.str.val, "r", IGNORE_PATH|IGNORE_URL_WIN, &issock, &socketd);
#endif
	if (!fp) {
		php3_strip_url_passwd(fn);
		php3_error(E_WARNING,
					"ImageCreateFromJpeg: Unable to open %s for reading", fn);
		RETURN_FALSE;
	}

	im = gdImageCreateFromJpeg (fp);

	fflush(fp);
	fclose(fp);

	if (!im) {
		php3_error(E_WARNING,"ImageCreateFromJPEG: %s is not a valid JPEG file", fn);
		RETURN_FALSE;
	}

	ind = php3_list_insert(im, GD_GLOBAL(le_gd));

	RETURN_LONG(ind);
#else /* HAVE_GD_JPG */
	php3_error(E_WARNING, "ImageCreateFromJpeg: No JPG support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_JPG */
}
/* }}} */

/* {{{ proto int imagedestroy(int im)
   Destroy an image */
void php3_imagedestroy(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imgind;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &imgind) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(imgind);

	php3_list_delete(imgind->value.lval);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagecolorallocate(int im, int red, int green, int blue)
   Allocate a color for an image */
void php3_imagecolorallocate(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imgind, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &imgind, &red,
											&green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorAllocate: Unable to find image pointer");
		RETURN_FALSE;
	}
	col = gdImageColorAllocate(im, r, g, b);
	RETURN_LONG(col);
}
/* }}} */

/* im, x, y */
/* {{{ proto int imagecolorat(int im, int x, int y)
   Get the index of the color of a pixel */
void php3_imagecolorat(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imgind, *x, *y;
	int ind, ind_type;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &imgind, &x, &y) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(x);
	convert_to_long(y);
	
	ind = imgind->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorAt: Unable to find image pointer");
		RETURN_FALSE;
	}
	if (gdImageBoundsSafe(im, x->value.lval, y->value.lval)) {
#if HAVE_GD_ANCIENT
		RETURN_LONG(im->pixels[x->value.lval][y->value.lval]);
#else
		RETURN_LONG(im->pixels[y->value.lval][x->value.lval]);
#endif
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imagecolorclosest(int im, int red, int green, int blue)
   Get the index of the closest color to the specified color */
void php3_imagecolorclosest(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imgind, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &imgind, &red,
											&green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorClosest: Unable to find image pointer");
		RETURN_FALSE;
	}
	col = gdImageColorClosest(im, r, g, b);
	RETURN_LONG(col);
}
/* }}} */

/* {{{ proto int imagecolordeallocate(int im, int index)
   De-allocate a color for an image */
void php3_imagecolordeallocate(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imgind, *index;
	int ind, ind_type, col;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &imgind, &index) == FAILURE) {
	WRONG_PARAM_COUNT;
	}

	convert_to_long(imgind);
	convert_to_long(index);
	ind = imgind->value.lval;
	col = index->value.lval;

	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorDeallocate: Unable to find image pointer");
		RETURN_FALSE;
	}

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		gdImageColorDeallocate(im, col);
		RETURN_TRUE;
        }
        else {
                php3_error(E_WARNING, "Color index out of range");
                RETURN_FALSE;
        }
}
/* }}} */

/* {{{ proto int imagecolorresolve(int im, int red, int green, int blue)
   Get the index of the specified color or its closest possible alternative */
void php3_imagecolorresolve(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imgind, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &imgind, &red,
											&green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorResolve: Unable to find image pointer");
		RETURN_FALSE;
	}
	col = gdImageColorResolve(im, r, g, b);
	RETURN_LONG(col);
}
/* }}} */

/* {{{ proto int imagecolorexact(int im, int red, int green, int blue)
   Get the index of the specified color */
void php3_imagecolorexact(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imgind, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &imgind, &red,
											&green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorExact: Unable to find image pointer");
		RETURN_FALSE;
	}
	col = gdImageColorExact(im, r, g, b);
	RETURN_LONG(col);
}
/* }}} */

/* {{{ proto int imagecolorset(int im, int col, int red, int green, int blue)
   Set the color for the specified palette index */
void php3_imagecolorset(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imgind, *color, *red, *green, *blue;
	int ind, ind_type;
	int col;
	int r, g, b;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 5 || getParameters(ht, 5, &imgind, &color, &red, &green, &blue) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(color);
	convert_to_long(red);
	convert_to_long(green);
	convert_to_long(blue);
	
	ind = imgind->value.lval;
	col = color->value.lval;
	r = red->value.lval;
	g = green->value.lval;
	b = blue->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorSet: Unable to find image pointer");
		RETURN_FALSE;
	}
	if (col >= 0 && col < gdImageColorsTotal(im)) {
		im->red[col] = r;
		im->green[col] = g;
		im->blue[col] = b;
	}
	else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array imagecolorsforindex(int im, int col)
   Get the colors for an index */
void php3_imagecolorsforindex(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imgind, *index;
	int col, ind, ind_type;
	gdImagePtr im;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &imgind, &index) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_long(imgind);
	convert_to_long(index);
	ind = imgind->value.lval;
	col = index->value.lval;
	
	im = php3_list_find(ind, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageColorsForIndex: Unable to find image pointer");
		RETURN_FALSE;
	}

	if (col >= 0 && col < gdImageColorsTotal(im)) {
		if (array_init(return_value) == FAILURE) {
			RETURN_FALSE;
		}
		add_assoc_long(return_value,"red",im->red[col]);
		add_assoc_long(return_value,"green",im->green[col]);
		add_assoc_long(return_value,"blue",im->blue[col]);
	}
	else {
		php3_error(E_WARNING, "Color index out of range");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto int imagegif(int im [, string filename])
   Output GIF image to browser or file */
void php3_imagegif (INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_GD_GIF
	pval *imgind, *file;
	gdImagePtr im;
	char *fn=NULL;
	FILE *fp;
	int argc;
	int ind_type;
	int output=1;
	GD_TLS_VARS;

	argc = ARG_COUNT(ht);
	if (argc < 1 || argc > 2 || getParameters(ht, argc, &imgind, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(imgind);

	if (argc == 2) {
		convert_to_string(file);
		fn = file->value.str.val;
		if (!fn || fn == empty_string || _php3_check_open_basedir(fn)) {
			php3_error(E_WARNING, "ImageGif: Invalid filename");
			RETURN_FALSE;
		}
	}

	im = php3_list_find(imgind->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "ImageGif: unable to find image pointer");
		RETURN_FALSE;
	}

	if (argc == 2) {
		fp = fopen(fn, "wb");
		if (!fp) {
			php3_error(E_WARNING, "ImageGif: unable to open %s for writing", fn);
			RETURN_FALSE;
		}
#if HAVE_GD_LZW && defined(LZW_LICENCED)
		gdImageLzw (im,fp);
#else
		gdImageGif (im,fp);
#endif
		fflush(fp);
		fclose(fp);
	}
	else {
		int   b;
		FILE *tmp;
		char  buf[4096];

		tmp = tmpfile();
		if (tmp == NULL) {
			php3_error(E_WARNING, "Unable to open temporary file");
			RETURN_FALSE;
		}

		output = php3_header();

		if (output) {
#if HAVE_GD_LZW && defined(LZW_LICENCED) 
			gdImageLzw (im, tmp);
#else
			gdImageGif (im, tmp);
#endif
                      fseek(tmp, 0, SEEK_SET);
#if APACHE && defined(CHARSET_EBCDIC)
                      /* This is a binary file already: avoid EBCDIC->ASCII conversion */
                      ap_bsetflag(php3_rqst->connection->client, B_EBCDIC2ASCII, 0);
#endif
                      while ((b = fread(buf, 1, sizeof(buf), tmp)) > 0) {
                              php3_write(buf, b);
                      }
              }

              fclose(tmp);
              /* the temporary file is automatically deleted */
      }

      RETURN_TRUE;
#else /* HAVE_GD_GIF */
	php3_error(E_WARNING, "ImageGif: No GIF support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_GIF */
}
/* }}} */

/* {{{ proto int imagepng(int im [, string filename])
   Output PNG image to browser or file */
void php3_imagepng (INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_GD_PNG
      pval *imgind, *file;
      gdImagePtr im;
      char *fn=NULL;
      FILE *fp;
      int argc;
      int ind_type;
      int output=1;
      GD_TLS_VARS;

      argc = ARG_COUNT(ht);
      if (argc < 1 || argc > 2 || getParameters(ht, argc, &imgind, &file) == FAILURE) {
              WRONG_PARAM_COUNT;
      }

      convert_to_long(imgind);

      if (argc == 2) {
              convert_to_string(file);
              fn = file->value.str.val;
              if (!fn || fn == empty_string || _php3_check_open_basedir(fn)) {
                      php3_error(E_WARNING, "ImagePng: Invalid filename");
                      RETURN_FALSE;
              }
      }

      im = php3_list_find(imgind->value.lval, &ind_type);
      if (!im || ind_type != GD_GLOBAL(le_gd)) {
              php3_error(E_WARNING, "ImagePng: unable to find image pointer");
              RETURN_FALSE;
      }

      if (argc == 2) {
              fp = fopen(fn, "wb");
              if (!fp) {
                      php3_error(E_WARNING, "ImagePng: unable to open %s for writing", fn);
                      RETURN_FALSE;
              }
              gdImagePng (im,fp);
              fflush(fp);
              fclose(fp);
      }
      else {
              int   b;
              FILE *tmp;
              char  buf[4096];

              tmp = tmpfile();
              if (tmp == NULL) {
                      php3_error(E_WARNING, "Unable to open temporary file");
                      RETURN_FALSE;
              }

              output = php3_header();

              if (output) {
                      gdImagePng (im, tmp);
			fseek(tmp, 0, SEEK_SET);
#if APACHE && defined(CHARSET_EBCDIC)
			/* This is a binary file already: avoid EBCDIC->ASCII conversion */
			ap_bsetflag(php3_rqst->connection->client, B_EBCDIC2ASCII, 0);
#endif
			while ((b = fread(buf, 1, sizeof(buf), tmp)) > 0) {
				php3_write(buf, b);
			}
		}

		fclose(tmp);
		/* the temporary file is automatically deleted */
	}

	RETURN_TRUE;
#else /* HAVE_GD_PNG */
	php3_error(E_WARNING, "ImagePng: No PNG support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_PNG */
}
/* }}} */

/* {{{ proto int imagejpeg(int im [, string filename [, int quality]])
   Output JPEG image to browser or file */
void php3_imagejpeg (INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_GD_JPG
      pval *imgind, *file, *qual;
      gdImagePtr im;
      char *fn=NULL;
      FILE *fp;
      int argc;
      int ind_type;
      int output=1, q=-1;
      GD_TLS_VARS;

      argc = ARG_COUNT(ht);
      if (argc < 1 || argc > 3 || getParameters(ht, argc, &imgind, &file, &qual) == FAILURE) {
              WRONG_PARAM_COUNT;
      }

      convert_to_long(imgind);
	  
	  if (argc == 3) {
			  convert_to_long(qual);
			  q = qual->value.lval;
	  }

      if (argc > 1) {
              convert_to_string(file);
              fn = file->value.str.val;
              if (fn && strlen(fn) && _php3_check_open_basedir(fn)) {
                      php3_error(E_WARNING, "ImageJpeg: Invalid filename");
                      RETURN_FALSE;
              }
      }

      im = php3_list_find(imgind->value.lval, &ind_type);
      if (!im || ind_type != GD_GLOBAL(le_gd)) {
              php3_error(E_WARNING, "ImageJpeg: unable to find image pointer");
              RETURN_FALSE;
      }

      if (fn && strlen(fn) && argc > 1) {
              fp = fopen(fn, "wb");
              if (!fp) {
                     php3_error(E_WARNING, "ImageJpeg: unable to open %s for writing", fn);
                     RETURN_FALSE;
              }
              gdImageJpeg (im,fp,q);
              fflush(fp);
              fclose(fp);
      }
      else {
              int   b;
              FILE *tmp;
              char  buf[4096];

              tmp = tmpfile();
              if (tmp == NULL) {
                      php3_error(E_WARNING, "Unable to open temporary file");
                      RETURN_FALSE;
              }

              output = php3_header();

              if (output) {
                      gdImageJpeg (im, tmp, q);
			fseek(tmp, 0, SEEK_SET);
#if APACHE && defined(CHARSET_EBCDIC)
			/* This is a binary file already: avoid EBCDIC->ASCII conversion */
			ap_bsetflag(php3_rqst->connection->client, B_EBCDIC2ASCII, 0);
#endif
			while ((b = fread(buf, 1, sizeof(buf), tmp)) > 0) {
				php3_write(buf, b);
			}
		}

		fclose(tmp);
		/* the temporary file is automatically deleted */
	}

	RETURN_TRUE;
#else /* HAVE_GD_JPG */
	php3_error(E_WARNING, "ImageJpg: No JPG support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_GD_JPG */
}
/* }}} */

/* {{{ proto int imagewbmp(int im [, string filename])
   Output WBMP image to browser or file */
void php3_imagewbmp (INTERNAL_FUNCTION_PARAMETERS) {
      pval *imgind, *file;
      gdImagePtr im;
      char *fn=NULL;
      FILE *fp;
      int argc;
      int ind_type;
      int output=1;
      int x, y;
      int c, p, width, height;
      
      GD_TLS_VARS;

      argc = ARG_COUNT(ht);
      if (argc < 1 || argc > 2 || getParameters(ht, argc, &imgind, &file) == FAILURE) {
              WRONG_PARAM_COUNT;
      }

      convert_to_long(imgind);

      if (argc == 2) {
              convert_to_string(file);
              fn = file->value.str.val;
              if (!fn || fn == empty_string || _php3_check_open_basedir(fn)) {
                      php3_error(E_WARNING, "ImageWbmp: Invalid filename");
                      RETURN_FALSE;
              }
      }

      im = php3_list_find(imgind->value.lval, &ind_type);
      if (!im || ind_type != GD_GLOBAL(le_gd)) {
              php3_error(E_WARNING, "ImageWbmp: unable to find image pointer");
              RETURN_FALSE;
      }       
      
      if (argc == 2) {
              fp = fopen(fn, "wb");
              if (!fp) {
                      php3_error(E_WARNING, "ImagePng: unable to open %s for writing", fn);
                      RETURN_FALSE;
              }
              
              /* WBMP header, black and white, no compression */
              fputc(0,fp); fputc(0,fp);
                      
              /* Width and height of image */
              c = 1; width = im->sx;
              while(width & 0x7f << 7*c) c++;
              while(c > 1) fputc(0x80 | ((width >> 7*--c) & 0xff), fp);
              fputc(width & 0x7f,fp);
              c = 1; height = im->sy;
              while(height & 0x7f << 7*c) c++;
              while(c > 1) fputc(0x80 | ((height >> 7*--c) & 0xff), fp);
              fputc(height & 0x7f,fp);
                      
              /* Actual image data */
              for(y = 0; y < im->sy; y++) {
                      p = c = 0;
                      for(x = 0; x < im->sx; x++) {
#if HAVE_GD_ANCIENT
                              if(im->pixels[x][y] == 0) c = c | (1 << (7-p));
#else
                              if(im->pixels[y][x] == 0) c = c | (1 << (7-p));
#endif
                              if(++p == 8) {
                                      fputc(c,fp);
                                      p = c = 0;
                              }
                      }
                      if(p) fputc(c,fp);
              }
              
              fflush(fp);
              fclose(fp);
      } else {
              output = php3_header();

              if (output) {
#if APACHE && defined(CHARSET_EBCDIC)
                      /* This is a binary file already: avoid EBCDIC->ASCII conversion */
                      ap_bsetflag(php3_rqst->connection->client, B_EBCDIC2ASCII, 0);
#endif

                      /* WBMP header, black and white, no compression */
                      php3_putc(0); php3_putc(0);
                      
                      /* Width and height of image */
                      c = 1; width = im->sx;
                      while(width & 0x7f << 7*c) c++;
                      while(c > 1) php3_putc(0x80 | ((width >> 7*--c) & 0xff));
                      php3_putc(width & 0x7f);
                      
                      c = 1; height = im->sy;
                      while(height & 0x7f << 7*c) c++;
                      while(c > 1) php3_putc(0x80 | ((height >> 7*--c) & 0xff));
                      php3_putc(height & 0x7f);
                      
                      /* Actual image data */
                      for(y = 0; y < im->sy; y++) {
                              p = c = 0;
                              for(x = 0; x < im->sx; x++) {
#if HAVE_GD_ANCIENT
                                      if(im->pixels[x][y] == 0) c = c | (1 << (7-p));
#else
                                      if(im->pixels[y][x] == 0) c = c | (1 << (7-p));
#endif
                                      if(++p == 8) {
                                              php3_putc(c);
                                              p = c = 0;
                                      }
                              }
                              if(p) php3_putc(c);
                      }
              }
      }
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagesetpixel(int im, int x, int y, int col)
   Set a single pixel */
void php3_imagesetpixel(INTERNAL_FUNCTION_PARAMETERS) {
	pval *imarg, *xarg, *yarg, *colarg;
	gdImagePtr im;
	int col, y, x;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 ||
		getParameters(ht, 4, &imarg, &xarg, &yarg, &colarg) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(imarg);
	convert_to_long(xarg);
	convert_to_long(yarg);
	convert_to_long(colarg);

	col = colarg->value.lval;
	y = yarg->value.lval;
	x = xarg->value.lval;

	im = php3_list_find(imarg->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageSetPixel(im,x,y,col);

	RETURN_TRUE;
}
/* }}} */	

/* im, x1, y1, x2, y2, col */

/* {{{ proto int imageline(int im, int x1, int y1, int x2, int y2, int col)
   Draw a line */
void php3_imageline(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM, *COL, *X1, *Y1, *X2, *Y2;
	gdImagePtr im;
	int col, y2, x2, y1, x1;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 ||
		getParameters(ht, 6, &IM, &X1, &Y1, &X2, &Y2, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X1);
	convert_to_long(Y1);
	convert_to_long(X2);
	convert_to_long(Y2);
	convert_to_long(COL);

	x1 = X1->value.lval;
	y1 = Y1->value.lval;
	x2 = X2->value.lval;
	y2 = Y2->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageLine(im,x1,y1,x2,y2,col);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagedashedline(int im, int x1, int y1, int x2, int y2, int col)
   Draw a dashed line */
void php3_imagedashedline(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM, *COL, *X1, *Y1, *X2, *Y2;
	gdImagePtr im;
	int col, y2, x2, y1, x1;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 || getParameters(ht, 6, &IM, &X1, &Y1, &X2, &Y2, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X1);
	convert_to_long(Y1);
	convert_to_long(X2);
	convert_to_long(Y2);
	convert_to_long(COL);

	x1 = X1->value.lval;
	y1 = Y1->value.lval;
	x2 = X2->value.lval;
	y2 = Y2->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageDashedLine(im,x1,y1,x2,y2,col);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagegammacorrect(int im, double inputgamma, double outputgamma)
   Apply a gamma correction to a GD image */
void php3_imagegammacorrect(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM,*inputgamma, *outputgamma;
	gdImagePtr im;
	int ind_type, i;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 3 || getParameters(ht, 3, &IM, &inputgamma, &outputgamma)
		== FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_double(inputgamma);
	convert_to_double(outputgamma);

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	for (i = 0; i < gdImageColorsTotal(im); i++) {
		im->red[i] = (int)((pow((pow((im->red[i] / 255.0),inputgamma->value.dval)), 1.0 / outputgamma->value.dval) * 255)+.5);
		im->green[i] = (int)((pow((pow((im->green[i] / 255.0),inputgamma->value.dval)), 1.0 / outputgamma->value.dval) * 255)+.5);
		im->blue[i] = (int)((pow((pow((im->blue[i] / 255.0),inputgamma->value.dval)), 1.0 / outputgamma->value.dval) * 255)+.5);
	}
	
	RETURN_TRUE;
}
/* }}} */

/* im, x1, y1, x2, y2, col */
/* {{{ proto int imagerectangle(int im, int x1, int y1, int x2, int y2, int col)
   Draw a rectangle */
void php3_imagerectangle(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM, *COL, *X1, *Y1, *X2, *Y2;
	gdImagePtr im;
	int col, y2, x2, y1, x1;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 ||
		getParameters(ht, 6, &IM, &X1, &Y1, &X2, &Y2, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X1);
	convert_to_long(Y1);
	convert_to_long(X2);
	convert_to_long(Y2);
	convert_to_long(COL);

	x1 = X1->value.lval;
	y1 = Y1->value.lval;
	x2 = X2->value.lval;
	y2 = Y2->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageRectangle(im,x1,y1,x2,y2,col);
	RETURN_TRUE;
}
/* }}} */	

/* im, x1, y1, x2, y2, col */

/* {{{ proto int imagefilledrectangle(int im, int x1, int y1, int x2, int y2, int col)
   Draw a filled rectangle */
void php3_imagefilledrectangle(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM, *COL, *X1, *Y1, *X2, *Y2;
	gdImagePtr im;
	int col, y2, x2, y1, x1;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 ||
		getParameters(ht, 6, &IM, &X1, &Y1, &X2, &Y2, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X1);
	convert_to_long(Y1);
	convert_to_long(X2);
	convert_to_long(Y2);
	convert_to_long(COL);

	x1 = X1->value.lval;
	y1 = Y1->value.lval;
	x2 = X2->value.lval;
	y2 = Y2->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageFilledRectangle(im,x1,y1,x2,y2,col);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagearc(int im, int cx, int cy, int w, int h, int s, int e, int col)
   Draw a partial ellipse */
void php3_imagearc(INTERNAL_FUNCTION_PARAMETERS) {
	pval *COL, *E, *ST, *H, *W, *CY, *CX, *IM;
	gdImagePtr im;
	int col, e, st, h, w, cy, cx;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 8 ||
		getParameters(ht, 8, &IM, &CX, &CY, &W, &H, &ST, &E, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(CX);
	convert_to_long(CY);
	convert_to_long(W);
	convert_to_long(H);
	convert_to_long(ST);
	convert_to_long(E);
	convert_to_long(COL);

	col = COL->value.lval;
	e = E->value.lval;
	st = ST->value.lval;
	h = H->value.lval;
	w = W->value.lval;
	cy = CY->value.lval;
	cx = CX->value.lval;

	if (e < 0) {
		e %= 360;
	}
	if (st < 0) {
		st %= 360;
	}

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageArc(im,cx,cy,w,h,st,e,col);
	RETURN_TRUE;
}
/* }}} */	

/* im, x, y, border, col */

/* {{{ proto int imagefilltoborder(int im, int x, int y, int border, int col)
   Flood fill to specific color */
void php3_imagefilltoborder(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM, *X, *Y, *BORDER, *COL;
	gdImagePtr im;
	int col, border, y, x;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 5 ||
		getParameters(ht, 5, &IM, &X, &Y, &BORDER, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X);
	convert_to_long(Y);
	convert_to_long(BORDER);
	convert_to_long(COL);

	col = COL->value.lval;
	border = BORDER->value.lval;
	y = Y->value.lval;
	x = X->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageFillToBorder(im,x,y,border,col);
	RETURN_TRUE;
}
/* }}} */	

/* im, x, y, col */

/* {{{ proto int imagefill(int im, int x, int y, int col)
   Flood fill */
void php3_imagefill(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM, *X, *Y, *COL;
	gdImagePtr im;
	int col, y, x;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 ||
		getParameters(ht, 4, &IM, &X, &Y, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(X);
	convert_to_long(Y);
	convert_to_long(COL);

	col = COL->value.lval;
	y = Y->value.lval;
	x = X->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageFill(im,x,y,col);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagecolorstotal(int im)
   Find out the number of colors in an image's palette */
void php3_imagecolorstotal(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM;
	gdImagePtr im;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &IM) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(IM);

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	RETURN_LONG(gdImageColorsTotal(im));
}
/* }}} */	

/* im, col */

/* {{{ proto int imagecolortransparent(int im [, int col])
   Define a color as transparent */
void php3_imagecolortransparent(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM, *COL = NULL;
	gdImagePtr im;
	int col;
	int ind_type;
	GD_TLS_VARS;

	switch(ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht, 1, &IM) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (getParameters(ht, 2, &IM, &COL) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(COL);
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_long(IM);

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	if (COL != NULL) {
		col = COL->value.lval;
		gdImageColorTransparent(im,col);
	}
	col = gdImageGetTransparent(im);
	RETURN_LONG(col);
}
/* }}} */	

/* im, interlace */

/* {{{ proto int imageinterlace(int im [, int interlace])
   Enable or disable interlace */
void php3_imageinterlace(INTERNAL_FUNCTION_PARAMETERS) {
	pval *IM, *INT = NULL;
	gdImagePtr im;
	int interlace;
	int ind_type;
	GD_TLS_VARS;

	switch(ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht, 1, &IM) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 2:
		if (getParameters(ht, 2, &IM, &INT) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(INT);
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_long(IM);

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	if (INT != NULL) {
		interlace = INT->value.lval;
		gdImageInterlace(im,interlace);
	}
	interlace = gdImageGetInterlaced(im);
	RETURN_LONG(interlace);
}
/* }}} */	

/* arg = 0  normal polygon
   arg = 1  filled polygon */
/* im, points, num_points, col */
static void _php3_imagepolygon(INTERNAL_FUNCTION_PARAMETERS, int filled) {
	pval *IM, *POINTS, *NPOINTS, *COL, *var;
	gdImagePtr im;
	gdPointPtr points;
	int npoints, col, nelem, i;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 4 ||
		getParameters(ht, 4, &IM, &POINTS, &NPOINTS, &COL) == FAILURE)
	{
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(NPOINTS);
	convert_to_long(COL);

	npoints = NPOINTS->value.lval;
	col = COL->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	if (POINTS->type != IS_ARRAY) {
		php3_error(E_WARNING, "2nd argument to imagepolygon not an array");
		RETURN_FALSE;
	}

/*
    ** we shouldn't need this check, should we? **

    if (!ParameterPassedByReference(ht, 2)) {
        php3_error(E_WARNING, "2nd argument to imagepolygon not passed by reference");
		RETURN_FALSE;
    }
*/

	nelem = _php3_hash_num_elements(POINTS->value.ht);
	if (nelem < 6) {
		php3_error(E_WARNING,
					"you must have at least 3 points in your array");
		RETURN_FALSE;
	}

	if (nelem < npoints * 2) {
		php3_error(E_WARNING,
					"trying to use %d points in array with only %d points",
					npoints, nelem/2);
		RETURN_FALSE;
	}

	points = (gdPointPtr) emalloc(npoints * sizeof(gdPoint));

	for (i = 0; i < npoints; i++) {
		if (_php3_hash_index_find(POINTS->value.ht, (i * 2), (void **)&var) == SUCCESS) {
			convert_to_long(var);
			points[i].x = var->value.lval;
		}
		if (_php3_hash_index_find(POINTS->value.ht, (i * 2) + 1, (void **)&var) == SUCCESS) {
			convert_to_long(var);
			points[i].y = var->value.lval;
		}
	}

	if (filled) {
		gdImageFilledPolygon(im, points, npoints, col);
	}
	else {
		gdImagePolygon(im, points, npoints, col);
	}

	efree(points);

	RETURN_TRUE;
}


/* {{{ proto int imagepolygon(int im, array point, int num_points, int col)
   Draw a polygon */
void php3_imagepolygon(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagefilledpolygon(int im, array point, int num_points, int col)
   Draw a filled polygon */
void php3_imagefilledpolygon(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_imagepolygon(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


static gdFontPtr _php3_find_gd_font(HashTable *list, int size)
{
	gdFontPtr font;
	int ind_type;
	GD_TLS_VARS;

	switch (size) {
    	case 1:
			 font = gdFontTiny;
			 break;
    	case 2:
			 font = gdFontSmall;
			 break;
    	case 3:
			 font = gdFontMediumBold;
			 break;
    	case 4:
			 font = gdFontLarge;
			 break;
    	case 5:
			 font = gdFontGiant;
			 break;
	    default:
			font = php3_list_find(size - 5, &ind_type);
			 if (!font || ind_type != GD_GLOBAL(le_gd_font)) {
				  if (size < 1) {
					   font = gdFontTiny;
				  } else {
					   font = gdFontGiant;
				  }
			 }
			 break;
	}

	return font;
}


/*
 * arg = 0  ImageFontWidth
 * arg = 1  ImageFontHeight
 */
static void _php3_imagefontsize(INTERNAL_FUNCTION_PARAMETERS, int arg)
{
	pval *SIZE;
	gdFontPtr font;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &SIZE) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(SIZE);
	font = _php3_find_gd_font(list, SIZE->value.lval);
	RETURN_LONG(arg ? font->h : font->w);
}

/* {{{ proto int imagefontwidth(int font)
   Get font width */
void php3_imagefontwidth(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_imagefontsize(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagefontheight(int font)
   Get font height */
void php3_imagefontheight(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_imagefontsize(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */


/* workaround for a bug in gd 1.2 */
void _php3_gdimagecharup(gdImagePtr im, gdFontPtr f, int x, int y, int c,
						 int color)
{
	int cx, cy, px, py, fline;
	cx = 0;
	cy = 0;
	if ((c < f->offset) || (c >= (f->offset + f->nchars))) {
		return;
	}
	fline = (c - f->offset) * f->h * f->w;
	for (py = y; (py > (y - f->w)); py--) {
		for (px = x; (px < (x + f->h)); px++) {
			if (f->data[fline + cy * f->w + cx]) {
				gdImageSetPixel(im, px, py, color);	
			}
			cy++;
		}
		cy = 0;
		cx++;
	}
}

/*
 * arg = 0  ImageChar
 * arg = 1  ImageCharUp
 * arg = 2  ImageString
 * arg = 3  ImageStringUp
 */
static void _php3_imagechar(INTERNAL_FUNCTION_PARAMETERS, int mode) {
	pval *IM, *SIZE, *X, *Y, *C, *COL;
	gdImagePtr im;
	int ch = 0, col, x, y, size, i, l = 0;
	unsigned char *string = NULL;
	int ind_type;
	gdFontPtr font;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 6 ||
		getParameters(ht, 6, &IM, &SIZE, &X, &Y, &C, &COL) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(IM);
	convert_to_long(SIZE);
	convert_to_long(X);
	convert_to_long(Y);
	convert_to_string(C);
	convert_to_long(COL);

	col = COL->value.lval;

	if (mode < 2) {
		ch = (int)((unsigned char)*(C->value.str.val));
	} else {
		string = (unsigned char *) estrndup(C->value.str.val,C->value.str.len);
		l = strlen(string);
	}

	y = Y->value.lval;
	x = X->value.lval;
	size = SIZE->value.lval;

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		if (string) {
			efree(string);
		}
		RETURN_FALSE;
	}
	
	font = _php3_find_gd_font(list, size);

	switch(mode) {
    	case 0:
			gdImageChar(im, font, x, y, ch, col);
			break;
    	case 1:
			_php3_gdimagecharup(im, font, x, y, ch, col);
			break;
    	case 2:
			for (i = 0; (i < l); i++) {
				gdImageChar(im, font, x, y, (int)((unsigned char)string[i]),
							col);
				x += font->w;
			}
			break;
    	case 3: {
			for (i = 0; (i < l); i++) {
				/* _php3_gdimagecharup(im, font, x, y, (int)string[i], col); */
				gdImageCharUp(im, font, x, y, (int)string[i], col);
				y -= font->w;
			}
			break;
		}
	}
	if (string) {
		efree(string);
	}
	RETURN_TRUE;
}	

/* {{{ proto int imagechar(int im, int font, int x, int y, string c, int col)
   Draw a character */ 
void php3_imagechar(INTERNAL_FUNCTION_PARAMETERS) {
	_php3_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ proto int imagecharup(int im, int font, int x, int y, string c, int col)
   Draw a character rotated 90 degrees counter-clockwise */
void php3_imagecharup(INTERNAL_FUNCTION_PARAMETERS) {
	_php3_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int imagestring(int im, int font, int x, int y, string str, int col)
   Draw a string horizontally */
void php3_imagestring(INTERNAL_FUNCTION_PARAMETERS) {
	_php3_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 2);
}
/* }}} */

/* {{{ proto int imagestringup(int im, int font, int x, int y, string str, int col)
   Draw a string vertically - rotated 90 degrees counter-clockwise */
void php3_imagestringup(INTERNAL_FUNCTION_PARAMETERS) {
	_php3_imagechar(INTERNAL_FUNCTION_PARAM_PASSTHRU, 3);
}
/* }}} */

/* {{{ proto int imagecopy(int dst_im, int src_im, int dst_x, int dst_y, int src_x, int src_y, int src_w, int src_h)
   Copy part of an image */ 
void php3_imagecopy(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *SIM, *DIM, *SX, *SY, *SW, *SH, *DX, *DY;
	gdImagePtr im_dst;
	gdImagePtr im_src;
	int srcH, srcW, srcY, srcX, dstY, dstX;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 8 ||
		getParameters(ht, 8, &DIM, &SIM, &DX, &DY, &SX, &SY, &SW, &SH)
						 == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(SIM);
	convert_to_long(DIM);
	convert_to_long(SX);
	convert_to_long(SY);
	convert_to_long(SW);
	convert_to_long(SH);
	convert_to_long(DX);
	convert_to_long(DY);

	srcX = SX->value.lval;
	srcY = SY->value.lval;
	srcH = SH->value.lval;
	srcW = SW->value.lval;
	dstX = DX->value.lval;
	dstY = DY->value.lval;

	im_src = php3_list_find(SIM->value.lval, &ind_type);
	if (!im_src || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	im_dst = php3_list_find(DIM->value.lval, &ind_type);
	if (!im_dst || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageCopy(im_dst, im_src, dstX, dstY, srcX, srcY, srcW, srcH);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int imagecopyresized(int dst_im, int src_im, int dst_x, int dst_y, int src_x, int src_y, int dst_w, int dst_h, int src_w, int src_h);
   Copy and resize part of an image */
void php3_imagecopyresized(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *SIM, *DIM, *SX, *SY, *SW, *SH, *DX, *DY, *DW, *DH;
	gdImagePtr im_dst;
	gdImagePtr im_src;
	int srcH, srcW, dstH, dstW, srcY, srcX, dstY, dstX;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 10 ||
		getParameters(ht, 10, &DIM, &SIM, &DX, &DY, &SX, &SY, &DW, &DH,
					  &SW, &SH) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(SIM);
	convert_to_long(DIM);
	convert_to_long(SX);
	convert_to_long(SY);
	convert_to_long(SW);
	convert_to_long(SH);
	convert_to_long(DX);
	convert_to_long(DY);
	convert_to_long(DW);
	convert_to_long(DH);

	srcX = SX->value.lval;
	srcY = SY->value.lval;
	srcH = SH->value.lval;
	srcW = SW->value.lval;
	dstX = DX->value.lval;
	dstY = DY->value.lval;
	dstH = DH->value.lval;
	dstW = DW->value.lval;

	im_src = php3_list_find(SIM->value.lval, &ind_type);
	if (!im_src || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	im_dst = php3_list_find(DIM->value.lval, &ind_type);
	if (!im_dst || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	gdImageCopyResized(im_dst, im_src, dstX, dstY, srcX, srcY, dstW, dstH,
					   srcW, srcH);
	RETURN_TRUE;
}
/* }}} */	

/* {{{ proto int imagesx(int im)
   Get image width */
void php3_imagesxfn(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *IM;
	gdImagePtr im;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &IM) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	RETURN_LONG(gdImageSX(im));
}
/* }}} */

/* {{{ proto int imagesy(int im)
   Get image height */
void php3_imagesyfn(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *IM;
	gdImagePtr im;
	int ind_type;
	GD_TLS_VARS;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &IM) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	im = php3_list_find(IM->value.lval, &ind_type);
	if (!im || ind_type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	RETURN_LONG(gdImageSY(im));
}
/* }}} */

#if HAVE_LIBTTF|HAVE_LIBFREETYPE
#define TTFTEXT_DRAW 0
#define TTFTEXT_BBOX 1
#endif

/* {{{ proto array imagettfbbox(int size, int angle, string font_file, string text)
   Give the bounding box of a text using TrueType fonts */
void php3_imagettfbbox(INTERNAL_FUNCTION_PARAMETERS)
{
#if HAVE_LIBTTF|HAVE_LIBFREETYPE
	php3_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_BBOX);
#else 
	php3_error(E_WARNING, "ImageTtfBBox: No TTF support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

/* {{{ proto array imagettftext(int im, int size, int angle, int x, int y, int col, string font_file, string text)
   Write text to the image using a TrueType font */
void php3_imagettftext(INTERNAL_FUNCTION_PARAMETERS)
{
#if HAVE_LIBTTF|HAVE_LIBFREETYPE
	php3_imagettftext_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, TTFTEXT_DRAW);
#else 
	php3_error(E_WARNING, "ImageTtfBBox: No TTF support in this PHP build");
	RETURN_FALSE;
#endif 
}
/* }}} */

#if HAVE_LIBTTF|HAVE_LIBFREETYPE

static
void php3_imagettftext_common(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	pval *IM, *PTSIZE, *ANGLE, *X, *Y, *C, *FONTNAME, *COL;
	gdImagePtr im;
	int  col, x, y, l=0, i;
	int brect[8];
	double ptsize, angle;
	unsigned char *string = NULL, *fontname = NULL;
	int ind_type;
	char				*error;

	GD_TLS_VARS;

	if (mode == TTFTEXT_BBOX) {
		if (ARG_COUNT(ht) != 4 || getParameters(ht, 4, &PTSIZE, &ANGLE, &FONTNAME, &C) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	} else {
		if (ARG_COUNT(ht) != 8 || getParameters(ht, 8, &IM, &PTSIZE, &ANGLE, &X, &Y, &COL, &FONTNAME, &C) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	}

	convert_to_double(PTSIZE);
	convert_to_double(ANGLE);
	convert_to_string(FONTNAME);
	convert_to_string(C);
	if (mode == TTFTEXT_BBOX) {
              im = NULL;
		col = x = y = -1;
	} else {
		convert_to_long(X);
		convert_to_long(Y);
		convert_to_long(IM);
		convert_to_long(COL);
		col = COL->value.lval;
		y = Y->value.lval;
		x = X->value.lval;
              im = php3_list_find(IM->value.lval, &ind_type);
              if (!im || ind_type != GD_GLOBAL(le_gd)) {
                      php3_error(E_WARNING, "Unable to find image pointer");
                      RETURN_FALSE;
              }
	}

	ptsize = PTSIZE->value.dval;
	angle = ANGLE->value.dval * (M_PI/180); /* convert to radians */

	string = (unsigned char *) C->value.str.val;
	l = strlen(string);
	fontname = (unsigned char *) FONTNAME->value.str.val;

	error = gdttf(im, brect, col, fontname, ptsize, angle, x, y, string);

	if (error) {
		php3_error(E_WARNING, "%s", error);
		RETURN_FALSE;
	}

	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	/* return array with the text's bounding box */
	for (i = 0; i < 8; i++) {
		add_next_index_long(return_value, brect[i]);
	}
}
#endif

#if HAVE_LIBT1

void _php3_free_ps_font(gd_ps_font *f_ind)
{
	T1_DeleteFont(f_ind->font_id);
	efree(f_ind);
}

void _php3_free_ps_enc(char **enc)
{
	T1_DeleteEncoding(enc);
}

#endif

/* {{{ proto int imagepsloadfont(string pathname)
   Load a new font from specified file */
void php3_imagepsloadfont(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_LIBT1
	pval *file;
	int l_ind;
	gd_ps_font *f_ind;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &file) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string(file);

	f_ind = emalloc(sizeof(gd_ps_font));
	f_ind->font_id = T1_AddFont(file->value.str.val);

	if (f_ind->font_id < 0) {
		l_ind = f_ind->font_id;
		efree(f_ind);
		switch (l_ind) {
		case -1:
			php3_error(E_WARNING, "Couldn't find the font file");
			RETURN_FALSE;
			break;
		case -2:
		case -3:
			php3_error(E_WARNING, "Memory allocation fault in t1lib");
			RETURN_FALSE;
			break;
		default:
			php3_error(E_WARNING, "An unknown error occurred in t1lib");
			RETURN_FALSE;
			break;
		}
	}

	T1_LoadFont(f_ind->font_id);
	f_ind->extend = 1;
	l_ind = php3_list_insert(f_ind, GD_GLOBAL(le_ps_font));
	RETURN_LONG(l_ind);
#else /* HAVE_LIBT1 */
	php3_error(E_WARNING, "ImagePsLoadFont: No T1lib support in this PHP build");
	RETURN_FALSE;
#endif /* HAVE_LIBT1 */
}
/* }}} */

/* {{{ The function in t1lib which this function uses seem to be buggy...
proto int imagepscopyfont(int font_index)
   Make a copy of a font for purposes like extending or reenconding */
/*
void php3_imagepscopyfont(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_LIBT1
	pval *fnt;
	int l_ind, type;
	gd_ps_font *nf_ind, *of_ind;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &fnt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fnt);

	of_ind = php3_list_find(fnt->value.lval, &type);

	if (type != GD_GLOBAL(le_ps_font)) {
		php3_error(E_WARNING, "%d is not a Type 1 font index", fnt->value.lval);
		RETURN_FALSE;
	}

	nf_ind = emalloc(sizeof(gd_ps_font));
	nf_ind->font_id = T1_CopyFont(of_ind->font_id);

	if (nf_ind->font_id < 0) {
		l_ind = nf_ind->font_id;
		efree(nf_ind);
		switch (l_ind) {
		case -1:
			php3_error(E_WARNING, "FontID %d is not loaded in memory", l_ind);
			RETURN_FALSE;
			break;
		case -2:
			php3_error(E_WARNING, "Tried to copy a logical font");
			RETURN_FALSE;
			break;
		case -3:
			php3_error(E_WARNING, "Memory allocation fault in t1lib");
			RETURN_FALSE;
			break;
		default:
			php3_error(E_WARNING, "An unknown error occurred in t1lib");
			RETURN_FALSE;
			break;
		}
	}

	nf_ind->extend = 1;
	l_ind = php3_list_insert(nf_ind, GD_GLOBAL(le_ps_font));
	RETURN_LONG(l_ind);
#else 
    php3_error(E_WARNING, "ImagePsCopyFont: No T1lib support in this PHP build");
    RETURN_FALSE;
#endif
}
*/
/* }}} */

/* {{{ proto bool imagepsfreefont(int font_index)
   Free memory used by a font */
void php3_imagepsfreefont(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_LIBT1
	pval *fnt;
	int type;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &fnt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fnt);

	php3_list_find(fnt->value.lval, &type);

	if (type != GD_GLOBAL(le_ps_font)) {
		php3_error(E_WARNING, "%d is not a Type 1 font index", fnt->value.lval);
		RETURN_FALSE;
	}

	php3_list_delete(fnt->value.lval);
	RETURN_TRUE;
#else 
    php3_error(E_WARNING, "ImagePsFreeFont: No T1lib support in this PHP build");
    RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool imagepsencodefont(int font_index, string filename)
   To change a fonts character encoding vector */
void php3_imagepsencodefont(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_LIBT1
	pval *fnt, *enc;
	char **enc_vector;
	int type;
	gd_ps_font *f_ind;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &fnt, &enc) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fnt);
	convert_to_string(enc);

	f_ind = php3_list_find(fnt->value.lval, &type);

	if (type != GD_GLOBAL(le_ps_font)) {
		php3_error(E_WARNING, "%d is not a Type 1 font index", fnt->value.lval);
		RETURN_FALSE;
	}

	if ((enc_vector = T1_LoadEncoding(enc->value.str.val)) == NULL) {
		php3_error(E_WARNING, "Couldn't load encoding vector from %s", enc->value.str.val);
		RETURN_FALSE;
	}

	T1_DeleteAllSizes(f_ind->font_id);
	if (T1_ReencodeFont(f_ind->font_id, enc_vector)) {
		T1_DeleteEncoding(enc_vector);
		php3_error(E_WARNING, "Couldn't reencode font");
		RETURN_FALSE;
	}
	php3_list_insert(enc_vector, GD_GLOBAL(le_ps_enc));
	RETURN_TRUE;
#else 
    php3_error(E_WARNING, "ImagePsEncodeFont: No T1lib support in this PHP build");
    RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool imagepsextendfont(int font_index, double extend)
   Extend or or condense (if extend < 1) a font */
void php3_imagepsextendfont(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_LIBT1
	pval *fnt, *ext;
	int type;
	gd_ps_font *f_ind;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &fnt, &ext) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fnt);
	convert_to_double(ext);

	f_ind = php3_list_find(fnt->value.lval, &type);

	if (type != GD_GLOBAL(le_ps_font)) {
		php3_error(E_WARNING, "%d is not a Type 1 font index", fnt->value.lval);
		RETURN_FALSE;
	}

	if (T1_ExtendFont(f_ind->font_id, ext->value.dval) != 0) RETURN_FALSE;
	f_ind->extend = ext->value.dval;
	RETURN_TRUE;
#else 
    php3_error(E_WARNING, "ImagePsExtendFont: No T1lib support in this PHP build");
    RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto bool imagepsslantfont(int font_index, double slant)
   Slant a font */
void php3_imagepsslantfont(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_LIBT1
	pval *fnt, *slt;
	int type;
	gd_ps_font*f_ind;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &fnt, &slt) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long(fnt);
	convert_to_double(slt);

	f_ind = php3_list_find(fnt->value.lval, &type);

	if (type != GD_GLOBAL(le_ps_font)) {
		php3_error(E_WARNING, "%d is not a Type 1 font index", fnt->value.lval);
		RETURN_FALSE;
	}

	if (T1_SlantFont(f_ind->font_id, slt->value.dval) != 0) RETURN_FALSE;
	RETURN_TRUE;
#else 
    php3_error(E_WARNING, "ImagePsSlantFont: No T1lib support in this PHP build");
    RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto array imagepstext(int image, string text, int font, int size, int xcoord, int ycoord [, int space, int tightness, double angle, int antialias])
   Rasterize a string over an image */
void php3_imagepstext(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_LIBT1
	pval *img, *str, *fnt, *sz, *fg, *bg, *sp, *px, *py, *aas, *wd, *ang;
	int i, j, x, y;
	int space, type;
	gd_ps_font *f_ind;
	int h_lines, v_lines, c_ind;
	int rd, gr, bl, fg_rd, fg_gr, fg_bl, bg_rd, bg_gr, bg_bl;
	int aa[16], aa_steps;
	int width, amount_kern, add_width;
	double angle;
	unsigned long aa_greys[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	gdImagePtr bg_img;
	GLYPH *str_img;
#ifdef HAVE_LIBT1_OUTLINE
	T1_OUTLINE *char_path, *str_path;
	T1_TMATRIX *transform = NULL;
#endif

	switch(ARG_COUNT(ht)) {
	case 8:
		if (getParameters(ht, 8, &img, &str, &fnt, &sz, &fg, &bg, &px, &py) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string(str);
		convert_to_long(fnt);
		convert_to_long(sz);
		convert_to_long(fg);
		convert_to_long(bg);
		convert_to_long(px);
		convert_to_long(py);
		x = px->value.lval;
		y = py->value.lval;
		space = 0;
		aa_steps = 4;
		width = 0;
		angle = 0;
		break;
	case 12:
		if (getParameters(ht, 12, &img, &str, &fnt, &sz, &fg, &bg, &px, &py, &sp, &wd, &ang, &aas) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string(str);
		convert_to_long(fnt);
		convert_to_long(sz);
		convert_to_long(sp);
		convert_to_long(fg);
		convert_to_long(bg);
		convert_to_long(px);
		convert_to_long(py);
		x = px->value.lval;
		y = py->value.lval;
		convert_to_long(sp);
		space = sp->value.lval;
		convert_to_long(aas);
		aa_steps = aas->value.lval;
		convert_to_long(wd);
		width = wd->value.lval;
		convert_to_double(ang);
		angle = ang->value.dval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	bg_img = php3_list_find(img->value.lval, &type);

	if (!bg_img || type != GD_GLOBAL(le_gd)) {
		php3_error(E_WARNING, "Unable to find image pointer");
		RETURN_FALSE;
	}

	f_ind = php3_list_find(fnt->value.lval, &type);

	if (!f_ind || type != GD_GLOBAL(le_ps_font)) {
		php3_error(E_WARNING, "%d is not a Type 1 font index", fnt->value.lval);
		RETURN_FALSE;
	}

	fg_rd = gdImageRed(bg_img, fg->value.lval);
	fg_gr = gdImageGreen(bg_img, fg->value.lval);
	fg_bl = gdImageBlue(bg_img, fg->value.lval);
	bg_rd = gdImageRed(bg_img, bg->value.lval);
	bg_gr = gdImageGreen(bg_img, bg->value.lval);
	bg_bl = gdImageBlue(bg_img, bg->value.lval);

	for (i = 0; i < aa_steps; i++) {
		rd = bg_rd+(double)(fg_rd-bg_rd)/aa_steps*(i+1);
		gr = bg_gr+(double)(fg_gr-bg_gr)/aa_steps*(i+1);
		bl = bg_bl+(double)(fg_bl-bg_bl)/aa_steps*(i+1);
		aa[i] = gdImageColorResolve(bg_img, rd, gr, bl);
	}

	T1_AASetBitsPerPixel(8);

	switch (aa_steps) {
	case 4:
		T1_AASetGrayValues(0, 1, 2, 3, 4);
		T1_AASetLevel(T1_AA_LOW);
		break;
	case 16:
		T1_AAHSetGrayValues(aa_greys);
		T1_AASetLevel(T1_AA_HIGH);
		break;
	default:
		php3_error(E_WARNING, "Invalid value %d as number of steps for antialiasing", aa_steps);
		RETURN_FALSE;
	}

	if (angle) {
		transform = T1_RotateMatrix(NULL, angle);
	}

	if (width) {
#ifdef HAVE_LIBT1_OUTLINE
	str_path = T1_GetCharOutline(f_ind->font_id, str->value.str.val[0], sz->value.lval, transform);

	for (i = 1; i < str->value.str.len; i++) {
		amount_kern = (int) T1_GetKerning(f_ind->font_id, str->value.str.val[i-1], str->value.str.val[i]);
		amount_kern += str->value.str.val[i-1] == ' ' ? space : 0;
		add_width = (int) (amount_kern+width)/f_ind->extend;

		char_path = T1_GetMoveOutline(f_ind->font_id, add_width, 0, 0, sz->value.lval, transform);
		str_path = T1_ConcatOutlines(str_path, char_path);

		char_path = T1_GetCharOutline(f_ind->font_id, str->value.str.val[i], sz->value.lval, transform);
		str_path = T1_ConcatOutlines(str_path, char_path);
	}
	str_img = T1_AAFillOutline(str_path, 0);
#else
	php3_error(E_WARNING, "Setting space between characters in function ImagePSText is supported only with t1lib version 0.9 or above");
	RETURN_FALSE;
#endif
	} else {
		str_img = T1_AASetString(f_ind->font_id, str->value.str.val,  str->value.str.len,
								 space, T1_KERNING, sz->value.lval, transform);
	}

	if (T1_errno) RETURN_FALSE;

	h_lines = str_img->metrics.ascent -  str_img->metrics.descent;
	v_lines = str_img->metrics.rightSideBearing - str_img->metrics.leftSideBearing;

	for (i = 0; i < v_lines; i++) {
		for (j = 0; j < h_lines; j++) {
			switch (str_img->bits[j*v_lines+i]) {
			case 0:
				break;
			default:
				c_ind = aa[str_img->bits[j*v_lines+i]-1];
				gdImageSetPixel(bg_img, x+str_img->metrics.leftSideBearing+i, y-str_img->metrics.ascent+j, c_ind);
			}
		}
	}

	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	add_next_index_long(return_value, str_img->metrics.leftSideBearing);
	add_next_index_long(return_value, str_img->metrics.descent);
	add_next_index_long(return_value, str_img->metrics.rightSideBearing);
	add_next_index_long(return_value, str_img->metrics.ascent);

#else 
    php3_error(E_WARNING, "ImagePsText: No T1lib support in this PHP build");
    RETURN_FALSE;
#endif
}
/* }}} */

/* {{{ proto array imagepsbbox(string text, int font, int size [, int space, int tightness, int angle])
   Return the bounding box needed by a string if rasterized */
void php3_imagepsbbox(INTERNAL_FUNCTION_PARAMETERS) {
#if HAVE_LIBT1
	pval *str, *fnt, *sz, *sp, *wd, *ang;
	int i, space, add_width, char_width, amount_kern, type;
	int cur_x, cur_y, dx, dy;
	int x1, y1, x2, y2, x3, y3, x4, y4;
	gd_ps_font *f_ind;
	int per_char = 0;
	double angle, sin_a, cos_a;
	BBox char_bbox, str_bbox = {0, 0, 0, 0};

	switch(ARG_COUNT(ht)) {
	case 3:
		if (getParameters(ht, 3, &str, &fnt, &sz) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string(str);
		convert_to_long(fnt);
		convert_to_long(sz);
		space = 0;
		break;
	case 6:
		if (getParameters(ht, 6, &str, &fnt, &sz, &sp, &wd, &ang) == FAILURE) {
			RETURN_FALSE;
		}
		convert_to_string(str);
		convert_to_long(fnt);
		convert_to_long(sz);
		convert_to_long(sp);
		space = sp->value.lval;
		convert_to_long(wd);
		add_width = wd->value.lval;
		convert_to_double(ang);
		angle = ang->value.dval * M_PI / 180;
		sin_a = sin(angle);
		cos_a = cos(angle);
		per_char =  add_width || angle ? 1 : 0;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	f_ind = php3_list_find(fnt->value.lval, &type);

	if (type != GD_GLOBAL(le_ps_font)) {
		php3_error(E_WARNING, "%d is not a Type 1 font index", fnt->value.lval);
		RETURN_FALSE;
	}

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)
#define new_x(a, b) (int) ((a) * cos_a - (b) * sin_a)
#define new_y(a, b) (int) ((a) * sin_a + (b) * cos_a)

	if (per_char) {
		space += T1_GetCharWidth(f_ind->font_id, ' ');
		cur_x = cur_y = 0;

		for (i = 0; i < str->value.str.len; i++) {
			if (str->value.str.val[i] == ' ') {
				char_bbox.llx = char_bbox.lly = char_bbox.ury = 0;
				char_bbox.urx = char_width = space;
			} else {
				char_bbox = T1_GetCharBBox(f_ind->font_id, str->value.str.val[i]);
				char_width = T1_GetCharWidth(f_ind->font_id, str->value.str.val[i]);
			}
			amount_kern = i ? T1_GetKerning(f_ind->font_id, str->value.str.val[i-1], str->value.str.val[i]) : 0;

			/* Transfer character bounding box to right place */
			x1 = new_x(char_bbox.llx, char_bbox.lly) + cur_x;
			y1 = new_y(char_bbox.llx, char_bbox.lly) + cur_y;
			x2 = new_x(char_bbox.llx, char_bbox.ury) + cur_x;
			y2 = new_y(char_bbox.llx, char_bbox.ury) + cur_y;
			x3 = new_x(char_bbox.urx, char_bbox.ury) + cur_x;
			y3 = new_y(char_bbox.urx, char_bbox.ury) + cur_y;
			x4 = new_x(char_bbox.urx, char_bbox.lly) + cur_x;
			y4 = new_y(char_bbox.urx, char_bbox.lly) + cur_y;

			/* Find min & max values and compare them with current bounding box */
			str_bbox.llx = min(str_bbox.llx, min(x1, min(x2, min(x3, x4))));
			str_bbox.lly = min(str_bbox.lly, min(y1, min(y2, min(y3, y4))));
			str_bbox.urx = max(str_bbox.urx, max(x1, max(x2, max(x3, x4))));
			str_bbox.ury = max(str_bbox.ury, max(y1, max(y2, max(y3, y4))));

			/* Move to the next base point */
			dx = new_x(char_width + add_width + amount_kern, 0);
			dy = new_y(char_width + add_width + amount_kern, 0);
			cur_x += dx;
			cur_y += dy;
			/*
			printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", x1, y1, x2, y2, x3, y3, x4, y4, char_bbox.llx, char_bbox.lly, char_bbox.urx, char_bbox.ury, char_width, amount_kern, cur_x, cur_y, dx, dy);
			*/
		}

	} else {
		str_bbox = T1_GetStringBBox(f_ind->font_id, str->value.str.val, str->value.str.len, space, T1_KERNING);
	}
	if (T1_errno) RETURN_FALSE;
	
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	/*
	printf("%d %d %d %d\n", str_bbox.llx, str_bbox.lly, str_bbox.urx, str_bbox.ury);
	*/
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.llx)*sz->value.lval/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.lly)*sz->value.lval/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.urx)*sz->value.lval/1000));
	add_next_index_long(return_value, (int) ceil(((double) str_bbox.ury)*sz->value.lval/1000));
#else 
    php3_error(E_WARNING, "ImagePsBBox: No T1lib support in this PHP build");
    RETURN_FALSE;
#endif
}
/* }}} */


#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
