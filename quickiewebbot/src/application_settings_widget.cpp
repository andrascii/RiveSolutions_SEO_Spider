#include "application_settings_widget.h"
#include "service_locator.h"
#include "settings_page_registry.h"
#include "quickie_web_bot_helpers.h"
#include "isettings_page.h"

namespace QuickieWebBot
{

ApplicationSettingsWidget::ApplicationSettingsWidget(QWidget* parent)
	: QDialog(parent)
	, m_stackedWidget(new QStackedWidget(this))
	, m_somethingChanged(false)
{
	initialize();

	VERIFY(connect(m_ui.okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked())));
	VERIFY(connect(m_ui.applyButton, SIGNAL(clicked()), this, SLOT(applyChanges())));
	VERIFY(connect(m_ui.cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked())));

}

void ApplicationSettingsWidget::showEvent(QShowEvent* event)
{
	reloadSettings();
}

void ApplicationSettingsWidget::applyChanges()
{
	for (int index = 0 ; index < m_stackedWidget->count(); index++)
	{
		QWidget* widget = m_stackedWidget->widget(index);

		dynamic_cast<ISettingsPage*>(widget)->applyChanges();
	}

	m_somethingChanged = false;
	m_ui.applyButton->setEnabled(m_somethingChanged);

}

void ApplicationSettingsWidget::okButtonClicked()
{
	applyChanges();
	close();
}

void ApplicationSettingsWidget::cancelButtonClicked()
{
	close();
}

void ApplicationSettingsWidget::reloadSettings()
{
	for (int index = 0; index < m_stackedWidget->count(); index++)
	{
		QWidget* widget = m_stackedWidget->widget(index);

		dynamic_cast<ISettingsPage*>(widget)->reloadSettings();
	}
}

ApplicationSettingsWidget::~ApplicationSettingsWidget()
{
	while (m_stackedWidget->count() > 0)
	{
		const int lastRemovingWidgetIndex = m_stackedWidget->count() - 1;

		QWidget* widget = m_stackedWidget->widget(lastRemovingWidgetIndex);
		widget->setParent(nullptr);

		m_stackedWidget->removeWidget(widget);
	}
}

void ApplicationSettingsWidget::initialize()
{
	m_ui.setupUi(this);

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget(m_stackedWidget);
	m_ui.currentGroup->setLayout(layout);

	VERIFY(connect(m_ui.propGroupsList, SIGNAL(currentRowChanged(int)),
		m_stackedWidget, SLOT(setCurrentIndex(int))));
	
	SettingsPageRegistry* settingsPageRegistry = ServiceLocator::instance()->service<SettingsPageRegistry>();

	foreach (QByteArray pageId, settingsPageRegistry->pagesKeys())
	{
		SettingsPage* page = settingsPageRegistry->settingsPageById(pageId);	
		
		if (page->isAutoApply())
		{
			INFOLOG << pageId << " is not auto apply settings page. It will be ignored!";

			continue;
		}

		QListWidgetItem* item = new QListWidgetItem(page->windowTitle());
		item->setData(Qt::UserRole, pageId);

		m_stackedWidget->addWidget(page);
		m_ui.propGroupsList->addItem(item);

	}
}

}