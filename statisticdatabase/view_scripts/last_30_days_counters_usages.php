<?php

require_once("connection.php");
require_once("make_table.php");

function last_30_days_counters_usages()
{
    global $link;
    $query = "SELECT * FROM last_30_days_counters_usages ORDER BY  `last_30_days_counters_usages`.`Uses Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_30_days_counters_usages'>Статистика по счетчикам за последние 30 дней</h1>";
    echo "<h2 align='center'>Всего использованных счетчиков: " . $number . "</h2><br>";
    
    makeTable($result);
}


?>