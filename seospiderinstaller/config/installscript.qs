function Controller()
{
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
	var widget = gui.currentPageWidget();
	widget.TargetDirectoryLineEdit.textChanged.connect(this, Controller.prototype.targetChanged);
	Controller.prototype.targetChanged(widget.TargetDirectoryLineEdit.text);
}

Controller.prototype.targetChanged = function(text)
{
	installer.setValue("RemoveTargetDir", true);
	
	if(text != "" && installer.fileExists(text + "/components.xml"))
	{
		installer.setValue("RemoveTargetDir", false);
	}
}