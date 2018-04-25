function Component()
{	
	//if(!needUpdate())
	//{
	//	QMessageBox["information"]("version.test", "Installer", "The newer version is already installed.", QMessageBox.Ok);
	//	abortInstallation();
	//	//installer.setUpdater();
	//}
			
	component.loaded.connect(this, addCreateMenuDirectory);
	component.loaded.connect(this, addCreateDesktopShortcut);
    component.unusualFileType = "sxr";
}

function abortInstallation()
{
	installer.setValue("FinishedText", "<font color='red' size=3>The installer was quit.</font>");
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, false);
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
    installer.setDefaultPageVisible(QInstaller.PerformInstallation, false);
    installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
	var widget = gui.pageById(QInstaller.InstallationFinished);
	
	//if(widget)
	//{
		widget.RunItCheckBox.setEnabled(false);
	//}
	
    gui.clickButton(buttons.NextButton);
}

// Register program metadata in system after installation
function registerProgram()
{
	if (installer.isInstaller()) 
	{
		QMessageBox["information"]("version.test", "Installer", "Registering program.", QMessageBox.Ok);
		var reg = installer.environmentVariable("SystemRoot") + "\\System32\\reg.exe";
		var key = "HKCU\\Software\\Rive Solutions\\Seo Spider\\MetaData";
		var value = component.value("Version");

		component.addOperation("Execute", reg, "ADD", key, "/v", "Version", "/t", "REG_SZ", "/d", value, "/f");
		component.addOperation("Execute", reg, "ADD", key, "/v", "InstallPath", "/t", "REG_SZ", "/d", "@TargetDir@", "/f");
	}
}

addCreateMenuDirectory = function()
{
	if(installer.isInstaller())
	{
        installer.addWizardPageItem(component, "CreateMenuDirCheckBoxForm", QInstaller.StartMenuSelection);
	}
}

addCreateDesktopShortcut = function()
{
	if(installer.isInstaller())
	{
		installer.addWizardPageItem(component, "CreateDesktopIconCheckBoxForm", QInstaller.TargetDirectory);
	}
}

function createMenuDirectory()
{
	var dontCreateMenuDirChecked = component.userInterface("CreateMenuDirCheckBoxForm").CreateMenuDirCheckBox.checked;
	
	if(systemInfo.productType === "windows" && (!dontCreateMenuDirChecked))
	{
		component.addOperation(
			"CreateShortcut", 
			"@TargetDir@/seospider.exe", 
			"@StartMenuDir@/RiveSolutions Seo Spider.lnk",
			"workingDirectory=@TargetDir@", 
			"iconPath=@TargetDir@/seospider.exe",
			"iconId=0" , 
			"description=Open Seo Spider");
			
		component.addOperation(
			"CreateShortcut", 
			"@TargetDir@/Uninstall SeoSpider.exe", 
			"@StartMenuDir@/Uninstall RiveSolutions Seo Spider.lnk",
			"workingDirectory=@TargetDir@", 
			"iconPath=@TargetDir@/Uninstall SeoSpider.exe",
			"iconId=0" , 
			"description=Open Seo Spider");	
	}
}

function createDesktopShortcut()
{
	var isCreateDesktopIconChecked = component.userInterface("CreateDesktopIconCheckBoxForm").CreateDesktopIconCheckBox.checked;
	
	if(systemInfo.productType === "windows" && isCreateDesktopIconChecked)
	{
		component.addOperation(
			"CreateShortcut", 
			"@TargetDir@/seospider.exe", 
			"@DesktopDir@/RiveSolutions Seo Spider.lnk",
			"workingDirectory=@TargetDir@", 
			"iconPath=@TargetDir@/seospider.exe",
			"iconId=0" , 
			"description=Open Seo Spider");
	}
}

// Check if newer version is already installed
function needUpdate()
{
	var newVersion = component.value("Version");
	var savedVersion = installer.value("HKCU\\Software\\Rive Solutions\\Seo Spider\\MetaData\\Version");
	
	if(savedVersion)
	{
		savedVersion.trim();
		
		if(compareVersions(savedVersion, newVersion) > 0)
		{
			return false;
		}
		if(compareVersions(savedVersion, newVersion) < 0)
		{
			return true;
		}
		if(compareVersions(savedVersion, newVersion) == 0)
		{
			return false;
		}
	}
	
	return true;
}

function compareVersions(localVersion, remoteVersion)
{
	var localVersionList = localVersion.split(".");
	var localMajor = localVersionList[0];
	var localMinor = localVersionList[1];
	var localMaitenance = localVersionList[2];
	
	var remoteVersionList = remoteVersion.split(".");
	var remoteMajor = remoteVersionList[0];
	var remoteMinor = remoteVersionList[1];
	var remoteMaitenance = remoteVersionList[2];
	
	if(localMajor > remoteMajor)
	{
		return 1;
	}
	if(localMinor > remoteMinor)
	{
		return 1;
	}
	if(localMaitenance > remoteMaitenance)
	{
		return 1;
	}
		
	//--------------------------------------------------------------------------------------------
	if(localMajor < remoteMajor)
	{
		return -1;
	}
	if(localMinor < remoteMinor)
	{
		return -1;
	}
	if(localMaitenance < remoteMaitenance)
	{
		return -1;
	}

	return 0;
}

Component.prototype.createOperations = function()
{
    component.createOperations();

	registerProgram();
	
	createMenuDirectory();
	
	createDesktopShortcut();
	
	// Registering new file type
    if (installer.value("os") === "win") 
	{
        var iconId = 1;
        var seoSpiderPath =  "@TargetDir@\\seospider.exe";
        component.addOperation("RegisterFileType",
                               component.unusualFileType,
                               seoSpiderPath + " --open_serialized_file %1",
                               "RiveSolutions SeoSpider project file type",
                               "text/xml",
                               seoSpiderPath + "," + iconId,
                               "ProgId=RiveSolutions.SeoSpider." + component.unusualFileType);
    }
}