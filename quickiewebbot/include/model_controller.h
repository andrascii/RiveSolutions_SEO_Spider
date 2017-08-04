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

	const DataCollection* data() const noexcept;
	DataCollection* data() noexcept;

private:
	DataCollection* m_data;
};

}