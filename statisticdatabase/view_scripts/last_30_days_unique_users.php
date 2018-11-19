<?php

require_once("connection.php");
require_once("make_table.php");

function last_30_days_unique_users()
{
    global $link;
    $query = "SELECT * FROM last_30_days_unique_users ORDER BY  `last_30_days_unique_users`.`Last Sesson Date Time` DESC ";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_30_days_unique_users'>Количество уникальных пользователей за последние 30 дней</h1>";
    echo "<h2 align='center'>Всего уникальных пользователей: " . $number . "</h2><br>";
    
    makeTable($result);
}


?>