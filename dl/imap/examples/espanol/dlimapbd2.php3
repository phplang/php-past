<title> Imap Example </title>
<?php
    dl("imap.so");

    $mbox=imap_open("{rs120.readysoft.es}INBOX","prova","prova");
    if ( !$mbox ){
       echo "Error de connexio";
    } else {
       $nmes = imap_num_msg( $mbox );
       for ( $i=1 ; $i <= $nmes ; $i++){
         $bd = imap_fetchbody( $mbox ,$i, "0" );
         echo "Longitud : " . $bd . "<br>";
         $bd = imap_fetchbody( $mbox ,$i, "1" );
         echo "Longitud : " . $bd . "<br>";
         $bd = imap_fetchbody( $mbox ,$i, "2" );
         echo "Longitud : " . $bd . "<br>";
         $bd = imap_fetchbody( $mbox ,$i, "2.1" );
         echo "Longitud : " . $bd . "<br>";
         $bd = imap_fetchbody( $mbox ,$i, "3" );
         echo "Longitud : " . $bd . "<br>";
         $bd = imap_fetchbody( $mbox ,$i, "4" );
         echo "Longitud : " . $bd . "<br>";
       }
       imap_close($mbox);
    }
?>

