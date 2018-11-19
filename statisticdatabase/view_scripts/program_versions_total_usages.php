<?php

require_once("connection.php");
require_once("make_table.php");

function program_versions_total_usages()
{
    global $link;
    $query = "SELECT * FROM program_versions_total_usages ORDER BY  `program_versions_total_usages`.`Users Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    echo "<h1 align='center' id='program_versions_total_usages'>Статистика по использованию разных версий программы</h1>";
    
    makeTable($result);
}


?>