--TEST--
RegMatch Test                                                 ...
--POST--
--GET--
--FILE--
<?$a="This is a nice and simple string";
  echo reg_match(".*nice and simple.*",$a)>
--EXPECT--
Content-type: text/html
32
