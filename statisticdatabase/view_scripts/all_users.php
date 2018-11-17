<?php

require("connection.php");

function all_users()
{
    global $link;
    $query = "SELECT * FROM all_users";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $resultArray = mysqli_fetch_array($result);
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='all_users'>Все пользователи</h1>";
    echo "<h2 align='center'>Всего: " . $number . "</h2><br>";
    
    echo "<table align='center'><tr class='tableHeader'>";
    echo "<td align='center'><h3>User Name</h3></td>";
    echo "<td align='center'><h3>OS</h3></td>";
    echo "<td align='center'><h3>Program version</h3></td>";
    echo "<td align='center'><h3>Program bitness</h3></td>";
    echo "<td align='center'><h3>Country</h3></td>";
    echo "<td align='center'><h3>Locale language</h3></td>";
    
    while($resultArray = mysqli_fetch_assoc($result))
    {
        echo "<tr>";
        echo "<td>" . $resultArray['UserName'] . "</td>";
        echo "<td>" . $resultArray['OS'] . "</td>";
        echo "<td>" . $resultArray['Program version'] . "</td>";
        echo "<td>" . $resultArray['Program bittness'] . "</td>";
        echo "<td>" . $resultArray['Country'] . "</td>";
        echo "<td>" . $resultArray['Locale language'] . "</td>";
        echo "</tr>";
    }
    
    echo "</table>";
}


?>