<script language=PHP>
//
// simple BLOB test script for the PHP3 Informix driver
// uses the ODS 7.x stores7 demo database
// sept 1998 Danny.Heijl@cevi.be
//

function check_ifx($res) {
    $msg = ifx_error($res);
    if ($msg[0] == ' ')
      return 1;
   else 
      return 0;
}

//
// execute informix query, blob id array is optional
// return informix result id
//

function do_query($qry, $connid, $blobar="") {
 if (is_array($blobar))
    $r = ifx_query($qry, $connid, $blobar);
 else
    $r = ifx_query($qry, $connid);
 if (!$r) {
    printf("Can't run query %s\n", $qry);
    printf("%s %s\n", ifx_error(), ifx_errormsg());
    die;
 } 
 return $r;
}


//
// main
//

printf(">>-INFORMIX BLOB TEST-<<\n");
printf("------------------------\n");

$cid = ifx_connect("stores7@ol_s40lm","informix","informix");
if (!$cid) {
    printf("ifx_connect : %s %s\n", ifx_error(), ifx_errormsg());
    printf("can not connect to server\n");
    die;
}
printf(">>-Connected to 'stores7'\n");

$rid=ifx_query("drop table testblobs", $cid);
if ($rid) {
    printf(">>-Table 'testblobs' dropped\n");	
    ifx_free_result($rid);
}

$query = "CREATE TABLE testblobs (k SERIAL PRIMARY KEY, t TEXT, b BYTE)";
$rid = do_query($query, $cid);
ifx_free_result($rid);
printf(">>-Table 'testblobs' created\n");

// use blob id's for select statements
ifx_textasvarchar(0);
ifx_byteasvarchar(0);

// blobs in memory, not in files
ifx_blobinfile_mode(0);

// return NULL columns as empty strings 
ifx_nullformat(0);

// create blob id for text column
$tid = ifx_create_blob(1, 0, "This is the text that will be stored \n" .
                             "in the TEXT column if all goes well.\n");
// create blob id for byte column
$bid = ifx_create_blob(0, 0, "This is the text that will be stored \n" .
                             "in the BYTE column if all goes well.\n");

// store both blob ids in the blob param array
$blobarray[0] = $tid;
$blobarray[1] = $bid;

// insert 
$query = "insert into testblobs values (0, ?, ?)";                             
$rid = do_query($query, $cid, $blobarray);
ifx_free_result($rid);

printf(">>-Blob columns inserted\n");

// destroy blobs
ifx_free_blob($tid);
ifx_free_blob($bid);

printf(">>-Now selecting blobs with blob ids\n");

// get them back from the "testblobs" table                           
$query = "select * from testblobs";
$rid = do_query($query, $cid);
$row = ifx_fetch_row($rid);
if (is_array($row)) {
   printf("Key : %s\n", $row["k"]);
   $tid = $row["t"];
   $bid = $row["b"];
   printf("Text column contents : %s\n", ifx_get_blob($tid));
   printf("Byte column contents : %s\n", ifx_get_blob($bid));
} else {
   printf("*** ERROR : can not fetch the data back !!\n");
}
// destroy blobs (autocreated by select !!!)
ifx_free_blob($tid);
ifx_free_blob($bid);

ifx_free_result($rid);

// now get the blobs in "varchar mode" without blob ids
printf(">>-Now getting blobs without blob ids\n");
ifx_textasvarchar(1);
ifx_byteasvarchar(1);

$query = "select * from testblobs";
$rid = do_query($query, $cid);

$row = ifx_fetch_row($rid);
if (is_array($row)) {
   for (reset($row); $colname=key($row); next($row)) {
   	printf("%s = %s\n", $colname, $row[$colname]);
   }
} else {
   printf("*** ERROR : can not fetch the data back !!\n");
}


printf(">>-Done\n");
ifx_free_result($rid);


ifx_close($cid);

</script>
