#!/bin/sh
###
# a wrapper around php.cgi to pass the env variables for connecting to
# sybase database
###
#database alias as in interface file
DSQUERY=XXXXXXX
#user
DBUSER=XXXXXXX
#password of the user
DBPW=XXXXX
export DSQUERY
export DBUSER
export DBPW

./php.cgi
