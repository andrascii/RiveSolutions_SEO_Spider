#include "crawler_settings_widget.h"
#include "application_properties.h"

namespace QuickieWebBot
{

CrawlerSettingsWidget::CrawlerSettingsWidget(QWidget* parent)
	: QWidget(parent)
{
	initialize();

	foreach(QObject* obj, m_ui.groupBox->children())
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
	m_ui.setupUi(this);
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
	theApp->properties()->setCheckImages(m_ui.checkImages->isChecked());
	theApp->properties()->setCheckCSS(m_ui.checkCSS->isChecked());
	theApp->properties()->setCheckJavaScript(m_ui.checkJS->isChecked());
	theApp->properties()->setCheckSWF(m_ui.checkSWF->isChecked());

	//
	//setting crawer beraviour
	//
	theApp->properties()->setCheckExternalUrls(m_ui.checkExternalLinks->isChecked());
	theApp->properties()->setFollowInternalNoFollow(m_ui.checkInternalNF->isChecked());
	theApp->properties()->setFollowExternalNoFollow(m_ui.checkExternalNF->isChecked());
	theApp->properties()->setCheckSubdomains(m_ui.checkSubdomains->isChecked());
	theApp->properties()->setCheckCanonicals(m_ui.checkCanonicals->isChecked());
	theApp->properties()->setFollowRobotsTxt(m_ui.checkFollowRobots->isChecked());
}

void CrawlerSettingsWidget::reloadSettings() noexcept
{
	//
	//Checking resourses
	//
	m_ui.checkImages->setChecked(theApp->properties()->checkImages());
	m_ui.checkImages->setEnabled(true);

	m_ui.checkCSS->setChecked(theApp->properties()->checkCSS());
	m_ui.checkCSS->setEnabled(true);

	m_ui.checkJS->setChecked(theApp->properties()->checkJavaScript());
	m_ui.checkJS->setEnabled(true);

	m_ui.checkSWF->setChecked(theApp->properties()->checkSWF());
	m_ui.checkSWF->setEnabled(true);

	//
	//Crawler behaviour
	//
	m_ui.checkExternalLinks->setChecked(theApp->properties()->checkExternalUrls());
	m_ui.checkExternalLinks->setEnabled(true);

	m_ui.checkInternalNF->setChecked(theApp->properties()->followInternalNoFollow());
	m_ui.checkInternalNF->setEnabled(true);

	m_ui.checkExternalNF->setChecked(theApp->properties()->followExternalNoFollow());
	m_ui.checkExternalNF->setEnabled(true);

	m_ui.checkSubdomains->setChecked(theApp->properties()->checkSubdomains());
	m_ui.checkSubdomains->setEnabled(true);

	m_ui.checkCanonicals->setChecked(theApp->properties()->checkCanonicals());
	m_ui.checkCanonicals->setEnabled(true);

	m_ui.checkFollowRobots->setChecked(theApp->properties()->followRobotsTxt());
	m_ui.checkFollowRobots->setEnabled(true);


}

bool CrawlerSettingsWidget::isAutoApply() const noexcept
{
	return false;
}

}