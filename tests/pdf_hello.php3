<?php
// This only works with pdflib 2.0

$pdffilename = "hello.pdf";
$fontname = "Helvetica-Bold";
$fontsize = 24.0;
$fontencoding = 4; /* winansi */
$a4_width = 595.0;
$a4_height = 842.0;

$fp = fopen($pdffilename, "w");
$pdf = pdf_open($fp);
pdf_set_info_creator($pdf, "pdf_hello.php3");
pdf_set_info_author($pdf, "Uwe Steinmann");
pdf_set_info_title($pdf, "Hello, world!");

pdf_begin_page($pdf, $a4_width, $a4_height);
pdf_set_font($pdf, $fontname, $fontsize, 4);
pdf_set_text_pos($pdf, 50, 700);
pdf_show($pdf, "Hello, world!");
pdf_end_page($pdf);
 
$pdf = pdf_close($pdf);
fclose($fp);
echo "<A HREF='$pdffilename'>pdf file</A>";
?>
