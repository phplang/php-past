--TEST--
Nested If/ElseIf/Else Test                                    ...
--POST--
--GET--
--FILE--
<?$a=1; $b=2;
  if($a==0);
	echo "bad";
  elseif($a==3);
	echo "bad";
  else;
	if($b==1);
		echo "bad";
	elseif($b==2);
		echo "good";
	else;
		echo "bad";
	endif;
  endif>	
--EXPECT--
Content-type: text/html
good
