--TEST--
Unset combined with non-indexed array test                    ...
--POST--
--GET--
--FILE--
<?
echo count($a);
$a[]="one";
$a[]="two";
echo count($a);
echo $a[0];
echo $a[1];
unset($a);
echo count($a);
>
--EXPECT--
Content-type: text/html

02onetwo0
