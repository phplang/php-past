--TEST--
GET and POST Method combined                                  ...
--POST--
a=Hello+World
--GET--
b=Hello+Again+World
--FILE--
<?echo "$a $b">
--EXPECT--
Content-type: text/html

Hello World Hello Again World
