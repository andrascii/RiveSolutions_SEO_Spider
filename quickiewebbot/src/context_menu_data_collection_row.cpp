#include "context_menu_data_collection_row.h"
#include "itable_model.h"
#include "table_view.h"
#include "quickie_web_bot_helpers.h"
#include "page_info_storage_model.h"

namespace QuickieWebBot
{

ContextMenuDataCollectionRow::ContextMenuDataCollectionRow(const TableView* associatedView, QWidget* parent)
	: QMenu(parent)
	, m_associatedView(associatedView)
{
	struct ActionDescriptor
	{
		int action;
		QString description;
		QIcon icon;
	};

	ActionDescriptor descriptors[] = 
	{
		{ OpenUrlActionType, tr("Open URL"), QIcon() },
		{ RefreshAllResultsActionType, tr("Refresh All Results"), QIcon() },
		{ RemoveRowActionType, tr("Remove Row"), QIcon() },
		{ CheckIndexActionType, tr("Check Index"), QIcon() },
		{ CheckGoogleActionType, tr("Check Google Cache"), QIcon() },
		{ OpenRobotsTxtActionType, tr("Open Robots.txt"), QIcon() },
		{ ShowOtherDomainsOnIpActionType, tr("Show Other Domains On IP"), QIcon() },
		{ CheckHtmlActionType, tr("Check HTML With W3C Validator"), QIcon() },
		{ CopyToClipboardAllColumnsActionType, tr("Copy to Clipboard All Columns Data"), QIcon() },
		{ CopyToClipboardAllPagesActionType, tr("Copy to Clipboard All Pages"), QIcon() },
		{ CopyToClipboardUrlActionType, tr("Copy to Clipboard URL"), QIcon() }
	};

	for (const auto& [actionType, description, icon] : descriptors)
	{
		m_actions[actionType] = new QAction(icon, description, this);
		m_actions[actionType]->setData(actionType);
	}

	QMenu* copySubMenu = new QMenu("Copy to Clipboard", this);
	copySubMenu->addAction(m_actions[CopyToClipboardAllColumnsActionType]);
	copySubMenu->addAction(m_actions[CopyToClipboardAllPagesActionType]);
	copySubMenu->addAction(m_actions[CopyToClipboardUrlActionType]);

	addAction(m_actions[OpenUrlActionType]);
	addAction(m_actions[RefreshAllResultsActionType]);
	addAction(m_actions[RemoveRowActionType]);
	addSeparator();
	addAction(m_actions[CheckIndexActionType]);
	addAction(m_actions[CheckGoogleActionType]);
	addAction(m_actions[OpenRobotsTxtActionType]);
	addAction(m_actions[ShowOtherDomainsOnIpActionType]);
	addAction(m_actions[CheckHtmlActionType]);
	addMenu(copySubMenu);

	VERIFY(QObject::connect(m_actions[OpenUrlActionType], &QAction::triggered, 
		this, &ContextMenuDataCollectionRow::openUrlAction));

	VERIFY(QObject::connect(m_actions[CopyToClipboardUrlActionType], &QAction::triggered, 
		this, &ContextMenuDataCollectionRow::copyToClipboardUrl));

	VERIFY(QObject::connect(m_actions[CopyToClipboardAllColumnsActionType], &QAction::triggered, 
		this, &ContextMenuDataCollectionRow::copyToClipboardAllColumnsData));
}

QModelIndexList ContextMenuDataCollectionRow::selectedIndexes() const noexcept
{
	QItemSelectionModel* associatedGridViewSelecionModel = m_associatedView->selectionModel();
	return associatedGridViewSelecionModel->selectedIndexes();
}

void ContextMenuDataCollectionRow::openUrlAction()
{
	for (QModelIndex index : selectedIndexes())
	{
		const PageInfoStorageModel* model = 
			QuickieWebBotHelpers::safe_runtime_static_cast<const PageInfoStorageModel*>(index.model());

		if (model->itemType(index) != PageInfo::UrlItemType)
		{
			continue;
		}

		QDesktopServices::openUrl(index.data(Qt::DisplayRole).toUrl());
	}
}

void ContextMenuDataCollectionRow::copyToClipboardAllColumnsData()
{
	QString allColumnsData;

	foreach(QModelIndex index, selectedIndexes())
	{
		allColumnsData += index.data(Qt::DisplayRole).toString() + "\n";
	}

	theApp->clipboard()->setText(allColumnsData);
}

void ContextMenuDataCollectionRow::copyToClipboardAllPages()
{

}

void ContextMenuDataCollectionRow::copyToClipboardUrl()
{
	foreach(QModelIndex index, selectedIndexes())
	{
		const PageInfoStorageModel* model = 
			QuickieWebBotHelpers::safe_runtime_static_cast<const PageInfoStorageModel*>(index.model());

		if (model->itemType(index) != PageInfo::UrlItemType)
		{
			continue;
		}

		theApp->clipboard()->setText(index.data(Qt::DisplayRole).toString() + " ");
	}
}

}