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

#include "server.h" /* from the vmailmgr daemon directory */
#include "phpvmail.h"

int _vm_adduser(char *vdomain, char *uname, char *password, char *newpass1){
	return server_call("adduser", vdomain, uname, password, newpass1).call().code;
}

int _vm_deluser(char *vdomain, char *uname, char *password){
	return server_call("deluser", vdomain, uname, password).call().code;
}

int _vm_delalias(char *vdomain, char *uname, char *password){
	return server_call("delalias", vdomain, uname, password).call().code;
}

int _vm_addalias(char *vdomain, char *uname, char *password, char *destination){
	return server_call("addalias", vdomain, uname, password, destination).call().code;
}

int _vm_passwd(char *vdomain, char *uname, char *password, char *newpass1){
	return server_call("chpass", vdomain, uname, password, newpass1).call().code;
}
