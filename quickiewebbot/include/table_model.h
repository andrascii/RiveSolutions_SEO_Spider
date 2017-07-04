#pragma once

#include "imodel_data_accessor.h"

namespace QuickieWebBot
{

class TableModel 
	: public QAbstractTableModel
{
	Q_OBJECT

public:
	TableModel(QObject* parent = nullptr);

	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const override;
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const override;
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;
	virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

	virtual bool insertColumns(int column, int count, QModelIndex const& parent = QModelIndex()) override;
	virtual bool removeColumns(int column, int count, QModelIndex const& parent = QModelIndex()) override;

	void setDataAccessor(std::unique_ptr<IModelDataAccessorItem> accessor);

private:
	Q_SLOT void onRowAdded(int row);

private:
	std::unique_ptr<IModelDataAccessorItem> m_accessor;
};

}
