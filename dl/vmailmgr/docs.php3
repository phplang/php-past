<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">

<HTML>
<HEAD>
	<TITLE>Vmailmgr functions</TITLE>
</HEAD>

<BODY>

<?php
dl("php3_vmailmgr.so"); //load the shared library
$vdomain="vdomain.com";
$basepwd="password";

//This documentation by Shane Caraveo (shane@caraveo.com)
//USE AT YOUR OWN RISK!

//These functions require QMAIL (www.qmail.org) and the
//vmailmgr package by Bruce Guenter http://www.qcc.sk.ca/~bguenter/distrib/vmailmgr/

//For all functions, the following two variables are defined as:
//    string vdomain  the domain name of your virtual domain (vdomain.com)
//    string basepwd  the password of the 'real' user that holds the virtual users
//
//Only up to 8 characters are recognized in passwords for virtual users

// return status for all functions //matches response in response.h
// 0 ok
// 1 bad
// 2 error
// 3 error connecting

//known problems
//vm_deluser does not delete the user directory as it should
//vm_addalias currently does not work correctly

// int vm_adduser(vdomain,basepwd,newusername,newuserpassword)
//    string newusername   email login name
//    string newuserpassword  the password for this vuser
//		adds a new virtual user with a password
ret=vm_adduser($vdomain,$basepwd,"newtest","test");
echo ret;

// int vm_addalias(vdomain,basepwd,username,alias)
//    string username  the email login name
//    string alias     the new alias for this vuser
//		add an alias to a virtual user
ret=vm_addalias($vdomain,$basepwd,"newtest","phptest");
echo ret;

// int vm_passwd(vdomain,username,pword,newpword)
//    string username  the email login name
//    string pword		the old password
//    string newpword   the new password
//		changes a virtual users password
ret=vm_passwd($vdomain,"newtest","test","newtest");
echo ret;

// int vm_delalias(vdomain,basepwd,alias)
//    string alias	the alias to be removed
//		removes an alias
ret=vm_delalias($vdomain,$basepwd,"phptest");
echo ret;

// int vm_deluser(vdomain,basepwd,username)
//    string username	the virtual user to be removed
//		removes a virtual user
ret=vm_deluser($vdomain,$basepwd,"newtest");
echo ret;
?>

</BODY>
</HTML>
