#pragma once

#include "parsed_page_info.h"

namespace QuickieWebBot
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

	void setPageRawInfo(const ParsedPageInfoPtr& pageRawInfoPtr);
	void setPageDataType(PageDataType pageDataType);

private:
	void setPageServerResponse(const ParsedPageInfoPtr& page);
	QString tabDescription(PageDataType pageDataType) const;
	PageLinkContext mapType(PageDataType pageDataType) const noexcept;

private:
	QTabWidget* m_tabWidget;
	QLabel* m_httpResponseLabel;

	QMap<PageDataType, PageModel*> m_models;
};

}