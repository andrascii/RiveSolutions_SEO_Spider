#pragma once

#include "ui_control_panel_widget.h"

namespace SeoSpider
{

class ControlPanelWidget : public QFrame
{
	Q_OBJECT

public:
	ControlPanelWidget(QWidget* parent);

	const Url& url() const noexcept;

protected:
	virtual bool eventFilter(QObject* object, QEvent* event) override;

private slots:
	void setUrl() const;
	void startCrawling() const;
	void stopCrawling() const;
	void clearCrawlingData() const;
	void onCrawlerStateChanged(int state);

private:
	Ui::ControlPanelWidget m_ui;
	Url m_url;
};

}