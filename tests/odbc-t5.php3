<HTML>
<HEAD>
<TITLE>Database test #5</TITLE>
</HEAD>
<BODY>
<H1>ODBC Test 5 - Blobs</H1>
<?
if(!isset($gif1file) && !isset($display) || ($gif1file == "none" && $gif2file == "none" && $gif3file == "none")){
?>
<H2>Please select the images (gif) you want to put into the database</H2>
<FORM METHOD="POST" ACTION="<? echo $PHP_SELF ?>" ENCTYPE="multipart/form-data">
Image 1: <INPUT TYPE="file" NAME="gif1file" VALUE="" SIZE="48"><P>
Image 2: <INPUT TYPE="file" NAME="gif2file" VALUE="" SIZE="48"><P>
Image 3: <INPUT TYPE="file" NAME="gif3file" VALUE="" SIZE="48"><P>
Blob database type name: <INPUT  TYPE="text" NAME="datatype" VALUE="LONG BYTE" SIZE="32"><P>
<INPUT  TYPE="hidden"  name="dsn" value="<? echo $dsn ?>">
<INPUT  TYPE="hidden"  name="dbuser" value="<? echo $dbuser ?>">
<INPUT  TYPE="hidden"  name="dbpwd" value="<? echo $dbpwd ?>">
<INPUT  TYPE="submit"  VALUE="Send  File(s)">
| <INPUT  TYPE="reset"  VALUE="reset">
</FORM>
</BODY>
</HTML>
<?
	exit;
}
 
if(isset($dbuser)){
?>
Connecting to <? echo $dsn; ?> as <? echo $dbuser; ?>
<?
	$conn = odbc_connect($dsn, $dbuser, $dbpwd);
	if(!$conn){
?>
<H2>Error connecting to database! Check DSN, username and password</H2>
<?
	}else{
?>
 - OK<p>
<?
		if(isset($display)){
			if(($res = odbc_exec($conn, "select id from php3_test"))){
				echo "<H3>Images in database</H3>";
				while(odbc_fetch_into($res, &$imgs)){
					echo $imgs[0], ": <IMG SRC=\"display.php3?id=", $imgs[0], "&dbuser=", $dbuser, "&dsn=", $dsn, "&dbpwd=", $dbpwd, "\"><P>";
				}
			}else{
				echo "Couldn't execute query";
			}
			echo "</BODY></HTML>";
			exit;
		}
?>
Dropping table "php3_test"
<?
		Error_Reporting(0);
		$res = odbc_exec($conn, "drop table php3_test");
		if($res){
			odbc_free_result($res);
		}
?>
 - OK<p>
Creating table "php3_test":
<?
		$res = odbc_exec($conn,"create table php3_test (id char(32), gif LONG BYTE)");
		if($res){
			echo " - OK<p>Table Info:<br>";
			$info = odbc_exec($conn,"select * from php3_test");
			$numfields = odbc_num_fields($info);
			echo "<table><tr><th>Name</th><th>Type</th><th>Length</th></tr>\n";
			for($i = 1; $i <= $numfields; $i++){
				printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",
					odbc_field_name($info, $i), odbc_field_type($info, $i),
					odbc_field_len($info, $i));
			}
			echo "</table>";
			odbc_free_result($info);
?>
Inserting data:
<?
			odbc_free_result($res);
			$res = odbc_prepare($conn, "insert into php3_test values(?,?)");
			if($gif1file != "none"){
				$params[0] = "image1";
				$params[1] = "'$gif1file'";
				odbc_execute($res, $params);
				unlink($gif1file);
			}
			if($gif2file != "none"){
				$params[0] = "image2";
				$params[1] = "'$gif2file'";
				odbc_execute($res, $params);
				unlink($gif2file);
			}
			if($gif3file != "none"){
				$params[0] = "image3";
				$params[1] = "'$gif3file'";
				odbc_execute($res, $params);
				unlink($gif3file);
			}
?>
 - OK<P>
<A HREF="<? echo "$PHP_SELF?display=y&dbuser=$dbuser&dsn=$dsn&dbpwd=$dbpwd" ?>">Display Images</A>
<?
		}
	}
} else {
?>
<form action=t5.php3 method=post>
<table border=0>
<tr><td>Database: </td><td><input type=text name=dsn></td></tr>
<tr><td>User: </td><td><input type=text name=dbuser></td></tr>
<tr><td>Password: </td><td><input type=password name=dbpwd></td></tr>
</table>
<input type=submit value=connect>

</form>
<? 
}
?>
</BODY>
</HTML>
