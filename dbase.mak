# Microsoft Developer Studio Generated NMAKE File, Based on dbase.dsp
!IF "$(CFG)" == ""
CFG=dbase - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dbase - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dbase - Win32 Release" && "$(CFG)" != "dbase - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbase.mak" CFG="dbase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbase - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dbase - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dbase - Win32 Release"

OUTDIR=.\module_release
INTDIR=.\module_release
# Begin Custom Macros
OutDir=.\module_release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_dbase.dll"

!ELSE 

ALL : "php3 - Win32 Release" "$(OUTDIR)\php3_dbase.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"php3 - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dbase.obj"
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\php3_dbase.dll"
	-@erase "$(OUTDIR)\php3_dbase.exp"
	-@erase "$(OUTDIR)\php3_dbase.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "dbase\\" /I "./" /I "../" /D DBASE=1 /D\
 "NDEBUG" /D "THREAD_SAFE" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)\dbase.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /version:3 /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\php3_dbase.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_dbase.dll" /implib:"$(OUTDIR)\php3_dbase.lib"\
 /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\dbase.obj" \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj"

"$(OUTDIR)\php3_dbase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

OUTDIR=.\module_debug
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=.\module_debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_dbase.dll"

!ELSE 

ALL : "php3 - Win32 Debug" "$(OUTDIR)\php3_dbase.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"php3 - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dbase.obj"
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\php3_dbase.dll"
	-@erase "$(OUTDIR)\php3_dbase.exp"
	-@erase "$(OUTDIR)\php3_dbase.ilk"
	-@erase "$(OUTDIR)\php3_dbase.lib"
	-@erase "$(OUTDIR)\php3_dbase.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "dbase\\" /I "./" /I "../" /D\
 DBASE=1 /D "DEBUG" /D "_DEBUG" /D "THREAD_SAFE" /D "COMPILE_DL" /D "MSVC5" /D\
 "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\dbase.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbase.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /version:3 /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\php3_dbase.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_dbase.dll" /implib:"$(OUTDIR)\php3_dbase.lib"\
 /pdbtype:sept /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\dbase.obj" \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj"

"$(OUTDIR)\php3_dbase.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "dbase - Win32 Release" || "$(CFG)" == "dbase - Win32 Debug"

!IF  "$(CFG)" == "dbase - Win32 Release"

"php3 - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\php3.mak CFG="php3 - Win32 Release" 
   cd "."

"php3 - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\php3.mak CFG="php3 - Win32 Release"\
 RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

"php3 - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\php3.mak CFG="php3 - Win32 Debug" 
   cd "."

"php3 - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\php3.mak CFG="php3 - Win32 Debug" RECURSE=1\
 
   cd "."

!ENDIF 

SOURCE=.\functions\dbase.c

!IF  "$(CFG)" == "dbase - Win32 Release"

DEP_CPP_DBASE=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dbase\dbf.h"\
	".\dbase\dbf_head.h"\
	".\dbase\dbf_misc.h"\
	".\dbase\dbf_rec.h"\
	".\dl\phpdl.h"\
	".\FlexSafe.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\number.h"\
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
	
NODEP_CPP_DBASE=\
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
	

"$(INTDIR)\dbase.obj" : $(SOURCE) $(DEP_CPP_DBASE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

DEP_CPP_DBASE=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dbase\dbf.h"\
	".\dbase\dbf_head.h"\
	".\dbase\dbf_misc.h"\
	".\dbase\dbf_rec.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\number.h"\
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
	".\tls.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\sendmail.h"\
	".\win95nt.h"\
	

"$(INTDIR)\dbase.obj" : $(SOURCE) $(DEP_CPP_DBASE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dbase\dbf_head.c

!IF  "$(CFG)" == "dbase - Win32 Release"

DEP_CPP_DBF_H=\
	".\dbase\dbf.h"\
	".\dbase\dbf_head.h"\
	".\dbase\dbf_misc.h"\
	".\dbase\dbf_rec.h"\
	

"$(INTDIR)\dbf_head.obj" : $(SOURCE) $(DEP_CPP_DBF_H) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

DEP_CPP_DBF_H=\
	".\dbase\dbf.h"\
	".\dbase\dbf_head.h"\
	".\dbase\dbf_misc.h"\
	".\dbase\dbf_rec.h"\
	

"$(INTDIR)\dbf_head.obj" : $(SOURCE) $(DEP_CPP_DBF_H) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dbase\dbf_misc.c

!IF  "$(CFG)" == "dbase - Win32 Release"

DEP_CPP_DBF_M=\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\dbf_misc.obj" : $(SOURCE) $(DEP_CPP_DBF_M) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"


"$(INTDIR)\dbf_misc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dbase\dbf_ndx.c

!IF  "$(CFG)" == "dbase - Win32 Release"

DEP_CPP_DBF_N=\
	".\dbase\dbf.h"\
	".\dbase\dbf_head.h"\
	".\dbase\dbf_misc.h"\
	".\dbase\dbf_ndx.h"\
	".\dbase\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

DEP_CPP_DBF_N=\
	".\dbase\dbf.h"\
	".\dbase\dbf_head.h"\
	".\dbase\dbf_misc.h"\
	".\dbase\dbf_ndx.h"\
	".\dbase\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dbase\dbf_rec.c

!IF  "$(CFG)" == "dbase - Win32 Release"

DEP_CPP_DBF_R=\
	".\dbase\dbf.h"\
	".\dbase\dbf_head.h"\
	".\dbase\dbf_misc.h"\
	".\dbase\dbf_rec.h"\
	

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) $(DEP_CPP_DBF_R) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

DEP_CPP_DBF_R=\
	".\dbase\dbf.h"\
	".\dbase\dbf_head.h"\
	".\dbase\dbf_misc.h"\
	".\dbase\dbf_rec.h"\
	

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) $(DEP_CPP_DBF_R) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

