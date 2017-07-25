#include "application.h"
#include "application_properties.h"
#include "model_controller.h"
#include "data_collection.h"

namespace QuickieWebBot
{

ModelController::ModelController(QObject* parent)
	: m_data(new DataCollection(this))
{
}

void ModelController::addPageInfo(PageInfoPtr pageInfo) noexcept
{
	data()->addPageInfo(pageInfo, DataCollection::CrawledUrlStorageType);

	if (pageInfo->itemValue(PageInfo::TitleItemType).toString().isEmpty())
	{
		data()->addPageInfo(pageInfo, DataCollection::EmptyTitleUrlStorageType);
	}

	if (pageInfo->itemValue(PageInfo::UrlItemType).toUrl().host() != theApp->properties()->url().host())
	{
		data()->addPageInfo(pageInfo, DataCollection::ExternalUrlStorageType);
	}
}

DataCollection* ModelController::data() noexcept
{
	return m_data;
}

}