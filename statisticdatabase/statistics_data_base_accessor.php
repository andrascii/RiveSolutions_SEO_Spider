<?php

class StatisticsDataBaseAccessor
{
    function StatisticsDataBaseAccessor($host, $dataBaseName, $user, $password)
    {
        $m_host = $host;
        $m_dataBaseName = $dataBaseName;
        $m_user = $user;
        $m_password = $password;
        
        $m_connection = mysqli_connect($m_host, $m_user, $m_password, $m_dataBaseName)
            or die("Error: " . mysqli_error($m_connection));
            
        mysqli_set_charset($m_connection, 'utf8') 
            or die("Error: " . mysqli_error($m_connection));
    }
    
    public function getCounterId($counterName)
	{
		$query = "SELECT `ID` FROM `counters` WHERE `CounterName` = \"$counterName\"";

		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 

		if($result)
		{
			$resultArray = mysqli_fetch_assoc($result);
			return $resultArray['ID'];
		}
	}
	
    public function getSessionId($sessionName)
	{
		$query = "SELECT `ID` FROM `sessions` WHERE `SessionName` = \"$sessionName\"";
    
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
		
		if($result)
		{
			$resultArray = mysqli_fetch_assoc($result);
			return $resultArray['ID'];
		}
	}
	
    public function getUserId($userName)
	{
		$query = "SELECT `ID` FROM `users` WHERE `UserName` = \"$userName\"";
    
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
		
		if($result)
		{
			$resultArray = mysqli_fetch_assoc($result);
			return $resultArray['ID'];
		}
	}
	
    public function getOsId($os, $bittness)
	{
		$query = "SELECT `ID` FROM `os` WHERE `os` = \"$os\" AND `bittness` = \"$bittness\"";
    
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
		
		if($result)
		{
			$resultArray = mysqli_fetch_assoc($result);
			return $resultArray['ID'];
		}
	}
	
    public function getVersionId($programVersion, $programBittness)
	{
		$query = "SELECT `ID` FROM `program_versions` WHERE `version` = \"$programVersion\" AND `programBittness` = \"$programBittness\"";

		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
		
		if($result)
		{
			$resultArray = mysqli_fetch_assoc($result);
			return $resultArray['ID'];
		}
	}
	
    public function getCountryId($country)
	{
		$query = "SELECT `ID` FROM `countries` WHERE `country` = \"$country\"";
    
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
		
		if($result)
		{
			$resultArray = mysqli_fetch_assoc($result);
			return $resultArray['ID'];
		}
	}
	
    public function getLanguageId($langauge)
	{
		$query = "SELECT `ID` FROM `locale_languages` WHERE `localeLanguage` = \"$langauge\"";
    
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
		
		if($result)
		{
			$resultArray = mysqli_fetch_assoc($result);
			return $resultArray['ID'];
		}
	}
	
    public function getSessionByUserId($sessionId)
	{
		$query = "SELECT `ID` FROM `session_by_user` WHERE `SessionID` = \"$sessionId\"";

		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
		
		if($result)
		{
			$resultArray = mysqli_fetch_assoc($result);
			return $resultArray['ID'];
		}
	}
    
    public function insertCounterDataBySessionData($sessionByUserId, $counterId, $value)
	{
		$query = "INSERT IGNORE INTO `counters_data_by_session_data` (`SessionByUserID`, `CounterID`, `Value`) 
			VALUES (\"$sessionByUserId\", \"$counterId\", \"$value\")";

		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
	}
	
    public function insertCounter($counterName)
	{
		$query = "INSERT IGNORE INTO `counters`(`CounterName`) VALUES (\"$counterName\")";

		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection));
	}
	
    public function insertSessionByUser($sessionID, $userId)
	{
		$query = "INSERT IGNORE INTO `session_by_user`(`SessionID`, `UserID`) VALUES (\"$sessionID\", \"$userId\")";
		
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
	}
	
    public function insertSession($sessionName, $sessionDateTime)
	{
		$query = "INSERT IGNORE INTO `sessions`(`sessionName`, `sessionDateTime`) VALUES (\"$sessionName\", \"$sessionDateTime\")";
    
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection));
	}
	
    public function insertOs($os, $bittness)
	{
		$query = "INSERT IGNORE INTO `os`(`os`, `bittness`) VALUES (\"$os\", \"$bittness\")";
 
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection));
	}
	
    public function insertProgramVersion($programVersion, $programBittness)
	{
		$query = "INSERT IGNORE INTO `program_versions` (`version`, `programBittness`) VALUES (\"$programVersion\", \"$programBittness\")";

		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
	}
	
    public function insertCountry($country)
	{
		$query = "INSERT IGNORE INTO `countries` (`country`) VALUES (\"$country\")";
    
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection));
	}
	
    public function insertLanguage($language)
	{
		$query = "INSERT IGNORE INTO `locale_languages` (`localeLanguage`) VALUES (\"$language\")";
     
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection));
	}
	
    public function insertUser($userName, $OSID, $versionID, $countryID, $languageID)
	{
		$userID = getUserIdSql($userName);

		if($userID)
		{
			$query = "UPDATE `users` SET `OSID`=\"$OSID\", `VersionID`=\"$versionID\", `CountryID`=\"$countryID\", `LanguageID`=\"$languageID\" WHERE `ID` = \"$userID\"";
			
			$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection)); 
			
			return;
		} 
		
		$query = "INSERT IGNORE INTO `users` (`UserName`, `OSID`, `VersionID`, `CountryID`, `LanguageID`) VALUES (\"$userName\", \"$OSID\", \"$versionID\", \"$countryID\", \"$languageID\")";
		
		$result = mysqli_query($m_connection, $query) or die("Ошибка " . mysqli_error($m_connection));
	}
    
    private $m_host;
    private $m_dataBaseName;
    private $m_user;
    private $m_password;
    
    private $m_connection;
}

?>