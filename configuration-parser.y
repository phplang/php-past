%{
/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors: Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */



/* $Id: configuration-parser.y,v 1.61 1998/02/12 18:30:37 zeev Exp $ */

#define DEBUG_CFG_PARSER 1
#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "modules.h"
#include "functions/dl.h"
#include "functions/file.h"
#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#include "win32/wfile.h"
#endif

#define PARSING_MODE_CFG 0
#define PARSING_MODE_BROWSCAP 1

static HashTable configuration_hash;
#ifndef THREAD_SAFE
extern HashTable browser_hash;
#endif
static HashTable *active_hash_table;
static YYSTYPE *current_section;
static char *currently_parsed_filename;

static int parsing_mode;

YYSTYPE yylval;

extern int cfglex(YYSTYPE *cfglval);
extern FILE *cfgin;
extern int cfglineno;
extern void init_cfg_scanner(void);

PHPAPI int cfg_get_long(char *varname,long *result)
{
	YYSTYPE *tmp,var;
	
	if (hash_find(&configuration_hash,varname,strlen(varname)+1,(void **) &tmp)==FAILURE) {
		*result=(long)NULL;
		return FAILURE;
	}
	var = *tmp;
	yystype_copy_constructor(&var);
	convert_to_long(&var);
	*result = var.value.lval;
	return SUCCESS;
}


PHPAPI int cfg_get_double(char *varname,double *result)
{
	YYSTYPE *tmp,var;
	
	if (hash_find(&configuration_hash,varname,strlen(varname)+1,(void **) &tmp)==FAILURE) {
		*result=(double)0;
		return FAILURE;
	}
	var = *tmp;
	yystype_copy_constructor(&var);
	convert_to_double(&var);
	*result = var.value.dval;
	return SUCCESS;
}


PHPAPI int cfg_get_string(char *varname, char **result)
{
	YYSTYPE *tmp;

	if (hash_find(&configuration_hash,varname,strlen(varname)+1,(void **) &tmp)==FAILURE) {
		*result=NULL;
		return FAILURE;
	}
	*result = tmp->value.strval;
	return SUCCESS;
}


static void yyerror(char *str)
{
	fprintf(stderr,"PHP:  Error parsing %s on line %d\n",currently_parsed_filename,cfglineno);
}


static void yystype_config_destructor(YYSTYPE *yystype)
{
	if (yystype->type == IS_STRING && yystype->value.strval != empty_string) {
		free(yystype->value.strval);
	}
}


static void yystype_browscap_destructor(YYSTYPE *yystype)
{
	if (yystype->type == IS_OBJECT || yystype->type == IS_ARRAY) {
		hash_destroy(yystype->value.ht);
		free(yystype->value.ht);
	}
}


int php3_init_config(void)
{
	if (hash_init(&configuration_hash, 0, NULL, (void (*)(void *))yystype_config_destructor, 1)==FAILURE) {
		return FAILURE;
	}

#if USE_CONFIG_FILE
	{
		char *env_location,*default_location,*php_ini_path;
		int safe_mode_state = php3_ini.safe_mode;
		char *opened_path;
		
		env_location = getenv("PHPRC");
		if (!env_location) {
			env_location="";
		}
#if WIN32|WINNT
		{
			default_location = (char *) malloc(512);

			if (!GetWindowsDirectory(default_location,255)) {
				default_location[0]=0;
			}
		}
#else
		default_location = CONFIGURATION_FILE_PATH;
#endif

/* build a path */
		php_ini_path = (char *) malloc(sizeof(".")+strlen(env_location)+strlen(default_location)+2+1);

#if WIN32|WINNT
		sprintf(php_ini_path,".;%s;%s",env_location,default_location);
#else
		sprintf(php_ini_path,".:%s:%s",env_location,default_location);
#endif

		php3_ini.safe_mode = 0;
		cfgin = php3_fopen_with_path("php3.ini","r",php_ini_path,&opened_path);
		free(php_ini_path);
		php3_ini.safe_mode = safe_mode_state;

		if (!cfgin) {
# if WIN32|WINNT
			return FAILURE;
# else
			return SUCCESS;  /* having no configuration file is ok */
# endif
		}

		if (opened_path) {
			YYSTYPE tmp;
			
			tmp.value.strval = opened_path;
			tmp.strlen = strlen(opened_path);
			tmp.type = IS_STRING;
			hash_update(&configuration_hash,"cfg_file_path",sizeof("cfg_file_path"),(void *) &tmp,sizeof(YYSTYPE),NULL);
#if 0
			php3_printf("INI file opened at '%s'\n",opened_path);
#endif
		}
			
		init_cfg_scanner();
		active_hash_table = &configuration_hash;
		parsing_mode = PARSING_MODE_CFG;
		currently_parsed_filename = "php3.ini";
		yyparse();
		fclose(cfgin);
	}
	
#endif
	
	return SUCCESS;
}


int php3_minit_browscap(INITFUNCARGS)
{
	TLS_VARS;

	if (php3_ini.browscap) {
		if (hash_init(&GLOBAL(browser_hash), 0, NULL, (void (*)(void *))yystype_browscap_destructor, 1)==FAILURE) {
			return FAILURE;
		}

		cfgin = fopen(php3_ini.browscap,"r");
		if (!cfgin) {
			php3_error(E_WARNING,"Cannot open '%s' for reading",php3_ini.browscap);
			return FAILURE;
		}
		init_cfg_scanner();
		active_hash_table = &GLOBAL(browser_hash);
		parsing_mode = PARSING_MODE_BROWSCAP;
		currently_parsed_filename = php3_ini.browscap;
		yyparse();
		fclose(cfgin);
	}

	return SUCCESS;
}


int php3_shutdown_config(void)
{
	hash_destroy(&configuration_hash);
	return SUCCESS;
}


int php3_mshutdown_browscap(void)
{
	TLS_VARS;

	if (php3_ini.browscap) {
		hash_destroy(&GLOBAL(browser_hash));
	}
	return SUCCESS;
}


static void convert_browscap_pattern(YYSTYPE *pattern)
{
	register int i,j;
	char *t;

	for (i=0; i<pattern->strlen; i++) {
		if (pattern->value.strval[i]=='*' || pattern->value.strval[i]=='?') {
			break;
		}
	}

	if (i==pattern->strlen) { /* no wildcards */
		return;
	}

	t = (char *) malloc(pattern->strlen*2);
	
	for (i=0,j=0; i<pattern->strlen; i++,j++) {
		switch (pattern->value.strval[i]) {
			case '?':
				t[j] = '.';
				break;
			case '*':
				t[j++] = '.';
				t[j] = '*';
				break;
			case '.':
				t[j++] = '\\';
				t[j] = '.';
				break;
			default:
				t[j] = pattern->value.strval[i];
				break;
		}
	}
	t[j]=0;
	free(pattern->value.strval);
	pattern->value.strval = t;
	pattern->strlen = j;
	return;
}

%}

