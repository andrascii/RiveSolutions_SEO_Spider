function Controller()
{
}

//Controller.prototype.TargetDirectoryPageCallback = function()
//{
//	var installPath = installer.value("HKCU\\Software\\Rive Solutions\\Seo Spider\\MetaData\\InstallPath");	
//	var widget = gui.currentPageWidget();
//	widget.TargetDirectoryLineEdit.textChanged.connect(this, Controller.prototype.targetChanged);
//	Controller.prototype.targetChanged(widget.TargetDirectoryLineEdit.text);
//	
//	if(installPath)
//	{			
//		widget.TargetDirectoryLineEdit.setText(installPath);
//	}
//}
//
//Controller.prototype.targetChanged = function(text)
//{
//	installer.setValue("RemoveTargetDir", true);
//	
//	if(text != "" && installer.fileExists(text + "/components.xml"))
//	{
//		installer.setValue("RemoveTargetDir", false);
//	}
//}