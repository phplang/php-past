/*
	Server API Abstraction Layer

	functions for CGI

*/
#include <stdio.h>
#include <stdarg.h>
#include "parser.h"
#include "getopt.h"
#define SAPI_INTERFACE 1
#include "sapi.h"

/* for cgi this can be global */
struct sapi_request_info sapi_info;

/* sapi functions */
int sapi_readclient(void *scid, char *buf, int size, int len){
	return fread(buf, size, len, stdin);
}

char *sapi_getenv(void *scid, char *string){
	return getenv(string);
}

void sapi_header(void *scid, char *header){
	puts(header);
}

void sapi_flush(void *scid){
	fflush(stdout);
}

void sapi_puts(void *scid, char *string){
	puts(string);
}

void sapi_putc(void *scid, char c){
	putc(c,stdout);
}

int sapi_writeclient(void *scid, char *string, int len){
	return fwrite(string,len,1,stdout);
}

void sapi_block(void *scid){}
void sapi_unblock(void *scid){}

void sapi_init(struct sapi_request_info *sapi_info)
{
	char *buf;

	/* for cgi we dont use request id's */
	sapi_info->scid=NULL;

	if(!sapi_info->filename){
		sapi_info->filename=NULL;
	}
	if(!sapi_info->path_info){
		sapi_info->path_info = getenv("PATH_INFO");
	}
	sapi_info->path_translated = getenv("PATH_TRANSLATED");
	if(!sapi_info->query_string){
		sapi_info->query_string = getenv("QUERY_STRING");
	}
	sapi_info->current_user = NULL;
	sapi_info->current_user_length=0;
	sapi_info->request_method = getenv("REQUEST_METHOD");
	sapi_info->script_name = getenv("SCRIPT_NAME");
	buf = getenv("CONTENT_LENGTH");
	sapi_info->content_length = (buf ? atoi(buf) : 0);
	sapi_info->content_type = getenv("CONTENT_TYPE");
	sapi_info->cookies = getenv("HTTP_COOKIE");
	sapi_info->puts=sapi_puts;
	sapi_info->putc=sapi_putc;
	sapi_info->getenv=sapi_getenv;
	sapi_info->writeclient=sapi_writeclient;
	sapi_info->flush=sapi_flush;
	sapi_info->header=sapi_header;
	sapi_info->readclient=sapi_readclient;
	sapi_info->block=sapi_block;
	sapi_info->unblock=sapi_unblock;
}
/* php cgi functions */
static void
cgi_usage(char *argv0)
{
	char *prog;

	prog = strrchr(argv0, '/');
	if (prog) {
		prog++;
	} else {
		prog = "php";
	}

	printf("Usage: %s [-q] [-h]"
				" [-s]"
				" [-v] [-i] [-f <file>] | "
				"{<file> [args...]}\n"
				"  -q       Quiet-mode.  Suppress HTTP Header output.\n"
			 	"  -s       Display colour syntax highlighted source.\n"
				"  -f<file> Parse <file>.  Implies `-q'\n"
				"  -v       Version number\n"
				"  -p       Pretokenize a script (creates a .php3p file)\n"
				"  -e       Execute a pretokenized (.php3p) script\n"
				"  -i       PHP information\n"
				"  -h       This help\n", prog);
}

/* some systems are missing these from their header files */
extern char *optarg;
extern int optind;

int main(int argc, char *argv[])
{
	int arg = 1, c, i, len;
	FILE *in = NULL;
	char *s;
	sapi_info.cgi=0;
	sapi_info.quiet_mode=0;
	sapi_info.preprocess=0;
	sapi_info.info_only=0;
	sapi_info.display_source_mode = 0;
	sapi_info.filename=NULL;

	while ((c = getopt(argc, argv, "f:qvishpe?v")) != -1) {
		switch (c) {
			case 'f':
				sapi_info.filename = estrdup(optarg);
			case 'q':
				sapi_info.quiet_mode=1;
				break;
			case 'v':
				printf("%s\n", PHP_VERSION);
				exit(1);
				break;
			case 'i':
				sapi_info.info_only=1;
				break;
			case 'p': /* preprocess */
				sapi_info.preprocess=1;
				break;
			case 'e': /* execute preprocessed script */
				sapi_info.preprocess=2;
				break;
			case 's':
				sapi_info.display_source_mode = 1;
				break;
			case 'h':
			case '?':
				cgi_usage(argv[0]);
				exit(1);
				break;
			default:
				printf("Warning: unrecognized option `-%c'\n", c);
				break;
		}
		arg++;
	}

	if ((sapi_info.path_info=getenv("PATH_INFO"))) {
		sapi_info.cgi = 1;
	} else if(!(sapi_info.query_string=getenv("QUERY_STRING"))) {
		for(i=arg,len=0; i<argc; i++) {
			len+=strlen(argv[i])+1;
		}
		s = malloc(len*sizeof(char)+1); /* leak - but only for command line version, so ok */
		*s='\0';                        /* we are pretending it came from the environment  */
		s[len-1]='\0';
		for(i=arg,len=0; i<argc; i++) {
			strcat(s,argv[i]);
			if(i<(argc-1)) strcat(s,"+");	
		}
		sapi_info.query_string = s;
	}

	if (sapi_info.filename == NULL && !sapi_info.cgi && argc > arg) {
		sapi_info.filename = estrdup(argv[arg]);
	} else if (sapi_info.cgi) {
		sapi_info.filename=NULL;
	}

		sapi_init(&sapi_info);

	return php3_sapi_main(&sapi_info);
}

