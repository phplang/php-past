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
	-@erase "$(INTDIR)\iptc.obj"
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
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wfile.obj"
	-@erase "$(INTDIR)\winsnmp.obj"
	-@erase "$(INTDIR)\winutil.obj"
	-@erase "$(INTDIR)\wsyslog.obj"
	-@erase "$(OUTDIR)\php.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "." /I "regex\\" /I "d:\src\bind\include" /D "NDEBUG" /D "MSVC5" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\php3.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\php3.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=lib44bsd95.lib resolv.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3.0 /subsystem:console /incremental:no /pdb:"$(OUTDIR)\php.pdb" /machine:I386 /out:"$(OUTDIR)\php.exe" /libpath:"\src\lib" /libpath:"d:\src\bind\lib" 
LINK32_OBJS= \
	"$(INTDIR)\COM.obj" \
	"$(INTDIR)\pwd.obj" \
	"$(INTDIR)\readdir.obj" \
	"$(INTDIR)\sendmail.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\wfile.obj" \
	"$(INTDIR)\winsnmp.obj" \
	"$(INTDIR)\winutil.obj" \
	"$(INTDIR)\wsyslog.obj" \
	"$(INTDIR)\apache.obj" \
	"$(INTDIR)\base64.obj" \
	"$(INTDIR)\basic_functions.obj" \
	"$(INTDIR)\bcmath.obj" \
	"$(INTDIR)\browscap.obj" \
	"$(INTDIR)\datetime.obj" \
	"$(INTDIR)\dir.obj" \
	"$(INTDIR)\dl.obj" \
	"$(INTDIR)\dns.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\filestat.obj" \
	"$(INTDIR)\formatted_print.obj" \
	"$(INTDIR)\fsock.obj" \
	"$(INTDIR)\head.obj" \
	"$(INTDIR)\html.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\info.obj" \
	"$(INTDIR)\iptc.obj" \
	"$(INTDIR)\link.obj" \
	"$(INTDIR)\mail.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\microtime.obj" \
	"$(INTDIR)\mime.obj" \
	"$(INTDIR)\number.obj" \
	"$(INTDIR)\pack.obj" \
	"$(INTDIR)\pageinfo.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\rand.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\soundex.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\syslog.obj" \
	"$(INTDIR)\type.obj" \
	"$(INTDIR)\unified_odbc.obj" \
	"$(INTDIR)\uniqid.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\var.obj" \
	"$(INTDIR)\regcomp.obj" \
	"$(INTDIR)\regerror.obj" \
	"$(INTDIR)\regexec.obj" \
	"$(INTDIR)\regfree.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\configuration-parser.tab.obj" \
	"$(INTDIR)\configuration-scanner.obj" \
	"$(INTDIR)\constants.obj" \
	"$(INTDIR)\debugger.obj" \
	"$(INTDIR)\fopen-wrappers.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\highlight.obj" \
	"$(INTDIR)\internal_functions.obj" \
	"$(INTDIR)\language-parser.tab.obj" \
	"$(INTDIR)\language-scanner.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\operators.obj" \
	"$(INTDIR)\php3_hash.obj" \
	"$(INTDIR)\php3_realpath.obj" \
	"$(INTDIR)\request_info.obj" \
	"$(INTDIR)\safe_mode.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\token_cache.obj" \
	"$(INTDIR)\variables.obj"

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
	-@erase "$(INTDIR)\iptc.obj"
	-@erase "$(INTDIR)\iptc.sbr"
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
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
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

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "regex\\" /I "d:\src\bind\include" /D "DEBUG" /D "_DEBUG" /D "MSVC5" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\php3.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\php3.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\COM.sbr" \
	"$(INTDIR)\pwd.sbr" \
	"$(INTDIR)\readdir.sbr" \
	"$(INTDIR)\sendmail.sbr" \
	"$(INTDIR)\time.sbr" \
	"$(INTDIR)\wfile.sbr" \
	"$(INTDIR)\winsnmp.sbr" \
	"$(INTDIR)\winutil.sbr" \
	"$(INTDIR)\wsyslog.sbr" \
	"$(INTDIR)\apache.sbr" \
	"$(INTDIR)\base64.sbr" \
	"$(INTDIR)\basic_functions.sbr" \
	"$(INTDIR)\bcmath.sbr" \
	"$(INTDIR)\browscap.sbr" \
	"$(INTDIR)\datetime.sbr" \
	"$(INTDIR)\dir.sbr" \
	"$(INTDIR)\dl.sbr" \
	"$(INTDIR)\dns.sbr" \
	"$(INTDIR)\exec.sbr" \
	"$(INTDIR)\file.sbr" \
	"$(INTDIR)\filestat.sbr" \
	"$(INTDIR)\formatted_print.sbr" \
	"$(INTDIR)\fsock.sbr" \
	"$(INTDIR)\head.sbr" \
	"$(INTDIR)\html.sbr" \
	"$(INTDIR)\image.sbr" \
	"$(INTDIR)\info.sbr" \
	"$(INTDIR)\iptc.sbr" \
	"$(INTDIR)\link.sbr" \
	"$(INTDIR)\mail.sbr" \
	"$(INTDIR)\math.sbr" \
	"$(INTDIR)\md5.sbr" \
	"$(INTDIR)\microtime.sbr" \
	"$(INTDIR)\mime.sbr" \
	"$(INTDIR)\number.sbr" \
	"$(INTDIR)\pack.sbr" \
	"$(INTDIR)\pageinfo.sbr" \
	"$(INTDIR)\post.sbr" \
	"$(INTDIR)\rand.sbr" \
	"$(INTDIR)\reg.sbr" \
	"$(INTDIR)\soundex.sbr" \
	"$(INTDIR)\string.sbr" \
	"$(INTDIR)\syslog.sbr" \
	"$(INTDIR)\type.sbr" \
	"$(INTDIR)\unified_odbc.sbr" \
	"$(INTDIR)\uniqid.sbr" \
	"$(INTDIR)\url.sbr" \
	"$(INTDIR)\var.sbr" \
	"$(INTDIR)\regcomp.sbr" \
	"$(INTDIR)\regerror.sbr" \
	"$(INTDIR)\regexec.sbr" \
	"$(INTDIR)\regfree.sbr" \
	"$(INTDIR)\alloc.sbr" \
	"$(INTDIR)\configuration-parser.tab.sbr" \
	"$(INTDIR)\configuration-scanner.sbr" \
	"$(INTDIR)\constants.sbr" \
	"$(INTDIR)\debugger.sbr" \
	"$(INTDIR)\fopen-wrappers.sbr" \
	"$(INTDIR)\getopt.sbr" \
	"$(INTDIR)\highlight.sbr" \
	"$(INTDIR)\internal_functions.sbr" \
	"$(INTDIR)\language-parser.tab.sbr" \
	"$(INTDIR)\language-scanner.sbr" \
	"$(INTDIR)\list.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\operators.sbr" \
	"$(INTDIR)\php3_hash.sbr" \
	"$(INTDIR)\php3_realpath.sbr" \
	"$(INTDIR)\request_info.sbr" \
	"$(INTDIR)\safe_mode.sbr" \
	"$(INTDIR)\stack.sbr" \
	"$(INTDIR)\token_cache.sbr" \
	"$(INTDIR)\variables.sbr"

