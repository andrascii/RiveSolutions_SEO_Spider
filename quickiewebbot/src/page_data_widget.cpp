#include "page_data_widget.h"
#include "table_view.h"
#include "page_model.h"
#include "page_view_model.h"
#include "application.h"

namespace QuickieWebBot
{

PageDataWidget::PageDataWidget(QWidget* parent)
	: QFrame(parent)
	, m_tabWidget(new QTabWidget(this))
	, m_httpResponseLabel(new QLabel(this))
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(m_tabWidget);
}

void PageDataWidget::setPageRawInfo(const ParsedPageInfoPtr& page)
{
	StorageAdaptorFactory* factory = theApp->storageAdaptorFactory();

	for(auto beg = m_models.begin(); beg != m_models.end(); ++beg)
	{
		beg.value()->setStorageAdaptor(factory->createPageLinksStorage(mapType(beg.key()), page));
	}

	setPageServerResponse(page);
}

void PageDataWidget::setPageDataType(PageDataType pageDataType)
{
	DEBUG_ASSERT(pageDataType > BeginType && pageDataType < EndType);

	if (pageDataType == ServerResponseForPageType)
	{
		m_tabWidget->addTab(m_httpResponseLabel, tabDescription(pageDataType));
		return;
	}

	TableView* tableView = new TableView(this);
	m_models[pageDataType] = new PageModel(this);

	tableView->setModel(m_models[pageDataType]);
	tableView->setViewModel(new PageViewModel(m_models[pageDataType], this));
	tableView->setShowAdditionalGrid(true);

	m_tabWidget->addTab(tableView, tabDescription(pageDataType));
}

void PageDataWidget::setPageServerResponse(const ParsedPageInfoPtr& page) const
{
	m_httpResponseLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	QString selectedPageServerResponse = page->itemValue(ParsedPageInfo::Column::ServerResponseColumn).toString().trimmed();

	if (selectedPageServerResponse.isEmpty())
	{
		QPixmap oopsPixmap(":/images/oops.jpg");

		m_httpResponseLabel->setPixmap(oopsPixmap.scaled(QSize(oopsPixmap.width() / 3, oopsPixmap.height() / 3)));
		return;
	}

	QStringList serverResponseHeaders = selectedPageServerResponse.split("\n");

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
			return tr("Images To This Page");
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

}