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

	void selectTab(PageDataType pageDataType);

	QTabWidget* tabWidget();

public slots:
	void pageViewSelectionChangedSlot(const QItemSelection& selected, const QItemSelection& deselected);
	void onTabBarClicked(int index);

signals:
	void tabBarClicked(int index, int prevIndex);
	void tabSelected(int index);

private:
	void setPageServerResponse(const ParsedPageInfoPtr& page) const;

	static QString tabDescription(PageDataType pageDataType);
	static PageLinkContext mapType(PageDataType pageDataType) noexcept;

private:
	QTabWidget* m_tabWidget;

	QTextEdit* m_httpResponseLabel;

	QMap<PageDataType, PageModel*> m_models;
	QMap<PageDataType, int> m_pageIndices;
	QMap<PageDataType, QStackedWidget*> m_stackedWidgets;
};

}