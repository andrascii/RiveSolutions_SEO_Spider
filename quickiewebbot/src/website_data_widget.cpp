#include "website_data_widget.h"
#include "table_view.h"
#include "page_model.h"
#include "page_view_model.h"
#include "istorage_adaptor.h"
#include "storage_adaptor_factory.h"


namespace QuickieWebBot
{

WebSiteDataWidget::WebSiteDataWidget(QWidget* parent)
	: QFrame(parent)
	, m_stackedWidget(new QStackedWidget(this))
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	layout->addWidget(m_stackedWidget);

	layout->setMargin(0);
	layout->setSpacing(0);

	QLabel* selectFilterLabel = new QLabel(this);
	selectFilterLabel->setObjectName(QStringLiteral("SelectFilterLabel"));
	selectFilterLabel->setText(tr("Select The Filter"));
	selectFilterLabel->setAlignment(Qt::AlignCenter);

	m_tables[StorageAdaptorType::StorageAdaptorTypeNone] = m_stackedWidget->addWidget(selectFilterLabel);
	m_stackedWidget->setCurrentIndex(m_tables[StorageAdaptorType::StorageAdaptorTypeNone]);
}

void WebSiteDataWidget::setStorageAdaptorType(StorageAdaptorType storageAdaptorType)
{
	auto tableIndexIterator = m_tables.find(storageAdaptorType);

	if (tableIndexIterator != m_tables.end())
	{
		m_stackedWidget->setCurrentIndex(tableIndexIterator.value());
		return;
	}

	StorageAdaptorFactory* factory = theApp->storageAdaptorFactory();

	PageModel* pageModel = new PageModel(m_stackedWidget);
	pageModel->setStorageAdaptor(factory->createPageRawInfoStorage(storageAdaptorType, theApp->guiStorage()));

	PageViewModel* pageViewModel = new PageViewModel(pageModel, pageModel);

	TableView* tableView = new TableView(m_stackedWidget);
	tableView->setModel(pageModel);
	tableView->setViewModel(pageViewModel);

	m_tables[storageAdaptorType] = m_stackedWidget->addWidget(tableView);
	m_stackedWidget->setCurrentIndex(m_tables[storageAdaptorType]);

	//VERIFY(connect(tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
	//	this, SIGNAL(pageViewSelectionChanged(const QItemSelection&, const QItemSelection&))));
}

}