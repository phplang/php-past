--TEST--
Function call with global and static variables                ...
--POST--
--GET--
--FILE--
<?  $a = 10;
	Function Test (
		static $a=1;
		global $b;
		$c = 1;
		$b = 5;
		echo "$a $b ";
		$a++;
		$c++;
		echo "$a $c ";
	);
	Test();	
	echo "$a $b $c ";
	Test();	
	echo "$a $b $c ";
	Test()>
--EXPECT--
Content-type: text/html
1 5 2 2 10 5  2 5 3 2 10 5  3 5 4 2 
