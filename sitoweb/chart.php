<!doctype html>
<html lang="it">

<head>

    <title>UNICAS - Air Quality Control</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <link rel="stylesheet" href="css/bootstrap.css">

</head>

<body>
    <?php include("header.php"); ?>
    <script src="js/chart.min.js"></script>
    <div class="container-md">
    <?php 
        
        $date = null;
        $values = null;
        if (isset($_GET['date'])){
            $date = $_GET['date'];
            echo "<h3>Grafico per il giorno $date</h3>";
            require_once('database.php');
            $db = new database();
            $db->connect();
            $values = $db->get_chart_table($date);
            if ($values[0][0]== null){
                echo '<h3>Nessun grafico disponibile per questa data</h3>
                  <script src="js/jquery-3.6.0.min.js"></script>  
                   <script src="js/bootstrap.min.js"></script>
                   </body>
        
                   </html>';
                exit();
                $db->disconnect();
            }
            $db->disconnect();
        }
        else {
            echo '<h3>Nessun grafico disponibile per questa data</h3>
                  <script src="js/jquery-3.6.0.min.js"></script>  
                   <script src="js/bootstrap.min.js"></script>
                   </body>
        
                   </html>';
            exit();
        }

    
    ?>

    
        <canvas id="dataChart" width="200" height="200"></canvas>
        <br /> <br />
        <?php echo '<h4>Esporta i dati della giornata in csv: </h4>
                  <a href="export_csv.php?date='.$date.'">Download</a>'; ?>
        <br /> <br /> <br />

    </div>
    <?php echo "
    <script>
        const ctx = document.getElementById('dataChart');
        const dataChart = new Chart(ctx, {
            type: 'bar',
            data: {
                labels: ['Temperatura', 'Umidità', 'CO2', 'Luminosità', 'Qualità'],
                datasets: [{
                    label: '# Valori medi nella giornata',
                    data: [";

                            
                            foreach($values as $value)
                                echo $value[0].','.
                                    $value[1].','.
                                    $value[2].','.
                                    $value[3].','.
                                    $value[4];
                           
                        
                    
                    echo "],
                    backgroundColor: [
                        'rgba(255, 99, 132, 0.2)',
                        'rgba(54, 162, 235, 0.2)',
                        'rgba(255, 206, 86, 0.2)',
                        'rgba(75, 192, 192, 0.2)',
                        'rgba(153, 102, 255, 0.2)'
                    ],
                    borderColor: [
                        'rgba(255, 99, 132, 1)',
                        'rgba(54, 162, 235, 1)',
                        'rgba(255, 206, 86, 1)',
                        'rgba(75, 192, 192, 1)',
                        'rgba(153, 102, 255, 1)'
                    ],
                    borderWidth: 1
                }]
            },
            options: {
                scales: {
                    y: {
                        beginAtZero: true
                    }
                }
            }
        });
    </script>"; ?>

    <script src="js/jquery-3.6.0.min.js"></script>  
    <script src="js/bootstrap.min.js"></script>
</body>

</html>