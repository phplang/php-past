--TEST--
Passing to a function                                         ...
--POST--
--GET--
--FILE--
<?

function ary $b (

  $key= key ($b);
  while ("" != $key);
    echo "KEY: $key, VAL: $b[]\n";
    $key= key ($b);
  endwhile;
);


  $a[0]="a";
  $a[1]="b";
  $a[2]="c";

  ary ($a);

>
--EXPECT--
Content-type: text/html

KEY: 0, VAL: a
KEY: 1, VAL: b
KEY: 2, VAL: c

