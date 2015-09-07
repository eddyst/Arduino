<?php
$Input = file_get_contents('php://input');
$data = json_decode($Input,true);
$Log = array_map(function() { return ""; }, range(0, 24));

//Preprozessor für sonderfälle die mehrere Informationen in einem Wert enthalten
foreach ($data AS $address => $value) {
	switch ($address){
	case ($address == "7660" || $address == "7663" || $address == "A38F"):
		$data["$address-A"] = substr($value,0,2);
		$data["$address-B"] = substr($value,2,2);
	}
}
//var_dump($data);
$host = "volkszaehler";
foreach ($data AS $address => $value) {
	//echo "\r\n" . $address . " " . $value . "\r\n";
	$DataOK = true;
	switch (strtoupper((string)$address)){
	case "0808": //Rücklauftemperatur (17A) RLTS (C° / 10)
		$channel = "ab1328f0-2a56-11e2-8382-0b613d372620";
		$Log[0] = temperature ($value, 10);
		$post_data = array(
			"value" => $Log[0]);
		break;
	case "0810": //Kesseltemperatur Tiefpass (C° / 10)
		$channel = "28b5a5e0-2a7b-11e2-b14f-d39b9c7bbe50";
		$Log[1] = temperature ($value, 10);
		$post_data = array(
			"value" => $Log[1]);
		break;
	case "0812": //Speichertemperatur Tiefpass STS1 (C° / 10)
		$channel = "473b0100-2a7b-11e2-979f-c9120d10d578";
		$Log[2] = temperature ($value, 10);
		$post_data = array(
			"value" => $Log[2]);
		break;
	case "0814": //Speichertemperatur 2 Tiefpass STS2 (C° / 10)
		$channel = "735d3660-2a7b-11e2-86a7-6198235b9a1a";
		$Log[3] = temperature ($value, 10);
		$post_data = array(
			"value" => $Log[3]);
		break;
	case "0886": //Betriebsstunden Stufe 1 (Sek)
		$channel = "";
		$Log[4] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[4]);
	    $DataOK = false;
		break;
	case "088A": //Brennerstarts (Anzahl)
		$channel = "";
		$Log[5] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[5]);
	    $DataOK = false;
		break;
	case "0A10": //Umschaltventil (0..3)
		$channel = "6377f2c0-2a81-11e2-9bc0-65c60c34fa5c";
		$Log[6] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[6]);
		break;
	case "0A82": //Sammelstörung (0..1)
		$channel = "";
		$Log[7] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[7]);
	    $DataOK = false;
		break;
	case "2302": //Sparbetrieb A1M1 (0..1)
		$channel = "";
		$Log[8] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[8]);
	    $DataOK = false;
		break;
	case "2303": //Partybetrieb A1M1 (0..1)
		$channel = "";
		$Log[9] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[9]);
	    $DataOK = false;
		break;
	case "2323": //Betriebsart A1M1
		$channel = "";
		$Log[10] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[10]);
	    $DataOK = false;
		break;
	case "2500": //aktuelle Betriebsart A1M1
		$channel = "";
		$Log[11] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[11]);
	    $DataOK = false;
		break;
	case "2544": //Vorlaufsolltemperatur A1M1
		$channel = "103ef090-2a7c-11e2-b5c5-159f3d66c38a";
		$Log[12] = temperature ($value, 10);
		$post_data = array(
			"value" => $Log[12]);
		break;
	case "5525": //Aussentemperatur Tiefpass
		$channel = "3b3ba4e0-2a7c-11e2-97c1-8bd9d000f43f";
		$Log[13] = temperature ($value, 10);
		$post_data = array(
			"value" => $Log[13]);
		break;
	case "5527": //Aussentemperatur gedämpft
		$channel = "6dee2a00-2a7c-11e2-b3ca-b1954ded405a";
		$Log[14] = temperature ($value, 10);
		$post_data = array(
			"value" => $Log[14]);
		break;
	case "555A": //Kesselsolltemperatur
		$channel = "8cbda3e0-2a7c-11e2-9aed-4b44c4bcf27f";
		$Log[15] = temperature ($value, 10);
		$post_data = array(
			"value" => $Log[15]);
		break;
	case "6513": //Speicherladepumpe (0..1)
		$channel = "";
		$Log[16] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[16]);
	    $DataOK = false;
		break;
	case "6515": //Zirkulationspumpe (0..1)
		$channel = "";
		$Log[17] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[17]);
	    $DataOK = false;
		break;
	case "7660-A": //Interne Pumpe (0..1)
		$channel = "A";
		$Log[18] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[18]);
	    $DataOK = false;
		break;
	case "7660-B": //Interne Pumpe (0..100)
		$channel = "B";
		$Log[19] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[19]);
	    $DataOK = false;
		break;
	case "7663-A": //Heizkreispumpe A1M1 (0..1)
		$channel = "";
		$Log[20] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[20]);
	    $DataOK = false;
		break;
	case "7663-B": //Heizkreispumpe A1M1 (0..100)
		$channel = "";
		$Log[21] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[21]);
	    $DataOK = false;
		break;
	case "A38F-A": //Anlagen Ist-Leistung (0..200)
		$channel = "";
		$Log[22] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[22]);
	    $DataOK = false;
		break;
	case "A38F-B": //Anlagen Ist-Leistung (0..1)
		$channel = "";
		$Log[23] = littleEndianHexToDec ($value);
		$post_data = array(
			"value" => $Log[23]);
	    $DataOK = false;
		break;
	case "A393": //Kesseltemperatur
		$channel = "efde88c0-2a7c-11e2-bfda-8d4cea401467";
		$Log[24] = temperature ($value, 100);
		$post_data = array(
			"value" => $Log[24]);
		break;
	default:
		$DataOK = false;
	}
	if ($DataOK == true) {
	   	// open a socket connection on port 80 - timeout: 30 sec
		$fp = fsockopen($host, 80, $errno, $errstr, 30);
		if ($fp){
			$path = "POST /middleware.php/data/$channel.json?".http_build_query($post_data)." HTTP/1.1\r\n". 
			        "Host: $host\r\n\r\n";
			echo $path;
			fputs($fp, $path);
			$result = ''; 
			while(!feof($fp)) {
				// receive the results of the request
				$result .= fgets($fp, 128);
			}
			echo "Result: $result";
		}
		else { 
			echo "fsocketopen: $errstr ($errno)";
		}
		fclose($fp);
	}
}

//var_dump($Log);
//echo implode ( ";", $Log);

$datei = fopen("/var/logvitodens/VitoR.log", "a");
	fwrite($datei, date("d.m.Y H:i:s") . ";" . implode( ";", $Log) . ";" . str_replace( array("\r\n", "\n", "\r"), "-", $Input) . "\r\n") ;
fclose($datei);




//$data["zp"] = time();
//	echo $GLOBALS["HTTP_RAW_POST_DATA"];
//   echo '<pre>'.print_r($_SERVER,true).'</pre>';
//   echo '<pre>'.print_r($_REQUEST,true).'</pre>';
//   print_r($_REQUEST);

//	var_dump(json_decode($_Post, true));
?>
<?php
function temperature ($value, $factor) {
	$t = littleEndianHexToDec ($value) 
	if ($t >30000)
		$t -= 65535;
	return $t / $factor
}

function littleEndianHexToDec ($value) {
$r = $value;
echo $r;
	for ( $i = 0; $i<strlen($value); $i+=2) {
		$r[$i  ]=$value[strlen($value)-$i-2];
		$r[$i+1]=$value[strlen($value)-$i-1];
	};
echo "-".$r."-" . hexdec($r) . "\r\n";
	return hexdec($r);
}
?>