/*
 * Copyright (c) 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#define	USAGE	"usage: dbfpack <file>\n"

/*
 * display the database in question to the stdout
 */
#include "dbf.h"

#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	dbhead_t 	*dbh;

	if (argc < 2) {
		printf(USAGE);
		exit(1);
	}
	argv++;	argc--;
	if ((dbh = dbf_open(*argv, O_RDWR)) == NULL) {
		fprintf(stderr, "unable to open database file %s\n", *argv);
		exit(1);
	}
	argv++;	argc--;

	pack_dbf(dbh);

	put_dbf_info(dbh);
	exit(0);
}
