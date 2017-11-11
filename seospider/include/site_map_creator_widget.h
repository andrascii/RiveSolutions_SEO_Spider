#pragma once

#include "ui_site_map_creator_widget.h"

namespace SeoSpider
{

class SitemapCreatorWidget : public QDialog
{
public:
	SitemapCreatorWidget(QWidget* parent = nullptr);

private:
	std::unique_ptr<Ui_SitemapCreatorWidget> m_ui;
};

}