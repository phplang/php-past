<title> Imap Example </title>
<?php
    dl("imap.so");


    function display_body( $header ){
       echo "         Type : " . $header->type . "<br>";
       echo "        Lines : " . $header->lines . "<br>";
       echo "        Bytes : " . $header->bytes . "<br>";
       echo "     Encoding : " . $header->encoding . "<br>";
       echo "   Te Subtype : " . $header->ifsubtype . "<br>";
       if ( $header->ifsubtype ){
          echo "      Subtype : " . $header->subtype . "<br>";
       }
       echo "Te Descripcio : " . $header->ifdescription . "<br>";
       if ( $header->ifdescription == 1 ){
          echo "   Descripcio : " . $header->description . "<br>";
       }
       echo "        Te ID : " . $header->ifid . "<br>";
       if ( $header->ifid == 1 ){
          echo "              ID : " . $header->id . "<br>";
       }
       echo "Te Parametres : " . $header->ifparametres . "<br>";
       if ( $header->ifparametres == 1 ){
          $merda = $header->parametres;
          $len = count( $merda );
          echo "Hi ha " . $len . " parametre(s) <br>";
          for ( $j=0 ; $j < $len ; $j++){
              echo "      Atribut : " . $merda[$j]->attribute . "<br>";
              echo "        Valor : " . $merda[$j]->value . "<br>";
          }
       }
       if ( $header->type == 1 ){
          $merda = $header->parts;
          $len = count( $merda );
          echo "Hi ha " . $len . " part(s) <br>";
          for ( $k=0 ; $k < $len ; $k++){
              echo "<b>Part $k</b></br>";
              echo "<b>----------</b></br><pre>";
              display_body( $merda[$k] );
          }
       }
       if ( $header->type == 2 ){
          $merda = $header->parts;
          echo "<b>Missatge encapsulat</b><br>";
          echo "<b>-------------------</b><br>";
          display_body( $merda );
       }
    }

    $mbox=imap_open("{rs120.readysoft.es}INBOX","prova","prova");
    if ( !$mbox ){
       echo "Error de connexio";
    } else {
       $nmes = imap_num_msg( $mbox );
       /*$nmes = 1;*/
       for ( $i=1 ; $i <= $nmes ; $i++){
          $hea = imap_fetchstructure( $mbox, $i );
          echo "<b>Missatge $i</b></br>";
          echo "<b>----------</b></br><pre>";
          display_body( $hea );
          echo "</pre>";
       }
       imap_close($mbox);
    }
?>

