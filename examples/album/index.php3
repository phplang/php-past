<?
    /* 
     * The thumbnail function uses the pnmscale program from
     * the popular Netpbm image manipulation tools package
     * to create a thumbnail of the given image.  If a thumbnail
     * already exists for the image, the function simply returns.
     */
    cfunction thumbnail($filename) {
        global $theight;
        /* Define where to find the various external binaries we need            */
        $djpeg = "/usr/local/bin/djpeg"; /* decompresses a jpeg to ppm format    */ 
        $cjpeg = "/usr/local/bin/cjpeg"; /* compreses a ppm to jpeg format       */
        $pnmscale = "/usr/local/netpbm/pnmscale"; /* scales a ppm image          */
        $giftopnm = "/usr/local/netpbm/giftopnm"; /* convert a gif to ppm format */
        $ppmtogif = "/usr/local/netpbm/ppmtogif"; /* convert a ppm to gif format */
        $ppmquant = "/usr/local/netpbm/ppmquant"; /* colour quantize a ppm image */

        $tdir = dirname($filename)."/thumbnails"; /* thumbnail directory         */
        if(!filetype($tdir)) {
            if(!@mkdir($tdir,0777)) {
                echo "Unable to create $tdir directory - check permissions<br>\n";
                return;
            }
        }
        $tfile = $tdir."/".basename($filename);   /* thumbnail file              */
        if(!filesize($tfile)) {
            if(ereg("\.gif$",$filename)) {  /* Look for .gif extension */
                exec("$giftopnm $filename | $pnmscale -height $theight | $ppmquant 256 | $ppmtogif -interlace > $tfile");
            } elseif(ereg("\.jpe?g",$filename)) { /* Look for .jpg or .jpeg extension */
                exec("$djpeg $filename | $pnmscale -height $theight |".
                     " $cjpeg -outfile $tfile");
            } else {  /* not a GIF or JPG file */
                return("");
            }
        }
        return($tfile);
    }

    /* If the album variable isn't set, show the album list */
    if(!$album) {
        $title = "Photo Albums";
        $page = "albums";
        include "include/header.inc";

        /* Get a list of the available albums */
        $albums = mysql_query("select filename,album,count(*) as num_albums from photos group by album");
        echo "<table align=center>\n";

        /* Loop through each album */
        while($row=mysql_fetch_row($albums)) {
            $row[1] = stripslashes($row[1]);
            $thumb = thumbnail($row[0]);
            if(strlen($thumb)) {
                echo "<tr><td><a href=\"$PHP_SELF?album=".urlencode($row[1]);
                $size = getimagesize($thumb);
                echo "\"><img src=\"$thumb\" border=0 $size[3]></a></td>";
                echo " <td><font size=+1><b>".$row[1]."</b></font><br>";
                echo "$row[2] photos<br></td></tr>\n";
            }
        }
        echo "</table>\n";
        mysql_free_result($albums);
    } 
    /* Ok, an album was selected, show the thumbnails for this album */
    else {
        $title = stripslashes($album);
        $page = "thumbnails";
        include "include/header.inc";
        $pics = mysql_query("select filename,text from photos where album='$album'");
        $album=stripslashes($album);
        echo "<center>\n";
        while($row=mysql_fetch_row($pics)) {
            if($photo==$row[0]) {
                $desc=stripslashes($row[1]);
            }
            $thumb = thumbnail($row[0]);
            if(strlen($thumb)) {
                echo "<a href=\"$PHP_SELF?album=".urlencode($album);
                $size = getimagesize($thumb);
                echo "&photo=$row[0]\"><img src=$thumb border=0 $size[3]></a>\n";
            }
            flush(); /* Force web server to flush buffer */
        }	
        echo "</center>\n";

        /* If a picture was selected as well, show the picture */
        if($photo) {
            $size=getimagesize($photo);
            echo "<center><img src=\"$photo\" $size[3]><br>$desc</center>\n";
        }
        mysql_free_result($pics);
    }

    include "include/footer.inc";
?>
