# Microsoft Developer Studio Generated NMAKE File, Based on imap4.dsp
!IF "$(CFG)" == ""
CFG=imap4 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to imap4 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "imap4 - Win32 Release" && "$(CFG)" != "imap4 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "imap4.mak" CFG="imap4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "imap4 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "imap4 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "imap4 - Win32 Release"

OUTDIR=.\module_release
INTDIR=.\module_release
# Begin Custom Macros
OutDir=.\module_release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_imap4r2.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_imap4r2.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\imap.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\php3_imap4r2.dll"
	-@erase "$(OUTDIR)\php3_imap4r2.exp"
	-@erase "$(OUTDIR)\php3_imap4r2.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "./" /I "../" /I "../../include" /D\
 HAVE_IMAP=1 /D "NDEBUG" /D "MSVC5" /D "THREAD_SAFE" /D "COMPILE_DL" /D "WIN32"\
 /D "_WINDOWS" /Fp"$(INTDIR)\imap4.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\"\
 /FD /c 
CPP_OBJS=.\module_release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\imap4.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=cclient.lib php.lib wsock32.lib winmm.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:no /pdb:"$(OUTDIR)\php3_imap4r2.pdb" /machine:I386\
 /out:"$(OUTDIR)\php3_imap4r2.dll" /implib:"$(OUTDIR)\php3_imap4r2.lib"\
 /libpath:"..\..\lib" /libpath:"cgi_release" 
LINK32_OBJS= \
	"$(INTDIR)\imap.obj"

"$(OUTDIR)\php3_imap4r2.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "imap4 - Win32 Debug"

OUTDIR=.\module_Debug
INTDIR=.\module_Debug
# Begin Custom Macros
OutDir=.\module_Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php3_imap4r1.dll"

!ELSE 

ALL : "$(OUTDIR)\php3_imap4r1.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\imap.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\php3_imap4r1.dll"
	-@erase "$(OUTDIR)\php3_imap4r1.exp"
	-@erase "$(OUTDIR)\php3_imap4r1.ilk"
	-@erase "$(OUTDIR)\php3_imap4r1.lib"
	-@erase "$(OUTDIR)\php3_imap4r1.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "./" /I "../" /I "../../include"\
 /D HAVE_IMAP=1 /D "DEBUG" /D "_DEBUG" /D "MSVC5" /D "THREAD_SAFE" /D\
 "COMPILE_DL" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\imap4.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\module_Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\imap4.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=cclient.lib php.lib wsock32.lib winmm.lib kernel32.lib user32.lib\
 gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib\
 oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll\
 /incremental:yes /pdb:"$(OUTDIR)\php3_imap4r1.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\php3_imap4r1.dll" /implib:"$(OUTDIR)\php3_imap4r1.lib"\
 /pdbtype:sept /libpath:"..\..\lib" /libpath:"cgi_debug" 
LINK32_OBJS= \
	"$(INTDIR)\imap.obj"

"$(OUTDIR)\php3_imap4r1.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "imap4 - Win32 Release" || "$(CFG)" == "imap4 - Win32 Debug"
SOURCE=.\functions\imap.c

!IF  "$(CFG)" == "imap4 - Win32 Release"

DEP_CPP_IMAP_=\
	"..\..\include\linkage.h"\
	"..\..\include\mail.h"\
	"..\..\include\rfc822.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\imap.h"\
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
	

"$(INTDIR)\imap.obj" : $(SOURCE) $(DEP_CPP_IMAP_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "imap4 - Win32 Debug"

DEP_CPP_IMAP_=\
	"..\..\include\linkage.h"\
	"..\..\include\mail.h"\
	"..\..\include\rfc822.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\phpdl.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\imap.h"\
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
	

"$(INTDIR)\imap.obj" : $(SOURCE) $(DEP_CPP_IMAP_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

