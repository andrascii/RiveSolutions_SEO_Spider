#include "grid_view_selection_model.h"
#include "grid_view.h"
#include "igrid_data_accessor.h"
#include "grid_model.h"

namespace QuickieWebBot
{

GridViewSelectionModel::GridViewSelectionModel(GridView* parent)
	: QItemSelectionModel(parent->model())
	, m_gridView(parent)
{
}

void GridViewSelectionModel::select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
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
}

void GridViewSelectionModel::setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
	if (!(index.flags() & Qt::ItemIsSelectable))
	{
		return;
	}

	QItemSelectionModel::setCurrentIndex(index, command);
}

}