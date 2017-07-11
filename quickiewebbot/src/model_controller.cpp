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
}

void ModelController::setHost(QUrl const& host) noexcept
{
	m_host = host;
}

QUrl ModelController::host() const noexcept
{
	return QString("http://www.php.su")/*m_host*/;
}

DataCollection* ModelController::data() noexcept
{
	return m_data;
}

}