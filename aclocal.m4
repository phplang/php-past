dnl $Id: aclocal.m4,v 1.52 2000/07/19 15:39:18 rasmus Exp $
dnl
dnl This file contains local autoconf functions.

dnl dummy function for PHP4 compatibility
AC_DEFUN(PHP_EXTENSION,[])
AC_DEFUN(PHP_SUBST,[AC_SUBST($1)])

dnl
dnl AC_PHP_ONCE(namespace, variable, code)
dnl
dnl execute code, if variable is not set in namespace
dnl
AC_DEFUN(AC_PHP_ONCE,[
  unique=`echo $ac_n "$2$ac_c" | tr -c -d a-zA-Z0-9`
  cmd="echo $ac_n \"\$$1$unique$ac_c\""
  if test -n "$unique" && test "`eval $cmd`" = "" ; then
    eval "$1$unique=set"
    $3
  fi
])

dnl
dnl AC_EXPAND_PATH(path, variable)
dnl
dnl expands path to an absolute path and assigns it to variable
dnl
AC_DEFUN(AC_EXPAND_PATH,[
  if test -z "$1" || echo "$1" | grep '^/' >/dev/null ; then
    $2="$1"
  else
    $2="`pwd`/$1"
  fi
])

dnl
dnl AC_ADD_LIBPATH(path)
dnl
dnl add a library to linkpath/runpath
dnl
AC_DEFUN(AC_ADD_LIBPATH,[
  if test "$1" != "/usr/lib"; then
    AC_EXPAND_PATH($1, ai_p)
    AC_PHP_ONCE(LIBPATH, $ai_p, [
      EXTRA_LIBS="$EXTRA_LIBS -L$ai_p"
      if test -n "$APXS" ; then
        RPATHS="$RPATHS ${apxs_runpath_switch}$ai_p'"
      else
        RPATHS="$RPATHS ${ld_runpath_switch}$ai_p"
      fi
    ])
  fi
])

dnl
dnl AC_ADD_INCLUDE(path)
dnl
dnl add a include path
dnl
AC_DEFUN(AC_ADD_INCLUDE,[
  if test "$1" != "/usr/include"; then
    AC_EXPAND_PATH($1, ai_p)
    AC_PHP_ONCE(INCLUDEPATH, $ai_p, [
      INCLUDES="$INCLUDES -I$ai_p"
    ])
  fi
])

dnl
dnl AC_ADD_LIBRARY(library)
dnl
dnl add a library to the link line
dnl
AC_DEFUN(AC_ADD_LIBRARY,[
  AC_PHP_ONCE(LIBRARY, $1, [
    EXTRA_LIBS="$EXTRA_LIBS -l$1"
  ])
])

dnl
dnl AC_ADD_LIBRARY_WITH_PATH(library, path)
dnl
dnl add a library to the link line and path to linkpath/runpath
dnl
AC_DEFUN(AC_ADD_LIBRARY_WITH_PATH,[
  AC_ADD_LIBPATH($2)
  AC_ADD_LIBRARY($1)
])

AC_DEFUN(AC_TEMP_LDFLAGS,[
  old_LDFLAGS="$LDFLAGS"
  LDFLAGS="$1 $LDFLAGS"
  $2
  LDFLAGS="$old_LDFLAGS"
])

AC_DEFUN(AC_ORACLE_VERSION,[
  AC_MSG_CHECKING([Oracle version])
  if test -f "$ORACLEINST_TOP/orainst/unix.rgs"
  then
	ORACLE_VERSION=`grep '"ocommon"' $ORACLEINST_TOP/orainst/unix.rgs | sed 's/[ ][ ]*/:/g' | cut -d: -f 6 | cut -c 2-4`
    test -z "$ORACLE_VERSION" && ORACLE_VERSION=7.3
  else
    if test -f "$ORACLEINST_TOP/lib/libclntsh.so.8.0"
    then
        ORACLE_VERSION=8.1
    else
        ORACLE_VERSION=8.0
    fi
  fi
  AC_MSG_RESULT($ORACLE_VERSION)
])

