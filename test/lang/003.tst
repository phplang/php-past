--TEST--
Simple Switch Test                                            ...
--POST--
--GET--
--FILE--
<?$a=1; 
  switch($a);
	case 0;
		echo "bad";	
		break;
	case 1;
		echo "good";
		break;
	default;
		echo "bad";
		break;
  endswitch>
--EXPECT--
Content-type: text/html
good
