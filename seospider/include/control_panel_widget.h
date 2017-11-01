#pragma once

#include "ui_control_panel_widget.h"

namespace SeoSpider
{

class ControlPanelWidget : public QFrame
{
	Q_OBJECT

public:
	ControlPanelWidget(QWidget* parent);

	const QUrl& url() const noexcept;

private:
	Q_SLOT void setUrl() const;
	Q_SLOT void startCrawling() const;
	Q_SLOT void stopCrawling() const;

private:
	Ui::ControlPanelWidget m_ui;

	QUrl m_url;
};

}