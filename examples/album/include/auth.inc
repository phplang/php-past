<?
    cfunction authenticate() {
        Header("WWW-authenticate: basic realm=\"Photo Album\"");
        Header("HTTP/1.0 401 Unauthorized");
        $title="Invalid Login";
        include "include/header.inc";?>
        In order to proceed you will need a valid username/password.
        <?include "include/footer.inc";
        exit;
    }

    if(!isset($PHP_AUTH_USER)) {
        authenticate();
    } else {
        mysql_pconnect("localhost","nobody","") or die("Unable to connect to SQL server");
        mysql_select_db("rasmus") or die("Unable to select database");
        $id=strtolower($PHP_AUTH_USER);
        $query = mysql_query("select * from users where id='$id' and password='$PHP_AUTH_PW'");
        if(!mysql_num_rows($query)) {
            authenticate();
        }
    }
?>
