<?php

require_once("connection.php");
require_once("make_table.php");

function last_user_session()
{
    global $link;
    $query = "SELECT * FROM last_user_session ORDER BY `last_user_session`.`Sesson Date Time` DESC";
    
    $result = mysqli_query($link, $query) or die("Ошибка " . mysqli_error($link)); 
    
    if(!$result)
    {
        return;
    }
    
    $number = mysqli_num_rows($result);
    
    echo "<h1 align='center' id='last_user_session'>Последняя сессия для каждого пользователя</h1>";
    echo "<h2 align='center'>Всего пользователей: " . $number . "</h2><br>";
    echo "<div align='center'>Отображает дату последней сессии для каждого из существующих пользователей</div><br>";
    
    makeTable($result);
}


?>