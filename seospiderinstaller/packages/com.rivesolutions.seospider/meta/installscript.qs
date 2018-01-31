function Component()
{
	component.loaded.connect(this, addRegisterFileCheckBox);
    component.unusualFileType = "sxr";
}

addRegisterFileCheckBox = function()
{
	if (installer.isInstaller()) 
	{
        installer.addWizardPageItem(component, "RegisterFileCheckBoxForm", QInstaller.TargetDirectory);
        component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.text =
            component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.text + component.unusualFileType;
    }
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    var isRegisterFileChecked = component.userInterface("RegisterFileCheckBoxForm").RegisterFileCheckBox.checked;
    if (installer.value("os") === "win") {
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