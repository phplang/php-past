--TEST--
Add 3 variables together and print result                     ...
--POST--
--GET--
--FILE--
<?$a=1; $b=2; $c=3; $d=$a+$b+$c; echo $d>
--EXPECT--
Content-type: text/html

6
