/*
	Server API Abstraction Layer

	This set of functions is required to create a new
	api interface for php.  These functions map
	functions that would be used in cgi to server
	specific api's

	Some functions are specific to php so that the main
	php module may obtain information from the server
*/

#ifndef _PHP3_SAPI_H_
#define _PHP3_SAPI_H_
struct sapi_request_info{
	void *scid; /* request identifier */
	char *filename;
	char *path_info;
	char *path_translated;
	char *query_string;
	char *request_method;
	char *script_name;
	char *current_user;
	int current_user_length;
	unsigned int content_length;
	char *content_type;
	char *cookies;
	int cgi, display_source_mode,preprocess,info_only,quiet_mode;
	char *argv0,*ini_path;
	void (*info)(void *);
	void (*puts)(void * , char *);
	void (*putc)(void * , char);
	char *(*getenv)(void * , char *);
	int (*writeclient)(void * , char *, int);
	void (*flush)(void * ); /*flush writes to client*/
	void (*header)(void * , char *); /*write headers to client*/
	int (*readclient)(void * , char *, int , int );
	void (*block)(void *); /*signal blocking*/
	void (*unblock)(void *); /*signal blocking*/
	void (*log)(void *, char *); /*use server error log*/
};

#if !SAPI_INTERFACE
#if !defined(COMPILE_DL)
#define PUTS(a) GLOBAL(sapi_rqst)->puts(GLOBAL(sapi_rqst)->scid,(a))
#define PUTC(a) GLOBAL(sapi_rqst)->putc(GLOBAL(sapi_rqst)->scid,(a))
#define PHPWRITE(a,n) GLOBAL(sapi_rqst)->writeclient(GLOBAL(sapi_rqst)->scid,(a),(n))
#define BLOCK_INTERRUPTIONS
#define UNBLOCK_INTERRUPTIONS
/*
#define BLOCK_INTERRUPTIONS GLOBAL(sapi_rqst)->block(GLOBAL(sapi_rqst)->scid)
#define UNBLOCK_INTERRUPTIONS GLOBAL(sapi_rqst)->unblock(GLOBAL(sapi_rqst)->scid)
*/
#endif
#endif

/*emulates puts*/
extern void sapi_puts(void * scid, char *);

/*emulates putc*/
extern void sapi_putc(void * scid, char);

/*emulates getenv*/
extern char *sapi_getenv(void * scid, char *);

/*writes a string to the client*/
extern int sapi_writeclient(void * scid, char *, int);

/*flushes any writes to the client*/
extern void sapi_flush(void * scid);

/*sends a header to the client, adds \n\r to the end*/
extern void sapi_header(void * scid, char *header);

/*reads info from the client, used to get post*/
extern int sapi_readclient(void * scid, char *buf, int size, int len);

/*signal blocking so a signal doesnt interrupt us in a bad place (used in apache) */
extern void sapi_block(void *);
extern void sapi_unblock(void *);

/*logs messages to servers error log*/
extern void sapi_log(void *scid, char *log_message);

/* this is the main function in main.c  Initialization 
 * may be required before calling this function.
 */
extern PHPAPI int php3_sapi_main(struct sapi_request_info *sapi_info);

extern void sapi_print_info(void *);

#endif