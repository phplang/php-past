<?
	include "include/auth.inc";
	$title = "Upload a Picture";
	$page = "upload";
	include "include/header.inc";
?>
<?if(!$file) { ?>

<form action="upload.php3" enctype="multipart/form-data" method=POST>
<input type="hidden" name="MAX_FILE_SIZE" value="1000000">
<table border=2>
<tr><th align=right>Album: </th>
<td><select name=album>
<option value="none"> -- Create new album --
<?
	$albums = mysql_query("select distinct album from photos");
	while($row = mysql_fetch_row($albums)) {
		echo "<option>".stripslashes($row[0])."\n";
	}
	mysql_free_result($albums);
?>
</select> 
<input type=text name=new_album value="<new album name>"></td></tr>
<tr><th align=right>Filename: </th>
<td><input type=file name=file size=29></td></tr>
<tr><th align=right>Description: </th><td><input type=text name=desc size=42></td></tr>
</table>
<input type="submit" value=" Send File ">
</form>

<? } else {
    if(!filetype("uploads")) {
        if(!@mkdir("uploads",0777)) {
            echo "Unable to create uploads directory - check permissions<br>\n";
			exit;
        }
    }
	$dest = "$DOCUMENT_ROOT/".dirname($PHP_SELF)."/uploads/$file_name";
	if(@exec("cp $file $dest")!=0) {
		echo "Unable to create $dest - check permissions<br>\n";
		exit;
	}
	unlink($file);
	if($album=="none") {
		$album = $new_album;
	}
	if($ret=mysql_query("insert into photos values ('uploads/$file_name','$album','$desc')")) {
		$size = getimagesize("uploads/$file_name");
		echo "You uploaded this photo:<P><img src=\"uploads/$file_name\" $size[3]><br>";
		echo stripslashes($desc);
		$page="received";
	} else {
		echo "Unable to insert the photo in the database ($ret)<P>\n";
	}
}

	include "include/footer.inc";
?>
