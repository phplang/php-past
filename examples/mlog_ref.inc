<h1 align=center>Unique Refering URL's for <?echo $fn></h1>
<center>
<form action="<?echo $PHP_SELF>" method=POST><table border=3>
<tr><th>URL</th><th>Count</th></tr>
<?
	$uid = getmyuid();
	$result = msql("phpfi","select lref from log$uid where filename='$fn'");
	$num = msql_numrows($result);
	$i=0;
	while($i < $num);
		$u = msql_result($result,$i,"lref");
		if(strlen($u));
			 if(!$url[$u]); $url[$u]=1; else; $url[$u]++; endif;
		endif;
		$i++;
	endwhile;
	$num = count($url);
	reset($url);
	$i=0;
	while($i < $num);
		echo "<tr><td align=left>";
		$u = key($url);
		$uc = $url[];
		echo "<a href=\"$u\">$u</a>";
		echo "</td><td align=right>$uc</td></tr>\n";
		$i++;
	endwhile;	
>
</table>
<select name="screen">
<option value="mlog_ref.inc">Unique refering url's for a page
<option value="mlog_top.inc">Summary of hits on all pages
<option value="mlog_today.inc">Detailed Data for today's hits on a page
<option value="mlog_yesterday.inc">Detailed Data for yesterday's hits on a page
<option value="mlog_total.inc">Detailed Data for all hits on a page
<input type="hidden" name="show_host" value="on">
<input type="hidden" name="fn" value="<?echo $fn>">
<input type="hidden" name="PASSWORD" value="<?echo $PASSWORD>">
<input type="hidden" name="EMAIL_ADDR" value="<?echo $EMAIL_ADDR>">
</select><input type="submit" value=" View ">
</form></center>
