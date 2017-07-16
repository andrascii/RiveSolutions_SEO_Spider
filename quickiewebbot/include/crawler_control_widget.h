#pragma once

#include "ui_crawler_control_widget.h"

namespace QuickieWebBot
{

class CrawlerControlWidget : public QWidget
{
	Q_OBJECT

public:
	CrawlerControlWidget(QWidget* parent);

	Q_SLOT void showListView(bool value);
	Q_SLOT void showSummaryView(bool value);

private:
	void initialize();

private:
	Ui::CrawlerControlWidget m_ui;
};

}