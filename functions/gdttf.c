/* gd interface to freetype library         */
/*                                          */
/* John Ellson   ellson@lucent.com          */

#if WIN32|WINNT
#include "config.w32.h"
#else
#include "config.h"
#endif
#if HAVE_LIBTTF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <gd.h>
#include "gdcache.h"
#include <freetype.h>


/* number of fonts cached before least recently used is replaced */
#define FONTCACHESIZE 6

/* number of character glyphs cached per font before 
	least-recently-used is replaced */
#define GLYPHCACHESIZE 120

/* number of bitmaps cached per glyph before 
	least-recently-used is replaced */
#define BITMAPCACHESIZE 8

/* number of antialias color lookups cached */
#define TWEENCOLORCACHESIZE 32

/* ptsize below which anti-aliasing is ineffective */
#define MINANTIALIASPTSIZE 0

/* display resolution - (Not really.  This has to be 72 or hinting is wrong */ 
#define RESOLUTION 72

/* Number of colors used for anti-aliasing */
#define NUMCOLORS 4

/* Line separation as a factor of font height.  
	No space between if LINESPACE = 1.00 */
#define LINESPACE 1.05

#ifndef TRUE
#define FALSE 0
#define TRUE !FALSE
#endif

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

typedef struct {
	char				*fontname;
	double				ptsize;
	double				angle;
	double				sin_a, cos_a;
	TT_Engine           *engine;
	TT_Face				face;
	TT_Face_Properties  properties;
	TT_Instance			instance;
	TT_CharMap			char_map;
	TT_Matrix			matrix;
	TT_Instance_Metrics	imetrics;
	gdCache_head_t		*glyphCache;
} font_t;

typedef struct {
	char				*fontname;	/* key */
	double				ptsize;		/* key */
	double				angle;		/* key */
	TT_Engine			*engine;
} fontkey_t;

typedef struct {
	int					character;
	TT_Glyph			glyph;
	TT_Glyph_Metrics	metrics;
	TT_Outline			outline;
	TT_Pos				oldx, oldy;
	TT_Raster_Map		Bit;
	int					gray_render;
	int					xmin, xmax, ymin, ymax;
	gdCache_head_t		*bitmapCache;
} glyph_t;

typedef struct {
	int					character;	/* key */
	font_t				*font;
} glyphkey_t;

typedef struct {
	int					xoffset, yoffset;
	char				*bitmap;
} bitmap_t;

typedef struct {
	int					xoffset, yoffset;	/* key */
	glyph_t				*glyph;
} bitmapkey_t;

typedef struct { 
    unsigned char       pixel;
    unsigned char       bgcolor;
    unsigned char       fgcolor;
    unsigned char       tweencolor;
} tweencolor_t;

typedef struct {
    unsigned char       pixel;      /* key */
    unsigned char       bgcolor;    /* key */
    unsigned char       fgcolor;    /* key */
    gdImagePtr          im;
} tweencolorkey_t;  

/* forward declarations so that glyphCache can be initialized by font code */
static int glyphTest ( void *element, void *key );
static void *glyphFetch ( char **error, void *key );
static void glyphRelease( void *element );

/* forward declarations so that bitmapCache can be initialized by glyph code */
static int bitmapTest ( void *element, void *key );
static void *bitmapFetch ( char **error, void *key );
static void bitmapRelease( void *element );

/********************************************************************/
/* gdImageColorResolve is a replacement for the old fragment:       */
/*                                                                  */
/*      if ((color=gdImageColorExact(im,R,G,B)) < 0)                */
/*        if ((color=gdImageColorAllocate(im,R,G,B)) < 0)           */
/*          color=gdImageColorClosest(im,R,G,B);                    */
/*                                                                  */
/* in a single function                                             */

static int
gdImageColorResolve(gdImagePtr im, int r, int g, int b)
{
	int c; 
	int ct = -1;
	int op = -1;
	long rd, gd, bd, dist;
	long mindist = 3*255*255;  /* init to max poss dist */

	for (c = 0; c < im->colorsTotal; c++) {
		if (im->open[c]) {
			op = c;				/* Save open slot */
			continue;			/* Color not in use */
		}
		rd = (long)(im->red  [c] - r);
		gd = (long)(im->green[c] - g);
		bd = (long)(im->blue [c] - b);
		dist = rd * rd + gd * gd + bd * bd;
		if (dist < mindist) {
			if (dist == 0) {
				return c;		/* Return exact match color */
			}
			mindist = dist;
			ct = c;
		}
	}       
	/* no exact match.  We now know closest, but first try to allocate exact */
	if (op == -1) {
		op = im->colorsTotal;
		if (op == gdMaxColors) {    /* No room for more colors */
			return ct;  		/* Return closest available color */
		}
		im->colorsTotal++;
	}
	im->red  [op] = r;
	im->green[op] = g;
	im->blue [op] = b;
	im->open [op] = 0;
	return op;					/* Return newly allocated color */
}

