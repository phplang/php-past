# Microsoft Developer Studio Generated NMAKE File, Based on zlib.dsp
!IF "$(CFG)" == ""
CFG=zlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to zlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "zlib - Win32 Release" && "$(CFG)" != "zlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "zlib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "zlib - Win32 Release"

OUTDIR=.\module_Release
INTDIR=.\module_Release
# Begin Custom Macros
OutDir=.\module_Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_zlib.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_zlib.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\zlib.obj"
	-@erase "$(OUTDIR)\php3_zlib.dll"
	-@erase "$(OUTDIR)\php3_zlib.exp"
	-@erase "$(OUTDIR)\php3_zlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D\
 HAVE_ZLIB=1 /D "NDEBUG" /D "MSVC5" /D "COMPILE_DL" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)\zlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=zlibstat.lib php.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\php3_zlib.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_zlib.dll" /implib:"$(OUTDIR)\php3_zlib.lib"\
 /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\zlib.obj"

"$(OUTDIR)\php3_zlib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

OUTDIR=.\module_Debug
INTDIR=.\module_Debug
# Begin Custom Macros
OutDir=.\module_Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_zlib.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_zlib.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\zlib.obj"
	-@erase "$(OUTDIR)\php3_zlib.dll"
	-@erase "$(OUTDIR)\php3_zlib.exp"
	-@erase "$(OUTDIR)\php3_zlib.ilk"
	-@erase "$(OUTDIR)\php3_zlib.lib"
	-@erase "$(OUTDIR)\php3_zlib.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /I "../../include"\
 /D HAVE_ZLIB=1 /D "DEBUG" /D "_DEBUG" /D "MSVC5" /D "COMPILE_DL" /D\
 "THREAD_SAFE" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\zlib.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\zlib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib zlibstat.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\php3_zlib.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_zlib.dll" /implib:"$(OUTDIR)\php3_zlib.lib" /pdbtype:sept\
 /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\zlib.obj"

"$(OUTDIR)\php3_zlib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "zlib - Win32 Release" || "$(CFG)" == "zlib - Win32 Debug"
SOURCE=.\functions\zlib.c

!IF  "$(CFG)" == "zlib - Win32 Release"

DEP_CPP_ZLIB_=\
	"..\..\include\zconf.h"\
	"..\..\include\zlib.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\FlexSafe.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\number.h"\
	".\functions\php3_string.h"\
	".\functions\php3_zlib.h"\
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
	".\serverapi\sapi.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\tls.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\sendmail.h"\
	".\win95nt.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_ZLIB_=\
	".\ap_compat.h"\
	".\ap_config.h"\
	".\compat.h"\
	".\config.h"\
	".\functions\ext\phpdl.h"\
	".\functions\ext\standard\php3_string.h"\
	".\functions\os\nt\param.h"\
	".\http_config.h"\
	".\http_core.h"\
	".\http_log.h"\
	".\http_main.h"\
	".\http_protocol.h"\
	".\http_request.h"\
	".\httpd.h"\
	

"$(INTDIR)\zlib.obj" : $(SOURCE) $(DEP_CPP_ZLIB_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"

DEP_CPP_ZLIB_=\
	"..\..\include\zconf.h"\
	"..\..\include\zlib.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\functions\php3_zlib.h"\
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
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\zlib.obj" : $(SOURCE) $(DEP_CPP_ZLIB_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

