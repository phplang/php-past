--TEST--
Testing recursive function                                    ...
--POST--
--GET--
--FILE--
<?  Function Test (
		static $a=1;

		echo "$a ";	
		$a++;
		if($a<10); Test(); endif;
	);
	Test()>
--EXPECT--
Content-type: text/html
1 2 3 4 5 6 7 8 9 
