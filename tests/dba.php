<?php

error_reporting(15);

function dba_test($handler, $readonly = false) {
	echo "testing with $handler\n";
	$count = 1000;
	if($readonly) 
		$h = dba_open("test.$handler", "r", $handler, 0600);
	else
		$h = dba_open("test.$handler", "n", $handler, 0600);
	
	if(!$h) 
		die("cannot open db\n");
	
	if(!$readonly) {
	echo "replacing\n";
	for($i = 0; $i < $count; $i++)
		if(!dba_replace($i, "test$i.val", $h))
			die( "dba_replace failed for $i\n");

	echo "optimizing\n";
	if(!dba_optimize($h)) 
		die( "dba_optimize failed\n");
	}

	echo "fetching\n";
	for($i = 0; $i < $count; $i++)
		if(dba_fetch($i, $h) == false || dba_fetch($i, $h) != "test$i.val") 
			die ("dba_fetch failed for $i\n");
	
	echo "traversing\n";
	$n = 0;
	for($key = dba_firstkey($h); $key != false; $key = dba_nextkey($h))
		$n++;

	if($n != $count) 
		die ("db broken: $n entries, $count expected\n");
	
	echo "testing existence\n";
	for($i = 0; $i < $count; $i++) 
		if(!dba_exists($i, $h)) 
			die("dba_exists failed for $i\n");
	
	
	if(!$readonly) {
	echo "deleting\n";
	for($i = 0; $i < $count; $i++)
		if(!dba_delete($i, $h))
			die ("dba_delete failed for $i\n");
	}

	echo "traversing again\n";
	$n = 0;
	for($key = dba_firstkey($h); $key != false; $key = dba_nextkey($h))
		$n++;
	if($n != 0) die("database still contains data\n");

	dba_optimize($h);
	dba_close($h);
	echo "passed\n\n";
}

dba_test("cdb", true);

$dbs = array("db2", "dbm", "ndbm", "gdbm");

for($i = 0; $i < count($dbs); $i++)
	dba_test($dbs[$i]);
