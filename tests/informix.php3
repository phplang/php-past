<script language=PHP>
//
// test script to exercise basic PHP3 Informix driver functionality
// uses the ODS/SE 7.x stores7 demo database
// sept 1998 Danny.Heijl@cevi.be
//

function check_ifx($res) {
    $msg = ifx_error($res);
    if ($msg[0] == ' ')
      return 1;
   else 
      return 0;
}

$res = ifx_connect("stores7@demo_se","informix","informix");
if (!check_ifx($res)) {
    printf("ifx_connect : %s %s\n", ifx_error($res), ifx_errormsg());
}
if (! $res) {
    printf("can not connect to server\n");
    exit;
}
printf("+----------------------------------------+\n");
printf("|        query : 'select * from orders'  |\n");
printf("+----------------------------------------+\n");
$query = "select * from orders";
$resultid = ifx_query($query, $res, IFX_SCROLL);
if (!check_ifx($resultid)) {
    printf("ifx_query : %s:%s \n", ifx_error($res), ifx_errormsg());
}
if (! $resultid) {
   printf("ifx_query : bailing out \n");
   die;
}
printf("+----------------------------------------+\n");
printf("|      affected rows :                   |\n");
printf("+----------------------------------------+\n");
printf("Estimated number of rows : %d\n\n", ifx_affected_rows($resultid));
printf("+----------------------------------------+\n");
printf("|        Number of columns :             |\n");
printf("+----------------------------------------+\n");

printf("Number of columns = %d\n", ifx_num_fields($resultid));

printf("+----------------------------------------+\n");
printf("|        field types and properties :    |\n");
printf("+----------------------------------------+\n");

unset($types);
unset($props);
$types = ifx_fieldtypes($resultid);
$props = ifx_fieldproperties($resultid);
if ((! isset($types)) || (! isset($props))) {
    printf("ifx_fieldxxxx : %s\n", ifx_error($res));
    ifx_close($res);
    die;
}
for ($i = 0; $i < count($types); $i++) {
    $fname = key($types);
    printf("%s :\t type =  %s\n", $fname, $types[$fname]);
    printf("%s :\t properties =  %s\n", $fname, $props[$fname]);
    next($types);
    next($props);
}

// the quick way :
// ifx_htmltbl_resultid($resultid,"border=\"1\"");

// with more user control :
printf("+----------------------------------------+\n");
printf("|       in normal order :                |\n");
printf("+----------------------------------------+\n");
unset($row);
$row = ifx_fetch_row($resultid, "first");
while (is_array($row)) {
  for(reset($row); $fname = key($row); next($row)) {
        printf("%s=%s;", $fname, $row[$fname]);
   }
   printf("\n");
   $row = ifx_fetch_row($resultid, "next");
}

printf("Number of rows fetched so far : %d\n", ifx_num_rows($resultid));

printf("+----------------------------------------+\n");
printf("|       and now in reverse order :       |\n");
printf("+----------------------------------------+\n");
unset($row);
$row = ifx_fetch_row($resultid, "last");
while (is_array($row)) {
  for(reset($row); $fname = key($row); next($row)) {
        printf("%s=%s;", $fname, $row[$fname]);
   }
   printf("\n");
   $row = ifx_fetch_row($resultid, "previous");
}

printf("Number of rows fetched so far : %d\n", ifx_num_rows($resultid));

printf("+-----------------------------------------+\n");
printf("|         and now row number 10           |\n");
printf("+-----------------------------------------+\n");
unset($row);
$row = ifx_fetch_row($resultid, 10);
for(reset($row); $fname = key($row); next($row)) {
    printf("%s=%s;", $fname, $row[$fname]);
}
printf("\n\nResultid was : %d\n",$resultid);
ifx_free_result($resultid);

printf("\n\n\n********* and now prepare/do test *******\n\n\n");

printf("+----------------------------------------+\n");
printf("| prepare query : 'select * from orders' |\n");
printf("+----------------------------------------+\n");
$query = "select * from orders";
$resultid = ifx_prepare($query, $res, IFX_SCROLL);
if (!check_ifx($resultid)) {
    printf("ifx_prepare : %s:%s \n", ifx_error($res), ifx_errormsg());
}
if (! $resultid) {
   printf("ifx_prepare : bailing out \n");
   die;
}
printf("+----------------------------------------+\n");
printf("|      affected rows :                   |\n");
printf("+----------------------------------------+\n");
printf("Estimated number of rows : %d\n\n", ifx_affected_rows($resultid));
printf("+----------------------------------------+\n");
printf("|        Number of columns :             |\n");
printf("+----------------------------------------+\n");

printf("Number of columns = %d\n", ifx_num_fields($resultid));

printf("+----------------------------------------+\n");
printf("|        field types and properties :    |\n");
printf("+----------------------------------------+\n");

unset($types);
unset($props);
$types = ifx_fieldtypes($resultid);
$props = ifx_fieldproperties($resultid);
if ((! isset($types)) || (! isset($props))) {
    printf("ifx_fieldxxxx : %s\n", ifx_error($res));
    ifx_close($res);
    die;
}
for ($i = 0; $i < count($types); $i++) {
    $fname = key($types);
    printf("%s :\t type =  %s\n", $fname, $types[$fname]);
    printf("%s :\t properties =  %s\n", $fname, $props[$fname]);
    next($types);
    next($props);
}
printf("+----------------------------------------+\n");
printf("| execute query : 'select * from orders' |\n");
printf("+----------------------------------------+\n");
if (! ifx_do($resultid)) {
    printf("ifx_query : %s:%s \n", ifx_error($res), ifx_errormsg());
    printf("ifx_do : bailing out\n");
    die;
}
printf("+----------------------------------------+\n");

