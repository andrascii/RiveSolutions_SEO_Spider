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

	const QModelIndex underlyingModelIndex = qvariant_cast<QModelIndex>(index.data(AbstractTableModel::underlyingIndexRole));
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
	if (!index.isValid())
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
T getUnderlyingModel(const QAbstractItemModel* model)
{
#ifdef USE_SORTING
	if (model == nullptr)
	{
		return nullptr;
	}

	return getUnderlyingModelByIndex<T>(model->index(0, 0));
#else
	return qobject_cast<T>(const_cast<QAbstractItemModel*>(model));
#endif
}

}
