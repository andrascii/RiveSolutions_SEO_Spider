#pragma once

#include "imodel_data_accessor.h"
#include "model_controller_data.h"

namespace QuickieWebBot
{

class ModelDataAccessorAllItems 
	: public QObject
	, public IModelDataAccessorItem
{
	Q_OBJECT
public:
	ModelDataAccessorAllItems(ModelControllerData* data, ModelControllerData::StorageType storageType);
	virtual int columnCount() const override;
	virtual QString columnText(int column) const override;

	virtual int rowCount() const override;
	virtual QVariant itemValue(const QModelIndex& index) const override;
	virtual int itemColSpan(const QModelIndex& index) const override;

	virtual QAbstractItemDelegate* itemDelegate(const QModelIndex& index) const override;

	virtual QObject* qobject() override;

	// signals
	Q_SIGNAL void itemChanged(QModelIndex index);
	Q_SIGNAL void rowRemoved(int row);
	Q_SIGNAL void rowAdded(int row);
	Q_SIGNAL void reset();

protected:
	Q_SLOT void onModelDataRowAdded(int row, int type);

private:
	ModelControllerData* m_modelControllerData;
	ModelControllerData::StorageType m_storageType;
	std::vector<int> m_columns;
};

}