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
		ERRLOG << "Cannot open " << userAgentsFile.fileName();
		return;
	}

	const QString userAgentString = userAgentsFile.readAll();
	const QStringList userAgentList = userAgentString.split('\n', QString::SkipEmptyParts);

	foreach(auto line, userAgentList)
	{
		targetComboBox->addItem(line, line);
	}
}

bool UserAgentSettingsWidget::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_ui.label && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.useCustomUserAgentCheckBox->toggle();
	}

	if (object == m_ui.label_2 && event->type() == QEvent::MouseButtonRelease)
	{
		m_ui.followRobotsTxtCheckBox->toggle();
	}

	return false;
}

void UserAgentSettingsWidget::init()
{
	m_ui.label->installEventFilter(this);
	m_ui.label_2->installEventFilter(this);

	loadUserAgentsFromFile(":/config/desktopUserAgents.cfg", m_ui.desktopUserAgentComboBox);
	loadUserAgentsFromFile(":/config/mobileUserAgents.cfg", m_ui.mobileUserAgentComboBox);
		
	m_ui.robotsTxtComboBox->addItem("AnyBot", QVariant::fromValue(CrawlerEngine::UserAgentType::AnyBot));
	m_ui.robotsTxtComboBox->addItem("GoogleBot", QVariant::fromValue(CrawlerEngine::UserAgentType::GoogleBot));
	m_ui.robotsTxtComboBox->addItem("YandexBot", QVariant::fromValue(CrawlerEngine::UserAgentType::YandexBot));
	m_ui.robotsTxtComboBox->addItem("MailRuBot", QVariant::fromValue(CrawlerEngine::UserAgentType::MailRuBot));
	m_ui.robotsTxtComboBox->addItem("YahooBot", QVariant::fromValue(CrawlerEngine::UserAgentType::YahooBot));

	SettingsPage::init();

	checkBoxStateChanged(m_ui.useCustomUserAgentCheckBox->isChecked());
	m_ui.robotsTxtComboBox->setEnabled(m_ui.followRobotsTxtCheckBox->isChecked());
}

void UserAgentSettingsWidget::applyChanges() noexcept
{
	theApp->preferences()->setProperty("userAgent", (m_ui.userAgentTypeButtonGroup->checkedId() == 0 ? m_ui.desktopUserAgentComboBox->currentText() : m_ui.mobileUserAgentComboBox->currentText()));

	DEBUGLOG << "userAgent" << theApp->preferences()->property("userAgent").toString();

	SettingsPage::applyChanges();
}

}