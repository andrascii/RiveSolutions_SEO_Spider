#include "filter_table_selection_model.h"
#include "isummary_data_accessor.h"
#include "summary_model.h"

namespace SeoSpider
{

FilterTableSelectionModel::FilterTableSelectionModel(QAbstractItemModel* model)
	: QItemSelectionModel(model)
	, m_dataAccessor(nullptr)
{
	init();
}

FilterTableSelectionModel::FilterTableSelectionModel(QAbstractItemModel* model, QObject* parent)
	: QItemSelectionModel(model, parent)
	, m_dataAccessor(nullptr)
{
	init();
}

void FilterTableSelectionModel::onAboutSelectionChanged(const QItemSelection& selected, const QItemSelection&)
{
	if (!m_dataAccessor)
	{
		return;
	}

	const QModelIndexList indexes = selected.indexes();
	const QModelIndex index = !indexes.isEmpty() ? indexes[0] : QModelIndex();

	const auto uniqueRowNumberPredicate = [index](const QModelIndex& modelIndex)
	{
		return modelIndex.row() == index.row();
	};

	ASSERT(std::all_of(indexes.begin(), indexes.end(), uniqueRowNumberPredicate));

	if (index.row() != m_dataAccessor->selectedRow())
	{
		const QSignalBlocker signalBlocker(m_dataAccessor->qobject());
		m_dataAccessor->selectRow(index.row());
	}
}

void FilterTableSelectionModel::onSummaryDataAccessorSelectionChanged(int row)
{
	if (row == -1)
	{
		clearSelection();
	}
	else
	{
		select(model()->index(row, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
	}
}

void FilterTableSelectionModel::select(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
	QItemSelectionModel::select(index, command);
}

void FilterTableSelectionModel::select(const QItemSelection& selection, QItemSelectionModel::SelectionFlags command)
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

void FilterTableSelectionModel::setCurrentIndex(const QModelIndex& index, QItemSelectionModel::SelectionFlags command)
{
	if (!(index.flags() & Qt::ItemIsSelectable))
	{
		return;
	}

	QItemSelectionModel::setCurrentIndex(index, command);
}

void FilterTableSelectionModel::init()
{
	SummaryModel* summaryModel = dynamic_cast<SummaryModel*>(model());

	DEBUG_ASSERT(summaryModel);

	if (!summaryModel)
	{
		return;
	}

	m_dataAccessor = summaryModel->dataAccessor();

	VERIFY(connect(this, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this, SLOT(onAboutSelectionChanged(const QItemSelection&, const QItemSelection&))));

	VERIFY(connect(m_dataAccessor->qobject(), SIGNAL(rowSelected(int)), this, SLOT(onSummaryDataAccessorSelectionChanged(int))));
}

}