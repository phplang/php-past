<? 
	$theight = 50;           /* height of thumbnails */

	mysql_pconnect("localhost","nobody","") or die("Unable to connect to SQL server");
	mysql_select_db("rasmus") or die("Unable to select database");
?>
<html><head><title><?echo $title?></title>
</head>
<body bgcolor="#000000" text="#FFCA2F" link="#DEC000" vlink="#ffffff">
<FONT FACE="Verdana, Arial, Helvetica">
<h1 align=center><?echo $title?></h1>
<br>
