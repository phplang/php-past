--TEST--
mSQL insert/query test (See test/msql/README to set up test)  ...
--POST--
--GET--
--FILE--
<?msql("test","delete from test");
  msql("test","insert into test values ('Rasmus Lerdorf',27)");
  $result = msql("test","select * from test");
  $name = msql_result($result,0,"name");
  $age = msql_result($result,0,"age");
  echo "$name $age">
--EXPECT--
Content-type: text/html
Rasmus Lerdorf 27
