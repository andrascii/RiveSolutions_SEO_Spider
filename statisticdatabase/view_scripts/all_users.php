<?php

require_once("connection.php");
require_once("make_table.php");

function all_users()
{
    global $link;
    $query = "SELECT * FROM all_users";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='all_users'>Все пользователи</h1>";
    echo "<h2 align='center'>Всего: " . $number . "</h2><br>";
    makeTable($result);
}


?>