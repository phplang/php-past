--TEST--
Assigning from a function                                     ...
--POST--
--GET--
--FILE--
<?

function rtn $rtn (
  return ($rtn);
);


  $a[0]="a";
  $a[1]="b";
  $a[2]="c";

  $b= rtn ($a);

  $key= key ($b);
  while ("" != $key);
    echo "KEY: $key, VAL: $b[]\n";
    $key= key ($b);
  endwhile;
>
<HR>
<?

  $c[0]= rtn ($a);

  $key= key ($c);
  while ("" != $key);
    echo "KEY: $key, VAL: $c[]\n";
    $key= key ($c);
  endwhile;
>
--EXPECT--
Content-type: text/html

KEY: 0, VAL: a
KEY: 1, VAL: b
KEY: 2, VAL: c

<HR>
KEY: 0, VAL: a
KEY: 1, VAL: b
KEY: 2, VAL: c
