--TEST--
mSQL MAGIC_QUOTE Test (will fail if MAGIC_QUOTES is off)      ...
--POST--
a=%27test
--GET--
--FILE--
<?msql("test","delete from test");
  msql("test","insert into test values ('$a',0)");
  $result = msql("test","select * from test");
  echo msql_result($result,0,"name")>
--EXPECT--
Content-type: text/html
'test
