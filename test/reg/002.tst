--TEST--
RegReplace test 2                                             ...
--POST--
--GET--
--FILE--
<?$a="abc123";
  echo reg_replace("123","",$a)>
--EXPECT--
Content-type: text/html
abc
