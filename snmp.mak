# Microsoft Developer Studio Generated NMAKE File, Based on snmp.dsp
!IF "$(CFG)" == ""
CFG=snmp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to snmp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "snmp - Win32 Release" && "$(CFG)" != "snmp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "snmp.mak" CFG="snmp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "snmp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "snmp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "snmp - Win32 Release"

OUTDIR=.\module_Release
INTDIR=.\module_Release
# Begin Custom Macros
OutDir=.\module_Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_snmp.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_snmp.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\winsnmp.obj"
	-@erase "$(OUTDIR)\php3_snmp.dll"
	-@erase "$(OUTDIR)\php3_snmp.exp"
	-@erase "$(OUTDIR)\php3_snmp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D\
 HAVE_SNMP=1 /D "THREAD_SAFE" /D "NDEBUG" /D "MSVC5" /D "COMPILE_DL" /D "WIN32"\
 /D "_WINDOWS" /Fp"$(INTDIR)\snmp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\module_Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\snmp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib mgmtapi.lib snmpapi.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\php3_snmp.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_snmp.dll" /implib:"$(OUTDIR)\php3_snmp.lib"\
 /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\winsnmp.obj"

"$(OUTDIR)\php3_snmp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "snmp - Win32 Debug"

OUTDIR=.\module_Debug
INTDIR=.\module_Debug
# Begin Custom Macros
OutDir=.\module_Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_snmp.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_snmp.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\winsnmp.obj"
	-@erase "$(OUTDIR)\php3_snmp.dll"
	-@erase "$(OUTDIR)\php3_snmp.exp"
	-@erase "$(OUTDIR)\php3_snmp.ilk"
	-@erase "$(OUTDIR)\php3_snmp.lib"
	-@erase "$(OUTDIR)\php3_snmp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /I "../../" /I\
 "../../../" /I "../../include" /D HAVE_SNMP=1 /D "THREAD_SAFE" /D "DEBUG" /D\
 "_DEBUG" /D "MSVC5" /D COMPILE_DL=1 /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)\snmp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\snmp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib mgmtapi.lib snmpapi.lib kernel32.lib user32.lib gdi32.lib\
 winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib\
 uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\php3_snmp.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_snmp.dll" /implib:"$(OUTDIR)\php3_snmp.lib" /pdbtype:sept\
 /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\winsnmp.obj"

"$(OUTDIR)\php3_snmp.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "snmp - Win32 Release" || "$(CFG)" == "snmp - Win32 Debug"
SOURCE=.\dl\snmp\winsnmp.c

!IF  "$(CFG)" == "snmp - Win32 Release"

DEP_CPP_WINSN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\dl\snmp\php3_snmp.h"\
	".\fopen-wrappers.h"\
	".\functions\dl.h"\
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
	

"$(INTDIR)\winsnmp.obj" : $(SOURCE) $(DEP_CPP_WINSN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "snmp - Win32 Debug"

DEP_CPP_WINSN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\dl\snmp\php3_snmp.h"\
	".\fopen-wrappers.h"\
	".\functions\dl.h"\
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
	

"$(INTDIR)\winsnmp.obj" : $(SOURCE) $(DEP_CPP_WINSN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

