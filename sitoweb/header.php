<?php

echo '
<nav class="navbar navbar-expand-lg navbar-light bg-light">
    <a class="navbar-brand" href="index.php">
        <img src="imgs/logo.gif" width="30" height="30" class="d-inline-block align-top" alt="">
        Unicas - Air Quality Control
    </a>

    <button onClick="showNavMenu()" class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="#navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
        <span class="navbar-toggler-icon"></span>
    </button>
 
    <div class="collapse navbar-collapse" id="navbarSupportedContent">
        <ul class="navbar-nav ms-auto">
            <li class="nav-item active">
                <a class="nav-link" href="index.php">Home</a>
            </li>
            <li class="nav-item active">
                <a class="nav-link" href="history.php">Storico</a>
            </li>
        </ul>
    </div>
</nav>
<script type="text/javascript">

function showNavMenu() {
    var x = document.getElementById("navbarSupportedContent");
    if (x.style.display === "block") {
        x.style.display = "none";
    } else {
        x.style.display = "block";
    }
}


</script>';
;

?>