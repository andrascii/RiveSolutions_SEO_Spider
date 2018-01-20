#pragma once

#include "ui_control_panel_widget.h"

namespace SeoSpider
{

class UrlLineEdit;

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
	void onControlsChanged(int page);

private:
	Ui::ControlPanelWidget m_ui;
	Url m_url;

	UrlLineEdit* m_urlLineEdit;

	QAction* m_startOrConrinueCrawlingAction;
	QAction* m_stopCrawlingAction;
	QAction* m_clearCrawlingDataAction;
	
};

}