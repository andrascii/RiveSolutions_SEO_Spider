#pragma once

#include "parsed_page_info.h"

namespace SeoSpider
{

class PageModel;

class PageDataWidget : public QFrame
{
	Q_OBJECT

public:
	enum PageDataType
	{
		BeginType,
		LinksToThisPageType,
		LinksOnThisPageType,
		ImagesOnThisPageType,
		ServerResponseForPageType,
		EndType,
	};

	PageDataWidget(QWidget* parent = nullptr);

	void setParsedPageInfo(const ParsedPageInfoPtr& pageRawInfoPtr);

	void setPageDataType(PageDataType pageDataType);

private:
	void setPageServerResponse(const ParsedPageInfoPtr& page) const;

	static QString tabDescription(PageDataType pageDataType);
	static PageLinkContext mapType(PageDataType pageDataType) noexcept;

private:
	QTabWidget* m_tabWidget;

	QLabel* m_httpResponseLabel;

	QMap<PageDataType, PageModel*> m_models;
};

}