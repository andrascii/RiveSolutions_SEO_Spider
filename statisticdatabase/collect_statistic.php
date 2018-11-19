<?php

$statisticsDir = dirname(__FILE__);
$statisticsFiles = scandir($statisticsDir);

echo $statisticsDir;

$host = 'localhost';
$database = 'j775420_seospiderstatisticstest';
$user = '046767862_stats';
$password = '8462'; 

$link = mysqli_connect($host, $user, $password, $database) 
    or die("Ошибка " . mysqli_error($link));
    
function getCounterIdSql($counterName)
{
    $insertOsSql = "SELECT `ID` FROM `counters` WHERE `CounterName` = \"$counterName\"";
    
    global $link;
    
    $result = mysqli_query($link, $insertOsSql) or die("Ошибка " . mysqli_error($link)); 
    
    if(isset($result))
    {
        $resultArray = mysqli_fetch_assoc($result);
        return $resultArray['ID'];
    }
}
    
function getSessionIdSql($sessionName)
{
    $insertOsSql = "SELECT `ID` FROM `sessions` WHERE `SessionName` = \"$sessionName\"";
    
    global $link;
    
    $result = mysqli_query($link, $insertOsSql) or die("Ошибка " . mysqli_error($link)); 
    
    if(isset($result))
    {
        $resultArray = mysqli_fetch_assoc($result);
        return $resultArray['ID'];
    }
}

function getUserIdSql($userName)
{
    $insertOsSql = "SELECT `ID` FROM `users` WHERE `UserName` = \"$userName\"";
    
    global $link;
    
    $result = mysqli_query($link, $insertOsSql) or die("Ошибка " . mysqli_error($link)); 
    
    if(isset($result))
    {
        $resultArray = mysqli_fetch_assoc($result);
        return $resultArray['ID'];
    }
}

function getOsIdSql($os, $bittness)
{
    $insertOsSql = "SELECT `ID` FROM `os` WHERE `os` = \"$os\" AND `bittness` = \"$bittness\"";
    
    global $link;
    
    $result = mysqli_query($link, $insertOsSql) or die("Ошибка " . mysqli_error($link)); 
    
    if(isset($result))
    {
        $resultArray = mysqli_fetch_assoc($result);
        return $resultArray['ID'];
    }
}

function getVersionIdSql($programVersion, $programBittness)
{
    $insertOsSql = "SELECT `ID` FROM `program_versions` WHERE `version` = \"$programVersion\" AND `programBittness` = \"$programBittness\"";
    
    global $link;
    
    $result = mysqli_query($link, $insertOsSql) or die("Ошибка " . mysqli_error($link)); 
    
    if(isset($result))
    {
        $resultArray = mysqli_fetch_assoc($result);
        return $resultArray['ID'];
    }
}

function getCountryIdSql($country)
{
    $getCountryIdSql = "SELECT `ID` FROM `countries` WHERE `country` = \"$country\"";
    
    global $link;
    
    $result = mysqli_query($link, $getCountryIdSql) or die("Ошибка " . mysqli_error($link)); 
    
    if(isset($result))
    {
        $resultArray = mysqli_fetch_assoc($result);
        return $resultArray['ID'];
    }
}

function getLanguageIdSql($langauge)
{
    $getLanguageIdSql = "SELECT `ID` FROM `locale_languages` WHERE `localeLanguage` = \"$langauge\"";
    
    global $link;
    
    $result = mysqli_query($link, $getLanguageIdSql) or die("Ошибка " . mysqli_error($link)); 
    
    if(isset($result))
    {
        $resultArray = mysqli_fetch_assoc($result);
        return $resultArray['ID'];
    }
}

function getSessionByUserIdSql($sessionId)
{
    $getSessionByUserIdSql = "SELECT `ID` FROM `session_by_user` WHERE `SessionID` = \"$sessionId\"";
    
    global $link;
    
    $result = mysqli_query($link, $getSessionByUserIdSql) or die("Ошибка " . mysqli_error($link)); 
    
    if(isset($result))
    {
        $resultArray = mysqli_fetch_assoc($result);
        return $resultArray['ID'];
    }
}

