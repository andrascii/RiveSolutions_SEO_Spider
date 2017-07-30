#pragma once

#include "igrid_data_accessor.h"

namespace QuickieWebBot
{

class GridModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	GridModel(QObject* parent = nullptr);

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	void setModelDataAccessor(std::unique_ptr<IGridDataAccessor> accessor);

	const IGridDataAccessor* modelDataAcessor() const;
	IGridDataAccessor* modelDataAcessor();

	const IGridViewResizeStrategy* resizeStrategy() const;
	IGridViewResizeStrategy* resizeStrategy();

signals:
	void modelDataAccessorChanged(IGridDataAccessor* accessor, IGridDataAccessor* oldAccessor);

private slots:
	void onRowAdded(int row);
	void onItemChanged(int row, int column);

private:
	std::unique_ptr<IGridDataAccessor> m_accessor;
};

}