/********************************************************************/
/* font cache functions                                             */

static int
fontTest ( void *element, void *key )
{
	font_t *a=(font_t *)element;
	fontkey_t *b=(fontkey_t *)key;

	return ( strcmp(a->fontname, b->fontname) == 0
			&&	a->ptsize == b->ptsize
			&&  a->angle == b->angle);
}

static void *
fontFetch ( char **error, void *key )
{
	TT_Error		err;
	font_t			*a;
	fontkey_t		*b=(fontkey_t *)key;
	int				i, n;
	short			platform, encoding;

	a = (font_t *)malloc(sizeof(font_t));
	a->fontname = (char *)malloc(strlen(b->fontname) + 1);
	strcpy(a->fontname,b->fontname);
	a->ptsize = b->ptsize;
	a->angle = b->angle;
	a->sin_a = sin(a->angle);
	a->cos_a = cos(a->angle);
	a->engine = b->engine;
	if ((err = TT_Open_Face(*b->engine, a->fontname, &a->face))) {
		if (err == TT_Err_Could_Not_Open_File) {
			*error = "Could not find/open font";
		}
		else {
			*error = "Could not read font";
		}
		return NULL;
	}
	/* get face properties and allocate preload arrays */
	TT_Get_Face_Properties(a->face, &a->properties);

	/* create instance */
	if (TT_New_Instance(a->face, &a->instance)) {
		*error = "Could not create face instance";
		return NULL;
	}
	
	if (TT_Set_Instance_Resolutions(a->instance, RESOLUTION, RESOLUTION)) {
		*error = "Could not set device resolutions";
		return NULL;
	}

	if (TT_Set_Instance_CharSize(a->instance, (long)(a->ptsize*64))) {
		*error = "Could not set character size";
		return NULL;
	}

	/* Set flag if font is rotated */
	if (a->angle == 0) {
		TT_Set_Instance_Transform_Flags(a->instance, FALSE, FALSE);
	}
	else {
		TT_Set_Instance_Transform_Flags(a->instance, TRUE, FALSE);
	}

	TT_Get_Instance_Metrics(a->instance, &a->imetrics);
	
	/* First, look for a Unicode charmap */
	n = TT_Get_CharMap_Count(a->face);

	for (i = 0; i < n; i++) {
		TT_Get_CharMap_ID(a->face, i, &platform, &encoding);
		if ((platform == 3 && encoding == 1)  ||
	  	(platform == 0 && encoding == 0)) {
			TT_Get_CharMap(a->face, i, &a->char_map);
			i = n+1;
		}
	}

	if (i == n) {
		*error = "Sorry, but this font doesn't contain any Unicode mapping table";
		return NULL;
	}

	a->matrix.xx = (TT_Fixed) (a->cos_a * (1<<16));
	a->matrix.yx = (TT_Fixed) (a->sin_a * (1<<16));
	a->matrix.xy = - a->matrix.yx;
	a->matrix.yy = a->matrix.xx;

	a->glyphCache = gdCacheCreate( GLYPHCACHESIZE,
					glyphTest, glyphFetch, glyphRelease);

	return (void *)a;
}

static void
fontRelease( void *element )
{
	font_t *a=(font_t *)element;

	gdCacheDelete(a->glyphCache);
	TT_Done_Instance(a->instance);
	TT_Close_Face(a->face);
	free(a->fontname);
	free( (char *)element );
}

/********************************************************************/
/* glyph cache functions                                            */

static int
glyphTest ( void *element, void *key )
{
	glyph_t *a=(glyph_t *)element;
	glyphkey_t *b=(glyphkey_t *)key;

	return (a->character == b->character);
}

