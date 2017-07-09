#pragma once

#include "imodel_data_accessor.h"
#include "data_collection.h"

namespace QuickieWebBot
{

class ModelDataAccessorAllItems 
	: public QObject
	, public ModelDataAccessorItemBase
{
	Q_OBJECT

public:
	ModelDataAccessorAllItems(DataCollection::StorageType storageType);
	virtual int columnCount() const override;
	virtual QString columnText(int column) const override;

	virtual int rowCount() const override;
	virtual QVariant itemValue(const QModelIndex& index) const override;
	virtual QColor itemBackgroundColor(const QModelIndex& index) const override;
	virtual int itemColSpan(const QModelIndex& index) const override;
	virtual int flags(const QModelIndex& index) const override;

	virtual QAbstractItemDelegate* itemDelegate(const QModelIndex& index) const override;
	virtual QPixmap* pixmap(const QModelIndex& index) const override;

	virtual QObject* qobject() override;

	// signals
	Q_SIGNAL void itemChanged(QModelIndex index);
	Q_SIGNAL void rowRemoved(int row);
	Q_SIGNAL void rowAdded(int row);
	Q_SIGNAL void reset();

protected:
	Q_SLOT void onModelDataRowAdded(int row, int type);

private:
	const DataCollection* m_modelControllerData;
	DataCollection::StorageType m_storageType;
	std::vector<int> m_columns;
};

}