dnl
dnl Test mSQL version by checking if msql.h has "IDX_TYPE" defined.
dnl
AC_DEFUN(AC_MSQL_VERSION,[
  AC_MSG_CHECKING([mSQL version])
  ac_php_oldcflags=$CFLAGS
  CFLAGS="$INCLUDES $CFLAGS";
  AC_TRY_COMPILE([#include <sys/types.h>
#define APIENTRY
#include "msql.h"],[int i = IDX_TYPE],[
    AC_DEFINE(MSQL1,0)
    MSQL_VERSION="2.0 or newer"
  ],[
    AC_DEFINE(MSQL1,1)
    MSQL_VERSION="1.0"
  ])
  CFLAGS=$ac_php_oldcflags
  AC_MSG_RESULT($MSQL_VERSION)
])

dnl
dnl Figure out which library file to link with for the Solid support.
dnl
AC_DEFUN(AC_FIND_SOLID_LIBS,[
  AC_MSG_CHECKING([Solid library file])
  ac_solid_uname_s=`uname -s 2>/dev/null`
  case $ac_solid_uname_s in
    AIX) ac_solid_os=a3x;;
    HP-UX) ac_solid_os=h9x;;
    IRIX) ac_solid_os=irx;;
    Linux) ac_solid_os=lux;;
    SunOS) ac_solid_os=ssx;; # should we deal with SunOS 4?
    FreeBSD) ac_solid_os=fbx;;
    # "uname -s" on SCO makes no sense.
  esac
  SOLID_LIBS=`echo $1/scl${ac_solid_os}*.so | cut -d' ' -f1`
  if test ! -f $SOLID_LIBS; then
    SOLID_LIBS=`echo $1/scl${ac_solid_os}*.a | cut -d' ' -f1`
  fi
  if test ! -f $SOLID_LIBS; then
    SOLID_LIBS=`echo $1/scl2x${ac_solid_os}*.a | cut -d' ' -f1`
  fi
  if test ! -f $SOLID_LIBS; then
    SOLID_LIBS=`echo $1/scl2x${ac_solid_os}*.a | cut -d' ' -f1`
  fi
  if test ! -f $SOLID_LIBS; then
    SOLID_LIBS=`echo $1/bcl${ac_solid_os}*.so | cut -d' ' -f1`
  fi
  if test ! -f $SOLID_LIBS; then
    SOLID_LIBS=`echo $1/bcl${ac_solid_os}*.a | cut -d' ' -f1`
  fi
  if test ! -f $SOLID_LIBS; then
    SOLID_LIBS="-L$1 -lsolcli"
  fi
  AC_MSG_RESULT(`echo $SOLID_LIBS | sed -e 's!.*/!!'`)
  AC_SOLID_PTHREADS
])

dnl
dnl Figure out whether Solid needs to be linked with pthreads.
dnl
AC_DEFUN(AC_SOLID_PTHREADS,[
  AC_MSG_CHECKING([whether Solid needs pthreads])
  save_LIBS=$LIBS
  LIBS="$SOLID_LIBS"
  AC_TRY_LINK_FUNC(SQLAllocEnv,
                   [LIBS="$save_LIBS"
                    AC_MSG_RESULT(no)],
                   [LIBS="-lpthread $save_LIBS"
                    AC_MSG_RESULT(yes)])
])

dnl
dnl Figure out which library file to link with for the Empress support.
dnl
AC_DEFUN(AC_FIND_EMPRESS_LIBS,[
  AC_MSG_CHECKING([Empress library file])
  EMPRESS_LIBS=`echo $1/empodbc.so | cut -d' ' -f1`
  if test ! -f $EMPRESS_LIBS; then
    EMPRESS_LIBS=`echo $1/empodbc.a | cut -d' ' -f1`
  fi
  AC_MSG_RESULT(`echo $EMPRESS_LIBS | sed -e 's!.*/!!'`)
])


dnl
dnl See if we have broken header files like SunOS has.
dnl
AC_DEFUN(AC_MISSING_FCLOSE_DECL,[
  AC_MSG_CHECKING([for fclose declaration])
  AC_TRY_COMPILE([#include <stdio.h>],[int (*func)() = fclose],[
    AC_DEFINE(MISSING_FCLOSE_DECL,0)
    AC_MSG_RESULT(ok)
  ],[
    AC_DEFINE(MISSING_FCLOSE_DECL,1)
    AC_MSG_RESULT(missing)
  ])
])

