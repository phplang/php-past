/*
 * Copyright (c) 1991, 1992, 1993, 1994 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#define	USAGE	"usage: dbflst [-vdhr] <file>\n"

/*
 * display the database in question to the stdout
 */
#include "dbf.h"

#include <stdio.h>
#include <fcntl.h>

#if WIN32|WINNT
extern int optiserr(int argc, char * const *argv, int oint, const char *optstr, int optchr, int err);
int getopt(int argc, char* const *argv, const char *optstr);
extern int optind;
#endif


int main(int argc, char *argv[])
{
	dbhead_t 	*dbh;
	dbfield_t	*dbf, *cur_f;
	char	*fnp;
	char	*cp;
	int	nfields;
	int	i;
	int	verbose = 0, out_num = 0, head = 0, del_recs = 0;

	while ((i = getopt(argc, argv, "vdhr")) != EOF) {
		switch (i) {
		    case 'v':
			verbose++;
			break;
		    case 'd':
			del_recs++;
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
	argc -= optind;  argv = &argv[optind];
	if (argc < 1) {
		printf(USAGE);
		exit(1);
	}
	if ((dbh = dbf_open(*argv, O_RDONLY)) == NULL) {
		fprintf(stderr, "unable to open database file %s\n", *argv);
		exit(1);
	}
	if (verbose)
		dbf_head_info(dbh);

	nfields = dbh->db_nfields;
	dbf = dbh->db_fields;

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
	while ((cp = dbf_get_next(dbh))) {
		if (del_recs || is_valid_rec(cp)) {
		    if (out_num) {
			printf("%8d ", dbh->db_cur_rec);
		    }
		    out_rec(dbh, dbf, cp);
		}
		free(cp);
	}
	exit(0);
}
