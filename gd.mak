# Microsoft Developer Studio Generated NMAKE File, Based on gd.dsp
!IF "$(CFG)" == ""
CFG=gd - Win32 Debug
!MESSAGE No configuration specified. Defaulting to gd - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "gd - Win32 Release" && "$(CFG)" != "gd - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gd.mak" CFG="gd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gd - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gd - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "gd - Win32 Release"

OUTDIR=.\module_Release
INTDIR=.\module_Release
# Begin Custom Macros
OutDir=.\module_Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_gd.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_gd.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\gd.obj"
	-@erase "$(INTDIR)\gdcache.obj"
	-@erase "$(INTDIR)\gdttf.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\php3_gd.dll"
	-@erase "$(OUTDIR)\php3_gd.exp"
	-@erase "$(OUTDIR)\php3_gd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D\
 HAVE_LIBGD=1 /D HAVE_LIBGD13=1 /D HAVE_LIBTTF=1 /D "NDEBUG" /D "COMPILE_DL" /D\
 "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\gd.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=freetype.lib php.lib libgd.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\php3_gd.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_gd.dll" /implib:"$(OUTDIR)\php3_gd.lib"\
 /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\gd.obj" \
	"$(INTDIR)\gdcache.obj" \
	"$(INTDIR)\gdttf.obj"

"$(OUTDIR)\php3_gd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gd - Win32 Debug"

OUTDIR=.\module_Debug
INTDIR=.\module_Debug
# Begin Custom Macros
OutDir=.\module_Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_gd.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_gd.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\gd.obj"
	-@erase "$(INTDIR)\gdcache.obj"
	-@erase "$(INTDIR)\gdttf.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\php3_gd.dll"
	-@erase "$(OUTDIR)\php3_gd.exp"
	-@erase "$(OUTDIR)\php3_gd.ilk"
	-@erase "$(OUTDIR)\php3_gd.lib"
	-@erase "$(OUTDIR)\php3_gd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /I "../../include"\
 /D HAVE_LIBGD=1 /D HAVE_LIBGD13=1 /D HAVE_LIBTTF=1 /D "DEBUG" /D "_DEBUG" /D\
 "HAVE_GDTTF" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)\gd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=freetype.lib php.lib libgd.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\php3_gd.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_gd.dll" /implib:"$(OUTDIR)\php3_gd.lib" /pdbtype:sept\
 /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\gd.obj" \
	"$(INTDIR)\gdcache.obj" \
	"$(INTDIR)\gdttf.obj"

"$(OUTDIR)\php3_gd.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "gd - Win32 Release" || "$(CFG)" == "gd - Win32 Debug"
SOURCE=.\functions\gd.c

!IF  "$(CFG)" == "gd - Win32 Release"

DEP_CPP_GD_C0=\
	"..\..\include\gd.h"\
	"..\..\include\gdfontg.h"\
	"..\..\include\gdfontl.h"\
	"..\..\include\gdfontmb.h"\
	"..\..\include\gdfonts.h"\
	"..\..\include\gdfontt.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\FlexSafe.h"\
	".\fopen-wrappers.h"\
	".\functions\gdttf.h"\
	".\functions\head.h"\
	".\functions\number.h"\
	".\functions\php3_gd.h"\
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
	
NODEP_CPP_GD_C0=\
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
	

"$(INTDIR)\gd.obj" : $(SOURCE) $(DEP_CPP_GD_C0) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gd - Win32 Debug"

DEP_CPP_GD_C0=\
	"..\..\include\gd.h"\
	"..\..\include\gdfontg.h"\
	"..\..\include\gdfontl.h"\
	"..\..\include\gdfontmb.h"\
	"..\..\include\gdfonts.h"\
	"..\..\include\gdfontt.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\gdttf.h"\
	".\functions\head.h"\
	".\functions\php3_gd.h"\
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
	

"$(INTDIR)\gd.obj" : $(SOURCE) $(DEP_CPP_GD_C0) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\gdcache.c

!IF  "$(CFG)" == "gd - Win32 Release"

DEP_CPP_GDCAC=\
	".\functions\gdcache.h"\
	
NODEP_CPP_GDCAC=\
	".\functions\config.h"\
	

"$(INTDIR)\gdcache.obj" : $(SOURCE) $(DEP_CPP_GDCAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gd - Win32 Debug"

DEP_CPP_GDCAC=\
	".\functions\gdcache.h"\
	

"$(INTDIR)\gdcache.obj" : $(SOURCE) $(DEP_CPP_GDCAC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\gdttf.c

!IF  "$(CFG)" == "gd - Win32 Release"

DEP_CPP_GDTTF=\
	"..\..\include\freetype.h"\
	"..\..\include\fterrid.h"\
	"..\..\include\ftnameid.h"\
	"..\..\include\gd.h"\
	".\config.w32.h"\
	".\functions\gdcache.h"\
	".\functions\gdttf.h"\
	
NODEP_CPP_GDTTF=\
	".\functions\config.h"\
	

"$(INTDIR)\gdttf.obj" : $(SOURCE) $(DEP_CPP_GDTTF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gd - Win32 Debug"

DEP_CPP_GDTTF=\
	"..\..\include\freetype.h"\
	"..\..\include\fterrid.h"\
	"..\..\include\ftnameid.h"\
	"..\..\include\gd.h"\
	".\config.w32.h"\
	".\functions\gdcache.h"\
	".\functions\gdttf.h"\
	

"$(INTDIR)\gdttf.obj" : $(SOURCE) $(DEP_CPP_GDTTF) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