# Checks for libraries.
# Prefer gdbm, Berkeley DB and ndbm/dbm, in that order
AC_DEFUN(AC_PREFERRED_DB_LIB,[
  AC_CHECK_LIB(gdbm, gdbm_open,[AC_DEFINE(GDBM) DBM_TYPE=gdbm; DBM_LIB=-lgdbm],
  [AC_CHECK_LIB(db, dbm_open,[AC_DEFINE(NDBM) DBM_TYPE=ndbm; DBM_LIB=-ldb],
   [AC_CHECK_LIB(c, dbm_open,[AC_DEFINE(NDBM) DBM_TYPE=ndbm; DBM_LIB=],
    [AC_CHECK_LIB(dbm, dbm_open,[AC_DEFINE(NDBM) DBM_TYPE=ndbm; DBM_LIB=-ldbm],
     [DBM_TYPE=""])])])])
  AC_MSG_CHECKING([preferred dbm library])
  if test "a$DBM_TYPE" = a; then
    AC_MSG_RESULT(none found)
    AC_MSG_WARN(No dbm library found - using built-in flatfile support)
  else
    AC_MSG_RESULT($DBM_TYPE chosen)
  fi
  AC_SUBST(DBM_LIB)
  AC_SUBST(DBM_TYPE)
])

dnl Assign INCLUDE/LFLAGS from PREFIX
AC_DEFUN(AC_DBA_STD_ASSIGN,[
  if test "$THIS_PREFIX" != "" -a "$THIS_PREFIX" != "/usr"; then
    THIS_INCLUDE="$THIS_PREFIX/include"
    THIS_LFLAGS="$THIS_PREFIX/lib"
  fi
])

dnl Standard check
AC_DEFUN(AC_DBA_STD_CHECK,[
  THIS_RESULT="yes"
  if test "$THIS_PREFIX" != "/usr"; then
  if test "$THIS_INCLUDE" = "" ; then
    AC_MSG_ERROR(cannot find necessary header file(s))
  elif test "$THIS_LIBS" = "" ; then
    AC_MSG_ERROR(cannot find necessary library)
  fi
  fi
])

dnl Attach THIS_x to DBA_x
AC_DEFUN(AC_DBA_STD_ATTACH,[
  AC_ADD_INCLUDE($THIS_INCLUDE)
  AC_ADD_LIBRARY_WITH_PATH($THIS_LIBS, $THIS_LFLAGS)

  THIS_INCLUDE=""
  THIS_LIBS=""
  THIS_LFLAGS=""
  THIS_PREFIX=""
])

dnl Print the result message
AC_DEFUN(AC_DBA_STD_RESULT,[
  if test "$THIS_RESULT" = "yes"; then
    HAVE_DBA=1
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
  THIS_RESULT=""
])


