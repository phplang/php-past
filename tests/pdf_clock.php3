<?php
// This only works with pdflib 2.0

$pdffilename = "clock.pdf";
$radius = 200;
$margin = 20;
$pagecount = 190;

$fp = fopen($pdffilename, "w");
$pdf = pdf_open($fp);
pdf_set_info_creator($pdf, "pdf_clock.php3");
pdf_set_info_author($pdf, "Uwe Steinmann");
pdf_set_info_title($pdf, "Analog Clock");

while($pagecount-- > 0) {
  pdf_begin_page($pdf, 2 * ($radius + $margin), 2 * ($radius + $margin));
  
  pdf_set_transition($pdf, 4);  /* wipe */
  pdf_set_duration($pdf, 0.5);
  
  pdf_translate($pdf, $radius + $margin, $radius + $margin);
  pdf_save($pdf);
  pdf_setrgbcolor($pdf, 0.0, 0.0, 1.0);
  
  /* minute strokes */
  pdf_setlinewidth($pdf, 2.0);
  for ($alpha = 0; $alpha < 360; $alpha += 6)
    {
    pdf_rotate($pdf, 6.0);
    pdf_moveto($pdf, $radius, 0.0);
    pdf_lineto($pdf, $radius-$margin/3, 0.0);
    pdf_stroke($pdf);
    }
  
  pdf_restore($pdf);
  pdf_save($pdf);
  
  /* 5 minute strokes */
  pdf_setlinewidth($pdf, 3.0);
  for ($alpha = 0; $alpha < 360; $alpha += 30)
  {
    pdf_rotate($pdf, 30.0);
    pdf_moveto($pdf, $radius, 0.0);
    pdf_lineto($pdf, $radius-$margin, 0.0);
    pdf_stroke($pdf);
  }
  
  $ltime = getdate();
  
  /* draw hour hand */
  pdf_save($pdf);
  pdf_rotate($pdf, -(($ltime['minutes']/60.0) + $ltime['hours'] - 3.0) * 30.0);
  pdf_moveto($pdf, -$radius/10, -$radius/20);
  pdf_lineto($pdf, $radius/2, 0.0);
  pdf_lineto($pdf, -$radius/10, $radius/20);
  pdf_closepath($pdf);
  pdf_fill($pdf);
  pdf_restore($pdf);
  
  /* draw minute hand */
  pdf_save($pdf);
  pdf_rotate($pdf, -(($ltime['seconds']/60.0) + $ltime['minutes'] - 15.0) * 6.0);
  pdf_moveto($pdf, -$radius/10, -$radius/20);
  pdf_lineto($pdf, $radius * 0.8, 0.0);
  pdf_lineto($pdf, -$radius/10, $radius/20);
  pdf_closepath($pdf);
  pdf_fill($pdf);
  pdf_restore($pdf);
  
  /* draw second hand */
  pdf_setrgbcolor($pdf, 1.0, 0.0, 0.0);
  pdf_setlinewidth($pdf, 2);
  pdf_save($pdf);
  pdf_rotate($pdf, -(($ltime['seconds'] - 15.0) * 6.0));
  pdf_moveto($pdf, -$radius/5, 0.0);
  pdf_lineto($pdf, $radius, 0.0);
  pdf_stroke($pdf);
  pdf_restore($pdf);
  
  /* draw little circle at center */
  pdf_circle($pdf, 0, 0, $radius/30);
  pdf_fill($pdf);
  
  pdf_restore($pdf);
  
  pdf_end_page($pdf);
}

$pdf = pdf_close($pdf);
fclose($fp);
echo "<A HREF='$pdffilename'>pdf file</A>";
?>
