<?php
/* $Id: log-db.php3,v 1.3 1998/01/19 03:08:03 jim Exp $ */

function _log_db_filename() {
	global $_log_logas;
	$ret = get_cfg_var("logging.directory");
	if ($ret) {
		$uid = getmyuid();
		$ret .= "/$uid/" . ereg_replace("/", "_", $_log_logas);
	}
	return $ret;
}

function _log_db_mkdir() {
	$dir = get_cfg_var("logging.directory");
	if ($dir) {
		@mkdir($dir, 0777);
		@mkdir($dir . "/" . getmyuid(), 0777);
	}
}

function _log_db_getlast() {
	$ret = new log_last_hit;

	/* we handle an error with opening the log file, so suppress messages */
	$file = _log_db_filename();
	if ($file) {
		$db = @dbmopen($file, "r");
	}

	if ($db) {
		$str = dbmfetch($db, "last");
		if ($str) {
			$ret->set_from_string($str);
		}
		dbmclose($db);
	}

	return $ret;
}
$_log_getlastfunc = '_log_db_getlast';

function _log_db_setlast($hit) {
	$file = _log_db_filename();
	_log_db_mkdir();
	if ($file) {
		$db = @dbmopen($file, "w");
		if (!$db) {
			$db = @dbmopen($file, "n");
		}
	}

	if ($db) {
		dbmreplace($db, "last", $hit->as_string());
		dbmclose($db);
	}
}
$_log_setlastfunc = '_log_db_setlast';

function _log_db_loghit($hit) {
	$file = _log_db_filename();
	_log_db_mkdir();

	if ($file) {
		$db = @dbmopen($file, "w");
		if (!$db) {
			$db = @dbmopen($file, "n");
		}
	}
	if ($db) {
		$retry = 0;
		while ($retry < 20) {
			if(dbminsert($db, $hit->time . $retry, $hit->as_string())) {
				$retry = 100;
			}
			$retry++;
		}
		dbmclose($db);
	}
}
$_log_loghitfunc = '_log_db_loghit';
?>
