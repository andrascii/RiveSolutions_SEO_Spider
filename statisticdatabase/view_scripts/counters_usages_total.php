<?php

require_once("connection.php");
require_once("make_table.php");

function counters_usages_total()
{
    global $link;
    $query = "SELECT * FROM counters_usages_total ORDER BY  `counters_usages_total`.`Uses Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='counters_usages_total'>Общая cтатистика по счетчикам</h1>";
    echo "<h2 align='center'>Всего использованных счетчиков: " . $number . "</h2><br>";
    
    makeTable($result);
}


?>