static void *
glyphFetch ( char **error, void *key )
{
	glyph_t				*a;
	glyphkey_t			*b=(glyphkey_t *)key;
	short				glyph_code;
	int					flags;
	int					crect[8], xmin, xmax, ymin, ymax;
	double				cos_a, sin_a;

	a = (glyph_t *)malloc(sizeof(glyph_t));
	a->character = b->character;
	a->gray_render = (b->font->ptsize < MINANTIALIASPTSIZE)?FALSE:TRUE;
	a->oldx = a->oldy = 0;

	/* create glyph container */
	if ((TT_New_Glyph(b->font->face, &a->glyph))) {
		*error = "Could not create glyph container";
		return NULL;
	}

	flags = TTLOAD_SCALE_GLYPH;
	if (b->font->angle == 0.0) {
		flags |= TTLOAD_HINT_GLYPH;
	}
	glyph_code = TT_Char_Index(b->font->char_map, (unsigned short)a->character);
	if ((TT_Load_Glyph(b->font->instance, a->glyph, glyph_code, flags))) {
		*error = "TT_Load_Glyph problem";
		return NULL;
	}

	TT_Get_Glyph_Metrics(a->glyph, &a->metrics);
	if (b->font->angle != 0.0) {
		TT_Get_Glyph_Outline(a->glyph, &a->outline);
		TT_Transform_Outline(&a->outline, &b->font->matrix);
	}

	/* calculate bitmap size */
	xmin = a->metrics.bbox.xMin -64;
	ymin = a->metrics.bbox.yMin -64;
	xmax = a->metrics.bbox.xMax +64;
	ymax = a->metrics.bbox.yMax +64;

	cos_a = b->font->cos_a;
	sin_a = b->font->sin_a;
	crect[0] = (int)(xmin * cos_a - ymin * sin_a);
	crect[1] = (int)(xmin * sin_a + ymin * cos_a);
	crect[2] = (int)(xmax * cos_a - ymin * sin_a);
	crect[3] = (int)(xmax * sin_a + ymin * cos_a);
	crect[4] = (int)(xmax * cos_a - ymax * sin_a);
	crect[5] = (int)(xmax * sin_a + ymax * cos_a);
	crect[6] = (int)(xmin * cos_a - ymax * sin_a);
	crect[7] = (int)(xmin * sin_a + ymax * cos_a);
	a->xmin = MIN(MIN(crect[0],crect[2]),MIN(crect[4],crect[6]));
	a->xmax = MAX(MAX(crect[0],crect[2]),MAX(crect[4],crect[6]));
	a->ymin = MIN(MIN(crect[1],crect[3]),MIN(crect[5],crect[7]));
	a->ymax = MAX(MAX(crect[1],crect[3]),MAX(crect[5],crect[7]));

	/* allocate bitmap large enough for character */
	a->Bit.rows = (a->ymax - a->ymin + 32 + 64) / 64;
	a->Bit.width = (a->xmax - a->xmin + 32 + 64) / 64;
	a->Bit.flow = TT_Flow_Up;
	if (a->gray_render) {
		a->Bit.cols = a->Bit.width;               /* 1 byte per pixel */
	}
	else {
		a->Bit.cols = (a->Bit.width + 7) / 8;     /* 1 bit per pixel */
	}
	a->Bit.cols = (a->Bit.cols + 3) & ~3;         /* pad to 32 bits */
	a->Bit.size = a->Bit.rows * a->Bit.cols;      /* # of bytes in buffer */
	a->Bit.bitmap = NULL;

	a->bitmapCache = gdCacheCreate( BITMAPCACHESIZE,
					bitmapTest, bitmapFetch, bitmapRelease);

	return (void *)a;
}

static void
glyphRelease( void *element )
{
	glyph_t *a=(glyph_t *)element;

	gdCacheDelete(a->bitmapCache);
	TT_Done_Glyph( a->glyph );
	free( (char *)element );
}

/********************************************************************/
/* bitmap cache functions                                            */

static int
bitmapTest ( void *element, void *key )
{
	bitmap_t *a=(bitmap_t *)element;
	bitmapkey_t *b=(bitmapkey_t *)key;

	if (a->xoffset == b->xoffset && a->yoffset == b->yoffset) {
		b->glyph->Bit.bitmap = a->bitmap;
		return TRUE;
	}
	return FALSE;
}

static void *
bitmapFetch ( char **error, void *key )
{
	bitmap_t			*a;
	bitmapkey_t			*b=(bitmapkey_t *)key;

	a = (bitmap_t *)malloc(sizeof(bitmap_t));
	a->xoffset = b->xoffset;
	a->yoffset = b->yoffset;

	b->glyph->Bit.bitmap = a->bitmap = (char *)malloc(b->glyph->Bit.size);
	memset(a->bitmap, 0, b->glyph->Bit.size);
	/* render glyph */
	if (b->glyph->gray_render) {
		TT_Get_Glyph_Pixmap(b->glyph->glyph, &b->glyph->Bit,
			a->xoffset, a->yoffset);
	}
	else {
		TT_Get_Glyph_Bitmap(b->glyph->glyph, &b->glyph->Bit,
			a->xoffset, a->yoffset);
	}
	return (void *)a;
}

