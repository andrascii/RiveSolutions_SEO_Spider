#include "naviagation_panel_widget.h"
#include "summary_model.h"
#include "summary_view_model.h"
#include "page_info_storage_model.h"
#include "page_info_storage_view_model.h"
#include "storage_adaptor.h"
#include "storage_adaptor_factory.h"
#include "model_controller.h"
#include "context_menu_data_collection_row.h"

namespace QuickieWebBot
{

NavigationPanelWidget::NavigationPanelWidget(QWidget* parent)
	: QWidget(parent)
	, m_prevSelectedButton(nullptr)
{
	init();
}

bool NavigationPanelWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() != QEvent::MouseButtonPress)
	{
		return false;
	}

	if (QPushButton* button = qobject_cast<QPushButton*>(watched))
	{
		if (m_prevSelectedButton == button)
		{
			return false;
		}

		if (button->property("childElement").toBool() == false)
		{
			if (button == siteStructureButton)
			{
				siteStructurePanel->setVisible(!siteStructurePanel->isVisible());
				return false;
			}
		}

		if (m_prevSelectedButton)
		{
			m_prevSelectedButton->setProperty("selected", false);
			m_prevSelectedButton->repaint();
			m_prevSelectedButton->style()->unpolish(m_prevSelectedButton);
			m_prevSelectedButton->style()->polish(m_prevSelectedButton);
		}

		m_prevSelectedButton = button;
		m_prevSelectedButton->setProperty("selected", true);
		m_prevSelectedButton->style()->unpolish(m_prevSelectedButton);
		m_prevSelectedButton->style()->polish(m_prevSelectedButton);

		int index = 1;

		if (m_prevSelectedButton == allPagesButton)
		{
			index = 0;
		}
		else if (m_prevSelectedButton == seoAnalysisButton)
		{
			index = 1;
		}
		else if (m_prevSelectedButton == allResourcesButton)
		{
			index = 2;
		}
		else if (m_prevSelectedButton == domainMetricsButton)
		{
			index = 3;
		}
		else if (m_prevSelectedButton == reportsButton)
		{
			index = 4;
		}

		mainGuiStackedWidget->setCurrentIndex(index);
	}

	return false;
}

void NavigationPanelWidget::init()
{
	setupUi(this);

	seoAnalysisButton->setProperty("selected", true);
	mainGuiStackedWidget->setCurrentIndex(1);
	m_prevSelectedButton = seoAnalysisButton;

	siteStructureButton->installEventFilter(this);
	seoAnalysisButton->installEventFilter(this);
	allPagesButton->installEventFilter(this);
	allResourcesButton->installEventFilter(this);
	domainMetricsButton->installEventFilter(this);
	reportsButton->installEventFilter(this);

	initCrawlingPage();
}

void NavigationPanelWidget::initCrawlingPage()
{
	PageInfoStorageModel* model = new PageInfoStorageModel(this);
	PageInfoStorageViewModel* modelView = new PageInfoStorageViewModel(model, this);

	StorageAdaptorFactory* storageAdaptorFactory = theApp->modelController()->data()->storageAdaptorFactory();
	StorageAdaptor* storageAdaptor = storageAdaptorFactory->create(DataCollection::CrawledUrlStorageType);

	model->setStorageAdaptor(storageAdaptor);

	crawlingGridView->setModel(model);
	crawlingGridView->setViewModel(modelView);

	crawlingGridView->setContextMenu(new ContextMenuDataCollectionRow(crawlingGridView));
}

}