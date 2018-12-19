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

	QTabBar* tabWidget();

public slots:
	void pageViewSelectionChangedSlot(const QItemSelection& selected, const QItemSelection& deselected);
	void onTabBarClicked(int index);

signals:
	void tabBarClicked(int index, int prevIndex);
	void tabSelected(int index);

protected:
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private:
	void setPageServerResponse(const ParsedPageInfoPtr& page) const;

	static QString tabDescription(PageDataType pageDataType);
	static PageLinkContext mapType(PageDataType pageDataType) noexcept;

private:
	QStackedWidget* m_stackedWidget;
	QTabBar* m_tabBar;

	QTextEdit* m_httpResponseLabel;

	QMap<PageDataType, PageModel*> m_models;
	QMap<PageDataType, int> m_pageIndices;
	QMap<PageDataType, QStackedWidget*> m_stackedWidgets;
	QMap<QWidget*, QWidget*> m_filterResourceTypeWidgets;
	QHBoxLayout* m_topWidgetLayout;

};

}