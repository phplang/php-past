typedef union {
    time_t		Number;
    enum _MERIDIAN	Meridian;
} YYSTYPE;
#define	tDAY	257
#define	tDAYZONE	258
#define	tMERIDIAN	259
#define	tMONTH	260
#define	tMONTH_UNIT	261
#define	tSEC_UNIT	262
#define	tSNUMBER	263
#define	tUNUMBER	264
#define	tZONE	265


extern YYSTYPE yylval;
