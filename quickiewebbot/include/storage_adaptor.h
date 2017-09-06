#pragma once

#include "page_raw_info.h"
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

	virtual void setAvailableColumns(QList<PageRawInfo::ItemType> availableColumns) noexcept;
	virtual QList<PageRawInfo::ItemType> availableColumns() const noexcept;
	virtual QString columnDescription(int columnIndex) const noexcept;

	virtual int itemCount() const noexcept;
	virtual QVariant item(const QModelIndex& index) const noexcept;
	virtual PageRawInfo::ItemType itemType(const QModelIndex& index) const noexcept;

#ifdef QT_DEBUG
	WebCrawler::PageRaw* pageRaw(const QModelIndex& index) const noexcept;
#endif // DEBUG


signals:
	void pageInfoAdded(int rowIndex);

private slots:
	void onStorageUpdated(int row, int type);

private:
	WebCrawler::DataCollection::GuiStorageTypePtr m_associatedStorage;
	QList<PageRawInfo::ItemType> m_availableColumns;
	WebCrawler::DataCollection::StorageType m_storageType;
};

}