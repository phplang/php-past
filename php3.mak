# Microsoft Developer Studio Generated NMAKE File, Based on php3.dsp
!IF "$(CFG)" == ""
CFG=php3 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to php3 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "php3 - Win32 Release" && "$(CFG)" != "php3 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "php3 - Win32 Release"

OUTDIR=.\cgi_release
INTDIR=.\cgi_release
# Begin Custom Macros
OutDir=.\cgi_release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php.exe"

!ELSE 

ALL : "php_custom_build - Win32 Release" "$(OUTDIR)\php.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"php_custom_build - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\apache.obj"
	-@erase "$(INTDIR)\base64.obj"
	-@erase "$(INTDIR)\basic_functions.obj"
	-@erase "$(INTDIR)\bcmath.obj"
	-@erase "$(INTDIR)\browscap.obj"
	-@erase "$(INTDIR)\COM.obj"
	-@erase "$(INTDIR)\configuration-parser.tab.obj"
	-@erase "$(INTDIR)\configuration-scanner.obj"
	-@erase "$(INTDIR)\constants.obj"
	-@erase "$(INTDIR)\datetime.obj"
	-@erase "$(INTDIR)\debugger.obj"
	-@erase "$(INTDIR)\dir.obj"
	-@erase "$(INTDIR)\dl.obj"
	-@erase "$(INTDIR)\dns.obj"
	-@erase "$(INTDIR)\exec.obj"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\filestat.obj"
	-@erase "$(INTDIR)\fopen-wrappers.obj"
	-@erase "$(INTDIR)\formatted_print.obj"
	-@erase "$(INTDIR)\fsock.obj"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\head.obj"
	-@erase "$(INTDIR)\highlight.obj"
	-@erase "$(INTDIR)\html.obj"
	-@erase "$(INTDIR)\image.obj"
	-@erase "$(INTDIR)\info.obj"
	-@erase "$(INTDIR)\internal_functions.obj"
	-@erase "$(INTDIR)\language-parser.tab.obj"
	-@erase "$(INTDIR)\language-scanner.obj"
	-@erase "$(INTDIR)\link.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\mail.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\microtime.obj"
	-@erase "$(INTDIR)\mime.obj"
	-@erase "$(INTDIR)\number.obj"
	-@erase "$(INTDIR)\operators.obj"
	-@erase "$(INTDIR)\pack.obj"
	-@erase "$(INTDIR)\pageinfo.obj"
	-@erase "$(INTDIR)\php3_hash.obj"
	-@erase "$(INTDIR)\php3_realpath.obj"
	-@erase "$(INTDIR)\post.obj"
	-@erase "$(INTDIR)\pwd.obj"
	-@erase "$(INTDIR)\rand.obj"
	-@erase "$(INTDIR)\readdir.obj"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\regcomp.obj"
	-@erase "$(INTDIR)\regerror.obj"
	-@erase "$(INTDIR)\regexec.obj"
	-@erase "$(INTDIR)\regfree.obj"
	-@erase "$(INTDIR)\request_info.obj"
	-@erase "$(INTDIR)\safe_mode.obj"
	-@erase "$(INTDIR)\sendmail.obj"
	-@erase "$(INTDIR)\soundex.obj"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\syslog.obj"
	-@erase "$(INTDIR)\time.obj"
	-@erase "$(INTDIR)\token_cache.obj"
	-@erase "$(INTDIR)\type.obj"
	-@erase "$(INTDIR)\unified_odbc.obj"
	-@erase "$(INTDIR)\uniqid.obj"
	-@erase "$(INTDIR)\url.obj"
	-@erase "$(INTDIR)\var.obj"
	-@erase "$(INTDIR)\variables.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\wfile.obj"
	-@erase "$(INTDIR)\winsnmp.obj"
	-@erase "$(INTDIR)\winutil.obj"
	-@erase "$(INTDIR)\wsyslog.obj"
	-@erase "$(OUTDIR)\php.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "." /I "regex\\" /I "d:\src\bind\include"\
 /D "NDEBUG" /D "MSVC5" /D "WIN32" /D "_CONSOLE" /D "_MBCS"\
 /Fp"$(INTDIR)\php3.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\cgi_release/
CPP_SBRS=.
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\php3.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=lib44bsd95.lib resolv.lib winmm.lib wsock32.lib netapi32.lib\
 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /version:3.0 /subsystem:console /incremental:no /pdb:"$(OUTDIR)\php.pdb"\
 /machine:I386 /out:"$(OUTDIR)\php.exe" /libpath:"\src\lib"\
 /libpath:"d:\src\bind\lib" 
LINK32_OBJS= \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\apache.obj" \
	"$(INTDIR)\base64.obj" \
	"$(INTDIR)\basic_functions.obj" \
	"$(INTDIR)\bcmath.obj" \
	"$(INTDIR)\browscap.obj" \
	"$(INTDIR)\COM.obj" \
	"$(INTDIR)\configuration-parser.tab.obj" \
	"$(INTDIR)\configuration-scanner.obj" \
	"$(INTDIR)\constants.obj" \
	"$(INTDIR)\datetime.obj" \
	"$(INTDIR)\debugger.obj" \
	"$(INTDIR)\dir.obj" \
	"$(INTDIR)\dl.obj" \
	"$(INTDIR)\dns.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\filestat.obj" \
	"$(INTDIR)\fopen-wrappers.obj" \
	"$(INTDIR)\formatted_print.obj" \
	"$(INTDIR)\fsock.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\head.obj" \
	"$(INTDIR)\highlight.obj" \
	"$(INTDIR)\html.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\info.obj" \
	"$(INTDIR)\internal_functions.obj" \
	"$(INTDIR)\language-parser.tab.obj" \
	"$(INTDIR)\language-scanner.obj" \
	"$(INTDIR)\link.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\mail.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\microtime.obj" \
	"$(INTDIR)\mime.obj" \
	"$(INTDIR)\number.obj" \
	"$(INTDIR)\operators.obj" \
	"$(INTDIR)\pack.obj" \
	"$(INTDIR)\pageinfo.obj" \
	"$(INTDIR)\php3_hash.obj" \
	"$(INTDIR)\php3_realpath.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\pwd.obj" \
	"$(INTDIR)\rand.obj" \
	"$(INTDIR)\readdir.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\regcomp.obj" \
	"$(INTDIR)\regerror.obj" \
	"$(INTDIR)\regexec.obj" \
	"$(INTDIR)\regfree.obj" \
	"$(INTDIR)\request_info.obj" \
	"$(INTDIR)\safe_mode.obj" \
	"$(INTDIR)\sendmail.obj" \
	"$(INTDIR)\soundex.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\syslog.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\token_cache.obj" \
	"$(INTDIR)\type.obj" \
	"$(INTDIR)\unified_odbc.obj" \
	"$(INTDIR)\uniqid.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\var.obj" \
	"$(INTDIR)\variables.obj" \
	"$(INTDIR)\wfile.obj" \
	"$(INTDIR)\winsnmp.obj" \
	"$(INTDIR)\winutil.obj" \
	"$(INTDIR)\wsyslog.obj"

"$(OUTDIR)\php.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

