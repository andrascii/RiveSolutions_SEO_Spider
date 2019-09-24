#pragma once

#include "parsed_page_info_storage_adapter.h"
#include "iflexible_columns.h"

namespace SeoSpider
{

class FlexibleLinksStorageAdapter : public ParsedPageInfoStorageAdapter
{
	Q_OBJECT

public:
	FlexibleLinksStorageAdapter(
		CrawlerEngine::SequencedDataCollection* dataCollection,
		CrawlerEngine::ISequencedStorage* associatedStorage,
		CrawlerEngine::StorageType storageType,
		IFlexibleColumns* flexibleColumns,
		QObject* parent = nullptr
	);

	virtual QVariant item(const QModelIndex& index) const noexcept override;
	virtual ItemType itemType(const QModelIndex& index) const noexcept override;
	virtual QString columnDescription(int columnIndex) const noexcept override;
	virtual int columnWidth(int columnIndex) const noexcept override;
	virtual int columnCount() const noexcept override;
	virtual bool columnEnabled(int columnIndex) const noexcept override;

private:
	IFlexibleColumns* m_flexibleColumns;
};

}