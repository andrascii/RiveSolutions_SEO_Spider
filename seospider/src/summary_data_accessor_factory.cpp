#include "summary_data_accessor_factory.h"
#include "summary_data_accessor.h"
#include "summary_data_accessor_pixmap_decorator.h"

namespace SeoSpider
{

ISummaryDataAccessor* SummaryDataAccessorFactory::create(DataAccessorType accessorType, CrawlerEngine::SequencedDataCollection* guiStorage) const
{
	ASSERT(accessorType > DataAccessorType::DataAccessorTypeBegin &&
		accessorType < DataAccessorType::DataAccessorTypeEnd);

	ISummaryDataAccessor* summaryDataAccessor = nullptr;

	switch (accessorType)
	{
		case DataAccessorType::ErrorsFilterPage:
		{
			summaryDataAccessor = new SummaryDataAccessorPixmapDecorator(new SummaryDataAccessor(guiStorage));

			summaryDataAccessor->addGroup(AuditGroup::LinkAuditGroup);
			summaryDataAccessor->addGroup(AuditGroup::PageProblemsAuditGroup);
			summaryDataAccessor->addGroup(AuditGroup::OnPageAuditGroup);
			summaryDataAccessor->addGroup(AuditGroup::H1AuditGroup);
			summaryDataAccessor->addGroup(AuditGroup::H2AuditGroup);
			summaryDataAccessor->addGroup(AuditGroup::ImageAuditGroup);

			return summaryDataAccessor;
		}

		case DataAccessorType::AllResourcesPage:
		{
			summaryDataAccessor = new SummaryDataAccessor(guiStorage);

			summaryDataAccessor->addGroup(AuditGroup::InternalResourcesGroup);
			summaryDataAccessor->addGroup(AuditGroup::ExternalResourcesGroup);

			return summaryDataAccessor;
		}
	}

	return summaryDataAccessor;
}

}