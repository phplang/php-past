--TEST--
Test Regular expression register support in RegSearch         ...
--POST--
--GET--
--FILE--
<?$a="This is a nice and simple string";
  echo reg_search("\(is\)",$a,$registers);
  echo "\n";
  echo $registers[0];
  echo "\n";
  echo $registers[1];
  echo "\n";
  echo $registers[2];
  echo "\n";
>
--EXPECT--
Content-type: text/html
is is a nice and simple string
is
is
