--TEST--
Auto index loop                                               ...
--POST--
--GET--
--FILE--
<?
  $a[0]="a";
  $a[1]="b";
  $a[2]="c";

  $key= key ($a);
  while ("" != $key);
    echo "KEY: $key, VAL: $a[]\n";
    $key= key ($a);
  endwhile;
>
--EXPECT--
Content-type: text/html

KEY: 0, VAL: a
KEY: 1, VAL: b
KEY: 2, VAL: c

