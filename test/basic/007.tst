--TEST--
Multiply 3 variables and print result                         ...
--POST--
--GET--
--FILE--
<?$a=2; $b=4; $c=8; $d=$a*$b*$c; echo $d>
--EXPECT--
Content-type: text/html

64
