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
   | Authors: Thies C. Arntzen (thies@digicol.de)                         |
   |          Rasmus Lerdorf   (rasmus@php.net)                           |
   +----------------------------------------------------------------------+
 */

/* $Id: magick.c,v 1.11 2000/01/01 04:31:16 sas Exp $ */

/* TODO list:
 *
 * - make this thing work!
 * - redirect stdout to phpwrite for $img->save("gif:-");
 * - Error/Warning Handlers are dodgy....
 */

/* {{{ includes & stuff */

#if defined(COMPILE_DL)
# include "dl/phpdl.h"
#endif

#include "php.h"
#include "internal_functions.h"

#if HAVE_LIBMAGICK

#if !(WIN32|WINNT)
# include "build-defs.h"
#endif

#include "php3_list.h"
#include "head.h"
#include "php3_magick.h"

/* XXX i would love to see that in php3_magick.h - but i'm not able to compile the internal_functions then!!! */

#include "magick.h"


#define MAGICK_IMAGE_TYPE(x) ((x)==MAGICK_GLOBAL(php3_magick_module).le_image)

typedef struct {
	int id;
	ImageInfo ImageInfo;
} magick_imageinfo;

typedef struct {
	int id;
	magick_imageinfo *info;
	Image *Image;
} magick_image;

typedef struct {
    long debug_mode;
    int le_imageinfo;
    int le_image; 
} magick_module;

/* }}} */

#define MAGICK_GLOBAL(a) a
#define MAGICK_TLS_VARS
magick_module php3_magick_module;

/* {{{ dynamically loadable module stuff */

#if COMPILE_DL
DLEXPORT php3_module_entry *get_module() { return &magick_module_entry; };
#endif /* COMPILE_DL */

/* }}} */
/* {{{ startup/shutdown/info/internal function prototypes */

int php3_minit_magick(INIT_FUNC_ARGS);
int php3_rinit_magick(INIT_FUNC_ARGS);
int php3_mshutdown_magick(void);
int php3_rshutdown_magick(void);
void php3_info_magick(void);
static magick_image *magick_get_image(int, const char *, HashTable *);
static void magick_debug(int level,const char *format,...);
static void magick_free_imageinfo(magick_imageinfo *imageinfo);
static void magick_free_image(magick_image *image);
static void magick_errorhandler(const unsigned int error,const char *message,const char *qualifier);
static void magick_warninghandler(const unsigned int error,const char *message,const char *qualifier);
/* }}} */
/* {{{ extension function prototypes */

void php3_ReadMagick(INTERNAL_FUNCTION_PARAMETERS);
void php3_SaveMagick(INTERNAL_FUNCTION_PARAMETERS);
void php3_ScaleMagick(INTERNAL_FUNCTION_PARAMETERS);
void php3_FreeMagick(INTERNAL_FUNCTION_PARAMETERS);
void php3_MagickDebug(INTERNAL_FUNCTION_PARAMETERS);

/* }}} */
/* {{{ extension definition structures */

#define D_IMAGEINFO 1<<1
#define D_IMAGE     1<<2
#define D_PROCESS   1<<3

function_entry magick_functions[] = {
	{"readmagick",			php3_ReadMagick,			NULL},
	{"savemagick",			php3_SaveMagick,			NULL},
	{"freemagick",			php3_FreeMagick,			NULL},
	{"scalemagick",			php3_ScaleMagick,			NULL},
	{"magickdebug",			php3_MagickDebug,			NULL},
	{NULL, NULL, NULL}
};

php3_module_entry magick_module_entry = {
	"Magick",              /* extension name */
	magick_functions,      /* extension function list */ 
	php3_minit_magick,     /* extension-wide startup function */
	php3_mshutdown_magick, /* extension-wide shutdown function */
	php3_rinit_magick,     /* per-request startup function */
	php3_rshutdown_magick, /* per-request shutdown function */
	php3_info_magick,      /* information function */
	STANDARD_MODULE_PROPERTIES
};

/* }}} */
/* {{{ startup, shutdown and info functions */

