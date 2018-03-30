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

	const QModelIndex index = selected.size() ? selected.indexes()[0] : QModelIndex();

	m_dataAccessor->selectRow(index.row());
}

void FilterTableSelectionModel::onSummaryDataAccessorSelectionChanged(int row)
{
	select(model()->index(row, 0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
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