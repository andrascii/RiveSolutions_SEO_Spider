<?php

require_once("connection.php");
require_once("make_table.php");

function countries_count()
{
    global $link;
    $query = "SELECT * FROM countries_count ORDER BY `countries_count`.`Users Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='countries_count'>Статистика по странам</h1>";
    echo "<h2 align='center'>Всего стран: " . $number . "</h2><br>";
    
    makeTable($result);
}


?>