<?php

$host = 'localhost';
$database = 'j775420_seospiderstatisticstest';
$user = '046767862_stats';
$password = '8462';

$link = mysqli_connect($host, $user, $password, $database) 
    or die("Ошибка " . mysqli_error($link));
    
?>