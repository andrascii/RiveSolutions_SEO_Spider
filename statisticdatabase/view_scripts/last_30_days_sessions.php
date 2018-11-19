<?php

require_once("connection.php");
require_once("make_table.php");

function last_30_days_sessions()
{
    global $link;
    $query = "SELECT * FROM last_30_days_sessions ORDER BY  `last_30_days_sessions`.`SessionDateTime` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_30_days_sessions'>Количество сессий за последние 30 дней</h1>";
    echo "<h2 align='center'>Всего: " . $number . "</h2><br>";
    echo "<div align='center'>Отображает всех сессии за 30 дней начиная с сегодняшнего дня</div><br>";
    
    makeTable($result);
}


?>