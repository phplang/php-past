--TEST--
RegReplace single-quote test                                  ...
--POST--
--GET--
--FILE--
<?$a="\'test";
  echo reg_replace("\\'","'",$a)>
--EXPECT--
Content-type: text/html
'test
