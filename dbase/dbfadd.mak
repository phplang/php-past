# Microsoft Developer Studio Generated NMAKE File, Based on dbfadd.dsp
!IF "$(CFG)" == ""
CFG=dbfadd - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dbfadd - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dbfadd - Win32 Release" && "$(CFG)" != "dbfadd - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbfadd.mak" CFG="dbfadd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbfadd - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dbfadd - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dbfadd - Win32 Release"

OUTDIR=.\dbfadd__
INTDIR=.\dbfadd__
# Begin Custom Macros
OutDir=.\dbfadd__
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbfadd.exe"

!ELSE 

ALL : "$(OUTDIR)\dbfadd.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\dbfadd.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\dbfadd.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "MSVC5" /D "WIN32" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\dbfadd.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\dbfadd__/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbfadd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\dbfadd.pdb" /machine:I386 /out:"$(OUTDIR)\dbfadd.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj" \
	"$(INTDIR)\dbfadd.obj"

"$(OUTDIR)\dbfadd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbfadd - Win32 Debug"

OUTDIR=.\dbfadd_0
INTDIR=.\dbfadd_0
# Begin Custom Macros
OutDir=.\dbfadd_0
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbfadd.exe"

!ELSE 

ALL : "$(OUTDIR)\dbfadd.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\dbfadd.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\dbfadd.exe"
	-@erase "$(OUTDIR)\dbfadd.ilk"
	-@erase "$(OUTDIR)\dbfadd.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "MSVC5" /D "WIN32" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\dbfadd.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\dbfadd_0/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbfadd.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\dbfadd.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dbfadd.exe"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj" \
	"$(INTDIR)\dbfadd.obj"

"$(OUTDIR)\dbfadd.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "dbfadd - Win32 Release" || "$(CFG)" == "dbfadd - Win32 Debug"
SOURCE=.\dbf_head.c
DEP_CPP_DBF_H=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_head.obj" : $(SOURCE) $(DEP_CPP_DBF_H) "$(INTDIR)"


SOURCE=.\dbf_misc.c

"$(INTDIR)\dbf_misc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\dbf_ndx.c

!IF  "$(CFG)" == "dbfadd - Win32 Release"

DEP_CPP_DBF_N=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfadd - Win32 Debug"

DEP_CPP_DBF_N=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_rec.c
DEP_CPP_DBF_R=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) $(DEP_CPP_DBF_R) "$(INTDIR)"


SOURCE=.\dbfadd.c
DEP_CPP_DBFAD=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbfadd.obj" : $(SOURCE) $(DEP_CPP_DBFAD) "$(INTDIR)"



!ENDIF 

