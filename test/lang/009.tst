--TEST--
Testing function parameter passing                            ...
--POST--
--GET--
--FILE--
<?  Function Test $a,$b (
		echo $a+$b;	
	);
	Test(1,2)>
--EXPECT--
Content-type: text/html
3
