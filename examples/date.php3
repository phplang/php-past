<HTML>
<HEAD>
  <TITLE>Date/Time Functions Demo</TITLE>
</HEAD>

<BODY>

<H1>Date/Time Functions Demo</H1>

<DL>
<DT>Functions demonstrated</DT>
<DD><CODE>&lt;?date($format,$time)?&gt;</CODE>
<DD><CODE>&lt;?gmdate($format,$time)?&gt;</CODE>
<DD><CODE>&lt;?time()?&gt;</TT>
</DL>

<P>The <CODE>date()</CODE> function is used to display times and dates in
various ways. The function takes a format string and a time as arguments.
If the time argument is left off, the current time and date will be used.
The time argument is specified as an integer in number of seconds since
the Unix Epoch on January 1, 1970.  The format string is used to indicate
which date/time components should be displayed and how they should
be formatted.

<P>For example, if we wanted to print the current date and time,
we might use a tag like: <CODE>&lt;?echo date("D M d, Y H:i:s")?&gt;</CODE>.
This looks like: <EM><?echo date("D M d, Y H:i:s", time())?></EM>

<P>In the above you will find that the various characters in
the formatting string were replaced with a date/time component.
Any characters not replaced with anything were displayed verbosely.
The valid formatting characters are:

<CODE>
<UL>
  <LI>Y - Year eg. <?echo date("Y")?>
  <LI>y - Year eg. <?echo date("y")?>
  <LI>M - Month eg. <?echo date("M")?>
  <LI>m - Month eg. <?echo date("m")?>
  <LI>D - Day eg. <?echo date("D")?>
  <LI>d - Day eg. <?echo date("d")?>
  <LI>z - Day of the year eg. <?echo date("z")?>
  <LI>H - Hours in 24 hour format eg. <?echo date("H")?>
  <LI>h - Hours in 12 hour format eg. <?echo date("h")?>
  <LI>i - Minutes eg. <?echo date("i")?>
  <LI>s - Seconds eg. <?echo date("s")?>
  <LI>U - Seconds since epoch eg. <?echo date("U")?>
</UL>
</CODE>

<P>The <CODE>gmdate()</CODE> function is identical to the
<CODE>date()</CODE> function except that it uses Greenwich Mean Time
instead of the current local time.

<P>The <b>time()</b> function simply returns the current local
time in seconds since Unix epoch.  It is equivalent to calling
<CODE>date("U")</CODE>.

</BODY>
</HTML>
