--TEST--
Testing user-defined function in included file                ...
--POST--
--GET--
--FILE--
<?
	include "lang/016.inc";
	MyFunc("Hello");
>
--EXPECT--
Content-type: text/html
Hello
