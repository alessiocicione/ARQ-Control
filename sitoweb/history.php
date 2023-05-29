<!doctype html>
<html lang="it">

<head>

    <title>UNICAS - Air Quality Control</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <link rel="stylesheet" href="css/bootstrap.css">

</head>

<body>

    <?php 
        include("header.php");
       
    ?>

    <div class="container-md">
        <h3>Lista dei dati raccolti</h3>
        <h6>Cerca un determinato giorno:</h6>
        <form action="chart.php" method="GET">
            <input type="text" name="date" placeholder="AAAA-MM-GG"/>
            <input type="submit" value="Cerca" />
        </form>
        <br /> <br />

        <?php

            include("database.php");

            $db = new database();
            $db->connect();
            $values = $db->get_history_table();

            echo "| ";
            foreach ($values as $row) 
                echo "<a href='chart.php?date=$row[0]'>$row[0]</a> | ";

            $db->disconnect();

        ?>
    </div>


    <script src="js/jquery-3.6.0.min.js"></script>
    <script src="js/bootstrap.min.js"></script>
</body>

</html>

