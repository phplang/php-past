/*
 * Copyright (c) 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#define	USAGE	"usage: dbfdel <file> [<record> <record> ...]\n"

/*
 * display the records of the database in question to the stdout
 */
#include <stdio.h>
#include <fcntl.h>

#include "dbf.h"

int main(int argc, char *argv[])
{
	dbhead_t 	*dbh;
	int	rec_num;

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

	while (argc-- > 0) {
		rec_num = strtol(*argv++, NULL, 0);
		if (del_dbf_record(dbh, rec_num) < 0) {
			if (rec_num > dbh->db_records) {
				fprintf(stderr, "record %d out of bounds\n",
						 rec_num);
				continue;
			} else {
				fprintf(stderr, "unable to delete record %d\n",
						 rec_num);
			}
		}
	}
	put_dbf_info(dbh);
	exit(0);
}
