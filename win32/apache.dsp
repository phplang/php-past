# Microsoft Developer Studio Project File - Name="apache" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=apache - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "apache.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "apache.mak" CFG="apache - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "apache - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "apache - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "apache - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "apache_release"
# PROP Intermediate_Dir "apache_release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "\usr\src\phpcvs\php3" /I "regex\\" /I "\usr\src\apache_1.3b3\src\main" /I "\usr\src\apache_1.3b3\src\regex" /D "NDEBUG" /D "APACHE" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ApacheCore.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3.0 /subsystem:windows /dll /machine:I386 /out:"apache_release/php3_mod.dll" /libpath:"\usr\src\lib"

!ELSEIF  "$(CFG)" == "apache - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "apache_Debug"
# PROP Intermediate_Dir "apache_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "\usr\src\phpcvs\php3" /I "regex\\" /I "\usr\src\apache_1.3b3\src\main" /I "\usr\src\apache_1.3b3\src\regex" /D "DEBUG" /D "_DEBUG" /D "APACHE" /D "MSVC5" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ApacheCore.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3.0 /subsystem:windows /dll /debug /machine:I386 /out:"apache_Debug/php3_mod.dll" /pdbtype:sept /libpath:"\usr\src\lib"
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "apache - Win32 Release"
# Name "apache - Win32 Debug"
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\alloc.c
# End Source File
# Begin Source File

SOURCE=..\alloc.h
# End Source File
# Begin Source File

SOURCE=..\config.w32.h
# End Source File
# Begin Source File

SOURCE=".\configuration-parser.tab.c"
# End Source File
# Begin Source File

SOURCE=".\configuration-parser.tab.h"
# End Source File
# Begin Source File

SOURCE=".\configuration-scanner.c"
# End Source File
# Begin Source File

SOURCE=..\control_structures.h
# End Source File
# Begin Source File

SOURCE=..\control_structures_inline.h
# End Source File
# Begin Source File

SOURCE=..\debugger.c
# End Source File
# Begin Source File

SOURCE=..\getopt.c
# End Source File
# Begin Source File

SOURCE=..\getopt.h
# End Source File
# Begin Source File

SOURCE=..\php3_hash.c
# End Source File
# Begin Source File

SOURCE=..\php3_hash.h
# End Source File
# Begin Source File

SOURCE=..\highlight.c
# End Source File
# Begin Source File

SOURCE=..\highlight.h
# End Source File
# Begin Source File

SOURCE=..\internal_functions.c
# End Source File
# Begin Source File

SOURCE=..\internal_functions.h
# End Source File
# Begin Source File

SOURCE=..\internal_functions_registry.h
# End Source File
# Begin Source File

SOURCE=".\language-parser.tab.c"
# End Source File
# Begin Source File

SOURCE=".\language-parser.tab.h"
# End Source File
# Begin Source File

SOURCE=".\language-scanner.c"
# End Source File
# Begin Source File

SOURCE=".\language-scanner.h"
# End Source File
# Begin Source File

SOURCE=..\list.c
# End Source File
# Begin Source File

SOURCE=..\main.c
# End Source File
# Begin Source File

SOURCE=..\main.h
# End Source File
# Begin Source File

SOURCE=..\mod_php3.c
# End Source File
# Begin Source File

SOURCE=..\mod_php3.h
# End Source File
# Begin Source File

SOURCE=..\modules.h
# End Source File
# Begin Source File

SOURCE=..\operators.c
# End Source File
# Begin Source File

SOURCE=..\operators.h
# End Source File
# Begin Source File

SOURCE=..\php.h
# End Source File
# Begin Source File

SOURCE=..\php3_debugger.h
# End Source File
# Begin Source File

SOURCE=..\php3_list.h
# End Source File
# Begin Source File

SOURCE=..\request_info.c
# End Source File
# Begin Source File

SOURCE=..\request_info.h
# End Source File
# Begin Source File

SOURCE=..\safe_mode.c
# End Source File
# Begin Source File

SOURCE=..\safe_mode.h
# End Source File
# Begin Source File

SOURCE=..\stack.c
# End Source File
# Begin Source File

SOURCE=..\stack.h
# End Source File
# Begin Source File

SOURCE=..\token_cache.c
# End Source File
# Begin Source File

SOURCE=..\token_cache.h
# End Source File
# Begin Source File

SOURCE=..\variables.c
# End Source File
# Begin Source File

SOURCE=..\variables.h
# End Source File
# Begin Source File

SOURCE=..\win95nt.h
# End Source File
# End Group
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\win32\grp.h
# End Source File
# Begin Source File

SOURCE=..\win32\param.h
# End Source File
# Begin Source File

SOURCE=..\win32\pwd.c
# End Source File
# Begin Source File

SOURCE=..\win32\pwd.h
# End Source File
# Begin Source File

SOURCE=..\win32\readdir.c
# End Source File
# Begin Source File

SOURCE=..\win32\readdir.h
# End Source File
# Begin Source File

SOURCE=..\win32\sendmail.c
# End Source File
# Begin Source File

SOURCE=..\win32\sendmail.h
# End Source File
# Begin Source File

SOURCE=..\win32\syslog.h
# End Source File
# Begin Source File

SOURCE=..\win32\time.c
# End Source File
# Begin Source File

SOURCE=..\win32\time.h
# End Source File
# Begin Source File

