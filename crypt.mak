# Microsoft Developer Studio Generated NMAKE File, Based on crypt.dsp
!IF "$(CFG)" == ""
CFG=crypt - Win32 Debug
!MESSAGE No configuration specified. Defaulting to crypt - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "crypt - Win32 Release" && "$(CFG)" != "crypt - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "crypt.mak" CFG="crypt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "crypt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "crypt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "crypt - Win32 Release"

OUTDIR=.\module_Release
INTDIR=.\module_Release
# Begin Custom Macros
OutDir=.\module_Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_crypt.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_crypt.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\sflcryp.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\php3_crypt.dll"
	-@erase "$(OUTDIR)\php3_crypt.exp"
	-@erase "$(OUTDIR)\php3_crypt.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /D HAVE_SFLCRYPT=1 /D\
 "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\crypt.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\crypt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\php3_crypt.pdb" /machine:I386 /out:"$(OUTDIR)\php3_crypt.dll"\
 /implib:"$(OUTDIR)\php3_crypt.lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\sflcryp.obj"

"$(OUTDIR)\php3_crypt.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "crypt - Win32 Debug"

OUTDIR=.\module_debug
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=.\module_debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_crypt.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_crypt.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\crypt.obj"
	-@erase "$(INTDIR)\sflcryp.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\php3_crypt.dll"
	-@erase "$(OUTDIR)\php3_crypt.exp"
	-@erase "$(OUTDIR)\php3_crypt.ilk"
	-@erase "$(OUTDIR)\php3_crypt.lib"
	-@erase "$(OUTDIR)\php3_crypt.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /I "../../" /D\
 HAVE_SFLCRYPT=1 /D "DEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /Fp"$(INTDIR)\crypt.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\module_debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\crypt.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\php3_crypt.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_crypt.dll" /implib:"$(OUTDIR)\php3_crypt.lib"\
 /pdbtype:sept /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\crypt.obj" \
	"$(INTDIR)\sflcryp.obj"

"$(OUTDIR)\php3_crypt.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "crypt - Win32 Release" || "$(CFG)" == "crypt - Win32 Debug"
SOURCE=.\dl\crypt\crypt.c

!IF  "$(CFG)" == "crypt - Win32 Release"

DEP_CPP_CRYPT=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\crypt\prelude.h"\
	".\dl\crypt\sflcryp.h"\
	".\dl\phpdl.h"\
	".\FlexSafe.h"\
	".\fopen-wrappers.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
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
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CRYPT=\
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
	

"$(INTDIR)\crypt.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "crypt - Win32 Debug"

DEP_CPP_CRYPT=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\crypt\prelude.h"\
	".\dl\crypt\sflcryp.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
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
	

"$(INTDIR)\crypt.obj" : $(SOURCE) $(DEP_CPP_CRYPT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dl\crypt\sflcryp.c

!IF  "$(CFG)" == "crypt - Win32 Release"

DEP_CPP_SFLCR=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\crypt\prelude.h"\
	".\dl\crypt\sflcryp.h"\
	".\FlexSafe.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
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
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_SFLCR=\
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
	

"$(INTDIR)\sflcryp.obj" : $(SOURCE) $(DEP_CPP_SFLCR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "crypt - Win32 Debug"

DEP_CPP_SFLCR=\
	".\dl\crypt\prelude.h"\
	".\dl\crypt\sflcryp.h"\
	

"$(INTDIR)\sflcryp.obj" : $(SOURCE) $(DEP_CPP_SFLCR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

