#!/bin/sh
#
# A simple shell script to insert image entries into the database.
# First argument is the category to add the images to, followed
# by the relative path (relative to the photo album script files)
# of each of the image files.
#
# For example: ./ins.sh Family family/*
#
category=$1
shift
for i in $*; do
mysql rasmus -e "insert into photos values ('$i','$category','');"
done
