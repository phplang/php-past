--TEST--
Strlen() function test                                        ...
--POST--
--GET--
--FILE--
<?echo strlen("abcdef")>
--EXPECT--
Content-type: text/html
6
