:
# Copyright (c) 1994 Brad Eacker,
#               (Music, Intuition, Software, and Computers)
# All Rights Reserved
# $Revision: 1.1 $
USAGE="$0 <dbf file> <ndx file>"
if [ "x$2" = "x" ] ; then
	echo usage: $USAGE
	exit 1
fi
dbfndx $2 | xargs dbfget $1
