# Microsoft Developer Studio Generated NMAKE File, Based on dbfndx.dsp
!IF "$(CFG)" == ""
CFG=dbfndx - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dbfndx - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dbfndx - Win32 Release" && "$(CFG)" != "dbfndx - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dbfndx.mak" CFG="dbfndx - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dbfndx - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dbfndx - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "dbfndx - Win32 Release"

OUTDIR=.\dbfndx__
INTDIR=.\dbfndx__
# Begin Custom Macros
OutDir=.\dbfndx__
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbfndx.exe"

!ELSE 

ALL : "$(OUTDIR)\dbfndx.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\dbfndx.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\dbfndx.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "NDEBUG" /D "MSVC5" /D "WIN32" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\dbfndx.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\dbfndx__/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbfndx.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)\dbfndx.pdb" /machine:I386 /out:"$(OUTDIR)\dbfndx.exe" 
LINK32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj" \
	"$(INTDIR)\dbfndx.obj" \
	"$(INTDIR)\getopt.obj"

"$(OUTDIR)\dbfndx.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbfndx - Win32 Debug"

OUTDIR=.\dbfndx_0
INTDIR=.\dbfndx_0
# Begin Custom Macros
OutDir=.\dbfndx_0
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbfndx.exe"

!ELSE 

ALL : "$(OUTDIR)\dbfndx.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\dbfndx.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\dbfndx.exe"
	-@erase "$(OUTDIR)\dbfndx.ilk"
	-@erase "$(OUTDIR)\dbfndx.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "MSVC5" /D "WIN32" /D\
 "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\dbfndx.pch" /YX /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\dbfndx_0/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbfndx.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)\dbfndx.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dbfndx.exe"\
 /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj" \
	"$(INTDIR)\dbfndx.obj" \
	"$(INTDIR)\getopt.obj"

"$(OUTDIR)\dbfndx.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "dbfndx - Win32 Release" || "$(CFG)" == "dbfndx - Win32 Debug"
SOURCE=.\dbf_head.c

!IF  "$(CFG)" == "dbfndx - Win32 Release"

DEP_CPP_DBF_H=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_head.obj" : $(SOURCE) $(DEP_CPP_DBF_H) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfndx - Win32 Debug"

DEP_CPP_DBF_H=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_head.obj" : $(SOURCE) $(DEP_CPP_DBF_H) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_misc.c

!IF  "$(CFG)" == "dbfndx - Win32 Release"


"$(INTDIR)\dbf_misc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfndx - Win32 Debug"

DEP_CPP_DBF_M=\
	{$(INCLUDE)}"sys\types.h"\
	

"$(INTDIR)\dbf_misc.obj" : $(SOURCE) $(DEP_CPP_DBF_M) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_ndx.c

!IF  "$(CFG)" == "dbfndx - Win32 Release"

DEP_CPP_DBF_N=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfndx - Win32 Debug"

DEP_CPP_DBF_N=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbf_rec.c

!IF  "$(CFG)" == "dbfndx - Win32 Release"

DEP_CPP_DBF_R=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) $(DEP_CPP_DBF_R) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfndx - Win32 Debug"

DEP_CPP_DBF_R=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_rec.obj" : $(SOURCE) $(DEP_CPP_DBF_R) "$(INTDIR)"


!ENDIF 

SOURCE=.\dbfndx.c

!IF  "$(CFG)" == "dbfndx - Win32 Release"

DEP_CPP_DBFND=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbfndx.obj" : $(SOURCE) $(DEP_CPP_DBFND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbfndx - Win32 Debug"

DEP_CPP_DBFND=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbfndx.obj" : $(SOURCE) $(DEP_CPP_DBFND) "$(INTDIR)"


!ENDIF 

SOURCE=.\getopt.c

"$(INTDIR)\getopt.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

