--TEST--
Divide 3 variables and print result                           ...
--POST--
--GET--
--FILE--
<?$a=27; $b=3; $c=3; $d=$a/$b/$c; echo $d>
--EXPECT--
Content-type: text/html

3
