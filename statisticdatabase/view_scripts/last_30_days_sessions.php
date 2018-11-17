<?php

require("connection.php");

function last_30_days_sessions()
{
    global $link;
    $query = "SELECT * FROM last_30_days_sessions ORDER BY  `last_30_days_sessions`.`SessionDateTime` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_30_days_sessions'>Количество сессий за последние 30 дней</h1>";
    echo "<h2 align='center'>Всего: " . $number . "</h2><br>";
    
    echo "<table align='center'><tr class='tableHeader'>";
    echo "<td align='center'><h3>ID</h3></td>";
    echo "<td align='center'><h3>SessionName</h3></td>";
    echo "<td align='center'><h3>SessionDateTime</h3></td>";
    
    while($resultArray = mysqli_fetch_assoc($result))
    {
        echo "<tr>";
        echo "<td>" . $resultArray['ID'] . "</td>";
        echo "<td>" . $resultArray['SessionName'] . "</td>";
        echo "<td>" . $resultArray['SessionDateTime'] . "</td>";
        echo "</tr>";
    }
    
    echo "</table>";
}


?>