<?php

require("connection.php");

function program_versions_total_usages()
{
    global $link;
    $query = "SELECT * FROM program_versions_total_usages ORDER BY  `program_versions_total_usages`.`Users Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    
    echo "<h1 align='center' id='program_versions_total_usages'>Статистика по использованию разных версий программы</h1>";
    
    echo "<table align='center'><tr class='tableHeader'>";
    echo "<td align='center'><h3>Version</h3></td>";
    echo "<td align='center'><h3>Program Bitness</h3></td>";
    echo "<td align='center'><h3>Users Count</h3></td>";
    
    while($resultArray = mysqli_fetch_assoc($result))
    {
        echo "<tr>";
        echo "<td>" . $resultArray['Version'] . "</td>";
        echo "<td>" . $resultArray['Program Bittness'] . "</td>";
        echo "<td>" . $resultArray['Users Count'] . "</td>";
        echo "</tr>";
    }
    
    echo "</table>";
}


?>