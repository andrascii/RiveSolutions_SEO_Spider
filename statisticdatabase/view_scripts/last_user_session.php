<?php

require("connection.php");

function last_user_session()
{
    global $link;
    $query = "SELECT * FROM last_user_session ORDER BY `last_user_session`.`Sesson Date Time` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_user_session'>Статистика по последним сессиям</h1>";
    echo "<h2 align='center'>Всего сессий: " . $number . "</h2><br>";
    
    echo "<table align='center'><tr class='tableHeader'>";
    echo "<td align='center'><h3>User name</h3></td>";
    echo "<td align='center'><h3>User ID</h3></td>";
    echo "<td align='center'><h3>Session name</h3></td>";
    echo "<td align='center'><h3>Session ID</h3></td>";
    echo "<td align='center'><h3>Session date time</h3></td></tr>";
    
    while($resultArray = mysqli_fetch_assoc($result))
    {
        echo "<tr>";
        echo "<td>" . $resultArray['User Name'] . "</td>";
        echo "<td>" . $resultArray['User ID'] . "</td>";
        echo "<td>" . $resultArray['Sesson Name'] . "</td>";
        echo "<td>" . $resultArray['Sesson ID'] . "</td>";
        echo "<td>" . $resultArray['Sesson Date Time'] . "</td>";
        echo "</tr>";
    }
    
    echo "</table>";
}


?>