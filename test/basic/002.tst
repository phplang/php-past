--TEST--
Simple POST Method test                                       ...
--POST--
a=Hello+World
--GET--
--FILE--
<?echo $a>
--EXPECT--
Content-type: text/html

Hello World
