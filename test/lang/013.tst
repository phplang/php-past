--TEST--
Testing eval function                                         ...
--POST--
--GET--
--FILE--
<?
	$a="echo \"Hello\";";
	eval($a);
>
--EXPECT--
Content-type: text/html
Hello
