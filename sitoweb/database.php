<?php

class database{

private const HOST = "localhost";
private const USER = "airqualitycontrolz";
private const PASS = "";
private const DB = "my_airqualitycontrol";

private $conn;

public function connect(){

    $this->conn = new mysqli(database::HOST, database::USER, database::PASS, database::DB);

    // Check della connessione al db
    if ($this->conn->connect_error) 
        return;
    

}

public function get_table_rows($date = null){

    if (!isset($date))
        $date = date("Y-m-d");
        
    $sql = "SELECT date,temperature,humidity,co2,brightness,airquality
                FROM air_measures
                WHERE DATE(date) = '$date'
                ORDER BY date DESC; ";

    $result = $this->conn->query($sql);

    $rows = [];
    while($row = $result->fetch_row())
        $rows[] = $row;
    

    return $rows;

}

public function get_history_table($date = null){
    
    $sql = "SELECT DATE(date) FROM air_measures GROUP BY DATE(date); ";
    $result = $this->conn->query($sql);

    $rows= [];
    while($row = $result->fetch_row())
        $rows[] = $row;

    return $rows;

}

public function get_chart_table($date){
    $sql = "SELECT AVG(temperature), AVG(humidity), AVG(co2), AVG(brightness), AVG(airquality)
            FROM air_measures
            WHERE DATE(date) = DATE('$date') ;";
    $result = $this->conn->query($sql);

    $rows= [];
    while($row = $result->fetch_row())
        $rows[] = $row;

    return $rows;
}

public function get_all_values_by_date($date){
    $sql = "SELECT date, temperature, humidity, co2, brightness, airquality
            FROM air_measures
            WHERE DATE(date) = DATE('$date');";
    $result = $this->conn->query($sql);
    
    $rows= [];
    while($row = $result->fetch_row())
        $rows[] = $row;

    return $rows;
}

public function insert_values($temperature, $humidity, $co2, $brightness, $air_quality){

    $sql = "INSERT INTO air_measures VALUES (NOW(), '$temperature', '$humidity', '$co2', '$brightness', '$air_quality') ;";
    if(!$this->conn->query($sql)){
        $this->disconnect();
        die("Error");
    }
    
}

public function disconnect(){

    $this->conn->close();

}



}; 



?>