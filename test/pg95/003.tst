--TEST--
PG95 MAGIC_QUOTE Test (will fail if MAGIC_QUOTES is off)      ...
--POST--
a=%27test
--GET--
--FILE--
<?
	$conn = pg_connect("localhost", "5432", "", "", "template1");
	if (!$conn);
		echo "FAIL connect to template1";
		exit;
	endif;

	SetErrorReporting(0);
	$connb = pg_connect("localhost", "5432", "", "", "phptest");
	SetErrorReporting(1);

	if (!$connb);
		$res = pg_exec($conn, "create database phptest");
		if (!$res);
			echo "Could not create phptest database\n";
			exit;
		endif;

		$connb = pg_connect("localhost", "5432", "", "", "phptest");
		if (!$connb);
			echo "Could not connect to phptest database\n";
			exit;
		endif;
	endif;

	$res = pg_exec($connb, "create table test (name char(32), age int)");
	if (!$res);
		echo "FAIL create table\n";
		exit;
	endif;

	$res = pg_exec($connb, "insert into test values ('$a', 0)");
	if (!$res);
		echo "FAIL insert";
		exit;
	endif;

	$res = pg_exec($connb, "select * from test");
	if (!$res);
		echo "FAIL select";
		exit;
	endif;
	echo pg_result($res,0,"name");

	pg_freeResult($res);
	pg_close($connb);

	$res = pg_exec($conn, "drop database phptest");
	if (!$res);
		echo "Could not destroy database phptest";
	endif;

	pg_close($conn);
>
--EXPECT--
Content-type: text/html
'test
