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

	bool isSortable() const;
	bool isSortingEnabled() const;
	int columnCount() const;
	int rowCount() const;
	void addSortingPredicate(std::function<bool(const DCStorageDescription*, const DCStorageDescription*)>&& predicate);
	const CrawlerEngine::SequencedDataCollection* sequencedDataCollection() const;
	void appendGroup(AuditGroup auditGroup);
	void appendGroup(DCStorageGroupDescriptionPtr group);
	void prependGroup(AuditGroup auditGroup);
	void removeGroup(AuditGroup auditGroup);
	bool isHeaderRow(int row) const;
	QSize span(const QModelIndex& index) const;
	QVariant item(const QModelIndex& index) const;
	StorageAdapterType itemCategory(const QModelIndex& index) const;
	QString customDataFeed(const QModelIndex& index) const;
	const DCStorageDescription* storageDescriptionByRow(int row) const;
	const DCStorageGroupDescription* storageGroupDescriptionByRow(int row) const;
	const DCStorageDescription* storageDescription(CrawlerEngine::StorageType type) const;
	const DCStorageGroupDescription* storageGroupDescription(AuditGroup group) const;

signals:
	void sortingStarted();
	void sortingEnded();
	void dataSetChanged();

private slots:
	void sortItems(int, CrawlerEngine::StorageType);
	void reconfigureFilters();

private:
	QVector<AuditGroup> auditGroupsSequentialChain() const;
	void removeFilters();

private:
	static constexpr int s_summaryColumnCount = 2;

	const CrawlerEngine::SequencedDataCollection* m_sequencedDataCollection;

	QVector<DCStorageGroupDescriptionPtr> m_allGroups;
	QMap<int, DCStorageGroupDescriptionPtr> m_groupRows;
	QMap<int, const DCStorageDescription*> m_itemRows;

	std::function<bool(const DCStorageDescription*, const DCStorageDescription*)> m_sortPredicate;
};

}