OUTDIR=c:\php3
INTDIR=.\cgi_debug
# Begin Custom Macros
OutDir=c:\php3
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\php.exe" "$(OUTDIR)\php3.bsc"

!ELSE 

ALL : "php_custom_build - Win32 Debug" "$(OUTDIR)\php.exe" "$(OUTDIR)\php3.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"php_custom_build - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\alloc.obj"
	-@erase "$(INTDIR)\alloc.sbr"
	-@erase "$(INTDIR)\apache.obj"
	-@erase "$(INTDIR)\apache.sbr"
	-@erase "$(INTDIR)\base64.obj"
	-@erase "$(INTDIR)\base64.sbr"
	-@erase "$(INTDIR)\basic_functions.obj"
	-@erase "$(INTDIR)\basic_functions.sbr"
	-@erase "$(INTDIR)\bcmath.obj"
	-@erase "$(INTDIR)\bcmath.sbr"
	-@erase "$(INTDIR)\browscap.obj"
	-@erase "$(INTDIR)\browscap.sbr"
	-@erase "$(INTDIR)\COM.obj"
	-@erase "$(INTDIR)\COM.sbr"
	-@erase "$(INTDIR)\configuration-parser.tab.obj"
	-@erase "$(INTDIR)\configuration-parser.tab.sbr"
	-@erase "$(INTDIR)\configuration-scanner.obj"
	-@erase "$(INTDIR)\configuration-scanner.sbr"
	-@erase "$(INTDIR)\constants.obj"
	-@erase "$(INTDIR)\constants.sbr"
	-@erase "$(INTDIR)\datetime.obj"
	-@erase "$(INTDIR)\datetime.sbr"
	-@erase "$(INTDIR)\debugger.obj"
	-@erase "$(INTDIR)\debugger.sbr"
	-@erase "$(INTDIR)\dir.obj"
	-@erase "$(INTDIR)\dir.sbr"
	-@erase "$(INTDIR)\dl.obj"
	-@erase "$(INTDIR)\dl.sbr"
	-@erase "$(INTDIR)\dns.obj"
	-@erase "$(INTDIR)\dns.sbr"
	-@erase "$(INTDIR)\exec.obj"
	-@erase "$(INTDIR)\exec.sbr"
	-@erase "$(INTDIR)\file.obj"
	-@erase "$(INTDIR)\file.sbr"
	-@erase "$(INTDIR)\filestat.obj"
	-@erase "$(INTDIR)\filestat.sbr"
	-@erase "$(INTDIR)\fopen-wrappers.obj"
	-@erase "$(INTDIR)\fopen-wrappers.sbr"
	-@erase "$(INTDIR)\formatted_print.obj"
	-@erase "$(INTDIR)\formatted_print.sbr"
	-@erase "$(INTDIR)\fsock.obj"
	-@erase "$(INTDIR)\fsock.sbr"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt.sbr"
	-@erase "$(INTDIR)\head.obj"
	-@erase "$(INTDIR)\head.sbr"
	-@erase "$(INTDIR)\highlight.obj"
	-@erase "$(INTDIR)\highlight.sbr"
	-@erase "$(INTDIR)\html.obj"
	-@erase "$(INTDIR)\html.sbr"
	-@erase "$(INTDIR)\image.obj"
	-@erase "$(INTDIR)\image.sbr"
	-@erase "$(INTDIR)\info.obj"
	-@erase "$(INTDIR)\info.sbr"
	-@erase "$(INTDIR)\internal_functions.obj"
	-@erase "$(INTDIR)\internal_functions.sbr"
	-@erase "$(INTDIR)\language-parser.tab.obj"
	-@erase "$(INTDIR)\language-parser.tab.sbr"
	-@erase "$(INTDIR)\language-scanner.obj"
	-@erase "$(INTDIR)\language-scanner.sbr"
	-@erase "$(INTDIR)\link.obj"
	-@erase "$(INTDIR)\link.sbr"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\list.sbr"
	-@erase "$(INTDIR)\mail.obj"
	-@erase "$(INTDIR)\mail.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\math.sbr"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\md5.sbr"
	-@erase "$(INTDIR)\microtime.obj"
	-@erase "$(INTDIR)\microtime.sbr"
	-@erase "$(INTDIR)\mime.obj"
	-@erase "$(INTDIR)\mime.sbr"
	-@erase "$(INTDIR)\number.obj"
	-@erase "$(INTDIR)\number.sbr"
	-@erase "$(INTDIR)\operators.obj"
	-@erase "$(INTDIR)\operators.sbr"
	-@erase "$(INTDIR)\pack.obj"
	-@erase "$(INTDIR)\pack.sbr"
	-@erase "$(INTDIR)\pageinfo.obj"
	-@erase "$(INTDIR)\pageinfo.sbr"
	-@erase "$(INTDIR)\php3_hash.obj"
	-@erase "$(INTDIR)\php3_hash.sbr"
	-@erase "$(INTDIR)\php3_realpath.obj"
	-@erase "$(INTDIR)\php3_realpath.sbr"
	-@erase "$(INTDIR)\post.obj"
	-@erase "$(INTDIR)\post.sbr"
	-@erase "$(INTDIR)\pwd.obj"
	-@erase "$(INTDIR)\pwd.sbr"
	-@erase "$(INTDIR)\rand.obj"
	-@erase "$(INTDIR)\rand.sbr"
	-@erase "$(INTDIR)\readdir.obj"
	-@erase "$(INTDIR)\readdir.sbr"
	-@erase "$(INTDIR)\reg.obj"
	-@erase "$(INTDIR)\reg.sbr"
	-@erase "$(INTDIR)\regcomp.obj"
	-@erase "$(INTDIR)\regcomp.sbr"
	-@erase "$(INTDIR)\regerror.obj"
	-@erase "$(INTDIR)\regerror.sbr"
	-@erase "$(INTDIR)\regexec.obj"
	-@erase "$(INTDIR)\regexec.sbr"
	-@erase "$(INTDIR)\regfree.obj"
	-@erase "$(INTDIR)\regfree.sbr"
	-@erase "$(INTDIR)\request_info.obj"
	-@erase "$(INTDIR)\request_info.sbr"
	-@erase "$(INTDIR)\safe_mode.obj"
	-@erase "$(INTDIR)\safe_mode.sbr"
	-@erase "$(INTDIR)\sendmail.obj"
	-@erase "$(INTDIR)\sendmail.sbr"
	-@erase "$(INTDIR)\soundex.obj"
	-@erase "$(INTDIR)\soundex.sbr"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\stack.sbr"
	-@erase "$(INTDIR)\string.obj"
	-@erase "$(INTDIR)\string.sbr"
	-@erase "$(INTDIR)\syslog.obj"
	-@erase "$(INTDIR)\syslog.sbr"
	-@erase "$(INTDIR)\time.obj"
	-@erase "$(INTDIR)\time.sbr"
	-@erase "$(INTDIR)\token_cache.obj"
	-@erase "$(INTDIR)\token_cache.sbr"
	-@erase "$(INTDIR)\type.obj"
	-@erase "$(INTDIR)\type.sbr"
	-@erase "$(INTDIR)\unified_odbc.obj"
	-@erase "$(INTDIR)\unified_odbc.sbr"
	-@erase "$(INTDIR)\uniqid.obj"
	-@erase "$(INTDIR)\uniqid.sbr"
	-@erase "$(INTDIR)\url.obj"
	-@erase "$(INTDIR)\url.sbr"
	-@erase "$(INTDIR)\var.obj"
	-@erase "$(INTDIR)\var.sbr"
	-@erase "$(INTDIR)\variables.obj"
	-@erase "$(INTDIR)\variables.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\wfile.obj"
	-@erase "$(INTDIR)\wfile.sbr"
	-@erase "$(INTDIR)\winsnmp.obj"
	-@erase "$(INTDIR)\winsnmp.sbr"
	-@erase "$(INTDIR)\winutil.obj"
	-@erase "$(INTDIR)\winutil.sbr"
	-@erase "$(INTDIR)\wsyslog.obj"
	-@erase "$(INTDIR)\wsyslog.sbr"
	-@erase "$(OUTDIR)\php.exe"
	-@erase "$(OUTDIR)\php.ilk"
	-@erase "$(OUTDIR)\php.pdb"
	-@erase "$(OUTDIR)\php3.bsc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /I "." /I "regex\\" /I\
 "d:\src\bind\include" /D "DEBUG" /D "_DEBUG" /D "MSVC5" /D "WIN32" /D\
 "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\php3.pch" /YX\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\cgi_debug/
