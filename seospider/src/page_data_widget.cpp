#include "page_data_widget.h"
#include "table_view.h"
#include "page_model.h"
#include "page_view_model.h"
#include "application.h"
#include "storage_adapter_factory.h"
#include "helpers.h"
#include "cursor_factory.h"
#include "resource_type_filter_widget.h"
#include "table_proxy_model.h"

namespace
{
class MinStackedWidget: public QStackedWidget
{
public:
	MinStackedWidget(QWidget* parent = nullptr)
		: QStackedWidget(parent)
	{
	}

protected:
	virtual QSize minimumSizeHint() const override
	{
		QSize result = QStackedWidget::minimumSizeHint();
		result.setHeight(0);
		return result;
	}
};
}

namespace SeoSpider
{

PageDataWidget::PageDataWidget(QWidget* parent)
	: QFrame(parent)
	, m_httpResponseLabel(new QTextEdit(this))
	, m_stackedWidget(new MinStackedWidget(this))
	, m_tabBar(new QTabBar(this))
{
	m_tabBar->setObjectName("pageInfoTabBar");
	m_tabBar->setContentsMargins(0, 0, 0, 0);

	m_stackedWidget->setContentsMargins(0, 0, 0, 0);
	m_stackedWidget->setMinimumHeight(0);
	m_stackedWidget->setObjectName("pageInfoBody");
	m_tabBar->setCursor(CursorFactory::createCursor(Qt::PointingHandCursor));

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->setSpacing(0);
	vLayout->setMargin(0);
	setLayout(vLayout);

	QFrame* topWidget = new QFrame(this);
	topWidget->setContentsMargins(Common::Helpers::pointsToPixels(2), 0, Common::Helpers::pointsToPixels(3), 0);
	vLayout->addWidget(topWidget);
	vLayout->addWidget(m_stackedWidget);

	m_topWidgetLayout = new QHBoxLayout(this);
	m_topWidgetLayout->setSpacing(0);
	m_topWidgetLayout->setMargin(0);
	topWidget->setLayout(m_topWidgetLayout);

	m_topWidgetLayout->addWidget(m_tabBar);
	m_topWidgetLayout->addStretch();
	m_httpResponseLabel->setReadOnly(true);

	VERIFY(connect(m_tabBar, &QTabBar::tabBarClicked, this, &PageDataWidget::onTabBarClicked));
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
	selectPageLabel->setContentsMargins(0, 0, 0, 0);
	selectPageLabel->setObjectName(QStringLiteral("TablePlaseholderLabel"));
	selectPageLabel->setText(tr("Select The Page"));
	selectPageLabel->setAlignment(Qt::AlignCenter);
	selectPageLabel->setContentsMargins(0, 0, 0, 0);
	QStackedWidget* tabStackedWidget = new QStackedWidget(this);
	tabStackedWidget->setContentsMargins(0, 0, 0, 0);
	tabStackedWidget->setMinimumHeight(0);
	tabStackedWidget->setObjectName("tabContent");
	tabStackedWidget->addWidget(selectPageLabel);
	m_stackedWidget->setMinimumHeight(0);

	tabStackedWidget->installEventFilter(this);

	if (pageDataType == ServerResponseForPageType)
	{
		m_pageIndices[pageDataType] = m_tabBar->addTab(tabDescription(pageDataType));
		m_stackedWidget->addWidget(tabStackedWidget);
		tabStackedWidget->addWidget(m_httpResponseLabel);
		tabStackedWidget->setCurrentIndex(0);
		m_stackedWidgets[pageDataType] = tabStackedWidget;
		m_filterResourceTypeWidgets[tabStackedWidget] = nullptr;
		return;
	}

	TableView* tableView = new TableView(tabStackedWidget, false, true);
	tableView->setObjectName("TableView");
	tableView->setContentsMargins(0, 0, 0, 0);
	tableView->setMinimumWidth(0);
	m_models[pageDataType] = new PageModel(this);
	ResourceTypeFilterWidget* filterWidget = new ResourceTypeFilterWidget(this);
	filterWidget->setContentsMargins(0, 0, 0, 0);
	tableView->setMinimumHeight(0);
	m_filterResourceTypeWidgets[tabStackedWidget] = filterWidget;
	m_topWidgetLayout->addWidget(filterWidget);

	VERIFY(connect(filterWidget, &ResourceTypeFilterWidget::filterChanged, tableView, [tableView](int filter)
	{
		TableProxyModel* filterProxyModel =
			qobject_cast<TableProxyModel*>(tableView->model());

		ASSERT(filterProxyModel);
		filterProxyModel->setAcceptedResources(filter);
	}));

	tableView->setModel(m_models[pageDataType]);
	tableView->setViewModel(new PageViewModel(tableView, m_models[pageDataType], this));
	tableView->setShowAdditionalGrid(true);

	tabStackedWidget->addWidget(tableView);
	tabStackedWidget->setCurrentIndex(0);


	m_pageIndices[pageDataType] = m_tabBar->addTab(tabDescription(pageDataType));
	m_stackedWidget->addWidget(tabStackedWidget);
	m_stackedWidgets[pageDataType] = tabStackedWidget;
}

void PageDataWidget::selectTab(PageDataType pageDataType)
{
	ASSERT(pageDataType > BeginType && pageDataType < EndType);
	ASSERT(m_pageIndices.find(pageDataType) != m_pageIndices.end());

	m_tabBar->setCurrentIndex(m_pageIndices[pageDataType]);
	m_stackedWidget->setCurrentIndex(m_pageIndices[pageDataType]);
	emit tabSelected(m_pageIndices[pageDataType]);
}

QTabBar* PageDataWidget::tabWidget()
{
	return m_tabBar;
}

void PageDataWidget::onTabBarClicked(int index)
{
	m_stackedWidget->setCurrentIndex(index);
	emit tabBarClicked(index, index);
}

bool PageDataWidget::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::Show)
	{
		foreach (QWidget* tableView, m_filterResourceTypeWidgets.keys())
		{
			QWidget* filterWidget = m_filterResourceTypeWidgets[tableView];
			if (filterWidget != nullptr)
			{
				filterWidget->setVisible(tableView == object);
			}
		}
	}

	return false;
}

void PageDataWidget::setPageServerResponse(const ParsedPageInfoPtr& page) const
{
	m_httpResponseLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	m_httpResponseLabel->setContentsMargins(0, 0, 0, 0);

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

		const int endBoldPosition = serverResponseHeaders[i].indexOf(":") + 1;

		if (endBoldPosition != 0)
		{
			serverResponseHeaders[i].insert(endBoldPosition, "</b>");
		}
		else
		{
			serverResponseHeaders[i].append("</b>");
		}
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
