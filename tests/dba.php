<?php

error_reporting(15);

function dba_test($handler) {
	echo "testing with $handler\n";
	$count = 100;
	$h = dba_open("test.$handler", "n", $handler, 0600);
	
	if(!$h) 
		die("cannot open db\n");

	echo "replacing\n";
	for($i = 0; $i < $count; $i++)
		if(!dba_replace($i, "test$i.val", $h))
			die( "dba_replace failed for $i\n");

	echo "optimizing\n";
	if(!dba_optimize($h)) 
		die( "dba_optimize failed\n");

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
	
	
	echo "deleting\n";
	for($i = 0; $i < $count; $i++)
		if(!dba_delete($i, $h))
			die ("dba_delete failed for $i\n");

	dba_close($h);
	echo "passed\n\n";
}

# cdb only supports reading and is not tested as such
$dbs = array("dbm", "gdbm", "ndbm", "db2");

for($i = 0; $i < count($dbs); $i++) dba_test($dbs[$i]);

