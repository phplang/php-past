<?php
/* $Id: log-test.php3,v 1.1 1998/01/19 01:22:02 jim Exp $ */

function test_getlast() {
	$ret = new log_last_hit;
	$ret->set_from_string("/~jimw/test/ 112313 131223132 10 11 www.php.net - - -");
	return $ret;
}
$_log_getlastfunc = 'test_getlast';

function test_setlast($hit) {
	echo "<!-- setlast:" . $hit->as_string() . "-->";
}
$_log_setlastfunc = 'test_setlast';

function test_loghit($hit) {
	echo "<!-- loghit: " . $hit->as_string() . "-->";
}
$_log_loghitfunc = 'test_loghit';
?>
