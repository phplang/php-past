/* $Id: constants.h,v 1.12 1998/12/28 09:43:53 sas Exp $ */

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define CONST_CS 0x1				/* Case Sensitive */
#define CONST_PERSISTENT 0x2

typedef struct {
	pval value;
	int flags;
	char *name;
	uint name_len;
	int module_number;
} php3_constant;

#define REGISTER_LONG_CONSTANT(name,lval,flags)  php3_register_long_constant((name),sizeof(name),(lval),(flags),module_number)
#define REGISTER_DOUBLE_CONSTANT(name,dval,flags)  php3_register_double_constant((name),sizeof(name),(dval),(flags),module_number)
#define REGISTER_STRING_CONSTANT(name,str,flags)  php3_register_string_constant((name),sizeof(name),(str),(flags),module_number)
#define REGISTER_STRINGL_CONSTANT(name,str,len,flags)  php3_register_stringl_constant((name),sizeof(name),(str),(len),(flags),module_number)

#define REGISTER_MAIN_LONG_CONSTANT(name,lval,flags)  php3_register_long_constant((name),sizeof(name),(lval),(flags),0)
#define REGISTER_MAIN_DOUBLE_CONSTANT(name,dval,flags)  php3_register_double_constant((name),sizeof(name),(dval),(flags),0)
#define REGISTER_MAIN_STRING_CONSTANT(name,str,flags)  php3_register_string_constant((name),sizeof(name),(str),(flags),0)
#define REGISTER_MAIN_STRINGL_CONSTANT(name,str,len,flags)  php3_register_stringl_constant((name),sizeof(name),(str),(len),(flags),0)

extern void clean_module_constants(int module_number);
extern void free_php3_constant(php3_constant *c);
extern int php3_startup_constants(void);
extern int php3_shutdown_constants(void);
extern void clean_non_persistent_constants(void);
extern int php3_get_constant(char *name, uint name_len, pval *result);
extern PHPAPI void php3_register_long_constant(char *name, uint name_len, long lval, int flags, int module_number);
extern PHPAPI void php3_register_double_constant(char *name, uint name_len, double dval, int flags, int module_number);
extern PHPAPI void php3_register_string_constant(char *name, uint name_len, char *strval, int flags, int module_number);
extern PHPAPI void php3_register_stringl_constant(char *name, uint name_len, char *strval, uint strlen, int flags, int module_number);
extern int php3_get_constant(char *name, uint name_len, pval *result);

extern void php3_define(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_defined(INTERNAL_FUNCTION_PARAMETERS);

#endif

