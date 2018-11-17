<?php

require("connection.php");

function os_count()
{
    global $link;
    $query = "SELECT * FROM os_count";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='os_count'>Статистика по операционным системам</h1>";
    echo "<h2 align='center'>Всего типов ОС: " . $number . "</h2><br>";
    
    echo "<table align='center'><tr class='tableHeader'>";
    echo "<td align='center'><h3>ID</h3></td>";
    echo "<td align='center'><h3>OS</h3></td>";
    echo "<td align='center'><h3>Bitness</h3></td>";
    echo "<td align='center'><h3>Count</h3></td>";
    
    while($resultArray = mysqli_fetch_assoc($result))
    {
        echo "<tr>";
        echo "<td>" . $resultArray['ID'] . "</td>";
        echo "<td>" . $resultArray['OS'] . "</td>";
        echo "<td>" . $resultArray['Bitness'] . "</td>";
        echo "<td>" . $resultArray['Count'] . "</td>";
        echo "</tr>";
    }
    
    echo "</table>";
}


?>