CPP_SBRS=.\cgi_debug/
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\php3.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\alloc.sbr" \
	"$(INTDIR)\apache.sbr" \
	"$(INTDIR)\base64.sbr" \
	"$(INTDIR)\basic_functions.sbr" \
	"$(INTDIR)\bcmath.sbr" \
	"$(INTDIR)\browscap.sbr" \
	"$(INTDIR)\COM.sbr" \
	"$(INTDIR)\configuration-parser.tab.sbr" \
	"$(INTDIR)\configuration-scanner.sbr" \
	"$(INTDIR)\constants.sbr" \
	"$(INTDIR)\datetime.sbr" \
	"$(INTDIR)\debugger.sbr" \
	"$(INTDIR)\dir.sbr" \
	"$(INTDIR)\dl.sbr" \
	"$(INTDIR)\dns.sbr" \
	"$(INTDIR)\exec.sbr" \
	"$(INTDIR)\file.sbr" \
	"$(INTDIR)\filestat.sbr" \
	"$(INTDIR)\fopen-wrappers.sbr" \
	"$(INTDIR)\formatted_print.sbr" \
	"$(INTDIR)\fsock.sbr" \
	"$(INTDIR)\getopt.sbr" \
	"$(INTDIR)\head.sbr" \
	"$(INTDIR)\highlight.sbr" \
	"$(INTDIR)\html.sbr" \
	"$(INTDIR)\image.sbr" \
	"$(INTDIR)\info.sbr" \
	"$(INTDIR)\internal_functions.sbr" \
	"$(INTDIR)\language-parser.tab.sbr" \
	"$(INTDIR)\language-scanner.sbr" \
	"$(INTDIR)\link.sbr" \
	"$(INTDIR)\list.sbr" \
	"$(INTDIR)\mail.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\math.sbr" \
	"$(INTDIR)\md5.sbr" \
	"$(INTDIR)\microtime.sbr" \
	"$(INTDIR)\mime.sbr" \
	"$(INTDIR)\number.sbr" \
	"$(INTDIR)\operators.sbr" \
	"$(INTDIR)\pack.sbr" \
	"$(INTDIR)\pageinfo.sbr" \
	"$(INTDIR)\php3_hash.sbr" \
	"$(INTDIR)\php3_realpath.sbr" \
	"$(INTDIR)\post.sbr" \
	"$(INTDIR)\pwd.sbr" \
	"$(INTDIR)\rand.sbr" \
	"$(INTDIR)\readdir.sbr" \
	"$(INTDIR)\reg.sbr" \
	"$(INTDIR)\regcomp.sbr" \
	"$(INTDIR)\regerror.sbr" \
	"$(INTDIR)\regexec.sbr" \
	"$(INTDIR)\regfree.sbr" \
	"$(INTDIR)\request_info.sbr" \
	"$(INTDIR)\safe_mode.sbr" \
	"$(INTDIR)\sendmail.sbr" \
	"$(INTDIR)\soundex.sbr" \
	"$(INTDIR)\stack.sbr" \
	"$(INTDIR)\string.sbr" \
	"$(INTDIR)\syslog.sbr" \
	"$(INTDIR)\time.sbr" \
	"$(INTDIR)\token_cache.sbr" \
	"$(INTDIR)\type.sbr" \
	"$(INTDIR)\unified_odbc.sbr" \
	"$(INTDIR)\uniqid.sbr" \
	"$(INTDIR)\url.sbr" \
	"$(INTDIR)\var.sbr" \
	"$(INTDIR)\variables.sbr" \
	"$(INTDIR)\wfile.sbr" \
	"$(INTDIR)\winsnmp.sbr" \
	"$(INTDIR)\winutil.sbr" \
	"$(INTDIR)\wsyslog.sbr"

"$(OUTDIR)\php3.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=lib44bsd95.lib resolv.lib winmm.lib wsock32.lib netapi32.lib\
 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib\
 shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo\
 /version:3.0 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\php.pdb"\
 /debug /machine:I386 /out:"$(OUTDIR)\php.exe" /pdbtype:sept /libpath:"\src\lib"\
 /libpath:"d:\src\bind\lib" 
LINK32_OBJS= \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\apache.obj" \
	"$(INTDIR)\base64.obj" \
	"$(INTDIR)\basic_functions.obj" \
	"$(INTDIR)\bcmath.obj" \
	"$(INTDIR)\browscap.obj" \
	"$(INTDIR)\COM.obj" \
	"$(INTDIR)\configuration-parser.tab.obj" \
	"$(INTDIR)\configuration-scanner.obj" \
	"$(INTDIR)\constants.obj" \
	"$(INTDIR)\datetime.obj" \
	"$(INTDIR)\debugger.obj" \
	"$(INTDIR)\dir.obj" \
	"$(INTDIR)\dl.obj" \
	"$(INTDIR)\dns.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\filestat.obj" \
	"$(INTDIR)\fopen-wrappers.obj" \
	"$(INTDIR)\formatted_print.obj" \
	"$(INTDIR)\fsock.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\head.obj" \
	"$(INTDIR)\highlight.obj" \
	"$(INTDIR)\html.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\info.obj" \
	"$(INTDIR)\internal_functions.obj" \
	"$(INTDIR)\language-parser.tab.obj" \
	"$(INTDIR)\language-scanner.obj" \
	"$(INTDIR)\link.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\mail.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\microtime.obj" \
	"$(INTDIR)\mime.obj" \
	"$(INTDIR)\number.obj" \
	"$(INTDIR)\operators.obj" \
	"$(INTDIR)\pack.obj" \
	"$(INTDIR)\pageinfo.obj" \
	"$(INTDIR)\php3_hash.obj" \
	"$(INTDIR)\php3_realpath.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\pwd.obj" \
	"$(INTDIR)\rand.obj" \
	"$(INTDIR)\readdir.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\regcomp.obj" \
	"$(INTDIR)\regerror.obj" \
	"$(INTDIR)\regexec.obj" \
	"$(INTDIR)\regfree.obj" \
	"$(INTDIR)\request_info.obj" \
	"$(INTDIR)\safe_mode.obj" \
	"$(INTDIR)\sendmail.obj" \
	"$(INTDIR)\soundex.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\syslog.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\token_cache.obj" \
	"$(INTDIR)\type.obj" \
	"$(INTDIR)\unified_odbc.obj" \
	"$(INTDIR)\uniqid.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\var.obj" \
	"$(INTDIR)\variables.obj" \
	"$(INTDIR)\wfile.obj" \
	"$(INTDIR)\winsnmp.obj" \
	"$(INTDIR)\winutil.obj" \
	"$(INTDIR)\wsyslog.obj"

