#include "context_menu_data_collection_row.h"
#include "grid_view_model.h"
#include "imodel_data_accessor.h"

namespace QuickieWebBot
{

ContextMenuDataCollectionRow::ContextMenuDataCollectionRow(const QAbstractItemView* associatedAbstractView, QWidget* parent)
	: QMenu(parent)
	, m_associatedAbstractView(associatedAbstractView)
	, m_openUrlAction(new QAction(QIcon(), tr("Open Url"), this))
	, m_copyAction(new QAction(QIcon(), tr("Copy to Clipboard")))
	, m_refreshAllResultsAction(new QAction(QIcon(), tr("Refresh All Results"), this))
	, m_removeRowAction(new QAction(QIcon(), tr("Remove Row"), this))
	, m_checkIndexAction(new QAction(QIcon(), tr("Check Index"), this))
	, m_checkGoogleCacheAction(new QAction(QIcon(), tr("Check Google Cache"), this))
	, m_openRobotsTxtAction(new QAction(QIcon(), tr("Open robots.txt"), this))
	, m_showOtherDomainsOnIpAction(new QAction(QIcon(), tr("Show Other Domains on IP"), this))
	, m_checkHtmlAction(new QAction(QIcon(), tr("Check HTML With W3C Validator"), this))
{
	addAction(m_openUrlAction);
	addAction(m_copyAction);
	addAction(m_refreshAllResultsAction);
	addAction(m_removeRowAction);
	addSeparator();
	addAction(m_checkIndexAction);
	addAction(m_checkGoogleCacheAction);
	addAction(m_openRobotsTxtAction);
	addAction(m_showOtherDomainsOnIpAction);
	addAction(m_checkHtmlAction);

	VERIFY(QObject::connect(m_openUrlAction, &QAction::triggered, this, &ContextMenuDataCollectionRow::openUrlAction));
}

void ContextMenuDataCollectionRow::openUrlAction()
{
	QItemSelectionModel* selectionModel = m_associatedAbstractView->selectionModel();
	QModelIndexList selectedRowIndex = selectionModel->selectedRows();

	assert(dynamic_cast<GridViewModel*>(m_associatedAbstractView->model()));

	GridViewModel* gridViewModel = static_cast<GridViewModel*>(m_associatedAbstractView->model());

	foreach(const QModelIndex& modelIndex, selectedRowIndex)
	{
		int row = modelIndex.row();

		QUrl url = gridViewModel->modelDataAcessor()->itemValue(row, 0).toUrl();

		QDesktopServices::openUrl(url);
	}
}

}