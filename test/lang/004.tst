--TEST--
Simple If/Else Test                                           ...
--POST--
--GET--
--FILE--
<?$a=1; 
  if($a==0);
	echo "bad";
  else;
	echo "good";
  endif>	
--EXPECT--
Content-type: text/html
good
