#pragma once

#include "istorage_adaptor.h"
#include "page_raw_info.h"
#include "gui_storage.h"

namespace QuickieWebBot
{

class PageRawInfoStorageAdaptor : public QObject, public IStorageAdaptor
{
	Q_OBJECT

public:
	PageRawInfoStorageAdaptor(const WebCrawler::GuiStorage::GuiStorageTypePtr& associatedStorage,
		WebCrawler::DataCollection::StorageType storageType,
		QObject* parent = nullptr);

	virtual void setAvailableColumns(QList<PageRawInfo::Column> availableColumns) noexcept override;
	virtual QList<PageRawInfo::Column> availableColumns() const noexcept override;
	virtual QString columnDescription(int columnIndex) const noexcept override;

	virtual int itemCount() const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual PageRawInfo::Column itemType(const QModelIndex& index) const noexcept override;

	virtual PageRawInfoPtr pageRawInfoPtr(const QModelIndex& index) const noexcept override;

	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void pageRawInfoAdded(int rowIndex) const override;

#ifdef QT_DEBUG

	WebCrawler::PageRaw* pageRaw(const QModelIndex& index) const noexcept;

#endif

private:
	Q_SLOT void onStorageUpdated(int row, int type);

private:
	WebCrawler::GuiStorage::GuiStorageTypePtr m_associatedStorage;
	QList<PageRawInfo::Column> m_availableColumns;
	WebCrawler::DataCollection::StorageType m_storageType;
};

}