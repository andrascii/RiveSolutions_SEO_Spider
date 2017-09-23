#pragma once

#include "parsed_page_info.h"

namespace QuickieWebBot
{

class PageModel;

class PageDataWidget : public QFrame
{
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

	void setPageRawInfo(const PageRawInfoPtr& pageRawInfoPtr);
	void setPageDataType(PageDataType pageDataType);

private:
	void setPageServerResponse(const PageRawInfoPtr& page);
	QString tabDescription(PageDataType pageDataType) const;
	PageLinkType mapType(PageDataType pageDataType) const noexcept;

private:
	QTabWidget* m_tabWidget;
	QLabel* m_httpResponseLabel;

	QMap<PageDataType, PageModel*> m_models;
};

}