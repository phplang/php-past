/*
 * Copyright (c) 1991, 1992, 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#define	USAGE	"usage: dbfadd <file> <field> <field> ...\n"

/*
 * display the database in question to the stdout
 */
#include <stdio.h>
#include <fcntl.h>

#include "dbf.h"

int
main(int argc, char *argv[])
{
	dbhead_t 	*dbh;
	dbfield_t	*dbf, *cur_f;
	char	*cp, *t_cp;
	int	nfields;

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

	nfields = dbh->db_nfields;
	if (nfields != argc) {
		fprintf(stderr, "mismatch of field count, %d vs %d inputs\n",
				nfields, argc);
		exit(1);
	}
	cp = t_cp = (char *)malloc(dbh->db_rlen + 1);
	*t_cp++ = VALID_RECORD;

	dbf = dbh->db_fields;
	for (cur_f = dbf; cur_f < &dbf[nfields]; cur_f++, argv++) {
		sprintf(t_cp, cur_f->db_format, *argv);
		t_cp += cur_f->db_flen;
	}
	dbh->db_records++;
	if ( put_dbf_record(dbh, dbh->db_records, cp) < 0 ) {
		fprintf(stderr, "unable to put record at %ld\n",
				dbh->db_records);
	}
	put_dbf_info(dbh);
	exit(0);
}
