<?php
/* $Id: log-footer.php3,v 1.3 1998/01/19 03:08:03 jim Exp $ */

if ($_log_showinfo) {
	$last = get_last_hit();
	echo "<HR size=3 width=400 align=\"center\">\n";

	echo "<CENTER><I>";
	$last->update_counts();

	if ($last->total != 1) {
		$start_date = ereg_replace("0([0-9])", "\\1", Date("l F dS", $last->started));
		printf("%d total hit%s since %s. %d hit%s today.<BR>\n",
			$last->total, ($last->total == 1 ? "" : "s"),
			$start_date,
			$last->today, ($last->today == 1 ? "" : "s"));

		$last_date = ereg_replace("0([0-9])", "\\1", Date("l F dS", $last->time));
		$last_time = Date("H:i:s", $last->time);
		printf("Last access on %s at %s %s %s.<BR>\n",
			$last_date, $last_time,
			($last->email != "" ? "by" : "from"),
			($last->email != "" ? $last->email : $last->host));
	}
	else {
		echo "No previous access to this page.<BR>";
	}

	/* We can always show last modified information, since it has
	   nothing to do with logging, really. */
	$mod = getlastmod();
	$mod_date = ereg_replace("0([0-9])", "\\1", Date("l F d, Y", $mod));
	$mod_time = Date("H:i:s", $mod);
	printf("Page was last updated on %s at %s.",
		$mod_date, $mod_time);

	echo "</I></CENTER>\n";
}

if ($_log_dologging) {
	$last = get_last_hit();
	if ($last) {
		if ($_log_logsuccessive || ($last->host != $GLOBALS['REMOTE_HOST'])) {
			$last->update();
			$_log_setlastfunc($last);
		}
	}

	$hit = get_current_hit();
	if ($hit) {
		$_log_loghitfunc($hit);
	}
}
?>
