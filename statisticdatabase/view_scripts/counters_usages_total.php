<?php

require("connection.php");

function counters_usages_total()
{
    global $link;
    $query = "SELECT * FROM counters_usages_total ORDER BY  `counters_usages_total`.`Uses Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='counters_usages_total'>Общая cтатистика по счетчикам</h1>";
    echo "<h2 align='center'>Всего использованных счетчиков: " . $number . "</h2><br>";
    
    echo "<table align='center'><tr class='tableHeader'>";
    echo "<td align='center'><h3>Counter Name</h3></td>";
    echo "<td align='center'><h3>Use Count</h3></td>";
    echo "<td align='center'><h3>User Count</h3></td>";
    
    while($resultArray = mysqli_fetch_assoc($result))
    {
        echo "<tr>";
        echo "<td>" . $resultArray['Counter Name'] . "</td>";
        echo "<td>" . $resultArray['Uses Count'] . "</td>";
        echo "<td>" . $resultArray['Users Count'] . "</td>";
        echo "</tr>";
    }
    
    echo "</table>";
}


?>