"$(OUTDIR)\php3.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=lib44bsd95.lib resolv.lib winmm.lib wsock32.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /version:3.0 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\php.pdb" /debug /machine:I386 /out:"$(OUTDIR)\php.exe" /pdbtype:sept /libpath:"\src\lib" /libpath:"d:\src\bind\lib" 
LINK32_OBJS= \
	"$(INTDIR)\COM.obj" \
	"$(INTDIR)\pwd.obj" \
	"$(INTDIR)\readdir.obj" \
	"$(INTDIR)\sendmail.obj" \
	"$(INTDIR)\time.obj" \
	"$(INTDIR)\wfile.obj" \
	"$(INTDIR)\winsnmp.obj" \
	"$(INTDIR)\winutil.obj" \
	"$(INTDIR)\wsyslog.obj" \
	"$(INTDIR)\apache.obj" \
	"$(INTDIR)\base64.obj" \
	"$(INTDIR)\basic_functions.obj" \
	"$(INTDIR)\bcmath.obj" \
	"$(INTDIR)\browscap.obj" \
	"$(INTDIR)\datetime.obj" \
	"$(INTDIR)\dir.obj" \
	"$(INTDIR)\dl.obj" \
	"$(INTDIR)\dns.obj" \
	"$(INTDIR)\exec.obj" \
	"$(INTDIR)\file.obj" \
	"$(INTDIR)\filestat.obj" \
	"$(INTDIR)\formatted_print.obj" \
	"$(INTDIR)\fsock.obj" \
	"$(INTDIR)\head.obj" \
	"$(INTDIR)\html.obj" \
	"$(INTDIR)\image.obj" \
	"$(INTDIR)\info.obj" \
	"$(INTDIR)\iptc.obj" \
	"$(INTDIR)\link.obj" \
	"$(INTDIR)\mail.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\microtime.obj" \
	"$(INTDIR)\mime.obj" \
	"$(INTDIR)\number.obj" \
	"$(INTDIR)\pack.obj" \
	"$(INTDIR)\pageinfo.obj" \
	"$(INTDIR)\post.obj" \
	"$(INTDIR)\rand.obj" \
	"$(INTDIR)\reg.obj" \
	"$(INTDIR)\soundex.obj" \
	"$(INTDIR)\string.obj" \
	"$(INTDIR)\syslog.obj" \
	"$(INTDIR)\type.obj" \
	"$(INTDIR)\unified_odbc.obj" \
	"$(INTDIR)\uniqid.obj" \
	"$(INTDIR)\url.obj" \
	"$(INTDIR)\var.obj" \
	"$(INTDIR)\regcomp.obj" \
	"$(INTDIR)\regerror.obj" \
	"$(INTDIR)\regexec.obj" \
	"$(INTDIR)\regfree.obj" \
	"$(INTDIR)\alloc.obj" \
	"$(INTDIR)\configuration-parser.tab.obj" \
	"$(INTDIR)\configuration-scanner.obj" \
	"$(INTDIR)\constants.obj" \
	"$(INTDIR)\debugger.obj" \
	"$(INTDIR)\fopen-wrappers.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\highlight.obj" \
	"$(INTDIR)\internal_functions.obj" \
	"$(INTDIR)\language-parser.tab.obj" \
	"$(INTDIR)\language-scanner.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\operators.obj" \
	"$(INTDIR)\php3_hash.obj" \
	"$(INTDIR)\php3_realpath.obj" \
	"$(INTDIR)\request_info.obj" \
	"$(INTDIR)\safe_mode.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\token_cache.obj" \
	"$(INTDIR)\variables.obj"

