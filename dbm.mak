# Microsoft Developer Studio Generated NMAKE File, Based on dbm.dsp
!IF "$(CFG)" == ""
CFG=dbm - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dbm - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dbm - Win32 Release" && "$(CFG)" != "dbm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbm.mak" CFG="dbm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbm - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbm - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dbm - Win32 Release"

OUTDIR=.\module_release
INTDIR=.\module_release
# Begin Custom Macros
OutDir=.\module_release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_dbm.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_dbm.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\db.obj"
	-@erase "$(INTDIR)\flock.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\php3_dbm.dll"
	-@erase "$(OUTDIR)\php3_dbm.exp"
	-@erase "$(OUTDIR)\php3_dbm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../../include" /D\
 NDBM=1 /D GDBM=0 /D BSD2=1 /D "NDEBUG" /D "THREAD_SAFE" /D "COMPILE_DL" /D\
 "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\dbm.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib libdb.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /version:2 /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\php3_dbm.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_dbm.dll" /implib:"$(OUTDIR)\php3_dbm.lib"\
 /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\db.obj" \
	"$(INTDIR)\flock.obj"

"$(OUTDIR)\php3_dbm.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbm - Win32 Debug"

OUTDIR=.\module_debug
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=.\module_debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_dbm.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_dbm.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\db.obj"
	-@erase "$(INTDIR)\flock.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\php3_dbm.dll"
	-@erase "$(OUTDIR)\php3_dbm.exp"
	-@erase "$(OUTDIR)\php3_dbm.ilk"
	-@erase "$(OUTDIR)\php3_dbm.lib"
	-@erase "$(OUTDIR)\php3_dbm.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /I "../../include"\
 /D NDBM=1 /D GDBM=0 /D BSD2=1 /D "DEBUG" /D "_DEBUG" /D "THREAD_SAFE" /D\
 "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\dbm.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib libdb.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /version:2 /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\php3_dbm.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_dbm.dll" /implib:"$(OUTDIR)\php3_dbm.lib" /pdbtype:sept\
 /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\db.obj" \
	"$(INTDIR)\flock.obj"

"$(OUTDIR)\php3_dbm.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "dbm - Win32 Release" || "$(CFG)" == "dbm - Win32 Debug"
SOURCE=.\functions\db.c

!IF  "$(CFG)" == "dbm - Win32 Release"

DEP_CPP_DB_C0=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\FlexSafe.h"\
	".\fopen-wrappers.h"\
	".\functions\db.h"\
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
	".\win32\flock.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DB_C0=\
	".\ap_compat.h"\
	".\ap_config.h"\
	".\compat.h"\
	".\config.h"\
	".\http_config.h"\
	".\http_core.h"\
	".\http_log.h"\
	".\http_main.h"\
	".\http_protocol.h"\
	".\http_request.h"\
	".\httpd.h"\
	

"$(INTDIR)\db.obj" : $(SOURCE) $(DEP_CPP_DB_C0) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dbm - Win32 Debug"

DEP_CPP_DB_C0=\
	"..\..\include\db.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\db.h"\
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
	".\win32\flock.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\db.obj" : $(SOURCE) $(DEP_CPP_DB_C0) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\flock.c
DEP_CPP_FLOCK=\
	".\win32\flock.h"\
	

"$(INTDIR)\flock.obj" : $(SOURCE) $(DEP_CPP_FLOCK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

