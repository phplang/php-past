--TEST--
Testing nested functions                                      ...
--POST--
--GET--
--FILE--
<?
Function F ( 
	$a = "Hello ";
	return($a);
);

Function G (
  static $myvar = 4;
  
  echo "$myvar ";
  echo F();
  echo "$myvar";
);

G();
>
--EXPECT--
Content-type: text/html
4 Hello 4