"$(OUTDIR)\php.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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


!IF "$(CFG)" == "php3 - Win32 Release" || "$(CFG)" == "php3 - Win32 Debug"
SOURCE=.\functions\COM.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_COM_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_COM.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\COM.obj" : $(SOURCE) $(DEP_CPP_COM_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_COM_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_COM.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\COM.obj"	"$(INTDIR)\COM.sbr" : $(SOURCE) $(DEP_CPP_COM_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\pwd.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_PWD_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\grp.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\pwd.obj" : $(SOURCE) $(DEP_CPP_PWD_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_PWD_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\grp.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\pwd.obj"	"$(INTDIR)\pwd.sbr" : $(SOURCE) $(DEP_CPP_PWD_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\readdir.c
DEP_CPP_READD=\
	".\win32\readdir.h"\
	

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\readdir.obj" : $(SOURCE) $(DEP_CPP_READD) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\readdir.obj"	"$(INTDIR)\readdir.sbr" : $(SOURCE) $(DEP_CPP_READD)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\sendmail.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_SENDM=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\sendmail.h"\
	".\win95nt.h"\
	

"$(INTDIR)\sendmail.obj" : $(SOURCE) $(DEP_CPP_SENDM) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_SENDM=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\sendmail.h"\
	".\win95nt.h"\
	

"$(INTDIR)\sendmail.obj"	"$(INTDIR)\sendmail.sbr" : $(SOURCE) $(DEP_CPP_SENDM)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\time.c
DEP_CPP_TIME_=\
	".\win32\unistd.h"\
	

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\time.obj" : $(SOURCE) $(DEP_CPP_TIME_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\time.obj"	"$(INTDIR)\time.sbr" : $(SOURCE) $(DEP_CPP_TIME_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\wfile.c
DEP_CPP_WFILE=\
	".\win32\wfile.h"\
	

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\wfile.obj" : $(SOURCE) $(DEP_CPP_WFILE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\wfile.obj"	"$(INTDIR)\wfile.sbr" : $(SOURCE) $(DEP_CPP_WFILE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dl\snmp\winsnmp.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_WINSN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\snmp\php3_snmp.h"\
	".\fopen-wrappers.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\winsnmp.obj" : $(SOURCE) $(DEP_CPP_WINSN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_WINSN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\snmp\php3_snmp.h"\
	".\fopen-wrappers.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\winsnmp.obj"	"$(INTDIR)\winsnmp.sbr" : $(SOURCE) $(DEP_CPP_WINSN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\winutil.c
DEP_CPP_WINUT=\
	".\win32\winutil.h"\
	

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\winutil.obj" : $(SOURCE) $(DEP_CPP_WINUT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\winutil.obj"	"$(INTDIR)\winutil.sbr" : $(SOURCE) $(DEP_CPP_WINUT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\wsyslog.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_WSYSL=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\syslog.h"\
	".\win95nt.h"\
	

"$(INTDIR)\wsyslog.obj" : $(SOURCE) $(DEP_CPP_WSYSL) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_WSYSL=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\syslog.h"\
	".\win95nt.h"\
	

"$(INTDIR)\wsyslog.obj"	"$(INTDIR)\wsyslog.sbr" : $(SOURCE) $(DEP_CPP_WSYSL)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\apache.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_APACH=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\apache.obj" : $(SOURCE) $(DEP_CPP_APACH) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_APACH=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\apache.obj"	"$(INTDIR)\apache.sbr" : $(SOURCE) $(DEP_CPP_APACH)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\base64.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_BASE6=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\base64.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\base64.obj" : $(SOURCE) $(DEP_CPP_BASE6) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_BASE6=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\base64.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\base64.obj"	"$(INTDIR)\base64.sbr" : $(SOURCE) $(DEP_CPP_BASE6)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\basic_functions.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_BASIC=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\base64.h"\
	".\functions\basic_functions.h"\
	".\functions\datetime.h"\
	".\functions\dns.h"\
	".\functions\exec.h"\
	".\functions\fsock.h"\
	".\functions\global.h"\
	".\functions\html.h"\
	".\functions\image.h"\
	".\functions\info.h"\
	".\functions\md5.h"\
	".\functions\microtime.h"\
	".\functions\pageinfo.h"\
	".\functions\php3_filestat.h"\
	".\functions\php3_link.h"\
	".\functions\php3_mail.h"\
	".\functions\php3_string.h"\
	".\functions\php3_var.h"\
	".\functions\phpmath.h"\
	".\functions\post.h"\
	".\functions\uniqid.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\internal_functions_registry.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\unistd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\basic_functions.obj" : $(SOURCE) $(DEP_CPP_BASIC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_BASIC=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\base64.h"\
	".\functions\basic_functions.h"\
	".\functions\datetime.h"\
	".\functions\dns.h"\
	".\functions\exec.h"\
	".\functions\fsock.h"\
	".\functions\global.h"\
	".\functions\html.h"\
	".\functions\image.h"\
	".\functions\info.h"\
	".\functions\md5.h"\
	".\functions\microtime.h"\
	".\functions\pageinfo.h"\
	".\functions\php3_filestat.h"\
	".\functions\php3_link.h"\
	".\functions\php3_mail.h"\
	".\functions\php3_string.h"\
	".\functions\phpmath.h"\
	".\functions\post.h"\
	".\functions\uniqid.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\internal_functions_registry.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\unistd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\basic_functions.obj"	"$(INTDIR)\basic_functions.sbr" : $(SOURCE)\
 $(DEP_CPP_BASIC) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\bcmath.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_BCMAT=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\number.h"\
	".\functions\php3_bcmath.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\bcmath.obj" : $(SOURCE) $(DEP_CPP_BCMAT) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_BCMAT=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\number.h"\
	".\functions\php3_bcmath.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\bcmath.obj"	"$(INTDIR)\bcmath.sbr" : $(SOURCE) $(DEP_CPP_BCMAT)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\browscap.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_BROWS=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_browscap.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\browscap.obj" : $(SOURCE) $(DEP_CPP_BROWS) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_BROWS=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_browscap.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\browscap.obj"	"$(INTDIR)\browscap.sbr" : $(SOURCE) $(DEP_CPP_BROWS)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\datetime.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_DATET=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\datetime.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\datetime.obj" : $(SOURCE) $(DEP_CPP_DATET) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_DATET=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\datetime.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\datetime.obj"	"$(INTDIR)\datetime.sbr" : $(SOURCE) $(DEP_CPP_DATET)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\dir.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_DIR_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_dir.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\readdir.h"\
	".\win95nt.h"\
	

"$(INTDIR)\dir.obj" : $(SOURCE) $(DEP_CPP_DIR_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_DIR_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_dir.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\readdir.h"\
	".\win95nt.h"\
	

"$(INTDIR)\dir.obj"	"$(INTDIR)\dir.sbr" : $(SOURCE) $(DEP_CPP_DIR_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\dl.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_DL_C20=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\dl.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\winutil.h"\
	".\win95nt.h"\
	

"$(INTDIR)\dl.obj" : $(SOURCE) $(DEP_CPP_DL_C20) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_DL_C20=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\dl.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\winutil.h"\
	".\win95nt.h"\
	

"$(INTDIR)\dl.obj"	"$(INTDIR)\dl.sbr" : $(SOURCE) $(DEP_CPP_DL_C20) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\dns.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_DNS_C=\
	"..\..\bind\include\arpa\inet.h"\
	"..\..\bind\include\arpa\nameser.h"\
	"..\..\bind\include\netdb.h"\
	"..\..\bind\include\portability.h"\
	"..\..\bind\include\resolv.h"\
	"..\..\bind\include\sys\bitypes.h"\
	"..\..\bind\include\sys\cdefs.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\dns.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\dns.obj" : $(SOURCE) $(DEP_CPP_DNS_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_DNS_C=\
	"..\..\bind\include\arpa\inet.h"\
	"..\..\bind\include\arpa\nameser.h"\
	"..\..\bind\include\netdb.h"\
	"..\..\bind\include\portability.h"\
	"..\..\bind\include\resolv.h"\
	"..\..\bind\include\sys\bitypes.h"\
	"..\..\bind\include\sys\cdefs.h"\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\dns.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\dns.obj"	"$(INTDIR)\dns.sbr" : $(SOURCE) $(DEP_CPP_DNS_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\exec.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_EXEC_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\exec.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\exec.obj" : $(SOURCE) $(DEP_CPP_EXEC_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_EXEC_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\exec.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\exec.obj"	"$(INTDIR)\exec.sbr" : $(SOURCE) $(DEP_CPP_EXEC_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\file.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_FILE_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\file.h"\
	".\functions\fsock.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\winutil.h"\
	".\win95nt.h"\
	

"$(INTDIR)\file.obj" : $(SOURCE) $(DEP_CPP_FILE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_FILE_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\file.h"\
	".\functions\fsock.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\winutil.h"\
	".\win95nt.h"\
	

"$(INTDIR)\file.obj"	"$(INTDIR)\file.sbr" : $(SOURCE) $(DEP_CPP_FILE_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\filestat.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_FILES=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_filestat.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\filestat.obj" : $(SOURCE) $(DEP_CPP_FILES) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_FILES=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_filestat.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\filestat.obj"	"$(INTDIR)\filestat.sbr" : $(SOURCE) $(DEP_CPP_FILES)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\formatted_print.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_FORMA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\formatted_print.obj" : $(SOURCE) $(DEP_CPP_FORMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_FORMA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\formatted_print.obj"	"$(INTDIR)\formatted_print.sbr" : $(SOURCE)\
 $(DEP_CPP_FORMA) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\fsock.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_FSOCK=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\base64.h"\
	".\functions\file.h"\
	".\functions\fsock.h"\
	".\functions\post.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\fsock.obj" : $(SOURCE) $(DEP_CPP_FSOCK) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_FSOCK=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\base64.h"\
	".\functions\file.h"\
	".\functions\fsock.h"\
	".\functions\post.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\fsock.obj"	"$(INTDIR)\fsock.sbr" : $(SOURCE) $(DEP_CPP_FSOCK)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\head.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_HEAD_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\datetime.h"\
	".\functions\head.h"\
	".\functions\pageinfo.h"\
	".\functions\php3_string.h"\
	".\functions\post.h"\
	".\functions\reg.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\head.obj" : $(SOURCE) $(DEP_CPP_HEAD_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_HEAD_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\datetime.h"\
	".\functions\head.h"\
	".\functions\pageinfo.h"\
	".\functions\php3_string.h"\
	".\functions\post.h"\
	".\functions\reg.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\head.obj"	"$(INTDIR)\head.sbr" : $(SOURCE) $(DEP_CPP_HEAD_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\html.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_HTML_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\html.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\html.obj" : $(SOURCE) $(DEP_CPP_HTML_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_HTML_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\html.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\html.obj"	"$(INTDIR)\html.sbr" : $(SOURCE) $(DEP_CPP_HTML_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\image.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_IMAGE=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\image.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\image.obj" : $(SOURCE) $(DEP_CPP_IMAGE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_IMAGE=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\image.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\image.obj"	"$(INTDIR)\image.sbr" : $(SOURCE) $(DEP_CPP_IMAGE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\info.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_INFO_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\info.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\info.obj" : $(SOURCE) $(DEP_CPP_INFO_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_INFO_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\info.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\info.obj"	"$(INTDIR)\info.sbr" : $(SOURCE) $(DEP_CPP_INFO_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\link.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_LINK_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_link.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\link.obj" : $(SOURCE) $(DEP_CPP_LINK_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_LINK_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_link.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\link.obj"	"$(INTDIR)\link.sbr" : $(SOURCE) $(DEP_CPP_LINK_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\mail.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_MAIL_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_mail.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\sendmail.h"\
	".\win95nt.h"\
	

"$(INTDIR)\mail.obj" : $(SOURCE) $(DEP_CPP_MAIL_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_MAIL_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_mail.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\sendmail.h"\
	".\win95nt.h"\
	

"$(INTDIR)\mail.obj"	"$(INTDIR)\mail.sbr" : $(SOURCE) $(DEP_CPP_MAIL_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\math.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_MATH_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\phpmath.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\math.obj" : $(SOURCE) $(DEP_CPP_MATH_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_MATH_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\phpmath.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\math.obj"	"$(INTDIR)\math.sbr" : $(SOURCE) $(DEP_CPP_MATH_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\md5.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_MD5_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\global.h"\
	".\functions\md5.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\md5.obj" : $(SOURCE) $(DEP_CPP_MD5_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_MD5_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\global.h"\
	".\functions\md5.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) $(DEP_CPP_MD5_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\microtime.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_MICRO=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\microtime.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\microtime.obj" : $(SOURCE) $(DEP_CPP_MICRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_MICRO=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\microtime.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\microtime.obj"	"$(INTDIR)\microtime.sbr" : $(SOURCE)\
 $(DEP_CPP_MICRO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\mime.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_MIME_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\mime.h"\
	".\functions\post.h"\
	".\functions\type.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\mime.obj" : $(SOURCE) $(DEP_CPP_MIME_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_MIME_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\mime.h"\
	".\functions\post.h"\
	".\functions\type.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\mime.obj"	"$(INTDIR)\mime.sbr" : $(SOURCE) $(DEP_CPP_MIME_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\number.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_NUMBE=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\number.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\number.obj" : $(SOURCE) $(DEP_CPP_NUMBE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_NUMBE=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\number.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\number.obj"	"$(INTDIR)\number.sbr" : $(SOURCE) $(DEP_CPP_NUMBE)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\pack.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_PACK_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\fsock.h"\
	".\functions\head.h"\
	".\functions\pack.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\pack.obj" : $(SOURCE) $(DEP_CPP_PACK_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_PACK_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\fsock.h"\
	".\functions\head.h"\
	".\functions\pack.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\pack.obj"	"$(INTDIR)\pack.sbr" : $(SOURCE) $(DEP_CPP_PACK_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\pageinfo.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_PAGEI=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\pageinfo.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\pageinfo.obj" : $(SOURCE) $(DEP_CPP_PAGEI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_PAGEI=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\pageinfo.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\pageinfo.obj"	"$(INTDIR)\pageinfo.sbr" : $(SOURCE) $(DEP_CPP_PAGEI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\post.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_POST_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\mime.h"\
	".\functions\php3_string.h"\
	".\functions\post.h"\
	".\functions\type.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\post.obj" : $(SOURCE) $(DEP_CPP_POST_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_POST_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\mime.h"\
	".\functions\php3_string.h"\
	".\functions\post.h"\
	".\functions\type.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\post.obj"	"$(INTDIR)\post.sbr" : $(SOURCE) $(DEP_CPP_POST_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\rand.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_RAND_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\phpmath.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\rand.obj" : $(SOURCE) $(DEP_CPP_RAND_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_RAND_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\phpmath.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\rand.obj"	"$(INTDIR)\rand.sbr" : $(SOURCE) $(DEP_CPP_RAND_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\reg.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_REG_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_string.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\reg.obj" : $(SOURCE) $(DEP_CPP_REG_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_REG_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_string.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\reg.obj"	"$(INTDIR)\reg.sbr" : $(SOURCE) $(DEP_CPP_REG_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\soundex.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_SOUND=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\soundex.obj" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_SOUND=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_string.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\soundex.obj"	"$(INTDIR)\soundex.sbr" : $(SOURCE) $(DEP_CPP_SOUND)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\string.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_STRIN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_string.h"\
	".\functions\post.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\string.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_STRIN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_string.h"\
	".\functions\post.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\string.obj"	"$(INTDIR)\string.sbr" : $(SOURCE) $(DEP_CPP_STRIN)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\syslog.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_SYSLO=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_syslog.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\syslog.h"\
	".\win95nt.h"\
	

"$(INTDIR)\syslog.obj" : $(SOURCE) $(DEP_CPP_SYSLO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_SYSLO=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_syslog.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\syslog.h"\
	".\win95nt.h"\
	

"$(INTDIR)\syslog.obj"	"$(INTDIR)\syslog.sbr" : $(SOURCE) $(DEP_CPP_SYSLO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\type.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_TYPE_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\type.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\type.obj" : $(SOURCE) $(DEP_CPP_TYPE_) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_TYPE_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\type.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\type.obj"	"$(INTDIR)\type.sbr" : $(SOURCE) $(DEP_CPP_TYPE_)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\unified_odbc.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_UNIFI=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\functions\php3_unified_odbc.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\unified_odbc.obj" : $(SOURCE) $(DEP_CPP_UNIFI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_UNIFI=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\functions\php3_unified_odbc.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\unified_odbc.obj"	"$(INTDIR)\unified_odbc.sbr" : $(SOURCE)\
 $(DEP_CPP_UNIFI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\uniqid.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_UNIQI=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\uniqid.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\uniqid.obj" : $(SOURCE) $(DEP_CPP_UNIQI) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_UNIQI=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\uniqid.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\uniqid.obj"	"$(INTDIR)\uniqid.sbr" : $(SOURCE) $(DEP_CPP_UNIQI)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\url.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_URL_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\url.obj" : $(SOURCE) $(DEP_CPP_URL_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_URL_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\url.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\url.obj"	"$(INTDIR)\url.sbr" : $(SOURCE) $(DEP_CPP_URL_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\var.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_VAR_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\php3_string.h"\
	".\functions\post.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\var.obj" : $(SOURCE) $(DEP_CPP_VAR_C) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_VAR_C=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\FlexSafe.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\functions\number.h"\
	".\functions\php3_string.h"\
	".\functions\post.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\serverapi\sapi.h"\
	".\stack.h"\
	".\tls.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\sendmail.h"\
	".\win95nt.h"\
	{$(INCLUDE)}"sys\stat.h"\
	{$(INCLUDE)}"sys\types.h"\
	
NODEP_CPP_VAR_C=\
	".\ap_compat.h"\
	".\ap_config.h"\
	".\compat.h"\
	".\config.h"\
	".\http_config.h"\
	".\http_core.h"\
	".\http_log.h"\
	".\http_main.h"\
	".\http_protocol.h"\
	".\http_request.h"\
	".\httpd.h"\
	

"$(INTDIR)\var.obj"	"$(INTDIR)\var.sbr" : $(SOURCE) $(DEP_CPP_VAR_C)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\regex\regcomp.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_REGCO=\
	".\regex\cclass.h"\
	".\regex\cname.h"\
	".\regex\regcomp.ih"\
	".\regex\regex.h"\
	".\regex\regex2.h"\
	".\regex\utils.h"\
	

"$(INTDIR)\regcomp.obj" : $(SOURCE) $(DEP_CPP_REGCO) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_REGCO=\
	".\regex\cclass.h"\
	".\regex\cname.h"\
	".\regex\regcomp.ih"\
	".\regex\regex.h"\
	".\regex\regex2.h"\
	".\regex\utils.h"\
	

"$(INTDIR)\regcomp.obj"	"$(INTDIR)\regcomp.sbr" : $(SOURCE) $(DEP_CPP_REGCO)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\regex\regerror.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_REGER=\
	".\regex\regerror.ih"\
	".\regex\regex.h"\
	".\regex\utils.h"\
	

"$(INTDIR)\regerror.obj" : $(SOURCE) $(DEP_CPP_REGER) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_REGER=\
	".\regex\regerror.ih"\
	".\regex\regex.h"\
	".\regex\utils.h"\
	

"$(INTDIR)\regerror.obj"	"$(INTDIR)\regerror.sbr" : $(SOURCE) $(DEP_CPP_REGER)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\regex\regexec.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_REGEX=\
	".\regex\engine.c"\
	".\regex\engine.ih"\
	".\regex\regex.h"\
	".\regex\regex2.h"\
	".\regex\utils.h"\
	

"$(INTDIR)\regexec.obj" : $(SOURCE) $(DEP_CPP_REGEX) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_REGEX=\
	".\regex\engine.c"\
	".\regex\engine.ih"\
	".\regex\regex.h"\
	".\regex\regex2.h"\
	".\regex\utils.h"\
	

"$(INTDIR)\regexec.obj"	"$(INTDIR)\regexec.sbr" : $(SOURCE) $(DEP_CPP_REGEX)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\regex\regfree.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_REGFR=\
	".\regex\regex.h"\
	".\regex\regex2.h"\
	".\regex\utils.h"\
	

"$(INTDIR)\regfree.obj" : $(SOURCE) $(DEP_CPP_REGFR) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_REGFR=\
	".\regex\regex.h"\
	".\regex\regex2.h"\
	".\regex\utils.h"\
	

"$(INTDIR)\regfree.obj"	"$(INTDIR)\regfree.sbr" : $(SOURCE) $(DEP_CPP_REGFR)\
 "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\alloc.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_ALLOC=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\alloc.obj" : $(SOURCE) $(DEP_CPP_ALLOC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_ALLOC=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\alloc.obj"	"$(INTDIR)\alloc.sbr" : $(SOURCE) $(DEP_CPP_ALLOC)\
 "$(INTDIR)"


!ENDIF 

SOURCE=".\configuration-parser.tab.c"

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_CONFI=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\dl.h"\
	".\functions\file.h"\
	".\functions\php3_browscap.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\wfile.h"\
	".\win95nt.h"\
	

"$(INTDIR)\configuration-parser.tab.obj" : $(SOURCE) $(DEP_CPP_CONFI)\
 "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_CONFI=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\dl.h"\
	".\functions\file.h"\
	".\functions\php3_browscap.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\wfile.h"\
	".\win95nt.h"\
	

"$(INTDIR)\configuration-parser.tab.obj"\
	"$(INTDIR)\configuration-parser.tab.sbr" : $(SOURCE) $(DEP_CPP_CONFI)\
 "$(INTDIR)"


!ENDIF 

SOURCE=".\configuration-scanner.c"

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_CONFIG=\
	".\alloc.h"\
	".\config.w32.h"\
	".\configuration-parser.tab.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\configuration-scanner.obj" : $(SOURCE) $(DEP_CPP_CONFIG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_CONFIG=\
	".\alloc.h"\
	".\config.w32.h"\
	".\configuration-parser.tab.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\configuration-scanner.obj"	"$(INTDIR)\configuration-scanner.sbr" : \
$(SOURCE) $(DEP_CPP_CONFIG) "$(INTDIR)"


!ENDIF 

SOURCE=.\constants.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_CONST=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\constants.obj" : $(SOURCE) $(DEP_CPP_CONST) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_CONST=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\constants.obj"	"$(INTDIR)\constants.sbr" : $(SOURCE)\
 $(DEP_CPP_CONST) "$(INTDIR)"


!ENDIF 

SOURCE=.\debugger.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_DEBUG=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\fsock.h"\
	".\internal_functions.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_debugger.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\debugger.obj" : $(SOURCE) $(DEP_CPP_DEBUG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_DEBUG=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\fsock.h"\
	".\internal_functions.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_debugger.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\debugger.obj"	"$(INTDIR)\debugger.sbr" : $(SOURCE) $(DEP_CPP_DEBUG)\
 "$(INTDIR)"


!ENDIF 

SOURCE=".\fopen-wrappers.c"

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_FOPEN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\base64.h"\
	".\functions\fsock.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\functions\url.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php3_realpath.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\fopen-wrappers.obj" : $(SOURCE) $(DEP_CPP_FOPEN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_FOPEN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\base64.h"\
	".\functions\fsock.h"\
	".\functions\head.h"\
	".\functions\php3_string.h"\
	".\functions\url.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php3_realpath.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\fopen-wrappers.obj"	"$(INTDIR)\fopen-wrappers.sbr" : $(SOURCE)\
 $(DEP_CPP_FOPEN) "$(INTDIR)"


!ENDIF 

SOURCE=.\getopt.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_GETOP=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\getopt.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\getopt.obj" : $(SOURCE) $(DEP_CPP_GETOP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_GETOP=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\getopt.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\getopt.obj"	"$(INTDIR)\getopt.sbr" : $(SOURCE) $(DEP_CPP_GETOP)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\highlight.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_HIGHL=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\highlight.h"\
	".\language-parser.tab.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\highlight.obj" : $(SOURCE) $(DEP_CPP_HIGHL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_HIGHL=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\highlight.h"\
	".\language-parser.tab.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\highlight.obj"	"$(INTDIR)\highlight.sbr" : $(SOURCE)\
 $(DEP_CPP_HIGHL) "$(INTDIR)"


!ENDIF 

SOURCE=.\internal_functions.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_INTER=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\snmp\php3_snmp.h"\
	".\fopen-wrappers.h"\
	".\functions\adabasd.h"\
	".\functions\base64.h"\
	".\functions\basic_functions.h"\
	".\functions\db.h"\
	".\functions\dbase.h"\
	".\functions\dl.h"\
	".\functions\dns.h"\
	".\functions\exec.h"\
	".\functions\file.h"\
	".\functions\filepro.h"\
	".\functions\global.h"\
	".\functions\head.h"\
	".\functions\html.h"\
	".\functions\hw.h"\
	".\functions\imap.h"\
	".\functions\md5.h"\
	".\functions\oracle.h"\
	".\functions\pack.h"\
	".\functions\php3_bcmath.h"\
	".\functions\php3_browscap.h"\
	".\functions\php3_COM.h"\
	".\functions\php3_dir.h"\
	".\functions\php3_filestat.h"\
	".\functions\php3_gd.h"\
	".\functions\php3_ifx.h"\
	".\functions\php3_ldap.h"\
	".\functions\php3_mail.h"\
	".\functions\php3_msql.h"\
	".\functions\php3_mysql.h"\
	".\functions\php3_oci8.h"\
	".\functions\php3_pgsql.h"\
	".\functions\php3_solid.h"\
	".\functions\php3_string.h"\
	".\functions\php3_sybase-ct.h"\
	".\functions\php3_sybase.h"\
	".\functions\php3_syslog.h"\
	".\functions\php3_unified_odbc.h"\
	".\functions\php3_velocis.h"\
	".\functions\php3_zlib.h"\
	".\functions\phpmath.h"\
	".\functions\post.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\internal_functions_registry.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_debugger.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\internal_functions.obj" : $(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_INTER=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\dl\snmp\php3_snmp.h"\
	".\fopen-wrappers.h"\
	".\functions\adabasd.h"\
	".\functions\base64.h"\
	".\functions\basic_functions.h"\
	".\functions\db.h"\
	".\functions\dbase.h"\
	".\functions\dl.h"\
	".\functions\dns.h"\
	".\functions\exec.h"\
	".\functions\file.h"\
	".\functions\filepro.h"\
	".\functions\global.h"\
	".\functions\head.h"\
	".\functions\html.h"\
	".\functions\hw.h"\
	".\functions\imap.h"\
	".\functions\md5.h"\
	".\functions\oracle.h"\
	".\functions\pack.h"\
	".\functions\php3_bcmath.h"\
	".\functions\php3_browscap.h"\
	".\functions\php3_COM.h"\
	".\functions\php3_dir.h"\
	".\functions\php3_filestat.h"\
	".\functions\php3_gd.h"\
	".\functions\php3_ifx.h"\
	".\functions\php3_ldap.h"\
	".\functions\php3_mail.h"\
	".\functions\php3_msql.h"\
	".\functions\php3_mysql.h"\
	".\functions\php3_oci8.h"\
	".\functions\php3_pgsql.h"\
	".\functions\php3_solid.h"\
	".\functions\php3_string.h"\
	".\functions\php3_sybase-ct.h"\
	".\functions\php3_sybase.h"\
	".\functions\php3_syslog.h"\
	".\functions\php3_unified_odbc.h"\
	".\functions\php3_velocis.h"\
	".\functions\php3_zlib.h"\
	".\functions\phpmath.h"\
	".\functions\post.h"\
	".\functions\reg.h"\
	".\internal_functions.h"\
	".\internal_functions_registry.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_debugger.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\internal_functions.obj"	"$(INTDIR)\internal_functions.sbr" : \
$(SOURCE) $(DEP_CPP_INTER) "$(INTDIR)"


!ENDIF 

SOURCE=".\language-parser.tab.c"

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_LANGU=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\control_structures_inline.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\internal_functions.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\language-parser.tab.obj" : $(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_LANGU=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\control_structures_inline.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\internal_functions.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\language-parser.tab.obj"	"$(INTDIR)\language-parser.tab.sbr" : \
$(SOURCE) $(DEP_CPP_LANGU) "$(INTDIR)"


!ENDIF 

SOURCE=".\language-scanner.c"

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_LANGUA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\fopen-wrappers.h"\
	".\functions\post.h"\
	".\highlight.h"\
	".\language-parser.tab.h"\
	".\language-scanner.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\language-scanner.obj" : $(SOURCE) $(DEP_CPP_LANGUA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_LANGUA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\fopen-wrappers.h"\
	".\functions\post.h"\
	".\highlight.h"\
	".\language-parser.tab.h"\
	".\language-scanner.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\language-scanner.obj"	"$(INTDIR)\language-scanner.sbr" : $(SOURCE)\
 $(DEP_CPP_LANGUA) "$(INTDIR)"


!ENDIF 

SOURCE=.\list.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_LIST_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\db.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\readdir.h"\
	".\win95nt.h"\
	

"$(INTDIR)\list.obj" : $(SOURCE) $(DEP_CPP_LIST_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_LIST_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\db.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\readdir.h"\
	".\win95nt.h"\
	

"$(INTDIR)\list.obj"	"$(INTDIR)\list.sbr" : $(SOURCE) $(DEP_CPP_LIST_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\main.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_MAIN_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\fopen-wrappers.h"\
	".\functions\basic_functions.h"\
	".\functions\head.h"\
	".\functions\info.h"\
	".\functions\post.h"\
	".\functions\type.h"\
	".\getopt.h"\
	".\highlight.h"\
	".\internal_functions.h"\
	".\language-parser.tab.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\syslog.h"\
	".\win95nt.h"\
	

"$(INTDIR)\main.obj" : $(SOURCE) $(DEP_CPP_MAIN_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_MAIN_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\fopen-wrappers.h"\
	".\functions\basic_functions.h"\
	".\functions\head.h"\
	".\functions\info.h"\
	".\functions\post.h"\
	".\functions\type.h"\
	".\getopt.h"\
	".\highlight.h"\
	".\internal_functions.h"\
	".\language-parser.tab.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win32\syslog.h"\
	".\win95nt.h"\
	

"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) $(DEP_CPP_MAIN_)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\operators.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_OPERA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\number.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\operators.obj" : $(SOURCE) $(DEP_CPP_OPERA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_OPERA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\number.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\snprintf.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\operators.obj"	"$(INTDIR)\operators.sbr" : $(SOURCE)\
 $(DEP_CPP_OPERA) "$(INTDIR)"


!ENDIF 

SOURCE=.\php3_hash.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_PHP3_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\php3_hash.obj" : $(SOURCE) $(DEP_CPP_PHP3_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_PHP3_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\php3_hash.obj"	"$(INTDIR)\php3_hash.sbr" : $(SOURCE)\
 $(DEP_CPP_PHP3_) "$(INTDIR)"


!ENDIF 

SOURCE=.\php3_realpath.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_PHP3_R=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\php3_realpath.obj" : $(SOURCE) $(DEP_CPP_PHP3_R) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_PHP3_R=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\php3_realpath.obj"	"$(INTDIR)\php3_realpath.sbr" : $(SOURCE)\
 $(DEP_CPP_PHP3_R) "$(INTDIR)"


!ENDIF 

SOURCE=.\request_info.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_REQUE=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\request_info.obj" : $(SOURCE) $(DEP_CPP_REQUE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_REQUE=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\request_info.obj"	"$(INTDIR)\request_info.sbr" : $(SOURCE)\
 $(DEP_CPP_REQUE) "$(INTDIR)"


!ENDIF 

SOURCE=.\safe_mode.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_SAFE_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\pageinfo.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\safe_mode.obj" : $(SOURCE) $(DEP_CPP_SAFE_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_SAFE_=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\functions\pageinfo.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\safe_mode.obj"	"$(INTDIR)\safe_mode.sbr" : $(SOURCE)\
 $(DEP_CPP_SAFE_) "$(INTDIR)"


!ENDIF 

SOURCE=.\stack.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_STACK=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\stack.obj" : $(SOURCE) $(DEP_CPP_STACK) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_STACK=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\fopen-wrappers.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\stack.obj"	"$(INTDIR)\stack.sbr" : $(SOURCE) $(DEP_CPP_STACK)\
 "$(INTDIR)"


!ENDIF 

SOURCE=.\token_cache.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_TOKEN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\fopen-wrappers.h"\
	".\highlight.h"\
	".\language-parser.tab.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\token_cache.obj" : $(SOURCE) $(DEP_CPP_TOKEN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_TOKEN=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\fopen-wrappers.h"\
	".\highlight.h"\
	".\language-parser.tab.h"\
	".\main.h"\
	".\mod_php3.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\token_cache.obj"	"$(INTDIR)\token_cache.sbr" : $(SOURCE)\
 $(DEP_CPP_TOKEN) "$(INTDIR)"


!ENDIF 

SOURCE=.\variables.c

!IF  "$(CFG)" == "php3 - Win32 Release"

DEP_CPP_VARIA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\variables.obj" : $(SOURCE) $(DEP_CPP_VARIA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

DEP_CPP_VARIA=\
	".\alloc.h"\
	".\config.w32.h"\
	".\constants.h"\
	".\control_structures.h"\
	".\fopen-wrappers.h"\
	".\functions\head.h"\
	".\internal_functions.h"\
	".\mod_php3.h"\
	".\modules.h"\
	".\operators.h"\
	".\php.h"\
	".\php3_hash.h"\
	".\php3_list.h"\
	".\php_version.h"\
	".\regex\regex.h"\
	".\request_info.h"\
	".\safe_mode.h"\
	".\stack.h"\
	".\token_cache.h"\
	".\variables.h"\
	".\win32\param.h"\
	".\win32\pwd.h"\
	".\win95nt.h"\
	

"$(INTDIR)\variables.obj"	"$(INTDIR)\variables.sbr" : $(SOURCE)\
 $(DEP_CPP_VARIA) "$(INTDIR)"


!ENDIF 

!IF  "$(CFG)" == "php3 - Win32 Release"

"php_custom_build - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\php_custom_build.mak\
 CFG="php_custom_build - Win32 Release" 
   cd "."

"php_custom_build - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\php_custom_build.mak\
 CFG="php_custom_build - Win32 Release" RECURSE=1 
   cd "."

!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

"php_custom_build - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\php_custom_build.mak\
 CFG="php_custom_build - Win32 Debug" 
   cd "."

"php_custom_build - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) CLEAN /F .\php_custom_build.mak\
 CFG="php_custom_build - Win32 Debug" RECURSE=1 
   cd "."

!ENDIF 


!ENDIF 