int php3_minit_magick(INIT_FUNC_ARGS)
{
	MAGICK_GLOBAL(php3_magick_module).debug_mode = 0;
	
	MAGICK_GLOBAL(php3_magick_module).le_imageinfo =
		register_list_destructors(magick_free_imageinfo, NULL);

	MAGICK_GLOBAL(php3_magick_module).le_image =
		register_list_destructors(magick_free_image, NULL);


	REGISTER_LONG_CONSTANT("DEBUG_IMAGEINFO",D_IMAGEINFO, CONST_CS | CONST_PERSISTENT);

	SetWarningHandler(magick_warninghandler);
	SetErrorHandler(magick_errorhandler);

	return SUCCESS;
}

int php3_rinit_magick(INIT_FUNC_ARGS)
{
	MAGICK_TLS_VARS;
	
#if NEEDED
	MAGICK_GLOBAL(php3_magick_module).num_links = 
#endif

    return SUCCESS;
}

int php3_mshutdown_magick()
{
	return SUCCESS;
}

int php3_rshutdown_magick()
{
    return SUCCESS;
}

void php3_info_magick()
{
#if !(WIN32|WINNT)
	PUTS(MagickVersion);
	PUTS("<BR>\n");
	PUTS(MagickCopyright);
	PUTS("<br>RCS Version: $Id: magick.c,v 1.11 2000/01/01 04:31:16 sas Exp $<br>\n");
	php3_printf("<b>Compile Options</b><br><tt>IMAGICK_INCLUDE=%s<br>\n"
	"IMAGICK_LFLAGS=%s<br>\n"
	"IMAGICK_LIBS=%s<br></tt>\n",
	PHP_IMAGICK_INCLUDE,
	PHP_IMAGICK_LFLAGS,
	PHP_IMAGICK_LIBS);
#endif
}
/* }}} */
/************************* INTERNAL FUNCTIONS *************************/
/* {{{ magick_debug() */

static void magick_debug(int level,const char *format,...)
{
	MAGICK_TLS_VARS;

    if (MAGICK_GLOBAL(php3_magick_module).debug_mode & level) {
		char buffer[1024];
		va_list args;

		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer)-1, format, args);
		va_end(args);
		buffer[sizeof(buffer)-1] = '\0';
		if (php3_header()) {
			php3_printf("ImageMagick: %s<br>\n", buffer);
		}
	}
}

/* }}} */
/* {{{ magick_free_imageinfo() */

static void
magick_free_imageinfo(magick_imageinfo *imageinfo)
{
	MAGICK_TLS_VARS;

	magick_debug(D_IMAGEINFO,"Free ImageInfo %d -> %x",imageinfo->id,imageinfo);

	if (imageinfo->ImageInfo.size) {
		efree(imageinfo->ImageInfo.size);
	}
		
	efree(imageinfo);
}

/* }}} */
/* {{{ magick_free_image() */

static void
magick_free_image(magick_image *image)
{
	MAGICK_TLS_VARS;

	DestroyImage(image->Image);

	magick_debug(D_IMAGE,"Free Image %d -> %x",image->id,image->Image);

	efree(image);
}

/* }}} */
/* {{{ magick_get_image() */

static magick_image *
magick_get_image(int image_id, const char *func, HashTable *list)
{
	int type;
	magick_image *image;
	MAGICK_TLS_VARS;

	image = (magick_image *)php3_list_find(image_id, &type);
	if (!image || !MAGICK_IMAGE_TYPE(type)) {
		php3_error(E_WARNING, "%s: invalid image %d", func, image_id);
		return (magick_image *)NULL;
	}
	return image;
}

/* }}} */
/* {{{ magick_errorhandler() */
static void 
magick_errorhandler(const unsigned int error,const char *message,const char *qualifier)
{
	int error_number;

	error_number=errno;
	errno=0;

	php3_error(E_WARNING,"ImageMagick Error %d: %.1024s%s%.1024s%s%s%.64s%s",error,
			   (message ? message : "ERROR"),
			   qualifier ? " (" : "",qualifier ? qualifier : "",qualifier ? ")" : "",
			   error_number ? " [" : "",error_number ? strerror(error_number) : "",
			   error_number? "]" : "");
}
/* }}} */
/* {{{ magick_warninghandler() */
static void 
magick_warninghandler(const unsigned int error,const char *message,const char *qualifier)
{
	int error_number;

	error_number=errno;
	errno=0;

	php3_error(E_WARNING,"ImageMagick Warning %d: %.1024s%s%.1024s%s%s%.64s%s",error,
			   (message ? message : "ERROR"),
			   qualifier ? " (" : "",qualifier ? qualifier : "",qualifier ? ")" : "",
			   error_number ? " [" : "",error_number ? strerror(error_number) : "",
			   error_number? "]" : "");
}
/* }}} */
/************************* EXTENSION FUNCTIONS *************************/

