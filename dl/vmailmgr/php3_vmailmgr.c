/* interface functions for controlling qmail vusers
 * requires the vmailmgr package 
 * http://www.qcc.sk.ca/~bguenter/distrib/vmailmgr/ 
 * and of course, qmail, www.qmail.org
 *
 * PHP 3 can be found at www.php.net
 *
 * code by Shane Caraveo shane@caraveo.com
 * copy freely!
 *
 * USE AT YOUR OWN RISK!
 */

#include "dl/phpdl.h"
#include "phpvmail.h"
#define PWORD_MISSMATCH "The passwords you entered do not match"

void vm_adduser(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *vdomain,*username,*password,*newpass;
	int msg;

	if (getParameters(ht, 4, &vdomain,&password,&username,&newpass) == SUCCESS) {
		convert_to_string(vdomain);
		convert_to_string(password);
		convert_to_string(username);
		convert_to_string(newpass);
	
		_php3_strtolower(username->value.str.val);

		msg=_vm_adduser(vdomain->value.str.val, username->value.str.val, password->value.str.val, newpass->value.str.val);
		RETURN_LONG(msg)
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


void vm_deluser(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *vdomain,*username,*password;
	int msg;

	if (getParameters(ht, 3, &vdomain,&password,&username) == SUCCESS) {
		convert_to_string(vdomain);
		convert_to_string(password);
		convert_to_string(username);

		_php3_strtolower(username->value.str.val);

		msg=_vm_deluser(vdomain->value.str.val, username->value.str.val, password->value.str.val);
		RETURN_LONG(msg)
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


void vm_passwd(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *vdomain,*username,*password,*newpass;
	int msg;

	if (getParameters(ht, 4, &vdomain,&username,&password,&newpass) == SUCCESS) {
		convert_to_string(vdomain);
		convert_to_string(password);
		convert_to_string(username);
		convert_to_string(newpass);

		_php3_strtolower(username->value.str.val);
		
		msg=_vm_passwd(vdomain->value.str.val, username->value.str.val, password->value.str.val, newpass->value.str.val);
		RETURN_LONG(msg)
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


void vm_addalias(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *vdomain,*username,*password,*destination;
	int msg;

	if (getParameters(ht, 4, &vdomain,&password,&username,&destination) == SUCCESS) {
		convert_to_string(vdomain);
		convert_to_string(password);
		convert_to_string(username);
		convert_to_string(destination);

		_php3_strtolower(username->value.str.val);

		msg=_vm_addalias(vdomain->value.str.val, username->value.str.val, password->value.str.val, destination->value.str.val);
		RETURN_LONG(msg)
	} else {
		WRONG_PARAM_COUNT;
	}
}


void vm_delalias(INTERNAL_FUNCTION_PARAMETERS)
{
	pval *vdomain,*username,*password;
	int msg;

	if (getParameters(ht, 3, &vdomain,&password,&username) == SUCCESS) {
		convert_to_string(vdomain);
		convert_to_string(password);
		convert_to_string(username);

		_php3_strtolower(username->value.str.val);

		msg=_vm_delalias(vdomain->value.str.val, username->value.str.val, password->value.str.val);
		RETURN_LONG(msg)
	} else {
		WRONG_PARAM_COUNT;
	}
	return;
}


function_entry vmailmgr_functions[] = {
	{"vm_adduser", vm_adduser, NULL},
	{"vm_deluser", vm_deluser, NULL},
	{"vm_passwd", vm_passwd, NULL},
	{"vm_addalias", vm_addalias, NULL},
	{"vm_delalias", vm_delalias, NULL},
	{NULL, NULL, NULL}
};


php3_module_entry vmailmgr_module_entry = {
	"QmailVMailMgr", vmailmgr_functions, NULL, NULL, NULL, NULL, NULL, 0, 0, 0, NULL
};

php3_module_entry *get_module(void) { return &vmailmgr_module_entry; }
