--TEST--
Simple While Loop Test                                        ...
--POST--
--GET--
--FILE--
<?$a=1; 
  while($a<10);
	echo $a;
	$a++;
  endwhile>
--EXPECT--
Content-type: text/html
123456789
