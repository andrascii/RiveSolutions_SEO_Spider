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

	void addWebsiteAnalyseElement(std::shared_ptr<WebsiteAnalyseElement> urlInfo) noexcept;

	void setHost(QUrl const& host) noexcept;
	QUrl const& host() const noexcept;

private:


private:
	QUrl m_host;
};

}