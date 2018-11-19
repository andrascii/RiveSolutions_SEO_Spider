<!DOCTYPE html>
<html>
<head>
    
    <link href="https://fonts.googleapis.com/css?family=Open+Sans" rel="stylesheet">
    <link rel='stylesheet' id='seospider-font-awesome-css'  href='https://rivesolutions.com/wp-content/themes/seospider/assets/css/fonts/css/fontawesome-all.min.css?ver=4.9.8' type='text/css' media='all' />
    <link rel='stylesheet' id='seospider-style-css'  href='https://rivesolutions.com/wp-content/themes/seospider/assets/css/main.css?ver=3' type='text/css' media='all' />
    
    <style>
    table
    {
        width: 100%;
    }
    table, td
    {
        border: 1px solid grey;
        border-collapse: collapse;
        padding: 5px;
    }
    .tableHeader
    {
        background-color: #FFA96E;
    }
    </style>
    
</head>
<body>

<a href="#last_user_session">Последняя сессия для каждого пользователя</a><br>
<a href="#last_30_days_unique_users">Количество уникальных пользователей за последние 30 дней</a><br>
<a href="#last_30_days_sessions">Количество сессий за последние 30 дней</a><br>
<a href="#os_count">Статистика по операционным системам</a><br>
<a href="#program_versions_total_usages">Статистика по использованию разных версий программы</a><br>
<a href="#last_30_days_program_versions_usages">Количество использований программы за последние 30 дней</a><br>
<a href="#counters_usages_total">Общая cтатистика по счетчикам</a><br>
<a href="#last_30_days_counters_usages">Статистика по счетчикам за последние 30 дней</a><br>
<a href="#countries_count">Статистика по странам</a><br>
<a href="#all_users">Все пользователи</a><br>

<?php

require("view_scripts/last_user_session.php");
require("view_scripts/last_30_days_unique_users.php");
require("view_scripts/os_count.php");
require("view_scripts/program_versions_total_usages.php");
require("view_scripts/last_30_days_sessions.php");
require("view_scripts/last_30_days_program_versions_usages.php");
require("view_scripts/counters_usages_total.php");
require("view_scripts/last_30_days_counters_usages.php");
require("view_scripts/countries_count.php");
require("view_scripts/all_users.php");

last_user_session();
last_30_days_unique_users();
last_30_days_sessions();
last_30_days_program_versions_usages();
last_30_days_counters_usages();
os_count();
program_versions_total_usages();
counters_usages_total();
countries_count();
all_users();

?>

</body>
</html>