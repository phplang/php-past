--TEST--
Three variables in POST data                                  ...
--POST--
a=Hello+World\&b=Hello+Again+World\&c=1
--GET--
--FILE--
<?echo "$a $b $c">
--EXPECT--
Content-type: text/html

Hello World Hello Again World 1
