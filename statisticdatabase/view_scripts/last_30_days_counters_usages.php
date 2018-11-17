<?php

require("connection.php");

function last_30_days_counters_usages()
{
    global $link;
    $query = "SELECT * FROM last_30_days_counters_usages ORDER BY  `last_30_days_counters_usages`.`Uses Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_30_days_counters_usages'>Статистика по счетчикам за последние 30 дней</h1>";
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