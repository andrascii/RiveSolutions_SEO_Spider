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

	SummaryDataSet* dataSet = new SummaryDataSet(dataCollection);
	ISummaryDataAccessor* summaryDataAccessor = nullptr;

	switch (accessorType)
	{
		case DataAccessorType::ErrorsFilterPage:
		{
			dataSet->addGroup(AuditGroup::LinkAuditGroup);
			dataSet->addGroup(AuditGroup::PageProblemsAuditGroup);
			dataSet->addGroup(AuditGroup::OnPageAuditGroup);
			dataSet->addGroup(AuditGroup::NotIndexedPagesGroup);
			dataSet->addGroup(AuditGroup::H1AuditGroup);
			dataSet->addGroup(AuditGroup::H2AuditGroup);
			dataSet->addGroup(AuditGroup::ImageAuditGroup);

			summaryDataAccessor = new SummaryDataAccessorPixmapDecorator(new SummaryDataAccessor(dataSet));

			return summaryDataAccessor;
		}

		case DataAccessorType::GroupedErrorFilterPage:
		{
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

			dataSet->addGroup(AuditGroup::OrderedErrorsGroup);
			dataSet->addSortingPredicate(std::move(sortPredicate));

			summaryDataAccessor = new SummaryDataAccessorPixmapDecorator(new SummaryDataAccessor(dataSet));

			return summaryDataAccessor;
		}

		case DataAccessorType::AllResourcesPage:
		{
			dataSet->addGroup(AuditGroup::InternalResourcesGroup);
			dataSet->addGroup(AuditGroup::ExternalResourcesGroup);

			summaryDataAccessor = new SummaryDataAccessor(dataSet);

			return summaryDataAccessor;
		}
	}

	return summaryDataAccessor;
}

}