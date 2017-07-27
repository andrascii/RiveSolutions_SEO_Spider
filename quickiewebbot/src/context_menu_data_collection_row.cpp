#include "context_menu_data_collection_row.h"
#include "grid_view_model.h"
#include "imodel_data_accessor.h"

namespace QuickieWebBot
{

ContextMenuDataCollectionRow::ContextMenuDataCollectionRow(const GridView* associatedGridView, QWidget* parent)
	: QMenu(parent)
	, m_associatedGridView(associatedGridView)
	, m_openUrlAction(new QAction(QIcon(), tr("Open Url"), this))
	, m_refreshAllResultsAction(new QAction(QIcon(), tr("Refresh All Results"), this))
	, m_removeRowAction(new QAction(QIcon(), tr("Remove Row"), this))
	, m_checkIndexAction(new QAction(QIcon(), tr("Check Index"), this))
	, m_checkGoogleCacheAction(new QAction(QIcon(), tr("Check Google Cache"), this))
	, m_openRobotsTxtAction(new QAction(QIcon(), tr("Open robots.txt"), this))
	, m_showOtherDomainsOnIpAction(new QAction(QIcon(), tr("Show Other Domains on IP"), this))
	, m_checkHtmlAction(new QAction(QIcon(), tr("Check HTML With W3C Validator"), this))
	, m_copyToClipboardAllColumnsAction(new QAction(QIcon(), tr("Copy to Clipboard All Columns Data"), this))
	, m_copyToClipboardAllPagesAction(new QAction(QIcon(), tr("Copy to Clipboard All Pages"), this))
	, m_copyToClipboardUrlAction(new QAction(QIcon(), tr("Copy to Clipboard Url"), this))
{
	QMenu* copySubMenu = new QMenu(tr("Copy"), this);
	copySubMenu->addAction(m_copyToClipboardAllColumnsAction);
	copySubMenu->addAction(m_copyToClipboardAllPagesAction);
	copySubMenu->addAction(m_copyToClipboardUrlAction);

	addAction(m_openUrlAction);
	addMenu(copySubMenu);
	addAction(m_refreshAllResultsAction);
	addAction(m_removeRowAction);
	addSeparator();
	addAction(m_checkIndexAction);
	addAction(m_checkGoogleCacheAction);
	addAction(m_openRobotsTxtAction);
	addAction(m_showOtherDomainsOnIpAction);
	addAction(m_checkHtmlAction);

	VERIFY(QObject::connect(m_openUrlAction, &QAction::triggered, this, &ContextMenuDataCollectionRow::openUrlAction));
	VERIFY(QObject::connect(m_copyToClipboardUrlAction, &QAction::triggered, this, &ContextMenuDataCollectionRow::copyToClipboardUrl));
}

PageInfoPtr ContextMenuDataCollectionRow::pageInfoAtPopuppedPoint() const noexcept
{
	return model()->modelDataAcessor()->pageInfoAtRow(popuppedPointModelIndex().row());
}

QModelIndex ContextMenuDataCollectionRow::popuppedPointModelIndex() const noexcept
{
	QPoint popuppedPoint = pos();
	return m_associatedGridView->indexAt(popuppedPoint);
}

QModelIndexList ContextMenuDataCollectionRow::selectedRowsList() const noexcept
{
	QItemSelectionModel* associatedGridViewSelecionModel = m_associatedGridView->selectionModel();
	return associatedGridViewSelecionModel->selectedRows();
}

GridViewModel* ContextMenuDataCollectionRow::model() const noexcept
{
	assert(dynamic_cast<GridViewModel*>(m_associatedGridView->model()));
	return static_cast<GridViewModel*>(m_associatedGridView->model());
}

void ContextMenuDataCollectionRow::openUrlAction()
{
	QDesktopServices::openUrl(pageInfoAtPopuppedPoint()->itemValue(PageInfo::UrlItemType).toUrl());
}

void ContextMenuDataCollectionRow::copyToClipboardAllColumnsData()
{
	QModelIndexList selectedRows = selectedRowsList();

	foreach(QModelIndex index, selectedRows)
	{
// 		PageInfoPtr pageInfoPtr = model()->modelDataAcessor()->pageInfoAtRow(popuppedPointModelIndex().row());
// 
// 		clipboard->setText(pageInfoPtr->itemValue(PageInfo::UrlItemType).toUrl().toDisplayString());
	}
}

void ContextMenuDataCollectionRow::copyToClipboardAllPages()
{

}

void ContextMenuDataCollectionRow::copyToClipboardUrl()
{
	QModelIndexList selectedRows = selectedRowsList();
	QClipboard* clipboard = theApp->clipboard();

	foreach(QModelIndex index, selectedRows)
	{
		PageInfoPtr pageInfoPtr = model()->modelDataAcessor()->pageInfoAtRow(popuppedPointModelIndex().row());

		clipboard->setText(pageInfoPtr->itemValue(PageInfo::UrlItemType).toUrl().toDisplayString());
	}
}

}