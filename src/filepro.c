/***[filepro.c]***************************************************[TAB=4]****\
*                                                                            *
* PHP/FI                                                                     *
*                                                                            *
* Copyright 1995,1996,1997 Rasmus Lerdorf                                    *
*                                                                            *
*  This program is free software; you can redistribute it and/or modify      *
*  it under the terms of the GNU General Public License as published by      *
*  the Free Software Foundation; either version 2 of the License, or         *
*  (at your option) any later version.                                       *
*                                                                            *
*  This program is distributed in the hope that it will be useful,           *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*  GNU General Public License for more details.                              *
*                                                                            *
*  You should have received a copy of the GNU General Public License         *
*  along with this program; if not, write to the Free Software               *
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
*                                                                            *
*  ------------------------------------------------------------------------  *
*                                                                            *
*  filePro 4.x support developed by Chad Robinson, chadr@brttech.com         *
*  Contact Chad Robinson at BRT Technical Services Corp. for details.        *
*  filePro is a registered trademark by Fiserv, Inc.  This file contains     *
*  no code or information that is not freely available from the filePro      *
*  web site at http://www.fileproplus.com/                                   *
*                                                                            *
\*****************************************************************************/

#include "php.h"
#include "parse.h"
#include <string.h>


#ifdef HAVE_FILEPRO
typedef struct fp_field {
	char *name;
	char *format;
	int width;
	struct fp_field *next;
} FP_FIELD;

static char *fp_database = NULL;			/* Database directory */
static signed int fp_fcount = -1;			/* Column count */
static signed int fp_keysize = -1;			/* Size of key records */
static FP_FIELD *fp_fieldlist = NULL;		/* List of fields */
#endif


/*
 * LNUMBER filePro(STRING directory)
 * 
 * Read and verify the map file.  We store the field count and field info
 * internally, which means we become unstable if you modify the table while
 * a user is using it!  We cannot lock anything since Web connections don't
 * provide the ability to later unlock what we locked.  Be smart, be safe.
 */
void filePro(void)
{
#ifdef HAVE_FILEPRO
	FILE *fp;
	Stack *st;
	char workbuf[256]; /* FIX - should really be the max filename length */
	char readbuf[256];
	int i;
	FP_FIELD *new_field, *tmp;
	
	/* FIX - we should really check and free these if they are used! */
	fp_database = NULL;
    fp_fieldlist = NULL;
	fp_fcount = -1;
    fp_keysize = -1;
	
	if (!(st = Pop())) {
		Error("filePro: expecting argument <directory>");
		Push("0", LNUMBER);
		return;
	}
	sprintf(workbuf, "%s/map", st->strval);
	if (!(fp = fopen(workbuf, "r"))) {
		Error("filePro: cannot open map: [%d] %s", errno, strerror(errno));
		Push("0", LNUMBER);
		return;
	}
	if (!fgets(readbuf, 250, fp)) {
		fclose(fp);
		Error("filePro: cannot read map: [%d] %s", errno, strerror(errno));
		Push("-1", LNUMBER);
		return;
	}
	
	/* Get the field count, assume the file is readable! */
	if (strcmp(strtok(readbuf, ":"), "map")) {
		Error("filePro: map file corrupt or encrypted");
		Push("-1", LNUMBER);
		return;
	}
	fp_keysize = atoi(strtok(NULL, ":"));
	strtok(NULL, ":");
	fp_fcount = atoi(strtok(NULL, ":"));
    
    /* Read in the fields themselves */
	for (i = 0; i < fp_fcount; i++) {
		if (!fgets(readbuf, 250, fp)) {
			fclose(fp);
			Error("filePro: cannot read map: [%d] %s", errno, strerror(errno));
			Push("-1", LNUMBER);
			return;
		}
		new_field = emalloc(0, sizeof(FP_FIELD));
		new_field->next = NULL;
		new_field->name = estrdup(0, strtok(readbuf, ":"));
		new_field->width = atoi(strtok(NULL, ":"));
		new_field->format = estrdup(0, strtok(NULL, ":"));
        
		/* Store in forward-order to save time later */
		if (!fp_fieldlist) {
			fp_fieldlist = new_field;
		} else {
			for (tmp = fp_fieldlist; tmp; tmp = tmp->next) {
				if (!tmp->next) {
					tmp->next = new_field;
					tmp = new_field;
				}
			}
		}
	}
	fclose(fp);
		
	fp_database = estrdup(0, st->strval);
	Push("1", LNUMBER);
#else
	Push("0", LNUMBER);
	Error("No filePro support");
#endif
	return;
}


/*
 * LNUMBER filePro_rowcount(void)
 * 
 * Count the used rows in the database.  filePro just marks deleted records
 * as deleted; they are not removed.  Since no counts are maintained we need
 * to go in and count records ourselves.  <sigh>
 * 
 * Errors return -1, success returns the row count.
 */
void filePro_rowcount(void)
{
#ifdef HAVE_FILEPRO
	FILE *fp;
	char workbuf[256]; /* FIX - should really be the max filename length */
	char readbuf[256];
	int recsize = 0, bytes = 0, records = 0;
	
	if (!fp_database) {
		Error("filePro: must set database directory first!\n");
		Push("-1", LNUMBER);
	    return;
	}
	
	recsize = fp_keysize + 19; /* 20 bytes system info -1 to save time later */
	
	/* Now read the records in, moving forward recsize-1 bytes each time */
	sprintf(workbuf, "%s/key", fp_database);
	if (!(fp = fopen(workbuf, "r"))) {
		Error("filePro: cannot open key: [%d] %s", errno, strerror(errno));
		Push("-1", LNUMBER);
		return;
	}
	while (!feof(fp)) {
		if (fread(readbuf, 1, 1, fp) == 1) {
			if (readbuf[0])
				records++;
			fseek(fp, recsize, SEEK_CUR);
		}
	}
    fclose(fp);
	
	sprintf(workbuf, "%d", records);
	Push(workbuf, LNUMBER);
#else
	Push("-1", LNUMBER);
	Error("No filePro support");
#endif
	return;
}


