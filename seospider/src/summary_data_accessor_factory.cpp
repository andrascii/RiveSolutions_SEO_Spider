#include "summary_data_accessor_factory.h"
#include "summary_data_accessor.h"
#include "summary_data_accessor_pixmap_decorator.h"
#include "data_collection_groups_factory.h"
#include "summary_data_set.h"
#include "application.h"
#include "preferences.h"

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

			plainDataSet->appendGroup(AuditGroup::YandexMetricaCounters);
			plainDataSet->appendGroup(AuditGroup::LinkAuditGroup);
			plainDataSet->appendGroup(AuditGroup::PageProblemsAuditGroup);
			plainDataSet->appendGroup(AuditGroup::OnPageAuditGroup);
			plainDataSet->appendGroup(AuditGroup::NotIndexedPagesGroup);
			plainDataSet->appendGroup(AuditGroup::H1AuditGroup);
			plainDataSet->appendGroup(AuditGroup::H2AuditGroup);
			plainDataSet->appendGroup(AuditGroup::ImageAuditGroup);

			const auto onSearchYmCountersChanged = [plainDataSet](bool value)
			{
				if (!value)
				{
					plainDataSet->removeGroup(AuditGroup::YandexMetricaCounters);
				}
				else
				{
					plainDataSet->prependGroup(AuditGroup::YandexMetricaCounters);
				}
			};

			VERIFY(QObject::connect(theApp->preferences(), &Preferences::searchYandexMetricaCountersChanged, onSearchYmCountersChanged));

			DataCollectionGroupsFactory dcGroupsFactory;
			const DCStorageGroupDescriptionPtr customDataFeedGroup = dcGroupsFactory.create(dataCollection->customDataFeeds());
			if (customDataFeedGroup->hasDescriptions())
			{
				plainDataSet->appendGroup(customDataFeedGroup);
			}

			std::function<bool(const DCStorageDescription*, const DCStorageDescription*)>
			sortPredicate([](const DCStorageDescription* a, const DCStorageDescription* b)
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
			if (customDataFeedGroup->hasDescriptions())
			{
				for (const DCStorageDescription& description : customDataFeedGroup->descriptions())
				{
					orderedErrorsGroup->addDescription(description);
				}
			}

			sortableDataSet->appendGroup(orderedErrorsGroup);
			sortableDataSet->addSortingPredicate(std::move(sortPredicate));

			summaryDataAccessor = new SummaryDataAccessorPixmapDecorator(new SummaryDataAccessor(plainDataSet));
			summaryDataAccessor->setSortableDataSet(sortableDataSet);

			return summaryDataAccessor;
		}

		case DataAccessorType::AllResourcesPage:
		{
			SummaryDataSet* dataSet = new SummaryDataSet(dataCollection);

			dataSet->appendGroup(AuditGroup::InternalResourcesGroup);
			dataSet->appendGroup(AuditGroup::ExternalResourcesGroup);

			summaryDataAccessor = new SummaryDataAccessor(dataSet);

			return summaryDataAccessor;
		}
	}

	return summaryDataAccessor;
}

}