%pure_parser
%token STRING
%token ENCAPSULATED_STRING
%token SECTION
%token TRUE
%token FALSE
%token EXTENSION

%%

statement_list:
		statement_list statement
	|	/* empty */
;

statement:
		string '=' string_or_value {
#if 0
			printf("'%s' = '%s'\n",$1.value.strval,$3.value.strval);
#endif
			$3.type = IS_STRING;
			if (parsing_mode==PARSING_MODE_CFG) {
				hash_update(active_hash_table, $1.value.strval, $1.strlen+1, &$3, sizeof(YYSTYPE), NULL);
			} else if (parsing_mode==PARSING_MODE_BROWSCAP) {
				php3_str_tolower($1.value.strval,$1.strlen);
				hash_update(current_section->value.ht, $1.value.strval, $1.strlen+1, &$3, sizeof(YYSTYPE), NULL);
			}
			free($1.value.strval);
		}
	|	string { free($1.value.strval); }
	|	EXTENSION '=' string {
			YYSTYPE dummy;
			
			php3_dl(&$3,MODULE_PERSISTENT,&dummy);
		}
	|	SECTION { 
			if (parsing_mode==PARSING_MODE_BROWSCAP) {
				YYSTYPE tmp;

				/*printf("'%s' (%d)\n",$1.value.strval,$1.strlen+1);*/
				tmp.value.ht = (HashTable *) malloc(sizeof(HashTable));
				hash_init(tmp.value.ht, 0, NULL, (void (*)(void *))yystype_config_destructor, 1);
				tmp.type = IS_OBJECT;
				hash_update(active_hash_table, $1.value.strval, $1.strlen+1, (void *) &tmp, sizeof(YYSTYPE), (void **) &current_section);
				tmp.value.strval = php3_strndup($1.value.strval,$1.strlen);
				tmp.strlen = $1.strlen;
				tmp.type = IS_STRING;
				convert_browscap_pattern(&tmp);
				hash_update(current_section->value.ht,"browser_name_pattern",sizeof("browser_name_pattern"),(void *) &tmp, sizeof(YYSTYPE), NULL);
			}
			free($1.value.strval);
		}
	|	'\n'
;


string:
		STRING { $$ = $1; }
	|	ENCAPSULATED_STRING { $$ = $1; }
;

string_or_value:
		string { $$ = $1; }
	|	TRUE { $$ = $1; }
	|	FALSE { $$ = $1; }
	|	'\n' { $$.value.strval = strdup(""); $$.strlen=0; $$.type = IS_STRING; }
;


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
