--TEST--
Trivial "Hello World" test                                    ...
--POST--
--GET--
--FILE--
<?echo "Hello World">
--EXPECT--
Content-type: text/html

Hello World
