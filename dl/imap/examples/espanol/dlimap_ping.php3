<title> Imap Example </title>
<?php
    dl("imap.so");
    
    $mbox=imap_open("{rs120.readysoft.es}INBOX","prova","prova");
    $a = imap_ping( $mbox );
    echo "Ping: $a ";
    if ( !$mbox ){
       echo "Error de connexio";
    } else {
       imap_close($mbox);
    }
?>

