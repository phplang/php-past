<?php

$lastivmd5 = "";

function test_mcrypt($func, $cipher, $needsiv = true) {
	global $key, $data, $lastivmd5;
	
	echo "testing cipher $cipher: ".mcrypt_get_cipher_name($cipher)." + $func\n"; 
	$size = mcrypt_get_block_size($cipher);
	echo "  | blocksize is $size\n";
	if($needsiv) {
		$iv = mcrypt_create_iv($size, MCRYPT_DEV_URANDOM);
		$ivmd5 = md5($iv);
		if($lastivmd5 == $ivmd5) {
			echo "VERY INSECURE initial vector!\n";
		}
		$lastivmd5 = $ivmd5;
		$res = $func($cipher, $key, $data, MCRYPT_ENCRYPT, $iv);
	} else {
		$res = $func($cipher, $key, $data, MCRYPT_ENCRYPT);
	}
	
	if($needsiv) {
		$back = $func($cipher, $key, $res, MCRYPT_DECRYPT, $iv);
	} else {
		$back = $func($cipher, $key, $res, MCRYPT_DECRYPT);
	}
	
	echo sprintf("  | key is %d bytes long, data %d, res %d, back %d\n",
			strlen($key), strlen($data), strlen($res), strlen($back));

	if($back != $data) {
		echo "FAILED. data ain't back\n";
	} else echo "  | passed\n";
}

error_reporting(15);
$key = "this is a top secret key";
$datax = "1234567890abcdef";
$data = "";
for($i = 0; $i < 12; $i++) $data .= $datax;

for($i = 0; $i < 60; $i++) {
	if(mcrypt_get_cipher_name($i) != false) {
		test_mcrypt("mcrypt_ofb", $i);
		test_mcrypt("mcrypt_cfb", $i);
		test_mcrypt("mcrypt_cbc", $i);
		test_mcrypt("mcrypt_cbc", $i, false);
		test_mcrypt("mcrypt_ecb", $i, false);
	}
}
