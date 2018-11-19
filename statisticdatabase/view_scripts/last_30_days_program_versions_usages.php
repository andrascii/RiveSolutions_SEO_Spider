<?php

require_once("connection.php");
require_once("make_table.php");

function last_30_days_program_versions_usages()
{
    global $link;
    $query = "SELECT * FROM last_30_days_program_versions_usages ORDER BY  `last_30_days_program_versions_usages`.`Users Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_30_days_program_versions_usages'>Количество использований программы за последние 30 дней</h1>";
    
    makeTable($result);
}


?>