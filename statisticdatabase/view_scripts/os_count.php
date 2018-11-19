<?php

require_once("connection.php");
require_once("make_table.php");

function os_count()
{
    global $link;
    $query = "SELECT * FROM os_count ORDER BY  `os_count`.`Count` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='os_count'>Статистика по операционным системам</h1>";
    echo "<h2 align='center'>Всего типов ОС: " . $number . "</h2><br>";
    
    makeTable($result);
}


?>