dnl
dnl Check for concurrent available database libraries
dnl
AC_DEFUN(AC_AVAILABLE_DB_LIBS,[
dnl NDBM in -lc or -ldbm or -lndbm or -ldb1 or -lgdbm
  AC_CHECK_LIB(c, dbm_open, [AC_DEFINE(DBA_NDBM, 1)],
    [AC_CHECK_LIB(dbm, dbm_open, [AC_DEFINE(DBA_NDBM, 1) DBA_LIBS=-ldbm],
    [AC_CHECK_LIB(ndbm, dbm_open, [AC_DEFINE(DBA_NDBM, 1) DBA_LIBS=-lndbm],
    [AC_CHECK_LIB(db1, dbm_open, [AC_DEFINE(DBA_NDBM, 1) DBA_LIBS=-ldb1],
    [AC_CHECK_LIB(gdbm, dbm_open, [AC_DEFINE(DBA_NDBM, 1) DBA_LIBS=-lgdbm])])])])])
dnl DBM in -lc or -ldbm or -ldb1 or -lgdbm
  AC_CHECK_LIB(c, dbminit, [AC_DEFINE(DBA_DBM, 1)],
    [AC_CHECK_LIB(dbm, dbminit, 
        [AC_DEFINE(DBA_DBM, 1) DBA_LIBS="$DBA_LIBS -ldbm"],
    [AC_CHECK_LIB(db1, dbminit, 
        [AC_DEFINE(DBA_DBM, 1) DBA_LIBS="$DBA_LIBS -ldb1"],
	[AC_CHECK_LIB(db, __db_dbm_init,
		[AC_DEFINE(DBA_DBM, 1) DBA_LIBS="$DBA_LIBS -ldb"],
	[AC_CHECK_LIB(gdbm, dbminit,
		[AC_DEFINE(DBA_DBM, 1) DBA_LIBS="$DBA_LIBS -lgdbm"])])])])])
dnl GDBM in -lgdbm
  AC_CHECK_LIB(gdbm, gdbm_open, 
      [AC_DEFINE(DBA_GDBM, 1) DBA_LIBS="$DBA_LIBS -lgdbm"])
dnl Sleepycat DB2 in -ldb or -ldb2
  AC_CHECK_LIB(db, db_open,[AC_DEFINE(DBA_DB2, 1) DBA_LIBS="$DBA_LIBS -ldb"],
    [AC_CHECK_LIB(db2,db_open,[AC_DEFINE(DBA_DB2,1) DBA_LIBS="$DBA_LIBS -ldb2"])
  ])
dnl dbopen(3) interface 
  AC_CHECK_LIB(c, dbopen, [AC_DEFINE(DBA_DBOPEN, 1)],
    [AC_CHECK_LIB(db1, dbopen, 
        [AC_DEFINE(DBA_DBOPEN, 1) DBA_LIBS="$DBA_LIBS -ldb1"])])
dnl DJB's cdb
  AC_CHECK_LIB(cdb, cdb_bread, 
    [AC_DEFINE(DBA_CDB, 1) DBA_LIBS="$DBA_LIBS -lcdb"])
])

dnl
dnl Check for broken sprintf()
dnl
AC_DEFUN(AC_BROKEN_SPRINTF,[
  AC_MSG_CHECKING([for broken sprintf])
  AC_TRY_RUN([main() { char buf[20]; exit (sprintf(buf,"testing 123")!=11); }],[
    AC_DEFINE(BROKEN_SPRINTF,0)
    AC_MSG_RESULT(ok)
  ],[
    AC_DEFINE(BROKEN_SPRINTF,1)
    AC_MSG_RESULT(broken)
  ],[
    AC_DEFINE(BROKEN_SPRINTF,0)
    AC_MSG_RESULT(cannot check, guessing ok)
  ])
])

