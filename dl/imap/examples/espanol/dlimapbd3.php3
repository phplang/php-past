<title> Imap Example </title>
<?php
    dl("imap.so");

    function display_type( $type, $subtype ){
      switch( $type ){
         case 0: $result= "TEXT";
                 break;
         case 1: $result= "MULTIPART";
                 break;
         case 2: $result= "MESSAGE";
                 break;
         case 3: $result= "APLICATION";
                 break;
         case 4: $result= "AUDIO";
                 break;
         case 5: $result= "IMAGE";
                 break;
         case 6: $result= "VIDEO";
                 break;
         default: $result= "OTHER";
      }
      return $result . "/" . $subtype;
    }

    function display_body( $header, $sec, $seccio, $mbx, $msg ){
       
       echo "<tr><td>$seccio</tr></td><td colspan=2>";
       if ( $header->ifsubtype ){
          echo display_type( $header->type, $header->subtype );
       } else {
          echo display_type( $header->type, "UNKNOWN" );
       }
       echo "</td></tr>";
       $secciona = $seccio=="" ? "0" : $seccio;
       echo "<tr><td></td><td Width=50></td><td><A HREF=\"/php3/dl/dlimapbd4.php3?msg=$msg&secciona=$secciona\">Veure Missatge</a></td></tr>";
       if ( $header->type == 1 ){
          $merda = $header->parts;
          $len = count( $merda );
          for ( $k=0 ; $k < $len ; $k++){
             $sec++; 
             display_body( $merda[$k], $sec, $seccio . $sec, $mbx, $msg );
          }
       }
       if ( $header->type == 2 ){
          $merda = $header->parts;
          display_body( $merda, 0, $seccio . ".1", $mbx, $msg );
       }
    }

    $mbox=imap_open("{rs120.readysoft.es}INBOX","prova","prova");
    if ( !$mbox ){
       echo "Error de connexio";
    } else {
       $nmes = imap_num_msg( $mbox );
       for ( $i=1 ; $i <= $nmes ; $i++){
          $sec = 0;
          $hea = imap_fetchstructure( $mbox, $i );
          echo "<b>Missatge $i</b><br>";
          echo "<b>----------</b><br><pre>";
          echo "<table border=1>";
          echo "<tr><td>". $sec . "</td><td colspan=2>([RFC-822] header of the message)</td></tr>";
          display_body( $hea, $sec, "", $mbox, $i );
          echo "</table>";
       }
       imap_close($mbox);
    }
?>

