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

	VERIFY(connect(m_ui.okButton, SIGNAL(clicked()), 
		this, SLOT(okButtonClicked())));
	VERIFY(connect(m_ui.applyButton, SIGNAL(clicked()), 
		this, SLOT(applyChanges())));
	VERIFY(connect(m_ui.cancelButton, SIGNAL(clicked()),
		this, SLOT(cancelButtonClicked())));
	VERIFY(connect(m_ui.propGroupsList, SIGNAL(currentRowChanged(int)),
		m_stackedWidget, SLOT(setCurrentIndex(int))));
	VERIFY(connect(m_stackedWidget, SIGNAL(currentChanged(int)),
		this, SLOT(reloadSettingsSlot())));
}



void ApplicationSettingsWidget::showEvent(QShowEvent* event)
{
	reloadSettingsSlot();
	m_ui.applyButton->setEnabled(m_somethingChanged);
}

void ApplicationSettingsWidget::hideEvent(QHideEvent* event)
{
	uncheckChangedValues();
}


void ApplicationSettingsWidget::uncheckChangedValues()
{
	m_somethingChanged = false;

	for (int index = 0; index < m_stackedWidget->count(); index++)
	{
		QWidget* widget = m_stackedWidget->widget(index);

		QuickieWebBotHelpers::safe_runtime_static_cast<SettingsPage*>(widget)->setSomethingChanged(m_somethingChanged);
	}

	m_ui.applyButton->setEnabled(m_somethingChanged);
}

void ApplicationSettingsWidget::applyChanges()
{
	m_somethingChanged = false;

	for (int index = 0 ; index < m_stackedWidget->count(); index++)
	{
		QWidget* widget = m_stackedWidget->widget(index);

		QuickieWebBotHelpers::safe_runtime_static_cast<SettingsPage*>(widget)->applyChanges();
	}
		
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

void ApplicationSettingsWidget::reloadSettingsSlot()
{
	QWidget* widget = m_stackedWidget->currentWidget();
	
	QuickieWebBotHelpers::safe_runtime_static_cast<SettingsPage*>(widget)->reloadSettings();
	m_somethingChanged = false;
}

ApplicationSettingsWidget::~ApplicationSettingsWidget()
{
	VERIFY(disconnect(m_stackedWidget, SIGNAL(currentChanged(int)),
		this, SLOT(reloadSettingsSlot())));

	while (m_stackedWidget->count() > 0)
	{
		const int lastRemovingWidgetIndex = m_stackedWidget->count() - 1;

		QWidget* widget = m_stackedWidget->widget(lastRemovingWidgetIndex);
		widget->setParent(nullptr);

		m_stackedWidget->removeWidget(widget);
	}
}

void ApplicationSettingsWidget::somethingChangedSlot()
{
	m_somethingChanged = true;
	m_ui.applyButton->setEnabled(m_somethingChanged);
}

void ApplicationSettingsWidget::initialize()
{
	m_ui.setupUi(this);

	m_ui.settingsPageWidget->layout()->addWidget(m_stackedWidget);

	m_ui.propGroupsList->setCurrentRow(0);

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
		item->setData(Qt::DecorationRole, page->windowIcon());
		item->setData(Qt::UserRole, pageId);
		VERIFY(connect(page, SIGNAL(somethingChangedSignal()), this, SLOT(somethingChangedSlot())));

		m_stackedWidget->addWidget(page);
		m_ui.propGroupsList->addItem(item);
	}
}

}