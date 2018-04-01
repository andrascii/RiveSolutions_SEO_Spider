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