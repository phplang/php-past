<?
set_time_limit(0);
$conn = msql_connect();

if(!$conn) {
	echo "cannot connect";
}

if(!msql_select_db("sas", $conn)) {
	echo "cannot select database\n";
}

$sql="create table phptest (php char(20), test char(20))";

if(!msql_query($sql,$conn))
{
	echo ("Error creating test table.\n");
} else {
	echo ("test table created.\n");
} 

$sql="insert into phptest values('this is a','test insert')";

if(!msql_query($sql,$conn))
{
	echo ("Error Inserting data into test table.\n");
} else {
	echo ("Data inserted to test table.\n");
}

for($i = 0; $i < 50000; $i++) {
	$a = msql_query("INSERT INTO phptest VALUES ('asda      sdfssdasd','test $i')",$conn);
	msql_free_result($a);
}
	
	$sql="select * from phptest";
if(!($result=msql_query($sql,$conn)))
{
	echo ("Error selecting data from test table.\n");
}else{
	echo ("Data selected from test table.\n");

$i = msql_num_rows($result);
echo "num rows: $i\n";	
$i = msql_num_fields($result);
echo "num fields: $i\n";

    while(($data=msql_fetch_row($result))){
		echo("$data[0] $data[1]\n");
	}

}

#	$sql="delete from phptest where test like 'test%'";
#if(!($result = msql_query($sql,$conn)))
#{
#	echo ("Error deleting data from test table.\n");
#}else{
#	echo ("Data deleted from test table.\n");
#}
$af = msql_affected_rows($result);
echo "affected rows: $af\n";
	$sql="drop table phptest";
if(!msql_query($sql,$conn))
{
	echo ("Error deleting test table.\n");
}else{
	echo ("test table deleted.\n");
}
echo "freeing\n";
msql_free_result($result);
echo "dieing\n";
msql_close($conn);
?>
