--TEST--
RegReplace test 1                                             ...
--POST--
--GET--
--FILE--
<?$a="abc123";
  echo reg_replace("123","def",$a)>
--EXPECT--
Content-type: text/html
abcdef
