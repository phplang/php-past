<!SGML "ISO 8879:1986"

-- Local SGML declaration used at ART
   Baseline is SGMLS default declaration,
   with increased CAPACITY settings for large documents,
   and increased QUANTITY settings for reasonable DTDs

   "Stolen" by Steve Tynor (SDT) from a comp.text.sgml post by 
   Joe English <jenglish@crl.com>. 
--

CHARSET

-- SDT{ --
     BASESET   "ISO Registration Number 100//CHARSET
                ECMA-94 Right Part of Latin Alphabet Nr. 1//ESC 2/13 4/1"
        DESCSET 0  9 UNUSED
                9  2  9
               11  2 UNUSED
               13  1 13
               14 18 UNUSED
               32 95 32
              127  1 UNUSED 
	       128 32 UNUSED
               160 95 32
               255  1 UNUSED
-- }SDT --

--SDT{
        BASESET  "ISO 646-1983//CHARSET
                International Reference Version (IRV)//ESC 2/5 4/0"
        DESCSET 0  9 UNUSED
                9  2  9
               11  2 UNUSED
               13  1 13
               14 18 UNUSED
               32 95 32
              127  1 UNUSED 
   }SDT --
CAPACITY SGMLREF
	-- all quantities default to 35000 in reference capacity set --
	TOTALCAP	500000
	GRPCAP  	500000
	MAPCAP		100000

SCOPE    DOCUMENT

SYNTAX  
        SHUNCHAR CONTROLS    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
                        17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 127 255
        BASESET    "ISO 646-1983//CHARSET
                    International Reference Version (IRV)//ESC 2/5 4/0"
        DESCSET  0 128 0
        FUNCTION
                RE          	13
                RS          	10
                SPACE       	32
                TAB	SEPCHAR 9
        NAMING
                LCNMSTRT    	""
                UCNMSTRT    	""
                LCNMCHAR    	"-."
                UCNMCHAR    	"-."
                NAMECASE 
				GENERAL YES
				ENTITY NO
        DELIM
                GENERAL     	SGMLREF
                SHORTREF    	SGMLREF
        NAMES   SGMLREF

	-- All "interesting" quantity parameters are included here,
	   set to the default value unless otherwise noted.
	   Values used by other applications also included for reference.
	--
        QUANTITY SGMLREF
                ATTCNT          80	--  default: 40; 
					    rainbow: 80;
					    docbook: 256 --
                ATTSPLEN        960
                ENTLVL          16
                GRPCNT          200	-- default: 32 --
                GRPGTCNT        250	-- default: 96 --
                GRPLVL          16
                LITLEN          8092	--  default: 240 
					    HTML: 1024;
					    rainbow: 2048 
					    docbook: 8092 --
                NAMELEN         72	-- default: 8 
					   HTML: 72  (was 34)
					   docbook: 44 --
                PILEN           240
                TAGLEN          960
                TAGLVL          100	--  default: 24 
					    docbook: 100 --

FEATURES
        MINIMIZE
                DATATAG         NO
                OMITTAG         YES
                RANK            NO
                SHORTTAG        YES
        LINK
                SIMPLE  NO
                IMPLICIT NO
                EXPLICIT NO
        OTHER
                CONCUR  NO
                SUBDOC  YES	999999
                FORMAL  NO

APPINFO NONE
>

