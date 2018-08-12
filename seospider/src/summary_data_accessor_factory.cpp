#include "summary_data_accessor_factory.h"
#include "summary_data_accessor.h"
#include "summary_data_accessor_pixmap_decorator.h"
#include "data_collection_groups_factory.h"
#include "summary_data_set.h"

namespace SeoSpider
{

ISummaryDataAccessor* SummaryDataAccessorFactory::create(DataAccessorType accessorType, CrawlerEngine::SequencedDataCollection* dataCollection) const
{
	ASSERT(accessorType > DataAccessorType::DataAccessorTypeBegin &&
		accessorType < DataAccessorType::DataAccessorTypeEnd);

	ISummaryDataAccessor* summaryDataAccessor = nullptr;

	switch (accessorType)
	{
		case DataAccessorType::ErrorsFilterPage:
		{
			SummaryDataSet* plainDataSet = new SummaryDataSet(dataCollection);
			SummaryDataSet* sortableDataSet = new SummaryDataSet(dataCollection);

			plainDataSet->addGroup(AuditGroup::LinkAuditGroup);
			plainDataSet->addGroup(AuditGroup::PageProblemsAuditGroup);
			plainDataSet->addGroup(AuditGroup::OnPageAuditGroup);
			plainDataSet->addGroup(AuditGroup::NotIndexedPagesGroup);
			plainDataSet->addGroup(AuditGroup::H1AuditGroup);
			plainDataSet->addGroup(AuditGroup::H2AuditGroup);
			plainDataSet->addGroup(AuditGroup::ImageAuditGroup);

			DataCollectionGroupsFactory dcGroupsFactory;
			const DCStorageGroupDescriptionPtr customDataFeedGroup = dcGroupsFactory.create(dataCollection->customDataFeeds());
			if (!customDataFeedGroup->descriptions.empty())
			{
				plainDataSet->addGroup(customDataFeedGroup);
			}

			std::function<bool(DCStorageDescription*, DCStorageDescription*)> sortPredicate([](DCStorageDescription* a, DCStorageDescription* b)
			{
				if (CrawlerEngine::ErrorCategory::level(a->storageType) == CrawlerEngine::ErrorCategory::level(b->storageType))
				{
					return a->storageTypeDescriptionName < b->storageTypeDescriptionName;
				}
				else
				{
					return CrawlerEngine::ErrorCategory::level(a->storageType) > CrawlerEngine::ErrorCategory::level(b->storageType);
				}
			});

			
			const DCStorageGroupDescriptionPtr orderedErrorsGroup = dcGroupsFactory.create(AuditGroup::OrderedErrorsGroup);
			if (!customDataFeedGroup->descriptions.empty())
			{
				for (const DCStorageDescription& description : customDataFeedGroup->descriptions)
				{
					orderedErrorsGroup->descriptions.push_back(description);
				}
			}
			sortableDataSet->addGroup(orderedErrorsGroup);

			sortableDataSet->addSortingPredicate(std::move(sortPredicate));

			summaryDataAccessor = new SummaryDataAccessorPixmapDecorator(new SummaryDataAccessor(plainDataSet));
			summaryDataAccessor->setSortableDataSet(sortableDataSet);

			return summaryDataAccessor;
		}

		case DataAccessorType::AllResourcesPage:
		{
			SummaryDataSet* dataSet = new SummaryDataSet(dataCollection);

			dataSet->addGroup(AuditGroup::InternalResourcesGroup);
			dataSet->addGroup(AuditGroup::ExternalResourcesGroup);

			summaryDataAccessor = new SummaryDataAccessor(dataSet);

			return summaryDataAccessor;
		}
	}

	return summaryDataAccessor;
}

}