# Microsoft Developer Studio Generated NMAKE File, Based on msql.dsp
!IF "$(CFG)" == ""
CFG=msql - Win32 Debug
!MESSAGE No configuration specified. Defaulting to msql - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "msql - Win32 Release" && "$(CFG)" != "msql - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "msql.mak" CFG="msql - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "msql - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "msql - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "msql - Win32 Release"

OUTDIR=.\module_release
INTDIR=.\module_release
# Begin Custom Macros
OutDir=.\module_release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_msql2.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_msql2.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\msql.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\php3_msql2.dll"
	-@erase "$(OUTDIR)\php3_msql2.exp"
	-@erase "$(OUTDIR)\php3_msql2.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D\
 HAVE_MSQL=1 /D "NDEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)\msql.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\msql.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib mSQL_dll.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /version:2.1 /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\php3_msql2.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_msql2.dll" /implib:"$(OUTDIR)\php3_msql2.lib"\
 /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\msql.obj"

"$(OUTDIR)\php3_msql2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "msql - Win32 Debug"

OUTDIR=.\module_debug
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=.\module_debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_msql.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_msql.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\msql.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\php3_msql.dll"
	-@erase "$(OUTDIR)\php3_msql.exp"
	-@erase "$(OUTDIR)\php3_msql.ilk"
	-@erase "$(OUTDIR)\php3_msql.lib"
	-@erase "$(OUTDIR)\php3_msql.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /I "../../include"\
 /D HAVE_MSQL=1 /D "DEBUG" /D "_DEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D\
 "_WINDOWS" /Fp"$(INTDIR)\msql.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c\
 
CPP_OBJS=.\module_debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\msql.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib mSQL_dll.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /version:2.1 /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\php3_msql.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_msql.dll" /implib:"$(OUTDIR)\php3_msql.lib" /pdbtype:sept\
 /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\msql.obj"

"$(OUTDIR)\php3_msql.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "msql - Win32 Release" || "$(CFG)" == "msql - Win32 Debug"
SOURCE=.\functions\msql.c

!IF  "$(CFG)" == "msql - Win32 Release"

DEP_CPP_MSQL_=\
	"..\..\include\msql.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\dl.h"\
	".\functions\php3_msql.h"\
	".\functions\php3_string.h"\
	".\functions\reg.h"\
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
	

"$(INTDIR)\msql.obj" : $(SOURCE) $(DEP_CPP_MSQL_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "msql - Win32 Debug"

DEP_CPP_MSQL_=\
	"..\..\include\msql.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\dl.h"\
	".\functions\php3_msql.h"\
	".\functions\php3_string.h"\
	".\functions\reg.h"\
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
	

"$(INTDIR)\msql.obj" : $(SOURCE) $(DEP_CPP_MSQL_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

