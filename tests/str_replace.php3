<?

function str_test($src, $rep, $thr, $est) {
	static $count = 0;
	$count++;
	$new = str_replace($src, $rep, $thr);
	echo "$count ...";
	if($new == $est) {
		echo "successful";
	} else {
		echo "FAILED: $new\" instead of $est\"";
	}
	echo "\n";
	flush();
}

str_test("not ", "", "do not do", "do do");
str_test("test", "abc", "test me", "abc me");
str_test("aa", "b", "aaaaab", "bbab");
str_test("11", "ab", "11.11.11.11", "ab.ab.ab.ab");
str_test("дц", "!!!!!", "nixдцnow", "nix!!!!!now");
