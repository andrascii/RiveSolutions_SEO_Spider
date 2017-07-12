#pragma once

#include "imodel_data_accessor.h"

namespace QuickieWebBot
{

class GridViewModel 
	: public QAbstractTableModel
{
	Q_OBJECT

public:
	GridViewModel(QObject* parent = nullptr);

	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const override;
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const override;
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;
	virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex()) override;

	virtual bool insertColumns(int column, int count, QModelIndex const& parent = QModelIndex()) override;
	virtual bool removeColumns(int column, int count, QModelIndex const& parent = QModelIndex()) override;

	void setDataAccessor(std::unique_ptr<IModelDataAccessor> accessor);
	IModelDataAccessor* dataAcessor() const;

	Q_SIGNAL void colspan(const QModelIndex& index, int span) const;
	Q_SIGNAL void modelAccessorChanged(IModelDataAccessor* accessor);

private:
	Q_SLOT void onRowAdded(int row);

private:
	std::unique_ptr<IModelDataAccessor> m_accessor;
};

}
