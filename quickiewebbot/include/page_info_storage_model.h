#pragma once

#include "igrid_model.h"
#include "page_info.h"
#include "data_collection.h"

namespace QuickieWebBot
{

class IGridViewResizePolicy;
class GridViewResizePolicy;
class StorageAdaptor;

//
//! This model intended for establish associate with some PageInfo storage 
//! which placed in the DataCollection instance.
//! Using this model you can customize PageInfo columns set.
//

class PageInfoStorageModel : public IGridModel
{
	Q_OBJECT

	//
	// If you want to add a model for only one PageInfo then you need
	// to create a new derived type from IGridModel and customize it appropriately.
	//

public:
	PageInfoStorageModel(QObject* parent = nullptr);

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	void setStorageAdaptor(StorageAdaptor* storageAdaptor) noexcept;

private:
	const StorageAdaptor* storageAdaptor() const;
	StorageAdaptor* storageAdaptor();

private slots:
	void onPageInfoDataAdded(int rowIndex);
	void onPageInfoItemChanged(int row, int column);

private:
	StorageAdaptor* m_storageAdaptor;
};

}
