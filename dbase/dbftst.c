/*
 * Copyright (c) 1991, 1992, 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#define	USAGE	"usage: dbftst <file>\n"

/*
 * test routine to check out the header and fields
 */
#include <stdio.h>
#include <fcntl.h>

#include "dbf.h"

int main(int argc, char *argv[])
{
	dbhead_t 	*dbh;

	if (argc < 2) {
		printf(USAGE);
		exit(1);
	}
	argv++;
	if ((dbh = dbf_open(*argv, O_RDONLY)) == NULL) {
		fprintf(stderr, "unable to open database file %s\n", *argv);
		exit(1);
	}
	dbf_head_info(dbh);
	exit(0);
}