dnl
dnl Check for crypt() capabilities
dnl
AC_DEFUN(AC_CRYPT_CAP,[

  AC_MSG_CHECKING([for standard DES crypt])
  AC_TRY_RUN([
#if HAVE_CRYPT_H
#include <crypt.h>
#endif

main() {
#if HAVE_CRYPT
    exit (strcmp((char *)crypt("rasmuslerdorf","rl"),"rl.3StKT.4T8M"));
#else
	exit(0);
#endif
}],[
    AC_DEFINE(PHP3_STD_DES_CRYPT,1)
    AC_MSG_RESULT(yes)
  ],[
    AC_DEFINE(PHP3_STD_DES_CRYPT,0)
    AC_MSG_RESULT(no)
  ],[
    AC_DEFINE(PHP3_STD_DES_CRYPT,1)
    AC_MSG_RESULT(cannot check, guessing yes)
  ])

  AC_MSG_CHECKING([for extended DES crypt])
  AC_TRY_RUN([
#if HAVE_CRYPT_H
#include <crypt.h>
#endif

main() {
#if HAVE_CRYPT
    exit (strcmp((char *)crypt("rasmuslerdorf","_J9..rasm"),"_J9..rasmBYk8r9AiWNc"));
#else
	exit(0);
#endif
}],[
    AC_DEFINE(PHP3_EXT_DES_CRYPT,1)
    AC_MSG_RESULT(yes)
  ],[
    AC_DEFINE(PHP3_EXT_DES_CRYPT,0)
    AC_MSG_RESULT(no)
  ],[
    AC_DEFINE(PHP3_EXT_DES_CRYPT,0)
    AC_MSG_RESULT(cannot check, guessing no)
  ])

  AC_MSG_CHECKING([for MD5 crypt])
  AC_TRY_RUN([
#if HAVE_CRYPT_H
#include <crypt.h>
#endif

main() {
#if HAVE_CRYPT
    char salt[15], answer[40];

    salt[0]='$'; salt[1]='1'; salt[2]='$'; 
    salt[3]='r'; salt[4]='a'; salt[5]='s';
    salt[6]='m'; salt[7]='u'; salt[8]='s';
    salt[9]='l'; salt[10]='e'; salt[11]='$';
    salt[12]='\0';
    strcpy(answer,salt);
    strcat(answer,"rISCgZzpwk3UhDidwXvin0");
    exit (strcmp((char *)crypt("rasmuslerdorf",salt),answer));
#else
	exit(0);
#endif
}],[
    AC_DEFINE(PHP3_MD5_CRYPT,1)
    AC_MSG_RESULT(yes)
  ],[
    AC_DEFINE(PHP3_MD5_CRYPT,0)
    AC_MSG_RESULT(no)
  ],[
    AC_DEFINE(PHP3_MD5_CRYPT,0)
    AC_MSG_RESULT(cannot check, guessing no)
  ])

  AC_MSG_CHECKING([for Blowfish crypt])
  AC_TRY_RUN([
#if HAVE_CRYPT_H
#include <crypt.h>
#endif

main() {
#if HAVE_CRYPT
    char salt[25], answer[70];
    
    salt[0]='$'; salt[1]='2'; salt[2]='a'; salt[3]='$'; salt[4]='0'; salt[5]='7'; salt[6]='$'; salt[7]='\0';
    strcat(salt,"rasmuslerd");
    strcpy(answer,salt);
    strcpy(&answer[16],"O............gl95GkTKn53Of.H4YchXl5PwvvW.5ri");
    exit (strcmp((char *)crypt("rasmuslerdorf",salt),answer));
#else
	exit(0);
#endif
}],[
    AC_DEFINE(PHP3_BLOWFISH_CRYPT,1)
    AC_MSG_RESULT(yes)
  ],[
    AC_DEFINE(PHP3_BLOWFISH_CRYPT,0)
    AC_MSG_RESULT(no)
  ],[
    AC_DEFINE(PHP3_BLOWFISH_CRYPT,0)
    AC_MSG_RESULT(cannot check, guessing no)
  ])
])

## libtool.m4 - Configure libtool for the target system. -*-Shell-script-*-
## Copyright (C) 1996, 1997 Free Software Foundation, Inc.
## Gordon Matzigkeit <gord@gnu.ai.mit.edu>, 1996
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
##
## As a special exception to the GNU General Public License, if you
## distribute this file as part of a program that contains a
## configuration script generated by Autoconf, you may include it under
## the same distribution terms that you use for the rest of that program.

# serial 9 AM_PROG_LIBTOOL
AC_DEFUN(AM_PROG_LIBTOOL,
[AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_RANLIB])
AC_REQUIRE([AM_PROG_LD])
AC_REQUIRE([AC_PROG_LN_S])

# Always use our own libtool.
LIBTOOL='$(top_builddir)/libtool'
AC_SUBST(LIBTOOL)

dnl Allow the --disable-shared flag to stop us from building shared libs.
AC_ARG_ENABLE(shared,
[  --enable-shared         build shared libraries [default=yes]],
test "$enableval" = no && libtool_shared=" --disable-shared",
libtool_shared=)

dnl Allow the --disable-static flag to stop us from building static libs.
AC_ARG_ENABLE(static,
[  --enable-static         build static libraries [default=yes]],
test "$enableval" = no && libtool_static=" --disable-static",
libtool_static=)

libtool_flags="$libtool_shared$libtool_static"
test "$silent" = yes && libtool_flags="$libtool_flags --silent"
test "$ac_cv_prog_gcc" = yes && libtool_flags="$libtool_flags --with-gcc"
test "$ac_cv_prog_gnu_ld" = yes && libtool_flags="$libtool_flags --with-gnu-ld"

