# Microsoft Developer Studio Generated NMAKE File, Based on dbfcreate.dsp
!IF "$(CFG)" == ""
CFG=dbfcreate - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dbfcreate - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dbfcreate - Win32 Release" && "$(CFG)" !=\
 "dbfcreate - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbfcreate.mak" CFG="dbfcreate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbfcreate - Win32 Release" (based on\
 "Win32 (x86) Console Application")
!MESSAGE "dbfcreate - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "dbfcreate - Win32 Release"

OUTDIR=.\dbfcreat
INTDIR=.\dbfcreat
# Begin Custom Macros
OutDir=.\dbfcreat
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbfcreate.exe"

!ELSE 

ALL : "$(OUTDIR)\dbfcreate.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\dbfcreat.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\dbfcreate.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "MSVC5" /D "WIN32" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\dbfcreate.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\dbfcreat/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbfcreate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\dbfcreate.pdb" /machine:I386 /out:"$(OUTDIR)\dbfcreate.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj" \
	"$(INTDIR)\dbfcreat.obj" \
	"$(INTDIR)\getopt.obj"

"$(OUTDIR)\dbfcreate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbfcreate - Win32 Debug"

OUTDIR=.\dbfcrea0
INTDIR=.\dbfcrea0
# Begin Custom Macros
OutDir=.\dbfcrea0
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbfcreate.exe"

!ELSE 

ALL : "$(OUTDIR)\dbfcreate.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\dbfcreat.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\dbfcreate.exe"
	-@erase "$(OUTDIR)\dbfcreate.ilk"
	-@erase "$(OUTDIR)\dbfcreate.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "MSVC5" /D "WIN32" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\dbfcreate.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\dbfcrea0/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbfcreate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\dbfcreate.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)\dbfcreate.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj" \
	"$(INTDIR)\dbfcreat.obj" \
	"$(INTDIR)\getopt.obj"

"$(OUTDIR)\dbfcreate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "dbfcreate - Win32 Release" || "$(CFG)" ==\
 "dbfcreate - Win32 Debug"
SOURCE=.\dbf_head.c

!IF  "$(CFG)" == "dbfcreate - Win32 Release"

DEP_CPP_DBF_H=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_head.obj" : $(SOURCE) $(DEP_CPP_DBF_H) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfcreate - Win32 Debug"

DEP_CPP_DBF_H=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_head.obj" : $(SOURCE) $(DEP_CPP_DBF_H) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_misc.c

!IF  "$(CFG)" == "dbfcreate - Win32 Release"


"$(INTDIR)\dbf_misc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfcreate - Win32 Debug"

DEP_CPP_DBF_M=\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\dbf_misc.obj" : $(SOURCE) $(DEP_CPP_DBF_M) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_ndx.c

!IF  "$(CFG)" == "dbfcreate - Win32 Release"

DEP_CPP_DBF_N=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfcreate - Win32 Debug"

DEP_CPP_DBF_N=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_rec.c

!IF  "$(CFG)" == "dbfcreate - Win32 Release"

DEP_CPP_DBF_R=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) $(DEP_CPP_DBF_R) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfcreate - Win32 Debug"

DEP_CPP_DBF_R=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) $(DEP_CPP_DBF_R) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbfcreat.c

!IF  "$(CFG)" == "dbfcreate - Win32 Release"

DEP_CPP_DBFCR=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbfcreat.obj" : $(SOURCE) $(DEP_CPP_DBFCR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfcreate - Win32 Debug"

DEP_CPP_DBFCR=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbfcreat.obj" : $(SOURCE) $(DEP_CPP_DBFCR) "$(INTDIR)"


!ENDIF 

SOURCE=.\getopt.c

"$(INTDIR)\getopt.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

