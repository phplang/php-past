<HTML>
<HEAD>
<TITLE>Quick &amp; dirty ODBC test #2</TITLE>
</HEAD>
<BODY>
<H1>ODBC Test 2 - Create table</H1>
<?
if(isset($dbuser)){
?>
Connecting to <? echo $dsn; ?> as <? echo $dbuser; ?>
<?
$conn = odbc_connect($dsn,$dbuser,$dbpwd);
if (!$conn){
?>
<H2>Error connecting to database! Check DSN, username and password</H2>
<?
}else{
?>
- OK<p>
Dropping table "php3_test"
<?
Error_Reporting(0);
$res = odbc_exec($conn,"drop table php3_test");
if($res){
	odbc_free_result($res);
	$res = 0;
}
?>
- OK<p>
Create table "php3_test"
<?
error_reporting(1);
$res = odbc_exec($conn,"create table php3_test (a char(16), b integer, c float, d varchar(128))");
if($res){
	echo " - OK<p>Table Info:<br>";
	$info = odbc_exec($conn,"select * from php3_test");
	$numfields = odbc_num_fields($info);
	echo "<table><tr><th>Name</th><th>Type</th><th>Length</th></tr>\n";
	for($i=1;$i<=$numfields;$i++){
	printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n", odbc_field_name($info,$i),odbc_field_type($info,$i),odbc_field_len($info,$i));
	}
	echo "</table>";
?>
<p><HR width=50%"><p>
<A HREF="odbc-t3.php3<? echo "?dbuser=",$dbuser,"&dsn=",$dsn,"&dbpwd=",$dbpwd; ?>">Proceed to next test</A> 
| <A HREF="<? echo $PHP_SELF; ?>">Change login information</A>

<?
	}
    }
} else {
?>
<form action="odbc-t3.php3" method=post>
<table border=0>
<tr><td>Database (DSN): </td><td><input type=text name=dsn></td></tr>
<tr><td>User: </td><td><input type=text name=dbuser></td></tr>
<tr><td>Password: </td><td><input type=password name=dbpwd></td></tr>
</table>
<input type=submit value="Continue">

</form>
<? 
} ?>
</BODY>
</HTML>
