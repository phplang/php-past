/*
 * Copyright (c) 1993, 1994 Brad Eacker,
 *              (Music, Intuition, Software, and Computers)
 * All Rights Reserved
 */

#define	USAGE	"usage: dbfndx [-v] <ndx file>\n"

/*
 * list the records in question to std out using the specified
 * index file.
 */
#include "dbf.h"
#include "dbf_ndx.h"

#include <stdio.h>
#include <fcntl.h>

#if WIN32|WINNT
extern int optiserr(int argc, char * const *argv, int oint, const char *optstr, int optchr, int err);
int getopt(int argc, char* const *argv, const char *optstr);
extern int optind;
#endif


int main(int argc, char *argv[])
{
	ndx_header_t 	*ndh;
	char	*cp;
	int	fd, i;
	int	verbose = 0;
	ndx_record_t	*rp;

	if (argc < 2) {
		printf(USAGE);
		exit(1);
	}
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
	cp = (char *)malloc(256);
	if ((fd = open(*argv, O_RDONLY)) < 0) {
		strcpy(cp, *argv); strcat(cp, ".ndx");
		if ((fd = open(cp, O_RDONLY)) < 0) {
			perror("open");
			exit(1);
		}
	}

	if ((ndh = ndx_get_header(fd)) == 0) {
		fprintf(stderr, "Unable to get header\n");
		exit(1);
	}

	if (verbose) {
		fprintf(stderr, "# starting page %ld, total pages %ld\n",
			ndh->ndx_start_pg, ndh->ndx_total_pgs);
		fprintf(stderr, "# key length %d, keys/page %d, key size %ld\n",
			ndh->ndx_key_len, ndh->ndx_keys_ppg,
			ndh->ndx_key_size);
		fprintf(stderr, "# key type %d, unique %d\n",
			ndh->ndx_key_type, ndh->ndx_unique);
		fprintf(stderr, "# key name '%s'\n", ndh->ndx_key_name);
	}
	rp = ndx_get_first_rec(ndh);
	while (rp) {
		if ( !verbose )
			printf("%ld\n", rp->ndxr_rec);
		else {
			switch(ndh->ndx_key_type) {
			    case 1:
			      {
				double tmp_f = get_double(rp->ndxr_key_data);
				printf("%ld : %f\n", rp->ndxr_rec, tmp_f);
			      }
			      break;
			    case 0:
			    default:
			      strncpy(cp, rp->ndxr_key_data, ndh->ndx_key_len);
			      printf("%ld : '%s'\n", rp->ndxr_rec, cp);
			}
		}
		rp = ndx_get_next_rec(ndh, rp);
	}
	exit(0);
}
