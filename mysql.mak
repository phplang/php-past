# Microsoft Developer Studio Generated NMAKE File, Based on mysql.dsp
!IF "$(CFG)" == ""
CFG=mysql - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mysql - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mysql - Win32 Release" && "$(CFG)" != "mysql - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mysql.mak" CFG="mysql - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mysql - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mysql - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mysql - Win32 Release"

OUTDIR=.\module_Release
INTDIR=.\module_Release
# Begin Custom Macros
OutDir=.\module_Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_mysql.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_mysql.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\mysql.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\php3_mysql.dll"
	-@erase "$(OUTDIR)\php3_mysql.exp"
	-@erase "$(OUTDIR)\php3_mysql.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D\
 HAVE_MYSQL=1 /D HAVE_ERRMSG_H=1 /D "MSVC5" /D "COMPILE_DL" /D "WIN32" /D\
 "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\mysql.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /I /usr/src/phpcvs/php3" /I /usr/src/mysql/include" " " /c\
 
CPP_OBJS=.\module_Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mysql.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib libmysql.lib wsock32.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\php3_mysql.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_mysql.dll" /implib:"$(OUTDIR)\php3_mysql.lib"\
 /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\mysql.obj"

"$(OUTDIR)\php3_mysql.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mysql - Win32 Debug"

OUTDIR=c:\php3
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=c:\php3
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_mysql.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_mysql.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\mysql.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\php3_mysql.dll"
	-@erase "$(OUTDIR)\php3_mysql.exp"
	-@erase "$(OUTDIR)\php3_mysql.ilk"
	-@erase "$(OUTDIR)\php3_mysql.lib"
	-@erase "$(OUTDIR)\php3_mysql.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /I "../../include"\
 /D HAVE_MYSQL=1 /D HAVE_ERRMSG_H=1 /D "DEBUG" /D "MSVC5" /D "COMPILE_DL" /D\
 "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\mysql.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /I /usr/src/phpcvs/php3" /I /usr/src/mysql/include" " " /c\
 
CPP_OBJS=.\module_debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mysql.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib libmysql.lib wsock32.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\php3_mysql.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_mysql.dll" /implib:"$(OUTDIR)\php3_mysql.lib"\
 /pdbtype:sept /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\mysql.obj"

"$(OUTDIR)\php3_mysql.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "mysql - Win32 Release" || "$(CFG)" == "mysql - Win32 Debug"
SOURCE=.\functions\mysql.c

!IF  "$(CFG)" == "mysql - Win32 Release"

DEP_CPP_MYSQL=\
	"..\..\include\errmsg.h"\
	"..\..\include\mysql.h"\
	"..\..\include\mysql_com.h"\
	"..\..\include\mysql_version.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_mysql.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\mysql.obj" : $(SOURCE) $(DEP_CPP_MYSQL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mysql - Win32 Debug"

DEP_CPP_MYSQL=\
	"..\..\include\errmsg.h"\
	"..\..\include\mysql.h"\
	"..\..\include\mysql_com.h"\
	"..\..\include\mysql_version.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_mysql.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\mysql.obj" : $(SOURCE) $(DEP_CPP_MYSQL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

