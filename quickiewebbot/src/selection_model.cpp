#include "selection_model.h"
#include "table_view.h"
#include "abstract_table_model.h"
#include "quickie_web_bot_helpers.h"

#ifdef QT_DEBUG
#include "debug_info_web_page_widget.h"
#include "page_model.h"
#include "parsed_page_info_storage_adaptor.h"
#endif // DEBUG


namespace QuickieWebBot
{

SelectionModel::SelectionModel(TableView* parent)
	: QItemSelectionModel(parent->model())
	, m_view(parent)
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

	QItemSelectionModel::select(fixedSelection, command);

#ifdef QT_DEBUG

	QModelIndex firstSelectedIndex = fixedSelection.indexes().first();

	const PageModel* storageModel = dynamic_cast<const PageModel*>(firstSelectedIndex.model());

	if (!storageModel)
	{
		return;
	}

	if (const ParsedPageInfoStorageAdaptor* storageAdaptor = 
		dynamic_cast<const ParsedPageInfoStorageAdaptor*>(storageModel->storageAdaptor()); storageAdaptor)
	{
		WebCrawler::ParsedPage* pageRaw = storageAdaptor->parsedPage(firstSelectedIndex);
		GlobalWebPageSelectedNotifier::instanse()->pageSelected(pageRaw);
	}

#endif
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