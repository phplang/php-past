# Makefile to generate build tools
#
# Standard usage:
#		make -f build.mk
# 
# Remove all files which are ignored by CVS:
#		make -f build.mk cvsclean
#
# Written by Sascha Schumann
#
# $Id: build.mk,v 1.2 1999/12/02 02:16:39 sas Exp $

config_h_in = config.h.in

targets = configure $(config_h_in)

all: $(targets)

cvsclean:
	@for i in $(shell find . -name .cvsignore); do \
		(cd `dirname $$i` && rm -rf `cat .cvsignore`); \
	done

$(config_h_in): configure.in acconfig.h
	@rm -f $@
	autoheader
	
configure: aclocal.m4 configure.in
	autoconf