/* {{{ proto object ReadMagick(string filename)
   Load an Image */
void php3_ReadMagick(INTERNAL_FUNCTION_PARAMETERS)
{ 
	pval *filename, *size;
	magick_imageinfo *imageinfo;
	magick_image *image;
	char *sizestr;
	MAGICK_TLS_VARS;
	
	if (getParameters(ht, 2, &filename, &size) == SUCCESS) {
		convert_to_string(size);
		sizestr = estrdup(size->value.str.val);
	} else if (getParameters(ht, 1, &filename) == FAILURE) {
		sizestr = 0;
	} 
		
	convert_to_string(filename);

    if (_php3_check_open_basedir(filename->value.str.val)) {
		RETURN_FALSE;
	}
	
	imageinfo = emalloc(sizeof(magick_imageinfo));

	if (! imageinfo) {
		RETURN_FALSE; /* out of memory */
	}

	GetImageInfo(&imageinfo->ImageInfo);

	imageinfo->id = php3_list_insert(imageinfo, MAGICK_GLOBAL(php3_magick_module).le_imageinfo);

	magick_debug(D_IMAGEINFO,"New ImageInfo %d -> %x",imageinfo->id,imageinfo);

	strcpy(imageinfo->ImageInfo.filename,filename->value.str.val);

	imageinfo->ImageInfo.size = sizestr;

	image = emalloc(sizeof(magick_image));

	if (! image) {
		RETURN_FALSE; /* out of memory */
	}

	image->Image = ReadImage(&imageinfo->ImageInfo);
	image->info = imageinfo;

	if (! image->Image) {
		RETURN_FALSE; /* print some ImageMagick error here */
	}

	image->id = php3_list_insert(image, MAGICK_GLOBAL(php3_magick_module).le_image);

	magick_debug(D_IMAGE,"New Image %d -> %x",image->id,image->Image);
 
	object_init(return_value);
	add_property_long(return_value, "_imageinfo", imageinfo->id);
	add_property_long(return_value, "_image",     image->id);


	add_property_long(return_value,  "filesize",     image->Image->filesize);
	add_property_long(return_value,  "columns",      image->Image->columns);
	add_property_long(return_value,  "rows",         image->Image->rows);
	add_property_long(return_value,  "depth",        image->Image->depth);
	add_property_long(return_value,  "colors",       image->Image->colors);
	add_property_double(return_value,"x_resolution", image->Image->x_resolution);
	add_property_double(return_value,"y_resolution", image->Image->y_resolution);
	add_property_long(return_value,  "total_colors", image->Image->total_colors);


	add_method(return_value, "save",  php3_SaveMagick);
	add_method(return_value, "free",  php3_FreeMagick);
	add_method(return_value, "scale", php3_ScaleMagick);
}
/* }}} */

/* {{{ proto object SaveMagick(string filename)
   Save an Image */
