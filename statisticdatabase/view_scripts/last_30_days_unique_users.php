<?php

require("connection.php");

function last_30_days_unique_users()
{
    global $link;
    $query = "SELECT * FROM last_30_days_unique_users ORDER BY  `last_30_days_unique_users`.`Last Sesson Date Time` DESC ";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_30_days_unique_users'>Количество уникальных пользователей за последние 30 дней</h1>";
    echo "<h2 align='center'>Всего уникальных пользователей: " . $number . "</h2><br>";
    
    echo "<table align='center'><tr class='tableHeader'>";
    echo "<td align='center'><h3>User ID</h3></td>";
    echo "<td align='center'><h3>User Name</h3></td>";
    echo "<td align='center'><h3>Session Count</h3></td>";
    echo "<td align='center'><h3>Last Sesson Date Time</h3></td>";
    
    while($resultArray = mysqli_fetch_assoc($result))
    {
        echo "<tr>";
        echo "<td>" . $resultArray['User ID'] . "</td>";
        echo "<td>" . $resultArray['User Name'] . "</td>";
        echo "<td>" . $resultArray['Sessions Count'] . "</td>";
        echo "<td>" . $resultArray['Last Sesson Date Time'] . "</td>";
        echo "</tr>";
    }
    
    echo "</table>";
}


?>