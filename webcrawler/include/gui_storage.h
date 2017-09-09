#pragma once

#include "page_raw.h"
#include "page_raw_hasher_proxy.h"
#include "page_raw_comparator.h"
#include "data_collection.h"

namespace WebCrawler
{

class GuiStorage : public QObject
{
	Q_OBJECT
public:
	using GuiStorageType = QVector<PageRawPtr>;
	using GuiStorageTypePtr = std::shared_ptr<GuiStorageType>;

	GuiStorage();

	const GuiStorageTypePtr& guiStorage(DataCollection::StorageType type) const noexcept;
	GuiStorageTypePtr& guiStorage(DataCollection::StorageType type) noexcept;

	Q_SIGNAL void pageRawAdded(int row, int storageType);
private:
	void addPageRaw(PageRawPtr pageRaw, int type) noexcept;
	void initializeStorages();

private:
	std::unordered_map<int, GuiStorageTypePtr> m_guiStorageMap;

	friend class DataCollection;
};

}