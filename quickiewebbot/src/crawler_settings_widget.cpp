#include "crawler_settings_widget.h"
#include "application_properties.h"

namespace QuickieWebBot
{

CrawlerSettingsWidget::CrawlerSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
{
	initialize();

	QList<QWidget*> widgets = findChildren<QWidget*>();
	widgets.size();

	foreach(QObject* obj, children())
	{
		QCheckBox* tempCheck = qobject_cast<QCheckBox*>(obj);
		if (tempCheck)
		{
			VERIFY(connect(tempCheck, SIGNAL(stateChanged(int)), this, SLOT(somethingChangedSlot())));
		}
	}
}

void CrawlerSettingsWidget::somethingChangedSlot()
{
	INFOLOG << "changed someting";
}

void CrawlerSettingsWidget::initialize()
{
	//setupUi(this);
}

void CrawlerSettingsWidget::applyChanges() noexcept
{
	if (isAutoApply())
	{
		return;
	}

	//
	//setting checking resourses
	//
	theApp->properties()->setCheckImages(checkImages->isChecked());
	theApp->properties()->setCheckCSS(checkCSS->isChecked());
	theApp->properties()->setCheckJavaScript(checkJS->isChecked());
	theApp->properties()->setCheckSWF(checkSWF->isChecked());

	//
	//setting crawer beraviour
	//
	theApp->properties()->setCheckExternalUrls(checkExternalLinks->isChecked());
	theApp->properties()->setFollowInternalNoFollow(checkInternalNF->isChecked());
	theApp->properties()->setFollowExternalNoFollow(checkExternalNF->isChecked());
	theApp->properties()->setCheckSubdomains(checkSubdomains->isChecked());
	theApp->properties()->setCheckCanonicals(checkCanonicals->isChecked());
	theApp->properties()->setFollowRobotsTxt(checkFollowRobots->isChecked());
}

void CrawlerSettingsWidget::reloadSettings() noexcept
{
	//
	//Checking resourses
	//
	checkImages->setChecked(theApp->properties()->checkImages());
	checkImages->setEnabled(true);

	checkCSS->setChecked(theApp->properties()->checkCSS());
	checkCSS->setEnabled(true);

	checkJS->setChecked(theApp->properties()->checkJavaScript());
	checkJS->setEnabled(true);

	checkSWF->setChecked(theApp->properties()->checkSWF());
	checkSWF->setEnabled(true);

	//
	//Crawler behaviour
	//
	checkExternalLinks->setChecked(theApp->properties()->checkExternalUrls());
	checkExternalLinks->setEnabled(true);

	checkInternalNF->setChecked(theApp->properties()->followInternalNoFollow());
	checkInternalNF->setEnabled(true);

	checkExternalNF->setChecked(theApp->properties()->followExternalNoFollow());
	checkExternalNF->setEnabled(true);

	checkSubdomains->setChecked(theApp->properties()->checkSubdomains());
	checkSubdomains->setEnabled(true);

	checkCanonicals->setChecked(theApp->properties()->checkCanonicals());
	checkCanonicals->setEnabled(true);

	checkFollowRobots->setChecked(theApp->properties()->followRobotsTxt());
	checkFollowRobots->setEnabled(true);


}

bool CrawlerSettingsWidget::isAutoApply() const noexcept
{
	return false;
}

}