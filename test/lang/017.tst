--TEST--
Testing user-defined function falling out of an If into another..
--POST--
--GET--
--FILE--
<?$a = 1;
Function Test $a (
	if($a<3);
		return(3);
	endif;
);

if($a < Test($a));
	echo "$a\n";
	$a++;
endif>
--EXPECT--
Content-type: text/html
1
