--TEST--
Unset() index loop                                            ...
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
<HR>
<?
  unset ($a);
  $key= key ($a);
  while ("" != $key);
    echo "KEY: $key, VAL: $a[]\n";
    $key= key ($a);
  endwhile;

>
<HR>
<?
  $a[0]="d";
  $a[1]="e";
  $a[2]="f";

  $key= key ($a);
  while ("" != $key);
    echo "KEY: $key, VAL: $a[]\n";
    $key= key ($a);
  endwhile;
>
<HR>
<?
  unset ($a);
  $key= key ($a);
  while ("" != $key);
    echo "KEY: $key, VAL: $a[]\n";
    $key= key ($a);
  endwhile;
>
<HR>
<?
  $a[]="g";
  $a[]="h";
  $a[]="i";

  $key= key ($a);
  while ("" != $key);
    echo "KEY: $key, VAL: $a[]\n";
    $key= key ($a);
  endwhile;
>
<HR>
<?
  unset ($a);
  $key= key ($a);
  while ("" != $key);
    echo "KEY: $key, VAL: $a[]\n";
    $key= key ($a);
  endwhile;
>
<HR>
<?
  $a[]="j";
  $a[]="k";
  $a[]="l";

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

<HR>

<HR>
KEY: 0, VAL: d
KEY: 1, VAL: e
KEY: 2, VAL: f

<HR>

<HR>
KEY: 0, VAL: g
KEY: 1, VAL: h
KEY: 2, VAL: i

<HR>

<HR>
KEY: 0, VAL: j
KEY: 1, VAL: k
KEY: 2, VAL: l

