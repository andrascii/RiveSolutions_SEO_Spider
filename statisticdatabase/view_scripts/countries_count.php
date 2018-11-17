<?php

require("connection.php");

function countries_count()
{
    global $link;
    $query = "SELECT * FROM countries_count ORDER BY  `countries_count`.`Users Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='countries_count'>Статистика по странам</h1>";
    echo "<h2 align='center'>Всего стран: " . $number . "</h2><br>";
    
    echo "<table align='center'><tr class='tableHeader'>";
    echo "<td align='center'><h3>Country ID</h3></td>";
    echo "<td align='center'><h3>Country</h3></td>";
    echo "<td align='center'><h3>User Count</h3></td>";
    
    while($resultArray = mysqli_fetch_assoc($result))
    {
        echo "<tr>";
        echo "<td>" . $resultArray['Country ID'] . "</td>";
        echo "<td>" . $resultArray['Country'] . "</td>";
        echo "<td>" . $resultArray['Users Count'] . "</td>";
        echo "</tr>";
    }
    
    echo "</table>";
}


?>