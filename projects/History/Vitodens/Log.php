<?php
$Input = file_get_contents('php://input');
$datei = fopen("/var/log/VLog.log", "a");
	fwrite($datei, str_replace( array("\r\n", "\n", "\r"), "-", $Input)."\r\n");
fclose($datei);
?>