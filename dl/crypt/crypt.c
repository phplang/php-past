#include "../phpdl.h"
#include "prelude.h"
#include "sflcryp.h"

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


DLEXPORT void sflcrypt_encrypt(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *data, *type, *key;
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
	keylen=strlen(key->value.strval);
	if(((alg==0||alg==2||alg==3)&& keylen!=16)||(alg==4 && keylen!=8)){
		php3_error(E_WARNING, "Keylength incorrect!\n");
		RETURN_FALSE;
	}

	datalen=strlen(data->value.strval);
	if(datalen<crypt_block_size [alg])
	{
		php3_error(E_WARNING, "Data to short, must be at least %d bytes long!\n",crypt_block_size [alg]);
		RETURN_FALSE;
	}

	/*make the buffer size fit the block size for the algorithm*/
	buffersize=rup((double)datalen / crypt_block_size [alg]);
	buffersize=buffersize  * crypt_block_size [alg];
	buffer=emalloc(buffersize);
	memset(buffer,0,buffersize);
	memcpy(buffer,data->value.strval,datalen);

	if(!crypt_encode(buffer,buffersize,alg,key->value.strval))
	{
		php3_error(E_WARNING, "Crypt_encode failed!\n");
		efree(buffer);
		RETURN_FALSE;
	}
	
	return_value->value.strval = buffer;
	return_value->strlen = buffersize;
	return_value->type = IS_STRING;
	return;
}

DLEXPORT void sflcrypt_decrypt(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *data, *type, *key;
	byte *buffer;
	word buffersize, datalen;

	if (ARG_COUNT(ht) != 3 
		|| getParameters(ht,3,&data,&type,&key)==FAILURE)
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string(data);
	convert_to_long(type);
	convert_to_string(key);

	datalen=strlen(data->value.strval);
	buffersize=rup((double)datalen / crypt_block_size [type->value.lval]);
	buffersize=buffersize  * crypt_block_size [type->value.lval];
	buffer=emalloc(buffersize);
	memset(buffer,0,buffersize);
	memcpy(buffer,data->value.strval,datalen);
	if(!crypt_decode (buffer,buffersize,type->value.lval,key->value.strval))
	{
		php3_error(E_WARNING, "Crypt_decode failed!\n");
		efree(buffer);
		RETURN_FALSE;
	}
	
	return_value->value.strval = buffer;
	return_value->strlen = buffersize;
	return_value->type = IS_STRING;
	return;
}


function_entry sflcrypt_functions[] =
{
	{"encrypt", sflcrypt_encrypt},
	{"decrypt", sflcrypt_decrypt},
	{NULL,NULL}
};

php3_module_entry sflcrypt_module_entry = {
	"Calendar", sflcrypt_functions, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, NULL
};


#if COMPILE_DL
DLEXPORT php3_module_entry *get_module(void) { return &sflcrypt_module_entry; }
#endif
