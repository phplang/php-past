--TEST--
Assign Indexed, then and Auto assign                          ...
--POST--
--GET--
--FILE--
<HR> Assign Index from 0, then Auto assign
<?
  $z[0]="a";
  $z[1]="b";
  $z[2]="c";

  $z[]="d";
  $z[]="e";
  $z[]="f";

  $key= key ($z);
  while ("" != $key);
    echo "KEY: $key, VAL: $z[]\n";
    $key= key ($z);
  endwhile;

  $cz= count($z);
  echo "count(): $cz\n";
>
<HR> Assign Index from 1, then Auto assign
<?
  $a[1]="a";
  $a[2]="b";
  $a[3]="c";

  $a[]="d";
  $a[]="e";
  $a[]="f";

  $key= key ($a);
  while ("" != $key);
    echo "KEY: $key, VAL: $a[]\n";
    $key= key ($a);
  endwhile;

  $ca= count($a);
  echo "count(): $ca\n";
>
<HR> Assign Index from 2, then Auto assign
<?
  $b[2]="a";
  $b[3]="b";
  $b[4]="c";

  $b[]="d";
  $b[]="e";
  $b[]="f";

  $key= key ($b);
  while ("" != $key);
    echo "KEY: $key, VAL: $b[]\n";
    $key= key ($b);
  endwhile;

  $cb= count($b);
  echo "count(): $cb\n";
>
<HR> Assign Index from 3, then Auto assign
<?
  $c[3]="a";
  $c[4]="b";
  $c[5]="c";

  $c[]="d";
  $c[]="e";
  $c[]="f";

  $key= key ($c);
  while ("" != $key);
    echo "KEY: $key, VAL: $c[]\n";
    $key= key ($c);
  endwhile;

  $cc= count($c);
  echo "count(): $cc\n";
>
<HR> Assign Index from 4, then Auto assign
<?
  $d[4]="a";
  $d[5]="b";
  $d[6]="c";

  $d[]="d";
  $d[]="e";
  $d[]="f";

  $key= key ($d);
  while ("" != $key);
    echo "KEY: $key, VAL: $d[]\n";
    $key= key ($d);
  endwhile;

  $cd= count($d);
  echo "count(): $cd\n";
>
<HR> Assign Index from 5, then Auto assign
<?
  $e[5]="a";
  $e[6]="b";
  $e[7]="c";

  $e[]="d";
  $e[]="e";
  $e[]="f";

  $key= key ($e);
  while ("" != $key);
    echo "KEY: $key, VAL: $e[]\n";
    $key= key ($e);
  endwhile;

  $ce= count($e);
  echo "count(): $ce\n";
>
<HR> Assign Index from 6, then Auto assign
<?
  $f[6]="a";
  $f[7]="b";
  $f[8]="c";

  $f[]="d";
  $f[]="e";
  $f[]="f";

  $key= key ($f);
  while ("" != $key);
    echo "KEY: $key, VAL: $f[]\n";
    $key= key ($f);
  endwhile;

  $cf= count($f);
  echo "count(): $cf\n";
>
<HR> Assign Index from 7, then Auto assign
<?
  $g[7]="a";
  $g[8]="b";
  $g[9]="c";

  $g[]="d";
  $g[]="e";
  $g[]="f";

  $key= key ($g);
  while ("" != $key);
    echo "KEY: $key, VAL: $g[]\n";
    $key= key ($g);
  endwhile;

  $cg= count($g);
  echo "count(): $cg\n";
>
--EXPECT--
Content-type: text/html

<HR> Assign Index from 0, then Auto assign
KEY: 0, VAL: a
KEY: 1, VAL: b
KEY: 2, VAL: c
KEY: 3, VAL: d
KEY: 4, VAL: e
KEY: 5, VAL: f
count(): 6

<HR> Assign Index from 1, then Auto assign
KEY: 1, VAL: a
KEY: 2, VAL: b
KEY: 3, VAL: c
KEY: 4, VAL: d
KEY: 5, VAL: e
KEY: 6, VAL: f
count(): 6

<HR> Assign Index from 2, then Auto assign
KEY: 2, VAL: a
KEY: 3, VAL: b
KEY: 4, VAL: c
KEY: 5, VAL: d
KEY: 6, VAL: e
KEY: 7, VAL: f
count(): 6

<HR> Assign Index from 3, then Auto assign
KEY: 3, VAL: a
KEY: 4, VAL: b
KEY: 5, VAL: c
KEY: 6, VAL: d
KEY: 7, VAL: e
KEY: 8, VAL: f
count(): 6

<HR> Assign Index from 4, then Auto assign
KEY: 4, VAL: a
KEY: 5, VAL: b
KEY: 6, VAL: c
KEY: 7, VAL: d
KEY: 8, VAL: e
KEY: 9, VAL: f
count(): 6

<HR> Assign Index from 5, then Auto assign
KEY: 5, VAL: a
KEY: 6, VAL: b
KEY: 7, VAL: c
KEY: 8, VAL: d
KEY: 9, VAL: e
KEY: 10, VAL: f
count(): 6

<HR> Assign Index from 6, then Auto assign
KEY: 6, VAL: a
KEY: 7, VAL: b
KEY: 8, VAL: c
KEY: 9, VAL: d
KEY: 10, VAL: e
KEY: 11, VAL: f
count(): 6

<HR> Assign Index from 7, then Auto assign
KEY: 7, VAL: a
KEY: 8, VAL: b
KEY: 9, VAL: c
KEY: 10, VAL: d
KEY: 11, VAL: e
KEY: 12, VAL: f
count(): 6
