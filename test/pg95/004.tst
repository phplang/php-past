--TEST--
PG95 Inserts, selections and results, oh my                   ...
--POST--
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

	SetErrorReporting(0);
	$res = pg_exec($connb, "create table test (name varchar, age int, l float)");
	SetErrorReporting(1);

	/* actual test is here */
	$res = pg_exec($connb, "insert into test values ('fofo', 0, 1.2)");
	$oid_first = pg_getlastoid();
	if ($oid_first < 0);
		echo "FAIL insert/getlastoid\n";
	endif;

	$res = pg_exec($connb, "insert into test values ('gogo', 1, 2.3)");
	$oid_second = pg_getlastoid();
	if ($oid_second < 0);
		echo "FAIL insert/getlastoid 2\n";
	endif;

	/**/
	$res = pg_exec($connb, "select * from test where oid=$oid_first");
	if (!$res);
		echo "FAIL select1";
		exit;
	endif;
	echo pg_result($res,0, "name");
	echo "|%d" pg_result($res,0,"age");
	echo "|%.2f" pg_result($res,0,"l");
	echo "\n";

	/* test integer field indexing */
	echo pg_result($res,0, "name");
	echo "|%d" pg_result($res,0,"age");
	echo "|%.2f" pg_result($res,0,"l");
	echo "\n";
	pg_freeResult($res);

	/**/
	$res = pg_exec($connb, "select * from test where oid=$oid_second");
	if (!$res);
		echo "FAIL select2";
	endif;
	echo pg_result($res,0,"name");
	echo "|%d" pg_result($res,0,"age");
	echo "|%.2f" pg_result($res,0,"l");
	echo "\n";

	/**/
	$res2 = pg_exec($connb, "select name, age from test");
	if (!$res2);
		echo "FAIL select3";
	endif;

	echo pg_numRows($res);
	echo "\n";
	echo pg_numRows($res2);
	echo "\n";
	echo pg_numFields($res);
	echo "\n";
	echo pg_numFields($res2);
	echo "\n";

	pg_freeResult($res);
	pg_freeResult($res2);
	pg_close($connb);

	$res = pg_exec($conn, "drop database phptest");
	if (!$res);
		echo "Could not destroy database phptest";
	endif;

	pg_close($conn);
>
--EXPECT--
Content-type: text/html
fofo|0|1.20
fofo|0|1.20
gogo|1|2.30
1
2
3
2
