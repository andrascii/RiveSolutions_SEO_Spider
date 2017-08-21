#pragma once

#include "ui_debug_web_page_info_widget.h"

namespace WebCrawler
{
	struct PageRaw;
}

namespace QuickieWebBot
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

	Q_SIGNAL void pageSelected(WebCrawler::PageRaw* page);

private:
	GlobalWebPageSelectedNotifier() = default;

};

class DebugInfoWebPageWidget : public QWidget, public Ui::DebugWebPageInfoWidget
{
	Q_OBJECT
public:
	DebugInfoWebPageWidget(QWidget* parent);

	Q_SLOT void onPageSelected(WebCrawler::PageRaw* page);
};

}