--TEST--
Simple If condition test                                      ...
--POST--
--GET--
--FILE--
<?$a=1; if($a>0); echo "Yes"; endif>
--EXPECT--
Content-type: text/html
Yes
