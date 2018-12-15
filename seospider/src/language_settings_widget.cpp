#include "language_settings_widget.h"
#include "application.h"
#include "preferences.h"
#include "dialog.h"

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
	loadLanguagesFromFile(":/config/languages.cfg");

	SettingsPage::init();
}

void LanguageSettingsWidget::loadLanguagesFromFile(const QString& fileName)
{
	QFile languagesFile(fileName);

	languagesFile.open(QIODevice::ReadOnly);
	if (!languagesFile.isOpen())
	{
		ERRLOG << "Cannot open " << languagesFile.fileName();
		return;
	}

	const QString languagesString = languagesFile.readAll();
	const QStringList languagesList = languagesString.split('\n', QString::SkipEmptyParts);

	foreach(auto line, languagesList)
	{
		m_ui.comboBox->addItem(line, theApp->preferences()->localeFromString(line.trimmed()));
	}
}

void LanguageSettingsWidget::onClose()
{
}

void LanguageSettingsWidget::applyChanges() noexcept
{
	theApp->preferences()->setProperty("applicationLanguage", (m_ui.comboBox->currentData()));

	DEBUGLOG << "applicationLanguage: " << theApp->preferences()->property("applicationLanguage").toString();

	const bool languageChanged = somethingChanged();

	SettingsPage::applyChanges();
	
	if (languageChanged)
	{
		Dialog::showDeferredMessageBoxDialog(QObject::tr("Change language"),
			QObject::tr("You should restart the application to apply these changes"), QDialogButtonBox::Ok);
	}
}

void LanguageSettingsWidget::onShow()
{
}

}
