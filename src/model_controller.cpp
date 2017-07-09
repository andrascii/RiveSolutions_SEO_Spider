#include "application.h"
#include "model_controller.h"

namespace QuickieWebBot
{

ModelController::ModelController(QObject* parent)
	: QObject(parent)
{
}

void ModelController::addWebsiteAnalyseElement(std::shared_ptr<WebsiteAnalyseElement> urlInfo)
{

}

}