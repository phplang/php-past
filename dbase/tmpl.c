/*
 * Copyright (c) 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

/*
 * build the template file for creating databases
 */
#include <stdio.h>
#include <fcntl.h>

#include "dbf.h"

#define	TEMPL_FILE	"tmpl.dbf"
#define	OPEN_FLAGS	(O_RDWR | O_CREAT | O_TRUNC)

struct db_field creat_f[] = {
	{"FIELD_NAME", 'C', 10, 0, NULL,  1},
	{"FIELD_TYPE", 'C',  1, 0, NULL, 11},
	{"FIELD_LEN",  'N',  3, 0, NULL, 12},
	{"FIELD_DEC",  'N',  4, 0, NULL, 15},
};

dbhead_t creat_h = {
	0, DBH_TYPE_NORMAL, {'1','9','9','3','0','8','0','5',0},
	0, 0, 18+1, 4, creat_f
};

int main(int argc, char *argv[])
{
	dbhead_t 	*dbh = &creat_h;
	dbfield_t	*dbf, *cur_f;
	int		fd, nfields;

	if ((fd = open(TEMPL_FILE, OPEN_FLAGS, 0644)) < 0) {
		perror("open");
		exit(1);
	}

	nfields = dbh->db_nfields;
	printf("# fields: %d, record len: %d\n", nfields, dbh->db_rlen);

	/* step thru the field info */
	dbf = dbh->db_fields;
	for (cur_f = dbf; cur_f < &dbf[nfields] ; cur_f++) {
		printf("  %s, %c, %d\n", cur_f->db_fname, cur_f->db_type,
			cur_f->db_flen);
	}

	dbh->db_fd = fd;
	dbh->db_hlen = sizeof(struct dbf_dhead) + 2 +
			dbh->db_nfields * sizeof(struct dbf_dfield);

	put_dbf_info(dbh);
	exit(0);
}
