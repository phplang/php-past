# Microsoft Developer Studio Generated NMAKE File, Based on php_custom_build.dsp
!IF "$(CFG)" == ""
CFG=php_custom_build - Win32 Debug
!MESSAGE No configuration specified. Defaulting to php_custom_build - Win32\
 Debug.
!ENDIF 

!IF "$(CFG)" != "php_custom_build - Win32 Release" && "$(CFG)" !=\
 "php_custom_build - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php_custom_build.mak" CFG="php_custom_build - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php_custom_build - Win32 Release" (based on\
 "Win32 (x86) Static Library")
!MESSAGE "php_custom_build - Win32 Debug" (based on\
 "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "language-scanner.c" "language-parser.c" "configuration-scanner.c"\
 "configuration-parser.tab.c" 

!ELSE 

ALL : "language-scanner.c" "language-parser.c" "configuration-scanner.c"\
 "configuration-parser.tab.c" 

!ENDIF 

CLEAN :
	-@erase 
	-@erase "configuration-parser.tab.c"
	-@erase "configuration-scanner.c"
	-@erase "language-parser.c"
	-@erase "language-scanner.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\php_custom_build.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\php_custom_build.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\php_custom_build.lib" 
LIB32_OBJS= \
	

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "language-scanner.c" "language-parser.tab.c" "configuration-scanner.c"\
 "configuration-parser.tab.c" 

!ELSE 

ALL : "language-scanner.c" "language-parser.tab.c" "configuration-scanner.c"\
 "configuration-parser.tab.c" 

!ENDIF 

CLEAN :
	-@erase 
	-@erase "configuration-parser.tab.c"
	-@erase "configuration-scanner.c"
	-@erase "language-parser.tab.c"
	-@erase "language-scanner.c"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\php_custom_build.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\php_custom_build.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\php_custom_build.lib" 
LIB32_OBJS= \
	

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


!IF "$(CFG)" == "php_custom_build - Win32 Release" || "$(CFG)" ==\
 "php_custom_build - Win32 Debug"
SOURCE=".\configuration-parser.y"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

InputPath=.\configuration-parser.y

"configuration-parser.tab.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison.exe -p cfg -v -d configuration-parser.y

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

InputPath=.\configuration-parser.y

"configuration-parser.tab.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison.exe -p cfg -v -d configuration-parser.y

!ENDIF 

SOURCE=".\configuration-scanner.lex"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

InputPath=.\configuration-scanner.lex

"configuration-scanner.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex.exe -Pcfg -oconfiguration-scanner.c -i configuration-scanner.lex

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

InputPath=.\configuration-scanner.lex

"configuration-scanner.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex.exe -Pcfg -oconfiguration-scanner.c -i configuration-scanner.lex

!ENDIF 

SOURCE=".\language-parser.y"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

InputPath=.\language-parser.y

"language-parser.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison -p php -v -d language-parser.y

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

InputPath=.\language-parser.y

"language-parser.tab.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison -p php -v -d language-parser.y

!ENDIF 

SOURCE=".\language-scanner.lex"
INTDIR_SRC=.\.
"$(INTDIR_SRC)" :
    if not exist "$(INTDIR_SRC)/$(NULL)" mkdir "$(INTDIR_SRC)"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

InputPath=.\language-scanner.lex

"language-scanner.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -Pphp -olanguage-scanner.c -i language-scanner.lex

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

InputPath=.\language-scanner.lex

"language-scanner.c"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -Pphp -olanguage-scanner.c -i language-scanner.lex

!ENDIF 


!ENDIF 

