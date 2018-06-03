function Component()
{	
	//
	// TODO: remake this logic when new version of QTIF released
	//

	//if(isAlreadyInstalled() && needUpdate())
	//{
	//	if(QMessageBox["question"]("Already installed", "Installer", 
	//	"Installer found older version of Seo Spider installed.<br> Do you want to update program?", 
	//	QMessageBox.Ok | QMessageBox.No) == QMessageBox.No)
	//	{
	//		abortInstallation();
	//		
	//		return;
	//	}
	//	else
	//	{	
	//		var widget = gui.pageById(QInstaller.Introduction)
	//		var installPath = installer.value("HKCU\\Software\\Rive Solutions\\Seo Spider\\MetaData\\InstallPath");	
	//		
	//		if(widget)
	//		{
	//			widget.MessageLabel.setText("Installer found older version of Seo Spider installed.");
	//		}
	//		
	//		installer.setInstallerBaseBinary(installPath + "/Uninstall SeoSpider.exe")
	//		installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
	//		installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
	//		
	//		gui.clickButton(buttons.NextButton);
	//	}
	//}		
	//
	//if(isAlreadyInstalled() && (!needUpdate()))
	//{
	//	QMessageBox["information"]("Already installed", "Installer", 
	//	"Installer found newer or equal verion of SeoSpider.<br> You must delete installed program for continue.", 
	//	QMessageBox.Ok)
	//	
	//	abortInstallation();
	//	
	//	return;
	//}
	
	if(isAlreadyInstalled())
	{
		QMessageBox["information"]("Already installed", "Installer", 
		"Installer found newer or equal verion of SeoSpider.<br> You must delete installed program for continue.", 
		QMessageBox.Ok)
		
		abortInstallation();
		
		return;
	}
	
	component.loaded.connect(this, addCreateMenuDirectory);
	component.loaded.connect(this, addCreateDesktopShortcut);
    component.unusualFileType = "ssproj";
	
}

function abortInstallation()
{
	installer.setValue("FinishedText", "<font color='red' size=3>The installer was quit.</font>");
    //installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    //installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, false);
    //installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    //installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
    //installer.setDefaultPageVisible(QInstaller.PerformInstallation, false);
    //installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
	var widget = gui.pageById(QInstaller.InstallationFinished);
		
	if(widget)
	{
		widget.RunItCheckBox.setEnabled(false);
	}
	
    //gui.clickButton(buttons.NextButton);
	
	gui.reject();
}

// Register program metadata in system after installation
function registerProgram()
{
	if (installer.isInstaller() || installer.isUpdater()) 
	{
		var version = component.value("Version");
		
		component.addOperation(
			"GlobalConfig", 
			"Rive Solutions", 
			"Seo Spider",
			"MetaData/InstallPath", 
			"@TargetDir@");
		
		component.addOperation(
			"GlobalConfig", 
			"Rive Solutions", 
			"Seo Spider",
			"MetaData/Version", 
			version);
			
		component.addOperation(
			"GlobalConfig", 
			"Rive Solutions", 
			"Seo Spider",
			"MetaData/Installed", 
			true);
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

function installVCRedist()
{
	if(systemInfo.currentCpuArchitecture.search("64") < 0) 
	{
		component.addOperation("Execute", "@TargetDir@/vc_redist.x32.exe", "/quiet", "/norestart");
	} else 
	{
		component.addOperation("Execute", "@TargetDir@/vc_redist.x64.exe", "/quiet", "/norestart");
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

// Registering new file type
function registerSXRFileType()
{
	if (installer.value("os") === "win") 
	{
        var seoSpiderPath =  "@TargetDir@\\seospider.exe";
        component.addOperation("RegisterFileType",
                               component.unusualFileType,
                               seoSpiderPath + " --open_serialized_file %1",
                               "RiveSolutions SeoSpider project file type",
                               "text/xml",
                               seoSpiderPath + "," + 1,
                               "ProgId=RiveSolutions.SeoSpider." + component.unusualFileType);
    }
}

function isAlreadyInstalled()
{
	if(installer.isUpdater())
	{
		return false;
	}
	
	var version = installer.value("HKCU\\Software\\Rive Solutions\\Seo Spider\\MetaData\\Version");
	
	if(version)
	{
		return true;
	}
	
	return false;
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
	
	installVCRedist();
	
	createMenuDirectory();
	
	createDesktopShortcut();

	registerSXRFileType();
}