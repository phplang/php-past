--TEST--
Using another array                                           ...
--POST--
--GET--
--FILE--
<HR> $b= $a;
<?
  $a[0]="a";
  $a[1]="b";
  $a[2]="c";

  $b= $a;
  $key= key ($b);
  while ("" != $key);
    echo "KEY: $key, VAL: $b[]\n";
    $key= key ($b);
  endwhile;
>
<HR> $c= $a[0];
<?
  $c= $a[0];

  $key= key ($c);
  while ("" != $key);
    echo "KEY: $key, VAL: $c[]\n";
    $key= key ($c);
  endwhile;

  $cc= count($c);
  echo "count(): $cc\n";
>
<HR> $d[0]= $a[0];
<?
  $d[0]= $a[0];

  $key= key ($d);
  while ("" != $key);
    echo "KEY: $key, VAL: $d[]\n";
    $key= key ($d);
  endwhile;

  $cd= count($d);
  echo "count(): $cd\n";
>
<HR> $e[]= $a[0];
<?
  $e[]= $a[0];

  $key= key ($e);
  while ("" != $key);
    echo "KEY: $key, VAL: $e[]\n";
    $key= key ($e);
  endwhile;

  $ce= count($e);
  echo "count(): $ce\n";
>
<HR> append to $f[0] $f[1] $f[2], $f[]= $a;
<?
  $f[0]="e";
  $f[1]="f";
  $f[2]="g";

  $f[]= $a;
  $key= key ($f);
  while ("" != $key);
    echo "KEY: $key, VAL: $f[]\n";
    $key= key ($f);
  endwhile;

  $cf= count($f);
  echo "count(): $cf\n";
>
--EXPECT--
Content-type: text/html

<HR> $b= $a;
KEY: 0, VAL: a
KEY: 1, VAL: b
KEY: 2, VAL: c

<HR> $c= $a[0];
KEY: 0, VAL: a
count(): 1

<HR> $d[0]= $a[0];
KEY: 0, VAL: a
count(): 1

<HR> $e[]= $a[0];
KEY: 0, VAL: a
count(): 1

<HR> append to $f[0] $f[1] $f[2], $f[]= $a;
KEY: 0, VAL: e
KEY: 1, VAL: f
KEY: 2, VAL: g
KEY: 3, VAL: a
KEY: 4, VAL: b
KEY: 5, VAL: c
count(): 6

