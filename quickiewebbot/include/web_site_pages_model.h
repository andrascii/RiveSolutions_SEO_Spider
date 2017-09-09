#pragma once

#include "abstract_table_model.h"
#include "page_raw_info.h"
#include "data_collection.h"

namespace QuickieWebBot
{

class IResizePolicy;
class IStorageAdaptor;

class WebSitePagesModel : public AbstractTableModel
{
	Q_OBJECT

public:
	WebSitePagesModel(QObject* parent = nullptr);

	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	virtual IResizePolicy* resizePolicy() const noexcept override;

	void setStorageAdaptor(IStorageAdaptor* storageAdaptor) noexcept;
	const IStorageAdaptor* storageAdaptor() const;
	IStorageAdaptor* storageAdaptor();
	PageRawInfo::Column itemType(const QModelIndex& index) const noexcept;
	
	Q_SIGNAL virtual void internalDataChanged() override;

private slots:
	void onPageRawInfoAdded(int rowIndex);
	void onPageInfoItemChanged(int row, int column);

private:
	IStorageAdaptor* m_storageAdaptor;
	std::shared_ptr<IResizePolicy> m_resizePolicy;
};

}