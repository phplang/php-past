<script language=PHP>
//
// simple serial insert test script for the PHP3 Informix driver
// uses the ODS/SE 7.x demo database
// Mar 1999 Danny.Heijl@cevi.be
//

function check_ifx($res) {
    $msg = ifx_error($res);
    if ($msg[0] == ' ')
      return 1;
   else 
      return 0;
}

function do_query($qry, $connid) {
 $r = ifx_query($qry, $connid);
 if (!$r) {
    printf("Can't run query %s\n", $qry);
    printf("%s %s\n", ifx_error(), ifx_errormsg());
    die;
 } 
 return $r;
}


$cid = ifx_connect("stores7@demo_se","informix","informix");
if (!$cid) {
    printf("ifx_connect : %s %s\n", ifx_error(), ifx_errormsg());
    printf("can not connect to server\n");
    die;
}
printf("Connected to 'stores7'\n");

$rid=ifx_query("drop table tserial", $cid);
if ($rid) {
    printf("Table 'tserial' dropped\n");	
    ifx_free_result($rid);
}

$rid = ifx_query("create table tserial (kser serial(1), dser char(20))", $cid);
if ($rid) {
    printf("Table 'tserial' created\n");	
    ifx_free_result($rid);
}

echo "Inserting row 1\n";
$qid = do_query("insert into tserial values(0, 'TEST 1')", $cid);

/* use the values saved by the ifx driver (faster) */

echo "Affected rows from ifx_affected_rows() : " . ifx_affected_rows($qid) . "\n";
$row = ifx_getsqlca($qid);
echo "Serial value from ifx_getsqlca() : " . $row["sqlerrd1"] . "\n";
ifx_free_result($qid);

/* use the 'select dbinfo' method to check the values */
/* the order of these 2 selects is important here     */

$qid = do_query("select dbinfo('sqlca.sqlerrd1') as sqlerrd1 from systables where tabid = 1", $cid);
$row=ifx_fetch_row($qid);
echo "Serial value from 'dbinfo' : " . $row["sqlerrd1"] . "\n";
ifx_free_result($qid);

$qid = do_query("select dbinfo('sqlca.sqlerrd2') as sqlerrd2 from systables where tabid = 1", $cid);
$row=ifx_fetch_row($qid);
echo "Affected rows from 'dbinfo' : " . $row["sqlerrd2"] . "\n";
ifx_free_result($qid);


echo "Inserting row 2\n";
$qid = do_query("insert into tserial values(0, 'TEST 2')", $cid);

/* use the values saved by the ifx driver (faster) */

echo "Affected rows from ifx_affected_rows() : " . ifx_affected_rows($qid) . "\n";
$row = ifx_getsqlca($qid);
echo "Serial value from ifx_getsqlca() : " . $row["sqlerrd1"] . "\n";
ifx_free_result($qid);

/* use the 'select dbinfo' method to check the values */
/* the order of these 2 selects is important here     */

$qid = do_query("select dbinfo('sqlca.sqlerrd1') as sqlerrd1 from systables where tabid = 1", $cid);
$row=ifx_fetch_row($qid);
echo "Serial value from 'dbinfo' : " . $row["sqlerrd1"] . "\n";
ifx_free_result($qid);

$qid = do_query("select dbinfo('sqlca.sqlerrd2') as sqlerrd2 from systables where tabid = 1", $cid);
$row=ifx_fetch_row($qid);
echo "Affected rows from 'dbinfo' : " . $row["sqlerrd2"] . "\n";
ifx_free_result($qid);

ifx_close($cid);
</script>
