/*
 * Copyright (c) 1993 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#define	USAGE	"usage: dbfcreat [-v] <file> <target xbase>\n"

/*
 * display the database in question to the stdout
 */
#include "dbf.h"

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>

#define	OPEN_RDWR	(O_RDWR | O_CREAT | O_TRUNC)
#define	OPEN_RDONLY	(O_RDONLY)

#if WIN32|WINNT
extern int optiserr(int argc, char * const *argv, int oint, const char *optstr, int optchr, int err);
int getopt(int argc, char* const *argv, const char *optstr);
#endif

struct db_field creat_f[] = {
	{"FIELD_NAME", 'C', 10, 0, NULL},
	{"FIELD_TYPE", 'C',  1, 0, NULL},
	{"FIELD_LEN",  'N',  3, 0, NULL},
	{"FIELD_DEC",  'N',  4, 0, NULL},
};

int main(int argc, char *argv[])
{
	dbhead_t 	*dbh, *tdbh;
	dbfield_t	*dbf, *cur_f, *tdbf, *tcur_f;
	char	*cp, *tcp;
	int	nfields;
	int	tfd;
	int	rec_cnt, i, rlen;
	extern int optind;
	int	verbose = 0;
	int	fail = 0;

	while ((i = getopt(argc, argv, "v")) != EOF) {
		switch (i) {
		    case 'v':
			verbose++;
			break;
		    default:
			printf(USAGE);
			exit(1);
		}
	}
	argc -= optind;  argv = &argv[optind];
	if (argc < 2) {
		printf(USAGE);
		exit(1);
	}
	if ((dbh = dbf_open(*argv, O_RDONLY)) == NULL) {
		fprintf(stderr, "unable to open database file %s\n", *argv);
		exit(1);
	}
	argv++;

	cp = (char *)malloc(256);
	strcpy(cp, *argv);
	if (strstr(cp, ".dbf") == 0)
		strcat(cp, ".dbf");
	if ((tfd = open(cp, OPEN_RDWR, 0644)) < 0) {
		perror("open");
		exit(1);
	}
	free(cp);

	if (verbose) {
		printf("# total field records %ld\n", dbh->db_records);
	}
	dbf = dbh->db_fields;
	if ((nfields = dbh->db_nfields) != 4) {
		fprintf(stderr, "number of fields must be 4");
		exit(1);
	}
	tcur_f = creat_f;
	for (cur_f = dbf; cur_f < &dbf[nfields] ; cur_f++, tcur_f++) {
		if (strcmp(cur_f->db_fname, tcur_f->db_fname) != 0) {
			fprintf(stderr, "field name mismatch, '%s' != '%s'\n",
				cur_f->db_fname, tcur_f->db_fname);
			fail = 1;
		}
		if (cur_f->db_type != tcur_f->db_type) {
			fprintf(stderr, "field type mismatch, '%c' != '%c'\n",
				cur_f->db_type, tcur_f->db_type);
			fail = 1;
		}
	}
	if (fail) {
		exit(1);
	}

	tdbh = (dbhead_t *)malloc(sizeof(dbhead_t));
	tdbf = (dbfield_t *)malloc(sizeof(dbfield_t) * dbh->db_records);
	if (tdbh == NULL || tdbf == NULL) {
		fprintf(stderr, "unable to get memory for header info\n");
		exit(1);
	}
	tdbh->db_fields = tdbf;
	tdbh->db_fd = tfd;
	tdbh->db_dbt = DBH_TYPE_NORMAL;
	strcpy(tdbh->db_date, "19930818");
	tdbh->db_records = 0;

	/* step thru the records */
	tcp = (char *)malloc(256);
	tcur_f = tdbf; nfields = 0;
	rlen = 1;
	for (rec_cnt = 1; rec_cnt <= dbh->db_records; rec_cnt++) {
		if ((cp = get_dbf_record(dbh, rec_cnt)) == NULL) {
			printf("tried to read bad record %d\n", rec_cnt);
			break;
		}
		if (*cp == DELETED_RECORD) {
			free(cp);
			continue;
		}
		cur_f = dbf;
		copy_crimp(tcur_f->db_fname, &cp[cur_f->db_foffset],
					cur_f->db_flen);
		cur_f++;
		tcur_f->db_type = toupper(cp[cur_f->db_foffset]);
		cur_f++;
		strncpy(tcp, &cp[cur_f->db_foffset], cur_f->db_flen);
		tcp[cur_f->db_flen] = 0;
		tcur_f->db_flen = strtol(tcp, NULL, 0);
		rlen += tcur_f->db_flen;
		cur_f++;
		strncpy(tcp, &cp[cur_f->db_foffset], cur_f->db_flen);
		tcp[cur_f->db_flen] = 0;
		tcur_f->db_fdc = strtol(tcp, NULL, 0);

		switch (tcur_f->db_type) {
		    case 'L':
			if (tcur_f->db_flen != 1) {
				fprintf(stderr, "Logical field len != 1\n");
				fail = 1;
			}
			break;
		    case 'M':
			if (tcur_f->db_flen != 9) {
				fprintf(stderr, "Memo Field len != 9\n");
				fail = 1;
			}
			tdbh->db_dbt = DBH_TYPE_MEMO;
			break;
		    case 'C':
		    case 'D':
		    case 'N':
			break;
		    default:
			fprintf(stderr, "unknown field type - %c\n",
					 cur_f->db_type);
			fail = 1;
		}
		free(cp);
		tcur_f->db_format = get_dbf_f_fmt(tcur_f);
		tcur_f++; nfields++;
	}
	free(tcp);

	if (fail) {
		exit(1);
	}

	tdbh->db_nfields = nfields;
	tdbh->db_hlen = sizeof(struct dbf_dhead) + 2 +
				nfields * sizeof(struct dbf_dfield);
	tdbh->db_rlen = rlen;

	if (verbose) {
		printf("# total fields %d\n", tdbh->db_nfields);

		/* put out the field headings */
                for (cur_f = tdbf; cur_f < &tdbf[nfields] ; cur_f++) {
                        printf("# %s, %c, %d, %d\n", cur_f->db_fname,
                                cur_f->db_type, cur_f->db_flen, cur_f->db_fdc);
                }
	}

	put_dbf_info(tdbh);
	exit(0);
}
