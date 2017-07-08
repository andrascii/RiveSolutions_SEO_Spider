#include "application.h"
#include "model_controller.h"
#include "model_controller_data.h"

namespace QuickieWebBot
{

ModelController::ModelController(QObject* parent)
	: m_data(new ModelControllerData(this))
{
}

void ModelController::addPageInfo(PageInfoPtr pageInfo) noexcept
{
	data()->addPageInfo(pageInfo, ModelControllerData::CrawledUrlStorageType);
}

void ModelController::setHost(QUrl const& host) noexcept
{
	m_host = host;
}

QUrl ModelController::host() const noexcept
{
	return QString("http://www.php.su")/*m_host*/;
}

ModelControllerData* ModelController::data() noexcept
{
	return m_data;
}

}