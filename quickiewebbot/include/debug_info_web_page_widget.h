#pragma once

#include "ui_debug_web_page_info_widget.h"

namespace WebCrawler
{
	struct ParsedPage;
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

	Q_SIGNAL void pageSelected(WebCrawler::ParsedPage* page);

private:
	GlobalWebPageSelectedNotifier() = default;
};

class DebugInfoWebPageWidget : public QWidget, public Ui::DebugWebPageInfoWidget
{
	Q_OBJECT

public:
	static void attachDebugInfoWebPageWidget();

	Q_SLOT void onPageSelected(WebCrawler::ParsedPage* page);

protected:
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private:
	DebugInfoWebPageWidget(QWidget* parent);
};

}