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
!MESSAGE "dbase - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dbase - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe

!IF  "$(CFG)" == "dbase - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbase.lib"

!ELSE 

ALL : "dbfpack - Win32 Release" "dbfndx - Win32 Release"\
 "dbflst - Win32 Release" "dbfget - Win32 Release" "dbfdel - Win32 Release"\
 "dbfcreate - Win32 Release" "dbftst - Win32 Release" "dbfadd - Win32 Release"\
 "$(OUTDIR)\dbase.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dbfadd - Win32 ReleaseCLEAN" "dbftst - Win32 ReleaseCLEAN"\
 "dbfcreate - Win32 ReleaseCLEAN" "dbfdel - Win32 ReleaseCLEAN"\
 "dbfget - Win32 ReleaseCLEAN" "dbflst - Win32 ReleaseCLEAN"\
 "dbfndx - Win32 ReleaseCLEAN" "dbfpack - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\dbase.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "MSVC5" /D "WIN32" /D\
 "_WINDOWS" /Fp"$(INTDIR)\dbase.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbase.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dbase.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj"

"$(OUTDIR)\dbase.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\dbase.lib"

!ELSE 

ALL : "dbfpack - Win32 Debug" "dbfndx - Win32 Debug" "dbflst - Win32 Debug"\
 "dbfget - Win32 Debug" "dbfdel - Win32 Debug" "dbfcreate - Win32 Debug"\
 "dbftst - Win32 Debug" "dbfadd - Win32 Debug" "$(OUTDIR)\dbase.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"dbfadd - Win32 DebugCLEAN" "dbftst - Win32 DebugCLEAN"\
 "dbfcreate - Win32 DebugCLEAN" "dbfdel - Win32 DebugCLEAN"\
 "dbfget - Win32 DebugCLEAN" "dbflst - Win32 DebugCLEAN"\
 "dbfndx - Win32 DebugCLEAN" "dbfpack - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dbf_head.obj"
	-@erase "$(INTDIR)\dbf_misc.obj"
	-@erase "$(INTDIR)\dbf_ndx.obj"
	-@erase "$(INTDIR)\dbf_rec.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\dbase.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /GX /Z7 /Od /D "_DEBUG" /D "MSVC5" /D "WIN32" /D\
 "_WINDOWS" /Fp"$(INTDIR)\dbase.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dbase.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\dbase.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dbf_head.obj" \
	"$(INTDIR)\dbf_misc.obj" \
	"$(INTDIR)\dbf_ndx.obj" \
	"$(INTDIR)\dbf_rec.obj"

"$(OUTDIR)\dbase.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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

"dbfadd - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfadd.mak CFG="dbfadd - Win32 Release" 
   cd "."

"dbfadd - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfadd.mak CFG="dbfadd - Win32 Release"\
 RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

"dbfadd - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfadd.mak CFG="dbfadd - Win32 Debug" 
   cd "."

"dbfadd - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfadd.mak CFG="dbfadd - Win32 Debug"\
 RECURSE=1 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "dbase - Win32 Release"

"dbftst - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbftst.mak CFG="dbftst - Win32 Release" 
   cd "."

"dbftst - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbftst.mak CFG="dbftst - Win32 Release"\
 RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

"dbftst - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbftst.mak CFG="dbftst - Win32 Debug" 
   cd "."

"dbftst - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbftst.mak CFG="dbftst - Win32 Debug"\
 RECURSE=1 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "dbase - Win32 Release"

"dbfcreate - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfcreate.mak CFG="dbfcreate - Win32 Release" 
   cd "."

"dbfcreate - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfcreate.mak\
 CFG="dbfcreate - Win32 Release" RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

"dbfcreate - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfcreate.mak CFG="dbfcreate - Win32 Debug" 
   cd "."

"dbfcreate - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfcreate.mak CFG="dbfcreate - Win32 Debug"\
 RECURSE=1 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "dbase - Win32 Release"

"dbfdel - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfdel.mak CFG="dbfdel - Win32 Release" 
   cd "."

"dbfdel - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfdel.mak CFG="dbfdel - Win32 Release"\
 RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

"dbfdel - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfdel.mak CFG="dbfdel - Win32 Debug" 
   cd "."

"dbfdel - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfdel.mak CFG="dbfdel - Win32 Debug"\
 RECURSE=1 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "dbase - Win32 Release"

"dbfget - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfget.mak CFG="dbfget - Win32 Release" 
   cd "."

"dbfget - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfget.mak CFG="dbfget - Win32 Release"\
 RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

"dbfget - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfget.mak CFG="dbfget - Win32 Debug" 
   cd "."

"dbfget - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfget.mak CFG="dbfget - Win32 Debug"\
 RECURSE=1 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "dbase - Win32 Release"

"dbflst - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbflst.mak CFG="dbflst - Win32 Release" 
   cd "."

"dbflst - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbflst.mak CFG="dbflst - Win32 Release"\
 RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

"dbflst - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbflst.mak CFG="dbflst - Win32 Debug" 
   cd "."

"dbflst - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbflst.mak CFG="dbflst - Win32 Debug"\
 RECURSE=1 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "dbase - Win32 Release"

"dbfndx - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfndx.mak CFG="dbfndx - Win32 Release" 
   cd "."

"dbfndx - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfndx.mak CFG="dbfndx - Win32 Release"\
 RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

"dbfndx - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfndx.mak CFG="dbfndx - Win32 Debug" 
   cd "."

"dbfndx - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfndx.mak CFG="dbfndx - Win32 Debug"\
 RECURSE=1 
   cd "."

!ENDIF 

!IF  "$(CFG)" == "dbase - Win32 Release"

"dbfpack - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfpack.mak CFG="dbfpack - Win32 Release" 
   cd "."

"dbfpack - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfpack.mak CFG="dbfpack - Win32 Release"\
 RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

"dbfpack - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\dbfpack.mak CFG="dbfpack - Win32 Debug" 
   cd "."

"dbfpack - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\dbfpack.mak CFG="dbfpack - Win32 Debug"\
 RECURSE=1 
   cd "."

!ENDIF 

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

!IF  "$(CFG)" == "dbase - Win32 Release"

DEP_CPP_DBF_N=\
	".\dbf.h"\
	".\dbf_head.h"\
	".\dbf_misc.h"\
	".\dbf_ndx.h"\
	".\dbf_rec.h"\
	

"$(INTDIR)\dbf_ndx.obj" : $(SOURCE) $(DEP_CPP_DBF_N) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dbase - Win32 Debug"

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



!ENDIF 

