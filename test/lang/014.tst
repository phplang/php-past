--TEST--
Testing eval function inside user-defined function            ...
--POST--
--GET--
--FILE--
<?
Function F $a ( 
	eval($a);
);

F("echo \"Hello\";");
>
--EXPECT--
Content-type: text/html
Hello
