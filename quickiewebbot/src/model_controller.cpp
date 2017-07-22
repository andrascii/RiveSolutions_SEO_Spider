#include "application.h"
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

	if (pageInfo->title.isEmpty())
	{
		data()->addPageInfo(pageInfo, DataCollection::EmptyTitleUrlStorageType);
	}
}

DataCollection* ModelController::data() noexcept
{
	return m_data;
}

}