--TEST--
Two variables in POST data                                    ...
--POST--
a=Hello+World\&b=Hello+Again+World
--GET--
--FILE--
<?echo "$a $b">
--EXPECT--
Content-type: text/html

Hello World Hello Again World
