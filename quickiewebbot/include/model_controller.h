#pragma once

#include "page_info.h"

namespace QuickieWebBot
{

class DataCollection;

class ModelController : public QObject
{
	Q_OBJECT

public:
	ModelController(QObject* parent = nullptr);

	void addPageInfo(PageInfoPtr pageInfo) noexcept;

	void setHost(QUrl const& host) noexcept;
	QUrl host() const noexcept;

	DataCollection* data() noexcept;

private:


private:
	QUrl m_host;
	DataCollection* m_data;
};

}