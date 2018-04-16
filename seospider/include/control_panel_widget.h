#pragma once

#include "ui_control_panel_widget.h"
#include "crawler_options.h"

namespace CrawlerEngine
{
	
class Url;

}

namespace SeoSpider
{

class ControlPanelWidget : public QFrame
{
	Q_OBJECT

public:
	ControlPanelWidget(QWidget* parent);

	const CrawlerEngine::Url& url() const noexcept;

protected:
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private slots:
	void setUrl() const;
	void startCrawling() const;
	void stopCrawling() const;
	void clearCrawlingData() const;
	void onCrawlerStateChanged(int state);
	void onCrawlerOptionsChanged(CrawlerEngine::CrawlerOptions options);

private:
	Ui::ControlPanelWidget m_ui;
	CrawlerEngine::Url m_url;
};

}