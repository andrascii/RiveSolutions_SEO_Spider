#include "application_settings_dialog.h"
#include "service_locator.h"
#include "settings_page_registry.h"
#include "helpers.h"
#include "custom_proxy_styles.h"
#include "crawler.h"
#include "application.h"
#include "widget_helpers.h"
#include "cursor_factory.h"
#include "statistic_counter.h"

namespace SeoSpider
{

ApplicationSettingsDialog::ApplicationSettingsDialog(QWidget* parent)
	: Dialog(parent)
	, m_somethingChanged(false)
{
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::crawlerStarted, this, &ApplicationSettingsDialog::onCrawlerStarted));
	VERIFY(connect(theApp->crawler(), &CrawlerEngine::Crawler::crawlerFinished, this, &ApplicationSettingsDialog::onCrawlerFinished));

	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
	flags = flags & (~helpFlag);
	setWindowFlags(flags);

	initialize();

	connect(m_ui.okButton, &QPushButton::clicked, this, &ApplicationSettingsDialog::okButtonClicked);
	connect(m_ui.applyButton, &QPushButton::clicked, this, &ApplicationSettingsDialog::applyChanges);
	connect(m_ui.cancelButton, &QPushButton::clicked, this, &ApplicationSettingsDialog::cancelButtonClicked);

	VERIFY(connect(m_ui.propGroupsList, SIGNAL(currentRowChanged(int)), m_ui.stackedWidget, SLOT(setCurrentIndex(int))));
	VERIFY(connect(m_ui.stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(currentSettingsPageChanged(int))));

	const int width = Common::Helpers::pointsToPixels(800);
	const int height = Common::Helpers::pointsToPixels(500);

	resize(width, height);
}

void ApplicationSettingsDialog::showEvent(QShowEvent*)
{
	StatisticCounter showCounter("ApplicationSettingsDialogShow");
	showCounter.increment();

	reloadSettingsSlot();
	m_ui.applyButton->setEnabled(m_somethingChanged);

	WidgetHelpers::moveWidgetToHostCenter(this);
}

void ApplicationSettingsDialog::hideEvent(QHideEvent*)
{
	restoreChangedValues();
}


void ApplicationSettingsDialog::closeEvent(QCloseEvent*)
{
	for (int index = 0; index < m_ui.stackedWidget->count(); index++)
	{
		QWidget* widget = m_ui.stackedWidget->widget(index);
		Common::Helpers::fast_cast<SettingsPage*>(widget)->onClose();
	}
}

void ApplicationSettingsDialog::restoreChangedValues()
{
	m_somethingChanged = false;

	for (int index = 0; index < m_ui.stackedWidget->count(); index++)
	{
		QWidget* widget = m_ui.stackedWidget->widget(index);
		Common::Helpers::fast_cast<SettingsPage*>(widget)->setSomethingChanged(m_somethingChanged);
	}

	m_ui.applyButton->setEnabled(m_somethingChanged);
}

void ApplicationSettingsDialog::applyChanges()
{
	m_somethingChanged = false;

	for (int index = 0 ; index < m_ui.stackedWidget->count(); index++)
	{
		QWidget* widget = m_ui.stackedWidget->widget(index);
		Common::Helpers::fast_cast<SettingsPage*>(widget)->applyChanges();
	}

	m_ui.applyButton->setEnabled(m_somethingChanged);
}

void ApplicationSettingsDialog::okButtonClicked()
{
	applyChanges();
	close();
}

void ApplicationSettingsDialog::cancelButtonClicked()
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

void ApplicationSettingsDialog::reloadSettingsSlot()
{
	QWidget* widget = m_ui.stackedWidget->currentWidget();

	if (!widget)
	{
		return;
	}

	Common::Helpers::fast_cast<SettingsPage*>(widget)->reloadSettings();
	m_somethingChanged = false;
}

void ApplicationSettingsDialog::onCrawlerStarted()
{
	setEnabled(false);
}

void ApplicationSettingsDialog::onCrawlerFinished()
{
	setEnabled(true);
}


void ApplicationSettingsDialog::currentSettingsPageChanged(int index)
{
	if (index == -1)
	{
		return;
	}

	QWidget* widget = m_ui.stackedWidget->widget(index);
	Common::Helpers::fast_cast<SettingsPage*>(widget)->onShow();
}

ApplicationSettingsDialog::~ApplicationSettingsDialog()
{
	while (m_ui.stackedWidget->count() > 0)
	{
		const int lastRemovingWidgetIndex = m_ui.stackedWidget->count() - 1;

		QWidget* widget = m_ui.stackedWidget->widget(lastRemovingWidgetIndex);
		widget->setParent(nullptr);

		m_ui.stackedWidget->removeWidget(widget);
	}
}

void ApplicationSettingsDialog::setCurrentPage(const QByteArray& settingsPageName)
{
	if (settingsPageName.isEmpty())
	{
		m_ui.propGroupsList->setCurrentRow(0);
		return;
	}

	DEBUG_ASSERT(m_pageIndex.contains(settingsPageName));
	m_ui.propGroupsList->setCurrentRow(m_pageIndex[settingsPageName]);
}

void ApplicationSettingsDialog::somethingChangedSlot()
{
	m_somethingChanged = true;
	m_ui.applyButton->setEnabled(m_somethingChanged);
}

void ApplicationSettingsDialog::initialize()
{
	m_ui.setupUi(this);

	m_ui.propGroupsList->setStyle(new ListItemProxyStyle);
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
