#pragma once

#include "page_info.h"

namespace QuickieWebBot
{

class ModelControllerData;

class ModelController : public QObject
{
	Q_OBJECT

public:
	ModelController(QObject* parent = nullptr);

	void addPageInfo(std::shared_ptr<PageInfo> urlInfo) noexcept;

	void setHost(QUrl const& host) noexcept;
	QUrl const& host() const noexcept;

	const ModelControllerData* data() const noexcept;

private:


private:
	QUrl m_host;
	ModelControllerData* m_data;
};

}