static void
bitmapRelease( void *element )
{
	bitmap_t *a=(bitmap_t *)element;

	free( a->bitmap );
	free( (char *)element );
}

/********************************************************************/
/* tweencolor cache functions                                            */

static int
tweenColorTest (void *element, void *key)
{ 
    tweencolor_t *a=(tweencolor_t *)element;
    tweencolorkey_t *b=(tweencolorkey_t *)key;
    
    return (a->pixel == b->pixel    
         && a->bgcolor == b->bgcolor
         && a->fgcolor == b->fgcolor);
} 

static void *
tweenColorFetch (char **error, void *key)
{
    tweencolor_t *a;
    tweencolorkey_t *b=(tweencolorkey_t *)key;
	int pixel, npixel, bg, fg;
	gdImagePtr im;
   
    a = (tweencolor_t *)malloc(sizeof(tweencolor_t));
	pixel = a->pixel = b->pixel;
	bg = a->bgcolor = b->bgcolor;
	fg = a->fgcolor = b->fgcolor;
	im = b->im;

#if 0
	if (bg == im->transparent) {
		/* anti-aliasing looks weird on transparent backgrounds */
		a->tweencolor = fg;
	} else {
#endif
		npixel = NUMCOLORS - pixel;
   
		a->tweencolor = gdImageColorResolve(im,
			(pixel * im->red  [fg] + npixel * im->red  [bg]) / NUMCOLORS,
			(pixel * im->green[fg] + npixel * im->green[bg]) / NUMCOLORS,
			(pixel * im->blue [fg] + npixel * im->blue [bg]) / NUMCOLORS);
#if 0
	}
#endif
    *error = NULL;
    return (void *)a;
}   
        
static void
tweenColorRelease(void *element)
{   
    free((char *)element);
}   

/********************************************************************/
/* gdttfchar -  render one character onto a gd image                */

char *
gdttfchar(gdImage *im, int fg, font_t *font,
	int x, int y,					/* string start pos in pixels */
	TT_F26Dot6 x1,	TT_F26Dot6 y1,	/* char start offset (*64) from x,y */
	TT_F26Dot6 *advance,
	TT_BBox **bbox, 
	int ch)
{
    int pc;
	int row, col;
	int x2, y2;     /* char start pos in pixels */ 
	int x3, y3;     /* current pixel pos */
	unsigned char *pixel;

    glyph_t *glyph;
    glyphkey_t glyphkey;
    bitmapkey_t bitmapkey;
	tweencolor_t *tweencolor;
	tweencolorkey_t tweencolorkey;

	/****** set up tweenColorCache on first call ************/
	static gdCache_head_t   *tweenColorCache;

/* Disabling this caching stuff, it does not seem to work.
   We could perhaps try solving it with persistent fonts instead?
    - Stig
*/
/*	if (! tweenColorCache)*/
		tweenColorCache = gdCacheCreate(TWEENCOLORCACHESIZE,
			tweenColorTest, tweenColorFetch, tweenColorRelease);
	/**************/

	glyphkey.character = ch;
    glyphkey.font = font;
    glyph = (glyph_t *)gdCacheGet(font->glyphCache, &glyphkey);
    if (! glyph)
		return font->glyphCache->error;

	*bbox = &glyph->metrics.bbox;
	*advance = glyph->metrics.advance;

	/* if invalid color requested then assume user just wants brect */
	if (fg < 0 || fg > 255 || !im)
		return (char *)NULL;

	/* render (via cache) a bitmap for the current fractional offset */
	bitmapkey.xoffset = ((x1+32) & 63) - 32 - (glyph->xmin & -64);
	bitmapkey.yoffset = ((y1+32) & 63) - 32 - (glyph->ymin & -64);
	bitmapkey.glyph = glyph;
	gdCacheGet(glyph->bitmapCache, &bitmapkey);

	/* copy to gif, mapping colors */
	x2 = x + (glyph->xmin + ((x1+32) & -64)) / 64;
	y2 = y - (glyph->ymin + ((y1+32) & -64)) / 64;
	tweencolorkey.fgcolor = fg;
	tweencolorkey.im = im;
	for (row = 0; row < glyph->Bit.rows; row++) {
		if (glyph->gray_render)
			pc = row * glyph->Bit.cols;
		else
			pc = row * glyph->Bit.cols * 8;
		y3 = y2 - row;
		if (y3 >= im->sy || y3 < 0) continue;
		for (col = 0; col < glyph->Bit.width; col++, pc++) {
			if (glyph->gray_render) {
				tweencolorkey.pixel = 
					*((unsigned char *)(glyph->Bit.bitmap) + pc);
			} else {
				tweencolorkey.pixel = 
					(((*((unsigned char *)(glyph->Bit.bitmap) + pc/8))
						<<(pc%8))&128)?4:0;
			}
			/* if not background */
			if (tweencolorkey.pixel > 0) {
				x3 = x2 + col;
				if (x3 >= im->sx || x3 < 0) continue;
#if HAVE_LIBGD13
				pixel = &im->pixels[y3][x3];
#else
				pixel = &im->pixels[x3][y3];
#endif
				tweencolorkey.bgcolor = *pixel;
				tweencolor = (tweencolor_t *)gdCacheGet(
					tweenColorCache, &tweencolorkey);
				*pixel = tweencolor->tweencolor;
			}
		}
	}
	return (char *)NULL;
}

