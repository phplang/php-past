# Microsoft Developer Studio Generated NMAKE File, Based on calendar.dsp
!IF "$(CFG)" == ""
CFG=calendar - Win32 Debug
!MESSAGE No configuration specified. Defaulting to calendar - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "calendar - Win32 Release" && "$(CFG)" !=\
 "calendar - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "calendar.mak" CFG="calendar - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "calendar - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "calendar - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "calendar - Win32 Release"

OUTDIR=.\module_release
INTDIR=.\module_release
# Begin Custom Macros
OutDir=.\module_release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_calendar.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_calendar.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\calendar.obj"
	-@erase "$(INTDIR)\dow.obj"
	-@erase "$(INTDIR)\french.obj"
	-@erase "$(INTDIR)\gregor.obj"
	-@erase "$(INTDIR)\jewish.obj"
	-@erase "$(INTDIR)\julian.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\php3_calendar.dll"
	-@erase "$(OUTDIR)\php3_calendar.exp"
	-@erase "$(OUTDIR)\php3_calendar.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /D "NDEBUG" /D "COMPILE_DL"\
 /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\calendar.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calendar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /version:1 /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\php3_calendar.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_calendar.dll" /implib:"$(OUTDIR)\php3_calendar.lib"\
 /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\calendar.obj" \
	"$(INTDIR)\dow.obj" \
	"$(INTDIR)\french.obj" \
	"$(INTDIR)\gregor.obj" \
	"$(INTDIR)\jewish.obj" \
	"$(INTDIR)\julian.obj"

"$(OUTDIR)\php3_calendar.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

OUTDIR=.\module_debug
INTDIR=.\module_debug
# Begin Custom Macros
OutDir=.\module_debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_calendar.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_calendar.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\calendar.obj"
	-@erase "$(INTDIR)\dow.obj"
	-@erase "$(INTDIR)\french.obj"
	-@erase "$(INTDIR)\gregor.obj"
	-@erase "$(INTDIR)\jewish.obj"
	-@erase "$(INTDIR)\julian.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\php3_calendar.dll"
	-@erase "$(OUTDIR)\php3_calendar.exp"
	-@erase "$(OUTDIR)\php3_calendar.ilk"
	-@erase "$(OUTDIR)\php3_calendar.lib"
	-@erase "$(OUTDIR)\php3_calendar.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /D "DEBUG" /D\
 "_DEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)\calendar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calendar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /version:1 /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\php3_calendar.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_calendar.dll" /implib:"$(OUTDIR)\php3_calendar.lib"\
 /pdbtype:sept /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\calendar.obj" \
	"$(INTDIR)\dow.obj" \
	"$(INTDIR)\french.obj" \
	"$(INTDIR)\gregor.obj" \
	"$(INTDIR)\jewish.obj" \
	"$(INTDIR)\julian.obj"

"$(OUTDIR)\php3_calendar.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "calendar - Win32 Release" || "$(CFG)" ==\
 "calendar - Win32 Debug"
SOURCE=.\dl\calendar\calendar.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_CALEN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_CALEN=\
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
	

"$(INTDIR)\calendar.obj" : $(SOURCE) $(DEP_CPP_CALEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_CALEN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	

"$(INTDIR)\calendar.obj" : $(SOURCE) $(DEP_CPP_CALEN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dl\calendar\dow.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_DOW_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_DOW_C=\
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
	

"$(INTDIR)\dow.obj" : $(SOURCE) $(DEP_CPP_DOW_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_DOW_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	

"$(INTDIR)\dow.obj" : $(SOURCE) $(DEP_CPP_DOW_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dl\calendar\french.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_FRENC=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_FRENC=\
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
	

"$(INTDIR)\french.obj" : $(SOURCE) $(DEP_CPP_FRENC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_FRENC=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	

"$(INTDIR)\french.obj" : $(SOURCE) $(DEP_CPP_FRENC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dl\calendar\gregor.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_GREGO=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_GREGO=\
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
	

"$(INTDIR)\gregor.obj" : $(SOURCE) $(DEP_CPP_GREGO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_GREGO=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	

"$(INTDIR)\gregor.obj" : $(SOURCE) $(DEP_CPP_GREGO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dl\calendar\jewish.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_JEWIS=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_JEWIS=\
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
	

"$(INTDIR)\jewish.obj" : $(SOURCE) $(DEP_CPP_JEWIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_JEWIS=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	

"$(INTDIR)\jewish.obj" : $(SOURCE) $(DEP_CPP_JEWIS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dl\calendar\julian.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_JULIA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_JULIA=\
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
	

"$(INTDIR)\julian.obj" : $(SOURCE) $(DEP_CPP_JULIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_JULIA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\calendar\sdncal.h"\
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
	

"$(INTDIR)\julian.obj" : $(SOURCE) $(DEP_CPP_JULIA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

