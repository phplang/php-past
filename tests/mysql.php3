<?
$conn=mysql_connect("127.0.0.1");
if(!mysql_select_db("test",$conn)){echo ("Cannot select database.\n");}
$sql="create table phptest (php char(20), test char(20))";
if(!mysql_query($sql,$conn))
  {
  echo ("Error creating test table.\n");
  }else{
  echo ("test table created.\n");
  } 
$sql="insert into phptest values('this is a','test insert')";
if(!mysql_query($sql,$conn))
  {
  echo ("Error Inserting data into test table.\n");
  }else{
  echo ("Data inserted to test table.\n");
  }
$sql="select * from phptest";
if(!($result=mysql_query($sql,$conn)))
  {
  echo ("Error selecting data from test table.\n");
  }else{
  echo ("Data selected from test table.\n");

  while(($data=mysql_fetch_row($result))){
    echo("$data[0] $data[1]\n");
    }
  
  }


$sql="delete from phptest where test like 'test%'";
if(!mysql_query($sql,$conn))
  {
  echo ("Error deleting data from test table.\n");
  }else{
  echo ("Data deleted from test table.\n");
  }
if(!mysql_query($sql,$conn))
  {
  echo ("Error Inserting data into test table.\n");
  }else{
  echo ("Data inserted to test table.\n");
  }
$sql="drop table phptest";
if(!mysql_query($sql,$conn))
  {
  echo ("Error deleting test table.\n");
  }else{
  echo ("test table deleted.\n");
  }
mysql_close($conn);
?>