/********************************************************************/
/* gdttf -  render a string onto a gd image							*/

char *
gdttf(gdImage *im, int *brect, int fg, char *fontname,
	double ptsize, double angle, int x, int y, int *string, int len)
{
	TT_F26Dot6 ur_x=0, ur_y=0, ll_x=0, ll_y=0;
	TT_F26Dot6 advance_x, advance_y, advance, x1, y1;
	TT_BBox *bbox;
	double sin_a, cos_a;
    int i, ch;
	font_t *font;
	fontkey_t fontkey;
	char *error;

	/****** initialize font engine on first call ************/
    static gdCache_head_t	*fontCache;
	static TT_Engine 	engine;

	if (! fontCache) {
		if (TT_Init_FreeType(&engine)) {
			return "Failure to initialize font engine";
		}
		fontCache = gdCacheCreate( FONTCACHESIZE,
			fontTest, fontFetch, fontRelease);
	}
	/**************/

	/* get the font (via font cache) */
	fontkey.fontname = fontname;
	fontkey.ptsize = ptsize;
	fontkey.angle = angle;
	fontkey.engine = &engine;
	font = (font_t *)gdCacheGet(fontCache, &fontkey);
	if (! font) {
		return fontCache->error;
	}
	sin_a = font->sin_a;
	cos_a = font->cos_a;

	advance_x = advance_y = 0;
	for (i=0; i < len; i++) {	  
		ch = string[i];

		/* cariage returns */
		if (ch == '\r') {
			advance_x = 0;
			continue;
		}
		/* newlines */
		if (ch == '\n') {
			advance_y -= (long)((font->imetrics.y_ppem * LINESPACE) * 64);
			continue;
		}

		x1 = (long)(advance_x * cos_a - advance_y * sin_a);
		y1 = (long)(advance_x * sin_a + advance_y * cos_a);

		if ((error=gdttfchar(im, fg, font, x, y, x1, y1, &advance, &bbox, ch)))
			return error;

		if (! i) { /* if first character, init BB corner values */
			ll_x = bbox->xMin;
			ll_y = bbox->yMin;
			ur_x = bbox->xMax;
			ur_y = bbox->yMax;
		}
		else {
			if (! advance_x) ll_x = MIN(bbox->xMin, ll_x);
			ll_y = MIN(advance_y + bbox->yMin, ll_y);
			ur_x = MAX(advance_x + bbox->xMax, ur_x);
			if (! advance_y) ur_y = MAX(bbox->yMax, ur_y);
		}
		advance_x += advance;
	}

	/* rotate bounding rectangle */
	brect[0] = (int)(ll_x * cos_a - ll_y * sin_a);
	brect[1] = (int)(ll_x * sin_a + ll_y * cos_a);
	brect[2] = (int)(ur_x * cos_a - ll_y * sin_a);
	brect[3] = (int)(ur_x * sin_a + ll_y * cos_a);
	brect[4] = (int)(ur_x * cos_a - ur_y * sin_a);
	brect[5] = (int)(ur_x * sin_a + ur_y * cos_a);
	brect[6] = (int)(ll_x * cos_a - ur_y * sin_a);
	brect[7] = (int)(ll_x * sin_a + ur_y * cos_a);

	/* scale, round and offset brect */
	i = 0;
	while (i<8) {
		brect[i] = x + (brect[i] + 32) / 64;
		i++;
		brect[i] = y - (brect[i] + 32) / 64;
		i++;
	}

    return (char *)NULL;
}
   
#endif /* HAVE_LIBTTF */
