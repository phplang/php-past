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
   | Authors: Rasmus Lerdorf                                              |
   +----------------------------------------------------------------------+
 */
/* $Id: image.c,v 1.33 1998/05/21 23:57:30 zeev Exp $ */
/* 
 * Based on Daniel Schmitt's imageinfo.c which carried the following
 * Copyright notice.
 */

/*
 * imageinfo.c
 *
 * Simple routines to extract image width/height data from GIF/JPEG files.
 *
 * Copyright (c) 1997 Daniel Schmitt, opal online publishing, Bonn, Germany.
 *
 * Includes code snippets from rdjpgcom.c, 
 * Copyright (c) 1994-1995 Thomas G. Lane
 * from release 6a of the Independent JPEG Group's software.
 *
 * Legal status: see GNU General Public License version 2 or later.
 *
 */

#include "php.h"
#include <stdio.h>
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include "internal_functions.h"
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "image.h"

/* file type markers */
const char php3_sig_gif[3] =
{'G', 'I', 'F'};
const char php3_sig_jpg[3] =
{(char) 0xff, (char) 0xd8, (char) 0xff};
const char php3_sig_png[8] =
{(char) 0x89, (char) 0x50, (char) 0x4e,
 (char) 0x47, (char) 0x0d, (char) 0x0a,
 (char) 0x1a, (char) 0x0a};

/* return info as a struct, to make expansion easier */
struct gfxinfo {
	unsigned int width;
	unsigned int height;
};

/* routine to handle GIF files. If only everything were that easy... ;} */
static struct gfxinfo *php3_handle_gif (int infile)
{
	struct gfxinfo *result = NULL;
	unsigned char a[2];

	result = (struct gfxinfo *) emalloc(sizeof(struct gfxinfo));
	lseek(infile, 6L, 0);
	read(infile, a, 2);
	result->width = (unsigned short)a[0] | (((unsigned short)a[1])<<8);
	read(infile, a, 2);
	result->height = (unsigned short)a[0] | (((unsigned short)a[1])<<8);
	return result;
}

static unsigned long php3_read4(int infile)
{
	unsigned char c1, c2, c3, c4;

	/* just return 0 if we hit the end-of-file */
	if (!read(infile, &c1, 1)) return 0;
	if (!read(infile, &c2, 1)) return 0;
	if (!read(infile, &c3, 1)) return 0;
	if (!read(infile, &c4, 1)) return 0;
	return (((unsigned long) c1) << 24) + (((unsigned long) c2) << 16) + (((unsigned long) c3) << 8) + ((unsigned long) c4);
}

/* routine to handle PNG files. - even easier */
static struct gfxinfo *php3_handle_png(int infile)
{
	struct gfxinfo *result = NULL;
	unsigned long in_width, in_height;

	result = (struct gfxinfo *) emalloc(sizeof(struct gfxinfo));
	lseek(infile, 16L, 0);
	in_width = php3_read4(infile);
	in_height = php3_read4(infile);
	result->width = (unsigned int) in_width;
	result->height = (unsigned int) in_height;
	return result;
}

/* routines to handle JPEG data */

/* some defines for the different JPEG block types */
#define M_SOF0  0xC0			/* Start Of Frame N */
#define M_SOF1  0xC1			/* N indicates which compression process */
#define M_SOF2  0xC2			/* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5			/* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_EOI   0xD9			/* End Of Image (end of datastream) */
#define M_SOS   0xDA			/* Start Of Scan (begins compressed data) */

static unsigned short php3_read2(int infile)
{
	unsigned char c1, c2;

	/* just return 0 if we hit the end-of-file */
	if (!read(infile, &c1, 1)) return 0;
	if (!read(infile, &c2, 1)) return 0;
	return (((unsigned short) c1) << 8) + ((unsigned short) c2);
}

