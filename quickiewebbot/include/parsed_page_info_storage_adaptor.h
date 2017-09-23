#pragma once

#include "istorage_adaptor.h"
#include "parsed_page_info.h"
#include "sequenced_data_collection.h"

namespace QuickieWebBot
{

class ParsedPageInfoStorageAdaptor : public QObject, public IStorageAdaptor
{
	Q_OBJECT

public:
	ParsedPageInfoStorageAdaptor(const WebCrawler::SequencedDataCollection::SequencedStorageTypePtr& associatedStorage,
		WebCrawler::StorageType storageType,
		QObject* parent = nullptr);

	virtual void setAvailableColumns(QList<ParsedPageInfo::Column> availableColumns) noexcept override;
	virtual QList<ParsedPageInfo::Column> availableColumns() const noexcept override;
	virtual QString columnDescription(int columnIndex) const noexcept override;

	virtual int itemCount() const noexcept override;
	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual ParsedPageInfo::Column itemType(const QModelIndex& index) const noexcept override;

	virtual PageRawInfoPtr pageRawInfoPtr(const QModelIndex& index) const noexcept override;

	virtual QObject* qobject() noexcept override;

	Q_SIGNAL virtual void pageRawInfoAdded(int rowIndex) const override;

#ifdef QT_DEBUG
	WebCrawler::ParsedPage* pageRaw(const QModelIndex& index) const noexcept;
#endif

private:
	Q_SLOT void onStorageUpdated(int row, int type);

private:
	WebCrawler::SequencedDataCollection::SequencedStorageTypePtr m_associatedStorage;
	QList<ParsedPageInfo::Column> m_availableColumns;
	WebCrawler::StorageType m_storageType;
};

}