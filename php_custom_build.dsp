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
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
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
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
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

SOURCE=".\configuration-parser.y"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=".\configuration-parser.y"

"configuration-parser.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison.exe -p cfg -v -d configuration-parser.y

# End Custom Build

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=".\configuration-parser.y"

"configuration-parser.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison.exe -p cfg -v -d configuration-parser.y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\configuration-scanner.lex"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=".\configuration-scanner.lex"

"configuration-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex.exe -Pcfg -oconfiguration-scanner.c -i configuration-scanner.lex

# End Custom Build

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=".\configuration-scanner.lex"

"configuration-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex.exe -Pcfg -oconfiguration-scanner.c -i configuration-scanner.lex

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\language-parser.y"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# Begin Custom Build - Build Language Bison Parser
InputPath=".\language-parser.y"

"language-parser.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison -p php -v -d language-parser.y

# End Custom Build

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# Begin Custom Build - Build Language Bison Parser
InputPath=".\language-parser.y"

"language-parser.tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison -p php -v -d language-parser.y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\language-scanner.lex"

!IF  "$(CFG)" == "php_custom_build - Win32 Release"

# PROP Intermediate_Dir "./"
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=".\language-scanner.lex"

"language-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -Pphp -olanguage-scanner.c -i language-scanner.lex

# End Custom Build

!ELSEIF  "$(CFG)" == "php_custom_build - Win32 Debug"

# PROP Intermediate_Dir "./"
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=".\language-scanner.lex"

"language-scanner.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -Pphp -olanguage-scanner.c -i language-scanner.lex

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
