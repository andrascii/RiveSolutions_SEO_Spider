#include "application.h"
#include "model_controller.h"

namespace QuickieWebBot
{

ModelController::ModelController(QObject* parent)
	: ModelControllerData(parent)
{
}

void ModelController::addWebsiteAnalyseElement(std::shared_ptr<WebSiteAnalyseElement> urlInfo) noexcept
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

}