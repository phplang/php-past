<?php

  $filename = "test.gz";
  $zp = gzopen($filename, "w9");
  $s = "<html>\n<head></head>\n<body><h1>Only a test!</h1></body>\n</html>\n";
  gzwrite($zp, $s);
  gzclose($zp);
  if (readgzfile($filename) != strlen($s)) {
	  echo "Error with zlib functions!";
  }
  unlink($filename);

?>
