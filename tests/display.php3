<?
if(($conn = odbc_connect($dsn, $dbuser, $dbpwd))){
	if(($res = odbc_do($conn, "select gif from php3_test where id='$id'"))){
		odbc_binmode($res, 0);
		odbc_longreadlen($res, 0);
		if(odbc_fetch_row($res)){
			header("content-type: image/gif");
			odbc_result($res, 1);
			exit;
		}
	}
} 
echo "Oops";

