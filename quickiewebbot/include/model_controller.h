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
	DataCollection* data() noexcept;

private:


private:
	DataCollection* m_data;
};

}