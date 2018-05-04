#include "language_settings_widget.h"
#include "application.h"
#include "preferences.h"

namespace SeoSpider
{

LanguageSettingsWidget::LanguageSettingsWidget(QWidget* parent)
	: SettingsPage(parent)
{
	m_ui.setupUi(this);

	init();

	applyChanges();
}

void LanguageSettingsWidget::init()
{
	loadLanguagesFromFile(":/config/languages.cfg", m_ui.comboBox);

	SettingsPage::init();
}

void LanguageSettingsWidget::loadLanguagesFromFile(const QString& fileName, QComboBox* targetComboBox)
{
	QFile languagesFile(fileName);

	languagesFile.open(QIODevice::ReadOnly);
	if (!languagesFile.isOpen())
	{
		ERRLOG << "Cannot open " << languagesFile.fileName();
		return;
	}

	QString languagesString = languagesFile.readAll();
	QStringList languagesList = languagesString.split('\n', QString::SkipEmptyParts);

	foreach(auto line, languagesList)
	{
		targetComboBox->addItem(line, theApp->preferences()->localeFromString(line.trimmed()));
	}

	INFOLOG << "Languages loaded";
}

void LanguageSettingsWidget::applyChanges() noexcept
{
	theApp->preferences()->setProperty("applicationLanguage", (m_ui.comboBox->currentData()));

	DEBUGLOG << "applicationLanguage: " << theApp->preferences()->property("applicationLanguage").toString();

	SettingsPage::applyChanges();
}

}
