<HTML>
<HEAD>
  <TITLE>Directory routines demo</TITLE>
</HEAD>

<BODY>
<H1>Directory routines demo</H1>

<DL>
<DT>Functions demonstrated
<DD><CODE>&lt;?opendir("/path/dir")?&gt;</CODE>
<DD><CODE>&lt;?closedir()?&gt;</CODE>
<DD><CODE>&lt;?readdir()?&gt;</CODE>
<DD><CODE>&lt;?rewinddir()?&gt;</CODE>
</DL>

<P>It is often important to be able to "walk" through a directory
and do something based on the files in the directory.  PHP includes
functions that will let you do just that.  The first thing to
do is to open the directory.  The syntax for doing that is:

<PRE>
&lt;?$dir = opendir("/path/dir")?&gt;
</PRE>

<P>Once opened, the <CODE>readdir($dir)</CODE> function may be used
to read entries in the directory sequentially.  The first call to
<CODE>readdir($dir)</CODE> will return the first entry and the second call
the second entry, etc.

<P>The <CODE>rewinddir($dir)</CODE> function can be used to move the
directory pointer back to the beginning resulting in the next call to
<CODE>readdir($dir)</CODE> returning the first entry in the directory.

<P>And, at the end, the <CODE>closedir($dir)</CODE> function should be
called.

<P>Below is a simple example which reads the contents of the
directory in which this PHP script resides.

<P>Code:

<PRE>
&lt;?
<? $code = "\$dir = opendir(\".\");
  while(\$file = readdir(\$dir)) {
    echo \"\$file<BR>\";
  }
  closedir(\$dir);
";
  echo HTMLSpecialChars($code);
  echo "?>";
?>
</PRE>

<P>Output:

<PRE>
<?eval($code);?>
</PRE>

<HR>

<P>In PHP3, there is also an object-oriented syntax for working with
directories. So, for example, the code above would be:

<PRE>
&lt;?
<? $code = "\$dir = dir(\".\");
  while(\$file = \$dir->read()) {
    echo \"\$file<BR>\";
  }
  \$dir->close();
";
  echo HTMLSpecialChars($code);
  echo "?>";
?>
</PRE>

<P>Output:

<PRE>
<?eval($code);?>
</PRE>
</BODY>
</HTML>
