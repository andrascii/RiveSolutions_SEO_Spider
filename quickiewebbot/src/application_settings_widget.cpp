#include "application_settings_widget.h"
#include "service_locator.h"
#include "settings_page_registry.h"

namespace QuickieWebBot
{

ApplicationSettingsWidget::ApplicationSettingsWidget(QWidget* parent)
	: QWidget(parent, Qt::Dialog)
	, m_stackedWidget(new QStackedWidget(this))
{
	initialize();
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

	foreach (QByteArray pageId , settingsPageRegistry->pagesKeys())
	{
		QWidget* page = settingsPageRegistry->settingsPageById(pageId);	
		QListWidgetItem* item = new QListWidgetItem(page->windowTitle());

		item->setData(Qt::UserRole, pageId);

		m_stackedWidget->addWidget(page);

		m_ui.propGroupsList->addItem(item);
	}
}

}