--TEST--
PG95 Check connection and info funcs (see test/pg95/README)   ...
--POST--
--GET--
--FILE--
<?
	$conn = pg_connect("localhost", "5432", "-d 0", "/dev/null", "template1");
	if (!$conn);
		echo "FAIL connect to template1";
		exit;
	endif;

	echo "Info for \$conn\n";
	echo pg_host($conn);
	echo "\n";
	echo pg_port($conn);
	echo "\n";
	echo pg_options($conn);
	echo "\n";
	echo pg_tty($conn);
	echo "\n";
	echo pg_dbName($conn);
	echo "\n";

	SetErrorReporting(0);
	$connb = pg_connect("localhost", "5432", "-d 1", "/dev/null", "phptest");
	SetErrorReporting(1);
	if (!$connb);
		$res = pg_exec($conn, "create database phptest");
		if (!$res);
			echo "Could not create phptest database\n";
			exit;
		endif;

		$connb = pg_connect("localhost", "5432", "-d 1", "/dev/null", "phptest");
		if (!$connb);
			echo "Could not connect to phptest database\n";
			exit;
		endif;
	endif;

	

	echo "Info for \$connb\n";
	echo pg_host($connb);
	echo "\n";
	echo pg_port($connb);
	echo "\n";
	echo pg_options($connb);
	echo "\n";
	echo pg_tty($connb);
	echo "\n";
	echo pg_dbName($connb);
	echo "\n";

	pg_close($connb);

	$res = pg_exec($conn, "drop database phptest");
	if (!$res);
		echo "Could not destroy database phptest";
	endif;

	pg_close($conn);
>
--EXPECT--
Content-type: text/html
Info for $conn
localhost
5432
-d 0
/dev/null
template1
Info for $connb
localhost
5432
-d 1
/dev/null
phptest
