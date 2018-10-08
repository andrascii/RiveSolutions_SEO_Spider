<?php

class StatisticsFileManager
{
    function StatisticsFileManager($statisticFilesFolder, $backupFilesFolder, $brokenFilesFolder)
    {
        $m_statisticFilesFolder = $statisticFilesFolder;
        $m_backupFilesFolder = $backupFilesFolder;
        $m_brokenFilesFolder = $brokenFilesFolder;
        
        initialize();
    }
    
    private function initialize()
    {
        $statisticsFiles = scandir($m_statisticFilesFolder);
        
        foreach ($statisticFiles as $statisticFile)
        {
            $statisticFilePath = $m_statisticFilesFolder . "/" . $statisticFile;
            
            if(strpos($statisticFile, 'statistics_') !== false)
            {
                if(strpos($statisticFile, '_debug') === true)
                {
                    $m_debugStatisticFiles.apend($statisticFilePath);
                }
                else
                {
                    $m_releaseStatisticFiles.apend($statisticFilePath);
                }
            }
        }
    }
    
    public function getReleaseStatisticFiles() 
    {
        return $m_releaseStatisticFiles;
    }
    
    public function getDebugStatisticFiles()
    {
        return $m_debugStatisticFiles;
    }
    
    public function removeFiles($fileList)
    {
        foreach ($fileList as $file)
        {
            unlink($file);
        }
    }
    
    public function renameFile($oldName, $newName)
    {
        rename($oldName, $newName);
    }
    
    public function moveFileToBackup($filePath)
    {
        $fileName = basename($filePath);
        renameFile($filePath, $m_backupFilesFolder . "/" . $fileName);
    }
    
    public function moveFileToBroken($filePath)
    {
        $fileName = basename($filePath);
        renameFile($filePath, $m_brokenFilesFolder . "/" . $fileName);
    }
    
    private $m_statisticFilesFolder;
    private $m_backupFilesFolder;
    private $m_brokenFilesFolder;
    
    private $m_releaseStatisticFiles;
    private $m_debugStatisticFiles;
}

?>