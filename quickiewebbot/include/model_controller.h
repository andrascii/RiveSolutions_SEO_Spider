#pragma once

#include "website_analyse_element.h"
#include "model_controller_data.h"

namespace QuickieWebBot
{

class ModelController: public ModelControllerData
{
	Q_OBJECT

public:
	ModelController(QObject* parent = nullptr);

	Q_INVOKABLE void addWebsiteAnalyseElement(std::shared_ptr<WebsiteAnalyseElement> urlInfo);
};

}