static unsigned int php3_next_marker(int infile)
	 /* get next marker byte from file */
{
	unsigned char c;

	/* skip unimportant stuff */
	read(infile, &c, 1);
	while (c != (unsigned char) 0xff)
		if (read(infile, &c, 1) != 1)
			return M_EOI;		/* we hit EOF */

	/* get marker byte, swallowing possible padding */
	do {
		if (read(infile, &c, 1) != 1)
			return M_EOI;		/* we hit EOF */
	} while (c == (unsigned char) 0xff);

	return (unsigned int) c;
}

static void php3_skip_variable(int infile)
	 /* skip over a variable-length block; assumes proper length marker */
{
	unsigned short length;

	length = php3_read2(infile);
	length -= 2;				/* length includes itself */
	lseek(infile, (long) length, 1);	/* skip the header */
}

static struct gfxinfo *php3_handle_jpeg(int infile)
	 /* main loop to parse JPEG structure */
{
	struct gfxinfo *result = NULL;
	unsigned int marker;
	unsigned short in_width, in_height;

	lseek(infile, 2L, 0);		/* position file pointer on first marker */

	for (;;) {
		marker = php3_next_marker(infile);
		switch (marker) {
			case M_SOF0:
			case M_SOF1:
			case M_SOF2:
			case M_SOF3:
			case M_SOF5:
			case M_SOF6:
			case M_SOF7:
			case M_SOF9:
			case M_SOF10:
			case M_SOF11:
			case M_SOF13:
			case M_SOF14:
			case M_SOF15:
				/* handle SOFn block */
				lseek(infile, 3L, 1);	/* skip length and precision bytes */
				in_height = php3_read2(infile);
				in_width = php3_read2(infile);
				/* fill a gfxinfo struct to return the data */
				result = (struct gfxinfo *) emalloc(sizeof(struct gfxinfo));
				result->width = (unsigned int) in_width;
				result->height = (unsigned int) in_height;
				return result;	/* this is all we were looking for */
				break;
			case M_SOS:
			case M_EOI:
				return NULL;	/* we're about to hit image data, or are at EOF. Error. */
				break;
			default:
				php3_skip_variable(infile);		/* anything else isn't interesting */
				break;
		}
	}
}

/* main function */
void php3_getimagesize(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *arg1;
	int filehandle, itype = 0;
	char filetype[3];
	char pngtype[8];
	char temp[64];
	struct gfxinfo *result = NULL;

	if (getParameters(ht, 1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);

	if ((filehandle = open(arg1->value.str.val, O_RDONLY)) < 0) {
		php3_error(E_WARNING, "Unable to open %s", arg1->value.str.val);
		return;
	}
	read(filehandle, filetype, sizeof(filetype));
	if (!memcmp(filetype, php3_sig_gif, 3)) {
		result = php3_handle_gif (filehandle);
		itype = 1;
	} else if (!memcmp(filetype, php3_sig_jpg, 3)) {
		result = php3_handle_jpeg(filehandle);
		itype = 2;
	} else if (!memcmp(filetype, php3_sig_png, 3)) {
		lseek(filehandle, 0L, 0);
		read(filehandle, pngtype, sizeof(pngtype));
		if (!memcmp(pngtype, php3_sig_png, 8)) {
			result = php3_handle_png(filehandle);
			itype = 3;
		} else {
			php3_error(E_WARNING, "PNG file corrupted by ASCII conversion");
		}
	}
	close(filehandle);
	if (result) {
		if (array_init(return_value) == FAILURE) {
			php3_error(E_ERROR, "Unable to initialize array");
			if (result) efree(result);
			return;
		}
		add_index_long(return_value, 0, result->width);
		add_index_long(return_value, 1, result->height);
		add_index_long(return_value, 2, itype);
		sprintf(temp, "width=\"%d\" height=\"%d\"", result->width, result->height); /* safe */
		add_index_string(return_value, 3, temp, 1);
		efree(result);
	}
}
