--TEST--
Test ereg_replace of start-of-line                            ...
--POST--
--GET--
--FILE--
<?$a="This is a nice and simple string";
  echo ereg_replace("^This","That",$a);
>
--EXPECT--
Content-type: text/html
That is a nice and simple string
