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
 * $Id: phpvmail.h,v 1.3 1998/12/28 09:43:57 sas Exp $ 
 *
 * USE AT YOUR OWN RISK!
 */

#ifdef	__cplusplus
extern "C" {
#endif
int _vm_adduser(char *vdomain, char *uname, char *password, char *newpass1);
int _vm_deluser(char *vdomain, char *uname, char *password);
int _vm_delalias(char *vdomain, char *uname, char *password);
int _vm_addalias(char *vdomain, char *uname, char *password, char *destination);
int _vm_passwd(char *vdomain, char *uname, char *password, char *newpass1);
#ifdef	__cplusplus
}
#endif
