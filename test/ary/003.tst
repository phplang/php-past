--TEST--
Count() items                                                 ...
--POST--
--GET--
--FILE--
<?
  $a[]="a";
  $a[]="b";
  $a[]="c";

  $b= count ($a);
  echo "There are 3 items, count() shows: $b\n";
>
--EXPECT--
Content-type: text/html

There are 3 items, count() shows: 3

