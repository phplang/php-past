--TEST--
Subtract 3 variables and print result                         ...
--POST--
--GET--
--FILE--
<?$a=27; $b=7; $c=10; $d=$a-$b-$c; echo $d>
--EXPECT--
Content-type: text/html

10