// the quick way :
// ifx_htmltbl_resultid($resultid,"border=\"1\"");

// with more user control :
printf("+----------------------------------------+\n");
printf("|       in normal order :                |\n");
printf("+----------------------------------------+\n");
unset($row);
$row = ifx_fetch_row($resultid, "first");
while (is_array($row)) {
  for(reset($row); $fname = key($row); next($row)) {
        printf("%s=%s;", $fname, $row[$fname]);
   }
   printf("\n");
   $row = ifx_fetch_row($resultid, "next");
}
printf("+----------------------------------------+\n");
printf("|       and now in reverse order :       |\n");
printf("+----------------------------------------+\n");
unset($row);
$row = ifx_fetch_row($resultid, "last");
while (is_array($row)) {
  for(reset($row); $fname = key($row); next($row)) {
        printf("%s=%s;", $fname, $row[$fname]);
   }
   printf("\n");
   $row = ifx_fetch_row($resultid, "previous");
}

printf("+-----------------------------------------+\n");
printf("|         and now row number 10           |\n");
printf("+-----------------------------------------+\n");
unset($row);
$row = ifx_fetch_row($resultid, 10);
for(reset($row); $fname = key($row); next($row)) {
    printf("%s=%s;", $fname, $row[$fname]);
}
printf("\n");
unset($row);

printf("\n\nResultid was : %d\n",$resultid);
ifx_free_result($resultid);

printf("\n");
printf("+-----------------------------------------+\n");
printf("|      some execute immediates(prepared)  |\n");
printf("+-----------------------------------------+\n");

$query = "create table testdh (k serial, d char(100), primary key (k))";
$resultid = ifx_prepare($query, $res);
if (!check_ifx($resultid)) {
    printf("ifx_prepare : %s:%s \n", ifx_error($res), ifx_errormsg());
}
if (! $resultid) {
   printf("ifx_prepare : bailing out \n");
   die;
}
printf("create table successfully prepared\n");
if (! ifx_do($resultid)) {
    printf("ifx_query : %s:%s \n", ifx_error($res), ifx_errormsg());
    printf("ifx_do : bailing out\n");
    die;
}
printf("create table successfully executed\n");
ifx_free_result($resultid);
printf("\n");
printf("+-----------------------------------------+\n");
printf("|      some inserts (unprepared)           |\n");
printf("+-----------------------------------------+\n");

for ($i = 1; $i < 20; $i++) {
    $query = sprintf(
                  "insert into testdh values (0,\"- this is row number %d -\")",
	           $i);
    echo $query . "\n";
    $rc = ifx_query($query, $res);
    if (! $rc) {
        printf("insert fails : %s:%s\n", ifx_error($res), ifx_errormsg());
        die;
    }
    ifx_free_result($rc);
}

printf("\n");
printf("+-----------------------------------------+\n");
printf("|      some updates (unprepared)           |\n");
printf("+-----------------------------------------+\n");

for ($i = 1; $i < 20; $i++) {
    $query = sprintf(
                  "update testdh set d = \"- this is still row " .
                  "number %d -\" where k = %d",
	           $i, $i);
    echo $query . "\n";
    $rc = ifx_query($query, $res);
    if (! $rc) {
        printf("update fails : %s:%s\n", ifx_error($res), ifx_errormsg());
        die;
    }
    ifx_free_result($rc);
}


printf("\n");
printf("+-----------------------------------------+\n");
printf("|      deleting inserted rows             |\n");
printf("+-----------------------------------------+\n");
$query = "delete from testdh";
$resultid = ifx_prepare($query, $res);
if (!check_ifx($resultid)) {
    printf("ifx_prepare : %s:%s \n", ifx_error($res), ifx_errormsg());
}
printf("estimate : %d rows will be deleted\n", ifx_affected_rows($resultid));
if (!ifx_do($resultid)) {
    printf("ifx_do : %s:%s \n", ifx_error($res), ifx_errormsg());
    die;
}
printf("%d rows successfully deleted\n", ifx_affected_rows($resultid));

$query = "drop table testdh";
$resultid = ifx_prepare($query, $res);
if (!check_ifx($resultid)) {
    printf("ifx_prepare : %s:%s \n", ifx_error($res), ifx_errormsg());
    die;
}
if (! $resultid) {
   printf("ifx_prepare : bailing out \n");
   die;
}
printf("drop table successfully prepared\n");
if (! ifx_do($resultid)) {
    printf("ifx_query : %s:%s \n", ifx_error($res), ifx_errormsg());
    printf("ifx_do : bailing out\n");
    die;
}
printf("drop table successfully executed\n");
ifx_free_result($resultid);

printf("\n");
printf("+-----------------------------------------+\n");
printf("|      some execute immediates(unprepared)|\n");
printf("+-----------------------------------------+\n");
$query = "create table testdh (k serial, d char(100), primary key (k))";
$resultid = ifx_query($query, $res);
if (!check_ifx($resultid)) {
    printf("ifx_query : %s:%s \n", ifx_error($res), ifx_errormsg());
}
if (! $resultid) {
   printf("ifx_query : bailing out \n");
   die;
}
printf("create table successfully executed\n");
ifx_free_result($resultid);

$query = "drop table testdh";
$resultid = ifx_query($query, $res);
if (!check_ifx($resultid)) {
    printf("ifx_query : %s:%s \n", ifx_error($res), ifx_errormsg());
}
if (! $resultid) {
   printf("ifx_query : bailing out \n");
   die;
}
printf("drop table successfully executed\n");
ifx_free_result($resultid);

ifx_close($res);
printf("+-----------------------------------------+\n");
printf("|      --------  end ---------            |\n");
printf("+-----------------------------------------+\n");
</script>


