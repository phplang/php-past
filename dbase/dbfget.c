/*
 * Copyright (c) 1993, 1994 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

/*
 * display selected records of the database in question to the stdout
 */
#include <stdio.h>
#include <fcntl.h>

#define	USAGE	"usage: dbfget [-vhr] <file> [<record> <record>...]\n"

#include "dbf.h"

#if WIN32|WINNT
extern int optiserr(int argc, char * const *argv, int oint, const char *optstr, int optchr, int err);
int getopt(int argc, char* const *argv, const char *optstr);
#endif


int main(int argc, char *argv[])
{
	dbhead_t 	*dbh;
	dbfield_t	*dbf, *cur_f;
	char	*fnp;
	char	*cp;
	int	nfields;
	int	rec_cnt, i;
	extern int optind;
	int	verbose = 0, head = 0, out_num = 0;

	while ((i = getopt(argc, argv, "vhr")) != EOF) {
		switch (i) {
		    case 'v':
			verbose++;
			break;
		    case 'h':
			head++;
			break;
		    case 'r':
			out_num++;
			break;
		    default:
			printf(USAGE);
			exit(1);
		}
	}
	argc -= optind;  argv += optind;
	if (argc < 1) {
		printf(USAGE);
		exit(1);
	}
	if ((dbh = dbf_open(*argv, O_RDONLY)) == NULL) {
		fprintf(stderr, "unable to open database file %s\n", *argv);
		exit(1);
	}
	argv++; argc--;

	nfields = dbh->db_nfields;
	dbf = dbh->db_fields;

	if (verbose)
		dbf_head_info(dbh);

	/* put out the field headings */
	if (head) {
	    if (out_num) {
		printf("%8s ", " ");
	    }
	    printf(" ");
	    for (cur_f = dbf; cur_f < &dbf[nfields] ; cur_f++) {
		fnp = (char *)strdup(cur_f->db_fname);
		if (strlen(fnp) > (unsigned int)cur_f->db_flen) {
			fnp[cur_f->db_flen] = 0;
		}
		printf(" ");
		printf(cur_f->db_format, fnp);
		free((char *)fnp);
	   }
	   printf("\n");
	}

	/* step thru the records */
	while (argc-- > 0) {
		rec_cnt = strtol(*argv++, NULL, 0);
		if ((cp = get_dbf_record(dbh, rec_cnt)) == NULL) {
			printf("tried to read bad record %d\n", rec_cnt);
			break;
		}
		if (out_num) {
			printf("%8d ", rec_cnt);
		}
		out_rec(dbh, dbf, cp);
		free(cp);
	}
	exit(0);
}
