#pragma once

#include "ui_debug_web_page_info_widget.h"

namespace CrawlerEngine
{
	struct ParsedPage;
}

namespace SeoSpider
{

class GlobalWebPageSelectedNotifier : public QObject
{
	Q_OBJECT

public:
	static GlobalWebPageSelectedNotifier* instanse() noexcept
	{
		static GlobalWebPageSelectedNotifier s_instance;
		return &s_instance;
	}

	Q_SIGNAL void pageSelected(CrawlerEngine::ParsedPage* page);

private:
	GlobalWebPageSelectedNotifier() = default;
};

class DebugInfoWebPageWidget : public QWidget, public Ui::DebugWebPageInfoWidget
{
	Q_OBJECT

public:
	static void attach();

	Q_SLOT void onPageSelected(CrawlerEngine::ParsedPage* page) const;

protected:
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private:
	DebugInfoWebPageWidget(QWidget* parent);
};

}