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
	void processPageInfoUrl(PageInfoPtr pageInfo) noexcept;
	void processPageInfoTitle(PageInfoPtr pageInfo) noexcept;
	void processPageInfoMetaDescription(PageInfoPtr pageInfo) noexcept;
	void processPageInfoMetaKeywords(PageInfoPtr pageInfo) noexcept;
	void processPageInfoH1(PageInfoPtr pageInfo) noexcept;
	void processPageInfoH2(PageInfoPtr pageInfo) noexcept;
	void processPageInfoImage(PageInfoPtr pageInfo) noexcept;

private:
	DataCollection* m_data;
};

}