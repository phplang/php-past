# +----------------------------------------------------------------------+
# | PHP HTML Embedded Scripting Language Version 3.0                     |
# +----------------------------------------------------------------------+
# | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
# +----------------------------------------------------------------------+
# | This program is free software; you can redistribute it and/or modify |
# | it under the terms of one of the following licenses:                 |
# |                                                                      |
# |  A) the GNU General Public License as published by the Free Software |
# |     Foundation; either version 2 of the License, or (at your option) |
# |     any later version.                                               |
# |                                                                      |
# |  B) the PHP License as published by the PHP Development Team and     |
# |     included in the distribution in the file: LICENSE                |
# |                                                                      |
# | This program is distributed in the hope that it will be useful,      |
# | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
# | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
# | GNU General Public License for more details.                         |
# |                                                                      |
# | You should have received a copy of both licenses referred to here.   |
# | If you did not, or have any questions about PHP licensing, please    |
# | contact core@php.net.                                                |


CC = cc -O2 -Dbool=char -DHAVE_FREETDS=1 -I/usr/local/include -I/usr/local/freetds/include -I. -I.. -I../.. -fpic
LD = cc -shared -L/usr/local/lib -L/usr/local/freetds/lib -rdynamic 

all: php3_mssql.so

# MSSQL_OBJS = mssql.o /usr/local/src/freetds/src/dblib/dblib.o /usr/local/src/freetds/src/dblib/dbutil.o
MSSQL_OBJS = mssql.o 
MSSQL_LIBS = -lsybdb -Wl,-rpath /usr/local/freetds/lib
# MSSQL_LIBS = -ltds -Wl,-rpath /usr/local/freetds/lib

php3_mssql.so: $(MSSQL_OBJS)
	$(LD) -o $@ $(MSSQL_OBJS) $(MSSQL_LIBS)

mssql.o: mssql.c
	$(CC) $(CFLAGS) -DCOMPILE_DL=1 -c -o $@ $<

clean:
	-rm -f *.so *.o perl.out

# Local Variables:
# tab-width: 4
# End:
