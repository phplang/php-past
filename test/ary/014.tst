--TEST--
Unset a single element array                                  ...
--POST--
--GET--
--FILE--
<?
	$a[0]="abc";
	unset($a);
	$a[]="def";
	$a[]="ghi";
	$i=0;
	$c=count($a);
	reset($a);
	while($i<$c) {
		$key=key($a);
		echo "\$a[$key] -> $a[$key]\n";
		next($a);
		$i++;
	}	
>
--EXPECT--
Content-type: text/html

$a[0] -> def
$a[1] -> ghi
