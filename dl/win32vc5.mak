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
!MESSAGE You will need regex.lib.  Also, be sure to edit this makefile
!MESSAGE and change the include directories to be correct for you
!MESSAGE Setup!!!
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

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\calendar.dll"

!ELSE 

ALL : "$(OUTDIR)\calendar.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\calendar.obj"
	-@erase "$(INTDIR)\dow.obj"
	-@erase "$(INTDIR)\french.obj"
	-@erase "$(INTDIR)\gregor.obj"
	-@erase "$(INTDIR)\jewish.obj"
	-@erase "$(INTDIR)\julian.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\calendar.dll"
	-@erase "$(OUTDIR)\calendar.exp"
	-@erase "$(OUTDIR)\calendar.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\\" /I "c:\usr\src\include" /D "NDEBUG"\
 /D "MSVC5" /D "WIN32" /D "_WINDOWS" /Fp"$(INTDIR)\calendar.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calendar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\calendar.pdb" /machine:I386 /out:"$(OUTDIR)\calendar.dll"\
 /implib:"$(OUTDIR)\calendar.lib" 
LINK32_OBJS= \
	"$(INTDIR)\calendar.obj" \
	"$(INTDIR)\dow.obj" \
	"$(INTDIR)\french.obj" \
	"$(INTDIR)\gregor.obj" \
	"$(INTDIR)\jewish.obj" \
	"$(INTDIR)\julian.obj"

"$(OUTDIR)\calendar.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\calendar.dll"

!ELSE 

ALL : "$(OUTDIR)\calendar.dll"

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
	-@erase "$(OUTDIR)\calendar.dll"
	-@erase "$(OUTDIR)\calendar.exp"
	-@erase "$(OUTDIR)\calendar.ilk"
	-@erase "$(OUTDIR)\calendar.lib"
	-@erase "$(OUTDIR)\calendar.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "..\\" /I "c:\usr\src\include" /D\
 "DEBUG" /D "_DEBUG" /D "MSVC5" /D "WIN32" /D "_WINDOWS"\
 /Fp"$(INTDIR)\calendar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\calendar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\calendar.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\calendar.dll" /implib:"$(OUTDIR)\calendar.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\calendar.obj" \
	"$(INTDIR)\dow.obj" \
	"$(INTDIR)\french.obj" \
	"$(INTDIR)\gregor.obj" \
	"$(INTDIR)\jewish.obj" \
	"$(INTDIR)\julian.obj"

"$(OUTDIR)\calendar.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
SOURCE=.\calendar.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_CALEN=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\calendar.obj" : $(SOURCE) $(DEP_CPP_CALEN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_CALEN=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\calendar.obj" : $(SOURCE) $(DEP_CPP_CALEN) "$(INTDIR)"


!ENDIF 

SOURCE=.\dow.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_DOW_C=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\dow.obj" : $(SOURCE) $(DEP_CPP_DOW_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_DOW_C=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\dow.obj" : $(SOURCE) $(DEP_CPP_DOW_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\french.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_FRENC=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\french.obj" : $(SOURCE) $(DEP_CPP_FRENC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_FRENC=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\french.obj" : $(SOURCE) $(DEP_CPP_FRENC) "$(INTDIR)"


!ENDIF 

SOURCE=.\gregor.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_GREGO=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\gregor.obj" : $(SOURCE) $(DEP_CPP_GREGO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_GREGO=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\gregor.obj" : $(SOURCE) $(DEP_CPP_GREGO) "$(INTDIR)"


!ENDIF 

SOURCE=.\jewish.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_JEWIS=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\jewish.obj" : $(SOURCE) $(DEP_CPP_JEWIS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_JEWIS=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\jewish.obj" : $(SOURCE) $(DEP_CPP_JEWIS) "$(INTDIR)"


!ENDIF 

SOURCE=.\julian.c

!IF  "$(CFG)" == "calendar - Win32 Release"

DEP_CPP_JULIA=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\julian.obj" : $(SOURCE) $(DEP_CPP_JULIA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "calendar - Win32 Debug"

DEP_CPP_JULIA=\
	"..\..\..\include\regex.h"\
	"..\..\..\include\regexst.h"\
	"..\alloc.h"\
	"..\build-defs.h"\
	"..\config.w32.h"\
	"..\environment.h"\
	"..\hash.h"\
	"..\operators.h"\
	"..\parser.h"\
	"..\stack.h"\
	"..\token_cache.h"\
	"..\variables.h"\
	"..\win32\getini.h"\
	"..\win95nt.h"\
	".\sdncal.h"\
	

"$(INTDIR)\julian.obj" : $(SOURCE) $(DEP_CPP_JULIA) "$(INTDIR)"


!ENDIF 


!ENDIF 

