<?

# this is a stupid check script
# which retrieves data from a HTTP server

# fetches one file and prints out how many bytes it has retrieve


$host = "funweb.de";
$port = 80;
$uri = "/int21.php3";
$req = "GET $uri HTTP/1.0\r\n\r\n";
$url = "http://$host:$port$uri";

function get_request() {
	global $host, $port, $allbuf, $req, $block;

	$allbuf = "";
	if($block > 2)
		$fp = pfsockopen($host, $port, &$a, &$b, 30);
	else
		$fp = fsockopen($host, $port, &$a, &$b, 30);
	if(!$fp) die("socket open failed ($a, $b)\n");
	set_socket_blocking($fp, $block % 2);

	fputs($fp, $req);
	return $fp;
}

echo "fopen/fpassthru\n";
$fd = fopen($url, "r");
fpassthru($fd);

echo "readfile\n";
readfile($url);

echo "require\n";
require($url);

echo "include\n";
include($url);

$a = file($url);
echo join("", $a);

for($block = 0; $block < 2; $block++) {

#
# this does not work in non-blocking mode
# since false == empty_string
# (expect a too low value, if the server is slower than loopback)

$fp = get_request();
sleep(1);
while(($buf = fgets($fp, 256)) != false) $allbuf .= $buf;
echo strlen($allbuf)."\n";

#
# this does not work in non-blocking mode
# since there is no way to signal a I-don't-have-any-data-available
# from fgetc
# (expect some additional data in form of NUL bytes)

$fp = get_request();
while(!feof($fp)) $allbuf .= fgetc($fp);
echo strlen($allbuf)."\n";

$fp = get_request();
while(!feof($fp)) { $allbuf .= fread($fp, 10000); }
echo strlen($allbuf)."\n";

# this is the preferred (and only working) way to read line-wise

$fp = get_request();
while(!feof($fp)) { $allbuf .= fgets($fp, 256);  }
echo strlen($allbuf)."\n";

}
