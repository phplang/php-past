/* $Id: crypt.c,v 1.15 2000/02/20 18:40:32 eschmid Exp $ */

#include "../phpdl.h"
#include "prelude.h"
#include "sflcryp.h"
#ifdef HAVE_CRYPT_H
#if 0
#include <ufc-crypt.h>
#endif
#include <crypt.h>
#endif

static int
    crypt_block_size [] = {             /*  Block size for each algorithm    */
       8, 32, 8, 16
    };

/* emulate rint */
inline int rup(double n)
{
	double i, f;
	f = modf(n, &i);
	if (f > 0)
		i++;
	return (word)i;
}

/*
(string) encrypt(data,type,key)
(string) decrypt(data,type,key)

	encrypts or decrypts data
	(string)data
	(int)type
	(string)key

	type can be 0 to 3:
	IDEA = 0
	MDC = 1
	DES = 2
	XOR = 3

    IDEA      data is at least 8 bytes long, key is 16 bytes.
    MDC       data is at least 8 bytes long, key is 8 bytes.
    DES       data is at least 32 bytes long, key is 16 bytes.
    XOR       data is at least 16 bytes long, key is 16 bytes.

  returns encrypted or decrypted string.

  NOTES:
 IDEA is registered as the international patent WO 91/18459
 "Device for Converting a Digital Block and the Use thereof".
 For commercial use of IDEA, you should contact:
      ASCOM TECH AG
      Freiburgstrasse 370
      CH-3018 Bern, Switzerland
*/


/* {{{ proto string encrypt(string data, int type, string key)
   Encrypts string with key using algorithm type */
DLEXPORT void sflcrypt_encrypt(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *data, *type, *key;
	byte *buffer;
	word buffersize=0, datalen=0, keylen=0, alg=0;
	double nmb=0;

	if (ARG_COUNT(ht) != 3 
		|| getParameters(ht,3,&data,&type,&key)==FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string(data);
	convert_to_long(type);
	convert_to_string(key);

	alg=(int)type->value.lval;
	keylen=strlen(key->value.str.val);
	if (((alg==0||alg==2||alg==3)&& keylen!=16)||(alg==1 && keylen!=8)){
		php3_error(E_WARNING, "Keylength incorrect!\n");
		RETURN_FALSE;
	}

	datalen=strlen(data->value.str.val);
	if (datalen<crypt_block_size [alg])
	{
		php3_error(E_WARNING, "Data to short, must be at least %d bytes long!\n",crypt_block_size [alg]);
		RETURN_FALSE;
	}

	/*make the buffer size fit the block size for the algorithm*/
	buffersize=rup((double)datalen / crypt_block_size [alg]);
	buffersize=buffersize  * crypt_block_size [alg];
	buffer=emalloc(buffersize);
	memset(buffer,0,buffersize);
	memcpy(buffer,data->value.str.val,datalen);

	if (!crypt_encode(buffer,buffersize,alg,key->value.str.val))
	{
		php3_error(E_WARNING, "Crypt_encode failed!\n");
		efree(buffer);
		RETURN_FALSE;
	}
	
	return_value->value.str.val = buffer;
	return_value->value.str.len = buffersize;
	return_value->type = IS_STRING;
	return;
}
/* }}} */

/* {{{ proto string decrypt(string data, int type, string key)
   Decrypts string with key using algorithm type */
DLEXPORT void sflcrypt_decrypt(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *data, *type, *key;
	byte *buffer;
	word buffersize, datalen, alg, keylen;

	if (ARG_COUNT(ht) != 3 
		|| getParameters(ht,3,&data,&type,&key)==FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string(data);
	convert_to_long(type);
	convert_to_string(key);

	alg=(int)type->value.lval;
	keylen=strlen(key->value.str.val);
	if (((alg==0||alg==2||alg==3)&& keylen!=16)||(alg==1 && keylen!=8)){
		php3_error(E_WARNING, "Keylength incorrect!\n");
		RETURN_FALSE;
	}

	datalen=strlen(data->value.str.val);
	buffersize=rup((double)datalen / crypt_block_size [type->value.lval]);
	buffersize=buffersize  * crypt_block_size [type->value.lval];
	buffer=emalloc(buffersize);
	memset(buffer,0,buffersize);
	memcpy(buffer,data->value.str.val,datalen);
	if (!crypt_decode (buffer,buffersize,alg,key->value.str.val))
	{
		php3_error(E_WARNING, "Crypt_decode failed!\n");
		efree(buffer);
		RETURN_FALSE;
	}
	
	return_value->value.str.val = buffer;
	return_value->value.str.len = buffersize;
	return_value->type = IS_STRING;
	return;
}
/* }}} */

#ifdef HAVE_CRYPT_H
#if WIN32
extern int _getpid(void);
#endif

/* {{{ proto string crypt(string data [, string salt])
   A one-way crypt that can't be decrypted anymore */
void php3_crypt(INTERNAL_FUNCTION_PARAMETERS)
{
	char salt[4];
	int arg_count = ARG_COUNT(ht);
	pval *arg1, *arg2;
	static char seedchars[] =
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

	if (arg_count < 1 || arg_count > 2 ||
		getParameters(ht, arg_count, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(arg1);

	salt[0] = '\0';
	if (arg_count == 2) {
		convert_to_string(arg2);
		strncpy(salt, arg2->value.str.val, 2);
	}
	if (!salt[0]) {
		srand(time(0) * getpid());
		salt[0] = seedchars[rand() % 64];
		salt[1] = seedchars[rand() % 64];
	}
	salt[2] = '\0';

	return_value->value.str.val = (char *) crypt(arg1->value.str.val, salt);
	return_value->value.str.len = strlen(return_value->value.str.val);	/* can be optimized away to 13? */
	return_value->type = IS_STRING;
	pval_copy_constructor(return_value);
}
/* }}} */

#endif


function_entry sflcrypt_functions[] = {
	{"encrypt", sflcrypt_encrypt, NULL},
	{"decrypt", sflcrypt_decrypt, NULL},
#ifdef HAVE_CRYPT_H
	{"crypt",	php3_crypt,		NULL},
#endif
	{NULL,NULL,NULL}
};

php3_module_entry sflcrypt_module_entry = {
	"sflCrypt", sflcrypt_functions, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &sflcrypt_module_entry; }
#endif
