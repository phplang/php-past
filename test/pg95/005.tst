--TEST--
PG95 Field info tests                                         ...
--POST--
--GET--
--FILE--
<?
	$conn = pg_connect("localhost", "5432", "", "/dev/null", "template1");
	if (!$conn);
		echo "FAIL connect to template1";
		exit;
	endif;

	SetErrorReporting(0);
	$connb = pg_connect("localhost", "5432", "", "/dev/console", "phptest");
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

	/**/
	$res = pg_exec($connb, "select * from test");
	if (!$res);
		echo "FAIL select1";
		exit;
	endif;
	echo pg_result($res,0,"name");
	echo "|%d" pg_result($res,0,"age");
	echo "|%.2f" pg_result($res,0,"l");
	echo "\n";

	/* field info tests */
	echo pg_fieldName($res, 0);
	echo "\n";
	echo pg_fieldName($res, 1);
	echo "\n";
	echo pg_fieldName($res, 2);
	echo "\n";
	echo pg_fieldName($res, 3);
	echo "\n";

	echo pg_fieldNum($res, "name");
	echo "\n";
	echo pg_fieldNum($res, "age");
	echo "\n";
	echo pg_fieldNum($res, "l");
	echo "\n";
	echo pg_fieldNum($res, "foo");
	echo "\n";

	/* pg_fieldType, pg_fieldLen */
	echo pg_fieldType($res, 0);
	echo "\n";
	echo pg_fieldType($res, 1);
	echo "\n";
	echo pg_fieldType($res, 2);
	echo "\n";
	echo pg_fieldType($res, 3);
	echo "\n";

	echo pg_fieldPrtLen($res, 0, "name");
	echo "\n";
	echo pg_fieldPrtLen($res, 0, "age");
	echo "\n";
	echo pg_fieldPrtLen($res, 0, "l");
	echo "\n";
	echo pg_fieldPrtLen($res, 0, "foo");
	echo "\n";

	echo pg_fieldSize($res, "name");
	echo "\n";
	echo pg_fieldSize($res, "age");
	echo "\n";
	echo pg_fieldSize($res, "l");
	echo "\n";
	echo pg_fieldSize($res, "foo");
	echo "\n";

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
fofo|0|1.20
name
age
l
<b><i>phpfi.in:</i> Field index larger than number of fields on line 55</b><br>
<tt>	echo pg_fieldName($res, 3<b><blink>);
</blink></b></tt><br>
0
1
2
-1
varchar
int4
float8
<b><i>phpfi.in:</i> Field index is larger than the number of fields on line 74</b><br>
<tt>	echo pg_fieldType($res, 3<b><blink>);
</blink></b></tt><br>
4
1
3
<b><i>phpfi.in:</i> Field foo not found on line 83</b><br>
<tt>	echo pg_fieldPrtLen($res, 0, &quot;foo&quot;<b><blink>);
</blink></b></tt><br>-1
0
4
8
<b><i>phpfi.in:</i> Field foo not found on line 92</b><br>
<tt>	echo pg_fieldSize($res, &quot;foo&quot;<b><blink>);
</blink></b></tt><br>-1
