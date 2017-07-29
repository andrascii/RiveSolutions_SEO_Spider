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

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	void setModelDataAccessor(std::unique_ptr<IModelDataAccessor> accessor);

	const IModelDataAccessor* modelDataAcessor() const;
	IModelDataAccessor* modelDataAcessor();

	const IGridViewResizeStrategy* resizeStrategy() const;
	IGridViewResizeStrategy* resizeStrategy();

signals:
	void colspan(const QModelIndex& index, int span) const;
	void modelDataAccessorChanged(IModelDataAccessor* accessor, IModelDataAccessor* oldAccessor);

private slots:
	void onRowAdded(int row);
	void onItemChanged(int row, int column);

private:
	std::unique_ptr<IModelDataAccessor> m_accessor;
};

}
