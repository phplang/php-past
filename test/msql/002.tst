--TEST--
mSQL Multiple insert/query test                               ...
--POST--
--GET--
--FILE--
<?msql("test","delete from test");
  msql("test","insert into test values ('Rasmus Lerdorf',27)");
  msql("test","insert into test values ('Joe User',99)");
  $result = msql("test","select * from test order by age");
  $name = msql_result($result,0,"name");
  $age = msql_result($result,0,"age");
  echo "$name $age\n";
  $name = msql_result($result,1,"name");
  $age = msql_result($result,1,"age");
  echo "$name $age">
--EXPECT--
Content-type: text/html
Rasmus Lerdorf 27
Joe User 99
