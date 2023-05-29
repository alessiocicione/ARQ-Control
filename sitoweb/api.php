<?php

// Password necessaria lato client per accedere alle API
$api_key = "unicas_airqualitycontrol2022";

if (!isset($_POST) || $api_key != $_POST['api_key'])
    die("Invalid request");

require_once("database.php");

// Estrazione dei valori
$temperature = $_POST['temperature'];
$humidity = $_POST['humidity'];
$co2 = $_POST['co2'];
$brightness = $_POST['brightness'];
$air_quality = $_POST['air_quality'];


// Aggiornamento dei valori nel database
$db = new database();
$db->connect();
$db->insert_values($temperature, $humidity, $co2, $brightness, $air_quality);
$db->disconnect();

?>