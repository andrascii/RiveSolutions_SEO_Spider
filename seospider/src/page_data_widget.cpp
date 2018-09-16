#include "page_data_widget.h"
#include "table_view.h"
#include "page_model.h"
#include "page_view_model.h"
#include "application.h"
#include "storage_adapter_factory.h"
#include "helpers.h"
#include "cursor_factory.h"

namespace SeoSpider
{

PageDataWidget::PageDataWidget(QWidget* parent)
	: QFrame(parent)
	, m_tabWidget(new QTabWidget(this))
	, m_httpResponseLabel(new QTextEdit(this))
{
	m_tabWidget->tabBar()->setCursor(CursorFactory::createCursor(Qt::PointingHandCursor));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(m_tabWidget);
	m_httpResponseLabel->setReadOnly(true);

	VERIFY(connect(m_tabWidget, &QTabWidget::tabBarClicked, this, &PageDataWidget::onTabBarClicked));
}

void PageDataWidget::setParsedPageInfo(const ParsedPageInfoPtr& page)
{
	StorageAdapterFactory* factory = theApp->storageAdapterFactory();

	for(auto beg = m_models.begin(); beg != m_models.end(); ++beg)
	{
		beg.value()->setStorageAdapter(factory->createPageLinksStorage(mapType(beg.key()), page));
	}

	setPageServerResponse(page);
}

void PageDataWidget::setPageDataType(PageDataType pageDataType)
{
	DEBUG_ASSERT(pageDataType > BeginType && pageDataType < EndType);

	QLabel* selectPageLabel = new QLabel(this);
	selectPageLabel->setObjectName(QStringLiteral("TablePlaseholderLabel"));
	selectPageLabel->setText(tr("Select The Page"));
	selectPageLabel->setAlignment(Qt::AlignCenter);
	QStackedWidget* stackedWidget = new QStackedWidget(this);
	stackedWidget->addWidget(selectPageLabel);

	if (pageDataType == ServerResponseForPageType)
	{
		m_pageIndices[pageDataType] = m_tabWidget->addTab(stackedWidget, tabDescription(pageDataType));
		stackedWidget->addWidget(m_httpResponseLabel);
		stackedWidget->setCurrentIndex(0);
		m_stackedWidgets[pageDataType] = stackedWidget;
		return;
	}

	TableView* tableView = new TableView(stackedWidget, false, true);
	m_models[pageDataType] = new PageModel(this);

	tableView->setModel(m_models[pageDataType]);
	tableView->setViewModel(new PageViewModel(tableView, m_models[pageDataType], this));
	tableView->setShowAdditionalGrid(true);

	stackedWidget->addWidget(tableView);
	stackedWidget->setCurrentIndex(0);

	m_pageIndices[pageDataType] = m_tabWidget->addTab(stackedWidget, tabDescription(pageDataType));
	m_stackedWidgets[pageDataType] = stackedWidget;
}

void PageDataWidget::selectTab(PageDataType pageDataType)
{
	ASSERT(pageDataType > BeginType && pageDataType < EndType);
	ASSERT(m_pageIndices.find(pageDataType) != m_pageIndices.end());

	m_tabWidget->setCurrentIndex(m_pageIndices[pageDataType]);
	emit tabSelected(m_pageIndices[pageDataType]);
}

QTabWidget* PageDataWidget::tabWidget()
{
	return m_tabWidget;
}

void PageDataWidget::onTabBarClicked(int index)
{
	emit tabBarClicked(index, m_tabWidget->currentIndex());
}

void PageDataWidget::setPageServerResponse(const ParsedPageInfoPtr& page) const
{
	m_httpResponseLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QString selectedPageServerResponse = page->itemValue(ParsedPageInfo::Column::ServerResponseColumn).toString().trimmed();

	if (selectedPageServerResponse.isEmpty())
	{
		m_httpResponseLabel->setText("There is no response for this page.");
		return;
	}

	QStringList serverResponseHeaders = selectedPageServerResponse.split("\r\n");

	for (int i = 0; i < serverResponseHeaders.size(); ++i)
	{
		serverResponseHeaders[i].insert(0, "<b>");
		serverResponseHeaders[i].insert(serverResponseHeaders[i].indexOf(":") + 1, "</b>");
	}

	selectedPageServerResponse.clear();

	for (int i = 0; i < serverResponseHeaders.size(); ++i)
	{
		selectedPageServerResponse += serverResponseHeaders[i] + "<br>";
	}

	m_httpResponseLabel->setText(selectedPageServerResponse);
}

QString PageDataWidget::tabDescription(PageDataType pageDataType)
{
	switch (pageDataType)
	{
		case LinksToThisPageType:
		{
			return tr("Links To This Page");
		}
		case LinksOnThisPageType:
		{
			return tr("Links On This Page");
		}
		case ImagesOnThisPageType:
		{
			return tr("Images On This Page");
		}
		case ServerResponseForPageType:
		{
			return tr("HTTP Response");
		}
		default:
		{
			ASSERT(!"Undefined type");
		}
	}

	return QString();
}

PageLinkContext PageDataWidget::mapType(PageDataType pageDataType) noexcept
{
	switch (pageDataType)
	{
		case LinksToThisPageType:
		{
			return PageLinkContext::LinksToThisPage;
		}
		case LinksOnThisPageType:
		{
			return PageLinkContext::LinksOnThisPage;
		}
		case ImagesOnThisPageType:
		{
			return PageLinkContext::ImagesOnThisPage;
		}
		default:
		{
			ASSERT(!"Undefined type");
		}
	}

	return PageLinkContext();
}

void PageDataWidget::pageViewSelectionChangedSlot(const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(selected);
	Q_UNUSED(deselected);

	QItemSelectionModel* itemSelectionModel = qobject_cast<QItemSelectionModel*>(sender());
	DEBUG_ASSERT(itemSelectionModel);

	if(itemSelectionModel->hasSelection())
	{
		for(auto stackedWidget: m_stackedWidgets)
		{
			stackedWidget->setCurrentIndex(1);
		}

		return;
	}

	for (auto stackedWidget : m_stackedWidgets)
	{
		stackedWidget->setCurrentIndex(0);
	}
}

}
