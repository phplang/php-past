#
# Makefile for the PHP Tools
#
# By Rasmus Lerdorf
#

#
# Here are the configurable options.
#
# For BSDi systems, use: -DFLOCK
# For SVR4 systems (Solaris - SunOS 5.4), use: -DLOCKF
# For SunOS systems use: -DFLOCK -DFILEH
# For AIX systems use: -DLOCKF -DLOCKFH
# For Linux use: -DLOCKF
# For BSD 4.3 use: -DFLOCK -DFILEH -DDIRECT
#
# If you want to disable the <!--!command--> feature add this: -DNOSYSTEM

OPTIONS = -DFLOCK

# Generic compiler options
#CFLAGS	=	-g -O2 -Wall -DDEBUG $(OPTIONS)
CFLAGS =	-O2 $(OPTIONS)
CC =	gcc
# If you don't have gcc, use these instead:
#CFLAGS =	-g $(OPTIONS) 
#CC =	cc

TSOURCE =	php/phpf.c php/phpl.c php/phplview.c php/phplmon.c php/common.c \
			php/error.c php/post.c php/wm.c php/common.h php/config.h \
			php/subvar.c php/html_common.h php/post.h php/version.h php/wm.h \
			php/Makefile php/README php/License

SOURCE =	phpf.c phpl.c phplview.c phplmon.c common.c \
			error.c post.c wm.c common.h config.h \
			subvar.c html_common.h post.h version.h wm.h \
			Makefile README License

ALL: phpl.cgi phplmon.cgi phplview.cgi phpf.cgi

phpl.cgi:	phpl.o wm.o common.o post.o subvar.o error.o
		$(CC) -o phpl.cgi phpl.o wm.o common.o post.o error.o subvar.o

phplmon.cgi:	phplmon.o common.o
		$(CC) -o phplmon.cgi phplmon.o common.o

phplview.cgi:	phplview.o common.o post.o error.o
		$(CC) -o phplview.cgi phplview.o common.o post.o error.o

phpf.cgi:	phpf.o post.o error.o
		$(CC) -o phpf.cgi phpf.o post.o error.o common.o

php.tar:	$(SOURCE)
		cd ..;tar -cf php/php.tar $(TSOURCE);cd php

error.o:	error.c html_common.h
phpl.o:		phpl.c config.h
phplmon.o:	phplmon.c config.h
phplview.o:	phplview.c
wm.o:	wm.c
common.o:	common.c version.h common.h
post.o:		post.c html_common.h
phpf.o:		phpf.c html_common.h common.h
subvar.o:	subvar.c
