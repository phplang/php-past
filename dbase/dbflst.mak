# Microsoft Developer Studio Generated NMAKE File, Based on dbflst.dsp
!IF "$(CFG)" == ""
CFG=dbflst - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dbflst - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dbflst - Win32 Release" && "$(CFG)" != "dbflst - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbflst.mak" CFG="dbflst - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbflst - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dbflst - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "dbflst - Win32 Release"

OUTDIR=.\dbflst__
INTDIR=.\dbflst__
# Begin Custom Macros
OutDir=.\dbflst__
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbflst.exe"

!ELSE 

ALL : "$(OUTDIR)\dbflst.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\dbflst.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\dbflst.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "MSVC5" /D "WIN32" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\dbflst.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\dbflst__/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbflst.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\dbflst.pdb" /machine:I386 /out:"$(OUTDIR)\dbflst.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj" \
	"$(INTDIR)\dbflst.obj" \
	"$(INTDIR)\getopt.obj"

"$(OUTDIR)\dbflst.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbflst - Win32 Debug"

OUTDIR=.\dbflst_0
INTDIR=.\dbflst_0
# Begin Custom Macros
OutDir=.\dbflst_0
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbflst.exe"

!ELSE 

ALL : "$(OUTDIR)\dbflst.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\dbflst.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\dbflst.exe"
	-@erase "$(OUTDIR)\dbflst.ilk"
	-@erase "$(OUTDIR)\dbflst.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "MSVC5" /D "WIN32" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\dbflst.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\dbflst_0/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbflst.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\dbflst.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dbflst.exe"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj" \
	"$(INTDIR)\dbflst.obj" \
	"$(INTDIR)\getopt.obj"

"$(OUTDIR)\dbflst.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "dbflst - Win32 Release" || "$(CFG)" == "dbflst - Win32 Debug"
SOURCE=.\dbf_head.c

!IF  "$(CFG)" == "dbflst - Win32 Release"

DEP_CPP_DBF_H=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_head.obj" : $(SOURCE) $(DEP_CPP_DBF_H) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbflst - Win32 Debug"

DEP_CPP_DBF_H=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_head.obj" : $(SOURCE) $(DEP_CPP_DBF_H) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_misc.c

!IF  "$(CFG)" == "dbflst - Win32 Release"


"$(INTDIR)\dbf_misc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbflst - Win32 Debug"

DEP_CPP_DBF_M=\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\dbf_misc.obj" : $(SOURCE) $(DEP_CPP_DBF_M) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_ndx.c

!IF  "$(CFG)" == "dbflst - Win32 Release"

DEP_CPP_DBF_N=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbflst - Win32 Debug"

DEP_CPP_DBF_N=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_rec.c

!IF  "$(CFG)" == "dbflst - Win32 Release"

DEP_CPP_DBF_R=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) $(DEP_CPP_DBF_R) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbflst - Win32 Debug"

DEP_CPP_DBF_R=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) $(DEP_CPP_DBF_R) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbflst.c

!IF  "$(CFG)" == "dbflst - Win32 Release"

DEP_CPP_DBFLS=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbflst.obj" : $(SOURCE) $(DEP_CPP_DBFLS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbflst - Win32 Debug"

DEP_CPP_DBFLS=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbflst.obj" : $(SOURCE) $(DEP_CPP_DBFLS) "$(INTDIR)"


!ENDIF 

SOURCE=.\getopt.c

"$(INTDIR)\getopt.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

