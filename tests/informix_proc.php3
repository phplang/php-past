<script language=PHP>
//
// simple stored procedure test script for the PHP3 Informix driver
// uses the ODS/SE 7.x demo database
// sept 1998 Danny.Heijl@cevi.be
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

$rid=ifx_query("drop procedure read_address", $cid);
if ($rid) {
    printf("Procedure 'create_address' dropped\n");	
    ifx_free_result($rid);
}

$query = 
"CREATE PROCEDURE read_address  (lastname CHAR(15)) " .
    "RETURNING CHAR(15), CHAR(15), CHAR(20), CHAR(15),CHAR(2), CHAR(5); " .
    "DEFINE p_fname, p_lname, p_city CHAR(15); ".
    "DEFINE p_add CHAR(20); ".
    "DEFINE p_state CHAR(2); ".
    "DEFINE p_zip CHAR(5); ".
    "FOREACH ".
        "SELECT fname, lname, address1, city, state, zipcode ".
            "INTO p_fname, p_lname, p_add, p_city, p_state, p_zip ".
            "FROM customer ".
            "WHERE lname LIKE lastname ".
         "RETURN p_fname, p_lname, p_add, p_city, p_state, p_zip WITH RESUME; ".
    "END FOREACH; ".
"END PROCEDURE;";

$rid = do_query($query, $cid);
ifx_free_result($rid);
printf("Procedure 'create_address' created\n");

$query = "execute procedure read_address('P%')";
$rid = do_query($query, $cid);
printf("procedure 'create_address' executed\n");

$row = ifx_fetch_row($rid);
while (is_array($row)) {
   printf("----Result set :---------------------\n");
   for(reset($row); $fn = key($row); next($row)) {
       printf("\t%s=%s\n", $fn, $row[$fn]);
   }
   $row = ifx_fetch_row($rid);
}
printf("---------------EOF-------------------\n");
ifx_free_result($rid);

ifx_close($cid);

</script>
