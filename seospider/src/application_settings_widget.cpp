#include "application_settings_widget.h"
#include "service_locator.h"
#include "settings_page_registry.h"
#include "helpers.h"



class ListItemProxyStyle : public QProxyStyle
{

public:
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option,
		QPainter* painter, const QWidget* widget = nullptr) const
	{
		if (PE_FrameFocusRect == element)
		{
			return;
		}

		QProxyStyle::drawPrimitive(element, option, painter, widget);
	}
};




namespace SeoSpider
{

ApplicationSettingsWidget::ApplicationSettingsWidget(QWidget* parent)
	: QDialog(parent)
	, m_somethingChanged(false)
{
	initialize();

	connect(m_ui.okButton, &QPushButton::clicked, this, &ApplicationSettingsWidget::okButtonClicked);
	connect(m_ui.applyButton, &QPushButton::clicked, this, &ApplicationSettingsWidget::applyChanges);
	connect(m_ui.cancelButton, &QPushButton::clicked, this, &ApplicationSettingsWidget::cancelButtonClicked);

	VERIFY(connect(m_ui.propGroupsList, SIGNAL(currentRowChanged(int)), m_ui.stackedWidget, SLOT(setCurrentIndex(int))));
	//VERIFY(connect(m_ui.stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(reloadSettingsSlot())));

	const int width = Common::Helpers::pointsToPixels(800);
	const int height = Common::Helpers::pointsToPixels(500);

	resize(width, height);
}

void ApplicationSettingsWidget::showEvent(QShowEvent* event)
{
	reloadSettingsSlot();
	m_ui.applyButton->setEnabled(m_somethingChanged);
}

void ApplicationSettingsWidget::hideEvent(QHideEvent* event)
{
	restoreChangedValues();
}

void ApplicationSettingsWidget::restoreChangedValues()
{
	m_somethingChanged = false;

	for (int index = 0; index < m_ui.stackedWidget->count(); index++)
	{
		QWidget* widget = m_ui.stackedWidget->widget(index);

		Common::Helpers::fast_cast<SettingsPage*>(widget)->setSomethingChanged(m_somethingChanged);
	}

	m_ui.applyButton->setEnabled(m_somethingChanged);
}

void ApplicationSettingsWidget::applyChanges()
{
	m_somethingChanged = false;

	for (int index = 0 ; index < m_ui.stackedWidget->count(); index++)
	{
		QWidget* widget = m_ui.stackedWidget->widget(index);

		Common::Helpers::fast_cast<SettingsPage*>(widget)->applyChanges();
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
	ISettingsPageRegistry* settingsPageRegistry = CrawlerEngine::ServiceLocator::instance()->service<ISettingsPageRegistry>();
	
	foreach(const QByteArray& pageId, settingsPageRegistry->pagesKeys())
	{
		SettingsPage* page = settingsPageRegistry->settingsPageById(pageId);
		page->reloadSettings();
		page->clearChangedKeys();
	}
	
	m_somethingChanged = false;
	m_ui.applyButton->setEnabled(m_somethingChanged);
	close();
}

void ApplicationSettingsWidget::reloadSettingsSlot()
{
	QWidget* widget = m_ui.stackedWidget->currentWidget();

	if (!widget)
	{
		return;
	}
	
	Common::Helpers::fast_cast<SettingsPage*>(widget)->reloadSettings();
	m_somethingChanged = false;
}

ApplicationSettingsWidget::~ApplicationSettingsWidget()
{
	//VERIFY(disconnect(m_ui.stackedWidget, SIGNAL(currentChanged(int)),
	//	this, SLOT(reloadSettingsSlot())));

	while (m_ui.stackedWidget->count() > 0)
	{
		const int lastRemovingWidgetIndex = m_ui.stackedWidget->count() - 1;

		QWidget* widget = m_ui.stackedWidget->widget(lastRemovingWidgetIndex);
		widget->setParent(nullptr);

		m_ui.stackedWidget->removeWidget(widget);
	}
}

void ApplicationSettingsWidget::setCurrentPage(const QByteArray& settingsPageName)
{
	if (settingsPageName.isEmpty())
	{
		m_ui.propGroupsList->setCurrentRow(0);
		return;
	}

	DEBUG_ASSERT(m_pageIndex.contains(settingsPageName));
	m_ui.propGroupsList->setCurrentRow(m_pageIndex[settingsPageName]);
}

void ApplicationSettingsWidget::somethingChangedSlot()
{
	m_somethingChanged = true;
	m_ui.applyButton->setEnabled(m_somethingChanged);
}

void ApplicationSettingsWidget::initialize()
{
	m_ui.setupUi(this);

	m_ui.propGroupsList->setStyle(new ListItemProxyStyle());
	m_ui.propGroupsList->setCurrentRow(0);
	
	ISettingsPageRegistry* settingsPageRegistry = CrawlerEngine::ServiceLocator::instance()->service<ISettingsPageRegistry>();

	int pageIndex = 0;

	foreach (const QByteArray& pageId, settingsPageRegistry->pagesKeys())
	{
		SettingsPage* page = settingsPageRegistry->settingsPageById(pageId);
		page->setParent(this);

		if (page->isAutoApply())
		{
			INFOLOG << pageId << " is not auto apply settings page. It will be ignored!";

			continue;
		}

		QListWidgetItem* item = new QListWidgetItem(page->windowTitle());
		item->setData(Qt::DecorationRole, page->windowIcon());
		item->setData(Qt::UserRole, pageId);
		VERIFY(connect(page, SIGNAL(somethingChangedSignal()), this, SLOT(somethingChangedSlot())));

		m_ui.stackedWidget->addWidget(page);
		m_ui.propGroupsList->addItem(item);

		m_pageIndex[pageId] = pageIndex++;
	}

	m_ui.propGroupsList->setCurrentRow(0);
}

}