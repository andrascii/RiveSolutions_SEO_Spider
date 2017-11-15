#pragma once

#include "ui_site_map_creator_widget.h"

namespace SeoSpider
{

class SitemapCreatorWidget : public QDialog
{
	Q_OBJECT

public:
	SitemapCreatorWidget(QWidget* parent = nullptr);

private slots:
	void showSaveFileDialog() const;
	void controlStateChanged() const;

private:
	std::unique_ptr<Ui_SitemapCreatorWidget> m_ui;
};

}