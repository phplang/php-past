--TEST--
DBM Insert/Replace/Fetch Test                                 ...
--POST--
--GET--
--FILE--
<?
	dbmopen("./test.dbm","n");
	dbminsert("./test.dbm","key1","This is a test insert");
	dbmreplace("./test.dbm","key1","This is the replacement text");
	$a = dbmfetch("./test.dbm","key1");
	dbmclose("./test.dbm");
	echo $a
>
--EXPECT--
Content-type: text/html
This is the replacement text