# Some flags need to be propagated to the compiler or linker for good
# libtool support.
[case "$host" in
*-*-irix6*)
  for f in '-32' '-64' '-cckr' '-n32' '-mips1' '-mips2' '-mips3' '-mips4'; do
    if echo " $CC $CFLAGS " | egrep -e "[ 	]$f[	 ]" > /dev/null; then
      LD="${LD-ld} $f"
    fi
  done
  ;;

*-*-sco3.2v5*)
  # On SCO OpenServer 5, we need -belf to get full-featured binaries.
  CFLAGS="$CFLAGS -belf"
  ;;
esac]

# Actually configure libtool.  ac_aux_dir is where install-sh is found.
CC="$CC" CFLAGS="$CFLAGS" CPPFLAGS="$CPPFLAGS" \
LD="$LD" RANLIB="$RANLIB" LN_S="$LN_S" \
${CONFIG_SHELL-/bin/sh} $ac_aux_dir/ltconfig \
$libtool_flags --no-verify $ac_aux_dir/ltmain.sh $host \
|| AC_MSG_ERROR([libtool configure failed])
])

# AM_PROG_LD - find the path to the GNU or non-GNU linker
AC_DEFUN(AM_PROG_LD,
[AC_ARG_WITH(gnu-ld,
[  --with-gnu-ld           assume the C compiler uses GNU ld [default=no]],
test "$withval" = no || with_gnu_ld=yes, with_gnu_ld=no)
if test "$with_gnu_ld" = yes; then
  AC_MSG_CHECKING([for GNU ld])
else
  AC_MSG_CHECKING([for non-GNU ld])
fi
AC_CACHE_VAL(ac_cv_path_LD,
[case "$LD" in
  /*)
  ac_cv_path_LD="$LD" # Let the user override the test with a path.
  ;;
  *)
  IFS="${IFS= 	}"; ac_save_ifs="$IFS"; IFS="${IFS}:"
  for ac_dir in $PATH; do
    test -z "$ac_dir" && ac_dir=.
    if test -f "$ac_dir/ld"; then
      ac_cv_path_LD="$ac_dir/ld"
      # Check to see if the program is GNU ld.  I'd rather use --version,
      # but apparently some GNU ld's only accept -v.
      # Break only if it was the GNU/non-GNU ld that we prefer.
      if "$ac_cv_path_LD" -v 2>&1 < /dev/null | egrep '(GNU ld|with BFD)' > /dev/null; then
	test "$with_gnu_ld" = yes && break
      else
        test "$with_gnu_ld" != yes && break
      fi
    fi
  done
  IFS="$ac_save_ifs"
  ;;
esac])
LD="$ac_cv_path_LD"
if test -n "$LD"; then
  AC_MSG_RESULT($LD)
else
  AC_MSG_RESULT(no)
fi
test -z "$LD" && AC_MSG_ERROR([no acceptable ld found in \$PATH])
AC_SUBST(LD)
AM_PROG_LD_GNU
])

AC_DEFUN(AM_PROG_LD_GNU,
[AC_CACHE_CHECK([whether we are using GNU ld], ac_cv_prog_gnu_ld,
[# I'd rather use --version here, but apparently some GNU ld's only accept -v.
if $LD -v 2>&1 </dev/null | egrep '(GNU ld|with BFD)' > /dev/null; then
  ac_cv_prog_gnu_ld=yes
else
  ac_cv_prog_gnu_ld=no
fi])
])

# the following 'borrwed' from automake until we switch over
# Add --enable-maintainer-mode option to configure.
# From Jim Meyering

# serial 1

AC_DEFUN(AM_MAINTAINER_MODE,
[AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
  dnl maintainer-mode is disabled by default
  AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode enable make rules and dependencies not useful
                          (and sometimes confusing) to the casual installer],
      USE_MAINTAINER_MODE=$enableval,
      USE_MAINTAINER_MODE=no)
  AC_MSG_RESULT($USE_MAINTAINER_MODE)
  if test $USE_MAINTAINER_MODE = yes; then
    MAINT=
  else
    MAINT='#M#'
  fi
  AC_SUBST(MAINT)dnl
]
)

# another one stolen from automake temporarily
#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "$@" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

# @defmac AC_PROG_CC_STDC
# @maindex PROG_CC_STDC
# @ovindex CC
# If the C compiler in not in ANSI C mode by default, try to add an option
# to output variable @code{CC} to make it so.  This macro tries various
# options that select ANSI C on some system or another.  It considers the
# compiler to be in ANSI C mode if it handles function prototypes correctly.
#
# If you use this macro, you should check after calling it whether the C
# compiler has been set to accept ANSI C; if not, the shell variable
# @code{am_cv_prog_cc_stdc} is set to @samp{no}.  If you wrote your source
# code in ANSI C, you can make an un-ANSIfied copy of it by using the
# program @code{ansi2knr}, which comes with Ghostscript.
# @end defmac

AC_DEFUN(AM_PROG_CC_STDC,
[AC_REQUIRE([AC_PROG_CC])
AC_BEFORE([$0], [AC_C_INLINE])
AC_BEFORE([$0], [AC_C_CONST])
dnl Force this before AC_PROG_CPP.  Some cpp's, eg on HPUX, require
dnl a magic option to avoid problems with ANSI preprocessor commands
dnl like #elif.
dnl FIXME: can't do this because then AC_AIX won't work due to a
dnl circular dependency.
dnl AC_BEFORE([$0], [AC_PROG_CPP])
AC_MSG_CHECKING(for ${CC-cc} option to accept ANSI C)
AC_CACHE_VAL(am_cv_prog_cc_stdc,
[am_cv_prog_cc_stdc=no
ac_save_CC="$CC"
# Don't try gcc -ansi; that turns off useful extensions and
# breaks some systems' header files.
# AIX			-qlanglvl=ansi
# Ultrix and OSF/1	-std1
# HP-UX			-Aa -D_HPUX_SOURCE
# SVR4			-Xc -D__EXTENSIONS__
for ac_arg in "" -qlanglvl=ansi -std1 "-Aa -D_HPUX_SOURCE" "-Xc -D__EXTENSIONS__"
do
  CC="$ac_save_CC $ac_arg"
  AC_TRY_COMPILE(
[#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
/* Most of the following tests are stolen from RCS 5.7's src/conf.sh.  */
struct buf { int x; };
FILE * (*rcsopen) (struct buf *, struct stat *, int);
static char *e (p, i)
     char **p;
     int i;
{
  return p[i];
}
static char *f (char * (*g) (char **, int), char **p, ...)
{
  char *s;
  va_list v;
  va_start (v,p);
  s = g (p, va_arg (v,int));
  va_end (v);
  return s;
}
int test (int i, double x);
struct s1 {int (*f) (int a);};
struct s2 {int (*f) (double a);};
int pairnames (int, char **, FILE *(*)(struct buf *, struct stat *, int), int, int);
int argc;
char **argv;
], [
return f (e, argv, 0) != argv[0]  ||  f (e, argv, 1) != argv[1];
],
[am_cv_prog_cc_stdc="$ac_arg"; break])
done
CC="$ac_save_CC"
])
if test -z "$am_cv_prog_cc_stdc"; then
  AC_MSG_RESULT([none needed])
else
  AC_MSG_RESULT($am_cv_prog_cc_stdc)
fi
case "x$am_cv_prog_cc_stdc" in
  x|xno) ;;
  *) CC="$CC $am_cv_prog_cc_stdc" ;;
esac
])

AC_DEFUN(PHP_DECLARED_TIMEZONE,[
  AC_CACHE_CHECK(for declared timezone, ac_cv_declared_timezone,[
    AC_TRY_COMPILE([
#include <sys/types.h>
#include <time.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
],[
    time_t foo = (time_t) timezone;
],[
  ac_cv_declared_timezone=yes
],[
  ac_cv_declared_timezone=no
])])
  if test "$ac_cv_declared_timezone" = "yes"; then
    AC_DEFINE(HAVE_DECLARED_TIMEZONE, 1, [Whether system headers declare timezone])
  fi
])
