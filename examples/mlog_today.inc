<h1 align=center>Today's Summary for <?echo $fn></h1>
<center><form action="<?echo $PHP_SELF>" method=POST>
<input type="hidden" name="fn" value="<?echo $fn>">
<input type="hidden" name="PASSWORD" value="<?echo $PASSWORD>">
<input type="hidden" name="EMAIL_ADDR" value="<?echo $EMAIL_ADDR>">
<input type="hidden" name="screen" value="mlog_today.inc">
Host<input type="checkbox" name="show_host" <?if($show_host)>CHECKED<?endif> >
 Referer<input type="checkbox" name="show_ref" <?if($show_ref)>CHECKED<?endif> >
 Email<input type="checkbox" name="show_email" <?if($show_email)>CHECKED<?endif> >
 Browser<input type="checkbox" name="show_browser" <?if($show_browser)>CHECKED<?endif> ><br>
Order listing by <select name=order>
<?if(!$order); $order="timestamp"; endif>
<option><?echo $order>
<?if($order!="timestamp")>
	<option>timestamp
<?endif>
<?if($order!="host")>
	<option>host
<?endif>
<?if($order!="email")>
	<option>email
<?endif>
<?if($order!="lref")>
	<option>lref
<?endif>
<?if($order!="browser")>
	<option>browser
<?endif>
</select>
<input type="submit" value=" Redisplay ">
</form>
<form action="<?echo $PHP_SELF>" method=POST><table border=3>
<tr><th>Date</th><th>Time</th>
<?if($show_host)><th>From</th><?endif>
<?if($show_email)><th>Email</th><?endif>
<?if($show_ref)><th>Refering URL</th><?endif>
<?if($show_browser)><th>Browser</th><?endif>
</tr>
<?
	$uid = getmyuid();
	$m = date("m");
	$d = date("d");
	$y = date("y");
	$midnight = mktime(0,0,0,$m,$d,$y);
	$result = msql("phpfi","select * from log$uid where filename='$fn' and timestamp > $midnight order by $order desc");
	$num = msql_numrows($result);
	$i=0;
	while($i < $num);
		echo "<tr><td align=left>";
		$t = msql_result($result,$i,"timestamp");
		echo date("M d/y",$t);
		echo "</td><td align=center>";
		echo date("H:i:s",$t);
		if($show_host); 
			echo "</td><td align=center>";
			echo msql_result($result,$i,"host");
		endif;
		if($show_email); 
			echo "</td><td align=center>";
			echo msql_result($result,$i,"email");
		endif;
		if($show_ref); 
			echo "</td><td align=center>";
			echo msql_result($result,$i,"lref");
		endif;
		if($show_browser); 
			echo "</td><td align=center>";
			echo msql_result($result,$i,"browser");
		endif;
		echo "</td></tr>\n";
		$i++;
	endwhile;	
>
</table>
<select name="screen">
<option value="mlog_today.inc">Detailed Data for today's hits on a page
<option value="mlog_yesterday.inc">Detailed Data for yesterday's hits on a page
<option value="mlog_top.inc">Summary of hits on all pages
<option value="mlog_total.inc">Detailed Data for all hits on a page
<option value="mlog_ref.inc">Unique refering url's for a page
<input type="hidden" name="show_host" value="<?echo $show_host>">
<input type="hidden" name="show_email" value="<?echo $show_email>">
<input type="hidden" name="show_ref" value="<?echo $show_ref>">
<input type="hidden" name="show_browser" value="<?echo $show_browser>">
<input type="hidden" name="fn" value="<?echo $fn>">
<input type="hidden" name="PASSWORD" value="<?echo $PASSWORD>">
<input type="hidden" name="EMAIL_ADDR" value="<?echo $EMAIL_ADDR>">
</select><input type="submit" value=" View ">
</form></center>
