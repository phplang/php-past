# Microsoft Developer Studio Generated NMAKE File, Based on hyperwave.dsp
!IF "$(CFG)" == ""
CFG=hyperwave - Win32 Debug
!MESSAGE No configuration specified. Defaulting to hyperwave - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hyperwave - Win32 Release" && "$(CFG)" !=\
 "hyperwave - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hyperwave.mak" CFG="hyperwave - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hyperwave - Win32 Release" (based on\
 "Win32 (x86) Dynamic-Link Library")
!MESSAGE "hyperwave - Win32 Debug" (based on\
 "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "hyperwave - Win32 Release"

OUTDIR=.\module_Release
INTDIR=.\module_Release
# Begin Custom Macros
OutDir=.\module_Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_hyperwave.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_hyperwave.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dlist.obj"
	-@erase "$(INTDIR)\hg_comm.obj"
	-@erase "$(INTDIR)\hw.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\php3_hyperwave.dll"
	-@erase "$(OUTDIR)\php3_hyperwave.exp"
	-@erase "$(OUTDIR)\php3_hyperwave.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D\
 "HYPERWAVE" /D "NDEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)\hyperwave.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hyperwave.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\php3_hyperwave.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_hyperwave.dll" /implib:"$(OUTDIR)\php3_hyperwave.lib"\
 /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\dlist.obj" \
	"$(INTDIR)\hg_comm.obj" \
	"$(INTDIR)\hw.obj"

"$(OUTDIR)\php3_hyperwave.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hyperwave - Win32 Debug"

OUTDIR=.\module_Debug
INTDIR=.\module_Debug
# Begin Custom Macros
OutDir=.\module_Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_hyperwave.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_hyperwave.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dlist.obj"
	-@erase "$(INTDIR)\hg_comm.obj"
	-@erase "$(INTDIR)\hw.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\php3_hyperwave.dll"
	-@erase "$(OUTDIR)\php3_hyperwave.exp"
	-@erase "$(OUTDIR)\php3_hyperwave.ilk"
	-@erase "$(OUTDIR)\php3_hyperwave.lib"
	-@erase "$(OUTDIR)\php3_hyperwave.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /I "../../include"\
 /D "HYPERWAVE" /D "DEBUG" /D "_DEBUG" /D "COMPILE_DL" /D "MSVC5" /D "WIN32" /D\
 "_WINDOWS" /Fp"$(INTDIR)\hyperwave.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\module_Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hyperwave.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=php.lib wsock32.lib kernel32.lib user32.lib gdi32.lib winspool.lib\
 comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib\
 odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\php3_hyperwave.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_hyperwave.dll" /implib:"$(OUTDIR)\php3_hyperwave.lib"\
 /pdbtype:sept /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\dlist.obj" \
	"$(INTDIR)\hg_comm.obj" \
	"$(INTDIR)\hw.obj"

"$(OUTDIR)\php3_hyperwave.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "hyperwave - Win32 Release" || "$(CFG)" ==\
 "hyperwave - Win32 Debug"
SOURCE=.\functions\dlist.c

!IF  "$(CFG)" == "hyperwave - Win32 Release"

DEP_CPP_DLIST=\
	".\functions\debug.h"\
	".\functions\DList.h"\
	
NODEP_CPP_DLIST=\
	".\functions\config.h"\
	

"$(INTDIR)\dlist.obj" : $(SOURCE) $(DEP_CPP_DLIST) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "hyperwave - Win32 Debug"

DEP_CPP_DLIST=\
	".\functions\debug.h"\
	".\functions\DList.h"\
	

"$(INTDIR)\dlist.obj" : $(SOURCE) $(DEP_CPP_DLIST) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\hg_comm.c

!IF  "$(CFG)" == "hyperwave - Win32 Release"

DEP_CPP_HG_CO=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\FlexSafe.h"\
	".\fopen-wrappers.h"\
	".\functions\DList.h"\
	".\functions\head.h"\
	".\functions\hg_comm.h"\
	".\functions\hw_error.h"\
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
	
NODEP_CPP_HG_CO=\
	".\ap_compat.h"\
	".\ap_config.h"\
	".\compat.h"\
	".\config.h"\
	".\functions\config.h"\
	".\http_config.h"\
	".\http_core.h"\
	".\http_log.h"\
	".\http_main.h"\
	".\http_protocol.h"\
	".\http_request.h"\
	".\httpd.h"\
	

"$(INTDIR)\hg_comm.obj" : $(SOURCE) $(DEP_CPP_HG_CO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "hyperwave - Win32 Debug"

DEP_CPP_HG_CO=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\DList.h"\
	".\functions\head.h"\
	".\functions\hg_comm.h"\
	".\functions\hw_error.h"\
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
	

"$(INTDIR)\hg_comm.obj" : $(SOURCE) $(DEP_CPP_HG_CO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\hw.c

!IF  "$(CFG)" == "hyperwave - Win32 Release"

DEP_CPP_HW_C4=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\FlexSafe.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\hg_comm.h"\
	".\functions\hw.h"\
	".\functions\hw_error.h"\
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
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_HW_C4=\
	".\ap_compat.h"\
	".\ap_config.h"\
	".\compat.h"\
	".\config.h"\
	".\functions\config.h"\
	".\http_config.h"\
	".\http_core.h"\
	".\http_log.h"\
	".\http_main.h"\
	".\http_protocol.h"\
	".\http_request.h"\
	".\httpd.h"\
	

"$(INTDIR)\hw.obj" : $(SOURCE) $(DEP_CPP_HW_C4) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "hyperwave - Win32 Debug"

DEP_CPP_HW_C4=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\hg_comm.h"\
	".\functions\hw.h"\
	".\functions\hw_error.h"\
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
	

"$(INTDIR)\hw.obj" : $(SOURCE) $(DEP_CPP_HW_C4) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

