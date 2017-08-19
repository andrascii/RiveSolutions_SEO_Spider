#pragma once

#include "itable_model.h"
#include "page_info.h"
#include "data_collection.h"

namespace QuickieWebBot
{

class IResizePolicy;
class StorageAdaptor;

//
//! This model intended for establish associate with some PageInfo storage 
//! which placed in the DataCollection instance.
//! Using this model you can customize PageInfo columns set.
//

class PageInfoStorageModel : public ITableModel
{
	Q_OBJECT

public:
	PageInfoStorageModel(QObject* parent = nullptr);

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	// ITableView
	virtual IResizePolicy* resizePolicy() const noexcept override;

	void setStorageAdaptor(StorageAdaptor* storageAdaptor) noexcept;
	PageInfo::ItemType itemType(const QModelIndex& index) const noexcept;

signals:
	virtual void internalDataChanged() override;

private:
	const StorageAdaptor* storageAdaptor() const;
	StorageAdaptor* storageAdaptor();

private slots:
	void onPageInfoAdded(int rowIndex);
	void onPageInfoItemChanged(int row, int column);

private:
	StorageAdaptor* m_storageAdaptor;
	std::shared_ptr<IResizePolicy> m_resizePolicy;
};

}
