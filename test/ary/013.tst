--TEST--
Increment Array Elements                                      ...
--POST--
--GET--
--FILE--
<?
  $i=0;
  while($i < 5) {
    $a[$i] = 0;
    $j = 0;
    while($j < 5) {
      $a[$i]++;
      $j++;
    }
    $i++;
  }
  $i=0;
  while($i < 5) {
   echo $a[$i];
   echo "\n";
   $i++;
  }
>
--EXPECT--
Content-type: text/html

5
5
5
5
5