/*
 * STRING filePro_fieldname(int field_number)
 * 
 * Errors return an empty string, success returns the name of the field.
 */
void filePro_fieldname(void)
{
#ifdef HAVE_FILEPRO
	Stack *st;
	int i;
	FP_FIELD *lp;
	
	if (!fp_database) {
		Error("filePro: must set database directory first!\n");
		Push("-1", LNUMBER);
	    return;
	}
	
	if (!(st = Pop())) {
		Error("filePro: expecting argument <field number>");
		Push("", STRING);
		return;
	}
	
	for (i = 0, lp = fp_fieldlist; lp; lp = lp->next, i++) {
		if (i == st->intval) {
			Push(lp->name, STRING);
			return;
		}
	}
	Error("filePro: unable to locate field number %d.\n", st->intval);
	Push("", STRING);
#else
	Push("", STRING);
	Error("No filePro support");
#endif
	return;
}


/*
 * STRING filePro_fieldtype(int field_number)
 * 
 * Errors return an empty string, success returns the type (edit) of the field
 */
void filePro_fieldtype(void)
{
#ifdef HAVE_FILEPRO
	Stack *st;
	int i;
	FP_FIELD *lp;
	
	if (!fp_database) {
		Error("filePro: must set database directory first!\n");
		Push("-1", LNUMBER);
	    return;
	}
	
	if (!(st = Pop())) {
		Error("filePro: expecting argument <field number>");
		Push("", STRING);
		return;
	}
	
	for (i = 0, lp = fp_fieldlist; lp; lp = lp->next, i++) {
		if (i == st->intval) {
			Push(lp->format, STRING);
			return;
		}
	}
	Error("filePro: unable to locate field number %d.\n", st->intval);
	Push("", STRING);
#else
	Push("", STRING);
	Error("No filePro support");
#endif
	return;
}


/*
 * STRING filePro_fieldwidth(int field_number)
 * 
 * Errors return -1, success returns the character width of the field.
 */
void filePro_fieldwidth(void)
{
#ifdef HAVE_FILEPRO
	Stack *st;
	int i;
	FP_FIELD *lp;
	char workbuf[256];
	
	if (!fp_database) {
		Error("filePro: must set database directory first!\n");
		Push("-1", LNUMBER);
	    return;
	}
	
	if (!(st = Pop())) {
		Error("filePro: expecting argument <field number>");
		Push("", STRING);
		return;
	}
	
	for (i = 0, lp = fp_fieldlist; lp; lp = lp->next, i++) {
		if (i == st->intval) {
			sprintf(workbuf, "%d", lp->width);
			Push(workbuf, LNUMBER);
			return;
		}
	}
	Error("filePro: unable to locate field number %d.\n", st->intval);
	Push("-1", LNUMBER);
#else
	Push("-1", LNUMBER);
	Error("No filePro support");
#endif
	return;
}


/*
 * LNUMBER filePro_fieldcount(void)
 * 
 * Errors return -1, success returns the field count.
 */
void filePro_fieldcount(void)
{
#ifdef HAVE_FILEPRO
	char workbuf[256];
	
	if (!fp_database) {
		Error("filePro: must set database directory first!\n");
		Push("-1", LNUMBER);
	    return;
	}
	
	/* Read in the first line from the map file */
	sprintf(workbuf, "%d", fp_fcount);
	Push(workbuf, LNUMBER);
#else
	Push("-1", LNUMBER);
	Error("No filePro support");
#endif
	return;
}


/*
 * STRING filePro_retrieve(int row_number, int field_number)
 * 
 * Errors return and empty string, success returns the datum.
 */
void filePro_retrieve(void)
{
#ifdef HAVE_FILEPRO
    Stack *st;
    FILE *fp;
    char workbuf[256], readbuf[1024]; /* FIX - Work out better buffering! */
    long offset;
    FP_FIELD *lp;
    int i, fnum, rnum;
	
	if (!fp_database) {
		Error("filePro: must set database directory first!\n");
		Push("-1", LNUMBER);
	    return;
	}
    
    st = Pop(); /* reverse order */
    fnum = st->intval;
    st = Pop();
    rnum = st->intval;
    
    if (rnum < 0 || fnum < 0 || fnum >= fp_fcount) {
        Error("filepro: parameters out of range");
        Push("", STRING);
        return;
    }
    
    offset = (rnum + 1) * (fp_keysize + 20) + 20; /* Record location */
    for (i = 0, lp = fp_fieldlist; lp && i < fnum; lp = lp->next, i++) {
        offset += lp->width;
    }
    if (!lp) {
        Error("filePro: cannot locate field");
        Push("", STRING);
        return;
    }
    
	/* Now read the record in */
	sprintf(workbuf, "%s/key", fp_database);
	if (!(fp = fopen(workbuf, "r"))) {
		Error("filePro: cannot open key: [%d] %s", errno, strerror(errno));
		Push("", STRING);
	    fclose(fp);
		return;
	}
    fseek(fp, offset, SEEK_SET);
	if (fread(readbuf, lp->width, 1, fp) != 1) {
        Error("filePro: cannot read data: [%d] %s", errno, strerror(errno));
        Push("", STRING);
	    fclose(fp);
        return;
    }
    readbuf[lp->width] = '\0';
	Push(readbuf, STRING);
    fclose(fp);
#else
	Push("-1", LNUMBER);
	Error("No filePro support");
#endif
	return;
}

