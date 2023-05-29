<?php


function print_html($rows){

    $table = "";
    
    foreach ($rows as $row) {
        $table .= '<tr><th scope="row">'.$row[0].'</th>'
        .'<td>'.$row[1].'</td>'
        .'<td>'.$row[2].'</td>'
        .'<td>'.$row[3].'</td>'
        .'<td>'.$row[4].'</td>'
        .'<td>'.$row[5].'</td></tr>';
    }

    return $table;

}



require_once("database.php");

$db = new database();
$db->connect();
echo '
    <div class="table-responsive-xl">
        <table class="table">
            <thead>
                <tr>
                    <th scope="col">Data</th>
                    <th scope="col">Temperatura</th>
                    <th scope="col">Umidità</th>
                    <th scope="col">CO2</th>
                    <th scope="col">Luminosità</th>
                    <th scope="col">Qualità</th>
                </tr>
            </thead>
            <tbody>
                ' . print_html($db->get_table_rows()) . '
            </tbody>
        </table>
    </div>';

$db->disconnect();

?>
