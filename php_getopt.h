/* $Id: php_getopt.h,v 1.1 1999/06/19 19:06:19 sas Exp $ */

/* Borrowed from Apache NT Port */
#include "php.h"

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

extern int getopt(int argc, char* const *argv, const char *optstr);