"$(OUTDIR)\php.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("php3.dep")
!INCLUDE "php3.dep"
!ELSE 
!MESSAGE Warning: cannot find "php3.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "php3 - Win32 Release" || "$(CFG)" == "php3 - Win32 Debug"
SOURCE=.\functions\COM.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\COM.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\COM.obj"	"$(INTDIR)\COM.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\pwd.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\pwd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\pwd.obj"	"$(INTDIR)\pwd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\readdir.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\readdir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\readdir.obj"	"$(INTDIR)\readdir.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\sendmail.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\sendmail.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\sendmail.obj"	"$(INTDIR)\sendmail.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\time.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\time.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\time.obj"	"$(INTDIR)\time.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\wfile.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\wfile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\wfile.obj"	"$(INTDIR)\wfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\dl\snmp\winsnmp.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\winsnmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\winsnmp.obj"	"$(INTDIR)\winsnmp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\winutil.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\winutil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\winutil.obj"	"$(INTDIR)\winutil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\wsyslog.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\wsyslog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\wsyslog.obj"	"$(INTDIR)\wsyslog.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\apache.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\apache.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\apache.obj"	"$(INTDIR)\apache.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\base64.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\base64.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\base64.obj"	"$(INTDIR)\base64.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\basic_functions.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\basic_functions.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\basic_functions.obj"	"$(INTDIR)\basic_functions.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\bcmath.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\bcmath.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\bcmath.obj"	"$(INTDIR)\bcmath.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\browscap.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\browscap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\browscap.obj"	"$(INTDIR)\browscap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\datetime.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\datetime.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\datetime.obj"	"$(INTDIR)\datetime.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\dir.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\dir.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\dir.obj"	"$(INTDIR)\dir.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\dl.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\dl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\dl.obj"	"$(INTDIR)\dl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\dns.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\dns.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\dns.obj"	"$(INTDIR)\dns.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\exec.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\exec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\exec.obj"	"$(INTDIR)\exec.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\file.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\file.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\file.obj"	"$(INTDIR)\file.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\filestat.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\filestat.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\filestat.obj"	"$(INTDIR)\filestat.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\formatted_print.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\formatted_print.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\formatted_print.obj"	"$(INTDIR)\formatted_print.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\fsock.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\fsock.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\fsock.obj"	"$(INTDIR)\fsock.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\head.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\head.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\head.obj"	"$(INTDIR)\head.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\html.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\html.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\html.obj"	"$(INTDIR)\html.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\image.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\image.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\image.obj"	"$(INTDIR)\image.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\info.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\info.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\info.obj"	"$(INTDIR)\info.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\iptc.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\iptc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\iptc.obj"	"$(INTDIR)\iptc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\link.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\link.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\link.obj"	"$(INTDIR)\link.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\mail.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\mail.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\mail.obj"	"$(INTDIR)\mail.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\math.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\math.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\math.obj"	"$(INTDIR)\math.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\md5.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\microtime.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\microtime.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\microtime.obj"	"$(INTDIR)\microtime.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\mime.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\mime.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\mime.obj"	"$(INTDIR)\mime.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\number.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\number.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\number.obj"	"$(INTDIR)\number.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\pack.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\pack.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\pack.obj"	"$(INTDIR)\pack.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\pageinfo.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\pageinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\pageinfo.obj"	"$(INTDIR)\pageinfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\post.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\post.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\post.obj"	"$(INTDIR)\post.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\rand.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\rand.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\rand.obj"	"$(INTDIR)\rand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\reg.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\reg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\reg.obj"	"$(INTDIR)\reg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\soundex.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\soundex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\soundex.obj"	"$(INTDIR)\soundex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\string.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\string.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\string.obj"	"$(INTDIR)\string.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\syslog.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\syslog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\syslog.obj"	"$(INTDIR)\syslog.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\type.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\type.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\type.obj"	"$(INTDIR)\type.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\unified_odbc.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\unified_odbc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\unified_odbc.obj"	"$(INTDIR)\unified_odbc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\uniqid.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\uniqid.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\uniqid.obj"	"$(INTDIR)\uniqid.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\url.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\url.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\url.obj"	"$(INTDIR)\url.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\functions\var.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\var.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\var.obj"	"$(INTDIR)\var.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\regex\regcomp.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\regcomp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\regcomp.obj"	"$(INTDIR)\regcomp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\regex\regerror.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\regerror.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\regerror.obj"	"$(INTDIR)\regerror.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\regex\regexec.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\regexec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\regexec.obj"	"$(INTDIR)\regexec.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\regex\regfree.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\regfree.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\regfree.obj"	"$(INTDIR)\regfree.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\alloc.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\alloc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\alloc.obj"	"$(INTDIR)\alloc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=".\configuration-parser.tab.c"

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\configuration-parser.tab.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\configuration-parser.tab.obj"	"$(INTDIR)\configuration-parser.tab.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=".\configuration-scanner.c"

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\configuration-scanner.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\configuration-scanner.obj"	"$(INTDIR)\configuration-scanner.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\constants.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\constants.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\constants.obj"	"$(INTDIR)\constants.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\debugger.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\debugger.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\debugger.obj"	"$(INTDIR)\debugger.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=".\fopen-wrappers.c"

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\fopen-wrappers.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\fopen-wrappers.obj"	"$(INTDIR)\fopen-wrappers.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\getopt.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\getopt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\getopt.obj"	"$(INTDIR)\getopt.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\highlight.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\highlight.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\highlight.obj"	"$(INTDIR)\highlight.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\internal_functions.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\internal_functions.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\internal_functions.obj"	"$(INTDIR)\internal_functions.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=".\language-parser.tab.c"

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\language-parser.tab.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\language-parser.tab.obj"	"$(INTDIR)\language-parser.tab.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=".\language-scanner.c"

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\language-scanner.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\language-scanner.obj"	"$(INTDIR)\language-scanner.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\list.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\list.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\list.obj"	"$(INTDIR)\list.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\main.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\operators.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\operators.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\operators.obj"	"$(INTDIR)\operators.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\php3_hash.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\php3_hash.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\php3_hash.obj"	"$(INTDIR)\php3_hash.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\php3_realpath.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\php3_realpath.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\php3_realpath.obj"	"$(INTDIR)\php3_realpath.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\request_info.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\request_info.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\request_info.obj"	"$(INTDIR)\request_info.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\safe_mode.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\safe_mode.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\safe_mode.obj"	"$(INTDIR)\safe_mode.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stack.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\stack.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\stack.obj"	"$(INTDIR)\stack.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\token_cache.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\token_cache.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\token_cache.obj"	"$(INTDIR)\token_cache.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\variables.c

!IF  "$(CFG)" == "php3 - Win32 Release"


"$(INTDIR)\variables.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"


"$(INTDIR)\variables.obj"	"$(INTDIR)\variables.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

!IF  "$(CFG)" == "php3 - Win32 Release"

"php_custom_build - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\php_custom_build.mak CFG="php_custom_build - Win32 Release" 
   cd "."

"php_custom_build - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\php_custom_build.mak CFG="php_custom_build - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "php3 - Win32 Debug"

"php_custom_build - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\php_custom_build.mak CFG="php_custom_build - Win32 Debug" 
   cd "."

"php_custom_build - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F .\php_custom_build.mak CFG="php_custom_build - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

