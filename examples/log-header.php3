<?php
/* $Id: log-header.php3,v 1.4 1998/06/14 02:40:39 jim Exp $ */

/* These are global variables. You should use the setshowinfo(),
   setlogging(), and setlogsuccessive() functions to change these
   values. */

$_log_showinfo = 1;	# show log info at the bottom of the page
$_log_dologging = 1;	# log the page hit
$_log_logsuccessive = 1;# log successive hits from the same host

/* This is the name to log the page as. You can change this using the
   LogAs() function. */
$_log_logas = ereg_replace("^/~[^/]+/", "", $GLOBALS['SCRIPT_NAME']);

/* These are essentially function pointers that should get overridden
   by the logging module that gets loaded. */

function log_undefined() {
	echo "<P><B>Fatal error:</B> No logging module loaded. Make sure that logging.method is set in your php3.ini file.\n";
}

$_log_getlastfunc = 'log_undefined';	# function to get last info
$_log_setlastfunc = 'log_undefined';	# function to set last info
$_log_loghitfunc = 'log_undefined';	# function to log an individual hit

$_log_last_hit = 0;			# will be the object for the last hit

/* A "log_hit" object is saved on every page load. */
class log_hit {
	var $filename, $time, $host, $email, $referrer, $browser;

	cfunction as_string() {
		return  $this->host . ' ' . $this->email .  ' ' .
			$this->referrer . ' ' . $this->browser;
	}

	cfunction as_update_query() {
	}
};

/* A "log_last_hit" is an entry saved to the log table that stores
   information about the last hit received. */
class log_last_hit {
	var $filename, $started, $time, $total, $today, $host,
	    $email, $referrer, $browser;

	cfunction set_from_string($string) {
		$temp = explode(' ', $string);
		if (count($temp) != 9) {
			echo "<!-- error setting log info from '$string' -->";
		}
		else {
			$this->filename = $temp[0];
			$this->time = $temp[1];
			$this->started = $temp[2];
			$this->total = $temp[3];
			$this->today = $temp[4];
			$this->host = ereg_replace("^-\$", "", $temp[5]);
			$this->email = ereg_replace("^-\$", "", $temp[6]);
			$this->referrer = ereg_replace("^-\$", "", $temp[7]);
			/* need to use urldecode() because of note below */
			$this->browser = urldecode(ereg_replace("^-\$", "", $temp[8]));
		}
	}

	cfunction as_string() {
		return $this->filename . ' ' . $this->time . ' ' .
			$this->started . ' ' .
			$this->total .  ' ' . $this->today . ' ' .
			$this->host . ' ' . $this->email .  ' ' .
			$this->referrer . ' ' . $this->browser;
	}

	cfunction as_update_query() {
	}

	cfunction update_counts() {
		$this->total += 1;
		$last_date = Date("Ymd", $this->time);
		if ($last_date != Date("Ymd")) {
			$this->today = 1;
		}
		else {
			$this->today += 1;
		}
	}

	cfunction update() {
		$this->filename = $GLOBALS['SCRIPT_NAME'];
		$this->update_counts();
		$this->time = time();
		if ($GLOBALS['REMOTE_HOST']) {
			$this->host = $GLOBALS['REMOTE_HOST'];
		}
		else {
			$this->host = "-";
		}
		if ($GLOBALS['HTTP_EMAIL']) {
			$this->email = $GLOBALS['HTTP_EMAIL'];
		}
		else {
			$this->email = "-";
		}

		/* earlier versions of Apache spelled this one wrong */
		if ($GLOBALS['HTTP_REFERRER'] || $GLOBALS['HTTP_REFERER']) {
			$this->referrer = $GLOBALS['HTTP_REFERRER'] || $GLOBALS['$HTTP_REFERER'];
		}
		else {
			$this->referrer = "-";
		}
		if ($GLOBALS['HTTP_USER_AGENT']) {
			/* we use urlencode() because the agent may have a
			   space in it, which would screw up set_from_string()
			*/
			$this->browser = urlencode($GLOBALS['HTTP_USER_AGENT']);
		}
		else {
			$this->browser = "-";
		}

		if (!$this->started) {
			$this->started = time();
		}
	}
};

/* figure out what sort of logging to do */
if ($_log_method = get_cfg_var("logging.method")) {
	include ("log-$_log_method.php3");
}

function setshowinfo($value) {
	$GLOBALS['_log_showinfo'] = $value;
	return $value;
}

function setlogging($value) {
	$GLOBALS['_log_dologging'] = $value;
	return $value;
}

function setlogsuccessive($value) {
	$GLOBALS['_log_successive'] = $value;
	return $value;
}

function get_current_hit() {
	$hit = new log_hit;
	$hit->filename = $GLOBALS['SCRIPT_NAME'];
	$hit->time = time();
	$hit->host = $GLOBALS['HTTP_REQUEST_HOST'];
	$hit->email = $GLOBALS['HTTP_EMAIL'];
	$hit->referrer = $GLOBALS['HTTP_REFERRER'] || $GLOBALS['$HTTP_REFERER'];
	$hit->browser = $GLOBALS['HTTP_USER_AGENT'];
	return $hit;
}

function get_last_hit() {
	global $_log_getlastfunc;
	if (gettype($GLOBALS['_log_last_hit']) != 'object') {
		$GLOBALS['_log_last_hit'] = $_log_getlastfunc();
	}
	return $GLOBALS['_log_last_hit'];
}

function getlastaccess() {
	$last = get_last_hit();
	return $last->time;
}

function getlastbrowser() {
	$last = get_last_hit();
	return $last->browser;
}

function getlastemail() {
	$last = get_last_hit();
	return $last->email;
}

function getlasthost() {
	$last = get_last_hit();
	return $last->host;
}

function getlastref() {
	$last = get_last_hit();
	return $last->referrer;
}

function getstartlogging() {
	$last = get_last_hit();
	return $last->started;
}

function gettoday() {
	$last = get_last_hit();
	return $last->today;
}

function gettotal() {
	$last = get_last_hit();
	return $last->total;
}

function logas($filename) {
	$GLOBALS['_log_logas'] = $filename;
}

?>
