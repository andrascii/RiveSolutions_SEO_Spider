#pragma once

#include "ui_control_panel.h"

namespace QuickieWebBot
{

class ControlPanelWidget : public QFrame
{
	Q_OBJECT

public:
	ControlPanelWidget(QWidget* parent);

	const QUrl& url() const noexcept;

private:
	Q_SLOT void setUrl();
	Q_SLOT void startCrawling();
	Q_SLOT void stopCrawling();

private:
	Ui::ControlPanel m_ui;

	QUrl m_url;
};

}