#
# interface functions for controlling qmail vusers
# requires the vmailmgr package 
# http://www.qcc.sk.ca/~bguenter/distrib/vmailmgr/ 
# and of course, qmail, www.qmail.org
#
# PHP 3 can be found at www.php.net
#
# code by Shane Caraveo shane@caraveo.com
# copy freely!
#
# USE AT YOUR OWN RISK!
#

CFLAGS	= -O -fpic
CC	= gcc

CXX	= g++
CXXFLAGS= $(CFLAGS)

LD	= g++
LDFLAGS	= -G

PHPINCLUDE =  -I/usr/local/etc/php-3.0RC3
# if php is compiled as an apache module
APACHEINC = -I/usr/local/etc/httpd/src
OBJS    =  php3_vmailmgr.o phpvmail.o
CFLAGS	:= $(CFLAGS) -I../lib $(PHPINCLUDE) $(APACHEINC)
CXXFLAGS:= $(CXXFLAGS) -I../lib $(PHPINCLUDE) $(APACHEINC)
LIBS	:= $(LIBS) -L../lib -lvmailmgr

all: php3_vmailmgr.so

php3_vmailmgr.so: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LIBS)

php3_vmailmgr/%.o: php3_vmailmgr/%.c
	$(CC) $(CFLAGS) -DCOMPILE_DL=1 -c -o $@ $<

phpvmail/%.o: phpvmail/%.cc
	$(CC) $(CFLAGS) -DCOMPILE_DL=1 -c -o $@ $<

clean:
	$(RM) *.o
	$(RM) *.so
	
veryclean: clean
	$(RM) *~
