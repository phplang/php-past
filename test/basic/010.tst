--TEST--
Testing | and & operators                                     ...
--POST--
--GET--
--FILE--
<?$a=8; $b=4; $c=8; echo $a|$b&$c>
--EXPECT--
Content-type: text/html

8