void php3_SaveMagick(INTERNAL_FUNCTION_PARAMETERS)
{ 
	pval *filename, *id, *pimage;
	magick_image *image;
	char savename[ MAXPATHLEN ];
	MAGICK_TLS_VARS;
	
	if (getParameters(ht, 1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

    if (_php3_check_open_basedir(filename->value.str.val)) {
		RETURN_FALSE;
	}

	if (getThis(&id) == FAILURE) {
		RETURN_FALSE;
	}

	if (_php3_hash_find(id->value.ht, "_image", sizeof("_image"), (void **)&pimage) == FAILURE) {
		php3_error(E_WARNING, "unable to find my image property");
		RETURN_FALSE;
	}

	image = magick_get_image(pimage->value.lval, "SaveMagick", list);
	if (image == NULL) {
		RETURN_FALSE;
	}

	strcpy(savename,image->Image->filename);

	strcpy(image->Image->filename,filename->value.str.val);

	WriteImage(&(image->info->ImageInfo),image->Image);

	strcpy(image->Image->filename,savename);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int FreeMagick(object image)
   Free an Image */
void php3_FreeMagick(INTERNAL_FUNCTION_PARAMETERS)
{ 
	pval *id, *pimage;
	magick_image *image;
	MAGICK_TLS_VARS;
	
	if (getThis(&id) == FAILURE) {
		RETURN_FALSE;
	}

	if (_php3_hash_find(id->value.ht, "_image", sizeof("_image"), (void **)&pimage) == FAILURE) {
		php3_error(E_WARNING, "unable to find my image property");
		RETURN_FALSE;
	}

	image = magick_get_image(pimage->value.lval, "FreeMagick", list);
	if (image == NULL) {
		RETURN_FALSE;
	}

	php3_list_delete(image->info->id);
	php3_list_delete(image->id);

	/* XXX how do i free the object itself????? */

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ScaleMagick(object image)
   Scale an Image */
void php3_ScaleMagick(INTERNAL_FUNCTION_PARAMETERS)
{ 
	pval *id, *pimage, *oneval = 0, *colval, *rowval;
	magick_image *image;
	Image *newimage;
	double cols = 0, rows = 0, one = 0, curcols, currows;
	MAGICK_TLS_VARS;
	
	if (getThis(&id) == FAILURE) {
		RETURN_FALSE;
	}

	if (_php3_hash_find(id->value.ht, "_image", sizeof("_image"), (void **)&pimage) == FAILURE) {
		php3_error(E_WARNING, "unable to find my image property");
		RETURN_FALSE;
	}

	image = magick_get_image(pimage->value.lval, "ScaleMagick", list);
	if (image == NULL) {
		RETURN_FALSE;
	}

	if (getParameters(ht, 2, &colval,&rowval) == SUCCESS) {
		convert_to_long(colval);
		convert_to_long(rowval);
		cols = colval->value.lval;
		rows = rowval->value.lval;
		if ((cols == 0) && (rows == 0)) {
			php3_error(E_WARNING, "Cannot set size to 0/0");
			RETURN_FALSE;
		} else if ((cols < 0) || (rows < 0)) {
			php3_error(E_WARNING, "Cannot set size to negative values");
			RETURN_FALSE;
		}
	} else if (getParameters(ht, 1, &oneval) == SUCCESS) {
		convert_to_long(oneval);
		one = oneval->value.lval;
		if (one <= 0) {
			php3_error(E_WARNING, "Cannot set size to %d",(int) one);
			RETURN_FALSE;
		}
	} else {
		WRONG_PARAM_COUNT;
	}

	curcols = image->Image->columns;
	currows = image->Image->rows;

	if (oneval) {
		if (curcols > currows) {
			rows = currows / (curcols / one);
			cols = one;
		} else {
			cols = curcols / (currows / one);
			rows = one;
		}
	} else if (cols == 0) {
		cols = curcols * (rows / currows);
	} else if (rows == 0) {
		rows = currows * (cols / curcols);
	}

	magick_debug(D_PROCESS,"Scale %d (%d,%d) to (%d,%d)",
				 image->id,
				 image->Image->columns,image->Image->rows,
				 (int) cols, (int) rows);

	newimage = ScaleImage(image->Image,cols,rows);

	
	if (newimage== NULL) {
		RETURN_FALSE;
	}

	/* need to correct the structure members...*/ 

	DestroyImage(image->Image); /*XXX we don't want you anymore.... or do we???? */

	image->Image = newimage;

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int MagickDebug(int level)
   Crop an Image */
void php3_MagickDebug(INTERNAL_FUNCTION_PARAMETERS)
{ 
	pval *arg;
	int oldlevel;
	MAGICK_TLS_VARS;

	if (getParameters(ht, 1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(arg);
	oldlevel = MAGICK_GLOBAL(php3_magick_module).debug_mode;

	MAGICK_GLOBAL(php3_magick_module).debug_mode = arg->value.lval;

	RETURN_LONG(oldlevel);
}
/* }}} */

#endif /* HAVE_LIBMAGICK */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
