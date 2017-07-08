#include "application.h"
#include "model_controller.h"
#include "model_controller_data.h"

namespace QuickieWebBot
{

ModelController::ModelController(QObject* parent)
	: m_data(new ModelControllerData(this))
{
}

void ModelController::addPageInfo(std::shared_ptr<PageInfo> urlInfo) noexcept
{
}

void ModelController::setHost(QUrl const& host) noexcept
{
	m_host = host;
}

QUrl const& ModelController::host() const noexcept
{
	return m_host;
}

const ModelControllerData* ModelController::data() const noexcept
{
	return m_data;
}

}