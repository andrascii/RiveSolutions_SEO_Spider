#pragma once

#include "data_collection_groups_factory.h"
#include "storage_adapter_type.h"

namespace CrawlerEngine
{

class SequencedDataCollection;

}

namespace SeoSpider
{

class SummaryDataSet : public QObject
{
	Q_OBJECT

public:
	SummaryDataSet(const CrawlerEngine::SequencedDataCollection* sequencedDataCollection);

	bool isSortable() const noexcept;
	bool isSortingEnabled() const noexcept;
	int columnCount() const noexcept;
	int rowCount() const noexcept;
	void addSortingPredicate(std::function<bool(DCStorageDescription*, DCStorageDescription*)>&& predicate);
	const CrawlerEngine::SequencedDataCollection* sequencedDataCollection() const noexcept;
	void addGroup(AuditGroup group) noexcept;
	void addGroup(DCStorageGroupDescriptionPtr group) noexcept;
	bool isHeaderRow(int row) const noexcept;
	QSize span(const QModelIndex& index) const noexcept;
	QVariant item(const QModelIndex& index) const noexcept;
	StorageAdapterType itemCategory(const QModelIndex& index) const noexcept;
	QString customDataFeed(const QModelIndex& index) const noexcept;
	const DCStorageDescription* storageDescriptionByRow(int row) const noexcept;
	const DCStorageGroupDescription* storageGroupDescriptionByRow(int row) const noexcept;
	const DCStorageDescription* storageDescription(CrawlerEngine::StorageType type) const noexcept;
	const DCStorageGroupDescription* storageGroupDescription(AuditGroup group) const noexcept;

signals:
	void sortingStarted();
	void sortingEnded();

private slots:
	void sortItems(int, CrawlerEngine::StorageType);

private:
	static constexpr int s_summaryColumnCount = 2;

	const CrawlerEngine::SequencedDataCollection* m_sequencedDataCollection;

	QVector<DCStorageGroupDescriptionPtr> m_allGroups;
	QMap<int, DCStorageGroupDescriptionPtr> m_groupRows;
	QMap<int, DCStorageDescription*> m_itemRows;

	std::function<bool(DCStorageDescription*, DCStorageDescription*)> m_sortPredicate;
};

}