--TEST--
DBM File Creation Test                                        ...
--POST--
--GET--
--FILE--
<?
	dbmopen("./test.dbm","n");
	dbmclose("./test.dbm");
>
--EXPECT--
Content-type: text/html
