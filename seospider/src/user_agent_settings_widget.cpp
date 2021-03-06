#include "stdafx.h"
#include "user_agent_settings_widget.h"
#include "application.h"
#include "preferences.h"

namespace SeoSpider
{

UserAgentSettingsWidget::UserAgentSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
{
	m_ui.setupUi(this);

	init();

	VERIFY(connect(m_ui.useCustomUserAgentCheckBox, &QCheckBox::toggled,
		this, &UserAgentSettingsWidget::checkBoxStateChanged));

	VERIFY(connect(m_ui.userAgentTypeButtonGroup, SIGNAL(buttonToggled(int, bool)),
		this, SLOT(groupButtonStateChanged())));

	applyChanges();
}

void UserAgentSettingsWidget::checkBoxStateChanged(bool state)
{
	m_ui.desktopUserAgentComboBox->setEnabled(state & m_ui.useDesktopUserAgentRadioButton->isChecked());
	m_ui.mobileUserAgentComboBox->setEnabled(state & m_ui.useMobileUserAgentRadioButton->isChecked());
	m_ui.useDesktopUserAgentRadioButton->setEnabled(state);
	m_ui.useMobileUserAgentRadioButton->setEnabled(state);
}

void UserAgentSettingsWidget::groupButtonStateChanged()
{
	m_ui.desktopUserAgentComboBox->setEnabled(m_ui.useDesktopUserAgentRadioButton->isChecked() & m_ui.useCustomUserAgentCheckBox->isChecked());
	m_ui.mobileUserAgentComboBox->setEnabled(m_ui.useMobileUserAgentRadioButton->isChecked() & m_ui.useCustomUserAgentCheckBox->isChecked());
}

void UserAgentSettingsWidget::loadUserAgentsFromFile(const QString& fileName, QComboBox* targetComboBox)
{
	QFile userAgentsFile(fileName);

	userAgentsFile.open(QIODevice::ReadOnly);
	if (!userAgentsFile.isOpen())
	{
		ERRLOG << "Cannot open" << userAgentsFile.fileName();
		return;
	}

	const QString userAgentString = userAgentsFile.readAll();
	const QStringList userAgentList = userAgentString.split('\n', QString::SkipEmptyParts);

	foreach(auto line, userAgentList)
	{
		targetComboBox->addItem(line, line);
	}
}

void UserAgentSettingsWidget::onShow()
{
}

void UserAgentSettingsWidget::onClose()
{
}

void UserAgentSettingsWidget::init()
{
	loadUserAgentsFromFile(":/config/desktopUserAgents.cfg", m_ui.desktopUserAgentComboBox);
	loadUserAgentsFromFile(":/config/mobileUserAgents.cfg", m_ui.mobileUserAgentComboBox);

	SettingsPage::init();

	checkBoxStateChanged(m_ui.useCustomUserAgentCheckBox->isChecked());
}

void UserAgentSettingsWidget::applyChanges() noexcept
{
	theApp->preferences()->setProperty("userAgent", (m_ui.userAgentTypeButtonGroup->checkedId() == 0 ?
		m_ui.desktopUserAgentComboBox->currentText() :
		m_ui.mobileUserAgentComboBox->currentText())
	);

	DEBUGLOG << "userAgent" << theApp->preferences()->property("userAgent").toString();

	SettingsPage::applyChanges();
}

}