SOURCE=..\win32\wfile.c
# End Source File
# Begin Source File

SOURCE=..\win32\wfile.h
# End Source File
# Begin Source File

SOURCE=..\win32\winutil.c
# End Source File
# Begin Source File

SOURCE=..\win32\wsyslog.c
# End Source File
# End Group
# Begin Group "functions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\functions\apache.c
# End Source File
# Begin Source File

SOURCE=..\functions\base64.c
# End Source File
# Begin Source File

SOURCE=..\functions\base64.h
# End Source File
# Begin Source File

SOURCE=..\functions\basic_functions.c
# End Source File
# Begin Source File

SOURCE=..\functions\basic_functions.h
# End Source File
# Begin Source File

SOURCE=..\functions\bcmath.c
# End Source File
# Begin Source File

SOURCE=..\functions\datetime.c
# End Source File
# Begin Source File

SOURCE=..\functions\datetime.h
# End Source File
# Begin Source File

SOURCE=..\functions\dir.c
# End Source File
# Begin Source File

SOURCE=..\functions\dl.c
# End Source File
# Begin Source File

SOURCE=..\functions\dl.h
# End Source File
# Begin Source File

SOURCE=..\functions\dns.c
# End Source File
# Begin Source File

SOURCE=..\functions\dns.h
# End Source File
# Begin Source File

SOURCE=..\functions\exec.c
# End Source File
# Begin Source File

SOURCE=..\functions\exec.h
# End Source File
# Begin Source File

SOURCE=..\functions\file.c
# End Source File
# Begin Source File

SOURCE=..\functions\file.h
# End Source File
# Begin Source File

SOURCE=..\functions\filestat.c
# End Source File
# Begin Source File

SOURCE=..\functions\formatted_print.c
# End Source File
# Begin Source File

SOURCE=..\functions\fsock.c
# End Source File
# Begin Source File

SOURCE=..\functions\global.h
# End Source File
# Begin Source File

SOURCE=..\functions\head.c
# End Source File
# Begin Source File

SOURCE=..\functions\head.h
# End Source File
# Begin Source File

SOURCE=..\functions\html.c
# End Source File
# Begin Source File

SOURCE=..\functions\html.h
# End Source File
# Begin Source File

SOURCE=..\functions\image.c
# End Source File
# Begin Source File

SOURCE=..\functions\info.c
# End Source File
# Begin Source File

SOURCE=..\functions\link.c
# End Source File
# Begin Source File

SOURCE=..\functions\mail.c
# End Source File
# Begin Source File

SOURCE=..\functions\mail.h
# End Source File
# Begin Source File

SOURCE=..\functions\math.c
# End Source File
# Begin Source File

SOURCE=..\functions\md5.c
# End Source File
# Begin Source File

SOURCE=..\functions\md5.h
# End Source File
# Begin Source File

SOURCE=..\functions\microtime.c
# End Source File
# Begin Source File

SOURCE=..\functions\mime.c
# End Source File
# Begin Source File

SOURCE=..\functions\mime.h
# End Source File
# Begin Source File

SOURCE=..\functions\number.c
# End Source File
# Begin Source File

SOURCE=..\functions\odbc.c
# End Source File
# Begin Source File

SOURCE=..\functions\odbc.h
# End Source File
# Begin Source File

SOURCE=..\functions\pageinfo.c
# End Source File
# Begin Source File

SOURCE=..\functions\pageinfo.h
# End Source File
# Begin Source File

SOURCE=..\functions\php3_dir.h
# End Source File
# Begin Source File

SOURCE=..\functions\php3_filestat.h
# End Source File
# Begin Source File

SOURCE=..\functions\php3_string.h
# End Source File
# Begin Source File

SOURCE=..\functions\php3_syslog.h
# End Source File
# Begin Source File

SOURCE=..\functions\phpdir.h
# End Source File
# Begin Source File

SOURCE=..\functions\phpmath.h
# End Source File
# Begin Source File

SOURCE=..\functions\phpstring.h
# End Source File
# Begin Source File

SOURCE=..\functions\post.c
# End Source File
# Begin Source File

SOURCE=..\functions\post.h
# End Source File
# Begin Source File

SOURCE=..\functions\rand.c
# End Source File
# Begin Source File

SOURCE=..\functions\reg.c
# End Source File
# Begin Source File

SOURCE=..\functions\reg.h
# End Source File
# Begin Source File

SOURCE=..\functions\soundex.c
# End Source File
# Begin Source File

SOURCE=..\functions\string.c
# End Source File
# Begin Source File

SOURCE=..\functions\syslog.c
# End Source File
# Begin Source File

SOURCE=..\functions\type.c
# End Source File
# Begin Source File

SOURCE=..\functions\type.h
# End Source File
# Begin Source File

SOURCE=..\functions\uniqid.c
# End Source File
# Begin Source File

SOURCE=..\functions\url.c
# End Source File
# Begin Source File

SOURCE=..\functions\url.h
# End Source File
# End Group
# Begin Group "regex"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\regex\regcomp.c
# End Source File
# Begin Source File

SOURCE=..\regex\regerror.c
# End Source File
# Begin Source File

SOURCE=..\regex\regexec.c
# End Source File
# Begin Source File

SOURCE=..\regex\regfree.c
# End Source File
# End Group
# End Target
# End Project
