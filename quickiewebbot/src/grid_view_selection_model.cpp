#include "grid_view_selection_model.h"
#include "grid_view.h"
#include "imodel_data_accessor.h"

namespace QuickieWebBot
{

GridViewSelectionModel::GridViewSelectionModel(GridView* parent)
	: QItemSelectionModel(parent->model())
	, m_gridView(parent)
{
}

void GridViewSelectionModel::select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
	// TODO: use command to make selection more precisely
	/*if (m_gridView->dataAccessor()->flags(index) & IModelDataAccessor::ItemFlagNotSelectable)
	{
		return;
	}*/

	QItemSelectionModel::select(index, command);
}

void GridViewSelectionModel::select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command)
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

		auto indexes = rangeIt->indexes();
		for (auto indexIt = indexes.cbegin(); indexIt < indexes.cend(); ++indexIt)
		{
			if (m_gridView->dataAccessor()->flags(*indexIt) & IModelDataAccessor::ItemFlagNotSelectable)
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
				topLeft = *indexIt;
			}

			bottomRight = *indexIt;
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
}

void GridViewSelectionModel::setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
	if (m_gridView->dataAccessor()->flags(index) & IModelDataAccessor::ItemFlagNotSelectable)
	{
		return;
	}

	QItemSelectionModel::setCurrentIndex(index, command);
}

}