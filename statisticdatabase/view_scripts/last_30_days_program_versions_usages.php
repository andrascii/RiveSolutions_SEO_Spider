<?php

require("connection.php");

function last_30_days_program_versions_usages()
{
    global $link;
    $query = "SELECT * FROM last_30_days_program_versions_usages ORDER BY  `last_30_days_program_versions_usages`.`Users Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_30_days_program_versions_usages'>Количество использований программы за последние 30 дней</h1>";
    
    echo "<table align='center'><tr class='tableHeader'>";
    echo "<td align='center'><h3>Version</h3></td>";
    echo "<td align='center'><h3>Program Bitness</h3></td>";
    echo "<td align='center'><h3>User Count</h3></td>";
    
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