function isWindows()
{
	return systemInfo.productType === "windows";
}

function Component()
{
	installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
	
	if(isAlreadyInstalled() /*&& !needUpdate()*/) // qt installer framework is not supporting offline updates for now, see https://bugreports.qt.io/browse/QTIFW-573
	{
		QMessageBox["information"]("Already installed", "Installer", 
		"Installer found existing SEO Spider installation.<br> You must delete installed program for continue.", 
		QMessageBox.Ok)
		
		abortInstallation();
		
		return;
	}

	if (isWindows())
	{
		var programFiles = installer.environmentVariable("ProgramFiles");
		if (programFiles != "") {
			installer.setValue("TargetDir", programFiles + "/Rive Solutions/SEO Spider");
			installer.setValue("RunProgram", programFiles + "/Rive Solutions/SEO Spider/seospider.exe");
		}
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
			"RiveSolutions", 
			"SEO Spider",
			"MetaData/InstallPath", 
			"@TargetDir@");
		
		component.addOperation(
			"GlobalConfig", 
			"RiveSolutions", 
			"SEO Spider",
			"MetaData/Version", 
			version);
			
		component.addOperation(
			"GlobalConfig", 
			"RiveSolutions", 
			"SEO Spider",
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
	if (isWindows())
	{
		component.addOperation("Execute", "@TargetDir@/vcredist.exe", "/quiet", "/norestart");
	}
}

function addRegistryKey(key, value, data) 
{ 
	var args = [ "cmd", "cmd", "/c", "REG", "ADD", key, "/f", "/v", value, "/t", "REG_SZ", "/d", data ];
	component.addElevatedOperation( "Execute", args ); 
}

function addRegistryDefaultKey(key, data) 
{ 
	var args = [ "cmd", "cmd", "/c", "REG", "ADD", key, "/f", "/ve", "/d", data ];
	component.addElevatedOperation("Execute", args); 
}

function registerCustomURI()
{
	if (isWindows())
	{
		// only for windows
		addRegistryDefaultKey("HKEY_CLASSES_ROOT\\seospider", "SeoSpider protocol");
		addRegistryKey("HKEY_CLASSES_ROOT\\seospider", "URL Protocol", "");
		addRegistryDefaultKey("HKEY_CLASSES_ROOT\\seospider\\DefaultIcon", "seospider.exe,1");
		addRegistryDefaultKey("HKEY_CLASSES_ROOT\\seospider\\shell\\open\\command", "\"@TargetDir@\\seospider.exe\" \"%1\"");
	}
}

function createMenuDirectory()
{
	if (!isWindows())
	{
		return;
	}

	var dontCreateMenuDirChecked = component.userInterface("CreateMenuDirCheckBoxForm").CreateMenuDirCheckBox.checked;
	
	if(!dontCreateMenuDirChecked)
	{
		component.addOperation(
			"CreateShortcut", 
			"@TargetDir@/seospider.exe", 
			"@StartMenuDir@/RiveSolution SEO Spider.lnk",
			"workingDirectory=@TargetDir@", 
			"iconPath=@TargetDir@/seospider.exe",
			"iconId=0" , 
			"description=Open SEO Spider");
			
		component.addOperation(
			"CreateShortcut", 
			"@TargetDir@/uninstall.exe", 
			"@StartMenuDir@/Uninstall RiveSolution SEO Spider.lnk",
			"workingDirectory=@TargetDir@", 
			"iconPath=@TargetDir@/uninstall.exe",
			"iconId=0" , 
			"description=Open SEO Spider");	
	}
}

function createDesktopShortcut()
{
	if (!isWindows())
	{
		return;
	}
	var isCreateDesktopIconChecked = component.userInterface("CreateDesktopIconCheckBoxForm").CreateDesktopIconCheckBox.checked;
	
	if(isCreateDesktopIconChecked)
	{
		component.addOperation(
			"CreateShortcut", 
			"@TargetDir@/seospider.exe", 
			"@DesktopDir@/RiveSolution SEO Spider.lnk",
			"workingDirectory=@TargetDir@", 
			"iconPath=@TargetDir@/seospider.exe",
			"iconId=0" , 
			"description=Open SEO Spider");
	}
}

// Registering new file type
function registerSXRFileType()
{
	if (isWindows()) 
	{
        var seoSpiderPath =  "@TargetDir@\\seospider.exe";
        component.addOperation("RegisterFileType",
                               component.unusualFileType,
                               seoSpiderPath + " --open_serialized_file %1",
                               "RiveSolution SEO Spider project file type",
                               "text/xml",
                               seoSpiderPath + "," + 1,
                               "ProgId=RiveSolution.SEO Spider." + component.unusualFileType);
    }
}

function isAlreadyInstalled()
{
	if(installer.isUpdater())
	{
		return false;
	}
	
	if (isWindows())
	{
		var version = installer.value("HKCU\\Software\\RiveSolutions\\SEO Spider\\MetaData\\Version");
	
		if(version)
		{
			return true;
		}
	} else {
		// mac
		// TODO: implement for mac os
		return false;
	}
	
	
	return false;
}

// Check if newer version is already installed
function needUpdate()
{
	if (isWindows())
	{
		var newVersion = component.value("Version");
		var savedVersion = installer.value("HKCU\\Software\\RiveSolutions\\SEO Spider\\MetaData\\Version");

		if(savedVersion)
		{
			savedVersion.trim();

			return compareVersions(savedVersion, newVersion) < 0;
		}	
	}

	// TODO: add checks for mac os
	
	return true;
}

function compareVersions(localVersion, remoteVersion)
{
	var localVersionList = localVersion.split(".");
	var localMajor = parseInt(localVersionList[0]);
	var localMinor = parseInt(localVersionList[1]);
	var localMaitenance = parseInt(localVersionList[2]);
	
	var remoteVersionList = remoteVersion.split(".");
	var remoteMajor = parseInt(remoteVersionList[0]);
	var remoteMinor = parseInt(remoteVersionList[1]);
	var remoteMaitenance = parseInt(remoteVersionList[2]);

	var local = localMajor * 1000000 + localMinor * 1000 + localMaitenance;
	var remote = remoteMajor * 1000000 + remoteMinor * 1000 + remoteMaitenance;
	
	if (local > remote) {
		return 1;
	}

	if (local < remote) {
		return -1;
	}

	return 0;
}

Component.prototype.createOperations = function()
{
    component.createOperations();

	registerProgram();
	
	installVCRedist();

	registerCustomURI();
	
	createMenuDirectory();
	
	createDesktopShortcut();

	registerSXRFileType();
}