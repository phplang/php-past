<?

# this is a stupid check script
# which retrieves data from a HTTP server

# fetches one file and prints out how many bytes it has retrieve

$host = "guerilla";
$port = 8080;
$req = "GET /abc2 HTTP/1.0\r\n\r\n";

function get_request() {
	global $host, $port, $allbuf, $req, $block;

	$allbuf = "";
	$fp = fsockopen($host, $port, &$a, &$b, 5);
	if(!$fp) die("fsck\n");
	set_socket_blocking($fp, $block);

	fputs($fp, $req);
	return $fp;
}

for($block = 0; $block < 2; $block++) {

#
# this does not work in non-blocking mode
# since false == empty_string
# (expect a too low value, if the server is slower than loopback)

$fp = get_request();
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