function insertCounterDataBySessionData($sessionByUserId, $counterId, $value)
{
    $insertSessionSql = "INSERT IGNORE INTO `counters_data_by_session_data` (`SessionByUserID`, `CounterID`, `Value`) 
        VALUES (\"$sessionByUserId\", \"$counterId\", \"$value\")";
    
    global $link;
    
    mysqli_query($link, $insertSessionSql) or die("Ошибка " . mysqli_error($link)); 
}

function insertCounterSql($counterName)
{
    $insertSessionSql = "INSERT IGNORE INTO `counters`(`CounterName`) VALUES (\"$counterName\")";
    
    global $link;
    
    mysqli_query($link, $insertSessionSql) or die("Ошибка " . mysqli_error($link)); 
}

function insertSessionByUserSql($sessionID, $userId)
{
    $insertSessionSql = "INSERT IGNORE INTO `session_by_user`(`SessionID`, `UserID`) VALUES (\"$sessionID\", \"$userId\")";
    
    global $link;
    
    $result = mysqli_query($link, $insertSessionSql) or die("Ошибка " . mysqli_error($link)); 
}

function insertSessionSql($sessionName, $sessionDateTime)
{
    $insertSessionSql = "INSERT IGNORE INTO `sessions`(`sessionName`, `sessionDateTime`) VALUES (\"$sessionName\", \"$sessionDateTime\")";
    
    global $link;
    
    $result = mysqli_query($link, $insertSessionSql) or die("Ошибка " . mysqli_error($link)); 
}

function insertOsSql($os, $bittness)
{
    $insertOsSql = "INSERT IGNORE INTO `os`(`os`, `bittness`) VALUES (\"$os\", \"$bittness\")";
    
    global $link;
    
    $result = mysqli_query($link, $insertOsSql) or die("Ошибка " . mysqli_error($link)); 
}

function insertProgramVersionSql($programVersion, $programBittness)
{
    $insertProgramVersionSql = "INSERT IGNORE INTO `program_versions` (`version`, `programBittness`) VALUES (\"$programVersion\", \"$programBittness\")";
    
    global $link;
    
    $result = mysqli_query($link, $insertProgramVersionSql) or die("Ошибка " . mysqli_error($link)); 
}

function insertCountrySql($country)
{
    $insertProgramVersionSql = "INSERT IGNORE INTO `countries` (`country`) VALUES (\"$country\")";
    
    global $link;
    
    $result = mysqli_query($link, $insertProgramVersionSql) or die("Ошибка " . mysqli_error($link)); 
}

function insertLanguageSql($language)
{
    $insertProgramVersionSql = "INSERT IGNORE INTO `locale_languages` (`localeLanguage`) VALUES (\"$language\")";
    
    global $link;
    
    $result = mysqli_query($link, $insertProgramVersionSql) or die("Ошибка " . mysqli_error($link)); 
}

function insertUserSql($userName, $OSID, $versionID, $countryID, $languageID)
{
    $userID = getUserIdSql($userName);
    
    global $link;
    
    if(isset($userID))
    {
        $updateUserInfoSql = "UPDATE `users` SET `OSID`=\"$OSID\", `VersionID`=\"$versionID\", `CountryID`=\"$countryID\", `LanguageID`=\"$languageID\" WHERE `ID` = \"$userID\"";
        
        $result = mysqli_query($link, $updateUserInfoSql) or die("Ошибка " . mysqli_error($link)); 
        
        return;
    } 
    
    $insertProgramVersionSql = "INSERT IGNORE INTO `users` (`UserName`, `OSID`, `VersionID`, `CountryID`, `LanguageID`) VALUES (\"$userName\", \"$OSID\", \"$versionID\", \"$countryID\", \"$languageID\")";
    
    $result = mysqli_query($link, $insertProgramVersionSql) or die("Ошибка " . mysqli_error($link)); 
}

foreach ($statisticsFiles as $statisticsFile)
{
    $statisticsFilePath = $statisticsDir . "/" . $statisticsFile;
    
    if(strpos($statisticsFile, 'statistics_') !== false && strpos($statisticsFile, '_debug') === false)
    {
        $sessionName = str_replace(".json", "", str_replace("statistics_", "", $statisticsFile));
        $statisticsFileContent = file_get_contents($statisticsFilePath);
        $statisticsFileJson = json_decode($statisticsFileContent, true);
        $statisticsHeader = $statisticsFileJson['Header'];
        $statisticsCounter = $statisticsFileJson['Counters'];
        
        if(!isset($statisticsHeader["country"]) ||
            !isset($statisticsHeader["language"]) ||
            !isset($statisticsHeader["os"]) ||
            !isset($statisticsHeader["physicalMemory"]) ||
            !isset($statisticsHeader["programBittness"]) ||
            !isset($statisticsHeader["programVersion"]) ||
            !isset($statisticsHeader["sessionDateTime"]) ||
            !isset($statisticsHeader["userID"]))
        {
            rename($statisticsFilePath, $statisticsDir . "/broken/" . $statisticsFile);
            continue;
        }
        
        insertSessionSql($sessionName, $statisticsHeader['sessionDateTime']);
        insertOsSql($statisticsHeader['os'], $statisticsHeader['programBittness']);
        insertProgramVersionSql($statisticsHeader['programVersion'], $statisticsHeader['programBittness']);
        insertCountrySql($statisticsHeader['country']);
        insertLanguageSql($statisticsHeader['language']);
        
        insertUserSql($statisticsHeader['userID'], 
            getOsIdSql($statisticsHeader['os'], $statisticsHeader['programBittness']),    
            getVersionIdSql($statisticsHeader['programVersion'], $statisticsHeader['programBittness']),
            getCountryIdSql($statisticsHeader['country']),     
            getLanguageIdSql($statisticsHeader['language']));
        
        insertSessionByUserSql(getSessionIdSql($sessionName), getUserIdSql($statisticsHeader['userID']));
        
        foreach($statisticsCounter as $key=>$value)
        {
            insertCounterSql($key);
            
            if(isset($value))
            {
                insertCounterDataBySessionData(getSessionByUserIdSql(getSessionIdSql($sessionName)), getCounterIdSql($key), $value);
            }
        }
        
        rename($statisticsFilePath, $statisticsDir . "/backup/" . $statisticsFile);
    }
    
    if(strpos($statisticsFile, 'statistics_') !== false && strpos($statisticsFile, '_debug') !== false)
    {
        unlink($statisticsFilePath);
    }
}

mysqli_close($link);
?>
