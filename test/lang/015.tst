--TEST--
Testing include                                               ...
--POST--
--GET--
--FILE--
<?
	include "lang/015.inc";
>
--EXPECT--
Content-type: text/html
Hello
