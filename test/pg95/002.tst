--TEST--
PG95 Basic execution and selection                            ...
--POST--
--GET--
--FILE--
<?
	$conn = pg_connect("localhost", "5432", "", "", "template1");
	if (!$conn);
		echo "FAIL Connect";
		exit;
	endif;

	SetErrorReporting(0);
	$conn2 = pg_connect("localhost", "5432", "", "", "phptest");
	SetErrorReporting(1);
	if (!$conn2);
		$res = pg_exec($conn, "create database phptest");
		if (!$res);
			echo "FAIL create";
			exit;
		endif;
		$conn2 = pg_connect("localhost", "5432", "", "", "phptest");
		if (!$conn2);
			echo "FAIL Connect2";
			exit;
		endif;
	endif;


	$res = pg_exec($conn2, "create table test1 (t1 text, t2 int, t3 float)");
	if (!$res);
		echo "FAIL Table create";
		exit;
	endif;

	$res = pg_exec($conn2, "insert into test1 values ('lala', 1234, 56.78)");
	if (!$res);
		echo "FAIL insert1";
		exit;
	endif;
		
	/* throw in select on 1st conn just for fun */
	$res = pg_exec($conn, "select typname from pg_type where typname='varchar'");
	if (!$res);
		echo "FAIL select";
		exit;
	endif;

	pg_freeResult($res);

	$res = pg_exec($conn2, "insert into test1 values ('fafa', 9876, 54.32)");
	if (!$res);
		echo "FAIL insert1";
		exit;
	endif;
		

	$res = pg_exec($conn2, "select * from test1");
	if (pg_numRows($res) != 2);
		echo "FAIL select";
		exit;
	endif;

	$i = 0;
	$num = pg_numRows($res);
	while ($i < $num);
		echo pg_result($res, $i, "t1");
		echo "|";
		echo "%d" pg_result($res, $i, "t2");
		echo "|";
		echo "%.2f" pg_result($res, $i, "t3");
		echo "\n";
		$i++;
	endwhile;

	pg_close($conn2);

	$res = pg_exec($conn, "drop database phptest");
	pg_close($conn);

	echo "done\n";
>
--EXPECT--
Content-type: text/html
lala|1234|56.78
fafa|9876|54.32
done
