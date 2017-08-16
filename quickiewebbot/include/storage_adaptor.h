#pragma once

#include "page_info.h"
#include "data_collection.h"

namespace QuickieWebBot
{

class StorageAdaptor : public QObject
{
	Q_OBJECT

public:
	StorageAdaptor(const WebCrawler::DataCollection::GuiStorageTypePtr& associatedStorage,
		WebCrawler::DataCollection::StorageType storageType,
		QObject* parent = nullptr);

	virtual void setAvailableColumns(QList<PageInfo::ItemType> availableColumns) noexcept;
	virtual QList<PageInfo::ItemType> availableColumns() const noexcept;
	virtual QString columnDescription(int columnIndex) const noexcept;

	virtual int itemCount() const noexcept;
	virtual QVariant itemAt(const QModelIndex& index) const noexcept;
	virtual PageInfo::ItemType itemTypeAt(const QModelIndex& index) const noexcept;

signals:
	void pageInfoAdded(int rowIndex);

private slots:
	void onStorageUpdated(int row, int type);

private:
	WebCrawler::DataCollection::GuiStorageTypePtr m_associatedStorage;
	QList<PageInfo::ItemType> m_availableColumns;
	WebCrawler::DataCollection::StorageType m_storageType;
};

}