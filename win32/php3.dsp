# Microsoft Developer Studio Project File - Name="php3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=php3 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "php3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "php3.mak" CFG="php3 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "php3 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "php3 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "cgi_release"
# PROP Intermediate_Dir "cgi_release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../regex" /I ".." /I "include" /D "NDEBUG" /D "MSVC5" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 lib44bsd95.lib resolv.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3.0 /subsystem:console /machine:I386 /out:"cgi_release/php.exe" /libpath:"lib"

!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "cgi_debug"
# PROP Intermediate_Dir "cgi_debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../regex" /I ".." /I "include" /D DEBUG=1 /D "_DEBUG" /D "MSVC5" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 lib44bsd95.lib resolv.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3.0 /subsystem:console /debug /machine:I386 /out:"cgi_debug\php.exe" /pdbtype:sept /libpath:"lib"

!ENDIF 

# Begin Target

# Name "php3 - Win32 Release"
# Name "php3 - Win32 Debug"
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\functions\COM.c
# End Source File
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

SOURCE=..\win32\unistd.h
# End Source File
# Begin Source File

SOURCE=..\win32\wfile.c
# End Source File
# Begin Source File

SOURCE=..\win32\wfile.h
# End Source File
# Begin Source File

SOURCE=..\dl\snmp\winsnmp.c
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

SOURCE=..\functions\browscap.c
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

SOURCE=..\functions\iptc.c
# End Source File
# Begin Source File

SOURCE=..\functions\lcg.c
# End Source File
# Begin Source File

SOURCE=..\functions\levenshtein.c
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

SOURCE=..\functions\number.h
# End Source File
# Begin Source File

SOURCE=..\functions\pack.c
# End Source File
# Begin Source File

SOURCE=..\functions\pageinfo.c
# End Source File
# Begin Source File

SOURCE=..\functions\pageinfo.h
# End Source File
# Begin Source File

SOURCE=..\functions\parsedate.tab.c
# End Source File
# Begin Source File

SOURCE=..\functions\parsedate.tab.h
# End Source File
# Begin Source File

SOURCE=..\functions\php3_bcmath.h
# End Source File
# Begin Source File

SOURCE=..\functions\php3_dir.h
# End Source File
# Begin Source File

SOURCE=..\functions\php3_filestat.h
# End Source File
# Begin Source File

SOURCE=..\functions\php3_ldap.h
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

SOURCE=..\functions\unified_odbc.c
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
# Begin Source File

SOURCE=..\functions\var.c
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
# Begin Group "core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\alloc.c
# End Source File
# Begin Source File

SOURCE=..\config.w32.h
# End Source File
# Begin Source File

SOURCE="..\configuration-parser.tab.c"
# End Source File
# Begin Source File

SOURCE="..\configuration-parser.tab.h"
# End Source File
# Begin Source File

SOURCE="..\configuration-scanner.c"
# End Source File
# Begin Source File

SOURCE=..\constants.c
# End Source File
# Begin Source File

SOURCE=..\constants.h
# End Source File
# Begin Source File

SOURCE=..\control_structures.h
# End Source File
# Begin Source File

SOURCE=..\control_structures_inline.h
# End Source File
# Begin Source File

SOURCE=..\functions\cyr_convert.c
# End Source File
# Begin Source File

SOURCE=..\functions\cyr_convert.h
# End Source File
# Begin Source File

SOURCE=..\debugger.c
# End Source File
# Begin Source File

SOURCE="..\fopen-wrappers.c"
# End Source File
# Begin Source File

SOURCE=..\getopt.c
# End Source File
# Begin Source File

SOURCE=..\getopt.h
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

SOURCE="..\language-parser.tab.c"
# End Source File
# Begin Source File

SOURCE="..\language-parser.tab.h"
# End Source File
# Begin Source File

SOURCE="..\language-scanner.c"
# End Source File
# Begin Source File

SOURCE="..\language-scanner.h"
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

SOURCE=..\php3_hash.c
# End Source File
# Begin Source File

SOURCE=..\php3_hash.h
# End Source File
# Begin Source File

SOURCE=..\php3_list.h
# End Source File
# Begin Source File

SOURCE=..\php3_realpath.c
# End Source File
# Begin Source File

SOURCE=..\php3_realpath.h
# End Source File
# Begin Source File

SOURCE=..\php_compat.c
# End Source File
# Begin Source File

SOURCE=..\php_compat.h
# End Source File
# Begin Source File

SOURCE=..\functions\quot_print.c
# End Source File
# Begin Source File

SOURCE=..\functions\quot_print.h
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
# End Target
# End Project
