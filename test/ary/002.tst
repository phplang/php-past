--TEST--
Auto Assign a[] 3 elements and print result                   ...
--POST--
--GET--
--FILE--
<?
  $a[]="a";
  $a[]="b";
  $a[]="c";

  echo "a[0]: $a[0]\n";
  echo "a[1]: $a[1]\n";
  echo "a[2]: $a[2]\n";
>
--EXPECT--
Content-type: text/html

a[0]: a
a[1]: b
a[2]: c

