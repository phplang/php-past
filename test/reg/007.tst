--TEST--
Test empty result buffer in reg_replace                       ...
--POST--
--GET--
--FILE--
<?
	$a="abcd";
	$b=reg_replace("abcd","",$a);
	echo strlen($b);
>
--EXPECT--
Content-type: text/html
0
