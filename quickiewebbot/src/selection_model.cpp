#include "selection_model.h"
#include "table_view.h"
#include "itable_model.h"

#ifdef QT_DEBUG
#include "debug_info_web_page_widget.h"
#include "page_info_storage_model.h"
#include "storage_adaptor.h"
#endif // DEBUG


namespace QuickieWebBot
{

SelectionModel::SelectionModel(TableView* parent)
	: QItemSelectionModel(parent->model())
	, m_gridView(parent)
{
}

void SelectionModel::select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
	QItemSelectionModel::select(index, command);
}

void SelectionModel::select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command)
{
	if (!command.testFlag(QItemSelectionModel::Select))
	{
		QItemSelectionModel::select(selection, command);
	}

	// TODO: optimize
	QItemSelection fixedSelection;
	 
	QModelIndex topLeft;
	QModelIndex bottomRight;

	bool prevIndexSelectable = false;

	for (auto rangeIt = std::cbegin(selection); rangeIt != std::cend(selection); ++rangeIt)
	{
		if (prevIndexSelectable)
		{
			fixedSelection.append(QItemSelectionRange(topLeft, bottomRight));
		}

		QModelIndexList indexes = rangeIt->indexes();

		for (auto it = indexes.cbegin(); it < indexes.cend(); ++it)
		{
			if (!(it->flags() & Qt::ItemIsSelectable))
			{
				if (prevIndexSelectable)
				{
					fixedSelection.append(QItemSelectionRange(topLeft, bottomRight));
				}

				prevIndexSelectable = false;
				continue;
			}

			if (!prevIndexSelectable)
			{
				topLeft = *it;
			}

			bottomRight = *it;
			prevIndexSelectable = true;
		}
	}

	if (prevIndexSelectable)
	{
		fixedSelection.append(QItemSelectionRange(topLeft, bottomRight));
	}

	if (fixedSelection.empty())
	{
		return;
	}

#ifdef QT_DEBUG
	QModelIndex firstSelectedIndex = fixedSelection.indexes().first();
	const PageInfoStorageModel* storageModel = dynamic_cast<const PageInfoStorageModel*>(firstSelectedIndex.model());
	if (storageModel != nullptr)
	{
		WebCrawler::PageRaw* pageRaw = storageModel->storageAdaptor()->pageRaw(firstSelectedIndex);
		GlobalWebPageSelectedNotifier::instanse()->pageSelected(pageRaw);
	}
#endif // DEBUG


	QItemSelectionModel::select(fixedSelection, command);
}

void SelectionModel::setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
	if (!(index.flags() & Qt::ItemIsSelectable))
	{
		return;
	}

	QItemSelectionModel::setCurrentIndex(index, command);
}

}