:
# Copyright (c) 1994 Brad Eacker,
#               (Music, Intuition, Software, and Computers)
# All Rights Reserved
# $Revision: 1.1 $
USAGE="$0 <filename> f_name f_type f_len f_fdc [f_name ... f_fdc] ..."
if [ x$1 = x ] ; then
	echo usage: $USAGE
	exit 1
fi
DBF_LIB=${DBF_LIB-LIB_DEF}
DBFNAME=`basename $1 .dbf`
shift
if [ -f tmpl.dbf ] ; then
	cp tmpl.dbf tmp$$.dbf
else
	cp ${DBF_LIB}/tmpl.dbf tmp$$.dbf
fi
while [ x$1 != x ] ; do
	if [ x$4 = x ] ; then
		echo incorrect number of arguments
		echo usage: $USAGE
		rm tmp$$.dbf
		exit 1
	fi
	dbfadd tmp$$ $1 $2 $3 $4
	shift; shift; shift; shift
done
dbfcreat tmp$$ $DBFNAME
rm tmp$$.dbf
