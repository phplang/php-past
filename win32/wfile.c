/* $Id: wfile.c,v 1.7 1998/12/28 09:44:05 sas Exp $ */

/* Function borrowed from the Downhill Project */
#include "wfile.h"
#include "direct.h"

int readlink(char *file_Name, char *buf_Mem, int buf_Size)
{
	/* See if the file exists */
	if (access(file_Name, X_OK) == -1) {
		errno = ENOENT;
	} else {
		errno = EINVAL;
	}

	/* Either way, it's not a link */
	return -1;
}
