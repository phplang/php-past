<H1> Informix Test-Page </H1>
requires the stores7-database<P>

Set blob-output of select-qeuries to memory: 
<?
  ifx_blobinfile_mode(0);   //0=in memory, 1=in file
?>
<B>Done</B><P>

Connecting Informix-Server (adapt user/password if needed): 
<?
  $cid=ifx_connect("stores7@turin_on1","informix","4ever"); 
?>
<B>Done</B><P>

Create a TEXT-BLOB:
<?  
  $bidt=ifx_create_blob(1,0,"TEXT in memory");
?>  
<B>Done</B><P>
Create a BYTE-BLOB:
<?  
  $bidb=ifx_create_blob(0,0,"BYTE in memory");
?>  
<B>Done</B><P>

Insert a line into calatog:
<?
$textasvarchar=1;
$byteasvarchar=1;

ifx_nullformat(1);
ifx_textasvarchar($textasvarchar);
ifx_byteasvarchar($byteasvarchar);
  $idarray[]=$bidt;
  $idarray[]=$bidb;
  $qid=ifx_prepare('insert into catalog (stock_num, manu_code,cat_descr,cat_picture) values(1,"HRO",?,?)',$cid,$idarray);
  ifx_do($qid);
  $qid=ifx_query('insert into catalog (stock_num, manu_code,cat_descr,cat_picture) values(1,"HRO",?,?)',$cid,$idarray);

?>
<B>Done</B><P>

freeing blob-resources:
<?
 ifx_free_blob($bidt);
 ifx_free_blob($bidb);
?>
<B>Done</B><P>

Prepare a list of catalog:
<?
  $rid=ifx_prepare("select * from catalog order by catalog_num",$cid,"HOLD");
  ifx_do($rid);
?>
<B>Done</B><P>

Stats:<BR>
<?
  echo "Number of Columns: ".ifx_num_fields($rid)."<BR>";
  $prop=ifx_fieldproperties($rid);
  for(reset($prop);$fieldname=key($prop);next($prop)) {
   echo "<B>$fieldname:</B>";
   $proparray=explode(";",$prop[$fieldname]);
   echo "sqltype: ".$proparray[0]." / "; 
   echo "length: ".$proparray[1]." / "; 
   echo "percision: ".$proparray[2]." / "; 
   echo "scale: ".$proparray[3]." / "; 
   echo "nullable: ".$proparray[4]." <BR> "; 
  }
?>
<P>
  

<B>Content:</B><P>
<?   
   
  $array=ifx_fetch_row($rid,"NEXT");
  while(is_array($array)) {
   for(reset($array);$fieldname=key($array);next($array)) {
    if($fieldname=="cat_picture" && $byteasvarchar==0):
     $tmp=ifx_get_blob($array["$fieldname"]);
     echo $array["$fieldname"]."  - ".$tmp." / ";
     ifx_free_blob($array["$fieldname"]);
    elseif($fieldname=="cat_descr" && $textasvarchar==0):
     $tmp=ifx_get_blob($array["$fieldname"]);
     echo $array["$fieldname"]."  - ".$tmp." / ";
     ifx_free_blob($array["$fieldname"]);
    else:    
     echo $array["$fieldname"]." / ";
    endif;
   }
   echo "<br>";
   $array=ifx_fetch_row($rid);
   }
?>
<P>

delete testline in calatog:
<?
  $qid=ifx_prepare('delete from catalog where stock_num=1',$cid);
  ifx_do($qid);

  ifx_close($cid);
?>
<B>Done</B><P>

<P>
<H1>Test completed</H1>

