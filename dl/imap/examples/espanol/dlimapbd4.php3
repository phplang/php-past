<title> Imap Example </title>
<?php
    dl("imap.so");

    $mbox=imap_open("{rs120.readysoft.es}INBOX","prova","prova");
    if ( !$mbox ){
       echo "Error de connexio";
    } else {
       $bd = imap_fetchbody( $mbox , $msg, "$secciona" );
       echo "<b>Missatge : </b><br><pre>" . $bd . "</pre><br>";
       $ab =  base64_decode( $bd );
       if ($ab){
          echo "<b>Missatge : </b><br><pre>" . $ab . "</pre><br>";
        }
       imap_close($mbox);
    }
?>

