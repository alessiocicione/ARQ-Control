<?php 
 

if (!isset($_GET['date']))
    exit();

require_once("database.php"); 
$db = new database();
$db->connect();
$values = $db->get_all_values_by_date($_GET['date']); 
$db->disconnect();

if ($values[0][0] == null)
    exit();

$delimiter = ",";
$filename = "airqualitycontrol_report_" . $_GET['date'] . ".csv";

$f = fopen('php://memory', 'w');


$fields = array('Data', 'Temperatura', 'Umidita', 'CO2', 'Luminosita', 'Qualita');
fputcsv($f, $fields, $delimiter);


foreach ($values as $value){
    $lineData = array($value[0], $value[1], $value[2], $value[3], $value[4], $value[5]);
    fputcsv($f, $lineData, $delimiter);
}


fseek($f, 0);


header('Content-Type: text/csv');
header('Content-Disposition: attachment; filename="' . $filename . '";');


fpassthru($f); 

exit; 
 
?>

