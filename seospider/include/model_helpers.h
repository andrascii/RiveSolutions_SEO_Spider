#pragma once
#include "abstract_table_model.h"

namespace SeoSpider
{

inline QModelIndexList makeRowIndexes(const QModelIndex& index)
{
	if (!index.isValid() || !index.model()->rowCount())
	{
		return QModelIndexList();
	}

	QModelIndexList modelIndexes;

	const int columnCount = index.model()->columnCount();
	for (int i = 0; i < columnCount; ++i)
	{
		modelIndexes.append(index.model()->index(index.row(), i));
	}

	return modelIndexes;
}

inline QModelIndex getUnderlyingIndex(const QModelIndex& index)
{
#ifdef USE_SORTING
	if (!index.isValid())
	{
		return index;
	}

	// slow but safe: sometimes index is not mapped to source model ant it causes crash
	// sad but true: we can't just use index, it can be unmapped.
	// TODO: check it in the future version of Qt
	const QModelIndex fixedindex = index.model()->index(index.row(), index.column());

	if (!fixedindex.isValid() || fixedindex.model() == nullptr)
	{
		// again magic
		return QModelIndex();
	}

	const QModelIndex underlyingModelIndex = qvariant_cast<QModelIndex>(fixedindex.data(AbstractTableModel::underlyingIndexRole));
	if ((!underlyingModelIndex.isValid() || underlyingModelIndex.model() == nullptr) && fixedindex.model()->rowCount() > fixedindex.row())
	{
		// magic or bug it QSortProxyFilterModel
		return QModelIndex();
	}
	DEBUG_ASSERT(underlyingModelIndex.isValid());
	return underlyingModelIndex;
#else
	return index;
#endif
}

template <class T, bool CheckModelNotNull = true>
T getUnderlyingModelByIndex(const QModelIndex& index)
{
#ifdef USE_SORTING
	QModelIndex underlyingModelIndex = getUnderlyingIndex(index);
	if (!underlyingModelIndex.isValid())
	{
		return nullptr;
	}

	QAbstractItemModel* model = const_cast<QAbstractItemModel*>(underlyingModelIndex.model());
	T underlyingModel = qobject_cast<T>(model);
#else
	QAbstractItemModel* model = const_cast<QAbstractItemModel*>(index.model());
	T underlyingModel = qobject_cast<T>(model);
#endif
	DEBUG_ASSERT(!CheckModelNotNull || underlyingModel);
	return underlyingModel;
}

template <class T>
T getUnderlyingModel(QAbstractItemModel* model)
{
#ifdef USE_SORTING
	if (model == nullptr)
	{
		return nullptr;
	}

	const QModelIndex index = model->index(0, 0);

	if (index.isValid())
	{
		return getUnderlyingModelByIndex<T>(index);
	}

	QSortFilterProxyModel* proxyModel = qobject_cast<QSortFilterProxyModel*>(model);
	if (proxyModel == nullptr)
	{
		return qobject_cast<T>(model);
	}

	return qobject_cast<T>(proxyModel->sourceModel());
#else
	return qobject_cast<T>(const_cast<QAbstractItemModel*>(model));
#endif
}

}
