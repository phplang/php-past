# Microsoft Developer Studio Project File - Name="php_custom_build" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=php_custom_build - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php_custom_build.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php_custom_build.mak" CFG="php_custom_build - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php_custom_build - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "php_custom_build - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "cgi_release"
# PROP Intermediate_Dir "cgi_release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "cgi_debug"
# PROP Intermediate_Dir "cgi_debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "php_custom_build - Win32 Release"
# Name "php_custom_build - Win32 Debug"
# Begin Source File

SOURCE="..\configuration-parser.y"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# Begin Custom Build
InputPath="..\configuration-parser.y"

"../configuration-parser.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set BISON_SIMPLE=bin\bison.simple 
	bin\bison -d -p cfg ../configuration-parser.y -o ../configuration-parser.tab.c 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# Begin Custom Build
InputPath="..\configuration-parser.y"

"../configuration-parser.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	set BISON_SIMPLE=bin\bison.simple 
	bin\bison -d -p cfg ../configuration-parser.y -o ../configuration-parser.tab.c 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\configuration-scanner.lex"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# Begin Custom Build
InputPath="..\configuration-scanner.lex"

"..\configuration-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bin\flex -Pcfg -o../configuration-scanner.c -i ../configuration-scanner.lex

# End Custom Build

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# Begin Custom Build
InputPath="..\configuration-scanner.lex"

"..\configuration-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bin\flex -Pcfg -o../configuration-scanner.c -i ../configuration-scanner.lex

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\language-parser.y"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# Begin Custom Build - Build Language Bison Parser
InputPath="..\language-parser.y"

BuildCmds= \
	set BISON_SIMPLE=bin\bison.simple \
	bin\bison -v -d -p php ../language-parser.y -o ../language-parser.tab.c \
	

"..\language-parser.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\language-parser.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# Begin Custom Build - Performing Custom Build Step on $(InputPath)
InputPath="..\language-parser.y"

BuildCmds= \
	set BISON_SIMPLE=bin\bison.simple \
	bin\bison -v -d -p php ../language-parser.y -o ../language-parser.tab.c \
	

"..\language-parser.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\language-parser.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\language-scanner.lex"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# PROP Intermediate_Dir "./"
# Begin Custom Build
InputPath="..\language-scanner.lex"

"..\language-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bin\flex -Pphp -o../language-scanner.c -i ../language-scanner.lex

# End Custom Build

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# PROP Intermediate_Dir "./"
# Begin Custom Build
InputPath="..\language-scanner.lex"

"..\language-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bin\flex -Pphp -o../language-scanner.c -i ../language-scanner.lex

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\functions\parsedate.y

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# Begin Custom Build
InputPath=..\functions\parsedate.y

BuildCmds= \
	set BISON_SIMPLE=bin\bison.simple \
	bin\bison -v -d ../functions/parsedate.y -o ../functions/parsedate.tab.c \
	

"../functions/parsedate.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"../functions/parsedate.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# Begin Custom Build
InputPath=..\functions\parsedate.y

BuildCmds= \
	set BISON_SIMPLE=bin\bison.simple \
	bin\bison -v -d ../functions/parsedate.y -o ../functions/parsedate.tab.c \
	

"../functions/parsedate.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"../functions/parsedate.tab.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
